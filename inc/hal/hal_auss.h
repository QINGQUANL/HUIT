#ifndef _HAL_AUSS_LIB_H_
#define _HAL_AUSS_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys_inc.h"

enum auss_features_e {
    AUSS_FEATURES_DAC_SDM2          = (1 << 0),
    AUSS_FEATURES_LINEIN_OPTIMISE   = (1 << 1),
    AUSS_FEATURES_HEADSET_OPTIMISE  = (1 << 2),
    AUSS_FEATURES_MICBIAS_FOR_MUTE  = (1 << 3),
};

typedef struct {
    uint8_t enable;
    uint8_t coef_mp;
    uint16_t coef_rshift;
} AUSS_HPF_Config;

enum hal_anc_mode_e {
    HAL_ANC_MODE_CLOSE,          //关闭
    HAL_ANC_MODE_ANC,            //ANC降噪模式
    HAL_ANC_MODE_HT,             //通透模式
    HAL_ANC_MODE_TEST_SPK_FB,    //测试模式（测试SPK FB)
    HAL_ANC_MODE_TEST_FF_FB,     //测试模式（测试FF FB)
    HAL_ANC_MODE_PRE_SET,
    HAL_ANC_MODE_HT2,             //通透模式2
    HAL_ANC_MODE_ADPT,            //自适应
};

//ANC MIC选择
enum audio_anc_source_e {
    AUDIO_ANC_NONE                      = 0,
    AUDIO_ANC_AMIC0,    //模拟MIC0
    AUDIO_ANC_AMIC1,    //模拟MIC1
    AUDIO_ANC_AMIC2,    //模拟MIC2
    AUDIO_ANC_AUX0,
    AUDIO_ANC_AUX1,
    AUDIO_ANC_AUX2,
    AUDIO_ANC_LININ0,
    AUDIO_ANC_LININ1,
    AUDIO_ANC_LININ2,

    AUDIO_ANC_DMIC0F,/* DMICDATA0 falling edge */
    AUDIO_ANC_DMIC0R,/* DMICDATA0 rising edge */

    AUDIO_ANC_DMIC1F,/* DMICDATA1 falling edge */
    AUDIO_ANC_DMIC1R,/* DMICDATA1 rising edge */
};

typedef struct {
    uint32_t fs;

    const void *anc_coeff;
    const void *ht_coeff;

    uint8_t amic_cic_gain;
    uint8_t amic_order;
    uint8_t dmic_cic_gain;
    uint8_t dmic_order;
    uint8_t fade;

    uint8_t ht_fb_enable;

    enum audio_anc_source_e ff0_src;
    enum audio_anc_source_e fb0_src;
    float ff0_gain;
    float fb0_gain;
    float comp0_gain;
    float ht_ff0_gain;
    float ht_fb0_gain;
    float ht_comp0_gain;
    uint8_t dac_idx0;

    enum audio_anc_source_e ff1_src;
    enum audio_anc_source_e fb1_src;
    float ff1_gain;
    float fb1_gain;
    float comp1_gain;
    float ht_ff1_gain;
    float ht_fb1_gain;
    float ht_comp1_gain;
    uint8_t dac_idx1;
} AUSS_ANC_Config;


typedef struct {
    const void *ht2_coeff;
    float ht2_ff0_gain;
    float ht2_fb0_gain;
    float ht2_comp0_gain;
    float ht2_ff1_gain;
    float ht2_fb1_gain;
    float ht2_comp1_gain;
} AUSS_HT2_Config;



typedef struct {
    uint8_t anc_mode;//anc, ht
    uint8_t type;//ff, fb, comp
    uint8_t idx;//0~15
    int32_t coeff[5];
} auss_anc_coeff_rt_params_t;

typedef struct {
    void (*callback)(void *handle, void *buf, int32_t samples);
    void *callback_handle;
} auss_callback_params_t;

enum auss_ctl {
    AUSS_CTL_NONE,

