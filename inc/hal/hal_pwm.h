#ifndef _HAL_PWM_LIB_H_
#define _HAL_PWM_LIB_H_

#define PWM_GROUP_NUM   3
#define PWM_CH_NUM      6

enum pwm_ch_id_e {
    PWM_CH0 = 0x0,
    PWM_CH1,
    PWM_CH2,
    PWM_CH3,
    PWM_CH4,
    PWM_CH5,
};

enum pwm_led_mode_e {
    PWM_LED_INC_MODE = 0x0,
    PWM_LED_DEC_MODE,
    PWM_LED_TRI_MODE,
    PWM_LED_FLI_MODE,
    PWM_LED_BRE_MODE,
    PWM_LED_RES_MODE,
};

enum pwm_cap_lock_type_e {
    PWM_CAP_RL      = 0,
    PWM_CAP_FL      = 1,
};

enum pwm_cap_int_type_e {
    PWM_CAP_RL_INT      = (1<<0),
    PWM_CAP_FL_INT      = (1<<8),
};

enum pwm_mode_e {
    PWM_MODE_CYCLE  = 0x0,
    PWM_MODE_PULSE,
};

enum pwm_active_state_e {
    PWM_ACTIVE_LOW  = 0x0,
    PWM_ACTIVE_HIGH,
};

enum pwm_ch_en_e {
    PWM_CH_DIS      = 0x0,
    PWM_CH_EN,
};

/*
 * 一级分频系数:
 * 24MHz时钟源经分频后得到一个更低时钟源，周期为t。
 */
enum pwm_prescal_e {
    PWM_PRESCAL_120 = 0x0, //24MHz/120 = 200KHz (t=5us)
    PWM_PRESCAL_180 = 0x1, //24MHz/180 = 133KHz (t=7.5us)
    PWM_PRESCAL_240 = 0x2, //24MHz/240 = 100KHz (t=10us)
    PWM_PRESCAL_360 = 0x3, //24MHz/360 = 66KHz (t=15us)
    PWM_PRESCAL_480 = 0x4, //24MHz/480 = 50KHz (t=20us)
    PWM_PRESCAL_600 = 0x5, //24MHz/600 = 40KHz (t=25us)
    PWM_PRESCAL_720 = 0x6, //24MHz/720 = 33KHz (t=30us)
    PWM_PRESCAL_840 = 0x7, //24MHz/840 = 28KHz (t=35us)
    PWM_PRESCAL_12K = 0x8, //24MHz/12K = 2KHz (t=0.5ms)
    PWM_PRESCAL_24K = 0x9, //24MHz/24K = 1KHz (t=1ms)
    PWM_PRESCAL_36K = 0xA, //24MHz/36K = 666Hz (t=1.5ms)
    PWM_PRESCAL_48K = 0xB, //24MHz/48K = 500Hz (t=2ms)
    PWM_PRESCAL_72K = 0xC, //24MHz/72K = 333Hz (t=3ms)
    PWM_PRESCAL_60  = 0xD,  //24MHz/60 = 400K (t=2.5us)
    PWM_PRESCAL_30  = 0xE, //24MHz/30 = 800K (t=1.25us)
    PWM_PRESCAL_1   = 0xF, //24MHz/1 = 24MHzHz (t=0.04us)
    /* on fpga,the prescal is /1 */
};

enum pwm_int_sta_e {
    PWM_CAP0_RL_INT_STA      = (0x1<<0),
    PWM_CAP1_RL_INT_STA      = (0x1<<1),
    PWM_CAP2_RL_INT_STA      = (0x1<<2),
    PWM_CAP3_RL_INT_STA      = (0x1<<3),
    PWM_CAP4_RL_INT_STA      = (0x1<<4),
    PWM_CAP5_RL_INT_STA      = (0x1<<5),
    PWM_CAP0_FL_INT_STA      = (0x1<<8),
    PWM_CAP1_FL_INT_STA      = (0x1<<9),
    PWM_CAP2_FL_INT_STA      = (0x1<<10),
    PWM_CAP3_FL_INT_STA      = (0x1<<11),
    PWM_CAP4_FL_INT_STA      = (0x1<<12),
    PWM_CAP5_FL_INT_STA      = (0x1<<13),
    PWM_CAP_INT_STA_MASK     = (0x3f3f),
};

typedef struct {
    uint16_t thr_max;           //active cycles change between thr_min and thr_max
    uint16_t thr_min;
    uint16_t conti_period;      //the same inc_act keep (conti_period+1)*Te cycles
    uint16_t inc_act;           //active cycles add/reduce inc_act cycles
    uint16_t dark;              //breath/flick mode dark period cycles are (dark+1)*Te cycles
    uint16_t bright;            //breath/flick mode bright period cycles are (bright+1)*Te cycles
    uint8_t mode;               //LED MODE,see enum pwm_led_mode_e
    bool en;                    //LED en
} pwm_led_ctrl_t;

typedef struct {
    uint8_t en;
    uint8_t mode;
    uint8_t active_state;
    uint8_t prescal;
} pwm_param_t;


void HAL_pwm_module_init(uint8_t ch_num, pwm_param_t *pwm_param, bool is_losc_clk_sel);
/*
 * 周期：(entire_cycle + 1) * t
 * active state时间：(active_cycle + 1) * t
 * entire_cycle必须比active_cycle大
 */
void HAL_pwm_config(uint8_t ch_num, uint16_t entire_cycle, uint16_t active_cycle);
void HAL_pwm_pulse_start(uint8_t ch_num);
void HAL_pwm_pulse_finish_wait(uint8_t ch_num);
void HAL_pwm_group_com_ctrl_config(uint8_t group_num, uint8_t dz_val, bool dz_en);
void HAL_pwm_irq_callback(void(*pwm_cb)(uint32_t int_sta));
void HAL_pwm_interrupt_config(uint8_t ch_num, uint32_t int_tpye, bool en);
void HAL_pwm_cap_config(uint8_t ch_num,bool FL_en, bool RL_en, bool INV_en,bool cap_en);
uint32_t HAL_pwm_cap_lock_get(uint8_t ch_num, bool lock_type);
void HAL_pwm_led_mode_config(uint8_t ch_num, pwm_led_ctrl_t *led_ctrl);
void HAL_pwm_enable(uint8_t ch_num,bool enable);
void HAL_pwm_led_enable(uint8_t ch_num,bool enable);


#endif
