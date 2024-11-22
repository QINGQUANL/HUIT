#ifndef _G24_H_
#define _G24_H_

#include "event.h"

enum G24_CMD_type {
    G24_CMD_PREV = 0x00,
    G24_CMD_NEXT,
    G24_CMD_PLAY,
    G24_CMD_VOL_DN,
    G24_CMD_VOL_UP,
    G24_CMD_DEL_LINKKEY,
    G24_CMD_POWEROFF,
    G24_CMD_MONITOR_CHECK_MASTER_ID,    //handset的master mic要检测主MIC ID
};

void g24_init(uint8_t role);

void mode_g24(void);
void mode_g24_enter(void);
void mode_g24_loop(void);
void mode_g24_exit(void);

void task_comm();
void comm_init(void);
void g24_lm_init();
void *task_bt_isr(void *pvParameters);
void *task_bt_lmp(void *pvParameters);

void reconnect_rssi_en_set(bool en);
void g24_register_event_handler(void (*handler)(sys_event_info_t *info));
void rf_drift_set(uint32_t cap);
void bt_frontend_init(uint8_t type);

short *g24_freq_int_get(void);
int *g24_freq_frac_get(void);
void g24_rf_power_force(uint8_t enable, uint8_t power);
void g24_rf_maxpower_set(uint8_t enable, uint8_t power);

#endif

