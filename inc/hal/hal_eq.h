#ifndef _HAL_EQ_H_
#define _HAL_EQ_H_

#include "sys_inc.h"

#ifdef __cplusplus
extern "C" {
#endif

enum hal_eq_cmd_e {
    HAL_EQ_CMD_NONE,
    HAL_EQ_CMD_RESET,
    HAL_EQ_CMD_PAMPPRM_SET,
    HAL_EQ_CMD_SECTPRM_SET,
    HAL_EQ_CMD_RUNTIME_SECTPRM_WRITE,
    HAL_EQ_CMD_RUNTIME_SECTPRM_UPDATE,
};

enum hal_eq_filter_type_e {
    HAL_EQ_FILTER_LOWSHELVING,
    HAL_EQ_FILTER_PEAKNOTCH,
    HAL_EQ_FILTER_HIGHSHELVING,
    HAL_EQ_FILTER_LOWPASS,
    HAL_EQ_FILTER_BANDPASS,
    HAL_EQ_FILTER_HIGHPASS,
};


typedef struct HAL_eq_init_params_s {
    void (*period_finished_callback)(void *user);
    void *user;
    void *pcm_buf;
    uint32_t period_size; //samples
    uint32_t period_count;
    uint32_t fs;
    uint8_t ch;
    uint8_t sample_size;
    uint8_t band_cnt;
    uint8_t support_coeff_update;
} HAL_eq_init_params_t;

typedef struct {
    /*
     * 0 for left channel, 1 for right channel
     */
    int8_t ch;
    /*
     */
    float PreAmp;
} hal_eq_pampprm_t;

typedef struct {
    /* filter type shelf low, shelf high, peak/notch */
    int16_t filter_type;
    /* center frequency(peak/notch) or crossover frequency(shelving filter ) */
    uint16_t fc;
    /* Gain -12~12 dB, Q3 */
    float G;
    /* Qfactor, -64 ~ 64, Q10 */
    float Q_factor;
} hal_eq_biquad_setting_t;

typedef struct {
    /*
     * 0 for left channel, 1 for right channel
     */
    uint8_t ch_idx;
    /*
     * band idx, 0 to MAX_EQ_BANDS -1
     */
    uint8_t band_idx;
    /*
     * filter settings
     */
    hal_eq_biquad_setting_t setting;
    /* disable */
    uint8_t disable;
} hal_eq_sectprm_t;

void HAL_eq_init(uint8_t max_eq_instance);
void HAL_eq_deinit(void);
void *HAL_eq_open(void *HAL_eq_init_params);
void HAL_eq_close(void *handle);
int32_t HAL_eq_ioctl(void *handle, uint32_t cmd, void *arg);
int32_t HAL_eq_write(void *handle, void *data, uint32_t bytes);
int32_t HAL_eq_read(void *handle, void *data, uint32_t bytes);


#ifdef __cplusplus
}
#endif

#endif /* _HAL_EQ_H_ */
