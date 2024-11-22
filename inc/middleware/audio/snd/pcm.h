

#ifndef _PCM_H_
#define _PCM_H_

#include "sys_types.h"
#include "sys_inc.h"
#include "hal.h"
#include "sltk_audio.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "oal.h"

struct pcm;

enum pcm_flag {
    PCM_OUT     = 0x1,
    PCM_IN      = 0x2,
    PCM_NO_OS   = 0x4,
};

enum pcm_format {
    /* Signed, 8-bit */
    PCM_FORMAT_S8 = sizeof(uint8_t),
    /* Signed, 16-bit, little endian */
    PCM_FORMAT_S16_LE = sizeof(int16_t),
    /* Signed, 24-bit, little endian */
    PCM_FORMAT_S24_LE = sizeof(int8_t) * 3,
    /* Signed, 32-bit, little endian */
    PCM_FORMAT_S32_LE = sizeof(int32_t),
};

enum pcm_state {
    PCM_STATE_NOT_READY,
    PCM_STATE_READY,
    PCM_STATE_SUSPENDED,
    PCM_STATE_RUNNING,
    PCM_STATE_XRUN,
};

struct pcm_config {
    /* The number of channels in a frame */
    uint32_t ch;
    uint32_t sample_size;
    /* The number of frames per second */
    uint32_t rate;
    /* The number of frames in a period */
    uint32_t period_size;
    /* The number of periods in a PCM */
    uint32_t period_count;

    uint32_t start_threshold;
    uint32_t stop_threshold;
};

struct pcm {
    uint32_t id;
    enum pcm_flag pcm_flag;
    struct pcm_config config;
    void *buf;

    volatile enum pcm_state state;

    struct ring_buffer buf_ctrl;

    uint32_t hw_period_bytes;

    struct snd_hardware *hw;
};

struct pcm *pcm_open(uint32_t id,
    enum pcm_flag flag, const struct pcm_config *config);
int pcm_close(struct pcm *pcm);
int pcm_write(struct pcm *pcm, const void *data, uint32_t samples);
int pcm_read(struct pcm *pcm, void *data, uint32_t samples);

int pcm_start(struct pcm *pcm);
int pcm_stop(struct pcm *pcm);
bool pcm_is_ready(const struct pcm *pcm);
uint8_t pcm_get_hardware_status(struct pcm *pcm);
uint32_t pcm_get_remain(struct pcm *pcm);
play_timestamp_info_t pcm_get_play_timpstamp(struct pcm *pcm);
int64_t pcm_get_play_samples(struct pcm *pcm);
pcm_dbg_hook* pcm_export_dbg_interface();

bool sndcard_online(uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* _PCM_H_ */
