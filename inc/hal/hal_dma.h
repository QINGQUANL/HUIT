#ifndef _HAL_DMA_H_
#define _HAL_DMA_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys_inc.h"

enum hal_dma_ctl_e {
    HAL_DMA_CTL_NONE,
    HAL_DMA_CTL_SET_IRQ_CPU,
};

typedef struct {
    void (*pre_full_callback)(void);
    void (*full_callback)(void);

    uint32_t pre_full_level;

    uint8_t ch;
    uint8_t irq_priority;

    uint8_t dst_data_width;
    uint8_t dst_burst_len;
    uint8_t dst_drq_type;
    uint8_t dst_drq_blk;

    uint8_t src_data_width;
    uint8_t src_burst_len;
    uint8_t src_drq_type;
    uint8_t src_drq_blk;

    uint8_t remaining_mode : 1;
} hal_dma_init_params_t;

void *HAL_dma_open(hal_dma_init_params_t *params);
int HAL_dma_start(void *handle, uint32_t dst_addr, uint32_t src_addr, uint32_t bytes);
int HAL_dma_continue(void *handle, uint32_t dst_addr, uint32_t src_addr, uint32_t bytes);
int HAL_dma_continue_abort(void *handle);
int HAL_dma_pause(void *handle, uint8_t pause);
int HAL_dma_stop(void *handle);
uint32_t HAL_dma_remaining_get(void *handle);

int HAL_dma_ioctl(void *handle, uint32_t ctl, uint32_t prm);
void HAL_dma_close(void *handle);

#ifdef __cplusplus
}
#endif


#endif

