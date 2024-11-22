#ifndef _HOWLING_NOTCH_H_
#define _HOWLING_NOTCH_H_

#include "sltk_preproc_common.h"
#include "sltk_howling_detect.h"

typedef struct 
{
    uint32_t fs;
    /* frame length */
    uint16_t L;
    /* filter number */
    int8_t maxNf;
}hn_initprm_t;



void* hn_open(hn_initprm_t* prm);
int hn_proc(void* handle, int16_t* input, int16_t* output);
int hn_cmd(void* handle, uint32_t cmd, uint32_t arg);
void hn_close(void* handle);


#endif