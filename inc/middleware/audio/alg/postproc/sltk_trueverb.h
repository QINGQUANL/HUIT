#ifndef _SLTK_TRUEVERB_H_
#define _SLTK_TRUEVERB_H_

#include "sys_types.h"
#include "sltk_postproc_common.h"
#include "sltk_fdnrev.h"
#include "sltk_er.h"

typedef int32_t tb_io_t;

/* 该trueverb preset types 为默认开扩散开关，单声道，外部混干声 */
/* trueverb PRESET TYPES */
typedef enum
{
    /* theatre */
    TVB_PRESET_THEATRE,
    /* cathedral */
    TVB_PRESET_CATHEDRAL,
    /* drum room */
    TVB_PRESET_DRUMROOM,
    /* stadium */
    TVB_PRESET_STADIUM,
    /* bathroom */
    TVB_PRESET_BATHROOM,
    /* large concert hall */
    TVB_PRESET_LARGE_CONCERTHALL,
    /* large concert hall II */
    TVB_PRESET_LARGE_CONCERTHALLII,
    /* medium concert hall */
    TVB_PRESET_MEDIUM_CONCERTHALL,
    /* medium concert hall II */
    TVB_PRESET_MEDIUM_CONCERTHALLII,
    /* plate reverb */
    TVB_PRESET_MEDIUMPLATE,
    /* MilleniumVerb */
    TVB_PRESET_MILLENIUMVERB,
    /* NewyorkPlate */
    TVB_PRESET_NYPLATE,
    /* rock guitar room */
    TVB_PRESET_GUITARROOM,
    /* vocal plate */
    TVB_PRESET_VOCALPLATE,
    /* SnareTrap*/
    TVB_PRESET_SNARETRAP,
    /* studio A */
    TVB_PRESET_STUDIOA,
    /* studio B */
    TVB_PRESET_STUDIOB,
    /* TightNYRoom */
    TVB_PRESET_TIGHTNYROOM,
    /* VocalSpread */
    TVB_PRESET_VOCALSPREAD,
    /* add other presets idx here */

    /* do not use this preset idx, it is just used for preset number */
    TVB_PRESET_TOTALNUM,
}trueverb_preset_t;

/* init prms */
typedef struct
{
    /* frac bits for input */
    int8_t Qbits;
    /* channel count */
    int8_t channels;
    /* max delay in ms */
    int16_t max_dly;
    /* max cross feed delay in ms */
    int16_t max_dlyX;
    /* sampling frequency */
    int32_t fs;
    /* predly enable */
    int8_t predly_en;
    /* semi fs ? */
    bool semi_fs;
    /* input diff ? */
    bool input_diff;
    /* rec or plbk */
    uint8_t io;
    afx_hook_node_listener listener;
    void* user;
}trueverb_initprm_t;

/* real-time prms */
typedef struct
{
    /* early-reflection real-time prm */
    er_rtprm_t  erprm;
    /* reverb tail real-time prm*/
    fdnrev_rtprm_t tailprm;
}trueverb_rtprm_t;


void trueverb_close(void* handle);

void* trueverb_open(trueverb_initprm_t* prm);

int trueverb_process(void* handle, tb_io_t* input, tb_io_t* output, int32_t samples);

int trueverb_cmd(void* handle, uint32_t cmd, uint32_t arg);

typedef void* (*trueverb_lib_open)(trueverb_initprm_t * prm);
typedef int   (*trueverb_lib_proc)(void *, tb_io_t *input, tb_io_t *output, int32_t samples);
typedef int   (*trueverb_lib_cmd)(void *handle, uint32_t cmd, uint32_t arg);
typedef void  (*trueverb_lib_close)(void *handle);

typedef struct {
    trueverb_lib_open  open;
    trueverb_lib_proc  proc;
    trueverb_lib_cmd   cmd;
    trueverb_lib_close close;
} trueverb_lib_t;

extern const trueverb_lib_t sltk_trueverb_lib;

#define VOID2TRUEVERB(a) ((trueverb_lib_t*)(a))
#endif