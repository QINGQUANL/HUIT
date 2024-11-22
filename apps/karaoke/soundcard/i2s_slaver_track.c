
/*****************************************************************************
* usbaudio播放相关代码
****************************************************************************/
#define LOG_LEV 4
#include "user_config.h"
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

#define I2S_SLAVER_PLAY_TRACK_CH            (2)
#define I2S_SLAVER_PLAY_TRACK_FS            (48000)
#if (I2S0_SLOT_WIDTH==1)
#define I2S_SLAVER_SAMPLE_SIZE              (sizeof(int32_t))
#else
#define I2S_SLAVER_SAMPLE_SIZE              (sizeof(int16_t))
#endif

#define I2S_SLAVER_PLAY_TRACK_SAMPLE_SIZE   (sizeof(int16_t))       //由于插值使用16bit，暂固定使用16bit

#define I2S_BUF_SZ          ((AUDIO_SOURCE_PROC_PERIOD*4) * I2S_SLAVER_PLAY_TRACK_SAMPLE_SIZE * I2S_SLAVER_PLAY_TRACK_CH)
#define I2S_BUF_TRIGGER_TH  ((AUDIO_SOURCE_PROC_PERIOD*2) * I2S_SLAVER_PLAY_TRACK_SAMPLE_SIZE * I2S_SLAVER_PLAY_TRACK_CH)

typedef struct {
    struct pcm *in;
    uint8_t ch;
    uint32_t fs;
    uint32_t sample_size;
    void *out_pcmbuf;
    void *pcmbuf;
    OAL_THREAD task_handle;           //线程句柄
} i2s_slaver_t;

typedef struct {
    audio_source_common_t common;
    uint8_t buf[I2S_BUF_SZ];

    OAL_MUTEX mutex_lock;
    void *i2s_slaver_play_track_handle; //用于播放
    float vol_db;
    uint32_t fs;
    uint8_t muted;
    uint8_t ch;
    uint8_t ready;

    i2s_slaver_t i2s_slaver;
} i2s_slaver_play_track_t;

float i2s_slaver_play_volume_get(void *handle);
void i2s_slaver_play_volume_set(void *handle, float vol_db);

void i2s_slaver_play_track_destory(void *handle)
{
    i2s_slaver_play_track_t *hd = handle;

    if (NULL == hd) {
        return;
    }

    if (hd->mutex_lock) {
        OAL_mutex_destroy(&hd->mutex_lock);
        hd->mutex_lock = NULL;
    }

    OAL_free(hd);
    hd = NULL;
}

AT_SRAM(.i2s_slaver_track)
void i2s_slaver_play_track_write(void *handle, void *buf, uint32_t samples)
{
    i2s_slaver_play_track_t *hd = handle;

    if (NULL == hd || 0 == hd->ready) {
        return;
    }

    audio_source_write(&hd->common, buf, samples);
}

//此函数由Amixer线程回调
AT_SRAM(.i2s_slaver_track)
uint32_t i2s_slaver_play_track_notify(void* cookie, uint32_t who, uint32_t cmd, uint32_t val, uint32_t rev)
{
    i2s_slaver_play_track_t *hd = cookie;
    uint32_t *prm = (uint32_t *)val;

    if (NULL == hd || 0 == hd->ready) {
        return 0;
    }

    OAL_mutex_lock(&hd->mutex_lock);

    if (NULL == hd || 0 == hd->ready) {
        OAL_mutex_unlock(&hd->mutex_lock);
        return 0;
    }

    if (AUDIO_NOTIFY_ATRACK == who) {
        switch (cmd) {
        case AUDIOTRACK_MSG_WATRACK_PULLDATA_REQ: //amixer请求拿i2splaytrack数据
            audio_source_read(&hd->common, (void*)(prm[0]), prm[1]);
            break;

        case AUDIOTRACK_MSG_WATRACK_POSTPROC_REQ:
            break;

        default:
            break;
        }
    }
    OAL_mutex_unlock(&hd->mutex_lock);

    return 0;
}

void i2s_slaver_play_close(void *handle)
{
    i2s_slaver_play_track_t *hd = handle;

    if (hd) {
        if (hd->i2s_slaver.in) {
            pcm_close(hd->i2s_slaver.in);
        }
        if (hd->i2s_slaver.pcmbuf) {
            general_free(hd->i2s_slaver.pcmbuf);
        }
    }
}

AT_SRAM(.i2s_slaver_track)
void *i2s_slaver_play_loop(void *handle)
{
    i2s_slaver_play_track_t *hd = handle;

    while (hd) {
        if(hd->ready && hd->i2s_slaver.in) {
            pcm_read(hd->i2s_slaver.in, hd->i2s_slaver.pcmbuf, G_AUDIO_PROC_PERIOD);
            if((hd->i2s_slaver.sample_size == (sizeof(int32_t))) &&
               (I2S_SLAVER_PLAY_TRACK_SAMPLE_SIZE == (sizeof(int16_t)))) {
                stereo32_to_stereo16((int16_t*)hd->i2s_slaver.pcmbuf, (int32_t*)hd->i2s_slaver.pcmbuf, G_AUDIO_PROC_PERIOD);
            }
            i2s_slaver_play_track_write(hd, hd->i2s_slaver.pcmbuf, G_AUDIO_PROC_PERIOD);
        } else {
            OAL_sleep(1);
        }
    }

    return NULL;
}

