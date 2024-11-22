#define LOG_LEV 4

#include "sys_inc.h"
#include "user_config.h"
#include "karaoke_framework.h"
#include "tws_profile.h"
#include "bt.h"
#include "bt_host_api.h"
#include "disp.h"
#include "app_pmu.h"

#if LED_DIODE_EN
led_diode_t led_diode;
static bool red_led_sta = 0;
static bool blue_led_sta = 0;
static bool green_led_sta = 0;


void disp_led_diode_red_init(void)
{
#if LED_RED_EN

#if (LED_RED_PIN_ACTIVE_STATE == 0)
    HAL_gpio_config(LED_RED_PIN_NUM | PIN_FUNCOUTPUT | PIN_HIGH | PIN_DRI0);
#elif (LED_RED_PIN_ACTIVE_STATE == 1)
    HAL_gpio_config(LED_RED_PIN_NUM | PIN_FUNCOUTPUT | PIN_LOW | PIN_DRI0);
#elif (LED_RED_PIN_ACTIVE_STATE == 2)
    HAL_gpio_config(LED_RED_PIN_NUM | PIN_FUNCOUTPUT | PIN_HIGH | PIN_DRI0);
#elif (LED_RED_PIN_ACTIVE_STATE == 3)
    HAL_gpio_config(LED_RED_PIN_NUM | PIN_FUNCOUTPUT | PIN_LOW | PIN_DRI0);
#endif
#endif
}


void disp_led_diode_red_on(void)
{
    /* LED_RED_PIN_ACTIVE_STATE:
       0-输出低电平；1-输出高电平；2-下拉2K；3-上拉2K */
#if LED_RED_EN
#if (LED_RED_PIN_ACTIVE_STATE == 0)
    HAL_gpio_value_set(LED_RED_PIN_NUM, 0);
#elif (LED_RED_PIN_ACTIVE_STATE == 1)
    HAL_gpio_value_set(LED_RED_PIN_NUM, 1);
#elif (LED_RED_PIN_ACTIVE_STATE == 2)
    HAL_gpio_pull_set(LED_RED_PIN_NUM, PIN_PULL_DOWN2K);
    HAL_gpio_func_set(LED_RED_PIN_NUM, PIN_FUNCINPUT);
#elif (LED_RED_PIN_ACTIVE_STATE == 3)
    HAL_gpio_pull_set(LED_RED_PIN_NUM, PIN_PULL_UP2K);
    HAL_gpio_func_set(LED_RED_PIN_NUM, PIN_FUNCINPUT);
#endif
#endif
}


void disp_led_diode_red_off(void)
{
#if LED_RED_EN
#if (LED_RED_PIN_ACTIVE_STATE == 0)
    HAL_gpio_value_set(LED_RED_PIN_NUM, 1);
#elif (LED_RED_PIN_ACTIVE_STATE == 1)
    HAL_gpio_value_set(LED_RED_PIN_NUM, 0);
#elif (LED_RED_PIN_ACTIVE_STATE == 2)
    HAL_gpio_value_set(LED_RED_PIN_NUM, 1);
    HAL_gpio_func_set(LED_RED_PIN_NUM, PIN_FUNCOUTPUT);
#elif (LED_RED_PIN_ACTIVE_STATE == 3)
   // HAL_gpio_value_set(LED_RED_PIN_NUM, 0);
   // HAL_gpio_func_set(LED_RED_PIN_NUM, PIN_FUNCOUTPUT);
   HAL_gpio_config(LED_RED_PIN_NUM | PIN_FUNCDISABLE);
#endif
#endif
}


