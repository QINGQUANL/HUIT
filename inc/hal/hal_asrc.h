#ifndef _HAL_ASRC_H_
#define _HAL_ASRC_H_

#include "sys_inc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void (*inblock_callback_func)(void *user_handle);
    void (*outblock_callback_func)(void *user_handle);
    void *user_handle;
    uint32_t memory_addr;
    uint32_t inblock_size;
    uint32_t outblock_size;
    uint32_t inblock_count;
    uint32_t outblock_count;
    uint32_t in_fs;
    uint32_t out_fs;
    uint8_t ch;
    uint8_t sample_size;
    uint8_t asrc_idx;
} hal_asrc_init_t;

void HAL_asrc_init(uint8_t max_asrc_instance);
void HAL_asrc_deinit(void);
void *HAL_asrc_open(hal_asrc_init_t *params);
int32_t HAL_asrc_write(void *handle, void *data, uint32_t samples);
int32_t HAL_asrc_read(void *handle, void *data, uint32_t samples);
uint32_t HAL_asrc_inwrite_avail(void *handle);
uint32_t HAL_asrc_outread_avail(void *handle);
uint8_t HAL_asrc_status_get(void *handle);
void HAL_asrc_ioctrl(void *handle, char *ctrl, uint32_t arg);
void HAL_asrc_close(void *handle);

#ifdef __cplusplus
}
#endif

#endif /* _HAL_ASRC_H_ */
