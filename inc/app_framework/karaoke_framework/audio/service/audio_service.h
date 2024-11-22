#ifndef _AUDIO_SERVICE_H_
#define _AUDIO_SERVICE_H_

#include "sys_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "oal.h"
#include "sltk_codecs_common.h"

#define REC_IN   0
#define PLBK_OUT 1

#define INPUT_SEL_IS_1CH(_SEL) ((_SEL <= 4) || (_SEL >= 9 && _SEL <= 10))

/*
 * pcm数据流信息结构体
 */
typedef struct {
    void *buf;              /* pcm 数据buffer */
    uint32_t fs;            /* 采样率（e.g. 48000) */
    uint16_t samples;         /* buffer数据长度(samples) */
    uint8_t channels;       /* 声道数 */
    uint8_t sample_size;    /* 采样点数据大小（e.g. 16bits大小为2， 32bits大小为4） */
} audio_stream_info_t;


/*
 * 音频服务初始化参数
 */
typedef struct {
    uint32_t alg_memory_size;
    uint8_t vbus_in_mute;
    uint8_t vbus_in_pause;
    uint8_t incoming_ringtone_fix_volume;
} audio_service_init_params_t;


/*
 * 音频服务cmd
 */
enum audio_service_cmd_e {
    AS_CMD_NONE,

    /* vocal cut(消除人声功能); (karaoke mode command)
     * The command argument indicates enable/disable vocal cut
     * Example:
     *    //enable vocal cut
     *    audio_service_cmd(AS_CMD_VOCAL_CUT, 1);
     *    //disable vocal cut
     *    audio_service_cmd(AS_CMD_VOCAL_CUT, 0);
     */
    AS_CMD_VOCAL_CUT,

    /* audio duck(闪避功能); (karaoke mode command)
     * The command argument indicates enable/disable duck
     * Example:
     *    //enable duck
     *    audio_service_cmd(AS_CMD_DUCK_ENABLE, 1);
     *    //disable duck
     *    audio_service_cmd(AS_CMD_DUCK_ENABLE, 0);
     */
    AS_CMD_DUCK_ENABLE,

    /* config duck attack time; (karaoke mode command)
     * The command argument indicates attack time
     * Example:
     */
    AS_CMD_DUCK_SETTING_ATTACK_TIME,

    /* config duck release time; (karaoke mode command)
     * The command argument indicates release time
     * Example:
     */
    AS_CMD_DUCK_SETTING_RELEASE_TIME,

    /* config duck gain; (karaoke mode command)
     * The command argument indicates duck gain
     * Example:
     */
    AS_CMD_DUCK_SETTING_DUCK_GAIN,

    /* config duck thd; (karaoke mode command)
     * The command argument indicates duck thd
     * Example:
     */
    AS_CMD_DUCK_SETTING_DUCK_THD,
    AS_CMD_DUCK_GETTING_ATTACK_TIME,
    AS_CMD_DUCK_GETTING_RELEASE_TIME,
    AS_CMD_DUCK_GETTING_DUCK_GAIN,
    AS_CMD_DUCK_GETTING_DUCK_THD,

    /* set mic channels mask; (karaoke mode command)
     * The command argument indicates channels mask(bit0-ADC0, bit1-ADC1, bit2-ADC2)
     * Example:
     *    //ADC0
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x1);
     *    //ADC1
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x2);
     *    //ADC2
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x4);
     *    //ADC0,ADC1
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x3);
     *    //ADC0,ADC2
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x5);
     */
    AS_CMD_SET_KARAOKE_MIC_CH_MASK,

    /* set linein channels mask; (karaoke mode command)
     * The command argument indicates channels mask(bit0-ADC0, bit1-ADC1, bit2-ADC2)
     * Example:
     *    //ADC0
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x1);
     *    //ADC1
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x2);
     *    //ADC2
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x4);
     *    //ADC0,ADC1
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x3);
     *    //ADC0,ADC2
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_MIC_CH_MASK, 0x5);
     */
    AS_CMD_SET_KARAOKE_LINEIN_CH_MASK,

