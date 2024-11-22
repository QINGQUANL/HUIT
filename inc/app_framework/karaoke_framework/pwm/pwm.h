#ifndef _PWM_H_
#define _PWM_H_
#include "hal.h"

void pwm_init(uint8_t ch, pwm_led_ctrl_t *led_ctrl, uint8_t mode_cycle, uint8_t active_sta, uint8_t prescal, uint8_t en, bool is_losc_clk_sel);

#endif

