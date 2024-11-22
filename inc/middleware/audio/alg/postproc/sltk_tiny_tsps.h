#ifndef SLTK_TINY_TSPS_H
#define SLTK_TINY_TSPS_H

#ifndef WIN32
#include "sys_types.h"
#endif
#include "sltk_postproc_common.h"

#define AUDIO_PLBK_TSPS_PWOLA_LEN    280

typedef struct {
    int8_t Qbits;
    int8_t channels;
    int16_t L;
    int16_t Li;
    int32_t fs;
    /* rec or plbk*/
} tiny_tsps_prm_t;

typedef struct {
    float alpha_ts;
    float alpha_ps;
} tiny_tsps_rtprm_t;

void  tiny_tsps_close(void *handle);
void* tiny_tsps_open(tiny_tsps_prm_t * prm);
int   tiny_tsps_blockproc(void *handle, void **in, int *inL, void **out, int *outL);
int   tiny_tsps_cmd(void *handle, uint32_t cmd, uint32_t arg);

#endif