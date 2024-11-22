#ifndef _AEFFECT_FDNREV_H_
#define _AEFFECT_FDNREV_H_

#include "sltk_filter.h"

//混响参数调节
typedef struct {
    /* 混响时间(rt60 in ms) */
    int16_t revb_time;
    /* 低频阻尼频点(damping freq  for low freq in Hz) */
    uint16_t damp_freq_lo;
    /* 高频阻尼频点(damping freq for high freq in Hz) */
    uint16_t damp_freq_hi;
    /* 低通滤波截止频率(output equalizer freq (lowpass)) */
    uint16_t lpf_freq;
    /* 高通滤波截止频率(output equalizer freq (high pass)) */
    uint16_t hpf_freq;
    /* 混响预延时(predelay in ms) */
    uint16_t pre_delay;
    /* 扩散系数(diffusion) */
    float diff;
    /* 低频阻尼(damping factor for low freq) */
    float damp_ratio_lo;
    /* 高频阻尼(damping factor for high freq) */
    float damp_ratio_hi;
    /* 混响(湿声)音量(rev gain) */
    float g_wet;
    /* 直达(干声)音量(dry gain) */
    float g_dry;

    /* 单双通道模式切换((0-单声道, 1-立体声)) */
    int8_t m2s;
    /* 输入扩散开关(0-关, 1-开) */
    uint8_t input_diff;
    bool onChange;
} fdnrev_pro_setting;

typedef struct {
    uint8_t enable;
    bool onChange;
    bool semi_fs;
    uint16_t fs;
} fdnrev_enable_setting;


//混响参数调节区域
typedef struct {
    fdnrev_pro_setting setting;
    fdnrev_enable_setting enable_setting;
} fdnrev_tunningarea;

#endif