void disp_led_diode_blue_init(void)
{
#if LED_BLUE_EN
#if (LED_BLUE_PIN_ACTIVE_STATE == 0)
    HAL_gpio_config(LED_BLUE_PIN_NUM | PIN_FUNCOUTPUT | PIN_HIGH | PIN_DRI0);
#elif (LED_BLUE_PIN_ACTIVE_STATE == 1)
    HAL_gpio_config(LED_BLUE_PIN_NUM | PIN_FUNCOUTPUT | PIN_LOW | PIN_DRI0);
#elif (LED_BLUE_PIN_ACTIVE_STATE == 2)
    HAL_gpio_config(LED_BLUE_PIN_NUM | PIN_FUNCOUTPUT | PIN_HIGH | PIN_DRI0);
#elif (LED_BLUE_PIN_ACTIVE_STATE == 3)
    HAL_gpio_config(LED_BLUE_PIN_NUM | PIN_FUNCOUTPUT | PIN_LOW | PIN_DRI0);
#endif

#endif

}


void disp_led_diode_blue_on(void)
{
    /* LED_BLUE_PIN_ACTIVE_STATE:
       0-输出低电平；1-输出高电平；2-下拉2K；3-上拉2K */
#if LED_BLUE_EN
#if (LED_BLUE_PIN_ACTIVE_STATE == 0)
    HAL_gpio_value_set(LED_BLUE_PIN_NUM, 0);
#elif (LED_BLUE_PIN_ACTIVE_STATE == 1)
    HAL_gpio_value_set(LED_BLUE_PIN_NUM, 1);
#elif (LED_BLUE_PIN_ACTIVE_STATE == 2)
    HAL_gpio_pull_set(LED_BLUE_PIN_NUM, PIN_PULL_DOWN2K);
    HAL_gpio_func_set(LED_BLUE_PIN_NUM, PIN_FUNCINPUT);
#elif (LED_BLUE_PIN_ACTIVE_STATE == 3)
    HAL_gpio_pull_set(LED_BLUE_PIN_NUM, PIN_PULL_UP2K);
    HAL_gpio_func_set(LED_BLUE_PIN_NUM, PIN_FUNCINPUT);
#endif
#endif
}


void disp_led_diode_blue_off(void)
{
#if LED_BLUE_EN
#if (LED_BLUE_PIN_ACTIVE_STATE == 0)
    HAL_gpio_value_set(LED_BLUE_PIN_NUM, 1);
#elif (LED_BLUE_PIN_ACTIVE_STATE == 1)
    HAL_gpio_value_set(LED_BLUE_PIN_NUM, 0);
#elif (LED_BLUE_PIN_ACTIVE_STATE == 2)
    HAL_gpio_value_set(LED_BLUE_PIN_NUM, 1);
    HAL_gpio_func_set(LED_BLUE_PIN_NUM, PIN_FUNCOUTPUT);
#elif (LED_BLUE_PIN_ACTIVE_STATE == 3)
    //HAL_gpio_value_set(LED_BLUE_PIN_NUM, 0);
   // HAL_gpio_func_set(LED_BLUE_PIN_NUM, PIN_FUNCOUTPUT);
    HAL_gpio_config(LED_BLUE_PIN_NUM | PIN_FUNCDISABLE);
#endif
#endif

}



void disp_led_diode_green_init(void)
{
#if LED_GREEN_EN

#if (LED_GREEN_PIN_ACTIVE_STATE == 0)
    HAL_gpio_config(LED_GREEN_PIN_NUM | PIN_FUNCOUTPUT | PIN_HIGH | PIN_DRI0);
#elif (LED_GREEN_PIN_ACTIVE_STATE == 1)
    HAL_gpio_config(LED_GREEN_PIN_NUM | PIN_FUNCOUTPUT | PIN_LOW | PIN_DRI0);
#elif (LED_GREEN_PIN_ACTIVE_STATE == 2)
    HAL_gpio_config(LED_GREEN_PIN_NUM | PIN_FUNCOUTPUT | PIN_HIGH | PIN_DRI0);
#elif (LED_GREEN_PIN_ACTIVE_STATE == 3)
    HAL_gpio_config(LED_GREEN_PIN_NUM | PIN_FUNCOUTPUT | PIN_LOW | PIN_DRI0);
#endif
#endif
}


