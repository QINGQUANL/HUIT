#ifndef __SLTK_VSS_H__
#define __SLTK_VSS_H__

#include "sys_types.h"
#include "sltk_filter.h"
#include "sltk_postproc_common.h"

/* init prms for internal state allocation */
typedef struct {
    /* sampling rate, at now only support 44.1 and 48Khz, other
        sampling rate will be bypassed.(takes no effect)
    */
    int32_t sr;
    /* channels for sd, at now only support stereo(2channels )
        if set as mono input, then the sd processing will be bypassed
    */
    int8_t channels;
    /* frac bits */
    int8_t Qbits;
    /* mode select, 0: speaker, 1: headphone */
    int8_t mode;
    afx_hook_node_listener listener;
    void*    user;
    uint8_t use_rev;
} sdpro_initprm_t;

/* real time prms for headphone mode */
typedef struct {
    /* synthesis width 0~7 */
    int8_t synth;
    /* depth of space, 0~10 */
    int8_t depth;
    /* spatial intensity, 0~10 */
    int8_t sp_intensity;
    /* bass intensity, 0~10 */
    int8_t bass_intensity;
    /* definition, 0~10 */
    int8_t definition;
    /* bass crossoverfreq */
    int16_t bass_cofreq;
    /* headphone equalizer prms */
} sdpro_hprtprm_t;

/* real time prms for loudspeaker mode */
typedef struct {
    /* synthesis width, 0~7, 0: no synthesis*/
    int8_t synth;
    /* span width, 0~7 */
    int8_t span;
    /* spatial intensity, 0~10 */
    int8_t sp_intensity;
    /* bass intensity, 0~10 */
    int8_t bass_intensity;
    /* definition, 0~10 */
    int8_t definition;
    /* bass crossoverfreq */
    int16_t bass_cofreq;
} sdpro_spkrtprm_t;


typedef struct{
    sdpro_hprtprm_t  hp;
    sdpro_spkrtprm_t spk;
} sdpro_rtprm_internal_t;

typedef struct {
    float base;
    float step;
    int8_t on_off;
    int8_t mode_id;
    sdpro_rtprm_internal_t sd_rt;
}sdpro_rtprm_t;

void *sdpro_open(sdpro_initprm_t * prm);

int sdpro_proc(void *, int *input, int *output, int samples);

int sdpro_cmd(void *handke, uint32_t cmd, uint32_t arg);

void sdpro_close(void *handle);

void vss_enable_revb_set(bool revb_enable);

typedef void* (*sdpro_lib_open)(sdpro_initprm_t * prm);
typedef int (*sdpro_lib_proc)(void *, int *input, int *output, int samples);
typedef int (*sdpro_lib_cmd)(void *handke, uint32_t cmd, uint32_t arg);
typedef void (*sdpro_lib_close)(void *handle);

typedef struct {
    sdpro_lib_open open;
    sdpro_lib_proc proc;
    sdpro_lib_cmd cmd;
    sdpro_lib_close close;
} sdpro_lib_t;

extern const sdpro_lib_t sltk_sdpro_lib;

#define VOID2SDPRO(a) ((sdpro_lib_t*)(a))


#endif