    SIDETONE_CFG,   /* see @AUSS_SIDETONE_Config */
    MICBIAS,        /* mic bias: 0 ~ 3 */
    LPGA_GAIN,
    RPGA_GAIN,
    XPGA_GAIN,

    LPA_MUTE,
    RPA_MUTE,
    XPA_MUTE,

    OUTPUT_POWER_BOOST,

    LINEOUT_RMIXL,  /* 0: normal; 1: mix pa R to lineout L */
    LINEOUT_LMIXR,  /* 0: normal; 1: mix pa L to lineout R */
    LINEOUTL_EN,    /* 0: disable lineout L; 1: enable lineout L */
    LINEOUTR_EN,    /* 0: disable lineout R; 1: enable lineout R */
    LINEOUTL_GAIN,  /* lineout L gain: 0 ~ 7 */
    LINEOUTR_GAIN,  /* lineout R gain: 0 ~ 7 */

    LINEOUTL_GAIN_EX,
    LINEOUTR_GAIN_EX,


    ADC0_GAIN,      /* ADC gain: 0~0x1ff */
    ADC1_GAIN,
    ADC2_GAIN,
    ADC3_GAIN,
    ADC4_GAIN,

    DMIXER_RCH,     /* DMIXER RCH select: bit0-DataR, bit1-DataL, bit2-SideTone */
    DMIXER_LCH,     /* DMIXER LCH select: bit0-DataL, bit1-DataR, bit2-SideTone */
    DMIXER_RMODE,   /* DMIXER RCH mode: 0-L, 1-R, 2-(L+R)/2, 3-(L-R)/2, 4-(R-L)/2 */
    DMIXER_LMODE,   /* DMIXER LCH mode: 0-L, 1-R, 2-(L+R)/2, 3-(L-R)/2, 4-(R-L)/2 */
    DAC_GAIN,       /* DAC gain: 0~0x1ff */

    ADC0_DITHER_EN,
    ADC1_DITHER_EN,
    ADC2_DITHER_EN,

    ADC0_HPF_EN,
    ADC1_HPF_EN,
    ADC2_HPF_EN,
    ADC3_HPF_EN,
    ADC4_HPF_EN,
    DAC_HPF_EN,

    ADC0_HPF_CFG,   /* see @AUSS_HPF_Config */
    ADC1_HPF_CFG,
    ADC2_HPF_CFG,
    ADC3_HPF_CFG,
    ADC4_HPF_CFG,
    DAC_HPF_CFG,


    DMICDATA0F_EN, /* DMICDATA0 falling edge data enable */
    DMICDATA0R_EN, /* DMICDATA0 rising edge data enable */
    DMICDATA1F_EN, /* DMICDATA1 falling edge data enable */
    DMICDATA1R_EN, /* DMICDATA1 rising edge data enable */

    AMIC0_EN,
    AMIC1_EN,
    AMIC2_EN,

    AMIC0_GAIN,
    AMIC1_GAIN,
    AMIC2_GAIN,

    AD0PGA3MIXGAIN,
    AD1PGA3MIXGAIN,
    AD2PGA3MIXGAIN,

    AMIC0_SINGLE_END,
    AMIC1_SINGLE_END,
    AMIC2_SINGLE_END,

    LINEIN0_EN,
    LINEIN1_EN,

    LINEIN0_SINGLE_END,
    LINEIN1_SINGLE_END,

    LINEIN0_GAIN,
    LINEIN1_GAIN,

    AUX0_EN,
    AUX1_EN,
    AUX2_EN,

    AUX0_GAIN,
    AUX1_GAIN,
    AUX2_GAIN,

    BIAS_SHRINK_EN,

    VAD_EN,

    /* 配置ANC参数
     * Example:
     *    AUSS_ANC_Config cfg;
     *    cfg.fs = ...
     *    ...
     *    HAL_mixer_set_ctl(SND_CODEC, ANC_CFG, (uint32_t)&cfg);
     */
    ANC_CFG,