void disp_led_diode_green_on(void)
{
    /* LED_RED_PIN_ACTIVE_STATE:
       0-输出低电平；1-输出高电平；2-下拉2K；3-上拉2K */
#if LED_GREEN_EN
#if (LED_GREEN_PIN_ACTIVE_STATE == 0)
    HAL_gpio_value_set(LED_GREEN_PIN_NUM, 0);
#elif (LED_GREEN_PIN_ACTIVE_STATE == 1)
    HAL_gpio_value_set(LED_GREEN_PIN_NUM, 1);
#elif (LED_GREEN_PIN_ACTIVE_STATE == 2)
    HAL_gpio_pull_set(LED_GREEN_PIN_NUM, PIN_PULL_DOWN2K);
    HAL_gpio_func_set(LED_GREEN_PIN_NUM, PIN_FUNCINPUT);
#elif (LED_GREEN_PIN_ACTIVE_STATE == 3)
    HAL_gpio_pull_set(LED_GREEN_PIN_NUM, PIN_PULL_UP2K);
    HAL_gpio_func_set(LED_GREEN_PIN_NUM, PIN_FUNCINPUT);
#endif
#endif
}


void disp_led_diode_green_off(void)
{
#if LED_GREEN_EN
#if (LED_GREEN_PIN_ACTIVE_STATE == 0)
    HAL_gpio_value_set(LED_GREEN_PIN_NUM, 1);
#elif (LED_GREEN_PIN_ACTIVE_STATE == 1)
    HAL_gpio_value_set(LED_GREEN_PIN_NUM, 0);
#elif (LED_GREEN_PIN_ACTIVE_STATE == 2)
    HAL_gpio_value_set(LED_GREEN_PIN_NUM, 1);
    HAL_gpio_func_set(LED_GREEN_PIN_NUM, PIN_FUNCOUTPUT);
#elif (LED_GREEN_PIN_ACTIVE_STATE == 3)
   // HAL_gpio_value_set(LED_RED_PIN_NUM, 0);
   // HAL_gpio_func_set(LED_RED_PIN_NUM, PIN_FUNCOUTPUT);
   HAL_gpio_config(LED_GREEN_PIN_NUM | PIN_FUNCDISABLE);
#endif
#endif
}




void disp_led_diode_init(void)
{
#if LED_RED_EN
    disp_led_diode_red_init();
    led_diode.red_cnt = 0;
    led_diode.red_sta_last = DISP_OFF;
    led_diode.red_flash_on_time = 5;
#endif

#if LED_BLUE_EN
    disp_led_diode_blue_init();
    led_diode.blue_cnt = 0;
    led_diode.blue_sta_last = DISP_OFF;
    led_diode.blue_flash_on_time = 5;
#endif

#if LED_GREEN_EN
    disp_led_diode_green_init();
    led_diode.green_cnt = 0;
    led_diode.green_sta_last = DISP_OFF;
    led_diode.green_flash_on_time = 5;
#endif

}

void disp_led_diode_deinit(void)
{
#if LED_RED_EN
    disp_led_diode_red_off();
    HAL_gpio_pull_set(LED_RED_PIN_NUM, PIN_PULL_DISABLE);
    HAL_gpio_func_set(LED_RED_PIN_NUM, PIN_FUNCDISABLE);

    led_diode.red_cnt = 0;
    led_diode.red_sta_last = DISP_OFF;
#endif

#if LED_BLUE_EN
    disp_led_diode_blue_off();
    HAL_gpio_pull_set(LED_BLUE_PIN_NUM, PIN_PULL_DISABLE);
    HAL_gpio_func_set(LED_BLUE_PIN_NUM, PIN_FUNCDISABLE);

    led_diode.blue_cnt = 0;
    led_diode.blue_sta_last = DISP_OFF;
#endif

#if LED_GREEN_EN
    disp_led_diode_green_off();
    HAL_gpio_pull_set(LED_GREEN_PIN_NUM, PIN_PULL_DISABLE);
    HAL_gpio_func_set(LED_GREEN_PIN_NUM, PIN_FUNCDISABLE);

    led_diode.green_cnt = 0;
    led_diode.green_sta_last = DISP_OFF;
#endif

}



