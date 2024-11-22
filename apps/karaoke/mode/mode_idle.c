#define LOG_LEV     4
#define SLTK_LOG_TAG "idle"
#include "sys_inc.h"
#include "user_config.h"
#include "middleware.h"
#include "hal.h"
#include "karaoke_framework.h"
#include "disp.h"
#include "app_pmu.h"
#include "app_psm.h"
#include "audio_service.h"

void mode_idle_enter()
{
    logi("mode idle ");
    HAL_clk_set_cpux_freq(32000000, 32000000);  //时钟频率设置到32M
#if AUDIO_KARAOKE_LINEIN_EN
    //audio_service_disable_input_source(audio_linein_mode_input_sel_get());
#endif
#if AUDIO_KARAOKE_MIC_EN
    //audio_service_disable_input_source(audio_rec_input_sel_get());
#endif

}

void mode_idle_exit()
{
    logi("mode idle ");
}

void mode_idle_event_process(sys_event_info_t *info)
{
#if WATCHDOG_EN
    HAL_watchdog_refresh(NULL);
#endif

    uint32_t event = info->event;
    switch(event)
    {
        case KEY_EVENT_SU | KEY_NUM18:
            logi("key18");
            mode_set_force(MODE_BT);
            break;
    }
}

void mode_idle_loop()
{
    sys_event_info_t info;
    while(mode_sta_get() == MODE_IDLE) {
        info.event = event_get(info.params, &info.params_len, true);
        mode_idle_event_process(&info);
    }
}

void mode_idle()
{
    mode_idle_enter();
    mode_idle_loop();
    mode_idle_exit();
}


