#ifndef _TASK_MAIN_H_
#define _TASK_MAIN_H_
/*
#include "bt.h"
#include "music.h"
#include "record.h"
#include "fm.h"
#include "clock.h"
#include "usbdev.h"
#include "spdif_mode.h"
#include "sleep.h"
#include "poweroff.h"
#include "charge.h"
#include "event.h"
#include "local_play_manager.h"
#include "tone.h"
#include "key.h"
#include "disp.h"
#include "disk_update.h"
#include "info_mem.h"*/

//some modes of the main task:
enum mode_id_e {
    MODE_NULL,      // 0
    MODE_MUSIC,     // 1
    MODE_RECORD,    // 2
    MODE_BT,        // 3
    MODE_FM,        // 4
    MODE_LINEIN,    // 5
    MODE_USBDEV,    // 6
    MODE_SPDIF,     // 7
    MODE_G24,       // 8
    MODE_IDLE,      // 9
    MODE_SCENE_MAX, // 10

    MODE_SLEEP,     // 11
    MODE_POWEROFF,  // 12
    MODE_CHARGE,    // 13
    MODE_DISKUPDATE,// 14
    MODE_BTUPDATE,  // 15
    MODE_RESET,     // 16
    MODE_FCC,       // 17   
    MODE_AFC,       // 18

    MODE_SYSTEM_MAX,// 19
};

enum switch_reason_e {
    SW_INIT,
    SW_KEY,
    SW_AUTO,
    SW_BTBG,
    SW_SDCARD,
    SW_UDISK,
    SW_CHARGE,
    SW_STARTUP_FIRST,
    SW_HSW,
    SW_VBUSIN,
    SW_VBUSOUT,
    SW_LB,
    SW_BATFULL,
    SW_CHARGING_BIN,
    SW_UPDATE,
    SW_DISCON,
    SW_MODE_RESET,
};

typedef void (*mode_hook_t)(void);

typedef struct mode_info_s {
    uint32_t mode;
    mode_hook_t hook;
} mode_info_t;

typedef struct {
    enum mode_id_e sta;        //scene status
    enum mode_id_e last_sta;   //last scene status
    enum switch_reason_e switch_reason;
    bool exit_flag;
} scene_mode_t;

typedef struct {
    uint32_t val;
    uint32_t bcnt;
    uint32_t tcnt;
} knum_t;
extern knum_t knum;

typedef struct {
    bool DeviceDetectTimer;
    bool DispTimer;
    bool PmuTimer;
    bool PmuKeyTimer;
    bool SleepTimer;
    bool CpuLoadTimer;
    bool EqLoadTimer;
    bool UsbSofCheckTimer;
    bool UsbEdgeCheck;
    bool HidKeyDownTimer;
} timer_flag_t;

void var_init(void);
void mode_manager(void *pvParameters);
void common_event_process(uint32_t msg);
void bt_bg_preprocess(void);

void mode_update(int value);
void mode_set(uint32_t mode);
void mode_set_force(uint32_t mode);
void mode_switch(void);
void mode_switch_auto(void);
enum mode_id_e mode_get_sta(void);


void cpu_load_timer_cb(void *xTimer);
void app_framework_init(void);
void mode_manager(void *pvParameters);

bool startup_is_first(void);
void taskmain_init(void);
void mode_register(mode_info_t *mode, uint8_t size);
void common_register_event_handler(void (*handler)(uint32_t event));

void key_mode_set(bool flag);
bool key_mode_get(void);
void mode_switch_reason_set(uint32_t reason);
uint32_t mode_switch_reason_get(void);
void mode_last_sta_set(uint32_t state);
uint32_t mode_last_sta_get(void);
void mode_sta_set(uint32_t state);
uint32_t mode_sta_get(void);
void mode_exit_flag_set(bool flag);
bool mode_exit_flag_get(void);

void key_mode_exit_set(bool flag);

bool key_mode_exit_get(void);


enum overlay_type_e {
    OVERLAY_TYPE_G24,
    OVERLAY_TYPE_BT,
    OVERLAY_TYPE_USBDEV,
};

void as_overlay_code_load(enum overlay_type_e overlay_type);


#endif


