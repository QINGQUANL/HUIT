#ifndef _AEFFECT_VSS_H_
#define _AEFFECT_VSS_H_

#include "sltk_filter.h"
#include "sltk_eq.h"
#include "aeffect_eq14.h"

typedef enum {
    VSS_MODE_DEFAULT = 0,
    VSS_MODE_SPEAKER = VSS_MODE_DEFAULT,
    VSS_MODE_HEADPHONE,
    VSS_MODE_NUM
} vss_mode;

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
    /*
            enable switch
      */
    uint8_t  enable;

    bool onChange;
} vss_drc_subfunc_setting;

typedef union {
    int8_t hp_depth;
    int8_t spk_span;
    int8_t thiz;
} vss_modes_sound_field_alias;

typedef struct {
    /* synthesis width 0~7 */
    int8_t synth;
    /* depth of space, 0~10 */
    vss_modes_sound_field_alias sound_field_width;
    /* spatial intensity, 0~10 */
    int8_t sp_intensity;
    /* bass intensity, 0~10 */
    int8_t bass_intensity;
    /* definition, 0~10 */
    int8_t definition;
    /* bass crossoverfreq */
    int16_t bass_cofreq;

    bool onChange;
} vss_pro_setting;

//3D音效模式
typedef struct {
    int8_t mode; //参考@vss_mode
    bool onChange;
} vss_mode_setting;

//3D音效开关
typedef struct {
    int8_t enable; //0:关闭; 1-打开
    bool onChange;
} vss_enable_setting;


//虚拟环绕声参数调节区域
typedef struct {
    eq_sec_setting     eq5[MAX_EQ5_BANDS];
    vss_drc_subfunc_setting limiter;
    vss_pro_setting     hp_and_spk[VSS_MODE_NUM];
    vss_mode_setting    mode_setting;
    vss_enable_setting enable_setting;
} vss_tunningarea;

#endif