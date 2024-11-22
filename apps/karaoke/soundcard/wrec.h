#ifndef _WREC_H_
#define _WREC_H_

#include "sys_inc.h"


typedef struct {
    void (*open)(void *handle);
    int32_t (*encode)(void *handle, uint8_t **p_enc_out_buf);
    void (*close)(void *handle);
} wrec_encoder_plugin_t;

typedef struct {
    wrec_encoder_plugin_t plugin;
    uint8_t link;
} wrec_init_params_t;

extern const wrec_encoder_plugin_t wrec_sbc_plugin;
extern const wrec_encoder_plugin_t wrec_lc3_plugin;
extern uint32_t debug_wrec_normal_cnt;
extern uint32_t debug_wrec_underrun_cnt;
extern uint32_t debug_wrec_overrun_cnt;

void *wrec_create(wrec_init_params_t *prm);
uint8_t wrec_bs_read(void *handle, uint8_t link, uint8_t *bs, uint32_t bytes);
void wrec_encode(void *handle, int32_t *buf, uint32_t samples);
uint16_t wrec_bs_len_get(void);

#endif