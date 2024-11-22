/*****************************************************************************
* usbaudio录音相关代码
****************************************************************************/
#define LOG_LEV 4
#include "user_config.h"
#include "sltk_math.h"
#include "amixer.h"
#include "audio_in.h"
#include "audio_out.h"
#include "middleware.h"
#include "amsg.h"
#include "aeffect_component.h"
#include "audio_service.h"
#include "volume_control.h"
#include "arec_client.h"
#include "audio_sink_common.h"
#include "sltk_math.h"
#include "audiosrate_conventor.h"
#include "sltk_interp.h"


#define I2S_SLAVER_REC_AUDIO_FS           (48000)               //audio端采样率(固定和amixer一致)
#define I2S_SLAVER_REC_AUDIO_CH           2                     //audio端声道数(固定和amixer一致)
#define I2S_SLAVER_REC_AUDIO_SAMPLE_SIZE  (sizeof(int16_t))     //audio端采样精度(暂只支持16bit)

#if (I2S0_SLOT_WIDTH==1)
#define I2S_SLAVER_SAMPLE_SIZE            (sizeof(int32_t))
#else
#define I2S_SLAVER_SAMPLE_SIZE            (sizeof(int16_t))
#endif


//缓存大小
#define I2S_SLAVER_REC_BUF_SZ      ((AUDIO_SINK_PROC_PERIOD*5) * I2S_SLAVER_REC_AUDIO_SAMPLE_SIZE * I2S_SLAVER_REC_AUDIO_CH)
//触发读取的数据量阈值
#define I2S_SLAVER_REC_TRIGGER_TH  ((AUDIO_SINK_PROC_PERIOD*1) * I2S_SLAVER_REC_AUDIO_SAMPLE_SIZE * I2S_SLAVER_REC_AUDIO_CH)


enum {
    I2S_SLAVER_REC_STA_IDLE,
    I2S_SLAVER_REC_STA_ACTIVE,
};

typedef struct {
    struct pcm *out;
    uint8_t ch;
    uint32_t fs;
    uint32_t sample_size;
    void *pcmbuf;
    OAL_THREAD task_handle;           //线程句柄
} i2s_slaver_t;

//i2s slaver 录音
typedef struct {
    audio_sink_common_t common; //插值控制器
    uint8_t buf[I2S_SLAVER_REC_BUF_SZ];   //缓存buffer
    int16_t interpbuf[I2S_SLAVER_REC_AUDIO_CH * (G_AUDIO_PROC_PERIOD + MAX_INTERP_CNT)];

    void *resample_handle;

    float vol_db;               //音量(对数域)
    uint32_t i2s_fs;            //I2S端的采样率
    uint8_t i2s_sample_size;    //I2S端的采样精度
    uint8_t i2s_ch;             //I2S端的声道数

    uint8_t muted;              //静音状态: 1-静音; 0-不静音

    uint8_t status;             //状态

    i2s_slaver_t i2s_slaver;
} i2s_slaver_rec_t;

typedef struct {
    uint32_t out_fs;
    uint8_t  out_ch;
    uint8_t  out_samples_size;
    uint8_t  out_proc_period;
} aout_info_t;

#if 0
void i2s_slaver_rec_start(void *handle, uint32_t fs, uint8_t sample_size, uint8_t ch)
{
    i2s_slaver_rec_t *i2s_slaver_rec;
    audio_sink_init_params_t params;

    i2s_slaver_rec = OAL_calloc(1, sizeof(*i2s_slaver_rec));
    if (NULL == i2s_slaver_rec) {
        return NULL;
    }

    //rec info
    i2s_slaver_rec->i2s_slaver.ch           = I2S_SLAVER_REC_AUDIO_CH;
    i2s_slaver_rec->i2s_slaver.fs           = I2S_SLAVER_REC_AUDIO_FS;
    i2s_slaver_rec->i2s_slaver.sample_size  = I2S_SLAVER_SAMPLE_SIZE;

    //track info
    i2s_slaver_rec->i2s_fs                  = i2s_slaver_rec->i2s_slaver.fs;
    i2s_slaver_rec->i2s_sample_size         = I2S_SLAVER_REC_AUDIO_SAMPLE_SIZE;
    i2s_slaver_rec->i2s_ch                  = i2s_slaver_rec->i2s_slaver.ch;
    i2s_slaver_rec->resample_handle         = NULL;

    params.fs                = i2s_slaver_rec->i2s_fs;
    params.sample_size       = i2s_slaver_rec->i2s_sample_size;
    params.ch                = i2s_slaver_rec->i2s_ch;
    params.input_sample_size = i2s_slaver_rec->i2s_sample_size;
    params.input_ch          = i2s_slaver_rec->i2s_ch;
    params.buf               = i2s_slaver_rec->buf;
    params.buf_bytes         = I2S_SLAVER_REC_BUF_SZ;
    params.trigger_th        = I2S_SLAVER_REC_TRIGGER_TH;
    params.period_bytes      = AUDIO_SINK_PROC_PERIOD*params.sample_size*params.ch;
    params.sink_update_t_th  = 1700;
    audio_sink_init(&i2s_slaver_rec->common, &params);
    
    if (i2s_slaver_rec->i2s_fs != I2S_SLAVER_REC_AUDIO_FS) {
        //创建重采样器
    }

    i2s_slaver_rec->status = I2S_SLAVER_REC_STA_ACTIVE;

}
#endif