    ANC_FF0_PGA_GAIN,
    ANC_FF1_PGA_GAIN,
    ANC_FB0_PGA_GAIN,
    ANC_FB1_PGA_GAIN,

    /* 设置ANC模式ff0 gain
     * Example:
     *    float val = (float)0.5;
     *    HAL_mixer_set_ctl(SND_CODEC, ANC_ANC_FF0_GAIN, (uint32_t)&val);
     */
    ANC_ANC_FF0_GAIN,

    /* 设置ANC模式ff1 gain
     * Example:
     *    float val = (float)0.5;
     *    HAL_mixer_set_ctl(SND_CODEC, ANC_ANC_FF1_GAIN, (uint32_t)&val);
     */
    ANC_ANC_FF1_GAIN,

    /* 设置ANC模式fb0 gain
     * Example:
     *    float val = (float)0.5;
     *    HAL_mixer_set_ctl(SND_CODEC, ANC_ANC_FB0_GAIN, (uint32_t)&val);
     */
    ANC_ANC_FB0_GAIN,

    /* 设置ANC模式fb1 gain
     * Example:
     *    float val = (float)0.5;
     *    HAL_mixer_set_ctl(SND_CODEC, ANC_ANC_FB1_GAIN, (uint32_t)&val);
     */
    ANC_ANC_FB1_GAIN,

    /* 设置ANC模式comp0 gain
     * Example:
     *    float val = (float)0.5;
     *    HAL_mixer_set_ctl(SND_CODEC, ANC_ANC_COMP0_GAIN, (uint32_t)&val);
     */
    ANC_ANC_COMP0_GAIN,

    /* 设置ANC模式comp1 gain
     * Example:
     *    float val = (float)0.5;
     *    HAL_mixer_set_ctl(SND_CODEC, ANC_ANC_COMP1_GAIN, (uint32_t)&val);
     */
    ANC_ANC_COMP1_GAIN,

    /* 设置通透模式ff0 gain
     * Example:
     *    float val = (float)0.5;
     *    HAL_mixer_set_ctl(SND_CODEC, ANC_HT_FF0_GAIN, (uint32_t)&val);
     */
    ANC_HT_FF0_GAIN,

    /* 设置通透模式ff1 gain
     * Example:
     *    float val = (float)0.5;
     *    HAL_mixer_set_ctl(SND_CODEC, ANC_HT_FF1_GAIN, (uint32_t)&val);
     */
    ANC_HT_FF1_GAIN,

    /* 设置通透模式fb0 gain
     * Example:
     *    float val = (float)0.5;
     *    HAL_mixer_set_ctl(SND_CODEC, ANC_HT_FB0_GAIN, (uint32_t)&val);
     */
    ANC_HT_FB0_GAIN,

    /* 设置通透模式fb1 gain
     * Example:
     *    float val = (float)0.5;
     *    HAL_mixer_set_ctl(SND_CODEC, ANC_HT_FB1_GAIN, (uint32_t)&val);
     */
    ANC_HT_FB1_GAIN,

    /* 设置通透模式comp0 gain
     * Example:
     *    float val = (float)0.5;
     *    HAL_mixer_set_ctl(SND_CODEC, ANC_HT_COMP0_GAIN, (uint32_t)&val);
     */
    ANC_HT_COMP0_GAIN,

    /* 设置通透模式comp1 gain
     * Example:
     *    float val = (float)0.5;
     *    HAL_mixer_set_ctl(SND_CODEC, ANC_HT_COMP1_GAIN, (uint32_t)&val);
     */
    ANC_HT_COMP1_GAIN,

