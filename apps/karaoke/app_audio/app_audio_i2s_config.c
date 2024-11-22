#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "hal.h"
#include "twi.h"
#include "wm8978.h"

#if (I2S0_BCLK_PIN_SEL == 0)
#define I2S0_BCLK_PIN_CFG       (PIN_B9 | PIN_FUNC3 | PIN_DRI3)
#elif (I2S0_BCLK_PIN_SEL == 1)
#define I2S0_BCLK_PIN_CFG       (PIN_B14 | PIN_FUNC4 | PIN_DRI3)
#elif (I2S0_BCLK_PIN_SEL == 2)
#define I2S0_BCLK_PIN_CFG       (PIN_B21 | PIN_FUNC3 | PIN_DRI3)
#elif (I2S0_BCLK_PIN_SEL == 3)
#define I2S0_BCLK_PIN_CFG       (PIN_B22 | PIN_FUNC12 | PIN_DRI3)
#elif (I2S0_BCLK_PIN_SEL == 4)
#define I2S0_BCLK_PIN_CFG       (PIN_B28 | PIN_FUNC6 | PIN_DRI3)
#elif (I2S0_BCLK_PIN_SEL == 5)
#define I2S0_BCLK_PIN_CFG       (PIN_D1 | PIN_FUNC5 | PIN_DRI3)
#elif (I2S0_BCLK_PIN_SEL == 6)
#define I2S0_BCLK_PIN_CFG       (PIN_D5 | PIN_FUNC3 | PIN_DRI3)
#elif (I2S0_BCLK_PIN_SEL == 7)
#define I2S0_BCLK_PIN_CFG       (PIN_D8 | PIN_FUNC5 | PIN_DRI3)
#endif

#if (I2S0_LRCK_PIN_SEL == 0)
#define I2S0_LRCK_PIN_CFG       (PIN_B7 | PIN_FUNC10 | PIN_DRI3)
#elif (I2S0_LRCK_PIN_SEL == 1)
#define I2S0_LRCK_PIN_CFG       (PIN_B8 | PIN_FUNC3 | PIN_DRI3)
#elif (I2S0_LRCK_PIN_SEL == 2)
#define I2S0_LRCK_PIN_CFG       (PIN_B13 | PIN_FUNC4 | PIN_DRI3)
#elif (I2S0_LRCK_PIN_SEL == 3)
#define I2S0_LRCK_PIN_CFG       (PIN_B20 | PIN_FUNC3 | PIN_DRI3)
#elif (I2S0_LRCK_PIN_SEL == 4)
#define I2S0_LRCK_PIN_CFG       (PIN_B27 | PIN_FUNC6 | PIN_DRI3)
#elif (I2S0_LRCK_PIN_SEL == 5)
#define I2S0_LRCK_PIN_CFG       (PIN_D0 | PIN_FUNC3 | PIN_DRI3)
#elif (I2S0_LRCK_PIN_SEL == 6)
#define I2S0_LRCK_PIN_CFG       (PIN_D2 | PIN_FUNC3 | PIN_DRI3)
#elif (I2S0_LRCK_PIN_SEL == 7)
#define I2S0_LRCK_PIN_CFG       (PIN_D6 | PIN_FUNC5 | PIN_DRI3)
#elif (I2S0_LRCK_PIN_SEL == 8)
#define I2S0_LRCK_PIN_CFG       (PIN_D7 | PIN_FUNC3 | PIN_DRI3)
#elif (I2S0_LRCK_PIN_SEL == 9)
#define I2S0_LRCK_PIN_CFG       (PIN_D10 | PIN_FUNC6 | PIN_DRI3)
#endif

#if (I2S0_TX_PIN_SEL== 0)
#define I2S0_TX_PIN_CFG         (PIN_B5 | PIN_FUNC3 | PIN_DRI3)
#elif (I2S0_TX_PIN_SEL== 1)
#define I2S0_TX_PIN_CFG         (PIN_B10 | PIN_FUNC4 | PIN_DRI3)
#elif (I2S0_TX_PIN_SEL== 2)
#define I2S0_TX_PIN_CFG         (PIN_B17 | PIN_FUNC3 | PIN_DRI3)
#elif (I2S0_TX_PIN_SEL== 3)
#define I2S0_TX_PIN_CFG         (PIN_B23 | PIN_FUNC12 | PIN_DRI3)
#elif (I2S0_TX_PIN_SEL== 4)
#define I2S0_TX_PIN_CFG         (PIN_B29 | PIN_FUNC6 | PIN_DRI3)
#elif (I2S0_TX_PIN_SEL== 5)
#define I2S0_TX_PIN_CFG         (PIN_D1 | PIN_FUNC3 | PIN_DRI3)
#elif (I2S0_TX_PIN_SEL== 6)
#define I2S0_TX_PIN_CFG         (PIN_D3 | PIN_FUNC3 | PIN_DRI3)
#endif

