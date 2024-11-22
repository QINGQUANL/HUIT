#define LOG_LEV 4
#include "sys_inc.h" // libsys.a
#include "oal.h" // liboal.a
#include "hal.h" //libhal.a
#include "sltk_mem.h"

#include "system.h" //libmuddleware.a
#include "mode_manager.h"
#include "console.h"
#include "user_config.h"
#include "bt_host_api.h"
#include "info_mem.h"
#include "tws_profile.h"
#include "bt_api.h"
#include "bt.h"
#include "mode_afc.h"

#include "ui.h"
#include "tws_api.h"
#include "ble_profile.h"
#include "ble_api.h"
#include "tone_play_manager.h"
#include "tws_api.h"
#include "app_pmu.h"
#include "mixer.h"
#include "event.h"
#include "disp.h"
#include "key.h"
#include "rtc.h"
#include "pms.h"
#include "sleep.h"
#include "sensor.h"
#include "anc_service.h"
#include "ble52_api.h"
#include "app_psm.h"
#include "psm_manager.h"
#include "app_audio_anc.h"
#include "kws_manager.h"
#include "air_api.h"
#include "mode_bt.h"
#include "sys_version.h"
#include "rfdss.h"
#include "soundcard.h"
#include "sbt_api.h"

#if MODE_AFC_EN


void mode_afc_app_process(sys_event_info_t *info)
{
#if WATCHDOG_EN
    HAL_watchdog_refresh(NULL);
#endif

    uint32_t event = info->event;
    switch(event) {
    default:
        ui_process(event, info->params_len, info->params);
        break;
    }
}

void mode_afc_init()
{
    afc_register_event_handler(mode_afc_app_process);
    //observer_resiger_callback((obclbk)app_service_observer, OBSERVER_APP_SERVICE);
}

void mode_afc_enter(void)
{
    //logi("mode_afc_enter role:%d version= %d>>", G24_FUNC_ROLE, sys_ver);

	mode_afc_init();

#if (TONE_EN && TONE_G24_MODE_EN)
    if (mode_switch_reason_get() != SW_MODE_RESET) {
        am_tone_play(SYS_TONE_WELCOME_NAME, 0, 0, TONE_AFC_MODE_VOL, 0, 0);
    }
#endif

    logi("<<mode_afc_enter");

}

void mode_afc_exit(void)
{
    logi("mode_afc_exit>>");

    //comm_deinit();

    mode_last_sta_set(MODE_AFC);

    //关机前清掉事件，防止事件遗留到下一次开机过程中
    event_group_clear(SYSTEM_EVENT);

    logi("mode_afc_exit<<");
}

void mode_afc(void)
{
    logi("afc mode.");
    mode_afc_enter();
    mode_afc_loop();
    mode_afc_exit();
}
#endif