#ifndef _SLTK_FDNREVERB_H_
#define _SLTK_FDNREVERB_H_

#include "sys_types.h"
#include "sltk_filter.h"
#include "sltk_postproc_common.h"

typedef struct {
    /* rt60 in ms */
    int16_t revb_time;
    /* damping freq  for low freq in Hz */
    uint16_t damp_freq_lo;
    /* damping freq for high freq in Hz */
    uint16_t damp_freq_hi;
    /* output equalizer freq (lowpass) */
    uint16_t lpf_freq;
    /* output equalizer freq (high pass) */
    uint16_t hpf_freq;
    /* predelay in ms*/
    uint16_t pre_delay;
    /* diffusion */
    float diff;
    /* damping factor for low freq */
    float damp_ratio_lo;
    /* damping factor for high freq */
    float damp_ratio_hi;
    /* rev gain */
    float g_wet;
    /* dry gain */
    float g_dry;
    /* input diff flag */
    int8_t input_diff;
    /* programmable channel num */
    int8_t rt_ch;
    int8_t Qbits;
    bool   buffer_install;
} fdnrev_rtprm_t;

typedef struct {
    /* sampling frequency */
    int32_t sr;
    /* channels 1 or 2 */
    int8_t channels;
    /* Qbits of input/output data */
    int8_t Qbits;
    /* predly enable */
    int8_t predly_en;
    /* semi fs ? */
    bool semi_fs;
    /* input diff ? */
    bool input_diff;
    /* maxdelay of er in ms */
    uint16_t maxdly;
    /* rec or plbk*/
    uint8_t  io;
    afx_hook_node_listener listener;
    void*    user;
} fdnrev_prm_t;

void *fdnrev_open(fdnrev_prm_t * prm);
int fdnrev_proc(void *, int *input, int *output, int samples);
int fdnrev_cmd(void *handke, uint32_t cmd, uint32_t arg);
void fdnrev_close(void *handle);

typedef void* (*fdnrev_lib_open)(fdnrev_prm_t * prm);
typedef int   (*fdnrev_lib_proc)(void *, int *input, int *output, int samples);
typedef int   (*fdnrev_lib_cmd)(void *handle, uint32_t cmd, uint32_t arg);
typedef void  (*fdnrev_lib_close)(void *handle);

typedef struct {
    fdnrev_lib_open  open;
    fdnrev_lib_proc  proc;
    fdnrev_lib_cmd   cmd;
    fdnrev_lib_close close;
} fdnrev_lib_t;

extern const fdnrev_lib_t sltk_fdnrev_lib;

#define VOID2FDNREV(a) ((fdnrev_lib_t*)(a))
#endif
