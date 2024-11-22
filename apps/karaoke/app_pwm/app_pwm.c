#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "hal_clk.h"
#include "hal_gpio.h"
#include "hal_pwm.h"
#include "pwm.h"
#include "app_pwm.h"


#if LED_PWM_EN

void app_pwm_init(void)
{
    pwm_led_ctrl_t led_ctrl;
    led_ctrl.thr_max = 48000;
    led_ctrl.thr_min = 0;
    led_ctrl.conti_period = 3;
    led_ctrl.inc_act = 200;
    led_ctrl.dark = 0;
    led_ctrl.bright = 0;
    led_ctrl.mode = PWM_LED_INC_MODE;
    led_ctrl.en = DISABLE;
#if PWM_CH0_EN
    HAL_gpio_config(PWM_CH0_PIN_CFG);
    pwm_init(PWM_CH0, &led_ctrl, PWM_MODE_CYCLE, PWM_ACTIVE_HIGH, PWM_PRESCAL_30, PWM_CH_EN, 0);
#endif

#if PWM_CH1_EN
    HAL_gpio_config(PWM_CH1_PIN_CFG);
    pwm_init(PWM_CH1, &led_ctrl, PWM_MODE_CYCLE, PWM_ACTIVE_HIGH, PWM_PRESCAL_30, PWM_CH_EN,0);
#endif

#if PWM_CH2_EN
    HAL_gpio_config(PWM_CH2_PIN_CFG);
    pwm_init(PWM_CH2, &led_ctrl, PWM_MODE_CYCLE, PWM_ACTIVE_HIGH, PWM_PRESCAL_30, PWM_CH_EN,0);
#endif

#if PWM_CH3_EN
    HAL_gpio_config(PWM_CH3_PIN_CFG);
    pwm_init(PWM_CH3, &led_ctrl, PWM_MODE_CYCLE, PWM_ACTIVE_HIGH, PWM_PRESCAL_30, PWM_CH_EN,0);
#endif

#if PWM_CH4_EN
    HAL_gpio_config(PWM_CH4_PIN_CFG);
    pwm_init(PWM_CH4, &led_ctrl, PWM_MODE_CYCLE, PWM_ACTIVE_HIGH, PWM_PRESCAL_30, PWM_CH_EN,0);
#endif

    HAL_pwm_irq_callback(pwm_irq_cb);
}

AT_SRAM(.pwm)
void pwm_irq_cb(uint32_t int_sta)
{
#if PWM_CH0_EN
    if(int_sta & PWM_CAP0_RL_INT_STA) {
    }
    if(int_sta & PWM_CAP0_FL_INT_STA) {
    }
#endif
#if PWM_CH1_EN
    if(int_sta & PWM_CAP1_RL_INT_STA) {
    }
    if(int_sta & PWM_CAP1_FL_INT_STA) {
    }
#endif
#if PWM_CH2_EN
    if(int_sta & PWM_CAP2_RL_INT_STA) {
    }
    if(int_sta & PWM_CAP2_FL_INT_STA) {
    }
#endif
#if PWM_CH3_EN
    if(int_sta & PWM_CAP3_RL_INT_STA) {
    }
    if(int_sta & PWM_CAP3_FL_INT_STA) {
    }
#endif
#if PWM_CH4_EN
    if(int_sta & PWM_CAP4_RL_INT_STA) {
    }
    if(int_sta & PWM_CAP4_FL_INT_STA) {
    }
#endif
#if PWM_CH5_EN
    if(int_sta & PWM_CAP5_RL_INT_STA) {
    }
    if(int_sta & PWM_CAP5_FL_INT_STA) {
    }
#endif
}



uint32_t pwm_cnt = 0;
bool app_pwm_start = 0;