AT_SRAM(.disp)
void disp_led_diode_timer(void)
{

    if (led_diode.red_sta == DISP_FLASH) {
        led_diode.red_cnt++;
        if (red_led_sta && led_diode.red_cnt >= led_diode.red_flash_on_time) {
            led_diode.red_cnt = 0;
            disp_led_diode_red_off();
            red_led_sta = 0;
            led_diode.red_flash_count++;
        } else if (!red_led_sta && led_diode.red_cnt >= led_diode.red_flash_off_time) {
            led_diode.red_cnt = 0;
            disp_led_diode_red_on();
            red_led_sta = 1;
            led_diode.red_flash_count++;
        }

        if (led_diode.red_flash_count/2 >= led_diode.red_flash_round) {
           disp_led_onoff_set(LED_RED,0);
        }

    } else if (led_diode.red_sta == DISP_FLASH_INTERVAL) {
        led_diode.red_cnt++;
        if (led_diode.red_cnt >= led_diode.red_flash_on_time) {
            led_diode.red_cnt = 0;
            if (red_led_sta) {
                disp_led_diode_red_off();
                disp_led_diode_blue_on();
                red_led_sta = 0;
            } else {
                disp_led_diode_blue_off();
                disp_led_diode_red_on();
                red_led_sta = 1;
            }
        }
    }  else if (led_diode.red_sta == DISP_ON_DELAY_OFF) {
        led_diode.red_cnt++;
        if (led_diode.red_cnt >= led_diode.red_flash_on_time) {
            led_diode.red_cnt = 0;
            led_diode.red_sta = DISP_OFF;
            led_diode.red_sta_last = DISP_OFF;
            disp_led_diode_red_off();
            red_led_sta = 0;
        }
    } else if (led_diode.red_sta == DISP_FLAGH_LOOP) {
            if (red_led_sta == 1) {
                led_diode.red_cnt++;
                disp_led_diode_red_on();
                disp_led_diode_blue_off();
                disp_led_diode_green_off();
                 if (led_diode.red_cnt >= led_diode.red_flash_on_time){
                    red_led_sta = 0;
                    led_diode.red_cnt = 0;
                    disp_led_diode_red_off();
                    disp_led_diode_green_on();
                    green_led_sta = 1;
                 }
            }
    }

    if (led_diode.green_sta == DISP_FLASH) {
        led_diode.green_cnt++;
        if (green_led_sta && led_diode.green_cnt >= led_diode.green_flash_on_time) {
            led_diode.green_cnt = 0;
            disp_led_diode_green_off();
            green_led_sta = 0;
            led_diode.green_flash_count++;
        } else if (!green_led_sta && led_diode.green_cnt >= led_diode.green_flash_off_time) {
            led_diode.green_cnt = 0;
            disp_led_diode_green_on();
            green_led_sta = 1;
            led_diode.green_flash_count++;
        }
        if (led_diode.green_flash_count/2 >= led_diode.green_flash_round) {
           disp_led_onoff_set(LED_GREEN,0);
        }
    } else if (led_diode.green_sta == DISP_FLASH_INTERVAL) {
        led_diode.green_cnt++;
        if (led_diode.green_cnt >= led_diode.green_flash_on_time) {
            led_diode.green_cnt = 0;
            if (green_led_sta) {
                disp_led_diode_blue_off();
                disp_led_diode_red_on();
                green_led_sta = 0;
            } else {
                disp_led_diode_red_off();
                disp_led_diode_blue_on();
                green_led_sta = 1;
            }
        }
    } else if (led_diode.green_sta == DISP_ON_DELAY_OFF) {
        led_diode.green_cnt++;
        if (led_diode.green_cnt >= led_diode.green_flash_on_time) {
            led_diode.green_cnt = 0;
            led_diode.green_sta = DISP_OFF;
            led_diode.green_sta_last = DISP_OFF;
            disp_led_diode_green_off();
            green_led_sta = 0;
        }
    } else if (led_diode.green_sta == DISP_FLAGH_LOOP) {
            if (green_led_sta == 1) {
                led_diode.green_cnt++;
                disp_led_diode_green_on();
                disp_led_diode_blue_off();
                disp_led_diode_red_off();
                 if (led_diode.green_cnt >= led_diode.green_flash_on_time){
                    green_led_sta = 0;
                    led_diode.green_cnt = 0;
                    disp_led_diode_green_off();
                    disp_led_diode_blue_on();
                    blue_led_sta = 1;
                 }
            }
    }


    if (led_diode.blue_sta == DISP_FLASH) {
        led_diode.blue_cnt++;
        if (blue_led_sta && led_diode.blue_cnt >= led_diode.blue_flash_on_time) {
            led_diode.blue_cnt = 0;
            disp_led_diode_blue_off();
            blue_led_sta = 0;
            led_diode.blue_flash_count++;
        } else if (!blue_led_sta && led_diode.blue_cnt >= led_diode.blue_flash_off_time) {
             led_diode.blue_cnt = 0;
            disp_led_diode_blue_on();
            blue_led_sta = 1;
             led_diode.blue_flash_count++;
        }
        if (led_diode.blue_flash_count/2 >= led_diode.blue_flash_round) {
           disp_led_onoff_set(LED_BLUE,0);
        }
    } else if (led_diode.blue_sta == DISP_FLASH_INTERVAL) {
        led_diode.blue_cnt++;
        if (led_diode.blue_cnt >= led_diode.blue_flash_on_time) {
            led_diode.blue_cnt = 0;
            if (blue_led_sta) {
                disp_led_diode_blue_off();
                disp_led_diode_red_on();
                blue_led_sta = 0;
            } else {
                disp_led_diode_red_off();
                disp_led_diode_blue_on();
                blue_led_sta = 1;
            }
        }
    } else if (led_diode.blue_sta == DISP_ON_DELAY_OFF) {
        led_diode.blue_cnt++;
        if (led_diode.blue_cnt >= led_diode.blue_flash_on_time) {
            led_diode.blue_cnt = 0;
            led_diode.blue_sta = DISP_OFF;
            led_diode.blue_sta_last = DISP_OFF;
            disp_led_diode_blue_off();
            blue_led_sta = 0;
        }
    } else if (led_diode.blue_sta == DISP_FLAGH_LOOP) {
            if (blue_led_sta == 1) {
                led_diode.blue_cnt++;
                disp_led_diode_blue_on();
                disp_led_diode_green_off();
                disp_led_diode_red_off();
                 if (led_diode.blue_cnt >= led_diode.blue_flash_on_time) {
                    blue_led_sta = 0;
                    led_diode.blue_cnt = 0;
                    disp_led_diode_blue_off();
                    disp_led_diode_red_on();
                    red_led_sta = 1;
                 }
            }
    }

}



