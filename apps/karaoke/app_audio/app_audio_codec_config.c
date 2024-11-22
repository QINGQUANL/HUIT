#include "sys_inc.h"
#include "audio_service.h"
#include "hal.h"
#include "middleware.h"
#include "user_config.h"
#include "tws_profile.h"
#include "audio_in.h"
#include "app_audio_i2s_config.h"

#if (AUDIO_DMIC_CLK_PIN_SEL == 0)
#define AUDIO_DMIC_CLK_PIN_CFG      (PIN_A5 | PIN_FUNC6)
#elif (AUDIO_DMIC_CLK_PIN_SEL == 1)
#define AUDIO_DMIC_CLK_PIN_CFG      (PIN_B11 | PIN_FUNC2)
#elif (AUDIO_DMIC_CLK_PIN_SEL == 2)
#define AUDIO_DMIC_CLK_PIN_CFG      (PIN_B12 | PIN_FUNC2)
#elif (AUDIO_DMIC_CLK_PIN_SEL == 3)
#define AUDIO_DMIC_CLK_PIN_CFG      (PIN_B15 | PIN_FUNC2)
#elif (AUDIO_DMIC_CLK_PIN_SEL == 4)
#define AUDIO_DMIC_CLK_PIN_CFG      (PIN_B17 | PIN_FUNC2)
#elif (AUDIO_DMIC_CLK_PIN_SEL == 5)
#define AUDIO_DMIC_CLK_PIN_CFG      (PIN_B22 | PIN_FUNC4)
#elif (AUDIO_DMIC_CLK_PIN_SEL == 6)
#define AUDIO_DMIC_CLK_PIN_CFG      (PIN_B25 | PIN_FUNC12)
#elif (AUDIO_DMIC_CLK_PIN_SEL == 7)
#define AUDIO_DMIC_CLK_PIN_CFG      (PIN_B26 | PIN_FUNC8)
#elif (AUDIO_DMIC_CLK_PIN_SEL == 8)
#define AUDIO_DMIC_CLK_PIN_CFG      (PIN_B28 | PIN_FUNC3)
#elif (AUDIO_DMIC_CLK_PIN_SEL == 9)
#define AUDIO_DMIC_CLK_PIN_CFG      (PIN_D0 | PIN_FUNC8)
#elif (AUDIO_DMIC_CLK_PIN_SEL == 10)
#define AUDIO_DMIC_CLK_PIN_CFG      (PIN_D2 | PIN_FUNC7)
#elif (AUDIO_DMIC_CLK_PIN_SEL == 11)
#define AUDIO_DMIC_CLK_PIN_CFG      (PIN_D3 | PIN_FUNC8)
#elif (AUDIO_DMIC_CLK_PIN_SEL == 12)
#define AUDIO_DMIC_CLK_PIN_CFG      (PIN_D5 | PIN_FUNC8)
#elif (AUDIO_DMIC_CLK_PIN_SEL == 13)
#define AUDIO_DMIC_CLK_PIN_CFG      (PIN_D7 | PIN_FUNC8)
#elif (AUDIO_DMIC_CLK_PIN_SEL == 14)
#define AUDIO_DMIC_CLK_PIN_CFG      (PIN_D9 | PIN_FUNC2)
#endif

#if (AUDIO_DMICDATA0_PIN_SEL == 0)
#define AUDIO_DMICDATA0_PIN_CFG    (PIN_A4 | PIN_FUNC6)
#elif (AUDIO_DMICDATA0_PIN_SEL == 1)
#define AUDIO_DMICDATA0_PIN_CFG    (PIN_B10 | PIN_FUNC2)
#elif (AUDIO_DMICDATA0_PIN_SEL == 2)
#define AUDIO_DMICDATA0_PIN_CFG    (PIN_B16 | PIN_FUNC2)
#elif (AUDIO_DMICDATA0_PIN_SEL == 3)
#define AUDIO_DMICDATA0_PIN_CFG    (PIN_B21 | PIN_FUNC4)
#elif (AUDIO_DMICDATA0_PIN_SEL == 4)
#define AUDIO_DMICDATA0_PIN_CFG    (PIN_B27 | PIN_FUNC8)
#elif (AUDIO_DMICDATA0_PIN_SEL == 5)
#define AUDIO_DMICDATA0_PIN_CFG    (PIN_B30 | PIN_FUNC3)
#elif (AUDIO_DMICDATA0_PIN_SEL == 6)
#define AUDIO_DMICDATA0_PIN_CFG    (PIN_D2 | PIN_FUNC8)
#elif (AUDIO_DMICDATA0_PIN_SEL == 7)
#define AUDIO_DMICDATA0_PIN_CFG    (PIN_D6 | PIN_FUNC8)
#elif (AUDIO_DMICDATA0_PIN_SEL == 8)
#define AUDIO_DMICDATA0_PIN_CFG    (PIN_D10 | PIN_FUNC2)
#endif

