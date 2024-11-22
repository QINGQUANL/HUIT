#ifndef _SPDIF_MASTER_REC_H_
#define _SPDIF_MASTER_REC_H_

#include "sys_inc.h"
#include "audio_sink_common.h"
#include "sltk_math.h"
#include "sltk_interp.h"


#define SPDIF_REC_AUDIO_FS           (48000)               //audio端采样率(固定和amixer一致)
#define SPDIF_REC_AUDIO_CH           2                     //audio端声道数(固定和amixer一致)
#define SPDIF_REC_AUDIO_SAMPLE_SIZE  (sizeof(int16_t))     //audio端采样精度

//缓存大小
#define SPDIF_BUF_SZ ((AUDIO_SINK_PROC_PERIOD*4 + 48*2) * SPDIF_REC_AUDIO_SAMPLE_SIZE * SPDIF_REC_AUDIO_CH)
//触发读取的数据量阈值
#define TRIGGER_TH  ((AUDIO_SINK_PROC_PERIOD+48*2) * SPDIF_REC_AUDIO_SAMPLE_SIZE * SPDIF_REC_AUDIO_CH)

#define SPIID_TX_LEN    (SPDIF_REC_AUDIO_FS/1000 * SPDIF_REC_AUDIO_CH * SPDIF_REC_AUDIO_SAMPLE_SIZE)

//usb录音
typedef struct {
    audio_sink_common_t common; //插值控制器
    uint8_t buf[SPDIF_BUF_SZ];   //缓存buffer
    int16_t interpbuf[SPDIF_REC_AUDIO_CH * (G_AUDIO_PROC_PERIOD + MAX_INTERP_CNT)];

    void *resample_handle;

    float vol_db;               //音量(对数域)
    uint32_t usb_fs;            //usb端的采样率
    uint8_t usb_sample_size;    //usb端的采样精度
    uint8_t usb_ch;             //usb端的声道数

    uint8_t muted;              //静音状态: 1-静音; 0-不静音

    uint8_t status;             //状态
} spdif_rec_t;

void spdif_rec_store_data (void *handle,void *buf, uint32_t samples);
void *spdif_rec_create(void);
void spdif_rec_start();
void spdif_rec_stop();
#endif