    /* set self mix mode; (karaoke mode command)
     * The command argument is ignored
     * Example:
     *    //set self mix mode
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_SELF_MIX_MODE, 1);
     */
    AS_CMD_SET_KARAOKE_SELF_MIX_MODE,


    /* set direct mix mode; (karaoke mode command)
     * The command argument is ignored
     * Example:
     *    //set direct mix mode
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_DIRECT_MIX_MODE, 1);
     */
    AS_CMD_SET_KARAOKE_DIRECT_MIX_MODE,


    /* mute/unmute karaoke mic when play music; (karaoke mode command)
     * The command argument indicates mute/unmute
     * Example:
     *    //mute mic when play music
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_REC_SWITCH, 1);
     *    //unmute mic when play music
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_REC_SWITCH, 0);
     */
    AS_CMD_SET_KARAOKE_REC_SWITCH,


    /* enable/disabel mic0/mic1; (karaoke mode command)
     * The command argument indicates enable/disable
     * Example:
     *    //enable
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_AMIC01_SWITCH, 1);
     *    //disable
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_AMIC01_SWITCH, 0);
     */
    AS_CMD_SET_KARAOKE_AMIC01_SWITCH,

    /* enable/disabel linein0/linein1; (karaoke mode command)
     * The command argument indicates enable/disable
     * Example:
     *    //enable
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_LINE01_SWITCH, 1);
     *    //disable
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_LINE01_SWITCH, 0);
     */
    AS_CMD_SET_KARAOKE_LINE01_SWITCH,


    /* enable/disabel mic2; (karaoke mode command)
     * The command argument indicates enable/disable
     * Example:
     *    //enable
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_AMIC2_SWITCH, 1);
     *    //disable
     *    audio_service_cmd(AS_CMD_SET_KARAOKE_AMIC2_SWITCH, 0);
     */
    AS_CMD_SET_KARAOKE_AMIC2_SWITCH,


    /* Once freq shift's runtime prm has been settled, it should
     * be called by user to sync prm with driver
     * Example:
     *    audio_service_cmd(AS_CMD_NOTIFY_FS_LOAD_RTPRM, 0);
     */
    AS_CMD_NOTIFY_FS_LOAD_RTPRM,


    /* Once NHS's runtime prm has been settled, it should
     * be called by user to sync prm with driver
     * Example:
     *    audio_service_cmd(AS_CMD_NOTIFY_NHS_LOAD_RTPRM, 0);
     */
    AS_CMD_NOTIFY_NHS_LOAD_RTPRM
};

/*
 * audio input source define
 */
enum audio_input_source_e {
    AUDIO_INPUT_AMIC0           =  0, /* 模拟MIC0 */
    AUDIO_INPUT_AMIC1           =  1, /* 模拟MIC1 */
    AUDIO_INPUT_AMIC2           =  2, /* 模拟MIC2 */
    AUDIO_INPUT_DMIC0F          =  3, /* 数字MIC0下降沿(falling edge) */
    AUDIO_INPUT_DMIC0R          =  4, /* 数字MIC0上升沿(rising edge) */
    AUDIO_INPUT_AMIC0_1         =  5, /* 模拟MIC0和模拟MIC1 */
    AUDIO_INPUT_AMIC0_2         =  6, /* 模拟MIC0和模拟MIC2 */
    AUDIO_INPUT_AMIC1_2         =  7, /* 模拟MIC1和模拟MIC2 */
    AUDIO_INPUT_DMIC0F_R        =  8, /* 数字MIC0下降沿和下降沿 */
    AUDIO_INPUT_LINEIN0         =  9, /* linein0 */
    AUDIO_INPUT_LINEIN1         = 10, /* linein1 */
    AUDIO_INPUT_LINEIN0_1       = 11, /* linein0和linein1 */
    AUDIO_INPUT_AUX0            = 12, /* aux0 */
    AUDIO_INPUT_AUX1            = 13, /* aux1 */
    AUDIO_INPUT_AUX0_1          = 14, /* aux0和aux1 */
    AUDIO_INPUT_NONE            = 0xFF,
};

/*
 * 音频焦点拥有者，表示当前谁在使用音频资源
 */
