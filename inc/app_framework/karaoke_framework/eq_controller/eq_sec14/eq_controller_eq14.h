#ifndef EQ_CONTROLLER_EQ14_H
#define EQ_CONTROLLER_EQ14_H

#include "eq_controller_common.h"
#include "aeffect_eq14.h"
#include "eq14_tunning_channel.h"

aeffect_eq_sec14_tunningarea* getEq14TunningParareaForPageId(uint32_t page_id);
void loadEq14TunningParareaForPageId(uint32_t page_id);
void resetEq14TunningParareaForPageId(uint32_t page_id);
TUNNING_SETTING_STU getEq14AllSubTunningParareaForPageId(uint32_t page_id, eq_sec_setting** eq_sec_l_t, eq_sec_setting** eq_sec_r_t,
    eq_preamp_setting** eq_amp_l, eq_preamp_setting** eq_amp_r);
bool eq14_area_recive_cmd(uint32_t page_id, uint32_t cmd_type, uint32_t item_index, uint32_t data_ctx);

#endif