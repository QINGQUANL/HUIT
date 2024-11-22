/*****************************************************************************
* usbaudio录音相关代码
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
#include "audio_sink_common.h"
#include "sltk_math.h"
#include "sltk_interp.h"
#include "usb_app_config.h"
#include "audiosrate_conventor.h"

void usbrec_create_resample(uint32_t sample_rate);

#define USBREC_AUDIO_FS           (48000)               //audio端采样率(固定和amixer一致)
#define USBREC_AUDIO_CH           2                     //audio端声道数(固定和amixer一致)
#define USBREC_AUDIO_SAMPLE_SIZE  (sizeof(int16_t))     //audio端采样精度

//缓存大小
#define USBA_BUF_SZ ((AUDIO_SINK_PROC_PERIOD*4 + 48*2) * USBREC_AUDIO_SAMPLE_SIZE * USBREC_AUDIO_CH)
//触发读取的数据量阈值
#define TRIGGER_TH  ((AUDIO_SINK_PROC_PERIOD+48*2) * USBREC_AUDIO_SAMPLE_SIZE * USBREC_AUDIO_CH)


enum {
    USBREC_STA_IDLE,
    USBREC_STA_ACTIVE,
};


//usb录音
typedef struct {
    audio_sink_common_t common; //插值控制器
#if USB_MIC_NEED_RESAMPLE
    struct ring_buffer usb_resample_ctrl;
    uint8_t* usb_resample_ring_buffer;
#endif
    uint8_t buf[USBA_BUF_SZ];   //缓存buffer
    int16_t interpbuf[USBREC_AUDIO_CH * (G_AUDIO_PROC_PERIOD + MAX_INTERP_CNT)];

    void *resample_handle;

    float vol_db;               //音量(对数域)
    uint32_t usb_fs;            //usb端的采样率
    uint8_t usb_sample_size;    //usb端的采样精度
    uint8_t usb_ch;             //usb端的声道数

    uint8_t muted;              //静音状态: 1-静音; 0-不静音

    uint8_t status;             //状态
} usbrec_t;


void usbrec_data_read(void *handle, void *buf, uint32_t bytes);
__ALIGN_BEGIN uint8_t resample_raw[192] __ALIGN_END;
static usbrec_t* usbrec_handle;

#if USB_MIC_NEED_RESAMPLE

typedef struct {
    uint32_t sample;
    uint8_t channels;
    uint8_t sample_size;
}usbrec_resample_param_t;

void* usbd_resample_handle;
__ALIGN_BEGIN uint8_t resample_tmp[G_AUDIO_PROC_PERIOD*2*MIC_INPUT_CHANNALS] __ALIGN_END;
volatile usbrec_resample_param_t usbrec_resample_param;

OAL_THREAD usb_resample_thread_handle;
static bool resample_thread_end = false;
OAL_QUEUE g24_resample_queue = NULL;

AT_SRAM(.usbd_resmaple)
void ring_buffer_proceed(void* handle,void* buf, int sample)
{
    if(usbrec_handle) {
#if USB_MIC_CHANNEL == USB_MIC_CHANNEL_MONO
        stereo16_to_mono16((int16_t*)resample_tmp, (int16_t*)buf, sample);
#endif
        //启动采样率转化
        if(usbd_resample_handle) {
            if(g24_resample_queue != NULL) {
                uint32_t irq_stat = 0;
                if(OAL_FAILED == OAL_queue_send(&g24_resample_queue, &irq_stat, 0)) {
                    logi("err resample");
                }
            }
        }
    }
}

AT_SRAM(.usbd_resmaple)
void *usb_resample_loop(void *handle)
{
    logi("usb_resample_loop ++++++");
    OAL_QUEUE_POOL pool;
    uint32_t queue_stat;
    pool.size = 3 * sizeof(queue_stat);
    if(g24_resample_queue == NULL) {
        OAL_queue_create(&g24_resample_queue, NULL, sizeof(queue_stat), &pool);
    }
    
    while(!resample_thread_end) {
        OAL_queue_recv(&g24_resample_queue, &queue_stat, OAL_WAIT_FOREVER);
        if(resample_thread_end) {
            break;
        }
        audiosrate_conventor_set_period(usbd_resample_handle, resample_tmp);
        //HAL_gpio_config(PIN_D1 | PIN_FUNCOUTPUT);
        //HAL_gpio_value_set(PIN_D1 ,0);
    }
    
    return 0;
}

#endif

void *usbrec_create(void)
{
    usbrec_t *usbrec;
    audio_sink_init_params_t params;


    usbrec = OAL_calloc(1, sizeof(*usbrec));
    if (NULL == usbrec) {
        return NULL;
    }


    usbrec->usb_fs = USBREC_AUDIO_FS;
    usbrec->usb_sample_size = sizeof(int16_t);
    usbrec->usb_ch = 2;

    params.fs = usbrec->usb_fs;
    params.sample_size = USBREC_AUDIO_SAMPLE_SIZE;
    params.ch = USBREC_AUDIO_CH;
    params.input_sample_size = USBREC_AUDIO_SAMPLE_SIZE;
    params.input_ch = USBREC_AUDIO_CH;
    params.buf = usbrec->buf;
    params.buf_bytes = USBA_BUF_SZ;
    params.trigger_th = TRIGGER_TH;
    params.period_bytes = AUDIO_SINK_PROC_PERIOD*params.sample_size*params.ch;
    params.sink_update_t_th = 1400;
    params.use_new_interp_flag = 1;
    audio_sink_init(&usbrec->common, &params);
    usbrec->status = USBREC_STA_ACTIVE;
    usbrec_handle = usbrec;

#if USB_MIC_NEED_RESAMPLE
    usbrec->usb_resample_ring_buffer = libc_malloc(384* 2);
    ring_buffer_init(&usbrec->usb_resample_ctrl, usbrec->usb_resample_ring_buffer, 384* 2);
    
    usbrec_resample_param.sample = 48000;
    usbrec_resample_param.channels = MIC_INPUT_CHANNALS;
    usbrec_resample_param.sample_size = 2;
#endif

    return usbrec;
}

#if USB_MIC_NEED_RESAMPLE
AT_SRAM(.usbd_resmaple)
void usbd_upstream_audio_write(void* user, void* buf, int32_t samples)
{
    int wsize = ring_buffer_get_w_avail(&usbrec_handle->usb_resample_ctrl);
    if (wsize >= samples * 2 * MIC_INPUT_CHANNALS) {
        ring_buffer_write(&usbrec_handle->usb_resample_ctrl, buf,  samples * 2 * MIC_INPUT_CHANNALS);
    }
}

void usbrec_create_resample(uint32_t sample_rate)
{
    logi("usbrec_create_resample %d", sample_rate);

    if(usbd_resample_handle != NULL) {
        destroy_audiosrate_conventor(usbd_resample_handle);
    }
    
    asrc_init_param aip;
    aip.io_period = AUDIO_SINK_PROC_PERIOD;
    aip.ch        = MIC_INPUT_CHANNALS;
    aip.sample_size = USBD_MIC_SAMPLEBIT;
    aip.srate_in    = 48000;
    aip.srate_out   = sample_rate;
    aip.quality     = ASRC_QUALITY_HIGH;
    aip.user = NULL;
    aip.clbk = (user_xet_data_clbk)usbd_upstream_audio_write;
    aip.fast_mode = 1;
    usbd_resample_handle = create_audiosrate_conventor(&aip);
    if(!usbd_resample_handle) {
        logd("usbd resample src init fail!");
    }

    usbrec_resample_param.sample = sample_rate;
    usbrec_resample_param.channels = MIC_INPUT_CHANNALS;
    usbrec_resample_param.sample_size = 2;

    if(usb_resample_thread_handle == NULL) {
        usb_resample_thread_handle = OAL_thread_create(usb_resample_loop, NULL, "usbresample", BT_ISR_BH_TASK_PRIO, 256);
    }
}

void usbrec_destroy_resample()
{
    resample_thread_end = true;
    if(g24_resample_queue) {
        uint32_t irq_stat;
        OAL_queue_send(&g24_resample_queue, &irq_stat, 0);
        OAL_sleep(20);
        OAL_queue_delete(&g24_resample_queue);
        g24_resample_queue = NULL;

        usb_resample_thread_handle = NULL;
    }

    if(usbd_resample_handle != NULL) {
        destroy_audiosrate_conventor(usbd_resample_handle);
        usbd_resample_handle = NULL;
    }
}

void usbrec_samplerate_change(uint32_t sample_rate)
{
    logi("usbrec_samplerate_change %d-%d", sample_rate, usbrec_resample_param.sample);
    if(usbrec_resample_param.sample != sample_rate) {
        usbrec_resample_param.sample = sample_rate;

        usbrec_create_resample(sample_rate);
    }
}

#endif

void usbrec_stop(void *handle, uint32_t fs, uint8_t sample_size, uint8_t ch)
{

}


//保存usbrec数据, 只支持2ch, 16bit
AT_SRAM(.usbrec_store_data)
void usbrec_store_data(void *handle, void *buf, uint32_t samples)
{
    usbrec_t *usbrec = handle;
    if (NULL == usbrec) {
        return;
    }

    if (usbrec->status != USBREC_STA_ACTIVE) {
        return;
    }

    if (usbrec->resample_handle) {
        //TODO:重采样
    } else {
        
#if USB_MIC_NEED_RESAMPLE
        if(usbrec_resample_param.sample != 48000) {
            ring_buffer_proceed(&usbrec->common,buf, samples);
        } else {
            audio_sink_write(&usbrec->common, usbrec->interpbuf, buf, samples);
        }
#else
        audio_sink_write(&usbrec->common, usbrec->interpbuf, buf, samples);
#endif
    }
}

AT_SRAM(.usbrec_data_read)
void usbrec_data_read(void *handle, void *buf, uint32_t bytes)
{
    usbrec_t *usbrec = handle;
    uint32_t samples;

    if (NULL == usbrec) {
        memset(buf, 0, bytes);
        return;
    }

    if (usbrec->status != USBREC_STA_ACTIVE) {
        memset(buf, 0, bytes);
        return;
    }

    if (usbrec->usb_ch && usbrec->usb_sample_size) {
        samples = bytes / (usbrec->usb_ch * usbrec->usb_sample_size);
        audio_sink_read(&usbrec->common, buf, samples);
    }
}

AT_SRAM(.usbrec_data_read)
void usbrec_resample_data_read(void *buf, uint32_t bytes)
{
#if USB_MIC_NEED_RESAMPLE
    if(usbrec_resample_param.sample != 48000) {
        int rsize = ring_buffer_get_r_avail(&usbrec_handle->usb_resample_ctrl);
        if (rsize >= bytes) {
            ring_buffer_read(&usbrec_handle->usb_resample_ctrl, buf,  bytes);
        }
    } else {
        
#if USB_MIC_CHANNEL == USB_MIC_CHANNEL_MONO
        usbrec_data_read(&usbrec_handle->common, (void*)resample_raw, 48*2*2);
        stereo16_to_mono16(buf, (int16_t*)resample_raw, 48);
#else 
        usbrec_data_read(&usbrec_handle->common, (void*)buf, 48*2*2);
#endif
    }
#else   //48K only
#if USB_MIC_CHANNEL == USB_MIC_CHANNEL_MONO
    usbrec_data_read(&usbrec_handle->common, (void*)resample_raw, 48*2*2);
    stereo16_to_mono16(buf, (int16_t*)resample_raw, 48);
#else
    usbrec_data_read(&usbrec_handle->common, (void*)buf, 48*2*2);
#endif

#endif
}

