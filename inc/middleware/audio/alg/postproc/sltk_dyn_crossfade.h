#ifndef _DYN_CROSSFADE_H_
#define _DYN_CROSSFADE_H_


#include "sys_types.h"
#include "sltk_postproc_common.h"

typedef enum
{
    /* linear-arthmetic based cross fade */
    DYNFADE_MODE_LINEAR = 0,
    /* log-arthmetic based cross fade */
    DYNFADE_MODE_LOG,
}dyn_crossfade_mode_t;

/*fade direction */
typedef enum
{
    /* first fadein, second fade out */
    DYNFADE_DIR_INOUT = 0,
    /* first fade out, second fade in */
    DYNFADE_DIR_OUTIN,
}dyn_crossfade_dir_t;


typedef struct
{
    /* Qbits */
    int8_t Qbits;
    /* channles 1 or 2 */
    int8_t chn;
    /* fade time in ms */
    uint16_t fade_time;
    /* sampling rate */
    int32_t fs;
    /* fade mode */
    dyn_crossfade_mode_t mode;
}dyn_crossfade_initprm_t;

typedef struct
{
    /* fade direction */
    dyn_crossfade_dir_t dir;
}dyn_crossfade_rtprm_t;


void* dyn_crossfade_open(dyn_crossfade_initprm_t* prm);

int dyn_crossfade_process(void* handle, int32_t* input1, int32_t* input2, int32_t* output, int samples);

int dyn_crossfade_cmd(void* handle, uint32_t cmd, uint32_t arg);

void dyn_crossfade_close(void* handle);

#endif
