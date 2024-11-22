#ifndef AEFFECT_EQ14_H
#define AEFFECT_EQ14_H

#include "sltk_filter.h"
#include "sltk_eq.h"

typedef enum {
    EQ_BAND_1 = 0,
    EQ_BAND_2,
    EQ_BAND_3,
    EQ_BAND_4,
    EQ_BAND_5,
    MAX_EQ5_BANDS,
    EQ_BAND_6 = MAX_EQ5_BANDS,
    EQ_BAND_7,
    EQ_BAND_8,
    EQ_BAND_9,
    EQ_BAND_10,
    EQ_BAND_11,
    EQ_BAND_12,
    EQ_BAND_13,
    EQ_BAND_14,
    MAX_EQ_BANDS,
    EQ_SUBBAND_NUM = MAX_EQ_BANDS
}EQ_SUBBAND;

typedef enum {
    LEFT_CHANNEL = 0,
    RIGHT_CHANNEL,
    MAX_EQ_CHANNELS
}EQ_CHANNEL_TYPE;

typedef struct
{
    biquad_setting_t   bs;
    bool               disable;
    bool               onChange;
}eq_sec_setting;

typedef struct
{
    float              preamp;
    bool               onChange;
}eq_preamp_setting;

typedef struct
{
    eq_sec_setting     eq_bands[MAX_EQ_CHANNELS][MAX_EQ_BANDS];
    eq_preamp_setting  eq_preamp[MAX_EQ_CHANNELS];
}aeffect_eq_sec14_tunningarea;

#endif