AT_SRAM(.disp)
void disp_led_diode(enum disp_led_id_e id, uint8_t onoff)
{

#if LED_RED_EN

    if (id == LED_RED) {
        led_diode.current_id = id;
        if (led_diode.red_sta_last != onoff) {
            led_diode.red_sta = onoff;
            led_diode.red_sta_last = onoff;
            if ((onoff == DISP_ON) || (onoff == DISP_ON_DELAY_OFF)) {
                disp_led_diode_red_on();
                led_diode.red_cnt = 0;
                red_led_sta = 1;
            } else if((onoff == DISP_OFF) || (onoff == DISP_FLASH_INTERVAL)) {
                disp_led_diode_red_off();
                led_diode.red_cnt = 0;
                red_led_sta = 0;
            } else if(onoff == DISP_FLASH) {
                 disp_led_diode_red_off();
                led_diode.red_cnt = 0;
                red_led_sta = 0;
            } else if (onoff == DISP_FLAGH_LOOP) {
                disp_led_diode_red_on();
                led_diode.red_cnt = 0;
                red_led_sta = 1;
            }
        }
        
    }
#endif

#if LED_BLUE_EN
     if (id == LED_BLUE)  {
            led_diode.current_id = id;
        if (led_diode.blue_sta_last != onoff) {
            led_diode.blue_sta = onoff;
            led_diode.blue_sta_last = onoff;
            if ((onoff == DISP_ON) || (onoff == DISP_ON_DELAY_OFF) || (onoff == DISP_FLASH_INTERVAL)) {
                disp_led_diode_blue_on();
                blue_led_sta = 1;
                led_diode.blue_cnt = 0;
            } else if(onoff == DISP_OFF) {
                disp_led_diode_blue_off();
                led_diode.blue_cnt = 0;
                blue_led_sta = 0;
            } else if(onoff == DISP_FLASH) {
                    disp_led_diode_blue_off();
                    blue_led_sta = 0;
                    led_diode.blue_cnt = 0;
            } else if (onoff == DISP_FLAGH_LOOP) {
                disp_led_diode_blue_off();
                led_diode.blue_cnt = 0;
                blue_led_sta = 0;
            }
        }
    }
#endif

#if LED_GREEN_EN
         if (id == LED_GREEN)  {
            led_diode.current_id = id;
            if(led_diode.green_sta_last != onoff) {
                led_diode.green_sta = onoff;
                led_diode.green_sta_last = onoff;
                if ((onoff == DISP_ON) || (onoff == DISP_ON_DELAY_OFF)) {
                    disp_led_diode_green_on();
                    green_led_sta = 1;
                    led_diode.green_cnt = 0;
                } else if(onoff == DISP_OFF) {
                    disp_led_diode_green_off();
                    led_diode.green_cnt = 0;
                    blue_led_sta = 0;
                } else if(onoff == DISP_FLASH) {
                    disp_led_diode_green_off();
                    green_led_sta = 0;
                    led_diode.green_cnt = 0;
                }  else if (onoff == DISP_FLAGH_LOOP) {
                    disp_led_diode_green_off();
                    led_diode.green_cnt = 0;
                    green_led_sta = 0;
                }
            }
        }
#endif

   


}

