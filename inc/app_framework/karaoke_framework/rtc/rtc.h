#ifndef _RTC_H_
#define _RTC_H_
#include "hal.h"


enum clock_sub_mode_e {
    CLOCK_IDLE_MODE,
    CLOCK_TIME_DISPLAY_MODE,
    CLOCK_ALARM_DISPLAY_MODE,
    CLOCK_TIME_SETTING_MODE,
    CLOCK_ALARM_SETTING_MODE,
    CLOCK_ALARM_PENDING_MODE,
};

enum params_e {
    SEL_YEAR    = 0,
    SEL_MONTH   = 1,
    SEL_DAY     = 2,
    SEL_HOUR    = 3,
    SEL_MINUTE  = 4,
    SEL_SECOND  = 5,
};

struct rtc_t {
    RTC_TimeTypeDef t;
    RTC_TimeTypeDef alarm_time;
    RTC_TimeTypeDef cfg_t;
    RTC_TimeTypeDef cfg_alarm_t;
    int params_select;
    bool alarm_enable;
};
extern struct rtc_t rtc;





void rtc_init(rtc_cb *rtc_app_cb,bool rtc_en, bool sleep_en,bool arm0_en,bool arm1_en);
void rtc_deinit(void);
void rtc_alarm_set(void);
void rtc_alarm_sandglass_set(uint32_t sec, bool enable);
void rtc_alarm_get(void);
void rtc_alarm_switch(void);
//void rtc_select_next_time_param(void);
//void rtc_config_time_param(int inc);
//void rtc_select_next_alarm_param(void);
//void rtc_config_alarm_param(int inc);
uint32_t rtc_get_second(void);
void rtc_init_time(RTC_TimeTypeDef *time);


#endif