AT_SRAM(.pwm)
void app_pwm_timer_cb(void)
{
    pwm_led_ctrl_t led_ctrl;


    if (app_pwm_start == 1) {

    if (pwm_cnt == 0) {
        led_ctrl.en = DISABLE;
        HAL_gpio_config(PWM_CH3_PIN_CFG);

        pwm_init(PWM_CH3, &led_ctrl, PWM_MODE_CYCLE, PWM_ACTIVE_HIGH, PWM_PRESCAL_30, PWM_CH_EN,0);
        HAL_pwm_config(PWM_B_LED_CH, 800, 0);
        led_ctrl.thr_max = 400; //use thr_max as breath mode "O" area active cycles
        led_ctrl.thr_min = 0;     //use thr_min as breath mode "P" area active cycles
        led_ctrl.dark = 0;
        led_ctrl.bright = 0;
        led_ctrl.conti_period = 9;
        led_ctrl.inc_act = 8;
        led_ctrl.mode = PWM_LED_TRI_MODE;
        led_ctrl.en = 1;
        HAL_pwm_led_mode_config(PWM_B_LED_CH, &led_ctrl);
    } else if (pwm_cnt == 1){
        HAL_gpio_config(PWM_CH2_PIN_CFG);
        led_ctrl.en = DISABLE;
        pwm_init(PWM_G_LED_CH, &led_ctrl, PWM_MODE_CYCLE, PWM_ACTIVE_HIGH, PWM_PRESCAL_30, PWM_CH_DIS,0);
        HAL_pwm_config(PWM_G_LED_CH, 800, 0);
        HAL_pwm_enable(PWM_G_LED_CH,1);
        led_ctrl.thr_max = 400; //use thr_max as breath mode "O" area active cycles
        led_ctrl.thr_min = 0;     //use thr_min as breath mode "P" area active cycles
        led_ctrl.dark = 0;
        led_ctrl.bright = 0;
        led_ctrl.conti_period = 9;
        led_ctrl.inc_act = 8;
        led_ctrl.mode = PWM_LED_TRI_MODE;
        led_ctrl.en = 1;
        HAL_pwm_led_mode_config(PWM_G_LED_CH, &led_ctrl);
    } else if(pwm_cnt == 2) {
        HAL_gpio_config(PWM_CH5_PIN_CFG);
        led_ctrl.en = DISABLE;
        pwm_init(PWM_R_LED_CH, &led_ctrl, PWM_MODE_CYCLE, PWM_ACTIVE_HIGH, PWM_PRESCAL_30, PWM_CH_DIS,0);
        HAL_pwm_config(PWM_R_LED_CH, 800, 0);
        HAL_pwm_enable(PWM_R_LED_CH,1);
        led_ctrl.thr_max = 400; //use thr_max as breath mode "O" area active cycles
        led_ctrl.thr_min = 0;     //use thr_min as breath mode "P" area active cycles
        led_ctrl.dark = 0;
        led_ctrl.bright = 0;
        led_ctrl.conti_period = 9;
        led_ctrl.inc_act = 8;
        led_ctrl.mode = PWM_LED_TRI_MODE;
        led_ctrl.en = 1;
        HAL_pwm_led_mode_config(PWM_R_LED_CH, &led_ctrl);
        HAL_pwm_led_enable(PWM_B_LED_CH,0);
        HAL_pwm_enable(PWM_B_LED_CH,0);
    } else if (pwm_cnt == 3) {
        HAL_pwm_led_enable(PWM_G_LED_CH,0);
        HAL_pwm_enable(PWM_G_LED_CH,0);
    } else if (pwm_cnt == 4) {
        HAL_pwm_led_enable(PWM_R_LED_CH,0);
        HAL_pwm_enable(PWM_R_LED_CH,0);
    }
    pwm_cnt++;
    pwm_cnt %= 10;
   }

}
void pwm_breath_start(void)
{
    logi("pwm_breath_start");
    pwm_cnt = 0;
    app_pwm_start = 1;
}


