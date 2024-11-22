#ifndef _EQ_CONTROLLER_VVS_H_
#define _EQ_CONTROLLER_VVS_H_

#include "eq_controller_common.h"

void vss_reset_tunning_pararea(void);
bool vss_area_recive_cmd(uint32_t page_id, uint32_t cmd_type, uint32_t item_index, uint32_t data_ctx);
bool vss_enable_get();
void vss_enable_set(uint8_t enable);
#endif