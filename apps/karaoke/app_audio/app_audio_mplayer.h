#ifndef APP_AUDIO_MPLAYER_H
#define APP_AUDIO_MPLAYER_H

void app_audio_mplayer_mode_switch_proc(uint8_t mode, bool enter);
void app_audio_mplayer_scene_save(void);
void app_audio_mplayer_recovery(void);
int  app_audio_mplayer_init();

#endif