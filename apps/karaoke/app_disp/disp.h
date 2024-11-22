#ifndef _DISP_H_
#define _DISP_H_


#include "led_digital.h"
#include "led_diode.h"
#include "lcd_seg.h"
#include "ledc.h"
#include "sys_libc_common.h"
#include "event.h"
#include "local_play_manager.h"
#include "device.h"
#include "mode_manager.h"


typedef struct {
    uint32_t menu;
    uint32_t count;
    uint8_t sync_state;
} disp_t;

#define DISP_INTERVAL_TIME      5                         //interval time, unit:ms
#define DISP_DURATION_TIME      (2000/DISP_INTERVAL_TIME)   //duration time: 2s

enum disp_menu_e {
    MENU_WELCOME        = 0,
    MENU_SCENE_MAIN,
    MENU_NUM,
    MENU_INPUT_NUM,
    MENU_VOLUME,
    MENU_EQ,
    MENU_REPEAT,
    MENU_WAITING,
    MENU_MUSIC_PLAY,
    MENU_MUSIC_STOP,
    MENU_CLOCK_TIME,
    MENU_FM_FREQ,
    MENU_FM_CHANNEL,
    MENU_RECORD_ING,
    MENU_RECORD_PLAY,
};

enum disp_state_e {
    //system
    STATE_POWERON       = 0,
    STATE_POWEROFF,
    STATE_RESET,

    //charge
    STATE_CHARGEING,
    STATE_UNCHARGEING,
    STATE_BAT_FULL,
    STATE_BAT_LOW,
    STATE_BAT_UNLOW,

    //bt
    STATE_BT_PAIR,//8
    STATE_BT_CON,
    STATE_BT_DIS,
    STATE_BT_PEER_CON,
    STATE_BT_PEER_DIS,//12

    STATE_BT_CALLING,//13
    STATE_BT_CALLING_END,//14

    STATE_CLEAN_LINK,//15
    STATE_ENTER_DUT,//16
    STATE_BT_INCOMING,//17

    STATE_BT_PLAY,//18
    STATE_BT_PAUSE,//19

    STATE_DISP_SUSPEN,//20
    STATE_DISP_RESUME,//21

    STATE_DISP_SLEEP,//22
    STATE_DISP_WAKEUP,//23
};

enum disp_onoff_e {
    DISP_ON             = 0,
    DISP_OFF,
    DISP_FLASH,
    DISP_ON_DELAY_OFF,
    DISP_FLASH_INTERVAL,
    DISP_FLAGH_LOOP,
};

typedef struct {
    uint32_t clkn;
    uint32_t finecnt;
    uint32_t delay_t;
    uint8_t disp_state;
} tws_disp_time_info_t;

void disp_menu_set(uint32_t menu);
uint32_t disp_menu_get(void);
void disp_count_set(uint32_t count);
uint32_t disp_count_get(void);
void app_disp_init(void);
void disp_timer_cb(void* xTimer);
void disp_menu(enum disp_menu_e menu, uint32_t val0, uint32_t va11);
void disp_state(enum disp_state_e state);
void ui_led_diode_process(enum disp_state_e state);
void disp_state_sync(enum disp_state_e state,uint16_t delt_time);
void disp_timer_sync_start(uint32_t ms, enum disp_state_e state);
void disp_timer_sync_cb(void* xTimer);
void disp_timer_onoff(bool onoff);
void disp_slave_state_sync( uint8_t *params,uint32_t params_len);

#endif

