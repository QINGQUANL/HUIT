#ifndef _LED_DIODE_H_
#define _LED_DIODE_H_

enum disp_led_id_e {
    LED_RED,
    LED_BLUE,
    LED_GREEN,
};




typedef struct {
    uint8_t red_sta;
    uint8_t red_sta_last;
    uint16_t red_cnt;
    uint16_t red_flash_on_time;
    uint16_t red_flash_off_time;
    uint16_t red_flash_times;
    uint32_t red_flash_round;
    uint32_t red_flash_count;

    uint8_t blue_sta;
    uint8_t blue_sta_last;
    uint16_t blue_cnt;
    uint16_t blue_flash_on_time;
    uint16_t blue_flash_off_time;
    uint32_t blue_flash_round;
    uint32_t blue_flash_count;

    uint8_t green_sta;
    uint8_t green_sta_last;
    uint16_t green_cnt;
    uint16_t green_flash_on_time;
    uint16_t green_flash_off_time;
    uint32_t green_flash_round;
    uint32_t green_flash_count;

    uint8_t current_id;
} led_diode_t;



void led_diode_flash(enum disp_led_id_e led, bool enable);
void disp_led_diode_timer(void);
void disp_led_diode_init(void);
void disp_led_diode(enum disp_led_id_e id, uint8_t onoff);
void disp_led_flash_set(enum disp_led_id_e id, uint16_t flash_on_time, uint16_t flash_off_time, uint32_t round_time);

void disp_led_interval_set(enum disp_led_id_e id, uint16_t flash_on_time);
void disp_led_delayoff_set(enum disp_led_id_e id, uint16_t flash_on_time);
void disp_led_onoff_set(enum disp_led_id_e id, bool onoff);
void disp_led_diode_deinit(void);
void disp_led_loop_set(enum disp_led_id_e id, uint16_t flash_on_time);


#endif
