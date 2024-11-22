#ifndef _AEFFECT_TSPS_H_
#define _AEFFECT_TSPS_H_

#include "sltk_filter.h"

typedef struct {
    //should be 2^(-1) - 2^(1)
    float alpha_ts;
    //should be 2^(-1) - 2^(1)
    float alpha_ps;
    bool  enable;
    bool onChange;
} tsps_setting_t;

typedef struct {
    /* Notes base */
    int8_t note;
    /* octave idx */
    int8_t oct;
    /* scale style */
    int8_t scale;
    /* atune speed, 1~10 */
    int8_t speed;
    bool  enable;
    bool onChange;
} atune_setting_t;

//formant配置
typedef struct {
    float alpha_fc;
    bool  enable;
    bool onChange;
} fc_setting_t;

//混响参数调节区域
typedef struct {
    tsps_setting_t tsps_setting;
    atune_setting_t atune_setting;
    fc_setting_t fc_setting;
} tsps_tunningarea;

#endif