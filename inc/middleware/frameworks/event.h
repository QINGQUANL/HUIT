#ifndef _EVENT_H_
#define _EVENT_H_

#include "sys_inc.h"
#include "oal.h"

#define EVENT_PARAMS_SIZE (16)

typedef struct {
    uint32_t event;
    uint32_t params_len;
    uint8_t params[EVENT_PARAMS_SIZE];
} sys_event_info_t;

#define COMMON_MSG_SIZE                 36
#define PLAYER_MSG_SIZE                 10

#define EVENT_GROUP_MASK                (0xFF000000)

#define NONE_EVENT                      0
#define SENSOR_EVENT                    0x50000000
#define DISP_EVENT                      0x60000000
#define RTC_EVENT                       0x70000000
#define PMU_EVENT                       0x80000000
#define PM_CODE_EVENT                   0x81000000
#define EQ_EVENT                        0x90000000
#define DEVICE_EVENT                    0xA0000000
#define USB_EVENT                       0xA1000000
#define SYSTEM_EVENT                    0xB0000000
#define TEST_EVENT                      0xB1000000
#define MP_TEST_EVENT                   0xB2000000
#define KEY_EVENT                       0xC0000000
#define UI_EVENT                        0xC1000000
#define BT_EVENT                        0xD0000000
#define LE_EVENT                        0xD1000000
#define TWS_EVENT                       0xD2000000
#define AIR_EVENT                       0xD3000000
#define PLAYER_EVENT                    0xF0000000
#define BTAUDIO_EVENT                   0xF1000000
#define TWS_AUDIO_EVENT                 0xF2000000
#define AUDIO_KWS_EVENT                 0xF3000000
#define AUDIO_ANC_EVENT                 0xF4000000
#define G24_EVENT                       0xF5000000
#define APP_SYNC_EVENT                  0xF6000000
#define JUPLUS_EVENT                    0xF7000000
#define EVENT_DEBUG                     0xFFFFFFFF

enum tws_audio_event_e {
    TWS_AUDIO_EVENT_NONE                = TWS_AUDIO_EVENT | 0x00,
    TWS_AUDIO_EVENT_A2DP_START          = TWS_AUDIO_EVENT | 0x01,
    TWS_AUDIO_EVENT_A2DP_STOP           = TWS_AUDIO_EVENT | 0x02,
    TWS_AUDIO_EVENT_SCO_START           = TWS_AUDIO_EVENT | 0x03,
    TWS_AUDIO_EVENT_SCO_STOP            = TWS_AUDIO_EVENT | 0x04,
    TWS_AUDIO_EVENT_PLAY_TONE           = TWS_AUDIO_EVENT | 0x05,
    TWS_AUDIO_EVENT_STOP_TONE           = TWS_AUDIO_EVENT | 0x06,

    TWS_AUDIO_EVENT_CHANGE_MUSIC_VOLUME       = TWS_AUDIO_EVENT | 0x07,
    TWS_AUDIO_EVENT_NOTIFY_MUSIC_VOLUME_DOWN  = TWS_AUDIO_EVENT | 0x08,
    TWS_AUDIO_EVENT_NOTIFY_MUSIC_VOLUME_UP    = TWS_AUDIO_EVENT | 0x09,

    TWS_AUDIO_EVENT_CHANGE_CALL_VOLUME       = TWS_AUDIO_EVENT | 0x0A,
    TWS_AUDIO_EVENT_NOTIFY_CALL_VOLUME_DOWN  = TWS_AUDIO_EVENT | 0x0B,
    TWS_AUDIO_EVENT_NOTIFY_CALL_VOLUME_UP    = TWS_AUDIO_EVENT | 0x0C,

    TWS_AUDIO_EVENT_INCOMING_TONE_STEP1 = TWS_AUDIO_EVENT | 0x0D,
    TWS_AUDIO_EVENT_INCOMING_TONE_STEP2 = TWS_AUDIO_EVENT | 0x0E,
    TWS_AUDIO_EVENT_INCOMING_NUMBER     = TWS_AUDIO_EVENT | 0x0F,