void i2s_slaver_play_open(void *handle)
{
    i2s_slaver_play_track_t *hd = handle;
    struct pcm_config in_config;

    if (hd) {
        in_config.ch = hd->i2s_slaver.ch;
        in_config.sample_size = hd->i2s_slaver.sample_size;
        in_config.rate = hd->i2s_slaver.fs;
        in_config.period_size = G_AUDIO_PROC_PERIOD;
        in_config.period_count = 2;
        in_config.start_threshold = 0;
        in_config.stop_threshold = 0;
        hd->i2s_slaver.in = pcm_open(SND_I2S, PCM_IN, &in_config);
        
        hd->i2s_slaver.pcmbuf = general_calloc(G_AUDIO_PROC_PERIOD * hd->i2s_slaver.ch * hd->i2s_slaver.sample_size);

        logd("@@ I2S in:%#X, pcmbuf:%#X", hd->i2s_slaver.in, hd->i2s_slaver.pcmbuf);
        if (!hd->i2s_slaver.in || !hd->i2s_slaver.pcmbuf) {
            i2s_slaver_play_close(hd);
            return;
        }

        hd->i2s_slaver.task_handle = OAL_thread_create(i2s_slaver_play_loop, hd, "i2s_slaver_play", 9, 256);
    }
}

void *i2s_slaver_play_track_create(void)
{
    i2s_slaver_play_track_t *hd;

    hd = OAL_calloc(1, sizeof(*hd));
    if (NULL == hd) {
        return NULL;
    }
    
    hd->i2s_slaver.ch          = I2S_SLAVER_PLAY_TRACK_CH; //FIXME: 需要根据I2S来设置
    hd->i2s_slaver.fs          = I2S_SLAVER_PLAY_TRACK_FS;
    hd->i2s_slaver.sample_size = I2S_SLAVER_SAMPLE_SIZE;
    hd->ch                     = hd->i2s_slaver.ch;

    i2s_slaver_play_open(hd);
    
    hd->mutex_lock = OAL_mutex_create();
    if (NULL == hd->mutex_lock) {
        i2s_slaver_play_track_destory(hd);
    }
    hd->vol_db = (float)0.0;

    return hd;
}

AT_SRAM(.i2s_slaver_track)
bool i2s_slaver_play_ready_get(void *handle)
{
    i2s_slaver_play_track_t *hd = handle;
    return hd->ready;
}

void i2s_slaver_play_track_start(void *handle)
{
    i2s_slaver_play_track_t *hd = handle;
    audio_track_init_t track_init;
    audio_source_init_params_t params;

    if (NULL == hd) {
        return;
    }

    hd->ready = 0;

    OAL_mutex_lock(&hd->mutex_lock);
    audio_track_cmd(hd->i2s_slaver_play_track_handle, TRACK_CMD_STOP, 1);
    OAL_mutex_unlock(&hd->mutex_lock);
    audio_track_close(hd->i2s_slaver_play_track_handle);

    hd->fs = hd->i2s_slaver.fs;
    params.fs = hd->fs;
    params.sample_size = I2S_SLAVER_PLAY_TRACK_SAMPLE_SIZE;
    params.ch = hd->ch;
    params.buf = hd->buf;
    params.buf_bytes = I2S_BUF_SZ;
    params.trigger_th = I2S_BUF_TRIGGER_TH;
    params.period_bytes = AUDIO_SOURCE_PROC_PERIOD*params.sample_size*params.ch;
    params.type = AUDIO_SOURCE_TYPE_INTERP;
    params.use_new_interp_flag = 0;
    audio_source_init(&hd->common, &params);

    track_init.mode = AUDIO_MODE_I2S_SLAVER_PLAY;
    track_init.fs = hd->fs;
    track_init.ch = hd->ch;
    track_init.sample_size = I2S_SLAVER_PLAY_TRACK_SAMPLE_SIZE;
    track_init.input_interleave = true;
    track_init.fadein_ms = 100;
    track_init.fadeout_ms = 100;
    track_init.sharebuf_samples = G_AUDIO_PROC_PERIOD;
    track_init.user_data = hd;
    track_init.notify    = i2s_slaver_play_track_notify;
    track_init.pulldata_from_user = 1;
    hd->i2s_slaver_play_track_handle = audio_track_open(&track_init);

    i2s_slaver_play_volume_set(hd, i2s_slaver_play_volume_get(hd));

    audio_track_cmd(hd->i2s_slaver_play_track_handle, TRACK_CMD_START, 1);

    hd->ready = 1;
}

void i2s_slaver_play_mute_set(void *handle, uint8_t mute)
{
    i2s_slaver_play_track_t *hd = handle;
    float vol = (float)0.0;

    if (NULL == hd) {
        return;
    }

    hd->muted = mute;
    if (!mute) {
        vol = volume_db_to_volf32(hd->vol_db);
    }

    audio_track_cmd(hd->i2s_slaver_play_track_handle, TRACK_CMD_VOLUME_SET, (uint32_t)&vol);
}

uint8_t i2s_slaver_play_mute_get(void *handle)
{
    i2s_slaver_play_track_t *hd = handle;

    if (NULL == hd) {
        return 0;
    }

    return hd->muted;
}

void i2s_slaver_play_volume_set(void *handle, float vol_db)
{
    i2s_slaver_play_track_t *hd = handle;
    float vol = (float)0.0;

    if (NULL == hd) {
        return;
    }

    hd->vol_db = vol_db;
    if (!hd->muted) {
        vol = volume_db_to_volf32(hd->vol_db);
    }
    audio_track_cmd(hd->i2s_slaver_play_track_handle, TRACK_CMD_VOLUME_SET, (uint32_t)&vol);
}

float i2s_slaver_play_volume_get(void *handle)
{
    i2s_slaver_play_track_t *hd = handle;

    if (NULL == hd) {
        return (float)0.0;
    }

    return hd->vol_db;
}


