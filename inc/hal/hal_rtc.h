#ifndef _HAL_RTC_LIB_H_
#define _HAL_RTC_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif


#define ALARMID_0 0
#define ALARMID_1 1

/* BASE_YEAR is the rtc year 0 */
#define BASE_YEAR 2000  /* used in yp3 */

/* check hour, minute, second parameter */
#define IS_RTC_HOUR24(HOUR)     ((HOUR) <= (uint32_t)23)
#define IS_RTC_MINUTE(MINUTE)   ((MINUTE) <= (uint32_t)59)
#define IS_RTC_SECOND(SECOND)   ((SECOND) <= (uint32_t)59)

/* check year, month, date weekday parameter */
#define IS_RTC_YEAR(YEAR)       ((YEAR) <= (uint32_t)127)
#define IS_RTC_MONTH(MONTH) \
    (((MONTH) >= (uint32_t)1) && ((MONTH) <= (uint32_t)12))

#define IS_RTC_DAY(DAY)    \
    (((DAY) >= (uint32_t)1) && ((DAY) <= (uint32_t)31))

#define IS_RTC_WEEKDAY(WEEKDAY)             \
    (((WEEKDAY) == RTC_WEEKDAY_MONDAY) ||   \
    ((WEEKDAY) == RTC_WEEKDAY_TUESDAY) ||   \
    ((WEEKDAY) == RTC_WEEKDAY_WEDNESDAY) || \
    ((WEEKDAY) == RTC_WEEKDAY_THURSDAY) ||  \
    ((WEEKDAY) == RTC_WEEKDAY_FRIDAY) ||    \
    ((WEEKDAY) == RTC_WEEKDAY_SATURDAY) ||  \
    ((WEEKDAY) == RTC_WEEKDAY_SUNDAY))

/* check leap year */
#define IS_RTC_LEAP_YEAR(YEAR) \
    ( (((YEAR) + BASE_YEAR)%4 == 0 && ((YEAR) + BASE_YEAR)%100 != 0) || \
    ((YEAR) + BASE_YEAR)%400 == 0)

/* check alarm id */
#define IS_ALARM_ID(ID) ( ALARMID_0 == (ID) || ALARMID_1 == (ID))

typedef struct {
    uint16_t year;      //0~127
    uint8_t month;      //1~12
    uint8_t day;        //1~31
    uint16_t weekday;   //0~6
    uint8_t hour;       //0~23
    uint8_t minute;     //0~59
    uint8_t second;     //0~59
} RTC_TimeTypeDef;

/*  WeekDay Define */
typedef enum {
    MONDAY          = 0x0,
    TUESDAY         = 0x1,
    WEDNESDAY       = 0x2,
    THURSDAY        = 0x3,
    FRIDAY          = 0x4,
    SATURDAY        = 0x5,
    SUNDAY          = 0x6,
} WeekDayDef;

/*  month Define */
typedef enum {
    JANUARY         = 1,
    FEBRUARY        = 2,
    MARCH           = 3,
    APRIL           = 4,
    MAY             = 5,
    JUNE            = 6,
    JULY            = 7,
    AUGUST          = 8,
    SEPTEMBER       = 9,
    OCTOBER         = 10,
    NOVEMBER        = 11,
    DECEMBER        = 12,
} MonthDef;

/* WeekDay Mask Define */
typedef enum {
    MONDAY_MASK         = 0x1,
    TUESDAY_MASK        = 0x2,
    WEDNESDAY_MASK      = 0x4,
    THURSDAY_MASK       = 0x8,
    FRIDAY_MASK         = 0x10,
    SATURDAY_MASK       = 0x20,
    SUNDAY_MASK         = 0x40,
} WeekDayMaskDef;

typedef struct {
    union {
        uint32_t count;         //for count alarm
        RTC_TimeTypeDef t;      //for week alarm
    } cfg;

    union {
        bool enable;            //for count alarm
        uint8_t weekday_mask;   //for week alarm
    } status;
} RTC_AlarmConfigDef;

enum {
    RTC_CLK_HOSC = 0,
    RTC_CLK_LOSC,
};

enum {
    RTC_MODE_NORMAL = 0,
    RTC_MODE_SIM,
};

typedef struct {
    void (*app_alarm0_irq_callback)();
    void (*app_alarm1_irq_callback)();
    void (*app_second_irq_callback)();
} rtc_cb;


void HAL_rtc_module_init(rtc_cb *rtc_cb);
void rtc_module_deinit(void);
void HAL_rtc_alarm_en(uint32_t alarm_id);
void HAL_rtc_set_time(RTC_TimeTypeDef *time);
void HAL_rtc_get_time(RTC_TimeTypeDef *time);
void HAL_rtc_set_alarm(uint32_t alarm_id, RTC_AlarmConfigDef config);
void HAL_rtc_get_alarm(uint32_t alarm_id, RTC_AlarmConfigDef *config);
void HAL_rtc_clk_sel(uint32_t clksel);
void rtc_mode_sel(uint32_t modesel);
void HAL_rtc_enable_wakeup(uint32_t wakeup);
void HAL_rtc_disable_wakeup(uint32_t wakeup);
void HAL_rtc_memory_init(void);

/*
 * 参数addr的范围：0x00 ~ 0x0F
 * 每次写入4 byte
 */
void HAL_rtc_memory_write(uint32_t addr, uint32_t data);

/*
 * 参数addr的范围：0x00 ~ 0x0F
 * 每次读出4 byte
 */
uint32_t HAL_rtc_memory_read(uint32_t addr);

void rtc_second_cb(void);
void rtc_alarm0_cb(void);
void rtc_alarm1_cb(void);
uint32_t HAL_rtc_get_rc_freq(void);
uint32_t HAL_rtc_get_rc_freq_fraction(void);
uint32_t HAL_rtc_get_bt_cnt_sta(void);
void HAL_rtc_set_bt_cnt_sta(uint32_t val);

#ifdef __cplusplus
}
#endif

#endif
