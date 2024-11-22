#ifndef _HAL_VAD_H_
#define _HAL_VAD_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys_inc.h"

enum hal_vad_ioctl_e {
    HAL_VAD_IOCTL_NONE,
    HAL_VAD_IOCTL_START,
    HAL_VAD_IOCTL_STOP,
    HAL_VAD_IOCTL_PGA_SET,
    HAL_VAD_IOCTL_HPF_SET,
};

typedef struct {
    uint16_t rshift;
    uint16_t mp;
    uint8_t enable;
} hal_vad_hpf_params_t;

typedef struct {
    uint32_t data_ram_addr;
    uint32_t period_size;
    uint32_t period_count;
    uint32_t fs;
} hal_vad_init_t;

void *HAL_vad_open(void *vad_params);
void HAL_vad_close(void *handle);
int32_t HAL_vad_read(void *handle, void *buf, uint32_t bytes);
int32_t HAL_vad_ioctl(void *handle, uint32_t cmd, uint32_t arg);

#ifdef __cplusplus
}
#endif

#endif
