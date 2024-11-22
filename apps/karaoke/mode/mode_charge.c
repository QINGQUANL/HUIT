#define LOG_LEV 4

#define SLTK_LOG_TAG "charge"
#include "sys_inc.h"
#include "user_config.h"
#include "hal.h"
#include "charge.h"
#include "mode_manager.h"

#include "middleware.h"
#include "karaoke_framework.h"
#include "audio_service.h"
#include "charge.h"
#include "disp.h"
#include "bt.h"
#include "app_pmu.h"
#include "app_lcdc.h"

void mode_charg_app_process(uint32_t event)
{
#if (WATCHDOG_EN && RTC_ALARM0_EN)
    HAL_watchdog_refresh(NULL);
#endif

    switch(event) {
        case PMU_EVENT_INT:
        case PMU_EVENT_VBUSMIDIN:
        case PMU_EVENT_VBUSMIDOUT:
        case PMU_EVENT_CHARGE_IN:
        case PMU_EVENT_CHARGE_OUT:
        case PMU_EVENT_BAT_FULL:
        case PM_CODE_EVENT | PM_POWERON:
        case PM_CODE_EVENT | PM_POWEROFF:
        case PM_CODE_EVENT | PM_SHUTDOWN:
        case PM_CODE_EVENT | PM_DUT:
        case PM_CODE_EVENT | PM_PAIR:
        case PM_CODE_EVENT | PM_UNPAIR:
        case PM_CODE_EVENT | PM_RECOV:
            pmu_event_process(event);
            break;

#if (WATCHDOG_EN && RTC_ALARM0_EN)
        case RTC_EVENT_ALM0:
            rtc_alarm_sandglass_set(10, 1);
            break;
#endif

        case KEY_EVENT_L | KEY_POWER:
        case PMU_EVENT_POWERKEY_L:
        case PMU_EVENT_HSW_ON:
#if !PMU_VBUSIN_RESTART
            //disp_state(STATE_RESET);
#if (G24_2T4_SHORT_PACKET_3TIMES || G24_2T4_WITH_MONO_MONITOR) && G24_FUNC_ROLE
            //不做操作
#else
            restart();//不支持开关机码的充电仓方案，充电模式下会触发长按事件，但是不需要开机
#endif

#endif
            break;

        case PMU_EVENT_BAT_LOW_WRN:
            logd("l");
#if (DISP_EN && STATE_BAT_LOW_EN)
            disp_state(STATE_BAT_LOW);
#endif
            //disp_bat_low_digital(true);
            break;

#if BAT_EXIST
        case SYSTEM_EVENT_EXIT_SLEEP_MODE:
            pmu_timer_cb(NULL);
            break;

        case SYSTEM_EVENT_SECOND:
#if RTC_EN
            pmu_timer_cb(NULL);
#endif
            break;
#endif

#if MODE_KEY_ONOFF
    case KEY_EVENT_SU | KEY_MODE:
        logi("-->>mode key");
        event_put(PM_CODE_EVENT | PM_POWERON, NULL, 0);
        break;
#endif

        default:
            break;
    }
}


void mode_charge_enter(void)
{
    logi("mode charge enter");

#if LCDC_EN && EVB_LCD_DEMO                        //显示开机图片
    lcd_charging_show();
#endif

#if CHARGING_BIN
    HAL_pmu_charge_enable(true);
#endif
    HAL_pmu_bat_full_sta_set(0);
    HAL_clk_set_cpux_freq(32000000, 32000000);
    sleep_prevent_clear(PS_AUDIO_PLAY|PS_AUDIO_RECORD|PS_AUDIO_ANC|PS_BT_INCOMING|PS_BTC|PS_APP_PROCESS|PS_BTH|PS_UART|PS_AUDIO_KWS|PS_MODE);

#if CHARGING_BIN_MID_LEVEL
    HAL_pmu_enable_5kpd(false, false);
#endif

#if (MODE_KEY_ONOFF)
    sleep_prevent_set(PS_APP_PROCESS);
#endif
    if (HAL_pmu_is_use_hsw()) {
        HAL_pmu_hsw_reset_enable(false);
    }

#if WATCHDOG_EN
    HAL_watchdog_refresh(NULL);
#if RTC_ALARM0_EN
    rtc_alarm_sandglass_set(10, 1);
#else
    HAL_watchdog_deinit();
#endif
#endif

    pmu_timer_cb(NULL);
    HAL_pmu_set_iref(1);

   // bt_autotest_chargemode_do();
    charge_register_event_handler(mode_charg_app_process);

    charge_mode_sta_set(true);
#if (DISP_EN && STATE_CHARGEING_EN)
    disp_state(STATE_CHARGEING);
#endif
}


void mode_charge_exit(void)
{
    //pmu_regu_enable(PMU_AVCC, 1);

    if (HAL_pmu_is_use_hsw()) {
        HAL_pmu_hsw_reset_enable(true);
    }

#if WATCHDOG_EN
    HAL_watchdog_refresh(NULL);
#if RTC_ALARM0_EN
    rtc_alarm_sandglass_set(10, 0);
#else
    HAL_watchdog_init(WATCHDOG_TIMEOUT);
#endif
#endif

    mode_last_sta_set(MODE_CHARGE);

    if (mode_get_sta() == MODE_RESET) {
        mode_set_force(MODE_BT);
        mode_switch_reason_set(SW_MODE_RESET);
    }

#if MODE_KEY_ONOFF
    sleep_prevent_clear(PS_APP_PROCESS);
#endif
    HAL_pmu_vbus_state_init();
    HAL_pmu_set_iref(0);
#if CHARGING_BIN
    HAL_pmu_charge_enable(false);
#endif
    HAL_pmu_bat_full_sta_set(0);
    charge_mode_sta_set(false);

    logi("mode charge exit");
}

void mode_charge(void)
{
    mode_charge_enter();
    mode_charge_loop();
    mode_charge_exit();
}
