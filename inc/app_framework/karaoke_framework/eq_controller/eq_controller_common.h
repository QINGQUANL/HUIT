#ifndef EQ_CONTROLLER_COMMON_H
#define EQ_CONTROLLER_COMMON_H

#include "atunning_common.h"
#include "aeffect_plugin.h"

uint8_t* common_get_eq_controller_buf();
void common_set_eq_data_isr(uint8_t* buff,uint32_t length);
void common_set_eq_data(uint8_t* buff,uint32_t length);
uint32_t get_PageId(eq_data* block);
uint32_t get_CmdType(eq_data* block);
uint32_t get_ItemIndex(eq_data* block);
uint32_t get_Data(eq_data* block);
int eq_controller_cmdsbuffer_process(uint8_t *eq_buff, uint32_t length);
uint8_t* get_eq_param_buff_atom();

#endif
