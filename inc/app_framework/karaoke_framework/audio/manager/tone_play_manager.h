#ifndef _TONE_PLAY_MANAGER_H_
#define _TONE_PLAY_MANAGER_H_

#include "oal.h"
#include "audio_service.h"

#define TONE_TYPE_MAX_CNT (16)

#define SYS_TONE_WELCOME_NAME               "welcome.mp3"
#define SYS_TONE_BTMODE_NAME                "bluetooth_mode.mp3"
#define SYS_TONE_CONNECTED_NAME             "connected.mp3"
#define SYS_TONE_DISCONNECTED_NAME          "disconnected.mp3"
#define SYS_TONE_PEER_CONNECTED_NAME        "peer_connected.mp3"
#define SYS_TONE_PEER_DISCONNECTED_NAME     "peer_disconnected.mp3"
#define SYS_TONE_POWER_OFF_NAME             "power_off.mp3"
#define SYS_TONE_CALL_NAME                  "call.mp3"
#define SYS_TONE_LOW_POWER_NAME             "low_power.mp3"
#define SYS_TONE_GAME_MODE_OPEN_NAME        "game_mode_open.mp3"
#define SYS_TONE_GAME_MODE_OPEN_AFX_NAME    "game_mode_open_afx.mp3"
#define SYS_TONE_GAME_MODE_CLOSE_NAME       "game_mode_close.mp3"
#define SYS_TONE_KEY_NAME                   "key.wav"
#define SYS_TONE_KEY_UP_NAME                "key_up.wav"
#define SYS_TONE_NOISE_NAME                 "noise.mp3"
#define SYS_TONE_CLOSE_NAME                 "close.mp3"
#define SYS_TONE_PASS_THOSE_NAME            "pass_those.mp3"
#define SYS_TONE_SPACE_MODE_OPEN_NAME       "space_open.mp3"
#define SYS_TONE_SPACE_MODE_CLOSE_NAME      "space_close.mp3"
#define SYS_TONE_VERSION_ERROR_NAME         "version_error.mp3"
#define SYS_TONE_VOL_MAX_NAME               "vol_max.mp3"
#define SYS_TONE_VOL_MIN_NAME               "vol_min.mp3"
#define SYS_TONE_EQCONFIG0_NAME             "eqconfig0.mp3"
#define SYS_TONE_EQCONFIG1_NAME             "eqconfig1.mp3"
#define SYS_TONE_EQCONFIG2_NAME             "eqconfig2.mp3"
#define SYS_TONE_EQCONFIG3_NAME             "eqconfig3.mp3"
#define SYS_TONE_VOL_SHUT_PRE               "poweroff_pre.mp3"
#define SYS_TONE_CLEAN_LINK                 "clean_link.mp3"

enum sys_tone_type_e {
    SYS_TONE_TYPE_WELCOME                   = 0,
    SYS_TONE_TYPE_BTMODE                    = 1,
    SYS_TONE_TYPE_CONNECTED                 = 2,
    SYS_TONE_TYPE_DISCONNECTED              = 3,
    SYS_TONE_TYPE_PEER_CONNECTED            = 4,
    SYS_TONE_TYPE_PEER_DISCONNECTED         = 5,
    SYS_TONE_TYPE_POWER_OFF                 = 6,
    SYS_TONE_TYPE_CALL                      = 7,
    SYS_TONE_TYPE_LOW_POWER                 = 8,
    SYS_TONE_TYPE_GAME_MODE_OPEN            = 9,
    SYS_TONE_TYPE_GAME_MODE_OPEN_AFX        = 10,
    SYS_TONE_TYPE_GAME_MODE_CLOSE           = 11,
    SYS_TONE_TYPE_VERSION_ERROR             = 12,
    SYS_TONE_TYPE_VOL_MAX                   = 13,
    SYS_TONE_TYPE_VOL_MIN                   = 14,
    SYS_TONE_TYPE_EQCONFIG0                 = 15,
    SYS_TONE_TYPE_EQCONFIG1                 = 16,
    SYS_TONE_TYPE_EQCONFIG2                 = 17,
    SYS_TONE_TYPE_EQCONFIG3                 = 18,
};

