#ifndef __SLTK_MSDRC_H__
#define __SLTK_MSDRC_H__

#include "sltk_aenc.h"


typedef struct
{
    /* Qbits */
    int8_t Qbits;
    /* channles 1 or 2 */
    int8_t chn;
    /* decimator ratio, <= 4*/
    int8_t decN;
    /* section number, > 2 */
    int8_t sectnum;
    /* sampling frequency */
    int32_t fs;
}msdrc_initprm_t;

typedef struct
{
    /* compress/expander ratio */
    float* ratio;
    /* threshold array */
    float* thd;
    /* attack array */
    float* att;
    /* release array */
    float* rel;
    /* kneed width array */
    float* kw;
    /* gain base of the first section */
    float gbase;
}msdrc_rtprm_t;

void* msdrc_open(msdrc_initprm_t* prm);
int msdrc_process(void* handle, int32_t* input, int32_t* output, int samples);
int msdrc_cmd(void* handle, uint32_t cmd, uint32_t arg);
void msdrc_close(void* handle);

/* proc single sample and return gain */
float msdrc_realsample_gain(void* handle, float x);



#endif