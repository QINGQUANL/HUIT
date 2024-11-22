#ifndef _AEFFECT_FREQSHIRT_H_
#define _AEFFECT_FREQSHIRT_H_

#include "sltk_filter.h"

//移频参数调节
typedef struct {
    uint8_t  deltafreq;     //移频量：0~10Hz

    bool onChange;
} freqshift_subfunc_setting;


//移频参数调节区域
typedef struct {
    freqshift_subfunc_setting freqshift_setting;
} freqshift_tunningarea;

#endif

