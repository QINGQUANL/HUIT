#ifndef _KWS_MANAGER_H_
#define _KWS_MANAGER_H_

#include "sys_inc.h"
#include "event.h"

typedef struct {
    void (*on_keyword_wakeup)(const char *keyword);
    uint32_t mic_pga_gain;
    /* minimum psd of noise in dBfs */
    int32_t lambda_min;
    /* minimum psd of voice in concerned freq region in dBfs */
    int32_t voice_min;
} am_kws_init_params_t;

int32_t am_kws_create(am_kws_init_params_t *params);
void am_kws_destroy(void);
void am_kws_global_enable_set(uint8_t enable);
uint8_t am_kws_global_enable_get(void);
int32_t am_kws_start(void);
int32_t am_kws_stop(void);
int32_t am_kws_get_suspend_stat(void);
#endif