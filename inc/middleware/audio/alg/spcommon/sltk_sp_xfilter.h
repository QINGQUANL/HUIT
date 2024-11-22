#ifndef _SLTK_SP_XFILTER_H_
#define _SLTK_SP_XFILTER_H_

#include "sys_types.h"

typedef struct
{
    /* frac bits */
    int8_t Qbits;
    /* channel number */
    int8_t chn;
    /* block length, the more the much RAM will be occupied */
    int16_t blockLen;
    /* sampling frequency in Hz */
    int32_t fs;
    /* extern buf */
    int8_t extern_buf;
}xfilter_initprm_t;


/*
    2 bands crossover filter open
*/
void* xfilter2band_open(xfilter_initprm_t* prm );

/*
    2 bands crossover filter reset interface
*/
int xfilter2band_reset(void* handle);

/*
    2 bands crossover filter proc
*/

int xfilter2band_process(void* handle, int16_t* input, int16_t* outputL, int16_t* outputH, int32_t samples);
/*
    proc func for real io
*/
int xfilter2band_process_realchan(void* handle, float* input, float*outputL, float* outputH, int32_t samples, int ch);

/*
    block filter for external real_t buffer
*/
void xfilter2band_blockfilter(void* handle, int32_t samples);
/*
    cross over frequency setting
*/
int xfilter2band_setfc(void* handle, int32_t fc);

/*
    attach external buffer
*/
int xfilter2band_attach_buffer(void* handle, uint32_t* bufaddr);

/*
    2 bands crossover filter proc
*/
void xfilter2band_close(void* handle);




/*
    complementary filter destroy
*/
void xfilter2band_complement_close(void* handle);

/*
    3 bands crossover filter open
*/
void* xfilter3band_open(xfilter_initprm_t* prm);

/*
    3 bands crossover filter proc
*/
int xfilter3band_process(void* handle, int16_t* input, int16_t* outputL, int16_t* outputM, int16_t* outputH, int32_t samples);


/*
    block filter for external real buffer
*/
void xfilter3band_blockfilter(void* handle, int32_t samples);

/*
    cross over frequency setting, make sure fc1 < fc2
*/
int xfilter3band_setfc(void* handle, int32_t fc1, int32_t fc2);

/*
    attach external buffer
*/
int xfilter3band_attach_buffer(void* handle, uint32_t* bufaddr);

/*
    3 bands crossover filter proc
*/
void xfilter3band_close(void* handle);

/*
    reset api
*/
int xfilter3band_reset(void* handle);

#endif