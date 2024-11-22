#ifndef _AEFFECT_ECHO_H_
#define _AEFFECT_ECHO_H_

#include "sltk_filter.h"

typedef struct {
    /* feedback coefficient, 0<g<0.85 */
    float gFbc; //回声重复率
    /* gain of direct arrival sound */
    float gDry;     //回声直达音量
    /* gain of echo in the final output */
    float gWet;     //回声混响音量
    /* dly ms : full band 0 - 200ms, narrow band 0 - 300ms */
    uint16_t dly;   //回声延时
    /* need lp for fbdata */
    uint16_t need_lp;       //低通滤波
    /* lp cut-off freq */
    uint16_t cutoff_freq;   //低通频率
    /* enable */
    bool enable;
    bool onChange;
} echo_setting_t;

//混响参数调节区域
typedef struct {
    echo_setting_t setting;
} echo_tunningarea;

#endif