#ifndef _SLTK_FORMANT_H_
#define _SLKT_FORMANT_H_
#include "sys_types.h"
#include "sltk_filter.h"
#include "sltk_postproc_common.h"

typedef struct
{
    /* Qbits */
    int8_t Qbits;
    /* channel count */
    int8_t channels;
    /* hop size */
    int16_t L;
    /* sampling rate */
    int32_t fs;
    /* rec or plbk */
    int8_t  io;
    afx_hook_node_listener listener;
    void*    user;
}fc_initprm_t;

typedef struct
{
    /* formant chang ratio */
    float alpha;
    bool        enable;
}fc_rtprm_t;

void* fc_open(fc_initprm_t* prm);
void fc_close(void* handle);
int fc_proc(void* handle, int *in, int *out, int samples);
int fc_cmd(void* handle, uint32_t cmd, uint32_t arg);

typedef void* (*fc_lib_open)(fc_initprm_t* prm);
typedef void (*fc_lib_close)(void* handle);
typedef int (*fc_lib_proc)(void* handle, int *in, int *out, int samples);
typedef int (*fc_lib_cmd)(void* handle, uint32_t cmd, uint32_t arg);

typedef struct {
    fc_lib_open open;
    fc_lib_proc proc;
    fc_lib_cmd cmd;
    fc_lib_close close;
} fc_lib_t;

extern const fc_lib_t sltk_fc_lib;

#define VOID2FC(a) ((fc_lib_t*)(a))
#endif