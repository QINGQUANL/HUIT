
/*****************************************************************************
* usbaudio播放相关代码
****************************************************************************/
#define LOG_LEV 4
#include "amixer.h"
#include "audio_in.h"
#include "audio_out.h"
#include "middleware.h"
#include "amsg.h"
#include "aeffect_component.h"
#include "audio_service.h"
#include "volume_control.h"
#include "arec_client.h"
#include "audio_source_common.h"
#include "sltk_math.h"

#define USBPLAY_TRACK_CH            (2)
#define USBPLAY_TRACK_SAMPLE_SIZE   (sizeof(int16_t))

#define USBA_BUF_SZ ((AUDIO_SOURCE_PROC_PERIOD*4 + 48*2) * USBPLAY_TRACK_SAMPLE_SIZE * USBPLAY_TRACK_CH)
#define TRIGGER_TH  ((AUDIO_SOURCE_PROC_PERIOD*2+48) * USBPLAY_TRACK_SAMPLE_SIZE * USBPLAY_TRACK_CH)


typedef struct {
    audio_source_common_t common;
    uint8_t buf[USBA_BUF_SZ];

    OAL_MUTEX mutex_lock;
    void *usbplay_track_handle; //用于播放
    float vol_db;
    uint32_t fs;
    uint8_t muted;
    uint8_t ch;
    uint8_t ready;
} usbplay_track_t;

float usbplay_volume_get(void *handle);
void usbplay_volume_set(void *handle, float vol_db);

void usbplay_track_destory(void *handle)
{
    usbplay_track_t *usbplay = handle;

    if (NULL == usbplay) {
        return;
    }

    if (usbplay->mutex_lock) {
        OAL_mutex_destroy(&usbplay->mutex_lock);
        usbplay->mutex_lock = NULL;
    }

    OAL_free(usbplay);
}

AT_SRAM(.usbplay_track_write)
void usbplay_track_write(void *handle, void *buf, uint32_t samples)
{
    usbplay_track_t *usbplay = handle;

    if (NULL == usbplay || 0 == usbplay->ready) {
        return;
    }

    audio_source_write(&usbplay->common, buf, samples);
}

//此函数由Amixer线程回调
AT_SRAM(.usbplay_track_notify)
uint32_t usbplay_track_notify(void* cookie, uint32_t who, uint32_t cmd, uint32_t val, uint32_t rev)
{
    usbplay_track_t *usbplay = cookie;
    uint32_t *prm = (uint32_t *)val;

    if (NULL == usbplay || 0 == usbplay->ready) {
        return 0;
    }

    OAL_mutex_lock(&usbplay->mutex_lock);

    if (NULL == usbplay || 0 == usbplay->ready) {
        OAL_mutex_unlock(&usbplay->mutex_lock);
        return 0;
    }

    if (AUDIO_NOTIFY_ATRACK == who) {
        switch (cmd) {
        case AUDIOTRACK_MSG_WATRACK_PULLDATA_REQ: //amixer请求拿usbplaytrack数据
            audio_source_read(&usbplay->common, (void*)(prm[0]), prm[1]);
            break;

        case AUDIOTRACK_MSG_WATRACK_POSTPROC_REQ:
            break;

        default:
            break;
        }
    }
    OAL_mutex_unlock(&usbplay->mutex_lock);

    return 0;
}

void *usbplay_track_create(void)
{
    usbplay_track_t *usbplay;

    usbplay = OAL_calloc(1, sizeof(*usbplay));
    if (NULL == usbplay) {
        return NULL;
    }

    usbplay->ch = USBPLAY_TRACK_CH; //FIXME: 需要根据USB来设置

    usbplay->mutex_lock = OAL_mutex_create();
    if (NULL == usbplay->mutex_lock) {
        usbplay_track_destory(usbplay);
    }
    usbplay->vol_db = (float)0.0;

    return usbplay;
}

AT_SRAM(.usbplay)
bool usbplay_ready_get(void *handle)
{
    usbplay_track_t *usbplay = handle;
    if(usbplay == NULL) {
        return FALSE;
    }
    return usbplay->ready;
}

void usbplay_track_start(void *handle, uint32_t fs)
{
    usbplay_track_t *usbplay = handle;
    audio_track_init_t track_init;
    audio_source_init_params_t params;

    if (NULL == usbplay) {
        return;
    }

    usbplay->ready = 0;

    OAL_mutex_lock(&usbplay->mutex_lock);
    audio_track_cmd(usbplay->usbplay_track_handle, TRACK_CMD_STOP, 1);
    OAL_mutex_unlock(&usbplay->mutex_lock);
    audio_track_close(usbplay->usbplay_track_handle);

    usbplay->fs = fs;
    params.fs = usbplay->fs;
    params.sample_size = USBPLAY_TRACK_SAMPLE_SIZE;
    params.ch = usbplay->ch;
    params.buf = usbplay->buf;
    params.buf_bytes = USBA_BUF_SZ;
    params.trigger_th = TRIGGER_TH;
    params.period_bytes = AUDIO_SOURCE_PROC_PERIOD*params.sample_size*params.ch;
    params.type = AUDIO_SOURCE_TYPE_INTERP;
    params.use_new_interp_flag = 1;
    audio_source_init(&usbplay->common, &params);

    track_init.mode = AUDIO_MODE_USBPLAY;
    track_init.fs = usbplay->fs;
    track_init.ch = usbplay->ch;
    track_init.sample_size = USBPLAY_TRACK_SAMPLE_SIZE;
    track_init.input_interleave = true;
    track_init.fadein_ms = 100;
    track_init.fadeout_ms = 100;
    track_init.sharebuf_samples = G_AUDIO_PROC_PERIOD;
    track_init.user_data = usbplay;
    track_init.notify    = usbplay_track_notify;
    track_init.pulldata_from_user = 1;
    usbplay->usbplay_track_handle = audio_track_open(&track_init);

    usbplay_volume_set(usbplay, usbplay_volume_get(usbplay));

    audio_track_cmd(usbplay->usbplay_track_handle, TRACK_CMD_START, 1);

    usbplay->ready = 1;
}

void usbplay_mute_set(void *handle, uint8_t mute)
{
    usbplay_track_t *usbplay = handle;
    float vol = (float)0.0;

    if (NULL == usbplay) {
        return;
    }

    usbplay->muted = mute;
    if (!mute) {
        vol = volume_db_to_volf32(usbplay->vol_db);
    }

    audio_track_cmd(usbplay->usbplay_track_handle, TRACK_CMD_VOLUME_SET, (uint32_t)&vol);
}

uint8_t usbplay_mute_get(void *handle)
{
    usbplay_track_t *usbplay = handle;

    if (NULL == usbplay) {
        return 0;
    }

    return usbplay->muted;
}

void usbplay_volume_set(void *handle, float vol_db)
{
    usbplay_track_t *usbplay = handle;
    float vol = (float)0.0;

    if (NULL == usbplay) {
        return;
    }

    usbplay->vol_db = vol_db;
    if (!usbplay->muted) {
        vol = volume_db_to_volf32(usbplay->vol_db);
    }
    audio_track_cmd(usbplay->usbplay_track_handle, TRACK_CMD_VOLUME_SET, (uint32_t)&vol);
}

float usbplay_volume_get(void *handle)
{
    usbplay_track_t *usbplay = handle;

    if (NULL == usbplay) {
        return (float)0.0;
    }

    return usbplay->vol_db;
}
