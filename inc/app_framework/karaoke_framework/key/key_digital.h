#ifndef _KEY_DIGITAL_H_
#define _KEY_DIGITAL_H_
#include "hal.h"

typedef struct {
    uint32_t long_sta;
    uint32_t hold_sta;
} kdig_t;

typedef struct key_dig_info {
    bool enable;
    uint32_t key_event;
} key_dig_info_t;

void kdig_init(uint8_t clk_div, const kdig_ch_param_t* init_table, key_dig_info_t *table, uint8_t table_size);
void kdig_irq_cb(uint32_t pending, key_dig_sta_t *kdata);

#endif

