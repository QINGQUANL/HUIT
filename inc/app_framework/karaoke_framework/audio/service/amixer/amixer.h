#ifndef AMIXER_H
#define AMIXER_H

#include "sys_inc.h"
#include "audio_track.h"

#define G_AUDIO_PROC_PERIOD   (64)

#define AMIXER_AUDIOTRACK_INSUFFICENT   0xFF

typedef enum _amixer_cmd{
    AMIXER_CMD_NONE = 0,
    AMIXER_CMD_REQ_AUDIOTRACK, //请求创建一个音轨用于播放
    AMIXER_CMD_DES_AUDIOTRACK, //销毁一个音轨
    AMIXER_CMD_AUDIOTRACK_START, //开始播放
    AMIXER_CMD_AUDIOTRACK_DRAINING,//TODO: remove it
    AMIXER_CMD_AUDIOTRACK_STOP,  //停止播放
    AMIXER_CMD_GET_APPO_SHAREBUFFER_SIZE, //获取sharebuffer大小
    AMIXER_CMD_GET_AMIXER_PCM_CHANNEL_CFG, //获取amixer线程实际使用的声道数
    AMIXER_CMD_GET_AMIXER_PCM_BPS_CFG,     //获取amixer线程实际使用的采样精度
    AMIXER_CMD_GET_AMIXER_PCM_SRATE_CFG,   //获取amixer线程实际使用的采样率
    AMIXER_CMD_GET_AMIXER_PCM_PROC_PREIOD_CFG, //获取amixer线程一帧处理的点数
    AMIXER_CMD_GET_AMIXER_PCM_INTERLEAVE_FMT,
    AMIXER_CMD_GET_HAL_RUNTIME_STU,
    AMIXER_CMD_START,
    AMIXER_CMD_STOP,
    AMIXER_CMD_WAUDIOTRACK_GIVE_A_SHIT,
    AMIXER_CMD_TRACK_PLAYTIME_SET, //设置track开始播放的时间
    AMIXER_CMD_TRACK_PLAYTIME_FLAG_GET,
    AMIXER_CMD_TRACK_FADE_BOOK,
    AMIXER_CMD_TRACK_DATA_STA_GET,
    AMIXER_CMD_MIXSTAMP_GET, //混音时间戳获取
    AMIXER_CMD_PLAYSTAMP_INFO_GET, //播放时间戳信息获取
    AMIXER_CMD_TRACKPLAYSAMPLES_GET,
    AMIXER_CMD_AOUTPLAYSAMPLES_GET,
    AMIXER_CMD_PLAYSPEED_SET,      //设置播放速度
    AMIXER_CMD_TRACK_VOLUME_SET, //设置音量
    AMIXER_CMD_TRACK_MUTETIME_SET, //设置mute时间
    AMIXER_CMD_START_AEFFECT_ACPUX_SCHEDULE,
    AMIXER_CMD_STOP_AEFFECT_ACPUX_SCHEDULE,
    AMIXER_CMD_ENABLE_AEFFECT_PROC,
    AMIXER_CMD_DISABLE_AEFFECT_PROC,
    AMIXER_CMD_GET_AEFFECT_TUNNING_AREA,
    AMIXER_CMD_RESET_AEFFECT_TUNNING_AREA,
    AMIXER_CMD_LOAD_AEFFECT_TUNNING_AREA,
    AMIXER_CMD_EFFECTS_VOL_SET,
    AMIXER_CMD_AOUT_REQ,
    AMIXER_CMD_AOUT_REL,
    AMIXER_CMD_RELOAD_EQ,
}amixer_cmd;

typedef enum _amixer_stu{
    AMIXER_STU_STOP,
    AMIXER_STU_STOPPING,
    AMIXER_STU_RUNNING,
}amixer_stu;

typedef enum {
    AMIXER_MSG_NONE = 0,
    AMIXER_MSG_NOT_READY,
    AMIXER_MSG_RE_READY,
}amixer_msg;

typedef enum {
    AMIXER_WRX_CH_MIX_MODE_MONO = 0,
    AMIXER_WRX_CH_MIX_MODE_STEREO,
    AMIXER_WRX_CH_MIX_MODE_SAFE,
}amixer_dual_ch_mix_mode;

void* createAmixer(uint32_t paras);
void* create_usb_headphone_Amixer(uint32_t paras);
void  destroyAmixer(void* handle);
int   amixer_xet_ctrl(uint32_t cmd, uint32_t val, uint32_t rev);
void amixer_wrx_ch_mix_mode_set(uint8_t mode);
void amixer_mute_config(void);
void amixer_audio_rec_input_sel_set(uint8_t sel);
void amixer_audio_linein_mode_input_sel_set(uint8_t sel);
uint8_t music_eng_lvl_get();
uint8_t mic_eng_lvl_get(uint8_t index);

void amixer_close_mic();
void amixer_open_mic();
void amixer_close_linein();
void amixer_open_linein();
void vss_enable_revb_set(bool revb_enable);
void amixer_dac_switch(bool open);
void amixer_mode_lpc_off(void* a);
void amixer_mode_lpc_en(void *a);
#endif
