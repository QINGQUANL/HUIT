#ifndef _WRX_TRACK_H_
#define _WRX_TRACK_H_

#include "sys_inc.h"

typedef struct {
    uint32_t debug_wrx_normal_cnt;
    uint32_t debug_wrx_loss_cnt;
    uint32_t debug_wrx_bad_cnt;
    uint32_t debug_wrx_overrun_cnt;
    uint32_t debug_wrx_underrun_cnt;
} wrx_debug_t;

typedef struct {
    void (*open)(void *handle);
    int32_t (*decode)(void *handle, uint8_t *bs, int16_t **pcmbuf);
    void (*close)(void *handle);
} wrx_decoder_plugin_t;

typedef struct {
    wrx_decoder_plugin_t plugin;
    uint8_t link;
} wrx_track_init_params_t;

extern const wrx_decoder_plugin_t wrx_sbc_plugin;
extern const wrx_decoder_plugin_t wrx_lc3_plugin;

void *wrx_track_create(wrx_track_init_params_t *prm);
void wrx_track_start(void *handle);
wrx_debug_t wrx_debug_info_get(void *handle);
void wrx_mute_set(void *handle, uint8_t mute);
uint8_t wrx_mute_get(void *handle);
void wrx_volume_set(void *handle, float vol_db);
uint8_t wrx_ch_get(void *handle);
void put_g24_bs_to_wrx_track(void *handle, uint8_t *bs, uint32_t bytes, bool valid);
uint8_t wrx_info_ch_get(void *handle);
uint16_t wrx_track_bs_len_get(void);

#endif