    TWS_AUDIO_EVENT_ANC_SWITCH          = TWS_AUDIO_EVENT | 0x10,
    TWS_AUDIO_EVENT_ANC_STATUS_UPDATE   = TWS_AUDIO_EVENT | 0x11,

    TWS_AUDIO_EVENT_NOTIFY_PLAY_TONE    = TWS_AUDIO_EVENT | 0x12,
    TWS_AUDIO_EVENT_GAME_MODE           = TWS_AUDIO_EVENT | 0x13,
    TWS_AUDIO_EVENT_GAME_MODE_FIX       = TWS_AUDIO_EVENT | 0x14,
    TWS_AUDIO_EVENT_WAIT_BTAUDIO_IDLE   = TWS_AUDIO_EVENT | 0x15,
    TWS_AUDIO_EVENT_VOLTAB_UPDATE       = TWS_AUDIO_EVENT | 0x16,
    TWS_AUDIO_EVENT_ANC_SUSPEND         = TWS_AUDIO_EVENT | 0x17,
    TWS_AUDIO_EVENT_GAME_MODE_PROXY     = TWS_AUDIO_EVENT | 0x18,
    TWS_AUDIO_EVENT_EQCONFIG_ID         = TWS_AUDIO_EVENT | 0x19,
    TWS_AUDIO_EVENT_EQCONFIG_PROXY      = TWS_AUDIO_EVENT | 0x1A,
    TWS_AUDIO_EVENT_SPACE_MODE_PROXY    = TWS_AUDIO_EVENT | 0x1B,

    TWS_AUDIO_EVENT_SPACE_MODE_FIX      = TWS_AUDIO_EVENT | 0x20,

    TWS_AUDIO_EVENT_ADDA_OPEN           = TWS_AUDIO_EVENT | 0x21,
    TWS_AUDIO_EVENT_ADDA_CLOSE          = TWS_AUDIO_EVENT | 0x22,
    TWS_AUDIO_EVENT_TONEPLAY_PROXY      = TWS_AUDIO_EVENT | 0x23,
    TWS_AUDIO_EVENT_LOCAL_TALK_STA_UPDATE = TWS_AUDIO_EVENT | 0x24,
    TWS_AUDIO_EVENT_TALK_STA_UPDATE     = TWS_AUDIO_EVENT | 0x25,
    TWS_AUDIO_EVENT_ANC_ADPT_UPDATE     = TWS_AUDIO_EVENT | 0x26,
    TWS_AUDIO_EVENT_LOCAL_BACKGROUND_UPDATE = TWS_AUDIO_EVENT | 0x27,
    TWS_AUDIO_EVENT_APP_TALK_STA_RESET  = TWS_AUDIO_EVENT | 0x28,
};

enum audio_kws_event_e {
    AUDIO_KWS_EVENT_NONE                = AUDIO_KWS_EVENT | 0x00,
    AUDIO_KWS_EVENT_WAKEUP              = AUDIO_KWS_EVENT | 0x01,
    AUDIO_KWS_EVENT_REQ_STATUS_UPDATE   = AUDIO_KWS_EVENT | 0x02,
    AUDIO_KWS_EVENT_REQ_SLAVE_SWITCH    = AUDIO_KWS_EVENT | 0x03,
};

enum audio_anc_event_e {
    AUDIO_ANC_EVENT_NONE                    = AUDIO_ANC_EVENT | 0x00,
    AUDIO_ANC_EVENT_TEST                    = AUDIO_ANC_EVENT | 0x01,
    AUDIO_ANC_EVENT_FF_PGA_GAIN_SET         = AUDIO_ANC_EVENT | 0x02,
    AUDIO_ANC_EVENT_FB_PGA_GAIN_SET         = AUDIO_ANC_EVENT | 0x03,
    AUDIO_ANC_EVENT_DAC_PGA_GAIN_SET        = AUDIO_ANC_EVENT | 0x04,
    AUDIO_ANC_EVENT_DAC_DIGITAL_GAIN_SET    = AUDIO_ANC_EVENT | 0x05,
    AUDIO_ANC_EVENT_COEFF_SET               = AUDIO_ANC_EVENT | 0x06,
    AUDIO_ANC_EVENT_PROXY                   = AUDIO_ANC_EVENT | 0x07,

};