enum audiofocus_holder_e {
    AUDIOFOCUS_HOLDER_NONE,
    AUDIOFOCUS_HOLDER_AUDIOPLAYER,  /* 表示本地播放或者提示音播放 */
    AUDIOFOCUS_HOLDER_A2DPAUDIO,    /* 表示蓝牙音乐 */
    AUDIOFOCUS_HOLDER_SCOAUDIO,     /* 表示蓝牙通话 */
    AUDIOFOCUS_HOLDER_USBAUDIO,     /* 表示usb audio */
    AUDIOFOCUS_HOLDER_LINEINAUDIO,  /* 表示linein播放 */
    AUDIOFOCUS_HOLDER_SPDIFAUDIO,   /* 表示spdif播放 */
};

/*
 * 音频特性
 */
enum audio_feature_e {
    AUDIO_FEATURE_NONE                  = 0,
    AUDIO_FEATURE_PLAYBACK_DECODER      = (1 << 0),  /* 表示有解码器在工作 */
    AUDIO_FEATURE_PLAYBACK_EFFECTS      = (1 << 1),  /* 表示有播放音效在工作 */
    AUDIO_FEATURE_RECORD_ENCODER        = (1 << 2),  /* 表示有编码器在工作 */
    AUDIO_FEATURE_RECORD_EFFECTS        = (1 << 3),  /* 表示有录音（MIC）音效在工作 */
    AUDIO_FEATURE_SPEAKER_AENC          = (1 << 4),  /* 表示扬声器类型的通话算法在工作 */
    AUDIO_FEATURE_HEADSET_AENC          = (1 << 5),  /* 表示耳机类型的通话算法在工作 */
    AUDIO_FEATURE_AENC_INIT             = (1 << 6),  /* 表示通话算法初始化 */
    AUDIO_FEATURE_TONE                  = (1 << 7),  /* 表示提示音在播放 */
    AUDIO_FEATURE_INCOMING_TONE         = (1 << 8),  /* 表示来电铃声(本地铃声)在播放 */
    AUDIO_FEATURE_A2DP_FADE             = (1 << 9),  /* 表示蓝牙音乐正在进行淡入淡出处理 */
    AUDIO_FEATURE_SCO_TSPS              = (1 << 10), /* 表示通话变调处理在工作 */
    AUDIO_FEATURE_ANC                   = (1 << 11), /* 表示ANC在工作 */
    AUDIO_FEATURE_KWS                   = (1 << 12), /* 表示KWS在工作 */
    AUDIO_FEATURE_AMIXER                = (1 << 13), /* 表示AMIXER在工作 */
    AUDIO_FEATURE_SPACE_AUDIO           = (1 << 14), /* 表示space audio在工作 */
    AUDIO_FEATURE_OPUS                  = (1 << 15), /* 表示opus audio在工作 */
    AUDIO_FEATURE_MIC_AENC              = (1 << 16), /* 表示MIC AENC在工作 */
};

enum music_feature_e {
    MUSIC_FEATURE_METADATA  = (1 << 0),
    MUSIC_FEATURE_WAV       = (1 << 1),
    MUSIC_FEATURE_MP3       = (1 << 2),
    MUSIC_FEATURE_WMA       = (1 << 3),
    MUSIC_FEATURE_FLAC      = (1 << 4),
    MUSIC_FEATURE_AAC       = (1 << 5),
    MUSIC_FEATURE_APE       = (1 << 6),
    MUSIC_FEATURE_OGG       = (1 << 7),
};

/*
 * 解码器类型
 */
enum audio_decoder_type_e {
    AUDIO_DECODER_TYPE_NONE,
    AUDIO_DECODER_TYPE_WAV,
    AUDIO_DECODER_TYPE_MP3,
    AUDIO_DECODER_TYPE_WMA,
    AUDIO_DECODER_TYPE_FLAC,
    AUDIO_DECODER_TYPE_AAC,
    AUDIO_DECODER_TYPE_APE,
    AUDIO_DECODER_TYPE_OGG,
    AUDIO_DECODER_TYPE_SBC,
    AUDIO_DECODER_TYPE_CVSD,
    AUDIO_DECODER_TYPE_AAC_X
};

