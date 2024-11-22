#ifndef ID_WORKER_H
#define ID_WORKER_H

#include "sys_inc.h"
#include "sltk_mem.h"

typedef enum {
    SRATE_QUALITY_DEFAULT = 0,
    SRATE_QUALITY_PIECEWISE = SRATE_QUALITY_DEFAULT,
    SRATE_QUALITY_CUBIC_LVL_0,
    SRATE_QUALITY_CUBIC_LVL_1,
    SRATE_QUALITY_NUM,
}srate_type;

typedef struct {
    uint8_t  ch;
    uint8_t  sample_size;
    uint8_t  level;
    uint8_t  period;
    float    alpha;
}id_init;

void* id_worker_init(id_init* init);
void  id_worker_deinit(void* handle);
int   id_worker_get_period(void* handle, void **in, int32_t* insamples, void ** out, int32_t* outsamples);
int id_worker_interp_get_period(void* handle, void **in, int32_t* insamples, void ** out, int32_t* outsamples,
    float alpha);

#endif