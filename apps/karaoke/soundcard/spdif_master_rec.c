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
#include "app_spdif.h"
#include "spdif_master_rec.h"

void spdif_rec_data_read(void *handle, void *buf, uint32_t bytes);

uint8_t* spdif_tx_buff;

spdif_rec_t *spdifrec;
static void *spdif_tx_handle;
bool spdi_tx_stop = false;
AT_SRAM(.spdif_rec)
void spdif_tx_dma_full_callback(void)
{
    if(!spdi_tx_stop) {
        if(spdifrec) {
            spdif_rec_data_read(spdifrec, spdif_tx_buff, SPIID_TX_LEN);
            //printf_array(spdif_tx_buff, 32);
            //logi("");
            HAL_spdif_write(spdif_tx_handle, spdif_tx_buff, SPIID_TX_LEN  );
        } 
    }
}

enum {
    SPDIF_REC_STA_IDLE,
    SPDIF_REC_STA_ACTIVE,
};


void *spdif_rec_create(void)
{
    
    audio_sink_init_params_t params;

    spdif_tx_buff = OAL_malloc(SPDIF_REC_AUDIO_FS/1000 * SPDIF_REC_AUDIO_CH * SPDIF_REC_AUDIO_SAMPLE_SIZE);

    spdifrec = OAL_calloc(1, sizeof(*spdifrec));
    if (NULL == spdifrec) {
        return NULL;
    }


    spdifrec->usb_fs = SPDIF_REC_AUDIO_FS;
    spdifrec->usb_sample_size = sizeof(int16_t);
    spdifrec->usb_ch = 2;

    params.fs = spdifrec->usb_fs;
    params.sample_size = SPDIF_REC_AUDIO_SAMPLE_SIZE;
    params.ch = SPDIF_REC_AUDIO_CH;
    params.input_sample_size = SPDIF_REC_AUDIO_SAMPLE_SIZE;
    params.input_ch = SPDIF_REC_AUDIO_CH;
    params.buf = spdifrec->buf;
    params.buf_bytes = SPDIF_BUF_SZ;
    params.trigger_th = TRIGGER_TH;
    params.period_bytes = AUDIO_SINK_PROC_PERIOD*params.sample_size*params.ch;
    params.sink_update_t_th = 1400;
    params.use_new_interp_flag = 1;
    audio_sink_init(&spdifrec->common, &params);

    spdifrec->status = SPDIF_REC_STA_ACTIVE;


    hal_spdif_stream_init_t init;
    //HAL_gpio_config(PIN_B28 | PIN_FUNC9); //spdif tx gpio
    init.dma_pre_full_callback = NULL;
    init.dma_full_callback = spdif_tx_dma_full_callback;
    init.fs = SPDIF_REC_AUDIO_FS;
    init.sample_size = SPDIF_REC_AUDIO_SAMPLE_SIZE;
    init.ch = SPDIF_REC_AUDIO_CH;
    init.direction = M_SPDIF_TX;
    init.transfer_type = SPDIF_TRANSFER_TYPE_DMA;
    spdif_tx_handle = HAL_spdif_open(&init);

    return spdifrec;
}


//保存usbrec数据, 只支持2ch, 16bit
AT_SRAM(.spdif_rec)
void spdif_rec_store_data(void *handle, void *buf, uint32_t samples)
{
    spdif_rec_t *usbrec = handle;

    if (NULL == usbrec) {
        return;
    }

    if (usbrec->status != SPDIF_REC_STA_ACTIVE) {
        return;
    }

    if (usbrec->resample_handle) {
        //TODO:重采样
    } else {
        audio_sink_write(&usbrec->common, usbrec->interpbuf, buf, samples);
    }

}

AT_SRAM(.spdif_rec)
void spdif_rec_data_read(void *handle, void *buf, uint32_t bytes)
{
    spdif_rec_t *usbrec = handle;
    uint32_t samples;

    if (NULL == usbrec) {
        memset(buf, 0, bytes);
        return;
    }

    //if (usbrec->status != SPDIF_REC_STA_ACTIVE) {
    //    memset(buf, 0, bytes);
    //    return;
    //}

    if (usbrec->usb_ch && usbrec->usb_sample_size) {
        samples = bytes / (usbrec->usb_ch * usbrec->usb_sample_size);
        audio_sink_read(&usbrec->common, buf, samples);
    }
}

void spdif_rec_start()
{
    spdi_tx_stop = false;
    logi("spdif start ----");
    memset(spdif_tx_buff, 0x00, SPIID_TX_LEN);
    HAL_spdif_write(spdif_tx_handle, spdif_tx_buff, SPIID_TX_LEN  ); //启动
}

void spdif_rec_stop()
{
    spdi_tx_stop = true;
}

