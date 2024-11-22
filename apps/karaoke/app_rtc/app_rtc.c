#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "rtc.h"
#include "event.h"
#include "mode_manager.h"
#include "device.h"
#include "app_psm.h"

volatile uint32_t global_seconed = 0;

#if RTC_EN
rtc_cb rtc_app_cb;
#endif

#if RTC_EN
AT_SRAM(.rtc)
void rtc_second_cb(void)
{
    global_seconed++;
    if (mode_get_sta() != MODE_CHARGE && device_startup_get()) {
        event_put(SYSTEM_EVENT_SECOND, NULL, 0);
    } else if(!SLEEP_EN) {
        event_put(SYSTEM_EVENT_SECOND, NULL, 0);
    }
}

AT_SRAM(.rtc)
uint32_t rtc_get_second(void)
{
    return global_seconed;
}
#else
AT_SRAM(.rtc)
uint32_t rtc_get_second(void)
{
    return 0;
}
#endif

#if RTC_CLOCK_EN
void rtc_time_display(void)
{
    HAL_rtc_get_time(&rtc.t);
    if (0 == rtc.t.second) {
        logi("rtc:%04d-%02d-%02d, %02d:%02d:%02d",
            rtc.t.year+BASE_YEAR, rtc.t.month, rtc.t.day,
            rtc.t.hour, rtc.t.minute, rtc.t.second);
    }

}


#endif


#if RTC_ALARM0_EN
AT_SRAM(.rtc_alarm)
void rtc_alarm0_cb(void)
{
    event_put(RTC_EVENT_ALM0, NULL, 0);
}
#endif


#if RTC_ALARM1_EN
AT_SRAM(.rtc_alarm)
void rtc_alarm1_cb(void)
{
    event_put(RTC_EVENT_ALM1, NULL, 0);
}
#endif


void app_rtc_init(void)
{
    HAL_clk_dcxo_set(RTC_DCXO_CUR, RTC_DCXO_LP_EN, RTC_XO32K_EN);

#if RTC_EN
#if RTC_ALARM0_EN
    rtc_app_cb.app_alarm0_irq_callback = rtc_alarm0_cb;
#endif

#if RTC_ALARM1_EN
    rtc_app_cb.app_alarm1_irq_callback = rtc_alarm1_cb;
#endif
    rtc_app_cb.app_second_irq_callback = rtc_second_cb;
    rtc_init(&rtc_app_cb, RTC_EN, SLEEP_EN, RTC_ALARM0_EN, RTC_ALARM1_EN);
#else
    rtc_init(NULL, RTC_EN, SLEEP_EN, RTC_ALARM0_EN, RTC_ALARM1_EN);
#endif

#if RTC_CLOCK_EN
    if(!psm_is_rtc_time_has_set()) {
        RTC_TimeTypeDef t;
        t.year = 0;     //the BASE_YEAR is defined 2018
        t.month = 1;
        t.day = 1;
        t.hour = 8;
        t.minute = 0;
        t.second = 0;
        t.weekday = MONDAY;
        rtc_init_time(&t);

        psm_rtc_time_flag_set();
    }
#endif

}



