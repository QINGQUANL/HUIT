#ifndef _EQ_CONTROLLER_FDNREV_H_
#define _EQ_CONTROLLER_FDNREV_H_

#include "eq_controller_common.h"

void fdnrev_reset_tunning_pararea(void);
bool fdnrev_area_recive_cmd(uint32_t page_id, uint32_t cmd_type, uint32_t item_index, uint32_t data_ctx);

#endif