void i2s_slaver_rec_stop(void *handle, uint32_t fs, uint8_t sample_size, uint8_t ch)
{

}

AT_SRAM(.i2s_slaver_rec)
uint8_t i2s_slaver_rec_sample_size_get(void *handle)
{
    i2s_slaver_rec_t *i2s_slaver_rec = handle;
    
    if (NULL == i2s_slaver_rec) {
        return (sizeof(int32_t));
    }
    
    return i2s_slaver_rec->i2s_sample_size;
}


//保存i2srec数据
AT_SRAM(.i2s_slaver_rec)
void i2s_slaver_rec_store_data(void *handle, void *buf, uint32_t samples)
{
    i2s_slaver_rec_t *i2s_slaver_rec = handle;

    if (NULL == i2s_slaver_rec) {
        return;
    }

    if (i2s_slaver_rec->status != I2S_SLAVER_REC_STA_ACTIVE) {
        return;
    }

    if (i2s_slaver_rec->resample_handle) {
        //重采样
        audiosrate_conventor_set_period(i2s_slaver_rec->resample_handle, buf);
    } else {
        audio_sink_write(&i2s_slaver_rec->common, i2s_slaver_rec->interpbuf, buf, samples);
    }

}

AT_SRAM(.i2s_slaver_rec)
void i2s_slaver_rec_audio_sink_write(void* user, void* buf, int32_t samples)
{
    i2s_slaver_rec_t *i2s_slaver_rec = user;
    audio_sink_write(&i2s_slaver_rec->common, i2s_slaver_rec->interpbuf, buf, samples);
}


AT_SRAM(.i2s_slaver_rec)
void i2s_slaver_rec_data_read(void *handle, void *buf, uint32_t bytes)
{
    i2s_slaver_rec_t *i2s_slaver_rec = handle;
    uint32_t samples;


    if (NULL == i2s_slaver_rec) {
        memset(buf, 0, bytes);
        return;
    }

    if (i2s_slaver_rec->status != I2S_SLAVER_REC_STA_ACTIVE) {
        memset(buf, 0, bytes);
        return;
    }

    if (i2s_slaver_rec->i2s_ch && i2s_slaver_rec->i2s_sample_size) {
        samples = bytes / (i2s_slaver_rec->i2s_ch * i2s_slaver_rec->i2s_sample_size);
        audio_sink_read(&i2s_slaver_rec->common, buf, samples);
    }
}

void i2s_slaver_rec_close(void *handle)
{
    i2s_slaver_rec_t *hd = handle;

    if (hd) {
        if (hd->i2s_slaver.out) {
            pcm_close(hd->i2s_slaver.out);
        }

        if (hd->i2s_slaver.pcmbuf) {
            general_free(hd->i2s_slaver.pcmbuf);
        }
    }
}

AT_SRAM(.i2s_slaver_rec)
void *i2s_slaver_rec_loop(void *handle)
{
    i2s_slaver_rec_t *hd = handle;

    while (hd) {
        if(hd->i2s_slaver.out) {
            i2s_slaver_rec_data_read(hd, hd->i2s_slaver.pcmbuf, G_AUDIO_PROC_PERIOD * hd->i2s_ch * hd->i2s_sample_size);
            if((hd->i2s_sample_size == (sizeof(int16_t))) &&
                (hd->i2s_slaver.sample_size == (sizeof(int32_t)))) {
                stereo16_to_stereo32((int32_t *)hd->i2s_slaver.pcmbuf, (int16_t *)hd->i2s_slaver.pcmbuf, G_AUDIO_PROC_PERIOD);
            }
            pcm_write(hd->i2s_slaver.out, hd->i2s_slaver.pcmbuf, G_AUDIO_PROC_PERIOD);
        } else {
            OAL_sleep(1);
        }
    }

    return NULL;
}

