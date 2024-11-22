#ifndef _ANC_SERVICE_H_
#define _ANC_SERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sys_inc.h"

enum anc_working_mode_e {
    ANC_WORKING_MODE_DIS        = 0, /* 主动降噪关闭 */
    ANC_WORKING_MODE_ANC        = 1, /* 主动降噪打开 */
    ANC_WORKING_MODE_HEARING    = 2, /* 通透模式 */
    ANC_WORKING_MODE_TEST_SPK_FB = 3,
    ANC_WORKING_MODE_TEST_FF_FB = 4,
};

typedef struct anc_init_params_s {
    float default_anc_ff_gain;
    float default_anc_fb_gain;
    float default_anc_comp_gain;
    float default_ht_ff_gain;
    float default_ht_fb_gain;
    float default_ht_comp_gain;
    void (*test_mode_config_func)(uint8_t test_mode_enable);
} anc_init_params_t;

void as_anc_init(anc_init_params_t *params);

void as_anc_working_mode_set(enum anc_working_mode_e working_mode);
uint8_t as_anc_working_mode_get(void);

void as_anc_working_mode_suspend(void);
uint8_t as_anc_working_mode_suspend_get(void);
void as_anc_working_mode_resume(void);

void as_anc_ff_gain_set(float _ff_gain);
void as_anc_fb_gain_set(float _fb_gain);
void as_anc_comp_gain_set(float _comp_gain);
void as_ht_ff_gain_set(float _ht_ff_gain);
void as_ht_fb_gain_set(float _ht_fb_gain);
void as_ht_comp_gain_set(float _ht_comp_gain);

float as_anc_ff_gain_get(void);
float as_anc_fb_gain_get(void);
float as_anc_comp_gain_get(void);
float as_ht_ff_gain_get(void);
float as_ht_fb_gain_get(void);
float as_ht_comp_gain_get(void);

void as_anc_ff_mute_set(bool mute);
void as_anc_fb_mute_set(bool mute);

void as_anc_rt_coeff_write(uint8_t anc_mode, uint8_t coeff_type, uint8_t coeff_idx, int32_t *coeff);
void as_anc_rt_coeff_apply(void);

void as_anc_x_gain_store(void);

#ifdef __cplusplus
}
#endif

#endif /* _ANC_SERVICE_H_ */
