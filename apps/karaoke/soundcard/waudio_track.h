#ifndef WAUDIO_TRACK_H
#define WAUDIO_TRACK_H

#include "sys_inc.h"
#include "audio_track.h"
#include "ring_buffer.h"

/*
    Notify :
        Notify should be called by self or lower layer
*/
typedef enum {
    WAUDIOTRACK_CMD_STOP = 0,
    WAUDIOTRACK_CMD_DRAINING,
    WAUDIOTRACK_CMD_START,
    WAUDIOTRACK_CMD_FLUSH,
    WAUDIOTRACK_CMD_GIVE_A_SHIT,
    WAUDIOTRACK_CMD_GET_STU,
    WAUDIOTRACK_CMD_NOTIFY,
    WAUDIOTRACK_CMD_PLAYTIME_SET, //设置track开始播放的时间点
    WAUDIOTRACK_CMD_PLAYTIME_FLAG_GET,
    WAUDIOTRACK_CMD_FADE_BOOK,
    WAUDIOTRACK_CMD_BASESTAMP_GET,
    WAUDIOTRACK_CMD_VOLUME_SET,
    WAUDIOTRACK_CMD_MUTETIME_SET,
}wat_cmd;

enum waudio_track_data_sta_e {
    WAUDIO_TRACK_DATA_STA_INIT,
    WAUDIO_TRACK_DATA_STA_NORMAL,
    WAUDIO_TRACK_DATA_STA_UNDERRUN,
};

int   controlWAudioTrack(void* handle, uint32_t cmd, uint32_t val);
int   debufferWAudioTrack(void* handle, void* dst, uint32_t samples);
void WAudioTrackPostProc(void* handle, void* buf, uint32_t samples);
void  destroyWAudioTrack(void* wat);
void* createWAudioTrack(audio_track_init_t *init, int32_t id, struct audio_share_buffer** sb);

#endif