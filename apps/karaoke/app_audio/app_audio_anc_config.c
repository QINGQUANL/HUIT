
#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "audio_service.h"
#include "anc_service.h"
#include "info_mem.h"
#include "hal.h"
#include "mixer.h"
#include "audio_params_config.h"
#include "psm_manager.h"
#include "app_audio_anc.h"
#include "app_pmu.h"
#include "tws_profile.h"

#if AUDIO_ANC_EN

#define ANC_SOS_NUM (16*3)

#define ANC_POS_TO_FIXED(x,Q) ((int32_t)(((int64_t)((x)*(1<<((Q)+1)))+1)>>1))
#define ANC_NEG_TO_FIXED(x,Q) (int32_t)(ANC_POS_TO_FIXED(-(x),Q) | (1<<31))
#define ANC_Q_COEFF (27)
#define ANC_REAL_TO_COEFF(x) ((x) < 0 ? ANC_NEG_TO_FIXED(x,ANC_Q_COEFF) : ANC_POS_TO_FIXED(x,ANC_Q_COEFF))


const int32_t app_anc_sos[ANC_SOS_NUM][5] = CFG_ANC_MODE_PARAMS;
const int32_t app_ht_sos[ANC_SOS_NUM][5]= CFG_HT_MODE_PARAMS;
#ifdef CFG_RIGHT_ANC_MODE_PARAMS
const int32_t app_right_anc_sos[ANC_SOS_NUM][5] = CFG_RIGHT_ANC_MODE_PARAMS;
#endif
#ifdef CFG_RIGHT_HT_MODE_PARAMS
const int32_t app_right_ht_sos[ANC_SOS_NUM][5]= CFG_RIGHT_HT_MODE_PARAMS;
#endif

void anc_test_mode_config(uint8_t enable)
{
    pmu_vbus_spdif_set(enable);
}

void app_audio_anc_init(void)
{
    anc_init_params_t params;

    params.default_anc_ff_gain = DEFAULT_ANC_FF_GAIN;
    params.default_anc_fb_gain = DEFAULT_ANC_FB_GAIN;
    params.default_anc_comp_gain = DEFAULT_ANC_COMP_GAIN;
    params.default_ht_ff_gain = DEFAULT_HT_FF_GAIN;
    params.default_ht_fb_gain = DEFAULT_HT_FB_GAIN;
    params.default_ht_comp_gain = DEFAULT_HT_COMP_GAIN;
    params.test_mode_config_func = anc_test_mode_config;
    as_anc_init(&params);

#if AUDIO_ANC_EN
    AUSS_ANC_Config anc_cfg;
    anc_cfg.fs = 192000;
#ifdef AUDIO_ANC_AMIC_CIC_GAIN_CFG
    anc_cfg.amic_cic_gain = AUDIO_ANC_AMIC_CIC_GAIN_CFG;
#else
    anc_cfg.amic_cic_gain = 0;
#endif
    anc_cfg.amic_order = 4;
    anc_cfg.dmic_cic_gain = 0;
    anc_cfg.dmic_order = 4;
    anc_cfg.fade = 0;
#ifdef AUDIO_ANC_HT_FB_ENABLE
    anc_cfg.ht_fb_enable = AUDIO_ANC_HT_FB_ENABLE; //通透模式是否开FB
#else
    anc_cfg.ht_fb_enable = 0;
#endif
    anc_cfg.anc_coeff = app_anc_sos;
    anc_cfg.ht_coeff = app_ht_sos;

#ifdef CFG_RIGHT_ANC_MODE_PARAMS
    if (TWS_EAR_TYPE_RIGHT == tws_ear_type_get()) {
        anc_cfg.anc_coeff = app_right_anc_sos;
    }
#endif
#ifdef CFG_RIGHT_HT_MODE_PARAMS
    if (TWS_EAR_TYPE_RIGHT == tws_ear_type_get()) {
        anc_cfg.ht_coeff = app_right_ht_sos;
    }
#endif

    anc_cfg.ff0_src = AUDIO_ANC_FF_MIC_SEL;
    anc_cfg.fb0_src = AUDIO_ANC_FB_MIC_SEL;
    //gain值来自flash
    anc_cfg.ff0_gain = as_anc_ff_gain_get();
    anc_cfg.fb0_gain = as_anc_fb_gain_get();
    anc_cfg.comp0_gain = as_anc_comp_gain_get();
    anc_cfg.ht_ff0_gain = as_ht_ff_gain_get();
    anc_cfg.ht_fb0_gain = as_ht_fb_gain_get();
    anc_cfg.ht_comp0_gain = as_ht_comp_gain_get();
    anc_cfg.dac_idx0 = ffs(AUDIO_DAC_OUT_CH_MASK) - 1;

    //tws方案ff1,fb1,comp1未使用
    anc_cfg.ff1_src = AUDIO_ANC_NONE;
    anc_cfg.fb1_src = AUDIO_ANC_NONE;
    anc_cfg.ff1_gain = (float)1.0;
    anc_cfg.fb1_gain = (float)1.0;
    anc_cfg.comp1_gain = (float)1.0;
    anc_cfg.dac_idx1 = 0;

    HAL_mixer_set_ctl(SND_CODEC, ANC_CFG, (uint32_t)&anc_cfg);
    HAL_mixer_set_ctl(SND_CODEC, ANC_FF0_PGA_GAIN, AUDIO_ANC_FF_MIC_ANALOG_GAIN);
    HAL_mixer_set_ctl(SND_CODEC, ANC_FB0_PGA_GAIN, AUDIO_ANC_FB_MIC_ANALOG_GAIN);

    //spdif dump
    //HAL_gpio_config(PIN_B28 | PIN_FUNC9); //spdif tx gpio
    //HAL_mixer_set_ctl(SND_CODEC, AUSS_SPDIF_DUMP_SEL, 1);
    poweron_anc_mode_param_get();

#endif
}
#endif