#if (I2S0_RX_PIN_SEL == 0)
#define I2S0_RX_PIN_CFG         (PIN_B7 | PIN_FUNC3 | PIN_DRI3)
#elif (I2S0_RX_PIN_SEL == 1)
#define I2S0_RX_PIN_CFG         (PIN_B12 | PIN_FUNC4 | PIN_DRI3)
#elif (I2S0_RX_PIN_SEL == 2)
#define I2S0_RX_PIN_CFG         (PIN_B19 | PIN_FUNC3 | PIN_DRI3)
#elif (I2S0_RX_PIN_SEL == 3)
#define I2S0_RX_PIN_CFG         (PIN_B26 | PIN_FUNC6 | PIN_DRI3)
#elif (I2S0_RX_PIN_SEL == 4)
#define I2S0_RX_PIN_CFG         (PIN_D4 | PIN_FUNC3 | PIN_DRI3)
#elif (I2S0_RX_PIN_SEL == 5)
#define I2S0_RX_PIN_CFG         (PIN_B6 | PIN_FUNC7 | PIN_DRI3)
#endif

#if (I2S0_MCLK_PIN_SEL == 0)
#define I2S0_MCLK_PIN_CFG       (PIN_B4 | PIN_FUNC4 | PIN_DRI3)
#elif (I2S0_MCLK_PIN_SEL == 1)
#define I2S0_MCLK_PIN_CFG       (PIN_B6 | PIN_FUNC3 | PIN_DRI3)
#elif (I2S0_MCLK_PIN_SEL == 2)
#define I2S0_MCLK_PIN_CFG       (PIN_B7 | PIN_FUNC6 | PIN_DRI3)
#elif (I2S0_MCLK_PIN_SEL == 3)
#define I2S0_MCLK_PIN_CFG       (PIN_B9 | PIN_FUNC8 | PIN_DRI3)
#elif (I2S0_MCLK_PIN_SEL == 4)
#define I2S0_MCLK_PIN_CFG       (PIN_B11 | PIN_FUNC4 | PIN_DRI3)
#elif (I2S0_MCLK_PIN_SEL == 5)
#define I2S0_MCLK_PIN_CFG       (PIN_B18 | PIN_FUNC3 | PIN_DRI3)
#elif (I2S0_MCLK_PIN_SEL == 6)
#define I2S0_MCLK_PIN_CFG       (PIN_B25 | PIN_FUNC6 | PIN_DRI3)
#elif (I2S0_MCLK_PIN_SEL == 7)
#define I2S0_MCLK_PIN_CFG       (PIN_B30 | PIN_FUNC9 | PIN_DRI3)
#elif (I2S0_MCLK_PIN_SEL == 8)
#define I2S0_MCLK_PIN_CFG       (PIN_D4 | PIN_FUNC7 | PIN_DRI3)
#elif (I2S0_MCLK_PIN_SEL == 9)
#define I2S0_MCLK_PIN_CFG       (PIN_D6 | PIN_FUNC3 | PIN_DRI3)
#elif (I2S0_MCLK_PIN_SEL == 10)
#define I2S0_MCLK_PIN_CFG       (PIN_D9 | PIN_FUNC3 | PIN_DRI3)
#endif

void i2s_init(void)
{
#if I2S0_EN
#if (SYS_PLATFORM != PLATFORM_FPGA)
    HAL_clk_enable(AUDIO_PLL_CLK);
    module_ccu_enable(CCU_I2S);
    HAL_clk_enable(CLK_I2S);

    HAL_gpio_config(I2S0_BCLK_PIN_CFG);
    HAL_gpio_config(I2S0_LRCK_PIN_CFG);
#if I2S0_TX_EN
    HAL_gpio_config(I2S0_TX_PIN_CFG);
#endif
#if I2S0_RX_EN
    HAL_gpio_config(I2S0_RX_PIN_CFG);
#endif
#if I2S0_MCLK_EN
    HAL_gpio_config(I2S0_MCLK_PIN_CFG);
#endif
#endif

    hal_i2s_init_t i2s_initparams;
    if (I2S0_MCLK_FACTOR) {
        i2s_initparams.mclk_div = 24576000 / (I2S0_MCLK_FACTOR * 48000);
        i2s_initparams.mclk_en = 1;
    } else {
        i2s_initparams.mclk_div = 1;
        i2s_initparams.mclk_en = 0;
    }
    i2s_initparams.master = IS20_ROLE; //0:slave; 1:master
    i2s_initparams.format = I2S0_FORMAT; //0:I2S; 1:Left Justified; 2:Right Justified; 3:PCM long Frame; 4:PCM Short Frame
    i2s_initparams.slot_number = (1 << I2S0_SLOT_NUM);
    i2s_initparams.slot_width = (0 == I2S0_SLOT_WIDTH) ? 16 : 32;
    HAL_i2s_init(&i2s_initparams);
#if (I2S0_RX_PIN_SEL == 5)
    HAL_i2s_ctrl("rx map",0x7535); //I2S_D5 map RX CH0_CH1
#endif

    HAL_sndi2s_register();

#if (I2S0_USE_WM8978)
    twi_init();
#endif

#if I2S0_USE_WM8978
    WM8978_Init((1 == I2S0_USE_WM8978) ? TWI_ID_01 : TWI_ID_02);
#endif

#endif
}

void app_audio_i2s_config(void)
{
    i2s_init();
}