void pwm_breath_init(void)
{
    logi("pwm_breath_init");
    pwm_led_ctrl_t led_ctrl;
    pwm_cnt = 0;
    app_pwm_start = 0;
    pwm_init(PWM_B_LED_CH, &led_ctrl, PWM_MODE_CYCLE, PWM_ACTIVE_HIGH, PWM_PRESCAL_30, PWM_CH_DIS,0);
    pwm_init(PWM_G_LED_CH, &led_ctrl, PWM_MODE_CYCLE, PWM_ACTIVE_HIGH, PWM_PRESCAL_30, PWM_CH_DIS,0);
    pwm_init(PWM_R_LED_CH, &led_ctrl, PWM_MODE_CYCLE, PWM_ACTIVE_HIGH, PWM_PRESCAL_30, PWM_CH_DIS,0);
}

void pwm_breath_stop(void)
{
    if (app_pwm_start) {
        logi("pwm_breath_stop");
        app_pwm_start = 0;
        HAL_pwm_led_enable(PWM_B_LED_CH,0);
        HAL_pwm_enable(PWM_B_LED_CH,0);
        HAL_pwm_led_enable(PWM_R_LED_CH,0);
        HAL_pwm_enable(PWM_R_LED_CH,0);
        HAL_pwm_led_enable(PWM_G_LED_CH,0);
        HAL_pwm_enable(PWM_G_LED_CH,0);
    }
}


bool fliker_en = 0;

void pwm_fliker_init(void)
{
    logi("pwm_fliker_init");
    pwm_led_ctrl_t led_ctrl;
    fliker_en = 0;
    pwm_init(PWM_B_LED_CH, &led_ctrl, PWM_MODE_CYCLE, PWM_ACTIVE_HIGH, PWM_PRESCAL_1, PWM_CH_DIS,1);
}

void pwm_fliker_start(uint32_t ch, uint16_t dark,uint16_t bright)
{
            logi("##pwm_fliker_start");
            pwm_led_ctrl_t led_ctrl;
            led_ctrl.en = DISABLE;
            HAL_gpio_config(PWM_CH3_PIN_CFG);
            pwm_init(ch, &led_ctrl, PWM_MODE_CYCLE, PWM_ACTIVE_HIGH, PWM_PRESCAL_1, PWM_CH_EN,1);
            HAL_pwm_config(ch, 52, 0);
            led_ctrl.thr_max = 26; //use thr_max as breath mode "O" area active cycles
            led_ctrl.thr_min = 0;     //use thr_min as breath mode "P" area active cycles
            led_ctrl.dark = dark;
            led_ctrl.bright = bright;
            led_ctrl.mode = PWM_LED_FLI_MODE;
            led_ctrl.en = 1;
            HAL_pwm_led_mode_config(ch, &led_ctrl);
            fliker_en = 1;

}

void pwm_fliker_stop(uint32_t ch)
{
        if  (fliker_en) {
            logi("##pwm_fliker_stop");
            HAL_pwm_led_enable(ch,0);
            HAL_pwm_enable(ch,0);
            HAL_clk_disable(CLK_PWM);
            module_ccu_disable(CCU_PWM);
            fliker_en = 0;
            HAL_gpio_config(PWM_CH3_PIN_CFG);
       }

}


#define PWM_TEST_EN 0

#if PWM_TEST_EN
uint32_t pwm_cap_status = 0;

AT_SRAM(.pwm)
void pwm_test_irq_cb(uint32_t int_sta)
{
    logi("pwm_test_irq_cb.");
    pwm_cap_status |= int_sta;
}