typedef struct sys_tone_info_s {
    uint8_t type;
    const char *tone_name;
    uint8_t volume;
} sys_tone_info_t;

typedef struct {
    const vol_map_t *vol_map;
    uint8_t vol_map_size;
    const sys_tone_info_t *sys_tone_info;
    uint8_t sys_tone_info_size;
    uint8_t incoming_ringtone_policy;
    uint8_t s_con_tone_policy;
} tone_manager_init_params_t;

void am_tone_init(tone_manager_init_params_t *params);

bool tone_name_to_num(const char *tone_name, uint8_t *p_tone_num);
uint8_t am_sys_tone_info_get(enum sys_tone_type_e sys_tone_type, const char **p_tone_name, uint8_t *p_vol);

int am_incoming_tone_play(const char *tone_name, uint8_t volume, uint8_t set_trigger_play_t, uint32_t play_t);
void am_incoming_tone_stop(uint8_t sync_stop, uint32_t sync_stop_sys_t);
uint8_t am_incoming_tone_playing_status_get(void);

uint8_t am_incoming_ringtone_policy_get(void);
uint8_t am_s_con_tone_policy_get(void);

int am_tone_play_by_num(uint8_t tone_num, uint8_t block, uint32_t loop_cnt, uint8_t volume,
    uint8_t set_trigger_play_t, uint32_t play_t);

/*------------------------------------------------------------------------------
* 函数功能: 提示音播放
* 函数名称: int am_tone_play(const char *tone_name, uint8_t block, uint32_t loop_cnt,
*           uint8_t volume, uint8_t set_trigger_play_t, uint32_t play_t)
* 参    数:
*           tone_name: 提示音
*           block: 0-触发播放后马上退出此函数; 1-播放完成后退出此函数
*           loop_cnt: 循环播放次数
*           volume: 提示音音量（范围:0~100)
*           set_trigger_play_t：0-马上播放; 1-开始播放的时间点由play_t参数指定
*           play_t: 开始播放的系统时间(play_t = HAL_timestamp_to_us(HAL_get_timestamp()) + delay_us)
* 返 回 值: 播放成功返回0; 失败返回负值错误码
* 说明 :
*-------------------------------------------------------------------------------*/
int am_tone_play(const char *tone_name, uint8_t block, uint32_t loop_cnt,
    uint8_t volume, uint8_t set_trigger_play_t, uint32_t play_t);

void am_tws_tone_play_by_num(uint8_t tone_num, uint8_t volume, uint8_t peer_play);
/*------------------------------------------------------------------------------
* 函数功能: TWS提示音播放
* 函数名称: void am_tws_tone_play(uint8_t tone_num, uint8_t volume, uint8_t peer_play)
* 参    数:
*           tone_name: 提示音
*           volume: 提示音音量（范围:0~100)
*           peer_play: 0-仅仅调用此函数的耳机(左耳机或者右耳机）播放; 1-双耳同时播放
* 返 回 值:
* 说明 : 此函数会一直阻塞直到提示音播放完成才退出
*-------------------------------------------------------------------------------*/
void am_tws_tone_play(const char *tone_name, uint8_t volume, uint8_t peer_play);
/*------------------------------------------------------------------------------
* 函数功能: 获取提示音状态
* 函数名称: int8_t am_get_tone_status()
* 返 回 值: 获取失败返回-1，获取成功返回提示音状态 
*           0：提示音播放器为闲置状态；1：提示音播放器为暂停中
*           2：提示音播放器为播放中；3：提示音播放器为准备中
*           4：提示音播放器为准备完成；5：提示音播放器为无效状态
* 说明 :
*-------------------------------------------------------------------------------*/
int8_t am_get_tone_status();
#endif