/*
 * 编码器类型
 */
enum audio_encoder_type_e {
    AUDIO_ENCODER_TYPE_NONE,
    AUDIO_ENCODER_TYPE_WAV,
    AUDIO_ENCODER_TYPE_MP3,
    AUDIO_ENCODER_TYPE_SBC,
    AUDIO_ENCODER_TYPE_CVSD,
    AUDIO_ENCODER_TYPE_MP2
};

#define SPIFLASH_TYPE_MASK (0x0F)

/*
 * flash 提示音资源类型
 */
enum spiflash_tone_type_e {
    SPIFLASH_TONE_NUM       = 0,  /* mp3提示音(此类型提示音不能和音乐同时播放) */
};

typedef struct vol_map_s {
    int32_t vol;     //音量值
    float decibels;  //音量值对应衰减多少个dB
} vol_map_t;

/*
 * 获取录音（MIC)音效链组件句柄
 */
void *audio_service_get_record_afx_chain(void);

/*
 * 获取音乐音效链组件句柄
 */
void *audio_service_get_playback_afx_chain(void);

uint8_t audio_service_get_afx_chain_attach_status(uint8_t io);

uint32_t audio_service_get_base_audiopll_freq(uint32_t fs);

/*
 * 初始化音频服务
 */
int audio_service_init(audio_service_init_params_t *params);
void audio_arec_thread_init(void);

/*
    * 关闭音频服务
 */
void audio_service_deinit(void);

void audio_service_thread_init(void);

/* 添加音频特性；
 * 此函数一般由audio库调用
 * @param features: 参考@enum audio_feature_e
 * 返回 ：预设音频特性是否已经存在
 */
bool audio_service_add_features(uint32_t features);

/* 清除音频特性；
 * 此函数一般由audio库调用
 * @param features: 参考@enum audio_feature_e
 * 返回 ：预清音频特性是否已经存在
 */
bool audio_service_clear_features(uint32_t features);


/* 获取当前的音频特性；
 * @return: 返回当前音频特性，参考@enum audio_feature_e
 */
uint32_t audio_service_get_features(void);

/* 设置编码器类型；
 * 此函数一般由audio库调用
 * @param type: 编码器类型，参考@enum audio_encoder_type_e
 */
void audio_service_set_encoder_type(uint32_t type);

/* 获取编码器类型；
 * @return: 返回当前编码器类型，参考@enum audio_encoder_type_e
 */
uint32_t audio_service_get_encoder_type(void);

/* 设置解码器类型；
 * 此函数一般由audio库调用
 * @param type: 解码器类型，参考@enum audio_decoder_type_e
 */
void audio_service_set_decoder_type(uint32_t type);


/* 获取解码器类型；
 * @return: 返回当前解码器类型，参考@enum audio_decoder_type_e
 */
uint32_t audio_service_get_decoder_type(void);

/*
 * 请求音频焦点（由audio库内部调用）
 */
int audio_service_request_audiofocus(uint32_t focus_holder);

/*
 * 释放音频焦点（由audio库内部调用）
 */
void audio_service_free_audiofocus(uint32_t holder);

/* 获取音频焦点拥有者；
 * @return: 返回音频焦点拥有者，含义参考 @enum audiofocus_holder_e
 */
uint32_t get_audiofocus_holder(void);

play_timestamp_info_t get_play_timestamp(void *atrack);
int64_t get_play_samples(void *atrack);
uint32_t get_play_remain(void);
int64_t get_aout_play_samples(void);

float volume_db_to_volf32(float vol_db);
float vol2decibels(int32_t vol);


void audio_service_switch_input_source(enum audio_input_source_e input, bool enable);

/* 获取播放状态;
 * NOTE:此接口获取得到的状态是底层驱动的状态;
 * @return: 返回值含义参考 @enum pcm_hw_state
 */
uint8_t get_play_status(void *atrack);

