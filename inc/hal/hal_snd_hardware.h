
#ifndef _HAL_SND_HARDWARE_H_
#define _HAL_SND_HARDWARE_H_

#include "sys_types.h"
#include "ring_buffer.h"
#include "hal_dma.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SND_I2S         0
#define SND_CODEC       1
#define SND_AUSS        SND_CODEC

enum pcm_hw_trigger_cmd {
    PCM_HW_TRIGGER_START,
    PCM_HW_TRIGGER_STOP,
};

enum pcm_hw_direction {
    PCM_HW_DIR_OUT,
    PCM_HW_DIR_IN,
};

enum pcm_hw_state {
    PCM_HW_STATE_STOP,
    PCM_HW_STATE_IDLE,
    PCM_HW_STATE_RUNNING,
    PCM_HW_STATE_XRUN,
};

struct pcm_hw_params {
    uint32_t channels;
    uint32_t rate;
    uint32_t sample_size;

    uint32_t period_size;
    uint32_t period_count;

    void *sw_context;
};

struct pcm_hw_timestamp {
    uint64_t play_period_count; //TODO:remove
    uint32_t this_period_time;//TODO:remove

    volatile int64_t counter_sys_t; //采样率计数器更新的时间点
    volatile int64_t counter_fs; //采样率计数器，每播放一个点，计数器加1
    volatile uint32_t counter_fs_x128; //128倍过采样计数计，可理解为counter_fs的小数部分，每播放1/128个点计数器加1
};

typedef enum snd_hardware_ret_e {
    SND_HW_RET_NORAML = 0,
    SND_HW_INIT_FAIL = -1,
    SND_HW_RET_STREAM_END = -2,
    SND_HW_RET_XRUN = -3,
} sndhw_ret;

struct snd_hardware_info {
    uint8_t sample_size;
    uint8_t channels;
};

struct snd_hardware;

struct snd_hardware_ops {
    int (*hw_params)(struct snd_hardware *hw,
        enum pcm_hw_direction dir, struct pcm_hw_params *params);
    int (*hw_free)(struct snd_hardware *hw, enum pcm_hw_direction dir);
    int (*prepare)(struct snd_hardware *hw, enum pcm_hw_direction dir);
    int (*trigger)(struct snd_hardware *hw,
        enum pcm_hw_direction dir, enum pcm_hw_trigger_cmd cmd);
    int (*hw_set_ctl)(struct snd_hardware *hw, uint32_t ctl, uint32_t val);
    int (*hw_get_ctl)(struct snd_hardware *hw, uint32_t ctl, uint32_t *val);
    int (*timestamp_get)(struct snd_hardware *hw,
        enum pcm_hw_direction dir, struct pcm_hw_timestamp *stamp);
};


struct snd_hardware {
    struct snd_hardware_ops ops;

    void *tx_context;
    void *rx_context;

    volatile enum pcm_hw_state last_out_state;
    volatile enum pcm_hw_state out_state;
    volatile enum pcm_hw_state in_state;

    volatile uint32_t trigger_time;
    volatile uint64_t play_period_count;

    uint16_t tx_dma_bytes;
    uint16_t rx_dma_bytes;
    volatile bool out_wait_stop;
    volatile bool in_wait_stop;
};

int get_snd_hardware(struct snd_hardware **snd_hw, uint32_t id);

void hw_update_rptr(void *sw_context, uint32_t bytes);
void *hw_get_rptr(void *sw_context);
uint32_t hw_get_r_avail(void *sw_context);
void hw_update_wptr(void *sw_context, uint32_t bytes);
void *hw_get_wptr(void *sw_context);
uint32_t hw_get_w_avail(void *sw_context);

sndhw_ret snd_dma_free_early(struct snd_hardware *hw,
    enum pcm_hw_direction dir);

void snd_hardware_register(uint32_t id, struct snd_hardware *obj);

void HAL_sndi2s_register(void);
void HAL_sndauss_register(void);

#ifdef __cplusplus
}
#endif

#endif /* _HAL_SND_HARDWARE_H_ */