/*------------------------------------------------------------------------------
*函数功能: 设置红蓝灯闪烁： 
参数1：id 红蓝灯
参数2：亮灯时间 ms
参数3：灭队时间 ms
参数4：亮灭多少轮结束，0xffffffff 表示无限循环
*-------------------------------------------------------------------------------*/
void disp_led_flash_set(enum disp_led_id_e id, uint16_t flash_on_time, uint16_t flash_off_time, uint32_t round_time)
{

    if(id == led_diode.current_id && round_time == 0xffffffff) {
        if(led_diode.blue_sta == DISP_FLASH && id == LED_BLUE && (flash_on_time/DISP_INTERVAL_TIME) == led_diode.blue_flash_on_time &&  (flash_off_time/DISP_INTERVAL_TIME) == led_diode.blue_flash_off_time) {
             logi("disp_led_flash_set blue same return");
             return;
        }

        if(led_diode.red_sta == DISP_FLASH && id == LED_RED && (flash_on_time/DISP_INTERVAL_TIME) == led_diode.red_flash_on_time && (flash_off_time/DISP_INTERVAL_TIME) == led_diode.red_flash_off_time) {
             logi("disp_led_flash_set red same return");
             return;
        }
    }

    disp_led_diode_deinit();
    disp_timer_onoff(0);
    if (id == LED_BLUE) {
        led_diode.blue_flash_on_time = flash_on_time/DISP_INTERVAL_TIME;
        led_diode.blue_flash_off_time = flash_off_time/DISP_INTERVAL_TIME;
        led_diode.blue_flash_round = round_time;
        led_diode.blue_flash_count = 0;
    } else if (id == LED_RED) {
        led_diode.red_flash_on_time = flash_on_time/DISP_INTERVAL_TIME;
        led_diode.red_flash_off_time = flash_off_time/DISP_INTERVAL_TIME;
        led_diode.red_flash_round = round_time;
        led_diode.red_flash_count = 0;

    }else if (id == LED_GREEN) {
        led_diode.green_flash_on_time = flash_on_time/DISP_INTERVAL_TIME;
        led_diode.green_flash_off_time = flash_off_time/DISP_INTERVAL_TIME;
        led_diode.green_flash_round = round_time;
        led_diode.green_flash_count = 0;

    }
    disp_led_diode(id,DISP_FLASH);
    disp_timer_onoff(1);
}

