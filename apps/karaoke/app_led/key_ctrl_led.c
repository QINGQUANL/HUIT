#include "key_ctrl_led.h"
#include "hal.h"
#include "sltk_mem.h"
#include "user_config.h"
#include "g24_api.h"
#include "mode_g24.h"


static uint8_t light_index = 0;;
int8_t g24_connect_status[2] = {0,0}; //0未连接 1连接
bool led1_left_flag = true;     //开机灯
bool led1_right_flag = false;
bool led2_left_flag = false;     //g24连接灯
bool led2_right_flag = false;
bool led3_left_flag = false;
bool led3_right_flag = false;
bool led4_left_flag = false;    //Mic降噪使能灯
bool led4_right_flag = false;
bool led5_left_flag = false;    //充电灯
bool led5_right_flag = false;

void key_led_disable_gpio()
{
    HAL_gpio_config(PIN_B1|PIN_FUNCDISABLE);
    HAL_gpio_config(PIN_B2|PIN_FUNCDISABLE);
    HAL_gpio_config(PIN_B3|PIN_FUNCDISABLE);
    HAL_gpio_config(PIN_B4|PIN_FUNCDISABLE);
}

AT_SRAM(.led_ctrl)
void key_led_timer_proc(void)
{
#if MODE_G24_EN
    static uint32_t g24_light_num = 0;
#endif
    if(HAL_pmu_is_charging()) {
        key_led_set_charge_status(true);
    } else {
        key_led_set_charge_status(false);
    }
    key_led_on();
#if MODE_G24_EN
    if(is_in_mode_g24()) {
        if ( key_led_get_g24_connect_status(0) ) {
            key_led_set_g24_light_status(0, true);
        } else if (g24_light_num > 250) {
            key_led_set_g24_light_status(0, false);
        } else {
            key_led_set_g24_light_status(0, true);
        }
#if G24_DUAL_CHANNEL
        if(G24_FUNC_ROLE == G24_FUNC_ROLE_DONGLE) {
            if ( key_led_get_g24_connect_status(1) ) {
                key_led_set_g24_light_status(1, true);
            } else if (g24_light_num > 250) {
                key_led_set_g24_light_status(1, false);
            } else {
                key_led_set_g24_light_status(1, true);
            }
        }
#endif
        g24_light_num++;
        g24_light_num %= 500;
    }
#endif
}

