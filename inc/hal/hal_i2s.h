#ifndef _HAL_I2S_H_
#define _HAL_I2S_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys_inc.h"

enum hal_i2s_format_e {
    I2S_FORMAT_I2S,
    I2S_FORMAT_LEFT_JUSTIFIED,
    I2S_FORMAT_RIGHT_JUSTIFIED,
    I2S_FORMAT_PCM_LONG_FRAME,
    I2S_FORMAT_PCM_SHORT_FRAME
};

enum hal_i2s_transfer_type_e {
    I2S_TRANSFER_TYPE_BLOCK,
    I2S_TRANSFER_TYPE_IRQ,
    I2S_TRANSFER_TYPE_DMA,
};

//注意此处是配BCLK0/LRCK0, 6线模式等非标准接口需要通过ioctrl配置
typedef struct {
    uint32_t mclk_div; //mclk = audio_pll / mclk_div
    uint8_t mclk_en;
    uint8_t master; //0:slave; 1:master
    uint8_t format; //I2S; Left Justified; Right Justified; PCM long Frame; PCM Short Frame
    uint8_t slot_number;
    uint8_t slot_width;
} hal_i2s_init_t;

typedef struct {
    void (*dma_pre_full_callback)(void);
    void (*dma_full_callback)(void);

    uint32_t fs;
    uint8_t sample_size; //对应Sample Resolution
    uint8_t ch;
    uint8_t direction;
    uint8_t transfer_type;

    uint8_t dma_ch;
} hal_i2s_stream_init_t;

void HAL_i2s_init(hal_i2s_init_t *params);
void HAL_i2s_ctrl(char *ctrl, uint32_t arg);
void *HAL_i2s_open(hal_i2s_stream_init_t *params);
void HAL_i2s_close(void *handle);
uint32_t HAL_i2s_tx_timestamp_get(void *handle);

void HAL_i2s_tx_start(void *handle);
void HAL_i2s_tx_stop(void *handle);
int32_t HAL_i2s_tx_write(void *handle, void *data, int32_t bytes);

void HAL_i2s_rx_start(void *handle);
void HAL_i2s_rx_stop(void *handle);
int32_t HAL_i2s_rx_read(void *handle, void *data, uint32_t bytes);


#ifdef __cplusplus
}
#endif

#endif
