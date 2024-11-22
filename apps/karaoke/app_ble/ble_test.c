#define LOG_LEV 5
#include "sys_inc.h"
#include "ble_profile.h"
#include "ble_api.h"
#include "bt_host_api.h"
#include "user_config.h"

#if BLE_TEST_EN
ble_profile_t ble_test_app;
static uint8_t adv_data[31];
static uint8_t scan_rsp[31];
static uint8_t adv_offset_bat;     //电量参数的位置偏移
static uint8_t ble_test_name_len, ble_test_name[31];
static uint16_t sys_version;

/*
profile data is like this: if the attr is not empty
|Byte0~1    |Byte2~3    |Byte4~5        |Byte6 ~ Byte7  |   Byte8~ Byte(size -1)    |
|   size    |    flg    |    handle     |  UUID         |             data          |
    or
|Byte0~1    |Byte2~3    |Byte4~19       |Byte6~ Byte21  |   Byte22 ~ Byte(size -1)  |
|   size    |    flg    |    handle     |  UUID         |        data               |
*/

static const uint8_t simple_att_db[] =
{
    // 0x0001 PRIMARY_SERVICE-GAP_SERVICE
    ATT_SIZE(0x000a), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0001), ATT_UUID(GATT_PRIMARY_SERVICE_UUID), ATT_UUID(GAP_SERVICE_UUID),
    // 0x0002 CHARACTERISTIC-GAP_DEVICE_NAME-READ
    ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0002), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_READ, ATT_HANDLE(0x0003), ATT_UUID(GAP_DEVICE_NAME_UUID),
    // 0x0003 VALUE-GAP_DEVICE_NAME-READ-'SmartLink BLE'
    ATT_SIZE(0x0015), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0003), ATT_UUID(GAP_DEVICE_NAME_UUID), 'S','m','a','r','t','L','i','n','k',' ','B','L','E',

    // 0x0004 PRIMARY_SERVICE-GATT_SERVICE
    ATT_SIZE(0x000a), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0004), ATT_UUID(GATT_PRIMARY_SERVICE_UUID), ATT_UUID(GATT_SERVICE_UUID),
    // 0x0005 CHARACTERISTIC-GATT_SERVICE_CHANGED-READ
    ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0005), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_READ, ATT_HANDLE(0x0006), ATT_UUID(GAP_SERVICE_CHANGED),
    // 0x0006 VALUE-GATT_SERVICE_CHANGED-READ-''
    ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0006), ATT_UUID(GAP_SERVICE_CHANGED),
#if 0
    // 0x0007 PRIMARY_SERVICE
    ATT_SIZE(0x000a), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0007), ATT_UUID(GATT_PRIMARY_SERVICE_UUID), ATT_UUID(UUID_SERVICE_01),
    // 0x0008 CHARACTERISTIC | WRITE_WITHOUT_RESPONSE
    ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0008), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_WRITE_WITHOUT_RESPONSE, ATT_HANDLE(0x0009), ATT_UUID(UUID_RECV_01),
    // 0x0009 VALUE | WRITE_WITHOUT_RESPONSE
    ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(0x0009), ATT_UUID(UUID_RECV_01),

    // 0x000a CHARACTERISTIC | NOTIFY
    ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x000a), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_NOTIFY, ATT_HANDLE(0x000b), ATT_UUID(UUID_SEND_01),
    // 0x000b VALUE | NOTIFY
    ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(HANDLE_SEND_01), ATT_UUID(UUID_SEND_01),
#endif
    // 0x000c PRIMARY_SERVICE
    ATT_SIZE(0x000a), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x000c), ATT_UUID(GATT_PRIMARY_SERVICE_UUID), ATT_UUID(UUID_SERVICE_02),
    // 0x000d CHARACTERISTIC | WRITE_WITHOUT_RESPONSE
    ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x000d), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_WRITE_WITHOUT_RESPONSE, ATT_HANDLE(0x000e), ATT_UUID(UUID_RECV_02),
    // 0x000e VALUE | WRITE_WITHOUT_RESPONSE
    ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(0x000e), ATT_UUID(UUID_RECV_02),

    // 0x000f CHARACTERISTIC | READ | NOTIFY
    ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x000f), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY, ATT_HANDLE(0x0010), ATT_UUID(UUID_SEND_02),
    // 0x0010 VALUE | READ | NOTIFY | DYNAMIC
    ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(HANDLE_SEND_02), ATT_UUID(UUID_SEND_02),
    // 0x0011 CLINT_CONFIGURATION READ | DYNAMIC
    ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_READ | ATT_PROPERTY_WRITE | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(0x0011), ATT_UUID(GATT_CLIENT_CHARACTERISTICS_CONFIGURATION),
    // END
    0x00, 0x00,
};

static int ble_app_data_send(uint16_t attr_handle, uint8_t *value, uint16_t value_len)
{
    return ble_profile_data_send_notify(attr_handle, value, value_len);
}

