#ifndef EQ14_TUNNING_CHANNEL_H
#define EQ14_TUNNING_CHANNEL_H

#include "atunning_common.h"
#include "aeffect_eq14.h"

typedef enum {
    EQ_PARA_START = 0,
    EQ_FILTER_TYPE = EQ_PARA_START,
    EQ_FC,
    EQ_GAIN,
    EQ_Q_FACTOR,
    EQ_DISABLE,
    EQ_PARA_END = EQ_DISABLE,
    EQ_SBPARA_NUM
}EQ_SUBBAND_PARAS;

typedef enum {
    EQ_PREAMP_MAIN_FUNC = 0,
    EQ_PREAMP_SUBFUNC_NUM
}EQ_PREAMP_SUBFUNC;

typedef enum {
    EQ_PREAMP_SBPARA_GAIN = 0,
    EQ_PREAMP_SBPARA_NUM
}EQ_PREAMP_SBPARA;

#define EQ_SB_ALL_PAPAS_MASK   ( (1 << EQ_SBPARA_NUM) - 1 )

eq_sec_setting *getNormalLeftChEqTunningArea(aeffect_eq_sec14_tunningarea* setting);
eq_sec_setting *getNormalRightChEqTunningArea(aeffect_eq_sec14_tunningarea* setting);
eq_preamp_setting *getNormalLeftChEqPreampTunningArea(aeffect_eq_sec14_tunningarea* setting);
eq_preamp_setting *getNormalRightChEqPreampTunningArea(aeffect_eq_sec14_tunningarea* setting);
void x_eq_subband_tunning_para(void *eq_addr, EQ_SUBBAND band, EQ_SUBBAND_PARAS para, void* val, bool set);
void x_eq_preamp_tunning_para(void *eq_amp, EQ_PREAMP_SUBFUNC subfunc, EQ_PREAMP_SBPARA para, void* val, bool set);

#endif