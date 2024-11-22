#ifndef _LED_DIGITAL_H_
#define _LED_DIGITAL_H_

#if LED_DIGITAL_EN
void disp_led_digital_init(void);
void disp_led_digital_num(uint32_t num, uint32_t col, uint32_t led1);
void disp_led_digital_num_h(uint32_t num);
void disp_led_digital_num_l(uint32_t num);
void disp_menu_led_digital(uint32_t menu, uint32_t val0, uint32_t val1);
void disp_sleep_digital(bool state);
void disp_bat_low_digital(bool state);
void *get_led_buf(void);

#else
#define disp_led_digital_init()
#define disp_led_digital_num(num, col, led1)
#define disp_led_digital_num_h(num)
#define disp_led_digital_num_l(num)
#define disp_menu_led_digital(menu, val0, val1)
#define disp_sleep_digital(state)
#define disp_bat_low_digital(state)
void *get_led_buf(void);

#endif
#endif