enum sensor_event_e {
    SENSOR_EVENT_NONE                   = 0,
    SENSOR_EVENT_G                      = SENSOR_EVENT | 0x01,  //g sensor
    SENSOR_EVENT_P_LT                   = SENSOR_EVENT | 0x10,  //proximity sensor，远距离
    SENSOR_EVENT_P_HT                   = SENSOR_EVENT | 0x11,  //proximity sensor，近距离
    SENSOR_EVENT_P_LT_TIMER_START       = SENSOR_EVENT | 0x12,  //proximity sensor，出耳定时器启动
    SENSOR_EVENT_P_LT_TIMER_STOP        = SENSOR_EVENT | 0x13,  //proximity sensor，出耳定时器关闭
    SENSOR_EVENT_P_LT_TIMER_STA_CLEAR   = SENSOR_EVENT | 0x14,  //proximity sensor，出耳超时状态清0
    SENSOR_EVENT_P_HT_NEED_PLAY_CLEAR   = SENSOR_EVENT | 0x15,  //proximity sensor，入耳恢复播放变量清0
    SENSOR_EVENT_P_HT_NEED_PLAY_SET     = SENSOR_EVENT | 0x16,  //proximity sensor，入耳恢复播放变量置1
    SENSOR_EVENT_P_SYNC_STATE           = SENSOR_EVENT | 0x17,  //proximity sensor，副耳同步状态给主耳
    SENSOR_EVENT_P_HT_1                 = SENSOR_EVENT | 0x18,  //proximity sensor，近距离
    SENSOR_EVENT_MOTION_ENABLE          = SENSOR_EVENT | 0x19,  //motion sensor，enable
    SENSOR_EVENT_SLIP_DOWM              = SENSOR_EVENT | 0x20,  //下划
    SENSOR_EVENT_SLIP_UP                = SENSOR_EVENT | 0x21,  //下划
    SENSOR_EVENT_DATA_TIMEOUT           = SENSOR_EVENT | 0x22,  //陀螺仪数据超时

    SENSOR_TWS                          = (0x1<<16),            //副耳发送给主耳
};

enum disp_event_e {
    DISP_EVENT_NONE                     = 0,
    DISP_EVENT_UPDATE                   = DISP_EVENT | 0x01,
    DISP_EVENT_SWITCH                   = DISP_EVENT | 0x02,
};

enum ui_event_e {
    UI_EVENT_NONE                       = UI_EVENT | 0x00,
    UI_EVENT_POWEROFF                   = UI_EVENT | 0x01,
    UI_EVENT_NTC_TEMP                   = UI_EVENT | 0x02,
    UI_EVENT_ANC_SET_NOTIFY_APP         = UI_EVENT | 0x03,
    UI_EVENT_VBUS_PRINT_SET             = UI_EVENT | 0x04,
};

enum rtc_event_e {
    RTC_EVENT_NONE                      = 0,
    RTC_EVENT_SECOND                    = RTC_EVENT | 0x01,
    RTC_EVENT_ALM0                      = RTC_EVENT | 0x02,
    RTC_EVENT_ALM1                      = RTC_EVENT | 0x03,
};


enum eq_event_e {
    EQ_EVENT_NONE                       = 0,
    EQ_EVENT_SET_MODE_EQ                = EQ_EVENT | 0x01,
    EQ_EVENT_SET_MODE_AUDIO             = EQ_EVENT | 0x02,
    EQ_EVENT_SET_EQ_DATA                = EQ_EVENT | 0x03,
    EQ_EVENT_SET_EQ_DATA_ISR            = EQ_EVENT | 0x04,
    EQ_EVENT_UPLOAD_RAM_EQ              = EQ_EVENT | 0x05,
    EQ_EVENT_HW_CHANGE                  = EQ_EVENT | 0x06,
    EQ_EVENT_AFX_TEST                   = EQ_EVENT | 0x80,
};

