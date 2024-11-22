#ifndef _AEFFECT_HLDRC_H_
#define _AEFFECT_HLDRC_H_

#include "sltk_filter.h"

//DRC参数调节
typedef struct {
    /*
            start threshold
            -80 - 0db
      */
    int8_t   x_thd;
    /*
            subfunc ratio
            1 - 255
      */
    uint8_t  x_ratio;
    /*
            attack time
            N ms
      */
    uint16_t x_att;
    /*
            release time
            N ms
      */
    uint16_t x_rel;

    bool enable;
    bool onChange;
} hldrc_subfunc_setting;


typedef struct {
    int8_t makeup_gain;
    bool onChange;
} hldrc_comm_param_setting;

typedef struct {
    uint8_t enable;
    bool onChange;
} hldrc_enable_setting;

typedef struct {
    int32_t fc;   
    bool onChange;
} hlpass_fc_setting;


typedef enum {
    HLDRC_MASK_LIMITER    = 1,
    HLDRC_MASK_COMPRESSOR = 2,
    HLDRC_MASK_EXPANDER   = 4,
    HLDRC_MASK_NOISEGATE  = 8,
}HLDRC_SUBFUNC;

typedef enum {
    DRC_LOW_PASS        = 1,
    DRC_HIGH_PASS       = 2,
    DRC_DEFAULT_PASS    = 4,
}DRC_PASS_SUBFUNC;

typedef struct {
    hldrc_subfunc_setting limiter_setting;
    hldrc_subfunc_setting compress_setting;
    hldrc_subfunc_setting expander_setting;
    hldrc_subfunc_setting noisegate_setting;
    hldrc_comm_param_setting comm_param_setting;
}pass_drc_tunningarea;
//分频DRC参数调节区域
typedef struct {
    pass_drc_tunningarea *lowpass_drc_setting;
    pass_drc_tunningarea *highpass_drc_setting;
    hldrc_enable_setting enable_setting;
    hlpass_fc_setting fc_setting;
} hldrc_tunningarea;

#endif