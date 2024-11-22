#ifndef _HAL_MBOX_H_
#define _HAL_MBOX_H_

#include "sys_inc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mbox_init_params_s {
    void (*dst_callback)(void);
    uint8_t src_core_id;
    uint8_t dst_core_id;
} mbox_init_params_t;

typedef struct mbox_message_s {
    uint32_t msg;
    uint32_t val;
} mbox_message_t;

void hal_mbox_init(void *params);
uint8_t get_cpu_id(void);
void *hal_mbox_open(void *mbox_init_params);
void hal_mbox_close(void *handle);
int32_t hal_mbox_ioctl(void *handle, uint32_t cmd, void *arg);
int32_t hal_mbox_read(void *handle, void *message, uint32_t bytes);
int32_t hal_mbox_write(void *handle, void *message, uint32_t bytes);

#ifdef __cplusplus
}
#endif

#endif /* _HAL_MBOX_H_ */
