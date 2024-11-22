#ifndef _SLTK_ER_H_
#define _SLTK_ER_H_

#include "sys_types.h"
#include "sltk_postproc_common.h"


typedef float sltk_real_t;

/*
    early reflection(ER) is a room-simulation module for a natural perceptual
    The ER can be serially connected by late reverberation module (eg. fdnrev)
    to make a much more dense room image for the auditor . Also the ER module can
    be parallelly connected by late reververation module to give the auditor another
    perception of the room. The connection is up to the designer, There is no limits.
*/

/* PRESET TYPES */
typedef enum
{
    /* theatre */
    ER_PRESET_THEATRE,
    /* cathedral */
    ER_PRESET_CATHEDRAL,
    /* drum room */
    ER_PRESET_DRUMROOM,
    /* stadium */
    ER_PRESET_STADIUM,
    /* bathroom */
    ER_PRESET_BATHROOM,
    /* large concert hall */
    ER_PRESET_LARGE_CONCERTHALL,
    /* large concert hall II */
    ER_PRESET_LARGE_CONCERTHALLII,
    /* medium concert hall */
    ER_PRESET_MEDIUM_CONCERTHALL,
    /* medium concert hall II */
    ER_PRESET_MEDIUM_CONCERTHALLII,
    /* plate reverb */
    ER_PRESET_MEDIUMPLATE,
    /* MilleniumVerb */
    ER_PRESET_MILLENIUMVERB,
    /* NewyorkPlate */
    ER_PRESET_NYPLATE,
    /* rock guitar room */
    ER_PRESET_GUITARROOM,
    /* vocal plate */
    ER_PRESET_VOCALPLATE,
    /* SnareTrap*/
    ER_PRESET_SNARETRAP,
    /* studio A */
    ER_PRESET_STUDIOA,
    /* studio B */
    ER_PRESET_STUDIOB,
    /* TightNYRoom */
    ER_PRESET_TIGHTNYROOM,
    /* VocalSpread */
    ER_PRESET_VOCALSPREAD,
    /* add other presets idx here */

    /* do not use this preset idx, it is just used for preset number */
    ER_PRESET_TOTALNUM,
}er_preset_t;


/* input type definition, if changed, the algorithms lib should be re-compiled */
typedef int32_t er_io_t;

/* ER initial prms */
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
}er_initprm_t;

/* ER real-time prms */
typedef struct
{
    /* preset mod */
    int8_t ref_mode;
    /* cross feed delay in ms*/
    sltk_real_t dlyX;
    /* dry(direct sound) gain */
    sltk_real_t g_dry;
    /* wet(er) gain */
    sltk_real_t g_wet;
    /* width 0~1 */
    sltk_real_t g_wetX;
}er_rtprm_t;


void* er_open(er_initprm_t* prm);

int er_process(void* handle, er_io_t* input, er_io_t* output, int32_t samples);

int er_cmd(void* handle, uint32_t cmd, uint32_t arg);

void er_close(void* handle);


#endif