    /* 设置ANC工作模式
     * Example:
     *    //关闭
     *    HAL_mixer_set_ctl(SND_CODEC, ANC_EN, HAL_ANC_MODE_CLOSE);
     *    //ANC模式
     *    HAL_mixer_set_ctl(SND_CODEC, ANC_EN, HAL_ANC_MODE_ANC);
     *    //通透模式
     *    HAL_mixer_set_ctl(SND_CODEC, ANC_EN, HAL_ANC_MODE_HT);
     */
    ANC_EN,

    ANC_PRE_SET,

    ANC_COEFF_WRITE,
    ANC_COEFF_APPLY,
    ANC_COEFF_READ,

    DAC_CH_MASK,
    DAC_CH_MASK_GET,
    DAC_SAMPLE_SIZE,

    ADC_CH_MASK_GET,

    MBIAS1_EN, //mbias1 en
    MBIAS0_EN, //mbias0 en
    AD0_MIX_MODE, //ad0 mix mode
    AD1_MIX_MODE, //ad1 mix mode
    AD2_MIX_MODE, //ad2 mix mode
    AUSS_SPDIF_DUMP_SEL,
    DAC_CHSWAP,
    DAC0_HPF_EN,
    DAC0_HPF_RSHIFT,
    DAC0_HPF_MP,
    DAC1_HPF_EN,
    DAC1_HPF_RSHIFT,
    DAC1_HPF_MP,
    DAC2_HPF_EN,
    DAC2_HPF_RSHIFT,
    DAC2_HPF_MP,
    DMIC0_DAT_INV,
    DMIC0_CLK_INV,
    DMIC1_DAT_INV,
    DMIC1_CLK_INV,
    AMIC_XGPIO,
    BIAS_CURRENT_REG6,
    ANC_COEFF_TYPE,
    ANC_EXP_HOOK,
    ANC_FILTER,
    DAC_SDMPARA,
    MBIAS_BYPASS_EN, //mbias bypass en

    //DAC关闭独立控制功能，如果打开此功能。则HAL_auss_close不会关闭DAC，
    //需要APP自己在合适的地方调用DAC_STOP控制接口关闭
    DAC_STOP_INDEPENDENT_CTRL,
    DAC_STOP,
    DAC_START,
    DAC_START_STA_GET,
    DAC_STOP_DAC_RESET,
    DAC_POWEROFF,
    USER_MUTEX_LOCK_SET,
    BIASEN,
    DMIC_CIC_GAIN,
    DAC_IRQ_CPU,
    ADC_IRQ_CPU,
    DAC_TLEV,
    DAC_DMA_CALLBACK,
    ADC_DMA_CALLBACK,
    MICBIAS_READY_TIME,
    ANC_SWITCH_RESET_EN,
    AUSS_RESET,
    DAC_DC_OFFSET,

    AD0_OP1_BIAS,
    AD1_OP1_BIAS,
    AD2_OP1_BIAS,

    HT2_CFG,
    ANC_ADPT_LEV_UPDATE,
};

enum hal_auss_transfer_type_e {
    HAL_AUSS_TRANSFER_TYPE_BLOCK,
    HAL_AUSS_TRANSFER_TYPE_IRQ,
    HAL_AUSS_TRANSFER_TYPE_DMA,
};

typedef struct {
    void (*dma_pre_full_callback)(void);
    void (*dma_full_callback)(void);

    uint32_t fs;
    uint8_t sample_size; //对应Sample Resolution
    uint8_t ch;
    uint8_t direction;
    uint8_t transfer_type;

    uint8_t dma_ch;
} hal_auss_stream_init_t;

void HAL_auss_init(void *params);
void *HAL_auss_open(void *hal_auss_stream_params);
void HAL_auss_close(void *handle);
int32_t HAL_auss_read(void *handle, void *data, int32_t bytes);
int32_t HAL_auss_write(void *handle, void *data, int32_t bytes);
int32_t HAL_auss_ioctl(void *handle, uint32_t cmd, uint32_t arg);
void HAL_auss_deinit();
bool is_auss_enable();

#ifdef __cplusplus
}
#endif

#endif

