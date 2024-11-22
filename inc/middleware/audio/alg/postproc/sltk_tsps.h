#ifndef _SLTK_TSPS_H_
#define _SLTK_TSPS_H_

#include "sys_types.h"
#include "sltk_filter.h"
#include "sltk_postproc_common.h"

typedef struct {
    int8_t Qbits;
    int8_t channels;
    /* auto tune flag */
    int8_t atune_flag;
    int16_t L;
    int16_t Li;
    int32_t fs;
    // float alpha_ts;
    // float alpha_ps;
    /* rec or plbk*/
    int8_t io;
    afx_hook_node_listener listener;
    void*    user;
} tsps_prm_t;

typedef enum {
    UP_CORRECT = 0,
    DOMW_CORRECT
} LA_CORRECT_MODE;

typedef struct {
    float alpha_ts;
    float alpha_ps;
    int8_t buffer_install;
} tsps_rtprm_t;

typedef struct {
    /* Notes base */
    int8_t note;
    /* octave idx */
    int8_t oct;
    /* scale style */
    int8_t scale;
    /* atune speed, 1~10 */
    int8_t speed;
} atune_rtprm_t;


void tsps_close(void *handle);
void *tsps_open(tsps_prm_t * prm);
int tsps_blockproc(void *handle, int32_t *inb, int32_t *outb, int samples);
int tsps_cmd(void *handle, uint32_t cmd, uint32_t arg);

typedef void  (*tsps_lib_close)(void *handle);
typedef void* (*tsps_lib_open)(tsps_prm_t * prm);
typedef int   (*tsps_lib_proc)(void *handle, int32_t *inb, int32_t *outb, int samples);
typedef int   (*tsps_lib_cmd)(void *handle, uint32_t cmd, uint32_t arg);

typedef struct {
    tsps_lib_open  open;
    tsps_lib_proc  proc;
    tsps_lib_cmd   cmd;
    tsps_lib_close close;
} tsps_lib_t;

extern const tsps_lib_t sltk_tsps_lib;

#define VOID2TSPS(a) ((tsps_lib_t*)(a))

#endif