void pwm_test(uint8_t ch, uint8_t test_mode)
{
    pwm_param_t pwm_param;
    pwm_led_ctrl_t led_ctrl;
    __maybe_unused uint32_t RL_val=0,FL_val=0;
    logi("pwm test:%d",test_mode);

    led_ctrl.thr_max = 48000;
    led_ctrl.thr_min = 0;
    led_ctrl.conti_period = 3;
    led_ctrl.inc_act = 200;
    led_ctrl.dark = 0;
    led_ctrl.bright = 0;
    led_ctrl.mode = PWM_LED_INC_MODE;
    led_ctrl.en = ENABLE;

    HAL_pwm_config(ch, 48000, 24000);
    
    switch(test_mode) {
    case PWM_PULSE_TEST_MODE:
        pwm_param.en = PWM_CH_EN;
        pwm_param.mode = PWM_MODE_PULSE;
        pwm_param.active_state = PWM_ACTIVE_HIGH;
        pwm_param.prescal= PWM_PRESCAL_1;
        HAL_pwm_module_init(ch, &pwm_param);
        while(1) {
            HAL_mdelay(1000);
            HAL_pwm_pulse_start(ch);
            HAL_pwm_pulse_finish_wait(ch);
        }
        break;

    case PWM_LED_INC_TEST_MODE:     //Increment mode
        led_ctrl.mode = PWM_LED_INC_MODE;
        HAL_pwm_led_mode_config(ch, &led_ctrl);
        break;

    case PWM_LED_DEC_TEST_MODE:     //Decrement mode
        led_ctrl.mode = PWM_LED_DEC_MODE;
        HAL_pwm_led_mode_config(ch, &led_ctrl);
        break;

    case PWM_LED_TRI_TEST_MODE:     //Triangle mode
        led_ctrl.mode = PWM_LED_TRI_MODE;
        HAL_pwm_led_mode_config(ch, &led_ctrl);
        break;

    case PWM_LED_FLICK_TEST_MODE:   //Flick mode
        led_ctrl.thr_max = 1200;  //use thr_max as flick mode "O" area active cycles
        led_ctrl.thr_min = 0;     //use thr_min as flick mode "P" area active cycles
        led_ctrl.inc_act = 0;
        led_ctrl.dark = 500;
        led_ctrl.bright = 500;
        led_ctrl.mode = PWM_LED_FLI_MODE;
        HAL_pwm_led_mode_config(ch, &led_ctrl);
        break;

    case PWM_LED_BREATH_TEST_MODE:  //Breath mode
        led_ctrl.thr_max = 36000; //use thr_max as breath mode "O" area active cycles
        led_ctrl.thr_min = 0;     //use thr_min as breath mode "P" area active cycles
        led_ctrl.dark = 1000;
        led_ctrl.bright = 1000;
        led_ctrl.mode = PWM_LED_BRE_MODE;
        HAL_pwm_led_mode_config(ch, &led_ctrl);
        break;

    case PWM_CAP_TEST_MODE:
#if 1//(SYS_PLATFORM == PLATFORM_FPGA)
        pwm_param.en = PWM_CH_EN;
        pwm_param.mode = PWM_MODE_CYCLE;
        pwm_param.active_state = PWM_ACTIVE_HIGH;
        pwm_param.prescal= PWM_PRESCAL_1;       //for fpga
        HAL_pwm_module_init(ch, &pwm_param);
#endif
        HAL_pwm_irq_callback(pwm_test_irq_cb);
        HAL_pwm_cap_config(ch,1,1,0,1);
        HAL_pwm_interrupt_config(ch,PWM_CAP_RL_INT|PWM_CAP_FL_INT,1);
        while(1) {
            RL_val = 0;
            FL_val = 0;
            if(pwm_cap_status & (PWM_CAP0_RL_INT_STA<<ch)) {   //CAPx IRQ status clear
                pwm_cap_status &= ~(PWM_CAP0_RL_INT_STA<<ch);
                RL_val = HAL_pwm_cap_lock_get(ch,PWM_CAP_RL);
            }
            if(pwm_cap_status & (PWM_CAP0_FL_INT_STA<<ch)) {
                pwm_cap_status &= ~(PWM_CAP0_FL_INT_STA<<ch);
                FL_val = HAL_pwm_cap_lock_get(ch,PWM_CAP_FL);
            }
            if(RL_val || FL_val) {
                logi("CH%d RL_val:%d,FL_val:%d",ch,RL_val,FL_val);
            }
        }
        
        break;
    }
}

#endif


#else
void app_pwm_init(void)
{
}

#endif

