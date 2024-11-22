#define LOG_LEV     4
#define SLTK_LOG_TAG "poweroff"
#include "sys_inc.h"
#include "user_config.h"
#include "middleware.h"
#include "hal.h"
#include "karaoke_framework.h"
#include "disp.h"
#include "audio_player.h"
#include "app_pmu.h"
#include "app_audio_anc.h"
#include "app_psm.h"


extern uint8_t ui_poweroff_flag;

void mode_poweroff(void)
{
    logi("poweroff mode.");

#if CHARGING_BIN_MID_LEVEL
    if (HAL_pmu_get_vbus_mid_state()) {
        HAL_pmu_enable_5kpd(false, false);
    } else {
        HAL_pmu_enable_5kpd(false, true);
    }
#endif

    if (HAL_pmu_is_use_hsw()) {
        HAL_pmu_hsw_reset_enable(true);
    }



#if (TONE_EN && TONE_POWER_OFF_EN)
    logi("poweroff tone play.");
    if (!ui_poweroff_flag) {
        am_tone_play(SYS_TONE_POWER_OFF_NAME, 1, 0, TONE_POWER_OFF_VOL, 0, 0);//播放关机提示音
#if (DISP_EN && STATE_POWEROFF_EN)
        disp_state(STATE_POWEROFF);
#endif

    }
#endif

    HAL_mixer_set_ctl(SND_CODEC, DAC_POWEROFF, 1);

#if (MODE_MUSIC_EN && MUSIC_INFO_SAVE_MODE == 2)
    play_info_write_when_poweroff();
#endif
#if BT_VOLUME_EN
    bt_volume_info_save();
#endif


    am_volume_info_save();
#if MODE_G24_EN
    //g24_link_info_save();
#endif

    link_info_save();
#if BT_AIR_SEL
    bt_name_info_save();
#endif
#if AUDIO_ANC_EN
    poweroff_anc_mode_param_set();
#endif

#if MODE_MUSIC_EN
    /* 务必在info_write_page()后 */
    player_store_priv_data_to_flash();
#endif

    /* PC3/4在软关机（VBAT引脚保持有电）中，不会像其他GPIO那样自动处于低电平状态，
    * 需要手动配置GPIO的功能和状态，例如输出0。
    */
    HAL_gpio_value_set(PIN_C3, 0);
    HAL_gpio_value_set(PIN_C4, 0);
    HAL_gpio_func_set(PIN_C3, PIN_FUNCOUTPUT);
    HAL_gpio_func_set(PIN_C4, PIN_FUNCOUTPUT);

    /* suspend all tasks */
    OAL_thread_all_suspend(); //FIXME: or vTaskEndScheduler() ?
    INTC_IRQ_Suspend();
    pmu_deinit();
    HAL_watchdog_deinit();

    HAL_mdelay(POWER_OFF_DELAY);
#if !BAT_EXIST
    HAL_pmu_set_poweron_wakeupsrc(PMU_VBUSIN_WAKEUPSRC, false);
#endif

#if CHARGING_BIN
#if !CHARGING_BIN_CODE
    if (mode_switch_reason_get() == SW_BATFULL) {
        logi("bat full disable vbus wakesrc");
#if CHARGING_BIN_MID_LEVEL
        HAL_pmu_set_poweron_wakeupsrc(PMU_VBUSMIDIN_WAKEUPSRC, false);
        //因为打开了vbus out和vbus mid out中断电平触发方式，只要vbus为0就会导致唤醒，所以关闭vbus out唤醒源
        HAL_pmu_set_poweron_wakeupsrc(PMU_VBUSOUT_WAKEUPSRC, false);
        HAL_pmu_set_poweron_wakeupsrc(PMU_ONOFFL_WAKEUPSRC, false);
#else
        HAL_pmu_set_poweron_wakeupsrc(PMU_VBUSMIDIN_WAKEUPSRC, false);
        HAL_pmu_set_poweron_wakeupsrc(PMU_VBUSIN_WAKEUPSRC, false);
#endif
    }
#endif
#endif

    HAL_set_rc_cal_paras(RC_CAL_PRD_BASE, RC_CAL_PRD, RC_CAL_NUM, RC_CAL_CON_MODE, RC_POWEROFF_CAL_EN);
#if RTC_EN
    HAL_rtc_clk_sel(RTC_CLK_LOSC);
#endif

#if (G24_2T4_SHORT_PACKET_3TIMES || G24_2T4_WITH_MONO_MONITOR) && G24_FUNC_ROLE
    G24_2T4_POWER_OFF();
#endif

    enter_poweroff_mode();
}
