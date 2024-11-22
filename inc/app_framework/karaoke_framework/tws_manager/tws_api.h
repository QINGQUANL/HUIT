#ifndef _TWS_API_H_
#define _TWS_API_H_

enum tws_cmd_e {
    TWS_CMD_CHECK           = 0,
    TWS_CMD_PAIR_UPDATE     = 1,
    TWS_CMD_PAIR_DELETE     = 2,
    TWS_CMD_PAIR_MAC_GET    = 3,
    TWS_CMD_PAIR_ONLY       = 4,
    TWS_CMD_READ_FLASH      = 5,
    TWS_CMD_WRITE_FLASH     = 6,
    TWS_CMD_RESTART         = 7,
};


enum app_cmd_e {
    TWS_APP_CMD_NONE                = 0x00,
    TWS_APP_CMD_PHONE_CONNECTED     = 0x01,
    TWS_APP_CMD_PHONE_DISCONNECTED  = 0x02,
    TWS_APP_CMD_EQ_INIT             = 0x03,
    TWS_APP_CMD_EQ_EXIT             = 0x04,
    TWS_APP_CMD_EQ_CHANGE           = 0x05,
    TWS_APP_CMD_SCO_TYPE            = 0x06,
    TWS_APP_CMD_ENTER_SCAN_MODE     = 0x07,
    TWS_APP_CMD_CALLING_STATE_SYNC  = 0x08,
    TWS_APP_CMD_EXCHANGE_INFO       = 0x09,
    TWS_APP_CMD_VBUS_IN             = 0x0A,
    TWS_APP_CMD_NAME_SET            = 0x0B,
    TWS_APP_CMD_NAME_WITH_WRITE_SET = 0x0C,

    TWS_APP_CMD_BAT                 = 0x10,
    TWS_APP_CMD_BT_AIR_SYNC         = 0x11,
    TWS_APP_CMD_BLE_AIR_SYNC        = 0x12,
    TWS_APP_CMD_SET_AUDIOROT        = 0x13,
    TWS_APP_CMD_SET_SCOTSPS_TYPE    = 0x14,
    TWS_APP_CMD_ANC_SYNC            = 0x15,
    TWS_APP_CMD_SENSOR_SYNC         = 0x16,
    TWS_APP_CMD_SPACE_AUDIO_SYNC    = 0x17,
    TWS_APP_CMD_EN_SYNC             = 0x18,
    TWS_APP_CMD_SN_SYNC             = 0x19,
    TWS_APP_CMD_VERSION             = 0x1A,
    TWS_APP_CMD_KWS_SYNC            = 0x1B,
    TWS_APP_CMD_ADDR_SYNC           = 0x1C,

    TWS_APP_CMD_EVENT               = 0x20,     //向对方发送一个事件
    TWS_APP_CMD_SENSOR_SYNC_IN_APP  = 0x21,
    TWS_APP_CMD_ANC_SYNC_IN_APP     = 0x22,
    TWS_APP_CMD_SPACE_AUDIO_SYNC_IN_APP    = 0x23,
    TWS_APP_CMD_SYNC_BAT_PRECENT    = 0x24,     //双方同步电量
    TWS_APP_CMD_KWS_IN_SLAVE_STATE  = 0x25,     //控制slave 开关KWS
    TWS_APP_CMD_KWS_WAKEUP_IN_MASTER  = 0x26,   //slave 控制master 开启siri
    TWS_APP_CMD_KWS_IN_SLAVE_STATE_UPDATE =0x27,//kws 状态更新
    TWS_APP_CMD_DISP_SYNC             =0x28,//灯显同步控制

    TWS_APP_CMD_ANC_CHANGE            = 0x42,  //SPP ANC收到数据
};

typedef struct
{
    uint8_t ctl_packet_type; // should be TWS_CMD_APP_COMMON
    uint8_t app_ctrl;
    uint8_t params_len;
    uint8_t params[200];
} __attribute__ ((__packed__)) app_common_ctrl_t;


void tws_manager_init();
void tws_manager_exit();
void tws_seamless();
void tws_manager_mp_task_create(void);
void tws_manager_mp_task_delete(void);
void tws_manager_register_app_cmd_handler(void (*handler)(uint8_t *pctrl));


#endif

