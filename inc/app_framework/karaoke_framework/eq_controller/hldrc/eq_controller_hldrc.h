#ifndef _EQ_CONTROLLER_HLDRC_H_
#define _EQ_CONTROLLER_HLDRC_H_

#include "eq_controller_common.h"

typedef enum {
    HLDRC_ITEM_LIMITER    = 0,
    HLDRC_ITEM_COMPRESSOR = 1,
    HLDRC_ITEM_EXPANDER   = 2,
    HLDRC_ITEM_NOISEGATE  = 3,
} hldrc_item_index_type;

void hldrc_reset_tunning_pararea();
bool hldrc_area_recive_cmd(uint32_t page_id, uint32_t cmd_type, uint32_t item_index, uint32_t data_ctx);

#endif