

#ifndef _SLTK_AUDIO_H_
#define _SLTK_AUDIO_H_

#ifdef _MSC_VER
#include "al_types.h"
#else
#include "sys_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define VOLUME_MAX                          100
#define VOLUME_MIN                          0



typedef enum {
    SLTK_STYPE_FATFS = 0x0, /* The audio data comes from fatfs */
    SLTK_STYPE_FLASH_TONE,      /* The tone data comes from tonefs(flash) */
    SLTK_STYPE_FLASH_RECORD,
} streamtype_t;

typedef enum {
    SLTK_TTYPE_FILE,
    SLTK_TTYPE_DAC,
} terminal_t;

/* container type, for loading parser lib */
typedef enum {
    SLTK_CTYPE_UNSUPPORTED = -1,
    SLTK_CTYPE_MP3 = 0x0,
    SLTK_CTYPE_WMA,
    SLTK_CTYPE_AAC,
    SLTK_CTYPE_M4A,
    SLTK_CTYPE_APE,
    SLTK_CTYPE_FLAC,
    SLTK_CTYPE_OGG,
    SLTK_CTYPE_WAV,
    SLTK_CTYPE_SBC,
    SLTK_CTYPE_NUM
} ctype_t;
typedef ctype_t sltk_ctype_t;

/* codec type, for loading decoder lib */
typedef enum {
    SLTK_ATYPE_UNSUPPORTED = -1,
    SLTK_ATYPE_MP3 = 0x0,
    SLTK_ATYPE_WMA,
    SLTK_ATYPE_AAC,
    SLTK_ATYPE_APE,
    SLTK_ATYPE_FLAC,
    SLTK_ATYPE_VORBIS,
    SLTK_ATYPE_PCM,
    SLTK_ATYPE_SBC,
} atype_t;
typedef atype_t sltk_atype_t;

typedef struct {
    /* codec type */
    atype_t codec_type;
    /* total time in ms */
    int32_t total_time;
    /* average bit rate in kbps */
    int32_t avg_br;
    /* sampling rate */
    int32_t fs;
    /* channels number */
    int32_t channels;
    /* frame size */
    int32_t chunk_size;
    /* bs buffer */
    uint8_t *bs_buf;
    /* init buffer accordance with the codec type */
    void *init_data;
} stream_info_t;
typedef stream_info_t sltk_stream_info_t;

typedef struct {
    uint8_t fmt_ext[8];
    uint8_t codec_ext[8];
    /* total time in ms */
    int32_t total_time;
    /* average bit rate in kbps */
    int32_t avg_br;
    /* sampling rate */
    int32_t fs;
    /* channels number */
    int32_t channels;
    /* media or muxer type */
    ctype_t media_type;
} media_info_t;

typedef struct {
    float tone_gain;            /* 提示音增益（范围:0.0~1.0） */
    float ducking_gain;         /* 闪避增益（播放提示音时，音乐被压制的增益值, 范围:0.0~1.0） */
    int32_t sin_samples;        /* 正弦波播放多长时间（单位:采样点） */
    int32_t mute_samples;       /* 静音播放多长时间（单位:采样点） */
    int32_t period_count;       /* 播放多少个”正弦波-静音“周期 */
    const int32_t *psinbuf;     /* 指定正弦波数据 */
    int32_t sinbuf_len;         /* 正弦波数据长度 */
} sine_tone_info_t;

typedef enum _audio_notify {
    AUDIO_NOTIFY_AMSG = 0,
    AUDIO_NOTIFY_AMIXER,
    AUDIO_NOTIFY_ATRACK,
    AUDIO_NOTIFY_ARECORD,
    AUDIO_NOTIFY_TPLAYER,
    AUDIO_NOTIFY_AEFFECT_PLUGIN,
    AUDIO_NOTIFY_AEFFECT_COMPONENT,
    AUDIO_NOTIFY_ACPUX_INSTANCE,
    AUDIO_NOTIFY_LOCAL_PLAY_MANAGER,
    AUDIO_NOTIFY_AUDIO_PLAYER,
    AUDIO_NOTIFY_MUSIC_MODE_EVENT_PROCESSOR
}audio_notify;

typedef uint32_t (*notify_clbk)(void* cookie, uint32_t who, uint32_t cmd, uint32_t val, uint32_t rev);

typedef struct {
    uint32_t cmd;
    uint32_t val;
} cmd_msg_t;

typedef struct {
    uint8_t id;
    uint8_t direction;
    uint8_t ch;
    uint8_t sample_size;
    uint32_t rate;
}pcm_params_info;

typedef struct {
    int64_t audio_counter_fs;/* 采样率计数器，每播放一个点，计数器加1 */
    uint32_t audio_counter_fs_x128;/* 128倍过采样计数计，可理解为counter_fs的小数部分，每播放1/128个点计数器加1 */
    uint32_t audio_delta_sys_t;/* 从抓音频数据到此时的时间差 */
} play_timestamp_info_t;

typedef int (*pcm_dbg_hook)(pcm_params_info* info, const void* buf, int samples);

#ifdef __cplusplus
}
#endif

#endif /* _SLTK_AUDIO_H_ */
