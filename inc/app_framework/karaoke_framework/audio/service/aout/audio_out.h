
#ifndef _AUDIO_OUT_H_
#define _AUDIO_OUT_H_

#include "sys_types.h"
#include "sltk_audio.h"
#ifdef __cplusplus
extern "C" {
#endif

#define AOUT_USE_STATIC_MEM


typedef enum audio_mode_e {
    AUDIO_MODE_NATIVE_MUSIC         = 0,
    AUDIO_MODE_TONE                 = 1,
    AUDIO_MODE_A2DP_MUSIC           = 2,
    AUDIO_MODE_SCO                  = 3,
    AUDIO_MODE_PASSTHRU             = 4,
    AUDIO_MODE_LINEIN               = 5,
    AUDIO_MODE_LMIC                 = 6,
    AUDIO_MODE_WMIC                 = 7,
    AUDIO_MODE_WMIC1                = 8,
    AUDIO_MODE_USBPLAY              = 9,
    AUDIO_MODE_I2S_SLAVER_PLAY      = 10,
    AUDIO_MODE_SPDIF                = 11,
} audio_mode_t;

typedef enum audio_ao_status_e {
    AUDIO_AO_STATUS_IDLE            = 0,
    AUDIO_AO_STATUS_PLAYING         = 1,
    AUDIO_AO_STATUS_XRUN            = 2,
} audio_ao_status_t;

typedef enum aout_cmd_e {
    SLTK_CMD_AO_GET_STATUS                      = 0,
    SLTK_CMD_AO_SET_MUTE_MODE                   = 1,
    SLTK_CMD_AO_START_FADE                      = 2,
} aout_cmd_t;
typedef aout_cmd_t sltk_aout_cmd_t;

typedef struct aout_interface_s aout_interface_t;

typedef struct {
    uint32_t fs;                    /* sampling rate(e.g. 44100) */
    uint16_t period_size;           /* The number of sample processed at one time. (NOTE: If config 0, will use default) */
    uint16_t period_count;          /* The number of period. (NOTE: If config 0, will use default) */
    uint16_t fadein_ms;             /* Fade-in time (in milliseconds) */
    uint16_t fadeout_ms;            /* Fade-out time (in milliseconds) */
    uint16_t headmute_ms;           /* The number of sample that needs to be muted before normal playing */
    uint8_t ch;                     /* channels */
    uint8_t sample_size;            /* Data size of a sample. (e.g. 2-16bit, 4-32bit) */
    uint8_t mode;                   /* aout mode (e.g. AUDIO_MODE_NATIVE_MUSIC) */
} aout_init_t;

struct aout_interface_s {
    int32_t (*init)(aout_interface_t *self, aout_init_t *init);

    int32_t (*write)(aout_interface_t *self, void *buf, int32_t samples);

    int32_t (*deinit)(aout_interface_t *self);

    int32_t (*cmd)(aout_interface_t *self,
        int32_t cmd, int32_t val);

    uint8_t (*get_status)(aout_interface_t *self);
    uint32_t (*get_remain)(aout_interface_t *self);
    play_timestamp_info_t (*get_play_timestamp)(aout_interface_t *self);
    int64_t (*get_play_samples)(aout_interface_t *self);
};

typedef struct {
    uint8_t snd_id;
    uint8_t hw_ch_mask;
} aout_params_t;

void aout_params_set(aout_params_t *params);

typedef enum aout_type_e {
    AOUT_FATFS,
    AOUT_DAC,
    AOUT_I2S,
    AOUT_KARAOKE,
    AOUT_PRIMARY,
} aout_type_t;

aout_interface_t *create_aout(uint32_t type);
void destroy_aout(aout_interface_t *aout);

#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_OUT_H_ */
