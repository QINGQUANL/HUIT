#ifndef _RENDERING_7_1CH_MIX_H
#define _RENDERING_7_1CH_MIX_H

#include "sys_types.h"

void* render_7_1_init(uint32_t fs, uint32_t period);
void render_7_1_deinit(void* handle);
int32_t render_7_1_cmd(void* handle, uint32_t cmd, uint32_t val);
int32_t render_7_1_proc_stage_cpu0(void* handle, float** ch_in, int dummy);
int32_t render_7_1_proc_stage_cpu1(void* handle, float** ch_in, int dummy);

#endif