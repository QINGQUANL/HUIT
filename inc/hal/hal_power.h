
#ifndef _HAL_POWER_LIB_H_
#define _HAL_POWER_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys_types.h"
#include "oal.h"

enum pm_prio_type {
    PM_ARCH = 0,          /* arch operation */
    PM_SYSTEM,            /* system operation */
    PM_LATE_DEVICE,       /* late device operation */
    PM_EARLY_DEVICE,      /* early device operation */
};

enum pm_notify_cmd_type {
    PM_SUSPEND_NOTIFY = 0,
    PM_RESUME_NOTIFY,
    PM_SHUTDOWN_NOTIFY,
};

enum power_supply_type {
    POWER_SUPPLY_UNKNOWN = 0,   /* unknown power supply */
    POWER_SUPPLY_BATTERY,       /* battery power supply */
    POWER_SUPPLY_USB,           /* USB power supply */
    POWER_SUPPLY_WIRELESS,      /* wireless power supply */
};


/* Battery&Charger info */
struct power_supply_info {
    uint32_t battery_level;     /* level:0 ~ 100; 0:empty; 100:full */
    uint32_t type;              /* see @power_supply_type */
    uint32_t voltage;           /* voltage(uV) of power supply */
    uint32_t current;           /* current(uA) of power supply */
};

/*
 * Twait = cal_prd_base * (2 ^ rc_cal_prd) * 64 * 20us
 * Tcal  = rc_cal_num * 4 * 20us
 */
struct rc_cal_paras {
    uint32_t cal_prd_base;
    uint32_t rc_cal_prd;
    uint32_t rc_cal_num;
    bool continous_mode;
    bool enable;
};


void HAL_watchdog_init(uint32_t sec);
void HAL_watchdog_deinit(void);
void HAL_watchdog_refresh(void *xTimer);
void HAL_system_restart(uint32_t sec);
void power_test(void);
void HAL_set_rc_cal_paras(uint32_t cal_prd_base, uint32_t rc_cal_prd, uint32_t rc_cal_num, bool continous_mode, bool enable);
void HAL_set_rc_cal_reg(void);
void HAL_rtc_prcm_init(void);
void HAL_coreldo_enable(void);
void HAL_coreldo_disable(void);
unsigned int HAL_coreldo_normal_vol_set(unsigned int vol);
unsigned int HAL_coreldo_lp_vol_set(unsigned int vol);
uint32_t HAL_enter_sleep_mode(void);
uint32_t HAL_enter_poweroff_mode(void);

#ifdef __cplusplus
}
#endif

#endif /* _HAL_POWER_LIB_H_ */
