#define LOG_LEV 4
#include "sys_inc.h"

#include "user_config.h"
#include "karaoke_framework.h"
#include "audio_player.h"
#include "disp.h"
#include "oal.h"
#include "tws_api.h"
#include "bt_host_api.h"
#include "tws_profile.h"


disp_t disp;
OAL_TIMER DispTimer;
OAL_TIMER DispSyncTimer;

void disp_menu_set(uint32_t menu)
{
    disp.menu = menu;
}

uint32_t disp_menu_get(void)
{
    return disp.menu;
}

void disp_count_set(uint32_t count)
{
    disp.count = count;
}

uint32_t disp_count_get(void)
{
    return disp.count;
}

#if DISP_EN
void app_disp_init(void)
{
#if DISP_EN
    if (DispTimer == NULL) {
        OAL_timer_create(&DispTimer, "DispTimer", OAL_ms_to_ticks(DISP_INTERVAL_TIME),
            disp_timer_cb, (void *)1, 1, OAL_TIMER_AUTO_ACTIVATE);
    }

    if (DispSyncTimer == NULL) {
        OAL_timer_create(&DispSyncTimer, "DispSyncTimer", OAL_ms_to_ticks(DISP_INTERVAL_TIME),
            disp_timer_sync_cb, (void *)1, 0, OAL_TIMER_NO_ACTIVATE);
    }

#if LED_DIGITAL_EN
    disp_led_digital_init();
#endif

#if LED_DIODE_EN
    disp_led_diode_init();
#endif

#if LCD_SEG_EN
    disp_lcd_seg_init();
#endif

#endif
}




AT_SRAM(.disp)
void disp_timer_cb(void* xTimer)
{

#if LED_DIODE_EN
    disp_led_diode_timer();
#endif

#if LCD_SEG_EN
    disp_lcd_seg();
#endif

    if (disp.count < DISP_DURATION_TIME) {
        disp.count++;
        if(disp.count == DISP_DURATION_TIME) {
            disp.count = DISP_DURATION_TIME + 1;
            event_put(DISP_EVENT_SWITCH, NULL, 0);
        }
    }
}

AT_SRAM(.disp)
void disp_timer_sync_cb(void* xTimer)
{
    logi("##disp_timer_sync_cb state:%d",disp.sync_state);
   disp_state(disp.sync_state);
   sleep_prevent_clear(PS_APP_PROCESS);

}



void disp_timer_sync_start(uint32_t ms, enum disp_state_e state)
{
    logi("##disp_timer_sync_start, ms:%d,state:%d",ms,state);
    OAL_timer_activate(&DispSyncTimer,OAL_TIMER_NO_ACTIVATE);
    OAL_timer_ChangePeriod(&DispSyncTimer,OAL_ms_to_ticks(ms),0);
    disp.sync_state = state;
    OAL_timer_activate(&DispSyncTimer,OAL_TIMER_AUTO_ACTIVATE);
    sleep_prevent_set(PS_APP_PROCESS);
}

void disp_timer_onoff(bool onoff)
{
    if (onoff) {
        OAL_timer_activate(&DispTimer,OAL_TIMER_AUTO_ACTIVATE);
    }else {
        OAL_timer_activate(&DispTimer,OAL_TIMER_NO_ACTIVATE);
    }
}




/*------------------------------------------------------------------------------
*函数功能: 对耳同步显示灯控
*state：显示模式
*-------------------------------------------------------------------------------*/


void disp_state_sync(enum disp_state_e state,uint16_t delt_time)
{
        tws_master_time_info_t info;
        if (tws_role_get() != TWS_SLAVE) {
            tws_master_get_time_info(&info, NULL);
            tws_disp_time_info_t disp_info;
            disp_info.clkn = info.clkn;
            disp_info.finecnt = info.finecnt;
            disp_info.delay_t = delt_time;
            disp_info.disp_state = state;
            logi("clkn:%d",disp_info.clkn);
            logi("finecnt:%d",disp_info.finecnt);
            logi("disp_state_sync send ：0x%x",state);
            tws_manager_app_cmd_send(TWS_APP_CMD_DISP_SYNC, (uint8_t *)&disp_info, sizeof(disp_info));
            disp_timer_sync_start(delt_time,state);
        }
}

void disp_slave_state_sync( uint8_t *params,uint32_t params_len)
{
           tws_disp_time_info_t disp_info;
           tws_slave_time_info_t sinfo;
           uint32_t delta_us;;
           memcpy(&disp_info,params,sizeof(disp_info));
           logi("sync disp get:0x%x\n",disp_info.disp_state);
           logi("sync clk:%d",disp_info.clkn);
           logi("sync finecnt:%d",disp_info.finecnt);

            tws_slave_get_time_info(&sinfo, NULL);
            delta_us = get_bt_delta_us(disp_info.clkn, disp_info.finecnt,
            sinfo.clkn, sinfo.finecnt, sinfo.bofs);
            delta_us /= 1000;
           logi("delta:%d",delta_us);
           if(delta_us > disp_info.delay_t) {
                logi("too long deley");
                delta_us = 200;
           }
           disp_timer_sync_start((disp_info.delay_t-delta_us),disp_info.disp_state);
}


AT_SRAM(.disp)
void disp_menu(enum disp_menu_e menu, uint32_t val0, uint32_t va11)
{
    switch (menu) {
    case MENU_SCENE_MAIN:
    case MENU_NUM:
    case MENU_INPUT_NUM:
    case MENU_VOLUME:
    case MENU_EQ:
    case MENU_REPEAT:
    case MENU_WAITING:
    case MENU_CLOCK_TIME:
    case MENU_FM_CHANNEL:
        disp.menu = menu;
        disp.count = 0;
#if LED_DIGITAL_EN
        disp_menu_led_digital(menu, val0, va11);
#endif
        break;

    case MENU_WELCOME:
        disp.menu = menu;
        disp.count = 0xFF;
#if LED_DIGITAL_EN
        disp_menu_led_digital(menu, val0, va11);
#endif
        break;

    case MENU_FM_FREQ:
        disp.menu = MENU_SCENE_MAIN;
#if LED_DIGITAL_EN
        disp_menu_led_digital(menu, val0, va11);
#endif
        break;

    case MENU_MUSIC_STOP:
        disp.menu = menu;
#if LED_DIGITAL_EN
        disp_menu_led_digital(menu, val0, va11);
#endif
        break;

    case MENU_MUSIC_PLAY:
    case MENU_RECORD_PLAY:
    case MENU_RECORD_ING:
        if (disp.menu == MENU_SCENE_MAIN) {
#if LED_DIGITAL_EN
            disp_menu_led_digital(menu, val0, va11);
#endif
        }
        break;
    }
}

AT_SRAM(.disp)
void disp_state(enum disp_state_e state)
{
#if (LED_DIODE_EN ||LED_PWM_EN)
    ui_led_diode_process(state);
#endif

}



AT_SRAM(.disp)
void disp_sleep(bool state)
{
#if (LED_DIGITAL_EN && DEBUG_DEEP_SLEEP_EN)
    disp_sleep_digital(state);
#endif
}

#else
void app_disp_init(void)
{
}

AT_SRAM(.disp)
void disp_timer_cb(void *xTimer)
{
}

AT_SRAM(.disp)
void disp_menu(enum disp_menu_e menu, uint32_t val0, uint32_t va11)
{

}

AT_SRAM(.disp)
void disp_state(enum disp_state_e state)
{
}

AT_SRAM(.disp)
void disp_sleep(bool state)
{
}


#endif