#if (AUDIO_DMICDATA1_PIN_SEL == 0)
#define AUDIO_DMICDATA1_PIN_CFG    (PIN_B13 | PIN_FUNC2)
#elif (AUDIO_DMICDATA1_PIN_SEL == 1)
#define AUDIO_DMICDATA1_PIN_CFG    (PIN_B14 | PIN_FUNC2)
#elif (AUDIO_DMICDATA1_PIN_SEL == 2)
#define AUDIO_DMICDATA1_PIN_CFG    (PIN_B18 | PIN_FUNC2)
#elif (AUDIO_DMICDATA1_PIN_SEL == 3)
#define AUDIO_DMICDATA1_PIN_CFG    (PIN_B23 | PIN_FUNC4)
#elif (AUDIO_DMICDATA1_PIN_SEL == 4)
#define AUDIO_DMICDATA1_PIN_CFG    (PIN_B25 | PIN_FUNC8)
#elif (AUDIO_DMICDATA1_PIN_SEL == 5)
#define AUDIO_DMICDATA1_PIN_CFG    (PIN_B29 | PIN_FUNC3)
#elif (AUDIO_DMICDATA1_PIN_SEL == 6)
#define AUDIO_DMICDATA1_PIN_CFG    (PIN_D1 | PIN_FUNC8)
#elif (AUDIO_DMICDATA1_PIN_SEL == 7)
#define AUDIO_DMICDATA1_PIN_CFG    (PIN_D4 | PIN_FUNC8)
#elif (AUDIO_DMICDATA1_PIN_SEL == 8)
#define AUDIO_DMICDATA1_PIN_CFG    (PIN_D8 | PIN_FUNC2)
#endif

uint8_t audio_rec_input_sel;
uint8_t audio_linein_mode_input_sel;

uint8_t audio_rec_input_sel_get(void)
{
    return audio_rec_input_sel;
}

void audio_rec_input_sel_set(uint8_t sel)
{
    audio_rec_input_sel = sel;
}

uint8_t audio_linein_mode_input_sel_get(void)
{
    return audio_linein_mode_input_sel;
}

void audio_linein_mode_input_sel_set(uint8_t sel)
{
    audio_linein_mode_input_sel = sel;
}


void codec_init(void)
{
    audio_rec_input_sel = AUDIO_REC_INPUT_SEL;
    audio_linein_mode_input_sel = AUDIO_LINEIN_MODE_INPUT_SEL;
#if ((0 != AUDIO_OUT_DEVICE_SEL) || (0 != AUDIO_IN_DEVICE_SEL))
#if (AUDIO_DMICDATA0_EN || AUDIO_DMICDATA1_EN)
    HAL_gpio_config(AUDIO_DMIC_CLK_PIN_CFG); /* 数字MIC CLK引脚配置 */
#endif
#if AUDIO_DMICDATA0_EN
    HAL_gpio_config(AUDIO_DMICDATA0_PIN_CFG); /* 数字MICDATA0 引脚配置 */
#endif
#if AUDIO_DMICDATA1_EN
    HAL_gpio_config(AUDIO_DMICDATA1_PIN_CFG); /* 数字MICDATA1 引脚配置 */
#endif

    HAL_auss_ioctl(NULL, DAC_CH_MASK, 0x3);
    HAL_auss_ioctl(NULL, DAC_DC_OFFSET, 5);
    HAL_auss_init(NULL);
    HAL_sndauss_register();

#if AUDIO_DMICDATA0_EN
    HAL_mixer_set_ctl(SND_CODEC, DMICDATA0F_EN, true);
    HAL_mixer_set_ctl(SND_CODEC, DMICDATA0R_EN, true);
    audio_service_set_input_digital_gain(AUDIO_INPUT_AMIC0, AUDIO_DMICDATA0F_DIGITAL_GAIN);
    audio_service_set_input_digital_gain(AUDIO_INPUT_AMIC1, AUDIO_DMICDATA0R_DIGITAL_GAIN);
#endif

#if AUDIO_DMICDATA1_EN
    HAL_mixer_set_ctl(SND_CODEC, DMICDATA1F_EN, true);
    audio_service_set_input_digital_gain(AUDIO_INPUT_AMIC2, AUDIO_DMICDATA1F_DIGITAL_GAIN);
#endif
#endif
}


