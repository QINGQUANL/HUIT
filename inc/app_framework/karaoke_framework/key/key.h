#ifndef _KEY_H_
#define _KEY_H_

#include "event.h"
#include "key_adc.h"
#include "key_ir.h"
#include "key_io.h"
#include "key_onoff.h"
#include "key_coding.h"
#include "key_digital.h"
//#include "irrx.h"
#include "gpio.h"
#include "sys_libc_common.h"


typedef enum {
    KEY_EVENT_NONE  = 0,
    KEY_EVENT_S     = KEY_EVENT | 0x00100000,   //short
    KEY_EVENT_U     = KEY_EVENT | 0x00200000,   //up
    KEY_EVENT_SU    = KEY_EVENT | 0x00300000,   //short up
    KEY_EVENT_L     = KEY_EVENT | 0x00400000,   //long
    KEY_EVENT_LU    = KEY_EVENT | 0x00500000,   //long up
    KEY_EVENT_H     = KEY_EVENT | 0x00600000,   //hold
    KEY_EVENT_M     = KEY_EVENT | 0x00700000,   //multiple click
    KEY_EVENT_KNOB  = KEY_EVENT | 0x00F00000,   //tune

    KEY_TWS         = (0x1<<16),                //slave -> master
} key_event_t;

#define KEY_CNT_S           5                   //5ms*5 = 25ms
#define KEY_CNT_U           5                   //5ms*5 = 25ms
#define KEY_CNT_L           300                 //5ms*150 = 750ms
#define KEY_CNT_H           30                  //5ms*30 = 150ms
#define KEY_CNT_LH          (KEY_CNT_L + KEY_CNT_H)

#define KEY_SCAN_CYCLE      5                   //扫描周期5ms

#define KEY_CNT_1000MS      1000                 //5ms*200 = 1000ms
#define KEY_CNT_2000MS      2000                 //5ms*400 = 2000ms
#define KEY_CNT_4000MS      4000                 //5ms*800 = 4000ms

#define KEY_ADC_CNT_L       20
#define KEY_ADC_CNT_H       5
#define KEY_ADC_CNT_LH      (KEY_ADC_CNT_L + KEY_ADC_CNT_H)

#define KEY_MASK            0xFF
#define KEY_NONE            0x00

#define KDIG_CNT_S           6                   //4ms*6 = 24ms
#define KDIG_CNT_U           6                   //4ms*6 = 24ms
#define KDIG_CNT_L           188                 //4ms*188 = 752ms
#define KDIG_CNT_H           125                 //4ms*125 = 500ms
#define KDIG_CNT_REL         130                 //single click release 4ms*130 = 520ms
#define KDIG_CNT_MONITOR     125                 //multi click monitor 4ms*125 = 500ms

//num key
#define KEY_NUM0            0x10
#define KEY_NUM1            0x11
#define KEY_NUM2            0x12
#define KEY_NUM3            0x13
#define KEY_NUM4            0x14
#define KEY_NUM5            0x15
#define KEY_NUM6            0x16
#define KEY_NUM7            0x17
#define KEY_NUM8            0x18
#define KEY_NUM9            0x19


//function key
#define KEY_PREV            0x20
#define KEY_PLAY            0x21
#define KEY_NEXT            0x22
#define KEY_VOL_DOWN        0x23
#define KEY_VOL_UP          0x24
#define KEY_MODE            0x25
#define KEY_REPEAT          0x26
#define KEY_EQ              0x27
#define KEY_PREV_VOLDOWN    0x28
#define KEY_NEXT_VOLUP      0x29
#define KEY_HSF             0x2A
#define KEY_POWER           0x2B
#define KEY_PLAY_POWER      0x2C
#define KEY_PLAY_MODE       0x2D
#define KEY_CH_DOWN         0x2E
#define KEY_CH_UP           0x2F
#define KEY_N100_PLUS       0x30
#define KEY_N10_PLUS        0x31
#define KEY_N10_MINUS       0x32
#define KEY_VSS             0x33
#define KEY_REC             0x34
#define KEY_SIRI            0x35
#define KEY_M               0x36
#define KEY_ONOFF           0x37
#define KEY_SW              0x38

#define KEY_NUM10           0x10
#define KEY_NUM11           0x11
#define KEY_NUM12           0x12
#define KEY_NUM13           0x13
#define KEY_NUM14           0x14
#define KEY_NUM15           0x15
#define KEY_NUM16           0x16
#define KEY_NUM17           0x17
#define KEY_NUM18           0x18

#define KEY_VOL0            0xA0
#define KEY_VOL1            0xA1
#define KEY_VOL2            0xA2
#define KEY_VOL3            0xA3
#define KEY_VOL4            0xA4
#define KEY_VOL5            0xA5
#define KEY_VOL6            0xA6
#define KEY_VOL7            0xA7
#define KEY_VOL8            0xA8
#define KEY_VOL9            0xA9


#define KEY_M_CLICK_NAME        (KEY_EVENT_SU | KEY_M)
#define KEY_M_CLICK_INTERVAL    (500/5)                 //500ms




typedef struct{
    uint16_t cur_val;
    uint16_t last_val;
    uint16_t cur_cnt;
    uint16_t up_cnt;
    uint32_t long_cnt;
} key_scan_t;


typedef struct{
    bool key_ad_enable;
    bool key_io_enable;
    bool key_onoff_enable;
} key_mode_ctl;

typedef struct{
    bool key_func_s_en;
    bool key_func_u_en;
    bool key_func_su_en;
    bool key_func_l_en;
    bool key_func_lu_en;
    bool key_func_h_en;
    bool key_func_ll_en;
    bool key_func_m_en;
    bool key_onoff_mode;
    uint16_t key_short_time;
    uint16_t key_up_time;
    uint16_t key_long_time;
    uint16_t key_hold_time;
} key_func_ctl;

void key_init(void);
uint32_t key_get_value();
void key_scan(void);
void key_set_ctl_func(key_func_ctl *key_ctl);
void mode_update(int value);
void *key_get_ctl_func(void);
uint32_t key_multiple_click_process(uint32_t key_value);

#endif

