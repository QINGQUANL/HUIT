#ifndef _APP_PWM_H_
#define _APP_PWM_H_
#include "sys_inc.h"
#include "user_config.h"


#if (PWM_CH0_PIN_SEL == 0)
#define PWM_CH0_PIN_CFG     (PIN_B0 | PIN_FUNC2)    //for fpga
#elif (PWM_CH0_PIN_SEL == 1)
#define PWM_CH0_PIN_CFG     (PIN_A4 | PIN_FUNC4)
#elif (PWM_CH0_PIN_SEL == 2)
#define PWM_CH0_PIN_CFG     (PIN_B4 | PIN_FUNC2)
#elif (PWM_CH0_PIN_SEL == 3)
#define PWM_CH0_PIN_CFG     (PIN_B19 | PIN_FUNC2)
#elif (PWM_CH0_PIN_SEL == 4)
#define PWM_CH0_PIN_CFG     (PIN_B25 | PIN_FUNC10)
#elif (PWM_CH0_PIN_SEL == 5)
#define PWM_CH0_PIN_CFG     (PIN_D0 | PIN_FUNC5)
#elif (PWM_CH0_PIN_SEL == 6)
#define PWM_CH0_PIN_CFG     (PIN_D11 | PIN_FUNC10)
#endif

#if (PWM_CH1_PIN_SEL == 0)
#define PWM_CH1_PIN_CFG     (PIN_B1 | PIN_FUNC2)    //for fpga
#elif (PWM_CH1_PIN_SEL == 1)
#define PWM_CH1_PIN_CFG     (PIN_A5 | PIN_FUNC4)
#elif (PWM_CH1_PIN_SEL == 2)
#define PWM_CH1_PIN_CFG     (PIN_B7 | PIN_FUNC2)
#elif (PWM_CH1_PIN_SEL == 3)
#define PWM_CH1_PIN_CFG     (PIN_B14 | PIN_FUNC5)
#elif (PWM_CH1_PIN_SEL == 4)
#define PWM_CH1_PIN_CFG     (PIN_B20 | PIN_FUNC2)
#elif (PWM_CH1_PIN_SEL == 5)
#define PWM_CH1_PIN_CFG     (PIN_B24 | PIN_FUNC11)
#elif (PWM_CH1_PIN_SEL == 6)
#define PWM_CH1_PIN_CFG     (PIN_D1 | PIN_FUNC10)
#endif

#if (PWM_CH2_PIN_SEL == 0)
#define PWM_CH2_PIN_CFG     (PIN_B2 | PIN_FUNC2)    //for fpga
#elif (PWM_CH2_PIN_SEL == 1)
#define PWM_CH2_PIN_CFG     (PIN_B2 | PIN_FUNC10)
#elif (PWM_CH2_PIN_SEL == 2)
#define PWM_CH2_PIN_CFG     (PIN_B8 | PIN_FUNC2)
#elif (PWM_CH2_PIN_SEL == 3)
#define PWM_CH2_PIN_CFG     (PIN_B21 | PIN_FUNC2)
#elif (PWM_CH2_PIN_SEL == 4)
#define PWM_CH2_PIN_CFG     (PIN_B22 | PIN_FUNC5)
#elif (PWM_CH2_PIN_SEL == 5)
#define PWM_CH2_PIN_CFG     (PIN_B27 | PIN_FUNC12)
#elif (PWM_CH2_PIN_SEL == 6)
#define PWM_CH2_PIN_CFG     (PIN_D5 | PIN_FUNC7)
#endif

