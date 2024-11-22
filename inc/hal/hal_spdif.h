#ifndef _HAL_SPDIF_H_
#define _HAL_SPDIF_H_

#ifdef __cplusplus
 extern "C" {
#endif
#include "sys_inc.h"

enum hal_spdif_transfer_type_e {
    SPDIF_TRANSFER_TYPE_BLOCK,
    SPDIF_TRANSFER_TYPE_IRQ,
    SPDIF_TRANSFER_TYPE_DMA,
};

enum hal_spdif_cmd_e {
    SPDIF_CMD_NONE,
    SPDIF_CMD_RX_LOCK_STATUS_GET,
    SPDIF_CMD_RX_INFO_CAPTRUE,
    SPDIF_CMD_AUSS_DUMP,
};

typedef struct {
    uint32_t fs;
    uint8_t word_length; //bits
    uint8_t ch;
} hal_spdif_rx_info_t;

typedef struct {
    void (*dma_pre_full_callback)(void);
    void (*dma_full_callback)(void);
    void (*lock_callback)();
    void (*unlock_callback)();

    uint32_t fs;
    uint8_t sample_size;
    uint8_t ch;
    uint8_t direction;
    uint8_t transfer_type;

    uint8_t dma_ch;
} hal_spdif_stream_init_t;


typedef struct {
    void *dma_hd;
    int32_t (*rw_func)(void *handle, void *data, int32_t bytes);

    uint32_t fs;
    uint8_t sample_size;
    uint8_t ch;
    uint8_t direction;
    uint8_t transfer_type;
} hal_spdif_stream_t;

typedef struct {
    hal_spdif_stream_t stream[2];
    uint8_t rx_lock_sta;
    uint8_t inited;
    uint8_t use_audiopll;
    uint8_t rx_inited;
    uint8_t tx_inited;

    void (*lock_callback)();
    void (*unlock_callback)();
} hal_spdif_t;


void *HAL_spdif_open(void *init_params);
void HAL_spdif_close(void *handle);
int32_t HAL_spdif_read(void *handle, void *data, int32_t bytes);
int32_t HAL_spdif_write(void *handle, void *data, int32_t bytes);
int32_t HAL_spdif_ioctl(void *handle, uint32_t cmd, void *arg);
void HAL_spdif_rx_enable(bool enable);

#ifdef __cplusplus
}
#endif

#endif
