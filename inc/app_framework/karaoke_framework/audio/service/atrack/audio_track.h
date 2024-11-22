

#ifndef _AUDIO_TRACK_H_
#define _AUDIO_TRACK_H_

#include "sys_inc.h"
#include "sltk_codecs_common.h"
#include "oal.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    TRACK_STATUS_IDLE,
    TRACK_STATUS_PLAYING,
    TRACK_STATUS_ERROR,
};

//NONE: keep sync @enum waudio_track_data_sta_e
enum track_data_sta_e {
    TRACK_DATA_STA_INIT,
    TRACK_DATA_STA_NORMAL,
    TRACK_DATA_STA_UNDERRUN,
};

enum {
    TRACK_CMD_NONE,
    TRACK_CMD_START,
    TRACK_CMD_DRAINING,
    TRACK_CMD_STOP,

    TRACK_CMD_RESET_STATUS,
    TRACK_CMD_GET_STATUS,

    TRACK_CMD_PLAYTONE,
    TRACK_CMD_GET_PLAYTONE_STATUS,
    TRACK_CMD_SET_PLAYTONE_LOCK,
    TRACK_CMD_GET_PLAYTONE_LOCK,

    TRACK_CMD_IDLE,

    TRACK_CMD_SET_PLAYTIME,
    TRACK_CMD_PLAYSAMPLES_GET,
    TRACK_CMD_FADE_BOOK,

    TRACK_CMD_BREAK_BLOCK_WRITE,
    TRACK_CMD_VOLUME_SET,
    TRACK_CMD_MUTETIME_SET,
    TRACK_CMD_WRITE_AVAIL_GET,
    TRACK_CMD_PLAYTIME_FLAG_GET,
};

typedef enum {
    AUDIOTRACK_MSG_NONE = 0,
    AUDIOTRACK_MSG_WATRACK_PULLDATA_REQ,
    AUDIOTRACK_MSG_WATRACK_POSTPROC_REQ,
    AUDIOTRACK_MSG_AMIXER_NOT_READY,
    AUDIOTRACK_MSG_AMIXER_RE_READY,
    AUDIOTRACK_MSG_WATRACK_STOPPING_FINISHED
}atrack_msg;

enum atrack_flags_e {
    AUDIOTRACK_FLAGS_NONE,
    AUDIOTRACK_FLAGS_MUSIC_EFFECTS = (1 << 0),
    AUDIOTRACK_FLAGS_CALL_EFFECTS = (1 << 1),
};

typedef struct {
    notify_clbk notify;
    void*    user_data;
    uint32_t fs;
    uint16_t sharebuf_samples;
    uint16_t mode;
    uint16_t fadein_ms;
    uint16_t fadeout_ms;
    uint8_t  ch;
    uint8_t  sample_size;
    uint8_t  input_interleave;
    uint8_t pulldata_from_user;
} audio_track_init_t;

typedef struct {
    uint32_t track_id;
    uint32_t playtime;
} atrack_playtime_info_t;

typedef struct {
    uint32_t track_id;
    uint32_t book_fade_sys_t; //开始fade的时间点
    uint32_t fade_ms; //fade时间设置
    uint8_t  fade_type;
} atrack_fade_book_t;

typedef struct {
    uint32_t track_id;
    float vol_f32;
} atrack_volume_info_t;

typedef struct {
    uint32_t track_id;
    uint32_t mute_ms;
} atrack_mute_info_t;

typedef struct {
    int64_t play_stamp;
    int64_t mix_stamp;
    uint32_t sys_stamp;
    uint32_t bt_stamp;
} audio_stamp_info_t;

typedef struct {
    uint32_t track_id;
    uint32_t sharebuffer_ptr;
}atrack_init_rply;

typedef struct {
    struct audio_share_buffer* sharebuffer;
    audio_track_init_t         info;
    uint8_t                    track_id;
    uint8_t                    track_stu;
    uint32_t                   remain_samples;
    uint32_t                   remain_samples_t;
    notify_clbk                notify;
    void*                      user_data;
} audio_track_t;

void* audio_track_open(audio_track_init_t *init);
void audio_track_close(void *track);
int audio_track_cmd(void *track, uint32_t cmd, uint32_t val);
int audio_track_write(void *track, aoutinfo_t *info, bool interleave);
int audio_track_write_isr(void *track, aoutinfo_t *info, bool interleave);
uint32_t audio_track_get_remain_samples(void *track);

extern OAL_THREAD track_handler;

#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_TRACK_H_ */