/*------------------------------------------------------------------------------
*函数功能: 设置红蓝灯交替闪烁： 
参数1：id 红蓝灯
参数2：交替闪烁时间 ms
*-------------------------------------------------------------------------------*/
void disp_led_interval_set(enum disp_led_id_e id, uint16_t flash_on_time)
{
    
    disp_led_diode_deinit();
    disp_timer_onoff(0);
    if (id == LED_BLUE) {
        led_diode.blue_flash_on_time = flash_on_time/DISP_INTERVAL_TIME;
    } else if (id == LED_RED) {
        led_diode.red_flash_on_time = flash_on_time/DISP_INTERVAL_TIME;
    }else if (id == LED_GREEN) {
        led_diode.green_flash_on_time = flash_on_time/DISP_INTERVAL_TIME;

    }
    disp_led_diode(id,DISP_FLASH_INTERVAL);
    disp_timer_onoff(1);
}

/*------------------------------------------------------------------------------
*函数功能: 设置红蓝灯亮多少时间后关闭： 
参数1：id 红蓝灯
参数2：亮灯时间
*-------------------------------------------------------------------------------*/
void disp_led_delayoff_set(enum disp_led_id_e id, uint16_t flash_on_time)
{
    disp_led_onoff_set(id,0);
    disp_timer_onoff(0);

    if (id == LED_BLUE) {
        led_diode.blue_flash_on_time = flash_on_time/DISP_INTERVAL_TIME;
    } else if (id == LED_RED) {
        led_diode.red_flash_on_time = flash_on_time/DISP_INTERVAL_TIME;

    } else if (id == LED_GREEN) {
        led_diode.green_flash_on_time = flash_on_time/DISP_INTERVAL_TIME;
    }
    disp_led_diode(id,DISP_ON_DELAY_OFF);
    disp_timer_onoff(1);
}

/*------------------------------------------------------------------------------
*函数功能: 设置红蓝灯开关： 
参数1：id 红蓝灯
参数2：开关
*-------------------------------------------------------------------------------*/

void disp_led_onoff_set(enum disp_led_id_e id, bool onoff)
{
    if (onoff)
        disp_led_diode(id,DISP_ON);
    else
        disp_led_diode(id,DISP_OFF);
}

/*------------------------------------------------------------------------------
*函数功能: 设置红绿蓝灯循环闪烁： 
参数1：id 红蓝灯
参数2：交替闪烁时间 ms
*-------------------------------------------------------------------------------*/
void disp_led_loop_set(enum disp_led_id_e id, uint16_t flash_on_time)
{
    disp_led_diode_deinit();
    disp_timer_onoff(0);
    if (id == LED_BLUE) {
        led_diode.blue_flash_on_time = flash_on_time/DISP_INTERVAL_TIME;
    } else if (id == LED_RED) {
        led_diode.red_flash_on_time = flash_on_time/DISP_INTERVAL_TIME;
    }else if (id == LED_GREEN) {
        led_diode.green_flash_on_time = flash_on_time/DISP_INTERVAL_TIME;

    }
    disp_led_diode(id,DISP_FLAGH_LOOP);
    disp_timer_onoff(1);
}


#endif

