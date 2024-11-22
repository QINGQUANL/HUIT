#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "karaoke_framework.h"
#include "disp.h"
#include "app_pmu.h"

#if LED_DIGITAL_EN
bool disp_sleep_flag = false;
uint8_t led_buf[LED_COM_NUM];
/*
    led_buf[0] - other segment;
    led_buf[1] - the 1st '8';
    led_buf[2] - the 2nd '8';
    led_buf[3] - the 3rd '8';
    led_buf[4] - the 4th '8';
*/

void *get_led_buf(void)
{
    return (void*)led_buf;
}

AT_SRAM(.disp_rodata)
const uint32_t led_num_tbl[10] =
{
    LED_0, LED_1, LED_2, LED_3, LED_4,LED_5, LED_6, LED_7, LED_8, LED_9,
};

void disp_led_digital_init(void)
{
    memset(led_buf, 0, LED_COM_NUM);
    ledc_init();
}

AT_SRAM(.disp)
void disp_led_digital_num(uint32_t num, uint32_t col, uint32_t led1)
{
    memset(led_buf, 0, LED_COM_NUM);
    if (pmu_bat_is_low()) {
        led_buf[0] &= ~LED_SEG_BAT0;
        led_buf[0] |= LED_SEG_BAT1;
    } else {
        led_buf[0] |= LED_SEG_BAT0;
        led_buf[0] &= ~LED_SEG_BAT1;
    }

    if (disp_sleep_flag == true) {
        led_buf[0] |= LED_SEG_BT;
    } else {
        led_buf[0] &= ~LED_SEG_BT;
    }

    if (col) {
        led_buf[0] |= LED_SEG_COL;
    } else {
        led_buf[0] &= ~LED_SEG_COL;
    }

    if (num > 9999) {
        num = 9999;
    }

    led_buf[1] = led_num_tbl[num / 1000];
    led_buf[2] = led_num_tbl[(num % 1000) / 100];
    led_buf[3] = led_num_tbl[(num % 100) / 10];
    led_buf[4] = led_num_tbl[num % 10];

    if (led1 == 0) {
        if (led_buf[1] == LED_0) {      //if led1 num is '0', don't display
            led_buf[1] = LED_OFF;
        }
    }
}

AT_SRAM(.disp)
void disp_led_digital_num_h(uint32_t num)
{
    if (num > 99) {
        num = 99;
    }
    led_buf[1] = led_num_tbl[num/10];
    led_buf[2] = led_num_tbl[num%10];
}

AT_SRAM(.disp)
void disp_led_digital_num_l(uint32_t num)
{
    if (num > 99) {
        num = 99;
    }
    led_buf[3] = led_num_tbl[num/10];
    led_buf[4] = led_num_tbl[num%10];
}

