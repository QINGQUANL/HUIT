#ifndef _SPDIF_TRACK_H_
#define _SPDIF_TRACK_H_

#include "sys_inc.h"

void *spdif_play_track_create(void);
void spdif_play_track_start(void *handle, uint32_t fs);
bool spdif_play_ready_get(void *handle);

void spdif_play_track_write(void *handle, void *buf, uint32_t samples);
void spdif_play_mute_set(uint8_t mute);
uint8_t spdif_play_mute_get();
void spdif_play_volume_set(float vol_db);
float spdif_play_volume_get();
void spdif_input_sel(uint8_t input_sel);
uint8_t spdif_get_work_input();
#endif