uint32_t codec_adc_input_source_get(uint8_t mode)
{
    if (AUDIO_IN_MODE_KWS == mode) {
        return AUDIO_INPUT_AMIC2;
    } else if (AUDIO_IN_MODE_LINEIN == mode) {
        return audio_linein_mode_input_sel;//AUDIO_LINEIN_MODE_INPUT_SEL;
    } else {
        return audio_rec_input_sel;//AUDIO_REC_INPUT_SEL;
    }
}

void app_audio_codec_analog_gain_config(void)
{
    HAL_mixer_set_ctl(SND_CODEC, AMIC0_GAIN, AUDIO_AMIC0_ANALOG_GAIN);          /* 模拟MIC0模拟音量 */
    HAL_mixer_set_ctl(SND_CODEC, AMIC1_GAIN, AUDIO_AMIC1_ANALOG_GAIN);          /* 模拟MIC1模拟音量 */
    HAL_mixer_set_ctl(SND_CODEC, AMIC2_GAIN, AUDIO_AMIC2_ANALOG_GAIN);          /* 模拟MIC2模拟音量 */
    HAL_mixer_set_ctl(SND_CODEC, LINEIN0_GAIN, AUDIO_LINEIN0_ANALOG_GAIN);      /* linein0模拟音量 */
    HAL_mixer_set_ctl(SND_CODEC, LINEIN1_GAIN, AUDIO_LINEIN1_ANALOG_GAIN);      /* linein1模拟音量 */

    HAL_mixer_set_ctl(SND_CODEC, AD0PGA3MIXGAIN, AUDIO_AMIC0_ANALOG_GAIN);      /* ADC0 mix模拟音量 */
    HAL_mixer_set_ctl(SND_CODEC, AD1PGA3MIXGAIN, AUDIO_AMIC1_ANALOG_GAIN);      /* ADC1 mix模拟音量 */
    HAL_mixer_set_ctl(SND_CODEC, AD2PGA3MIXGAIN, AUDIO_AMIC2_ANALOG_GAIN);      /* ADC2 mix模拟音量 */

    HAL_mixer_set_ctl(SND_CODEC, AUX0_GAIN, AUDIO_AUX0_ANALOG_GAIN);
    HAL_mixer_set_ctl(SND_CODEC, AUX1_GAIN, AUDIO_AUX1_ANALOG_GAIN);
    HAL_mixer_set_ctl(SND_CODEC, AUX2_GAIN, AUDIO_AUX2_ANALOG_GAIN);
}

/*
 * audiocodec用户配置
 */
