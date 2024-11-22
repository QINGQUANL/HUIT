#ifndef _LOCAL_PLAY_SERVICE_H_
#define _LOCAL_PLAY_SERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sys_inc.h"

/*
 * 获取本地播放特性
 */
uint32_t as_localplay_get_music_features(void);

/*
 * 获取audio_player组件句柄
 */
void *as_localplay_get_audio_player(void);

void as_localplay_set_localmusic_volume(uint8_t volume);
float as_localplay_get_localmusic_volume_f32(void);
uint8_t as_localplay_get_localmusic_volume(void);
int as_localplay_service_init(void *local_play_init);

#ifdef __cplusplus
}
#endif

#endif /* _LOCAL_PLAY_SERVICE_H_ */
