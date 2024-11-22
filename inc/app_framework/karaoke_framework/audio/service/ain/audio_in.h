

#ifndef _AUDIO_IN_H_
#define _AUDIO_IN_H_

#include "sys_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief: audio_in_mode indicates what scenarios pcm data is used for
 */
typedef enum audio_in_mode_e {
    AUDIO_IN_MODE_NATIVE_REC = 0, //本地录音
    AUDIO_IN_MODE_LINEIN     = 1, //linein
    AUDIO_IN_MODE_SCO        = 2, //SCO通话
    AUDIO_IN_MODE_KWS        = 3, //语音唤醒
    AUDIO_IN_MODE_AIANC      = 4, //自适应ANC
    AUDIO_IN_MODE_BONE       = 5, //骨振动传感器
} audio_in_mode_t;

/*
 * @brief: audio in command
 */
typedef enum ain_cmd_e {
    /* start fade-in or fade-out
     * Example:
     *      //start fade-in
     *      ain->cmd(ain, SLTK_CMD_AI_START_FADE, FADE_IN);
     *      //start fade-out
     *      ain->cmd(ain, SLTK_CMD_AI_START_FADE, FADE_OUT);
     */
    SLTK_CMD_AI_START_FADE    = 1,

    SLTK_CMD_AI_HW_PATH_OPEN = 2, //打开硬件通路
    SLTK_CMD_AI_HW_PATH_CLOSE = 3, //关闭硬件通路
    SLTK_CMD_AI_CH_MASK_GET = 4, //获取数据通道mask
    SLTK_CMD_AI_SESSION_REQ, //请求ain session
    SLTK_CMD_AI_SESSION_DES, //释放ain session
} ain_cmd_t;


enum AIN_HW_PATH_PRM_TYPE_E {
    AIN_HW_PATH_PRM_TYPE_CONFIG_BY_MODE,
    AIN_HW_PATH_PRM_TYPE_CONFIG_BY_INPUT_SOURCE,
};

typedef struct {
    uint8_t session;
    uint8_t prm_type;
    union {
        uint8_t mode;
        uint32_t input_source;
    };
} ain_hw_path_params_t;

typedef ain_cmd_t sltk_ain_cmd_t;
typedef struct ain_interface_s ain_interface_t;

typedef struct {
    uint32_t fs;            /* sampling rate(e.g. 44100) */
    uint16_t fadein_ms;     /* Fade-in time (in milliseconds) */
    uint16_t fadeout_ms;    /* Fade-out time (in milliseconds) */
    uint16_t headmute_ms;   /* The number of sample that needs to be muted before normal recording */
    uint16_t period_size;   /* The number of sample processed at one time. (NOTE: If config 0, will use default) */
    uint16_t period_count;  /* The number of period. (NOTE: If config 0, will use default) */
    uint8_t ch;             /* channels */
    uint8_t sample_size;    /* Data size of a sample. (e.g. 2-16bit, 4-32bit) */
    uint8_t mode;           /* ain mode (e.g. AUDIO_IN_MODE_NATIVE_REC) */
} ain_init_t;

struct ain_interface_s {
    int32_t (*init)(ain_interface_t *self, ain_init_t *init);
    int32_t (*read)(ain_interface_t *self, void *buf, int32_t samples);
    int32_t (*deinit)(ain_interface_t *self);
    int32_t (*cmd)(ain_interface_t *self,
        int32_t cmd, int32_t val);
};

typedef enum ain_type_e {
    AIN_PRIMARY = 0,
    AIN_XPHONE  = 1,
    AIN_KARAOKE = 2,
} ain_type_t;

typedef struct {
    uint8_t snd_id;
    uint32_t (*get_input_source_func)(enum audio_in_mode_e mode);
} ain_params_t;

void ain_params_set(ain_params_t *params);

typedef ain_interface_t* (*create_ain_t)(uint32_t type);
typedef void (*destroy_ain_t)(ain_interface_t *ain);

ain_interface_t* create_ain(uint32_t type);
void destroy_ain(ain_interface_t *ain);


#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_IN_H_ */
