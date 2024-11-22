#ifndef AEFFECT_COMPONENT_H
#define AEFFECT_COMPONENT_H

#include "sys_inc.h"
#include "aeffect_plugin.h"

typedef struct {
    acommon_init_para  asource_common;
    const aeffect_list_info *cpu0_list;
    const aeffect_list_info *cpu1_list;
    notify_clbk notify;
    void*    user_data;
    int8_t   acpux_accelerate;
}aeffect_comp_init;

typedef enum {
    AEFFECT_COMPONENT_CMD_NONE = 0,
    AEFFECT_COMPONENT_CMD_GET_ASSIGNED_TUNNINGAREA,
    AEFFECT_COMPONENT_CMD_RESET_ASSIGNED_TUNNINGAREA,
    AEFFECT_COMPONENT_CMD_LOAD_ASSIGNED_TUNNINGAREA,
    AEFFECT_COMPONENT_CMD_CLEAR_EFFECT_PLG_DATA,
    AEFFECT_COMPONENT_CMD_START_ACPUX_INSTANCE,
    AEFFECT_COMPONENT_CMD_STOP_ACPUX_INSTANCE,
    AEFFECT_COMPONENT_CMD_ENABLE_PROC,
    AEFFECT_COMPONENT_CMD_DISABLE_PROC
}aeffect_comp_cmd;

int32_t aeffect_component_xet_ctrl(void* handle, uint32_t cmd, uint32_t val, uint32_t rev);
void* aeffect_component_create(aeffect_comp_init* init);
void  aeffect_component_destroy(void* handle);
int32_t aeffect_component_proc_period(void* handle, void* io_buffer, int samples);

#endif