enum device_event_e  {
    DEVICE_EVENT_NONE                   = 0,
    DEVICE_EVENT_SD_IN                  = DEVICE_EVENT | 0x01,  //sdcard in
    DEVICE_EVENT_SD_OUT                 = DEVICE_EVENT | 0x02,  //sdcard out
    DEVICE_EVENT_SD_FAIL                = DEVICE_EVENT | 0x03,  //sdcard fail
    DEVICE_EVENT_LINE_IN                = DEVICE_EVENT | 0x04,
    DEVICE_EVENT_LINE_OUT               = DEVICE_EVENT | 0x05,

    DEVICE_EVENT_SD_1_IN                = DEVICE_EVENT | 0x06,  //sdcard in
    DEVICE_EVENT_SD_1_OUT               = DEVICE_EVENT | 0x07,  //sdcard out
    DEVICE_EVENT_SD_1_FAIL              = DEVICE_EVENT | 0x08,  //sdcard fail
};

enum usb_sub_event_e {
    USB_SUBEVENT_USB_CABLE_IN = 1,
    USB_SUBEVENT_PC_ARRIVAL,

    USB_SUBEVENT_BUS_NO_RESPOND = 0xEE,
};

enum usb_event_e {
    USB_EVENT_NONE                      = 0,
    USB_EVENT_PC_CONNECT                = USB_EVENT | 0x01,
    USB_EVENT_PC_CONNECT_TIMEOUT        = USB_EVENT | 0x02,
    USB_EVENT_PC_RX_SOF                 = USB_EVENT | 0x03,
    USB_EVENT_PCM_VOLUME                = USB_EVENT | 0x04,     //pc pcm volume set
    USB_EVENT_MIC_VOLUME                = USB_EVENT | 0x05,     //pc mic volume set
    USB_EVENT_TEST_UNIT                 = USB_EVENT | 0x06,
    USB_EVENT_PLAY_PARAMS_CHANGE        = USB_EVENT | 0x07,
    USB_EVENT_REC_PARAMS_CHANGE         = USB_EVENT | 0x08,
    USB_EVENT_CABLE_IN                  = USB_EVENT | 0x09,
    USB_EVENT_CABLE_OUT                 = USB_EVENT | 0x0A,
    USB_ENUM_SETUP_TRANSACTION          = USB_EVENT | 0x0B,
    USB_EVENT_UPDATE_COMMAND_REC        = USB_EVENT | 0x0C,
    USB_EVENT_UDISK_SIGNAL_IN           = USB_EVENT | 0x0D,     //usb disk node signal change
    USB_EVENT_UDISK_ARRIVAL             = USB_EVENT | 0x0E,     //usb disk connect
    USB_EVENT_UDISK_REMOVAL             = USB_EVENT | 0x0F,     //usb disl remove
    USB_EVENT_SOFWWARE_USB_DETECT_PC_IN = USB_EVENT | 0x10,
    USB_EVENT_TEST_HOST_IS_PS4          = USB_EVENT | 0x11,     //测试是否是PS4
    USB_EVENT_BUS_RESET                 = USB_EVENT | 0x12,
};

enum system_event_e {
    SYSTEM_EVENT_NONE                   = 0,
    SYSTEM_EVENT_MODE_CHANGE            = SYSTEM_EVENT | 0x1,
    SYSTEM_EVENT_MODE_RESET             = SYSTEM_EVENT | 0x2,
    SYSTEM_EVENT_POWEROFF               = SYSTEM_EVENT | 0x3,
    SYSTEM_EVENT_UPDATE_STANBY          = SYSTEM_EVENT | 0x4,
    SYSTEM_EVENT_SECOND                 = SYSTEM_EVENT | 0x5,
    SYSTEM_EVENT_SEAMLESS               = SYSTEM_EVENT | 0x6,
    SYSTEM_EVENT_FORCE_CHARGE_MODE      = SYSTEM_EVENT | 0x7,
    SYSTEM_EVENT_EXIT_SLEEP_MODE        = SYSTEM_EVENT | 0x8,
    SYSTEM_EVENT_CON_DIS_TIMEOUT        = SYSTEM_EVENT | 0x9,
    SYSTEM_EVENT_SEAMLESS_WAIT_TIMER_STOP = SYSTEM_EVENT | 0xa,
    SYSTEM_EVENT_SYNC_BAT_PRECENT       = SYSTEM_EVENT | 0x10,
    SYSTEM_EVENT_VERSION_ERROR_TIMER_STOP = SYSTEM_EVENT | 0x11,
    SYSTEM_EVENT_WIRELESS_SCAN_EN       = SYSTEM_EVENT | 0x12,
    SYSTEM_EVENT_POWEROFF_DELAY         = SYSTEM_EVENT | 0x13,
};