void i2s_slaver_rec_open(void *handle)
{
    i2s_slaver_rec_t *hd = handle;
    struct pcm_config out_config;

    if (hd) {
        out_config.ch               = hd->i2s_slaver.ch;
        out_config.sample_size      = hd->i2s_slaver.sample_size;
        out_config.rate             = hd->i2s_slaver.fs;
        out_config.period_size      = G_AUDIO_PROC_PERIOD;
        out_config.period_count     = 2;
        out_config.start_threshold  = 0;
        out_config.stop_threshold   = 0;
        hd->i2s_slaver.out = pcm_open(SND_I2S, PCM_OUT, &out_config);

        hd->i2s_slaver.pcmbuf = general_calloc(G_AUDIO_PROC_PERIOD * hd->i2s_slaver.ch * hd->i2s_slaver.sample_size);

        logd("@@ I2S out:%#X, pcmbuf:%#X", hd->i2s_slaver.out, hd->i2s_slaver.pcmbuf);
        if (!hd->i2s_slaver.out || !hd->i2s_slaver.pcmbuf) {
            i2s_slaver_rec_close(hd);
            return;
        }

        hd->i2s_slaver.task_handle = OAL_thread_create(i2s_slaver_rec_loop, hd, "i2s_slaver_rec", 10, 256);
    }
}

void *i2s_slaver_rec_create(void)
{
    i2s_slaver_rec_t *i2s_slaver_rec;
    audio_sink_init_params_t params;
    aout_info_t aout_info;

    i2s_slaver_rec = OAL_calloc(1, sizeof(*i2s_slaver_rec));
    if (NULL == i2s_slaver_rec) {
        return NULL;
    }

    //i2s pcm info
    i2s_slaver_rec->i2s_slaver.ch           = I2S_SLAVER_REC_AUDIO_CH;
    i2s_slaver_rec->i2s_slaver.fs           = I2S_SLAVER_REC_AUDIO_FS;
    i2s_slaver_rec->i2s_slaver.sample_size  = I2S_SLAVER_SAMPLE_SIZE;

    //rec info
    i2s_slaver_rec->i2s_fs                  = i2s_slaver_rec->i2s_slaver.fs;
    i2s_slaver_rec->i2s_sample_size         = I2S_SLAVER_REC_AUDIO_SAMPLE_SIZE;
    i2s_slaver_rec->i2s_ch                  = i2s_slaver_rec->i2s_slaver.ch;
    i2s_slaver_rec->resample_handle         = NULL;

    params.fs                = i2s_slaver_rec->i2s_fs;
    params.sample_size       = i2s_slaver_rec->i2s_sample_size;
    params.ch                = i2s_slaver_rec->i2s_ch;
    params.input_sample_size = i2s_slaver_rec->i2s_sample_size;
    params.input_ch          = i2s_slaver_rec->i2s_ch;
    params.buf               = i2s_slaver_rec->buf;
    params.buf_bytes         = I2S_SLAVER_REC_BUF_SZ;
    params.trigger_th        = I2S_SLAVER_REC_TRIGGER_TH;
    params.period_bytes      = AUDIO_SINK_PROC_PERIOD*params.sample_size*params.ch;
    params.sink_update_t_th  = 1700;
    params.use_new_interp_flag = 0;
    audio_sink_init(&i2s_slaver_rec->common, &params);

    amixer_xet_ctrl(AMIXER_CMD_GET_AMIXER_PCM_SRATE_CFG, 0, (uint32_t)&aout_info.out_fs);
    
    if(aout_info.out_fs != i2s_slaver_rec->i2s_slaver.fs){
        asrc_init_param aip;
        aip.io_period = AUDIO_SINK_PROC_PERIOD;
        aip.ch        = i2s_slaver_rec->i2s_ch;
        aip.sample_size = i2s_slaver_rec->i2s_sample_size;
        aip.srate_in    = aout_info.out_fs;
        aip.srate_out   = i2s_slaver_rec->i2s_slaver.fs;;
        aip.quality     = ASRC_QUALITY_HIGH;
        aip.user = (void*)i2s_slaver_rec;
        aip.clbk = (user_xet_data_clbk)i2s_slaver_rec_audio_sink_write;
        i2s_slaver_rec->resample_handle = create_audiosrate_conventor(&aip);
        if(!i2s_slaver_rec->resample_handle) {
            logd("src init fail!");
            OAL_free(i2s_slaver_rec);
            i2s_slaver_rec = NULL;
            return i2s_slaver_rec;
        }
    }

    i2s_slaver_rec_open(i2s_slaver_rec);

    i2s_slaver_rec->status = I2S_SLAVER_REC_STA_ACTIVE;

    return i2s_slaver_rec;
}