#if (PWM_CH3_PIN_SEL == 0)
#define PWM_CH3_PIN_CFG     (PIN_B3 | PIN_FUNC2)    //for fpga
#elif (PWM_CH3_PIN_SEL == 1)
#define PWM_CH3_PIN_CFG     (PIN_B9 | PIN_FUNC2)
#elif (PWM_CH3_PIN_SEL == 2)
#define PWM_CH3_PIN_CFG     (PIN_B17 | PIN_FUNC5)
#elif (PWM_CH3_PIN_SEL == 3)
#define PWM_CH3_PIN_CFG     (PIN_B23 | PIN_FUNC5)
#elif (PWM_CH3_PIN_SEL == 4)
#define PWM_CH3_PIN_CFG     (PIN_B28 | PIN_FUNC12)
#elif (PWM_CH3_PIN_SEL == 5)
#define PWM_CH3_PIN_CFG     (PIN_D6 | PIN_FUNC10)
#elif (PWM_CH3_PIN_SEL == 6)
#define PWM_CH3_PIN_CFG     (PIN_D8 | PIN_FUNC9)
#endif

#if (PWM_CH4_PIN_SEL == 0)
#define PWM_CH4_PIN_CFG     (PIN_B0 | PIN_FUNC3)    //for fpga
#elif (PWM_CH4_PIN_SEL == 1)
#define PWM_CH4_PIN_CFG     (PIN_B6 | PIN_FUNC2)
#elif (PWM_CH4_PIN_SEL == 2)
#define PWM_CH4_PIN_CFG     (PIN_B15 | PIN_FUNC8)
#elif (PWM_CH4_PIN_SEL == 3)
#define PWM_CH4_PIN_CFG     (PIN_B29 | PIN_FUNC12)
#elif (PWM_CH4_PIN_SEL == 4)
#define PWM_CH4_PIN_CFG     (PIN_C2 | PIN_FUNC12)
#elif (PWM_CH4_PIN_SEL == 5)
#define PWM_CH4_PIN_CFG     (PIN_D3 | PIN_FUNC10)
#elif (PWM_CH4_PIN_SEL == 6)
#define PWM_CH4_PIN_CFG     (PIN_D9 | PIN_FUNC9)
#endif

#if (PWM_CH5_PIN_SEL == 0)
#define PWM_CH5_PIN_CFG     (PIN_B1 | PIN_FUNC3)    //for fpga
#elif (PWM_CH5_PIN_SEL == 1)
#define PWM_CH5_PIN_CFG     (PIN_B11 | PIN_FUNC5)
#elif (PWM_CH5_PIN_SEL == 2)
#define PWM_CH5_PIN_CFG     (PIN_B18 | PIN_FUNC5)
#elif (PWM_CH5_PIN_SEL == 3)
#define PWM_CH5_PIN_CFG     (PIN_B30 | PIN_FUNC5)
#elif (PWM_CH5_PIN_SEL == 4)
#define PWM_CH5_PIN_CFG     (PIN_C2 | PIN_FUNC13)
#elif (PWM_CH5_PIN_SEL == 5)
#define PWM_CH5_PIN_CFG     (PIN_D2 | PIN_FUNC10)
#elif (PWM_CH5_PIN_SEL == 6)
#define PWM_CH5_PIN_CFG     (PIN_D10 | PIN_FUNC5)
#endif

enum pwm_test_mode_e {
    PWM_PULSE_TEST_MODE,
    PWM_LED_INC_TEST_MODE,
    PWM_LED_DEC_TEST_MODE,
    PWM_LED_TRI_TEST_MODE,
    PWM_LED_FLICK_TEST_MODE,
    PWM_LED_BREATH_TEST_MODE,
    PWM_CAP_TEST_MODE,
};

void app_pwm_init(void);
void pwm_irq_cb(uint32_t int_sta);
void pwm_test(uint8_t ch, uint8_t test_mode);
void app_pwm_config(uint8_t ch,bool en, uint8_t mode,uint16_t entire_cycle, uint16_t active_cycle);
void app_pwm_deinit(void);
void app_pwm_timer_cb(void);
void pwm_breath_start(void);
void pwm_breath_stop(void);
void pwm_breath_init(void);
void pwm_fliker_start(uint32_t ch,uint16_t dark,uint16_t bright);
void pwm_fliker_stop(uint32_t ch);
void pwm_fliker_init(void);

#endif