//充电自动化测试事件
//事件格式：事件类型前缀|左右耳(bit4)|vbus code(bit3~bit0)
//0:左耳， 1:右耳
enum test_event_e {
    TEST_EVENT_NONE                   = 0,
    TEST_EVENT_POWERON_LEFT           = TEST_EVENT | (0x0 << 4) | 0x0,
    TEST_EVENT_POWEROFF_LEFT          = TEST_EVENT | (0x0 << 4) | 0x1,
    TEST_EVENT_SHUTDOWN_LEFT          = TEST_EVENT | (0x0 << 4) | 0x2,
    TEST_EVENT_DUT_LEFT               = TEST_EVENT | (0x0 << 4) | 0x4,
    TEST_EVENT_DISCOV_LEFT            = TEST_EVENT | (0x0 << 4) | 0x5,
    TEST_EVENT_POWERON_RIGHT          = TEST_EVENT | (0x1 << 4) | 0x0,
    TEST_EVENT_POWEROFF_RIGHT         = TEST_EVENT | (0x1 << 4) | 0x1,
    TEST_EVENT_SHUTDOWN_RIGHT         = TEST_EVENT | (0x1 << 4) | 0x2,
    TEST_EVENT_DUT_RIGHT              = TEST_EVENT | (0x1 << 4) | 0x4,
    TEST_EVENT_DISCOV_RIGHT           = TEST_EVENT | (0x1 << 4) | 0x5,
};

enum pmu_event_e {
    PMU_EVENT_NONE                      = 0,
    PMU_EVENT_BAT_LOW                   = PMU_EVENT | 0x01,
    PMU_EVENT_BAT_FULL                  = PMU_EVENT | 0x02,
    PMU_EVENT_BAT_TO                    = PMU_EVENT | 0x03,
    PMU_EVENT_POWERKEY_US               = PMU_EVENT | 0x04,
    PMU_EVENT_POWERKEY_SU               = PMU_EVENT | 0x05,
    PMU_EVENT_POWERKEY_L                = PMU_EVENT | 0x06,
    PMU_EVENT_INT                       = PMU_EVENT | 0x07,
    PMU_EVENT_CHARGE_IN                 = PMU_EVENT | 0x08,
    PMU_EVENT_CHARGE_OUT                = PMU_EVENT | 0x09,
    PMU_EVENT_HSW_OFF                   = PMU_EVENT | 0x0a,
    PMU_EVENT_HSW_ON                    = PMU_EVENT | 0x0b,
    PMU_EVENT_VBUSUV                    = PMU_EVENT | 0x0c,
    PMU_EVENT_BAT_UNLOW                 = PMU_EVENT | 0x0d,
    PMU_EVENT_BAT_LOW_WRN               = PMU_EVENT | 0x0e,
    PMU_EVENT_BAT_LOW_SHUT              = PMU_EVENT | 0x0f,
    PMU_EVENT_VBUS_IN                   = PMU_EVENT | 0x10,
    PMU_EVENT_VBUSMIDIN                 = PMU_EVENT | 0x11,
    PMU_EVENT_VBUSMIDOUT                = PMU_EVENT | 0x12,
    PMU_EVENT_CHARGING_BIN_PAIR_TOKEN   = PMU_EVENT | 0x13,
    PMU_EVENT_EARPHONE_PAIR_TOKEN       = PMU_EVENT | 0x14,
    PMU_EVENT_CAL_FREQ                  = PMU_EVENT | 0x15,
    PMU_EVENT_IN_CHARGING_BIN           = PMU_EVENT | 0x16,
    PMU_EVENT_OUT_CHARGING_BIN          = PMU_EVENT | 0x17,
    PMU_EVENT_CHARGING_BIN_CHARGING     = PMU_EVENT | 0x18,
    PMU_EVENT_CHARGING_BIN_UNCHARGING   = PMU_EVENT | 0x19,
    PMU_EVENT_CHARGING_BIN_BAT_UPDATE   = PMU_EVENT | 0x20,
    PMU_EVENT_RECOV                     = PMU_EVENT | 0x21,
    PMU_EVENT_SET_EQ                    = PMU_EVENT | 0x22,
    PMU_EVENT_OPEN_CHARGE               = PMU_EVENT | 0x23,
};