/* 输入源转换成ADC channels mask
 * @param input: 输入源， 参考@enum audio_input_source_e;
 * @return: return ADC channels(bit0-ADC0, bit1-ADC1, bit2-ADC2) mask;
 *
 * Example:
 *      uint32_t amic_ch_mask;
 *      amic_ch_mask = audio_service_get_input_channels_mask(AUDIO_INPUT_AMIC2);
 *      switch (amic_ch_mask) {
 *      case 1: logi("channel 0"); break;
 *      case 2: logi("channel 1"); break;
 *      case 3: logi("channel 0 and channel 1"); break;
 *      case 4: logi("channel 2"); break;
 *      case 5: logi("channel 0 and channel 2"); break;
 *      case 6: logi("channel 1 and channel 2"); break;
 *      }
 */
uint32_t audio_service_get_input_channels_mask(enum audio_input_source_e input);

uint8_t audio_service_get_input_channels_count(enum audio_input_source_e input);

/* 设置输入源的模拟音量
 * @param input: audio input source, see @enum audio_input_source_e
 * @param gain: analog gain, range see "user_config.h 'analog gain range' ""
 *
 * Example:
 *      //set amic2 analog gain as 4
 *      audio_service_set_input_analog_gain(AUDIO_INPUT_AMIC2, 4);
 *      // set linein0 linein1 analog gain as 6
 *      audio_service_set_input_analog_gain(AUDIO_INPUT_LINEIN0_1, 6);
 */
void audio_service_set_input_analog_gain(enum audio_input_source_e input, uint32_t gain);

/* 设置输入源的数字音量
 * @param input: audio input source, see @enum audio_input_source_e;
 * @param gain: digital gain, range see "user_config.h 'digital gain' ""
 *
 * Example:
 *      //set amic2 digital gain as 0
 *      audio_service_set_input_digital_gain(AUDIO_INPUT_AMIC2, 0);
 *      //set linein0 linein1 digital gain as -6
 *      audio_service_set_input_digital_gain(AUDIO_INPUT_LINEIN0_1, -6);
 */
void audio_service_set_input_digital_gain(enum audio_input_source_e input, int32_t gain);

/* 打开输入源
 * @param input: audio input source, see @enum audio_input_source_e
 *
 * Example:
 *      //enable amic2
 *      audio_service_enable_input_source(AUDIO_INPUT_AMIC2);
 *      //enable linein0 linein1
 *      audio_service_enable_input_source(AUDIO_INPUT_LINEIN0_1);
 */
void audio_service_enable_input_source(enum audio_input_source_e input);

/* 关闭输入源
 * @param input: audio input source, see @enum audio_input_source_e
 *
 * Example:
 *      //disable amic2
 *      audio_service_disable_input_source(AUDIO_INPUT_AMIC2);
 *      //disable linein0 linein1
 *      audio_service_disable_input_source(AUDIO_INPUT_LINEIN0_1);
 */
void audio_service_disable_input_source(enum audio_input_source_e input);

/* 选择和打开mic(record, call)
 * @param input: audio input source, see @enum audio_input_source_e
 *
 * Example:
 *      //select amic2 as mic(record, call) input
 *      audio_service_disable_input_source(AUDIO_INPUT_AMIC2);
 *      //select linein0 linein1 as mic(record, call) input
 *      audio_service_disable_input_source(AUDIO_INPUT_LINEIN0_1);
 */
void audio_service_set_mic_input_source(enum audio_input_source_e input);

/* 获取当前MIC输入
 * @return: return current mic input source
 *
 * Example:
 *      enum audio_input_source_e src;
 *      src = audio_service_get_mic_input_source();
 *      switch (src) {
 *      case AUDIO_INPUT_AMIC0: logi("current mic input is analog mic 0"); break;
 *      case AUDIO_INPUT_AMIC1: logi("current mic input is analog mic 1"); break;
 *      case AUDIO_INPUT_AMIC2: logi("current mic input is analog mic 2"); break;
 *      case AUDIO_INPUT_DMIC0F: logi("current mic input is digital mic0(DMICDATA0) falling edge data"); break;
 *      ...
 *      }
 */
enum audio_input_source_e audio_service_get_mic_input_source(void);