AT_SRAM(.led_ctrl)
void key_led_on()
{
    switch(light_index) {
        case led1_red:
            HAL_gpio_config(PIN_B2|PIN_FUNCDISABLE);
            HAL_gpio_config(PIN_B4|PIN_FUNCDISABLE);
            if (led1_left_flag) {
                HAL_gpio_config(PIN_B1|PIN_FUNCOUTPUT|PIN_HIGH);
                HAL_gpio_config(PIN_B3|PIN_FUNCOUTPUT|PIN_LOW);
            } else {
                HAL_gpio_config(PIN_B1|PIN_FUNCDISABLE);
                HAL_gpio_config(PIN_B3|PIN_FUNCDISABLE);
            }
            break;
        case led1_blue:
            HAL_gpio_config(PIN_B2|PIN_FUNCDISABLE);
            HAL_gpio_config(PIN_B3|PIN_FUNCDISABLE);
            if (led1_right_flag) {
                HAL_gpio_config(PIN_B1|PIN_FUNCOUTPUT|PIN_HIGH);
                HAL_gpio_config(PIN_B4|PIN_FUNCOUTPUT|PIN_LOW);
            } else {
                HAL_gpio_config(PIN_B1|PIN_FUNCDISABLE);
                HAL_gpio_config(PIN_B4|PIN_FUNCDISABLE);
            }
            break;
        case led3_red:
            HAL_gpio_config(PIN_B4|PIN_FUNCDISABLE);
            HAL_gpio_config(PIN_B3|PIN_FUNCDISABLE);
            if (led3_left_flag) {
                HAL_gpio_config(PIN_B2|PIN_FUNCOUTPUT|PIN_HIGH);
                HAL_gpio_config(PIN_B1|PIN_FUNCOUTPUT|PIN_LOW);
            } else {
                HAL_gpio_config(PIN_B2|PIN_FUNCDISABLE);
                HAL_gpio_config(PIN_B1|PIN_FUNCDISABLE);
            }
            break;
        case led2_red:
            HAL_gpio_config(PIN_B4|PIN_FUNCDISABLE);
            HAL_gpio_config(PIN_B1|PIN_FUNCDISABLE);
            if (led2_left_flag) {
                HAL_gpio_config(PIN_B2|PIN_FUNCOUTPUT|PIN_HIGH);
                HAL_gpio_config(PIN_B3|PIN_FUNCOUTPUT|PIN_LOW);
            } else {
                HAL_gpio_config(PIN_B2|PIN_FUNCDISABLE);
                HAL_gpio_config(PIN_B3|PIN_FUNCDISABLE);
            }
            break;
        case led2_blue:
            HAL_gpio_config(PIN_B1|PIN_FUNCDISABLE);
            HAL_gpio_config(PIN_B3|PIN_FUNCDISABLE);
            if (led2_right_flag) {
                HAL_gpio_config(PIN_B2|PIN_FUNCOUTPUT|PIN_HIGH);
                HAL_gpio_config(PIN_B4|PIN_FUNCOUTPUT|PIN_LOW);
            } else {
                HAL_gpio_config(PIN_B2|PIN_FUNCDISABLE);
                HAL_gpio_config(PIN_B4|PIN_FUNCDISABLE);
            }
            break;
        case led4_green:
            HAL_gpio_config(PIN_B2|PIN_FUNCDISABLE);
            HAL_gpio_config(PIN_B4|PIN_FUNCDISABLE);
            if (led4_left_flag) {
                HAL_gpio_config(PIN_B3|PIN_FUNCOUTPUT|PIN_HIGH);
                HAL_gpio_config(PIN_B1|PIN_FUNCOUTPUT|PIN_LOW);
            } else {
                HAL_gpio_config(PIN_B3|PIN_FUNCDISABLE);
                HAL_gpio_config(PIN_B1|PIN_FUNCDISABLE);
            }
             break;
        case led4_blue:
            HAL_gpio_config(PIN_B4|PIN_FUNCDISABLE);
            HAL_gpio_config(PIN_B1|PIN_FUNCDISABLE);
            if (led4_right_flag) {
                HAL_gpio_config(PIN_B3|PIN_FUNCOUTPUT|PIN_HIGH);
                HAL_gpio_config(PIN_B2|PIN_FUNCOUTPUT|PIN_LOW);
            } else {
                HAL_gpio_config(PIN_B3|PIN_FUNCDISABLE);
                HAL_gpio_config(PIN_B2|PIN_FUNCDISABLE);
            }
            break;
        case led3_blue:
            HAL_gpio_config(PIN_B2|PIN_FUNCDISABLE);
            HAL_gpio_config(PIN_B1|PIN_FUNCDISABLE);
            if (led3_right_flag) {
                HAL_gpio_config(PIN_B3|PIN_FUNCOUTPUT|PIN_HIGH);
                HAL_gpio_config(PIN_B4|PIN_FUNCOUTPUT|PIN_LOW);
            } else {
                HAL_gpio_config(PIN_B4|PIN_FUNCDISABLE);
                HAL_gpio_config(PIN_B3|PIN_FUNCDISABLE);
            }
            break;
        case led5_red:
            HAL_gpio_config(PIN_B2|PIN_FUNCDISABLE);
            HAL_gpio_config(PIN_B3|PIN_FUNCDISABLE);
            if (led5_left_flag) {
                HAL_gpio_config(PIN_B4|PIN_FUNCOUTPUT|PIN_HIGH);
                HAL_gpio_config(PIN_B1|PIN_FUNCOUTPUT|PIN_LOW);
            } else {
                HAL_gpio_config(PIN_B1|PIN_FUNCDISABLE);
                HAL_gpio_config(PIN_B4|PIN_FUNCDISABLE);
            }
            break;
        case led5_blue:
            HAL_gpio_config(PIN_B1|PIN_FUNCDISABLE);
            HAL_gpio_config(PIN_B3|PIN_FUNCDISABLE);
            if (led5_right_flag) {
                HAL_gpio_config(PIN_B4|PIN_FUNCOUTPUT|PIN_HIGH);
                HAL_gpio_config(PIN_B2|PIN_FUNCOUTPUT|PIN_LOW);
            } else {
                HAL_gpio_config(PIN_B4|PIN_FUNCDISABLE);
                HAL_gpio_config(PIN_B2|PIN_FUNCDISABLE);
            }
            break;
    }
    light_index++;
    light_index %= LED_INDEX_NUM;
}

void key_led_set_g24_connect_status(uint8_t link, int8_t light)
{
    g24_connect_status[link] = light;
}

AT_SRAM(.led_ctrl)
int8_t key_led_get_g24_connect_status(uint8_t link)
{
    return g24_connect_status[link];
}

AT_SRAM(.led_ctrl)
void key_led_set_g24_light_status(uint8_t link, bool light)
{
    if(link==0) {
        led2_left_flag = light;
    } else {
        led3_left_flag = light;
    }
}

AT_SRAM(.led_ctrl)
void key_led_set_charge_status(bool light)
{
    led5_left_flag = light;
}


void key_led_set_status(uint8_t led_index, bool light)
{
    switch (led_index) {
        case led1_blue:
            led1_right_flag = light;
            break;
        case led2_blue:
            led2_right_flag  = light;
            break;
        case led3_red:
            led3_left_flag = light;
            break;
        case led3_blue:
            led3_right_flag = light;
            break;
        case led4_green:
            led4_left_flag = light;
            break;
        case led4_blue:
            led4_right_flag = light;
            break;
        case led5_red:
            led5_left_flag = light;
            break;
        case led5_blue:
            led5_right_flag = light;
            break;
        default:
            break;
    }
}
