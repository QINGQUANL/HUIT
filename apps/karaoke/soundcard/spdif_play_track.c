
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
#include "hal_spdif.h"
#include "spdif_play_track.h"
#include "app_spdif.h"

void pin_spdif_ana_en(uint8_t enable);

#define SPDIF_REC_BUF_CNT    (4)
#define SPDIF_REC_PERIOD     (G_AUDIO_PROC_PERIOD)
#define SPDIF_REC_BUF_LEN    (SPDIF_REC_BUF_CNT * SPDIF_REC_PERIOD*4*2)

#define SPDIF_SLAVER_PLAY_TRACK_CH            (2)
#define SPDIF_SLAVER_PLAY_TRACK_FS            (48000)
#define SPDIF_LOOPBACK_FS (SPDIF_REC_PERIOD*1000)

#define spdif_SAMPLE_SIZE              (sizeof(int16_t))

#define SPDIF_SLAVER_PLAY_TRACK_SAMPLE_SIZE   (sizeof(int16_t))       //由于插值使用16bit，暂固定使用16bit

#define SPDIF_BUF_SZ          ((AUDIO_SOURCE_PROC_PERIOD*16 + SPDIF_REC_PERIOD*2) * SPDIF_SLAVER_PLAY_TRACK_SAMPLE_SIZE * SPDIF_SLAVER_PLAY_TRACK_CH)
#define SPDIF_BUF_TRIGGER_TH  ((AUDIO_SOURCE_PROC_PERIOD*8 + SPDIF_REC_PERIOD) * SPDIF_SLAVER_PLAY_TRACK_SAMPLE_SIZE * SPDIF_SLAVER_PLAY_TRACK_CH)

typedef struct {
    //hal_spdif_stream_t *in;
    uint8_t bank;
    uint8_t ch;
    uint32_t fs;
    uint32_t sample_size;
    void *out_pcmbuf;
    void *pcmbuf[2];
    
    OAL_THREAD task_handle;           //线程句柄
} spdif_t;

typedef struct {
    audio_source_common_t common;
    uint8_t buf[SPDIF_BUF_SZ];
    OAL_MUTEX mutex_lock;
    void *spdif_play_track_handle; //用于播放
    float vol_db;
    uint32_t fs;
    uint8_t muted;
    uint8_t ch;
    uint8_t ready;

    spdif_t spdif_player;
    //audio_fade_t fade;
} spdif_play_track_t;

bool  spdif_clk_reset;
hal_spdif_stream_init_t spdif_init;
spdif_stream_info_t spdif_info;
void* hal_spdif_rx_handle;
spdif_play_track_t *spdif_play_track_handler;
static uint8_t spdif_sel;
void spdif_input_sel(uint8_t input_sel)
{
    if(input_sel == SEL_OPT) {
#if (SPDIF_RX_PIN_SEL >= 0) && (SPDIF_RX_PIN_SEL <= 6)
        HAL_gpio_config(SPDIF_OPT_PIN_CFG);
        spdif_sel = SEL_OPT;
#endif
#if SPDIF_COA_RX_PIN_SEL
        HAL_gpio_config(SPDIF_COA_DISABLE);
        pin_spdif_ana_en(false);
#endif

    } else if(input_sel == SEL_COA) {
#if (SPDIF_RX_PIN_SEL >= 0) && (SPDIF_RX_PIN_SEL <= 6)
        HAL_gpio_config(SPDIF_OPT_DISABLE);
#endif
#if SPDIF_COA_RX_PIN_SEL
        HAL_gpio_config(SPDIF_COA_PIN_CFG);
        pin_spdif_ana_en(true);
        spdif_sel = SEL_COA;
#endif
    }
    spdif_clk_reset = 0;
}

uint8_t spdif_get_work_input()
{
    return spdif_sel;
}