enum g24_event_e {
    G24_EVENT_NONE                      = 0,
    G24_EVENT_SLAVE_SAVE_BDADDR         = G24_EVENT | 0x01,
    G24_EVENT_MASTER_SAVE_LNK           = G24_EVENT | 0x02,
    G24_EVENT_DISCONNECT                = G24_EVENT | 0x03,
    G24_EVENT_CONNECT                   = G24_EVENT | 0x04,
    G24_EVENT_POWEROFF_REQ              = G24_EVENT | 0x05,
    G24_EVENT_RESET_AFH_RF              = G24_EVENT | 0x06,
    G24_EVENT_ENG0_UPDATE               = G24_EVENT | 0x07,
    G24_EVENT_ENG1_UPDATE               = G24_EVENT | 0x08,
    G24_EVENT_LINK0_STA_UPDATE          = G24_EVENT | 0x09,
    G24_EVENT_LINK1_STA_UPDATE          = G24_EVENT | 0x0A,
    G24_EVENT_MONITOR_CONNECT           = G24_EVENT | 0x0B,
    G24_EVENT_MASTER_MIC_CONNECT        = G24_EVENT | 0x0C,
    G24_EVENT_MASTER_MIC_DISCONNECT     = G24_EVENT | 0x0D,
    G24_EVENT_LPC_ENTER                 = G24_EVENT | 0x0E,
    G24_EVENT_HEARTBREAK_RECEIVE        = G24_EVENT | 0x0F,
    G24_EVENT_MONITOR_USER_DISCON       = G24_EVENT | 0x10,
    G24_EVENT_MONITOR_TX_ENABLE         = G24_EVENT | 0x11,
    G24_EVENT_MONITOR_START             = G24_EVENT | 0x12,
    G24_EVENT_MONITOR_FORCE_DISCONNECT  = G24_EVENT | 0x13,
    G24_EVENT_MONITOR_MIC_ID_NOT_MATCH  = G24_EVENT | 0x14,
    
};

enum mp_test_event_e {
    MP_TEST_EVENT_NONE = 0,
    MP_TEST_EVENT_ANC = MP_TEST_EVENT | 0x01,
    MP_TEST_AT_EVENT_ANC = MP_TEST_EVENT | 0x02,
    MP_TEST_AT_EVENT_ANC_R2L = MP_TEST_EVENT | 0x03,
};

enum juplus_event_e {
    JUPLUS_EVENT_NONE                       = 0,
    JUPLUS_EVENT_CONNECTED                  = JUPLUS_EVENT | 0x01,
    JUPLUS_EVENT_SPATIAL_AUDIO_MODE_FIX     = JUPLUS_EVENT | 0x02,
    JUPLUS_EVENT_GAME_MODE_FIX              = JUPLUS_EVENT | 0x03,
    JUPLUS_EVENT_SPATIAL_AUDIO_MODE_UPDATE  = JUPLUS_EVENT | 0x04,
    JUPLUS_EVENT_GAME_MODE_UPDATE           = JUPLUS_EVENT | 0x05,
};

void event_init(void);
void event_put(uint32_t event, const void *params, uint32_t params_len);
void event_unique_put(uint32_t event, const void *params, uint32_t params_len);
void event_force_put(uint32_t event, const void *params, uint32_t params_len);
void event_force_unique_put(uint32_t event, const void *params, uint32_t params_len);
uint32_t event_get(void *params, uint32_t *p_params_len, bool wait);
uint8_t event_cnt_get(void);
uint16_t event_waiting_get(uint32_t event);
uint16_t event_group_waiting_get(uint32_t event_group);
uint16_t event_clear(uint32_t event);
uint16_t event_group_clear(uint32_t event_group);

#endif
