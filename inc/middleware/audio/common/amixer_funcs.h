#ifndef AMIXER_FUNCS_H
#define AMIXER_FUNCS_H
#include "sys_types.h"

#define GET_REFORMAT_TYPE(src_ch, src_sample_sz, dst_ch, dst_sample_sz) \
    ((((src_ch) <= 2) && ((dst_ch) <= 2)) ? (((src_ch)<<0) | ((src_sample_sz)<<8) | ((dst_ch)<<16) | ((dst_sample_sz)<<24)) : REFMT_NONE)

#define GET_MIXER_TYPE GET_REFORMAT_TYPE

#define GET_SRC_CH(type)            (((type) >>  0) & 0xFF)
#define GET_SRC_SAMPLES_SIZE(type)  (((type) >>  8) & 0xFF)
#define GET_DST_CH(type)            (((type) >> 16) & 0xFF)
#define GET_DST_SAMPLES_SIZE(type)  (((type) >> 24) & 0xFF)

enum pcm_reformat_e {
    REFMT_NONE,

    REFMT_MONO16_TO_MONO16      = GET_REFORMAT_TYPE(1, 2, 1, 2),
    REFMT_MONO16_TO_MONO32      = GET_REFORMAT_TYPE(1, 2, 1, 4),
    REFMT_MONO16_TO_STEREO16    = GET_REFORMAT_TYPE(1, 2, 2, 2),
    REFMT_MONO16_TO_STEREO32    = GET_REFORMAT_TYPE(1, 2, 2, 4),

    REFMT_MONO32_TO_MONO16      = GET_REFORMAT_TYPE(1, 4, 1, 2),
    REFMT_MONO32_TO_STEREO16    = GET_REFORMAT_TYPE(1, 4, 2, 2),
    REFMT_MONO32_TO_MONO32      = GET_REFORMAT_TYPE(1, 4, 1, 4),
    REFMT_MONO32_TO_STEREO32    = GET_REFORMAT_TYPE(1, 4, 2, 4),

    REFMT_STEREO16_TO_MONO16    = GET_REFORMAT_TYPE(2, 2, 1, 2),
    REFMT_STEREO16_TO_STEREO16  = GET_REFORMAT_TYPE(2, 2, 2, 2),
    REFMT_STEREO16_TO_MONO32    = GET_REFORMAT_TYPE(2, 2, 1, 4),
    REFMT_STEREO16_TO_STEREO32  = GET_REFORMAT_TYPE(2, 2, 2, 4),

    REFMT_STEREO32_TO_MONO16    = GET_REFORMAT_TYPE(2, 4, 1, 2),
    REFMT_STEREO32_TO_MONO32    = GET_REFORMAT_TYPE(2, 4, 1, 4),
    REFMT_STEREO32_TO_STEREO16  = GET_REFORMAT_TYPE(2, 4, 2, 2),
    REFMT_STEREO32_TO_STEREO32  = GET_REFORMAT_TYPE(2, 4, 2, 4),
};

enum pcm_mixer_e {
    MIXER_NONE,

    MIXER_STEREO16_TO_STEREO16  = GET_REFORMAT_TYPE(2, 2, 2, 2),
    MIXER_STEREO16_TO_STEREO32  = GET_REFORMAT_TYPE(2, 2, 2, 4),
    MIXER_STEREO32_TO_STEREO32  = GET_REFORMAT_TYPE(2, 4, 2, 4),
    MIXER_STEREO32_TO_STEREO16  = GET_REFORMAT_TYPE(2, 4, 2, 2),
    MIXER_STEREO32_TO_MONO16    = GET_REFORMAT_TYPE(2, 4, 1, 2),
};

void stereo32_to_mono32(int32_t *pout, const int32_t *pin, int32_t samples);
void stereo32_to_mono16(int16_t *pout, const int32_t *pin, int32_t samples);
void stereo32_to_stereo16(int16_t *pout, const int32_t *pin, int32_t samples);
void stereo16_to_mono32(int32_t *pout, const int16_t *pin, int32_t samples);
void stereo16_to_mono16(int16_t *pout, const int16_t *pin, int32_t samples);
void stereo16_to_stereo32(int32_t *pout, const int16_t *pin, int32_t samples);
void mono32_to_mono16(int16_t *pout, const int32_t *pin, int32_t samples);
void mono32_to_stereo16(int16_t *pout, const int32_t *pin, int32_t samples);
void mono32_to_stereo32(int32_t *pout, const int32_t *pin, int32_t samples);
void mono16_to_mono32(int32_t *pout, const int16_t *pin, int32_t samples);
void mono16_to_stereo16(int16_t *pout, const int16_t *pin, int32_t samples);
void mono16_to_stereo32(int32_t *pout, const int16_t *pin, int32_t samples);

void __16_to_interleave_stereo16(void *dst, const void **src, uint32_t samples);
void __32_to_interleave_stereo16(void *dst, const void **src, uint32_t samples);
void __32_to_interleave_stereo32(void *dst, const void **src, uint32_t samples);
void __16_to_interleave_stereo32(void *dst, const void **src, uint32_t samples);
void __16_to_interleave_mono16(void *dst, const void **src, uint32_t samples);
void __32_to_interleave_mono16(void *dst, const void **src, uint32_t samples);
void __32_to_interleave_mono32(void *dst, const void **src, uint32_t samples);
void __16_to_interleave_mono32(void *dst, const void **src, uint32_t samples);
void stereo32_interleave_notinterleave_cnv(int32_t *io, int32_t samples, int8_t dir);

typedef void (*reformat_func_t)(void *pout, const void *pin, int32_t samples);

typedef void (*pcm_reformat_hook_t)(void *dst, const void *src, uint32_t samples);
typedef void (*pcm_interleave_hook_t)(void *dst, const void **src, uint32_t samples);
typedef void (*pcm_mixer_hook_t)(void* mix0, void* mix1, int32_t samples);

typedef union {
    pcm_reformat_hook_t   interleave2interleave_fmt;
    pcm_interleave_hook_t noninterleave2interleave_fmt;
    void*                 func;
}pcm_refmt_hook;

pcm_reformat_hook_t reformat_to_hook(uint32_t reformat);
pcm_interleave_hook_t reformat_to_interleave_hook(uint32_t reformat);
pcm_mixer_hook_t mixer_get_hook(uint32_t mixer);

#endif