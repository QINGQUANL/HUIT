
#ifndef _AUDIO_SHARE_BUFFER_H_
#define _AUDIO_SHARE_BUFFER_H_

#include "sys_types.h"
#include "amixer_funcs.h"
#include "ring_buffer.h"
#include "oal.h"

#ifdef __cplusplus
 extern "C" {
#endif


struct audio_share_buffer {
    struct ring_buffer ctrl;

    uint8_t store_ch;
    uint8_t store_sample_size;
    uint8_t input_ch;
    uint8_t input_sample_size;
    uint8_t input_interleave;
    pcm_refmt_hook write_refmt;
    pcm_refmt_hook read_refmt;
    OAL_SEM write_sem_sync;
    OAL_SEM read_sem_sync;
};

typedef struct audio_share_buffer_params {
    void *buf;
    uint32_t size;
    uint8_t store_ch;
    uint8_t store_sample_size;
    uint8_t input_ch;
    uint8_t input_sample_size;
    uint8_t input_interleave;
    pcm_refmt_hook write_refmt;
    pcm_refmt_hook read_refmt;
} audio_share_buffer_params_t;

int32_t audio_share_buffer_init(struct audio_share_buffer *sbuf,
    audio_share_buffer_params_t *params);
void audio_share_buffer_deinit(struct audio_share_buffer *sbuf);
void audio_share_buffer_reset(struct audio_share_buffer *sbuf);
uint32_t audio_share_buffer_avail_read_samples(struct audio_share_buffer *sbuf);
void audio_share_buffer_read(struct audio_share_buffer *sbuf,
    void **pbuf, uint32_t *psamples, uint8_t ch, uint8_t sample_size);
void audio_share_buffer_get_read_ptr(struct audio_share_buffer *sbuf, void **p_ptr);
void audio_share_buffer_update_read_ptr(struct audio_share_buffer *sbuf, uint32_t samples);

uint32_t audio_share_buffer_avail_write_samples(struct audio_share_buffer *sbuf);


void audio_share_buffer_write(struct audio_share_buffer *sbuf,
    const void **pbuf, uint32_t *psamples, uint8_t ch, uint8_t sample_size,
    bool interleave_data);


#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_SHARE_BUFFER_H_ */
