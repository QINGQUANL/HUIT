#ifndef _I2S_SLAVER_TRACK_H_
#define _I2S_SLAVER_TRACK_H_

#include "sys_inc.h"

void *i2s_slaver_play_track_create(void);
void i2s_slaver_play_track_start(void *handle);
bool i2s_slaver_play_ready_get(void *handle);

void i2s_slaver_play_track_write(void *handle, void *buf, uint32_t samples);
void i2s_slaver_play_mute_set(void *handle, uint8_t mute);
uint8_t i2s_slaver_play_mute_get(void *handle);
void i2s_slaver_play_volume_set(void *handle, float vol_db);
float i2s_slaver_play_volume_get(void *handle);

#endif