void app_audio_codec_config(void)
{
#if ((0 != AUDIO_OUT_DEVICE_SEL) || (0 != AUDIO_IN_DEVICE_SEL))
    codec_init();

    //HAL_mixer_set_ctl(SND_CODEC, ADC0_HPF_EN, 1);         /* 使能ADC0高通滤波器 */
    //HAL_mixer_set_ctl(SND_CODEC, ADC1_HPF_EN, 1);         /* 使能ADC1高通滤波器 */
    //HAL_mixer_set_ctl(SND_CODEC, ADC2_HPF_EN, 1);         /* 使能ADC2高通滤波器 */
    //HAL_mixer_set_ctl(SND_CODEC, DAC_HPF_EN, 1);          /* 使能DAC高通滤波器 */
    //HAL_mixer_set_ctl(SND_CODEC, ADC0_DITHER_EN, 1);      /* 使能ADC0 dither */
    //HAL_mixer_set_ctl(SND_CODEC, ADC1_DITHER_EN, 1);      /* 使能ADC1 dither */
    //HAL_mixer_set_ctl(SND_CODEC, ADC2_DITHER_EN, 1);      /* 使能ADC2 dither */
    //HAL_mixer_set_ctl(SND_CODEC, OUTPUT_POWER_BOOST, 1);  /* 输出功率增强功能 */
    HAL_mixer_set_ctl(SND_CODEC, AMIC_XGPIO, 1);
    HAL_mixer_set_ctl(SND_CODEC, LPGA_GAIN, AUDIO_LPGA_GAIN); /* left pga gain: 0:0dB, 1:-3dB, ..., 7:-21dB; -3dB/step */
    HAL_mixer_set_ctl(SND_CODEC, RPGA_GAIN, AUDIO_RPGA_GAIN); /* right pga gain: 0:0dB, 1:-3dB, ..., 7:-21dB; -3dB/step */
    //HAL_mixer_set_ctl(SND_CODEC, MICBIAS, 0);  /* mic bias: 0 ~ 3,   0-2.0v; 1-2.34v; 2-2.67v; 3-2.9v */
    HAL_mixer_set_ctl(SND_CODEC, XPGA_GAIN, AUDIO_XPGA_GAIN);

    /* 加载user_config.h中的配置 */
    HAL_mixer_set_ctl(SND_CODEC, AMIC0_SINGLE_END, AUDIO_AMIC0_SINGLE_END);     /* 模拟MIC0单端模式： 0-差分模式， 1-单端模式 */
    HAL_mixer_set_ctl(SND_CODEC, AMIC1_SINGLE_END, AUDIO_AMIC1_SINGLE_END);     /* 模拟MIC1单端模式： 0-差分模式， 1-单端模式 */
    HAL_mixer_set_ctl(SND_CODEC, AMIC2_SINGLE_END, AUDIO_AMIC2_SINGLE_END);     /* 模拟MIC2单端模式： 0-差分模式， 1-单端模式 */
    HAL_mixer_set_ctl(SND_CODEC, LINEIN0_SINGLE_END, AUDIO_LINEIN0_SINGLE_END); /* LINEIN0单端模式： 0-差分模式， 1-单端模式 */
    HAL_mixer_set_ctl(SND_CODEC, LINEIN1_SINGLE_END, AUDIO_LINEIN1_SINGLE_END); /* LINEIN1单端模式： 0-差分模式， 1-单端模式 */
    
    //HAL_mixer_set_ctl(SND_CODEC, AMIC0_GAIN, AUDIO_AMIC0_ANALOG_GAIN);          /* 模拟MIC0模拟音量 */
    //HAL_mixer_set_ctl(SND_CODEC, AMIC1_GAIN, AUDIO_AMIC1_ANALOG_GAIN);          /* 模拟MIC1模拟音量 */
    //HAL_mixer_set_ctl(SND_CODEC, AMIC2_GAIN, AUDIO_AMIC2_ANALOG_GAIN);          /* 模拟MIC2模拟音量 */
    //HAL_mixer_set_ctl(SND_CODEC, LINEIN0_GAIN, AUDIO_LINEIN0_ANALOG_GAIN);      /* linein0模拟音量 */
    //HAL_mixer_set_ctl(SND_CODEC, LINEIN1_GAIN, AUDIO_LINEIN1_ANALOG_GAIN);      /* linein1模拟音量 */
    
    HAL_mixer_set_ctl(SND_CODEC, BIAS_CURRENT_REG6, 0x7);      /* mibas current */
    HAL_mixer_set_ctl(SND_CODEC, DAC_SDMPARA, 0xF << 12);
    HAL_mixer_set_ctl(SND_CODEC, DAC_GAIN, AUDIO_DAC_GAIN);
#if AUDIO_KARAOKE_LINEIN_EN         //在linein会悬空的方案，配置如下能减少底噪，功耗会有所增加
    HAL_mixer_set_ctl(SND_CODEC, AD0_OP1_BIAS, 6);              //AD0 SDM OP1 bias, n+1 uA, n max = 7
    HAL_mixer_set_ctl(SND_CODEC, AD1_OP1_BIAS, 6);              //AD1 SDM OP1 bias, n+1 uA, n max = 7
    HAL_mixer_set_ctl(SND_CODEC, AD2_OP1_BIAS, 6);              //AD2 SDM OP1 bias, n+1 uA, n max = 7
#endif
    
    //HAL_mixer_set_ctl(SND_CODEC, AUX0_GAIN, AUDIO_AUX0_ANALOG_GAIN);
    //HAL_mixer_set_ctl(SND_CODEC, AUX1_GAIN, AUDIO_AUX1_ANALOG_GAIN);
    //HAL_mixer_set_ctl(SND_CODEC, AUX2_GAIN, AUDIO_AUX2_ANALOG_GAIN);
#endif
}

