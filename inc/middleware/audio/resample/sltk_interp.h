#ifndef _SLTK_INTERP_H_
#define _SLTK_INTERP_H_

#include "sys_inc.h"
#include "sltk_mem.h"

#define MAX_INTERP_CNT1     (18)
#define MAX_INTERP_CNT2      (1)

#define MAX_INTERP_CNT      (MAX_INTERP_CNT1 + MAX_INTERP_CNT2)

typedef struct {
    uint8_t  ch;
    uint8_t  sample_size;
    int32_t  period;
} sltk_interp_init;

void *sltk_interp_open(sltk_interp_init *init);
void sltk_interp_close(void *handle);
void sltk_interp_proc(void* handle, void **in, int32_t* insamples,
    void ** out, int32_t* outsamples, float alpha);
void sltk_interp_get_histdata(void *handle, float *data, int32_t cnt);

void *sltk_interp_open2(sltk_interp_init *init);
void sltk_interp_close2(void *handle);
void sltk_interp_proc2(void* handle, void **in, int32_t* insamples,
    void ** out, int32_t* outsamples, float alpha);

#endif