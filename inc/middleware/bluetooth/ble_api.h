#ifndef _BLE_APP_
#define _BLE_APP_

#include "ble_profile.h"

#define TX_RETRY_EN     0

enum att_uuid_e {
    UUID_SERVICE_01 = 0xAA00,
    UUID_RECV_01    = 0xAA01,
    UUID_SEND_01    = 0xAA02,

    UUID_SERVICE_02 = 0xFFF0,
    UUID_RECV_02    = 0xFFF1,
    UUID_SEND_02    = 0xFFF2,
};

enum att_handle_e {
    HANDLE_RECV_01  = 0x09,
    HANDLE_SEND_01  = 0x0B,

    HANDLE_RECV_02  = 0x0E,
    HANDLE_SEND_02  = 0x10,

    HANDLE_RECV_03  = 0x17,
    HANDLE_SEND_03  = 0x19,
};

enum packet_tag_e {
    TAG_START   = 0xA0,             //获取设备状态(START)
    TAG_RESET   = 0xA1,             //设备复位(RESET)
};

enum packet_offset_e {
    OFFSET_TAG              = 0,    //1B
    OFFSET_START_VERSION    = 1,    //2B
    OFFSET_START_BAT        = 3,    //1B
};

enum packet_tx_len_e {
    TX_LEN_START            = 1,
    TX_LEN_RESET            = 1,
};

enum packet_rx_len_e {
    RX_LEN_START            = 1,
    RX_LEN_RESET            = 1,
};

enum system_status_e {
    STATUS_INIT             = 0,
    STATUS_START,
    STATUS_RESET,
};

enum adv_manufacturer_specific_data_e {
    ADV_FF_OFFSET_ADDR      = 0,    //0~5:经典蓝牙地址
    ADV_FF_OFFSET_LR       = 6,    //6:左右耳信息
    ADV_FF_OFFSET_VER       = 7,    //7~8:固件版本

    ADV_FF_LEN_ADDR         = 6,
    ADV_FF_LEN_BAT          = 1,
    ADV_FF_LEN_VER          = 2,
    ADV_FF_LEN              = ADV_FF_LEN_ADDR + ADV_FF_LEN_BAT + ADV_FF_LEN_VER,
};

/*
    0~3:重启后跳转标志
    4~9:蓝牙地址
    10~15:ble地址
    16:左/右耳标志
*/
enum tag_e {
    OFFSET_BRANCH   = 0,
    OFFSET_BT_ADDR  = 4,
    OFFSET_BLE_ADDR = 10,
    OFFSET_LR_FLAG  = 16,

    SIZE_BRANCH     = 4,
    SIZE_BT_ADDR    = 6,
    SIZE_BLE_ADDR   = 6,
    SIZE_LR_FLAG    = 1,
    SIZE_TAG = SIZE_BRANCH + SIZE_BT_ADDR + SIZE_BLE_ADDR + SIZE_LR_FLAG,
};

void ble_app_process(void);
void ble_app_receive_process(uint8_t *buf);
void ble_addr_set(uint8_t *addr);
void ble_name_set(uint8_t *ble_name);
void ble_adv_update(void);
void ble_air_adv_init(void);
void ble_juplus_send(uint8_t *buf, uint16_t size);
void ble_ota_erase_flash(void);
void ble_tx_timer_create(void);
void ble_tx_timer_start(void);
void ble_tx_timer_stop(void);
void ble_tx_timer(void *xTimer);

void ble_air_name_set(uint8_t* name, uint8_t len);
void ble_air_scan_rsp_set(void);

void tws_start_ble_ota(void);
void info_mem_clear(void);

uint8_t bat_quantity_get(void);
uint8_t gap_advertisements_enable_get(void);
void gap_advertisements_enable(int enabled);
void gap_advertisements_set_data(uint8_t advertising_data_length, uint8_t * advertising_data);
void gap_scan_response_set_data(uint8_t scan_response_data_length, uint8_t * scan_response_data);


void ble_ota_addr_set(uint8_t *addr);
void ble_ota_addr_type_set(uint8_t type);
void ble_ota_name_set(uint8_t *name, uint8_t len);
void ble_ota_adv_param_set(ble_adv_param_t *param);
void ble_linkplus_init();
void ble_linkplus_exit();
void ble_adv_update_enable_addr(bool enable);
void ble_juplus_con_set(uint8_t con);
uint8_t ble_juplus_con_get(void);

void ble_air_random_addr_set(uint8_t *addr);
void ble_air_random_addr_get(uint8_t *addr);
void ble_air_addr_reset(void);
void ble_air_addr_type_set(uint8_t type);
void ble_air_adv_param_set(ble_adv_param_t *param);
void ble_air_init(void);
void ble_air_reinit(void);

void ble_test_addr_set(uint8_t *addr);
void ble_test_addr_type_set(uint8_t type);
void ble_test_name_set(uint8_t *name, uint8_t len);
void ble_test_adv_param_set(ble_adv_param_t *param);
void ble_test_init();
void ble_test_exit();
void ble_test_adv_update(void);

bool ble_con_status_get(void);
void ble_con_status_set(bool status);
bool ble_ota_status_get(void);
void ble_ota_status_set(bool status);
uint8_t juplus_data_receive(uint8_t *buf, uint8_t len);

bool ble_enabled_get();

#endif