void app_audio_set_dac_gain(int8_t gain)
{
    logi("dac set gain %d",gain);
    HAL_mixer_set_ctl(SND_CODEC, DAC_GAIN, gain);
}

void app_audio_set_lpga_gain(int8_t gain)
{
    logi("lpga_gain %d", gain);
    HAL_mixer_set_ctl(SND_CODEC, LPGA_GAIN, AUDIO_LPGA_GAIN);
}

void app_audio_set_rpga_gain(int8_t gain)
{
    logi("rpga_gain %d", gain);
    HAL_mixer_set_ctl(SND_CODEC, RPGA_GAIN, gain);
}

void app_audio_set_xpga_gain(int8_t gain)
{
    logi("xpga_gain %d", gain);
    HAL_mixer_set_ctl(SND_CODEC, XPGA_GAIN, gain);
}

void app_audio_set_digital_mic0_gain(int8_t gain)
{
    //HAL_mixer_set_ctl(SND_CODEC, AMIC0_GAIN, gain);
}

void app_audio_set_digital_mic1_gain(int8_t gain)
{
    //HAL_mixer_set_ctl(SND_CODEC, AMIC1_GAIN, gain);
}

void app_audio_set_digital_mic2_gain(int8_t gain)
{
    //HAL_mixer_set_ctl(SND_CODEC, AMIC2_GAIN, gain);
}

void app_audio_set_analog_mic0_gain(int8_t gain)
{
    HAL_mixer_set_ctl(SND_CODEC, AD0PGA3MIXGAIN, gain); 
}

void app_audio_set_analog_mic1_gain(int8_t gain)
{
    HAL_mixer_set_ctl(SND_CODEC, AD1PGA3MIXGAIN, gain); 
}

void app_audio_set_analog_mic2_gain(int8_t gain)
{
    HAL_mixer_set_ctl(SND_CODEC, AD2PGA3MIXGAIN, gain); 
}

void app_audio_set_analog_linein0_gain(int8_t gain)
{
    HAL_mixer_set_ctl(SND_CODEC, LINEIN0_GAIN, gain);
}

void app_audio_set_analog_linein1_gain(int8_t gain)
{
    HAL_mixer_set_ctl(SND_CODEC, LINEIN1_GAIN, gain);
}

void app_audio_set_analog_aux0_gain(int8_t gain)
{
    HAL_mixer_set_ctl(SND_CODEC, AUX0_GAIN, gain);
}

void app_audio_set_analog_aux1_gain(int8_t gain)
{
    HAL_mixer_set_ctl(SND_CODEC, AUX1_GAIN, gain);
}

void app_audio_set_analog_aux2_gain(int8_t gain)
{
    HAL_mixer_set_ctl(SND_CODEC, AUX2_GAIN, gain);
}

void app_audio_hw_init(void)
{
#if ((0 != AUDIO_OUT_DEVICE_SEL) || (0 != AUDIO_IN_DEVICE_SEL))
    app_audio_codec_config();
#endif

#if I2S0_EN
    app_audio_i2s_config();
#endif
}