static uint16_t ble_app_data_read(uint16_t attr_handle, uint8_t *buffer, uint16_t buff_size)
{
    return 0;
}

static int ble_app_data_receive(uint16_t attr_handle, uint8_t *buffer, uint16_t buff_size)
{
    //logi("r,handle:%x, size:%d", attr_handle,buff_size);
    //printf_array(buffer, buff_size);

    if (HANDLE_RECV_02 == attr_handle) {
        ble_app_receive_process(buffer);
    }

    return 0;
}


void ble_test_send(uint8_t *buf, uint16_t size)
{
    logx("tx:");
    printf_array(buf, size);
    ble_app_data_send(HANDLE_SEND_02, buf, size);
}

void ble_test_name_set(uint8_t *name, uint8_t len)
{
    memcpy(ble_test_name, name, len);
    ble_test_name_len = len;
}

void ble_test_adv_data_init(void)
{
    uint8_t adv_offset;
    uint8_t name_len;

    libc_memset(adv_data, 0x0, 31);

    //01-AD_TYPE_FLAGS
    adv_data[0] = 0x02;
    adv_data[1] = AD_TYPE_FLAGS;
    adv_data[2] = FLAG_LE_GENERAL_DISCOVER_MODE | FLAG_BR_EDR_NOT_SUPPORTED;
    adv_offset = 3;

    //09-AD_TYPE_COMPLETE_LOCAL_NAME
    name_len = ble_test_name_len;
    adv_data[adv_offset] = name_len + 1;
    adv_data[adv_offset+1] = AD_TYPE_COMPLETE_LOCAL_NAME;
    memcpy(&adv_data[adv_offset+2], ble_test_name, ble_test_name_len);
    adv_offset += name_len + 2;

    //02-AD_TYPE_INCOMPLETE_UUID_16
    adv_data[adv_offset] = 3;
    adv_data[adv_offset+1] = AD_TYPE_INCOMPLETE_UUID_16;
    adv_data[adv_offset+2] = (uint8_t)UUID_SERVICE_02;
    adv_data[adv_offset+3] = (uint8_t)(UUID_SERVICE_02>>8);
    adv_offset += 4;

    //FF-AD_TYPE_MANUFACTURER_SPECIFIC_DATA
    adv_data[adv_offset] = ADV_FF_LEN + 1;                                     //length
    adv_data[adv_offset+1] = AD_TYPE_MANUFACTURER_SPECIFIC_DATA;               //ad type

    //经典蓝牙地址
    //logx("bt addr:");
    bt_addr_get(adv_data+adv_offset+2+ADV_FF_OFFSET_ADDR);

    //设备电量
    adv_offset_bat = adv_offset+2+2;
    adv_data[adv_offset_bat] = bat_quantity_get();
    //logi("bat offset: %d", adv_offset_bat);

    //固件版本
    sys_version = 0x1000;
    adv_data[adv_offset+2+ADV_FF_OFFSET_VER] = (uint8_t)sys_version;
    adv_data[adv_offset+2+ADV_FF_OFFSET_VER+1] = (uint8_t)(sys_version>>8);

    logx("adv:");
    printf_array(adv_data, 31);

    //SCAN RSP
    scan_rsp[0] = 1 + ble_test_name_len;
    scan_rsp[1] = AD_TYPE_COMPLETE_LOCAL_NAME;
    memcpy(&scan_rsp[2], ble_test_name, ble_test_name_len);
}

void ble_test_addr_set(uint8_t *addr)
{
    libc_memcpy(ble_test_app.own_addr, addr, 6);
    ble_addr_set(ble_test_app.own_addr);
}

void ble_test_addr_type_set(uint8_t type)
{
    ble_test_app.own_addr_type = type;
}

void ble_test_adv_update(void)
{
    if (gap_advertisements_enable_get()) {
        adv_data[adv_offset_bat] = bat_quantity_get();    //设备电量
        gap_advertisements_set_data(sizeof(adv_data), adv_data);
    }
}

void ble_test_adv_param_set(ble_adv_param_t *param)
{
    ble_test_app.adv_param.adv_int_min = param->adv_int_min;
    ble_test_app.adv_param.adv_int_max = param->adv_int_max;
    ble_test_app.adv_param.adv_type = param->adv_type;
}

void ble_test_init()
{
    ble_test_adv_data_init();

    ble_test_app.adv_data = adv_data;
    ble_test_app.adv_data_len = sizeof(adv_data);
    ble_test_app.scan_rsp = scan_rsp;
    ble_test_app.scan_rsp_len = 0;
    ble_test_app.att_db = (uint8_t *)simple_att_db;
    ble_test_app.start_handle = 0x0000;
    ble_test_app.end_handle = 0xFFFF;
    ble_test_app.read_callback = ble_app_data_read;
    ble_test_app.write_callback = ble_app_data_receive;

    ble_profile_init(&ble_test_app);
}

void ble_test_exit(void)
{
    ble_profile_deinit();
}

#endif
