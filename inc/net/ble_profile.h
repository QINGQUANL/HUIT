#ifndef _BLE_PROFILE_H_
#define _BLE_PROFILE_H_

#include "event.h"

enum le_event_e {
    LE_EVENT_NONE                       = 0,
    LE_EVENT_CONNECTED                  = LE_EVENT | 0x1,
    LE_EVENT_DISCONNECTED               = LE_EVENT | 0x2,
    LE_EVENT_APP                        = LE_EVENT | 0x3,
};

#define UINT16_TO_LITTLE_ENDIAN_BYTES(x) ((x)&0xFF), (((x)>>8)&0xFF)
#define ATT_SIZE   UINT16_TO_LITTLE_ENDIAN_BYTES
#define ATT_FLAG   UINT16_TO_LITTLE_ENDIAN_BYTES
#define ATT_HANDLE UINT16_TO_LITTLE_ENDIAN_BYTES
#define ATT_UUID   UINT16_TO_LITTLE_ENDIAN_BYTES

#define UINT32_TO_LITTLE_ENDIAN_BYTES(x) ((x)&0xFF), (((x)>>8)&0xFF), (((x)>>16)&0xFF), (((x)>>24)&0xFF)
#define ATT_UUID_32   UINT32_TO_LITTLE_ENDIAN_BYTES


// GAP Service and Characteristics
#define GAP_SERVICE_UUID               0x1800
#define GATT_SERVICE_UUID              0x1801
#define GAP_DEVICE_NAME_UUID           0x2a00
#define GAP_APPEARANCE_UUID            0x2a01
#define GAP_PERIPHERAL_PRIVACY_FLAG    0x2a02
#define GAP_RECONNECTION_ADDRESS_UUID  0x2a03
#define GAP_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS_UUID 0x2a04
#define GAP_SERVICE_CHANGED            0x2a05

// MARK: GATT UUIDs
#define GATT_PRIMARY_SERVICE_UUID                   0x2800
#define GATT_SECONDARY_SERVICE_UUID                 0x2801
#define GATT_INCLUDE_SERVICE_UUID                   0x2802
#define GATT_CHARACTERISTICS_UUID                   0x2803
#define GATT_CHARACTERISTIC_EXTENDED_PROPERTIES     0x2900
#define GATT_CHARACTERISTIC_USER_DESCRIPTION        0x2901
#define GATT_CLIENT_CHARACTERISTICS_CONFIGURATION   0x2902
#define GATT_SERVER_CHARACTERISTICS_CONFIGURATION   0x2903
#define GATT_CHARACTERISTIC_PRESENTATION_FORMAT     0x2904
#define GATT_CHARACTERISTIC_AGGREGATE_FORMAT        0x2905

// MARK: Attribute Property Flags
#define ATT_PROPERTY_BROADCAST           0x01
#define ATT_PROPERTY_READ                0x02
#define ATT_PROPERTY_WRITE_WITHOUT_RESPONSE 0x04
#define ATT_PROPERTY_WRITE               0x08
#define ATT_PROPERTY_NOTIFY              0x10
#define ATT_PROPERTY_INDICATE            0x20
#define ATT_PROPERTY_AUTHENTICATED_SIGNED_WRITE 0x40
#define ATT_PROPERTY_EXTENDED_PROPERTIES 0x80

// MARK: Attribute Property Flag, BTstack extension
// value is asked from client
#define ATT_PROPERTY_DYNAMIC             0x100
// 128 bit UUID used
#define ATT_PROPERTY_UUID128             0x200
// Authentication required
#define ATT_PROPERTY_AUTHENTICATION_REQUIRED 0x400
// Authorization from user required
#define ATT_PROPERTY_AUTHORIZATION_REQUIRED  0x800

#define AD_TYPE_FLAGS                       0x01
#define AD_TYPE_INCOMPLETE_UUID_16          0x02
#define AD_TYPE_COMPLETE_UUID_16            0x03
#define AD_TYPE_INCOMPLETE_UUID_32          0x04
#define AD_TYPE_COMPLETE_UUID_32            0x05
#define AD_TYPE_INCOMPLETE_UUID_128         0x06
#define AD_TYPE_COMPLETE_UUID_128           0x07
#define AD_TYPE_SHORT_LOCAL_NAME            0x08
#define AD_TYPE_COMPLETE_LOCAL_NAME         0x09
#define AD_TYPE_MANUFACTURER_SPECIFIC_DATA  0xFF

#define FLAG_LE_LIMITED_DISCOVER_MODE   (1<<0)
#define FLAG_LE_GENERAL_DISCOVER_MODE   (1<<1)
#define FLAG_BR_EDR_NOT_SUPPORTED       (1<<2)
#define FLAG_LE_BR_EDR_SAME_CONTROLLER  (1<<3)
#define FLAG_LE_BR_EDR_SAME_HOST        (1<<4)

enum ble_address_e {
    PUBLIC_ADDRESS,
    RANDOM_ADDRESS,
    PRIVATE_ADDRESS_BUBLIC,
    PRIVATE_ADDRESS_RANDOM
};

enum ble_advertising_type_e {
    ADV_IND,
    ADV_DIRECT_IND_HIGH ,
    ADV_SCAN_IND,
    ADV_NONCONN_IND,
    ADV_DIRECT_IND_LOW
};


typedef uint16_t (*profile_read_callback_t)(uint16_t attr_handle, uint8_t * buffer, uint16_t buffer_size);
typedef int (*profile_write_callback_t)(uint16_t attr_handle, uint8_t *buffer, uint16_t buffer_size);

typedef struct {
    uint16_t adv_int_min;
    uint16_t adv_int_max;
    uint8_t adv_type;
} ble_adv_param_t;

typedef struct ble_profile {
    uint8_t own_addr[6];
    uint8_t own_addr_type;
    uint8_t *adv_data;
    uint8_t adv_data_len;
    uint8_t *scan_rsp;
    uint8_t scan_rsp_len;
    uint8_t *att_db;
    uint16_t start_handle;
    uint16_t end_handle;
    ble_adv_param_t adv_param;
    profile_read_callback_t read_callback;
    profile_write_callback_t write_callback;
} ble_profile_t;

void ble_profile_init(ble_profile_t *profile);
void ble_profile_deinit(void);
void ble_profile_addr_reset(ble_profile_t *profile);
int ble_profile_data_send_notify(uint16_t attr_handle, uint8_t *value, uint16_t value_len);
int ble_profile_data_send_indicate(uint16_t attr_handle, uint8_t *value, uint16_t value_len);
void ble_adv_param_init(ble_adv_param_t *param);
void ble_profile_adv_param_get(ble_adv_param_t *adv_param);

void ble_addr_set(uint8_t *addr);
void ble_addr_get(uint8_t *addr);

#endif

