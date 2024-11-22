#ifndef _VOCALENHANCE_H_
#define _VOCALENHANCE_H_

#include "sys_types.h"


typedef struct
{
    /* sampling frequency */
    int32_t fs;
    /* block length */
    int16_t L;
    /* channel number 1 or 2 */
    int8_t chnum;
}ve_initprm_t;

/* style macros */
/* crisp vocal */
#define VE_STYLE_CRISP  0
/* female vocal */
#define VE_STYLE_FEMALE 1
/* lead vocal */
#define VE_STYLE_LEAD   2
/* male vocal */
#define VE_STYLE_MALE   3
/* strong enhancer */
#define VE_STYLE_STRONG 4

typedef struct 
{
    /* brightness 0-9 */
    int8_t brightness;
    /* style */
    int8_t style;
    /* richness 0-9*/
    int8_t richness;
    /* gain 0 - 9 */
    float gain;
}ve_rtprm_t;


void* vocalenhance_open(ve_initprm_t* prm);

int vocalenhance_proc(void* handle, int16_t* input, int16_t* output);

int vocalenhance_cmd(void* handle, uint32_t cmd, uint32_t arg);

void vocalenhance_close(void* handle);




#endif