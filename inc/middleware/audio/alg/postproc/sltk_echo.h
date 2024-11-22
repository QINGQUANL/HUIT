#ifndef _SLTK_ECHO_H_
#define _SLTK_ECHO_H_

#include "sys_types.h"
#include "sltk_filter.h"
#include "sltk_postproc_common.h"

/* real time prms, can be configured during processing */
typedef struct {
    /* need filter in the feedback path ? */
    uint16_t need_flt;
    /* feedback coefficient, -1<g<1 */
    float g;
    /*
        feedback filter damping frequency
    */
    uint16_t cutoff_freq;
    /* gain of direct arrival sound */
    float gDry;
    /* gain of echo in the final output */
    float gWet;
    /* dly prm */
    uint16_t dly;
    /* enable */
    uint8_t enable;
} echo_rtprm_t;

typedef struct {
    /* sampling rate */
    uint16_t sr;
    ///* delay time in ms */
    //uint16_t dly;
    /* channels */
    int8_t channels;
    /* Qbits */
    int8_t Qbits;
    /* max delay in ms */
    uint16_t maxdly;
    /* rec or plbk */
    int8_t  io;
    afx_hook_node_listener listener;
    void*    user;
    uint8_t semi_fs;
} echo_prm_t;

typedef int echo_input_t;

void *echo_open(echo_prm_t * prm);
int echo_blockproc(void *handle, echo_input_t * in, echo_input_t * out,
                   int samples);
int echo_cmd(void *handle, uint32_t cmd, uint32_t arg);
void echo_close(void *handle);

typedef void* (*echo_lib_open)(echo_prm_t * prm);
typedef int (*echo_lib_proc)(void *handle, echo_input_t * in, echo_input_t * out, int samples);
typedef int (*echo_lib_cmd)(void *handle, uint32_t cmd, uint32_t arg);
typedef void (*echo_lib_close)(void *handle);

typedef struct {
    echo_lib_open  open;
    echo_lib_proc  proc;
    echo_lib_cmd   cmd;
    echo_lib_close close;
} echo_lib_t;

extern const echo_lib_t sltk_echo_lib;

#define VOID2ECHO(a) ((echo_lib_t*)(a))
#endif