void spdif_play_track_destory(void *handle)
{
    spdif_play_track_t *hd = handle;

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

AT_SRAM(.spdif_track)
void spdif_play_track_write(void *handle, void *buf, uint32_t samples)
{
    spdif_play_track_t *hd = handle;

    if (NULL == hd || 0 == hd->ready) {
        return;
    }

    audio_source_write(&hd->common, buf, samples);
}

//此函数由Amixer线程回调
AT_SRAM(.spdif_track)
uint32_t spdif_play_track_notify(void* cookie, uint32_t who, uint32_t cmd, uint32_t val, uint32_t rev)
{
    spdif_play_track_t *hd = cookie;
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
        case AUDIOTRACK_MSG_WATRACK_PULLDATA_REQ: //amixer请求拿spdif playtrack数据
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

void spdif_play_close(void *handle)
{
    spdif_play_track_t *hd = handle;

    if (hd) {
        //if (hd->spdif_player.in) {
        //    pcm_close(hd->spdif_player.in);
        //}
        if (hd->spdif_player.pcmbuf[0]) {
            general_free(hd->spdif_player.pcmbuf[0]);
        }
        if (hd->spdif_player.pcmbuf[1]) {
            general_free(hd->spdif_player.pcmbuf[1]);
        }

        if(hd->spdif_player.out_pcmbuf) {
            general_free(hd->spdif_player.out_pcmbuf);
        }
    }

    if(spdif_info.rec_buf == NULL) {
        OAL_free(spdif_info.rec_buf);
        spdif_info.rec_buf = NULL;
    }
}

void sin_debug(void * buf);

uint16_t fade;
void spdif_fade_start()
{
    fade = 0;
}

uint8_t need_captrue = true;
uint8_t spdif_lock_count;

AT_SRAM(.spdif_track)
void *spdif_play_loop(void *handle)
{
    spdif_play_track_t *hd = handle;
    uint32_t rx_sta;
    spdif_clk_reset = true;
    logi("spdif_play_loop++++++++");
    while (1) {
        if(spdif_clk_reset == 0) {
            HAL_msleep(50);
            HAL_spdif_rx_enable(false);
            HAL_msleep(1);
            HAL_spdif_rx_enable(true);
            spdif_clk_reset = true;
            need_captrue = 1;
            HAL_msleep(100);
            logi("clk reset");
        }

        HAL_spdif_ioctl(handle, SPDIF_CMD_RX_LOCK_STATUS_GET, &rx_sta);
        //logi("rx_sta %x", rx_sta);
        if(rx_sta) {
            if(need_captrue) {
                hal_spdif_rx_info_t info;
                if (HAL_spdif_ioctl(handle, SPDIF_CMD_RX_INFO_CAPTRUE, &info) < 0) {
                    loge("captrue fail");
                    HAL_msleep(100);
                } else {
                    logd("captrue ok: %d, %d, %d +++++++++", info.fs, info.word_length, info.ch);
                    uint16_t size ;
                    size = SPDIF_REC_PERIOD * 2 * 2;    //我们只需要处理采样率即可
                    
                    memset(hd->spdif_player.pcmbuf[0], 0x00, size);
                    memset(hd->spdif_player.pcmbuf[1], 0x00, size);
                    spdif_lock_count = 0;

                    OAL_sleep(100);
                    need_captrue = 0;
                    spdif_clk_reset = true;
                
                    spdif_init.fs = info.fs;

                    if(1 || info.fs != 88200) {
                        spdif_play_track_start(spdif_play_track_handler, spdif_init.fs);
                        //ring_buffer_init(&spdif_info.spdif_rx_buf_ctrl, (uint8_t*)spdif_info.rec_buf, SPDIF_REC_BUF_LEN);

                        HAL_spdif_read(hal_spdif_rx_handle, hd->spdif_player.pcmbuf[0], size);    

                        spdif_play_track_handler->spdif_player.bank = 0;
                        logi("spdi dma start ------");
                    } else {
                        //192K以及88200的采样率的数据转换，CPU搞不过来,会导致异常，暂时不支持
                        logi("Unsupport sample rate ......fs %d", spdif_init.fs );
                    }
                    OAL_sleep(1);
                }
            } else {
                //uint32_t r_avail = ring_buffer_get_r_avail(&spdif_info.spdif_rx_buf_ctrl);
                //uint32_t size = spdif_init.fs / 1000 * 2 * 2;
                //if(r_avail >= size * 2) {
                //    ring_buffer_read(&spdif_info.spdif_rx_buf_ctrl, hd->spdif_player.out_pcmbuf, size);
                //    spdif_play_track_write(spdif_play_track_handler, hd->spdif_player.out_pcmbuf, size/2/2);
                //}
                OAL_sleep(1);
            } 
        }  else {
            OAL_sleep(100);
        }
    }
    return NULL;
}

AT_SRAM(.spdif)
void spdif_lock_callback()
{
    logi("lock...");
}

AT_SRAM(.spdif)
void spdif_unlock_callback()
{
    logi("unlock ----");
    spdif_lock_count = 0;
    spdif_clk_reset = false;
}


AT_SRAM(.HAL_spdif_read)
void spdif_dma_full_callback(void)
{    
#if 1
    if(need_captrue == 0) {
        uint16_t size = SPDIF_REC_PERIOD * 2 * 2;
        
        uint8_t bank = spdif_play_track_handler->spdif_player.bank;
        
        HAL_spdif_read(hal_spdif_rx_handle, spdif_play_track_handler->spdif_player.pcmbuf[!bank], size);
        spdif_play_track_handler->spdif_player.bank = !bank;
        uint32_t rx_sta;
        HAL_spdif_ioctl(hal_spdif_rx_handle, SPDIF_CMD_RX_LOCK_STATUS_GET, &rx_sta);
        if(spdif_lock_count < 100 || !rx_sta) {
            //logi("dma rx_sta unlock");
            //防止在切歌过程中产生啪啪声
            memset(spdif_play_track_handler->spdif_player.pcmbuf[bank], 0x00, size);
        }
        spdif_lock_count++;
        if(spdif_lock_count > 100) {    //上一次unlock后，100ms内静音
            spdif_lock_count = 100;
        }

        #if 0
        int32_t w_avail = ring_buffer_get_w_avail(&spdif_info.spdif_rx_buf_ctrl);
        if(w_avail >= size) {
            ring_buffer_write(&spdif_info.spdif_rx_buf_ctrl, spdif_play_track_handler->spdif_player.pcmbuf[bank], size);
        }
        #else
        spdif_play_track_write(spdif_play_track_handler, spdif_play_track_handler->spdif_player.pcmbuf[bank], size/2/2);
        //int32_t r_avail = ring_buffer_get_r_avail(&spdif_info.spdif_rx_buf_ctrl);
        #endif
        //spdif_play_track_write(spdif_play_track_handler, spdif_play_track_handler->spdif_player.pcmbuf[bank], size/2/2);
        spdif_play_track_handler->spdif_player.bank = !bank;
    }else {
        spdif_clk_reset = 1;
    }
#endif    
}

void spdif_play_open(void *handle)
{
    spdif_play_track_t *hd = handle;
    //struct pcm_config in_config;

    if (hd) {
        spdif_init.dma_pre_full_callback = NULL;
        spdif_init.dma_full_callback = spdif_dma_full_callback;
        spdif_init.lock_callback = spdif_lock_callback;
        spdif_init.unlock_callback = spdif_unlock_callback;
        spdif_init.fs = SPDIF_LOOPBACK_FS;  //unused in RX mode， 采样率我们要自己转
        spdif_init.sample_size = 2;     //设置硬件会为我们转成相关的位数
        spdif_init.ch = 2;            //这些参数都要从spdif连接后重设
        spdif_init.direction = M_SPDIF_RX;
        spdif_init.transfer_type = SPDIF_TRANSFER_TYPE_DMA;
        hal_spdif_rx_handle = HAL_spdif_open(&spdif_init);
        logi("spdif_play_open ...");

        //预设其缓冲器为192K*16bit*2ch
        hd->spdif_player.pcmbuf[0] = general_calloc(SPDIF_REC_PERIOD * 2 * 2);   //硬件会为我们处理位数成16位
        hd->spdif_player.pcmbuf[1] = general_calloc(SPDIF_REC_PERIOD * 2 * 2);
        hd->spdif_player.out_pcmbuf = general_calloc(SPDIF_REC_PERIOD * 2 * 2);

        if (!hd->spdif_player.pcmbuf[0] || !hd->spdif_player.pcmbuf[1]) {
            spdif_play_close(hd);
            return;
        }

        hd->spdif_player.task_handle = OAL_thread_create(spdif_play_loop, hd, "spdif_play", 9, 256);
    }
}

void *spdif_play_track_create(void)
{
#if 1
    //HAL_gpio_config(PIN_D1 | PIN_FUNCOUTPUT);
    
    spdif_play_track_handler = OAL_calloc(1, sizeof(*spdif_play_track_handler));
    if (NULL == spdif_play_track_handler) {
        return NULL;
    }

    spdif_info.rec_buf = OAL_malloc(SPDIF_REC_BUF_LEN);

    //ring_buffer_init(&spdif_info.spdif_rx_buf_ctrl, (uint8_t*)spdif_info.rec_buf, SPDIF_REC_BUF_LEN);
    //spdif_info.rec_buf_read_th = REC_BUF_LEN / 2;
    
    spdif_play_track_handler->spdif_player.ch          = SPDIF_SLAVER_PLAY_TRACK_CH; //FIXME: 需要根据I2S来设置
    spdif_play_track_handler->spdif_player.fs          = SPDIF_SLAVER_PLAY_TRACK_FS;
    spdif_play_track_handler->spdif_player.sample_size = spdif_SAMPLE_SIZE;
    spdif_play_track_handler->ch                     = spdif_play_track_handler->spdif_player.ch;

    spdif_play_open(spdif_play_track_handler);
    
    spdif_play_track_handler->mutex_lock = OAL_mutex_create();
    if (NULL == spdif_play_track_handler->mutex_lock) {
        spdif_play_track_destory(spdif_play_track_handler);
    }
    spdif_play_track_handler->vol_db = (float)0.0;

    return spdif_play_track_handler;
#else
    void HAL_adjust_audiopll(uint32_t rate);
    HAL_adjust_audiopll(24576000);  //PLL要根据采样率来
    return 0;
#endif    
}

AT_SRAM(.spdif_track)
bool spdif_play_ready_get(void *handle)
{
    spdif_play_track_t *hd = handle;
    return hd->ready;
}

void spdif_play_track_start(void *handle,uint32_t fs)
{
#if 1
    logi("spdif_play_track_start, %x", handle);
    spdif_play_track_t *hd = handle;
    audio_track_init_t track_init;
    audio_source_init_params_t params;

    if (NULL == hd) {
        return;
    }

    hd->ready = 0;

    OAL_mutex_lock(&hd->mutex_lock);
    audio_track_cmd(hd->spdif_play_track_handle, TRACK_CMD_STOP, 1);
    OAL_mutex_unlock(&hd->mutex_lock);
    audio_track_close(hd->spdif_play_track_handle);

    hd->fs = fs;//hd->spdif_player.fs;
    params.fs = hd->fs;
    params.sample_size = SPDIF_SLAVER_PLAY_TRACK_SAMPLE_SIZE;
    params.ch = hd->ch;
    params.buf = hd->buf;
    params.buf_bytes = SPDIF_BUF_SZ;
    params.trigger_th = SPDIF_BUF_TRIGGER_TH;
    params.period_bytes = AUDIO_SOURCE_PROC_PERIOD*params.sample_size*params.ch;
    params.type = AUDIO_SOURCE_TYPE_INTERP;
    params.use_new_interp_flag = 0;
    audio_source_init(&hd->common, &params);
    track_init.mode = AUDIO_MODE_SPDIF;
    track_init.fs = hd->fs;
    track_init.ch = hd->ch;
    track_init.sample_size = SPDIF_SLAVER_PLAY_TRACK_SAMPLE_SIZE;
    track_init.input_interleave = true;
    track_init.fadein_ms = 100;
    track_init.fadeout_ms = 100;
    track_init.sharebuf_samples = G_AUDIO_PROC_PERIOD;
    track_init.user_data = hd;
    track_init.notify    = spdif_play_track_notify;
    track_init.pulldata_from_user = 1;
    hd->spdif_play_track_handle = audio_track_open(&track_init);
    spdif_play_volume_set(spdif_play_volume_get());
    audio_track_cmd(hd->spdif_play_track_handle, TRACK_CMD_START, 1);
    hd->ready = 1;
    #endif
}

void spdif_play_mute_set(uint8_t mute)
{
    float vol = (float)0.0;

    spdif_play_track_handler->muted = mute;
    if (!mute) {
        vol = volume_db_to_volf32(spdif_play_track_handler->vol_db);
    }

    audio_track_cmd(spdif_play_track_handler->spdif_play_track_handle, TRACK_CMD_VOLUME_SET, (uint32_t)&vol);
}

uint8_t spdif_play_mute_get()
{
    return spdif_play_track_handler->muted;
}

void spdif_play_volume_set(float vol_db)
{
    float vol = (float)0.0;

    spdif_play_track_handler->vol_db = vol_db;
    if (!spdif_play_track_handler->muted) {
        vol = volume_db_to_volf32(spdif_play_track_handler->vol_db);
    }
    audio_track_cmd(spdif_play_track_handler->spdif_play_track_handle, TRACK_CMD_VOLUME_SET, (uint32_t)&vol);
}

float spdif_play_volume_get()
{
    return spdif_play_track_handler->vol_db;
}


