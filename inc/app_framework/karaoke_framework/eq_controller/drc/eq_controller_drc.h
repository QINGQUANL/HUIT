#ifndef _EQ_CONTROLLER_DRC_H_
#define _EQ_CONTROLLER_DRC_H_

#include "eq_controller_common.h"

typedef enum {
    DRC_ITEM_LIMITER    = 0,
    DRC_ITEM_COMPRESSOR = 1,
    DRC_ITEM_EXPANDER   = 2,
    DRC_ITEM_NOISEGATE  = 3,
} drc_item_index_type;
    
void drc_reset_tunning_pararea_for_page_id(uint32_t page_id);
bool drc_area_recive_cmd(uint32_t page_id, uint32_t cmd_type, uint32_t item_index, uint32_t data_ctx);

#endif