/* 选择和打开linein（模式）输入
 * @param input: audio input source, see @enum audio_input_source_e
 *
 * Example:
 *      //select amic2 as linein(mode) input
 *      audio_service_set_linein_input_source(AUDIO_INPUT_AMIC2);
 *      //select linein0 linein1 as linein(mode) input
 *      audio_service_set_linein_input_source(AUDIO_INPUT_LINEIN0_1);
 */
void audio_service_set_linein_input_source(enum audio_input_source_e input);

/* 关闭linein（模式）输入
 * @return: return current linein input source
 *
 * Example:
 *      enum audio_input_source_e src;
 *      src = audio_service_get_linein_input_source();
 *      switch (src) {
 *      case AUDIO_INPUT_AMIC0: logi("current linein(mode) input is analog mic 0"); break;
 *      case AUDIO_INPUT_AMIC1: logi("current linein(mode) input is analog mic 1"); break;
 *      case AUDIO_INPUT_AMIC2: logi("current linein(mode) input is analog mic 2"); break;
 *      case AUDIO_INPUT_DMIC0F: logi("current linein(mode) input is digital mic0(DMICDATA0) falling edge data"); break;
 *      ...
 *      }
 */
enum audio_input_source_e audio_service_get_linein_input_source(void);

uint32_t audio_service_input_source_to_x_mask(enum audio_input_source_e input);
enum audio_input_source_e audio_service_x_mask_to_input(uint32_t x_mask);

/* 发送一个命令给audio服务
 * @param cmd: audio_service command, see @enum audio_service_cmd_e
 * @param arg: audio_service command argument
 * @return: return zero on success, a negative errno value on failure
 *
 * Example:
 *    //enable duck
 *    audio_service_cmd(AS_CMD_DUCK_ENABLE, 1);
 *    //disable duck
 *    audio_service_cmd(AS_CMD_DUCK_ENABLE, 0);
 */
int audio_service_cmd(uint32_t cmd, uint32_t val);

/* 设置录音（MIC)音量
 * @param volume: record(mic) volume: 0 ~ 100
 *
 * Example:
 *    //set record(mic) volume as 50
 *    audio_service_set_record_volume(50);
 */
void audio_service_set_record_volume(uint8_t volume);

/* 获取录音（MIC)音量
 * @return: current record(mic) volume(0~100)
 *
 * Example:
 *    uint8_t vol = audio_service_get_record_volume();
 *    logi("current record(mic) volume is %d", vol);
 */
uint8_t audio_service_get_record_volume(void);

bool audio_service_get_mute_mode_status(void);

bool audio_service_get_mute_mode(void);
void audio_service_set_mute_mode(bool enter_mute_mode);

float volume_to_volumef32(int32_t vol, const vol_map_t *map, uint32_t map_size);

uint8_t audio_service_vbus_in_mute_get(void);
uint8_t audio_service_vbus_in_pause_get(void);
uint8_t audio_service_incoming_ringtone_fix_volume_get(void);



extern const parser_plugin_t wma_parser_plugin;
extern const decoder_plugin_t wma_decoder_plugin;

extern const parser_plugin_t ape_parser_plugin;
extern const decoder_plugin_t ape_decoder_plugin;

extern const parser_plugin_t ogg_parser_plugin;
extern const decoder_plugin_t ogg_decoder_plugin;

extern const parser_plugin_t aac_parser_plugin;
extern const parser_plugin_t aacm4a_parser_plugin;
extern const sltk_decoder_plugin_t aac_decoder_plugin;

extern const parser_plugin_t wav_parser_plugin;
extern const decoder_plugin_t pcm_decoder_plugin;

extern const parser_plugin_t mp3_parser_plugin;
extern const decoder_plugin_t mp3_decoder_plugin;

extern const parser_plugin_t flac_parser_plugin;
extern const decoder_plugin_t flac_decoder_plugin;

extern const parser_plugin_t sbc_parser_plugin;
extern const decoder_plugin_t sbc_decoder_plugin;

void parser_plugin_register(ctype_t ctype, const parser_plugin_t *plugin);
void decoder_plugin_register(atype_t atype, const decoder_plugin_t *plugin);
void usbdev_audio_service_thread_init();
#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_SERVICE_H_ */