AT_SRAM(.disp)
void disp_menu_led_digital(uint32_t menu, uint32_t val0, uint32_t val1)
{
    memset(led_buf, 0, LED_COM_NUM);
    if (pmu_bat_is_low()) {
        led_buf[0] &= ~LED_SEG_BAT0;
        led_buf[0] |= LED_SEG_BAT1;
    } else {
        led_buf[0] |= LED_SEG_BAT0;
        led_buf[0] &= ~LED_SEG_BAT1;
    }

    if (disp_sleep_flag == true) {
        led_buf[0] |= LED_SEG_BT;
    } else {
        led_buf[0] &= ~LED_SEG_BT;
    }

    switch(menu) {
    case MENU_WELCOME:
        led_buf[2] = LED_H;
        led_buf[3] = LED_I;
        break;

    case MENU_SCENE_MAIN:
        if (val0 == MODE_MUSIC) {
        } else if (val0 == MODE_RECORD) {
            led_buf[1] = LED_R;
            led_buf[2] = LED_E;
            led_buf[3] = LED_C;
        } else if (val0 == MODE_BT) {
            led_buf[2] = LED_B;
            led_buf[3] = LED_T;
        } else if (val0 == MODE_FM) {
        } else if (val0 == MODE_LINEIN) {
            led_buf[2] = LED_I;
            led_buf[3] = LED_N;
        } else if (val0 == MODE_USBDEV) {
            led_buf[2] = LED_P;
            led_buf[3] = LED_C;
        } else if (val0 == MODE_POWEROFF) {
            led_buf[1] = LED_O;
            led_buf[2] = LED_F;
            led_buf[3] = LED_F;
        }
        break;

    case MENU_NUM:
    case MENU_INPUT_NUM:
        disp_led_digital_num(val0, 0, 1);
        break;

    case MENU_VOLUME:
        disp_led_digital_num(val0, 0, 1);
        led_buf[1] = LED_V;
        break;

    case MENU_EQ:
        disp_led_digital_num(val0, 0, 1);
        led_buf[1] = LED_OFF;
        led_buf[2] = LED_E;
        led_buf[3] = LED_Q;
        break;

    case MENU_REPEAT:
        if (val0 == REPEAT_ALL) {
            led_buf[1] = LED_OFF;
            led_buf[2] = LED_A;
            led_buf[3] = LED_L;
            led_buf[4] = LED_L;
        } else if (val0 == REPEAT_SINGLE) {
            led_buf[1] = LED_OFF;
            led_buf[2] = LED_O;
            led_buf[3] = LED_N;
            led_buf[4] = LED_E;
        } else if (val0 == REPEAT_RANDOM) {
            led_buf[1] = LED_OFF;
            led_buf[2] = LED_R;
            led_buf[3] = LED_A;
            led_buf[4] = LED_N;
        } else if (val0 == REPEAT_FOLDER) {
            led_buf[1] = LED_F;
            led_buf[2] = LED_O;
            led_buf[3] = LED_L;
            led_buf[4] = LED_D;
        }
        break;

    case MENU_WAITING:
        led_buf[1] = LED__;
        led_buf[2] = LED__;
        led_buf[3] = LED__;
        led_buf[4] = LED__;
        break;

    case MENU_MUSIC_PLAY:
    case MENU_MUSIC_STOP:
    case MENU_RECORD_PLAY:
    case MENU_RECORD_ING:
        if (DEV_SDCARD == device_cur_dev_get()) {
            led_buf[0] |= LED_SEG_TF;
        } else if (DEV_UDISK == device_cur_dev_get()) {
            led_buf[0] |= LED_SEG_USB;
        } else {
            led_buf[0] &= ~LED_SEG_TF;
            led_buf[0] &= ~LED_SEG_USB;
            led_buf[2] = LED_N;
            led_buf[3] = LED_O;
            break;
        }

        if ((menu == MENU_MUSIC_PLAY || menu == MENU_MUSIC_STOP) &&
            np.manager.total_file == 0) {
            led_buf[2] = LED_N;
            led_buf[3] = LED_O;
            break;
        }

        if (menu == MENU_MUSIC_PLAY ||
            menu == MENU_RECORD_PLAY ||
            menu == MENU_RECORD_ING) {
            if (val0 >= 100) {
                val0 = val0 % 100;
            }

            led_buf[0] |= LED_SEG_COL;
            disp_led_digital_num_h(val0);
            disp_led_digital_num_l(val1);
        } else {
            led_buf[0] &= ~LED_SEG_COL;
            led_buf[1] = LED_P;
            led_buf[2] = LED_A;
            led_buf[3] = LED_U;
            led_buf[4] = LED_S;
        }
        break;

    case MENU_CLOCK_TIME:
        led_buf[0] |= LED_SEG_COL;
        disp_led_digital_num_h(val0);
        disp_led_digital_num_l(val1);
        break;

    case MENU_FM_FREQ:
        disp_led_digital_num(val0, 0, 0);
        led_buf[0] |= LED_SEG_FM;
        break;

    case MENU_FM_CHANNEL:
        disp_led_digital_num_l(val0);
        led_buf[1] = LED_C;
        led_buf[2] = LED_H;
        break;
    }

    ledc_update();
}

AT_SRAM(.disp)
void disp_sleep_digital(bool state)
{
    if (state) {
        disp_sleep_flag = true;
        led_buf[0] |= LED_SEG_BT;
    } else {
        disp_sleep_flag = false;
        led_buf[0] &= ~LED_SEG_BT;
    }
    ledc_update();
}

AT_SRAM(.disp)
void disp_bat_low_digital(bool state)
{
    if (state) {
        led_buf[0] &= ~LED_SEG_BAT0;
        led_buf[0] |= LED_SEG_BAT1;
    } else {
        led_buf[0] |= LED_SEG_BAT0;
        led_buf[0] &= ~LED_SEG_BAT1;
    }
    ledc_update();
}
#endif

