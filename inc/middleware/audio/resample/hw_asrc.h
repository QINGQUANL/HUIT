#ifndef _HW_ASRC_H_
#define _HW_ASRC_H_

#include "sys_types.h"
#include "sltk_codecs_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t in_fs;
    uint32_t out_fs;
    uint16_t insamples;
    uint16_t outsamples;

    uint8_t ch;
    uint8_t sample_size;
} hwasrc_initparams_t;

void *hwasrc_open(hwasrc_initparams_t *params);
void hwasrc_close(void *hwasrc_handle);
int hwasrc_blockproc(void *hwasrc_handle,
    int32_t **in, int32_t *in_samples,
    int32_t **out, int32_t *out_samples);

int32_t hwasrc_w_avail(void *hwasrc_handle);
int32_t hwasrc_write(void *hwasrc_handle, void *asrc_inbuf, int32_t asrc_samples);
int32_t hwasrc_r_avail(void *hwasrc_handle);
int32_t hwasrc_read(void *hwasrc_handle, void *asrc_outbuf, int32_t asrc_samples);
int32_t hwasrc_inblock_wait(void *hwasrc_handle);
int32_t hwasrc_outblock_wait(void *hwasrc_handle);

#ifdef __cplusplus
}
#endif

#endif
