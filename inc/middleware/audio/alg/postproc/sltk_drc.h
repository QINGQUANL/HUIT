#ifndef _SLTK_DRC_H_
#define _SLTK_DRC_H_

#include "sys_types.h"
#include "sltk_filter.h"
#include "sltk_postproc_common.h"

typedef int32_t drc_sample_t;

#define MASK_LIMITER    1
#define MASK_COMPRESSOR 2
#define MASK_EXPANDER   4
#define MASK_NOISEGATE  8

/*
prms:
x:          [in] input signal
fs:         [in] sampling rate in Hz
ch:         [in] channles num of input
drcprms:    [in] parameter of drc
.lt_thd:    [in] limiter threshold in dBFS
.lt_ratio:  [in] limiter ratio
.cp_thd:    [in] compress threshold in dBFS
.cp_ratio:  [in] compress ratio
.exp_thd:   [in] expander threshold in dBFS
.exp_ratio: [in] expander ratio
.ng_thd:    [in] noise gate threshold in dBFS
.ng_ratio:  [in] noise gate ratio
.ng_hold:   [in] noise gate hold time in ms
.peak_att:  [in] peak detector attack time constant in ms
.peak_rel:  [in] peak detector release time constant in ms
.rms_avt:   [in] root of rms detector attack time constant in ms
.gain_att:  [in] gain smooth attack time constant
.gain_rel:  [in] gain smooth release time constant
.look_ahead:[in] delay length in ms
.makeup_gain[in] make up gain
.stlink:    [in] stereo link mode: 0: choose lowest gain 1: mix as mono, 2: individual and chose the smaller gain.
.knee_width [in] knee_width
.**_lv      [in] level mode, 0: peak, 1: rms
*/
typedef struct {
    uint8_t func_mask;
    uint8_t lev_mode;
    //uint8_t st_link;
    uint8_t look_ahead;
    /* */
    uint8_t lt_ratio;
    /* threshold of limiter Q3 */
    int8_t lt_thd;
    uint8_t lt_kw;
    uint8_t cp_ratio;
    int8_t cp_thd;
    uint8_t cp_kw;
    uint8_t exp_ratio;
    int8_t exp_thd;
    uint8_t exp_kw;
    uint8_t ng_ratio;
    int8_t ng_thd;
    uint16_t ng_hold;
    uint8_t ng_kw;
    uint16_t peak_att;
    uint16_t peak_rel;
    uint16_t rms_avt;
    uint16_t lt_att;
    uint16_t lt_rel;
    uint16_t cp_att;
    uint16_t cp_rel;
    uint16_t exp_att;
    uint16_t exp_rel;
    uint16_t ng_att;
    uint16_t ng_rel;
    int8_t makeup_gain;
} drc_rtprm_t;

typedef struct {

    uint8_t ch_num;
    int8_t Qbits;
    uint16_t sampling_rate;

} drc_prms_t;

void *drc_init(drc_prms_t * prms);
/*
    Notes: Len must be times of 8
*/
int drc_process(void *handle, drc_sample_t * input, drc_sample_t * output,
                int32_t Len);
int drc_cmd(void *handle, uint32_t cmd, uint32_t arg);
int drc_cmd_ex(void *handle, uint32_t cmd, uint32_t arg);
void drc_close(void *handle);

typedef void* (*drc_lib_open)(drc_prms_t* prm);
typedef int   (*drc_lib_process)(void *handle, drc_sample_t * x, drc_sample_t * y, int32_t samples);
typedef int   (*drc_lib_cmd)(void *handle, uint32_t cmd, uint32_t arg);
typedef void  (*drc_lib_close)(void* handle);

typedef struct {
    drc_lib_open     open;
    drc_lib_process  proc;
    drc_lib_cmd      cmd;
    drc_lib_close    close;
} drc_lib_t;

extern const void* drc_handle;

#define VOID2DRC(a) ((drc_lib_t*)(a))

#endif