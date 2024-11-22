#ifndef _AEFFECT_DRC_H_
#define _AEFFECT_DRC_H_

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
} drc_subfunc_setting;


typedef struct {
    int8_t makeup_gain;
    uint8_t channel_gain;
    bool onChange;
} drc_comm_param_setting;

typedef struct {
    uint8_t enable;
    bool onChange;
} drc_enable_setting;


typedef enum {
    DRC_MASK_LIMITER    = 1,
    DRC_MASK_COMPRESSOR = 2,
    DRC_MASK_EXPANDER   = 4,
    DRC_MASK_NOISEGATE  = 8,
}DRC_SUBFUNC;

//混响参数调节区域
typedef struct {
    drc_subfunc_setting limiter_setting;
    drc_subfunc_setting compress_setting;
    drc_subfunc_setting expander_setting;
    drc_subfunc_setting noisegate_setting;
    drc_comm_param_setting comm_param_setting;
    drc_enable_setting enable_setting;
} drc_tunningarea;

#endif