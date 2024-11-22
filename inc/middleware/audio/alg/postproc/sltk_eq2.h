#ifndef __SLTK_EQ2_H__
#define __SLTK_EQ2_H__

#include "sys_types.h"
#include "sltk_filter.h"
#include "sltk_postproc_common.h"

/*
    eq2 is a newly designed equalizer, the difference between eq2 and eq is
    1. eq2 can config channels and bands for internal RAM occupy and CPU loading
    2. becasuse of the variable of bands and channels, we can not set all of rtprm once
    3. eq2 rearranged data to channel-based, and will with different optimisation method in assembly

*/

typedef struct {
    /*
       saturation  flag, if the eq is followed by limiter(like the one embedded in DRC),
       then should be set to 0 to maintain the dynamic range as much as possible
     */
    int8_t need_saturation;
    /*
       frac bits for input and output sample, Qbits == 15 for short input, 15 < Qbits < 32 for 32bit(int) input
       the eq process will keep the same Qbits in the output, if the eq will boost the input, it is strongly suggested
       that use a headroom for the internal dynamic range, that is use Qbits <= 24 for
     */
    int8_t Qbits;
    /* channel count */
    int8_t channels;
    /* bands count */
    int8_t bands;
    /* sampling frequency */
    int32_t fs;
} eq2_initprm_t;


typedef struct {
    /* 0 for left channel, 1 for right channel */
    int8_t ch;
    /* band idx, 0 to MAX_EQ_BANDS -1 */
    int8_t band;
    /* filter settings */
    biquad_setting_t setting;
    /* disable */
    uint8_t disable;
} eq2_sectprm_t;

typedef struct {
    /* 0 for left channel, 1 for right channel */
    int8_t ch;
    /* */
    float PreAmp;
} eq2_pampprm_t;

typedef struct
{
    float* coeffs[2];
    int32_t coeff_items[2];
}eq2_batchcoeff_t;

void *eq2_open(eq2_initprm_t * prm);
int eq2_process(void *handle, void * x, void * y, int32_t samples);
int eq2_cmd(void *handle, uint32_t cmd, uint32_t arg);
void eq2_close(void *handle);

typedef void* (*eq2_lib_open)(eq2_initprm_t* prm);
typedef int   (*eq2_lib_process)(void *handle, void * x, void * y, int32_t samples);
typedef int   (*eq2_lib_cmd)(void *handle, uint32_t cmd, uint32_t arg);
typedef void  (*eq2_lib_close)(void* handle);

typedef struct {
    eq2_lib_open     open;
    eq2_lib_process  proc;
    eq2_lib_cmd      cmd;
    eq2_lib_close    close;
} eq2_lib_t;

extern const void* eq2_handle;

#define VOID2EQ2(a) ((eq2_lib_t*)(a))

#endif