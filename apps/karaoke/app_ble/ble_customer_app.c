#define LOG_LEV 5
#include "sys_inc.h"
#include "ble_profile.h"
#include "ble_api.h"
#include "bt_host_api.h"
#include "user_config.h"
#include "psm_manager.h"
#include "info_mem.h"
#include "tws_profile.h"
#include "mode_bt.h"
#include "hal.h"

#if BLE_CUSTOMER_APP_EN
ble_profile_t ble_customer_app;
static uint8_t adv_data[31];
static uint8_t scan_rsp[31];
static uint8_t adv_offset_bat;     //电量参数的位置偏移
static uint8_t ble_customer_name_len, ble_customer_name[31];

/*
profile data is like this: if the attr is not empty
|Byte0~1    |Byte2~3    |Byte4~5        |Byte6 ~ Byte7  |   Byte8~ Byte(size -1)    |
|   size    |    flg    |    handle     |  UUID         |             data          |
    or
|Byte0~1    |Byte2~3    |Byte4~19       |Byte6~ Byte21  |   Byte22 ~ Byte(size -1)  |
|   size    |    flg    |    handle     |  UUID         |        data               |
*/

//#define  UUID_RECV_02_CUSTOMER      (0x00),(0x00),(0x11),(0x01),(0xD1),(0x02),(0x11),(0xE1),(0x9B),(0x23),(0x00),(0x02),(0x5B),(0x00),(0xA5),(0xA5)

#define  UUID_RECV_02_CUSTOMER_1      0x5B00A5A5
#define  UUID_RECV_02_CUSTOMER_2      0x9B230002
#define  UUID_RECV_02_CUSTOMER_3      0xD10211E1
#define  UUID_RECV_02_CUSTOMER_4      0x00001101

#define  UUID_RECV_02_CUSTOMER    ATT_UUID_32(UUID_RECV_02_CUSTOMER_1),ATT_UUID_32(UUID_RECV_02_CUSTOMER_2),ATT_UUID_32(UUID_RECV_02_CUSTOMER_3),ATT_UUID_32(UUID_RECV_02_CUSTOMER_4)

#define  UUID_RECV_03_CUSTOMER_4      0x00001103

#define  UUID_RECV_03_CUSTOMER    ATT_UUID_32(UUID_RECV_02_CUSTOMER_1),ATT_UUID_32(UUID_RECV_02_CUSTOMER_2),ATT_UUID_32(UUID_RECV_02_CUSTOMER_3),ATT_UUID_32(UUID_RECV_03_CUSTOMER_4)

#define  UUID_SEND_02_CUSTOMER_1      0x5B00A5A5
#define  UUID_SEND_02_CUSTOMER_2      0x9B230002
#define  UUID_SEND_02_CUSTOMER_3      0xD10211E1
#define  UUID_SEND_02_CUSTOMER_4      0x00001102


#define  UUID_SEND_02_CUSTOMER    ATT_UUID_32(UUID_SEND_02_CUSTOMER_1),ATT_UUID_32(UUID_SEND_02_CUSTOMER_2),ATT_UUID_32(UUID_SEND_02_CUSTOMER_3),ATT_UUID_32(UUID_SEND_02_CUSTOMER_4)

#define  UUID_SERVICE_CUSTOMER_4      0x00001100

#define  UUID_MAIN_SERVICE_CUSTOMER    ATT_UUID_32(UUID_SEND_02_CUSTOMER_1),ATT_UUID_32(UUID_SEND_02_CUSTOMER_2),ATT_UUID_32(UUID_SEND_02_CUSTOMER_3),ATT_UUID_32(UUID_SERVICE_CUSTOMER_4)


static const uint8_t simple_att_db[] =
{
    // 0x0001 PRIMARY_SERVICE-GAP_SERVICE
    ATT_SIZE(0x000a), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0001), ATT_UUID(GATT_PRIMARY_SERVICE_UUID), ATT_UUID(GAP_SERVICE_UUID),
    // 0x0002 CHARACTERISTIC-GAP_DEVICE_NAME-READ
    ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0002), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_READ, ATT_HANDLE(0x0003), ATT_UUID(GAP_DEVICE_NAME_UUID),
    // 0x0003 VALUE-GAP_DEVICE_NAME-READ-'SmartLink BLE'
    ATT_SIZE(0x000b), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0003), ATT_UUID(GAP_DEVICE_NAME_UUID), 'T','5','7',

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
#if 1
    // 0x000c PRIMARY_SERVICE
    ATT_SIZE(0x0018), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x000c), ATT_UUID(GATT_PRIMARY_SERVICE_UUID), UUID_MAIN_SERVICE_CUSTOMER,
    //0x000d CHARACTERISTIC | WRITE_WITHOUT_RESPONSE
    ATT_SIZE(0x001b), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x000d), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_WRITE_WITHOUT_RESPONSE, ATT_HANDLE(0x000e), UUID_RECV_02_CUSTOMER,
    // 0x000e VALUE | WRITE_WITHOUT_RESPONSE
    ATT_SIZE(0x0016), ATT_FLAG(ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(HANDLE_RECV_02), UUID_RECV_02_CUSTOMER,

    // 0x000f CHARACTERISTIC | READ | NOTIFY
    ATT_SIZE(0x001b), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x000f), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY, ATT_HANDLE(0x0010), UUID_SEND_02_CUSTOMER,
    // 0x0010 VALUE | READ | NOTIFY | DYNAMIC
    ATT_SIZE(0x0016), ATT_FLAG(ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(HANDLE_SEND_02), UUID_SEND_02_CUSTOMER,
    // 0x0011 CLINT_CONFIGURATION READ | DYNAMIC
    ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_READ | ATT_PROPERTY_WRITE | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(0x0011), ATT_UUID(GATT_CLIENT_CHARACTERISTICS_CONFIGURATION),

    ATT_SIZE(0x001b), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0012), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_READ|ATT_PROPERTY_WRITE_WITHOUT_RESPONSE, ATT_HANDLE(0x0013), UUID_RECV_03_CUSTOMER,
    // 0x000e VALUE | WRITE_WITHOUT_RESPONSE
    ATT_SIZE(0x0016), ATT_FLAG(ATT_PROPERTY_READ|ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(0x0014), UUID_RECV_03_CUSTOMER,
    // END
#endif
    // 0x000c PRIMARY_SERVICE
    #if 1
    ATT_SIZE(0x000a), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0015), ATT_UUID(GATT_PRIMARY_SERVICE_UUID), ATT_UUID(UUID_SERVICE_02),
    // 0x000d CHARACTERISTIC | WRITE_WITHOUT_RESPONSE
    ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0016), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_WRITE_WITHOUT_RESPONSE, ATT_HANDLE(HANDLE_RECV_03), ATT_UUID(UUID_RECV_02),
    // 0x000e VALUE | WRITE_WITHOUT_RESPONSE
    ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(HANDLE_RECV_03), ATT_UUID(UUID_RECV_02),

    // 0x000f CHARACTERISTIC | READ | NOTIFY
    ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0018), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY, ATT_HANDLE(HANDLE_SEND_03), ATT_UUID(UUID_SEND_02),
    // 0x0010 VALUE | READ | NOTIFY | DYNAMIC
    ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(HANDLE_SEND_03), ATT_UUID(UUID_SEND_02),
    // 0x0011 CLINT_CONFIGURATION READ | DYNAMIC
    ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_READ | ATT_PROPERTY_WRITE | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(0x001a), ATT_UUID(GATT_CLIENT_CHARACTERISTICS_CONFIGURATION),

    0x00, 0x00,
#endif

};

static int ble_app_data_send(uint16_t attr_handle, uint8_t *value, uint16_t value_len)
{
    return ble_profile_data_send_notify(attr_handle, value, value_len);
}

static uint16_t ble_app_data_read(uint16_t attr_handle, uint8_t *buffer, uint16_t buff_size)
{
    return 0;
}
 
extern uint8_t spp_app_data_handler(uint8_t* buf,uint8_t len);
static int ble_app_data_receive(uint16_t attr_handle, uint8_t *buffer, uint16_t buff_size)
{
 //   logi("r,handle:%x, size:%d", attr_handle,buff_size);
//    printf_array(buffer, buff_size);

    if (HANDLE_RECV_02 == attr_handle || HANDLE_RECV_03 == attr_handle) {
        //spp_app_data_handler(buffer,buff_size);
    }

    return 0;
}


void ble_customer_send(uint8_t *buf, uint16_t size)
{
    logx("tx:");
    printf_array(buf, size);
    ble_app_data_send(HANDLE_SEND_02, buf, size);
}

void ble_customer_ota_send(uint8_t *buf, uint16_t size)
{
    logx("ota tx:");
    printf_array(buf, size);
    ble_app_data_send(HANDLE_SEND_03, buf, size);
}

void ble_customer_name_set(uint8_t *name, uint8_t len)
{
    memcpy(ble_customer_name, name, len);
    ble_customer_name_len = len;
}

uint8_t bt_addr_index_s = 0;

void ble_customer_adv_update_enable_addr(bool enable)
{

    uint8_t addr[6] = {0};
    sys_info_read(BT_ADDR_INFO, addr, 6);
     printf_array(addr, 6);

    if (gap_advertisements_enable_get()) {
        if(enable) {
           // memcpy(adv_data+bt_addr_index_s,ble_ota_app.own_addr,6);
            for (int i = 0; i < 6; i++) {
               adv_data [bt_addr_index_s + i] = addr[5-i];
            }
             logx("adv:");
             printf_array(adv_data, 31);
        } else {
            memset(adv_data+bt_addr_index_s,0,6);
             logx("adv:");
             printf_array(adv_data, 31);
        }
        gap_advertisements_set_data(sizeof(adv_data), adv_data);
    }
}


void ble_customer_adv_data_init(void)
{
    uint8_t adv_offset;
    uint8_t name_len;

    libc_memset(adv_data, 0x0, 31);

    //01-AD_TYPE_FLAGS
    adv_data[0] = 0x02;
    adv_data[1] = AD_TYPE_FLAGS;
    adv_data[2] = FLAG_LE_GENERAL_DISCOVER_MODE | FLAG_BR_EDR_NOT_SUPPORTED;
    adv_offset = 3;

    //FF-AD_TYPE_MANUFACTURER_SPECIFIC_DATA
    adv_data[adv_offset] = 6 + 1;                                     //length
    adv_data[adv_offset+1] = AD_TYPE_MANUFACTURER_SPECIFIC_DATA;               //ad type
    //经典蓝牙地址
    //memcpy(adv_data+adv_offset+2,ble_customer_app.own_addr,6);
    bt_addr_index_s = adv_offset+2;

    adv_offset = 11;
    //09-AD_TYPE_COMPLETE_LOCAL_NAME
    name_len = (ble_customer_name_len);
    adv_data[adv_offset] = name_len + 1;
    adv_data[adv_offset+1] = AD_TYPE_COMPLETE_LOCAL_NAME;
    memcpy(&adv_data[adv_offset+2], ble_customer_name, ble_customer_name_len);
    adv_offset += name_len + 2;

    //02-AD_TYPE_INCOMPLETE_UUID_16
    adv_data[adv_offset] = 3;
    adv_data[adv_offset+1] = AD_TYPE_INCOMPLETE_UUID_16;
    adv_data[adv_offset+2] = (uint8_t)UUID_SERVICE_02;
    adv_data[adv_offset+3] = (uint8_t)(UUID_SERVICE_02>>8);
    adv_offset += 4;

    logx("adv: adv_offset:%d\n",adv_offset);
    printf_array(adv_data, 31);
    logi(">>>");
    //SCAN RSP
    scan_rsp[0] = 1 + ble_customer_name_len;
    scan_rsp[1] = AD_TYPE_COMPLETE_LOCAL_NAME;
    memcpy(&scan_rsp[2], ble_customer_name, ble_customer_name_len);
}

void ble_customer_addr_set(uint8_t *addr)
{
    libc_memcpy(ble_customer_app.own_addr, addr, 6);
    ble_addr_set(ble_customer_app.own_addr);
}

void ble_customer_addr_type_set(uint8_t type)
{
    ble_customer_app.own_addr_type = type;
}

void ble_customer_adv_update(void)
{
    if (gap_advertisements_enable_get()) {
        adv_data[adv_offset_bat] = bat_quantity_get();    //设备电量
        gap_advertisements_set_data(sizeof(adv_data), adv_data);
    }
}

void ble_customer_adv_param_set(ble_adv_param_t *param)
{
    ble_customer_app.adv_param.adv_int_min = param->adv_int_min;
    ble_customer_app.adv_param.adv_int_max = param->adv_int_max;
    ble_customer_app.adv_param.adv_type = param->adv_type;
}

void ble_customer_init()
{
    ble_customer_adv_data_init();

    ble_customer_app.adv_data = adv_data;
    ble_customer_app.adv_data_len = sizeof(adv_data);
    ble_customer_app.scan_rsp = scan_rsp;
    ble_customer_app.scan_rsp_len = 0;
    ble_customer_app.att_db = (uint8_t *)simple_att_db;
    ble_customer_app.start_handle = 0x0000;
    ble_customer_app.end_handle = 0xFFFF;
    ble_customer_app.read_callback = ble_app_data_read;
    ble_customer_app.write_callback = ble_app_data_receive;

    ble_profile_init(&ble_customer_app);
}

const uint8_t ble_customer_addr_default_l[] = BLE_ADDR_DEFAULT_L;
const uint8_t ble_customer_addr_default_r[] = BLE_ADDR_DEFAULT_R;

const uint8_t ble_customer_name_default_l[] = BLE_NAME_DEFAULT_L;
const uint8_t ble_customer_name_default_r[] = BLE_NAME_DEFAULT_R;


void ble_customer_addr_init()
{
        uint8_t addr[6] = {0};
        int rv;
        logi("####ble_customer_addr_init");
        rv = sys_info_read(BLE_ADDR_INFO, addr, 6);
        if (rv < 0) {
            if (((addr[0]==0x00)&&(addr[1]==0x00)&&(addr[2]==0x00)&&(addr[3]==0x00)&&(addr[4]==0x00)&&(addr[5]==0x00)) || \
                ((addr[0]==0xFF)&&(addr[1]==0xFF)&&(addr[2]==0xFF)&&(addr[3]==0xFF)&&(addr[4]==0xFF)&&(addr[5]==0xFF))) {
                   if (tws_ear_type_get() == TWS_EAR_TYPE_LEFT) {
                        libc_memcpy(addr, ble_customer_addr_default_l, 6);
                    } else {
                        libc_memcpy(addr, ble_customer_addr_default_r, 6);
                    }
                     uint32_t  trng;
                    trng = HAL_trng_get_data();
                    libc_memcpy(addr, &trng, 4);
                    sys_info_write(BLE_ADDR_INFO, addr, 6);
            }
        }
    ble_customer_addr_set(addr);
    logi("ble addr:%02x:%02x:%02x:%02x:%02x:%02x",
            addr[5],addr[4],addr[3],addr[2],addr[1],addr[0]);
    ble_customer_addr_type_set(PUBLIC_ADDRESS);
}

void ble_customer_name_init()
{
    uint8_t ble_name[32] = {0}, ble_name_len;
    if (tws_ear_type_get() == TWS_EAR_TYPE_LEFT) {
        ble_name_len = strlen((char *)ble_customer_name_default_l);
        libc_memcpy(ble_name, ble_customer_name_default_l, ble_name_len);
    } else {
        ble_name_len = strlen((char *)ble_customer_name_default_r);
        libc_memcpy(ble_name, ble_customer_name_default_r, ble_name_len);
    }

    logi("ble name: %s", ble_name);
    ble_customer_name_set(ble_name, ble_name_len);
    sys_info_write(BLE_NAME_INFO,ble_name,32);
}

void ble_customer_adv_param_init()
{
    ble_adv_param_t param;
    param.adv_int_min = 0x430;
    param.adv_int_max = 0x430;
    param.adv_type = ADV_IND;
    ble_customer_adv_param_set(&param);
}

void ble_customer_exit(void)
{
    ble_profile_deinit();
}

uint8_t ble_customer_con = 0;

void ble_customer_con_set(uint8_t con)
{
    ble_customer_con = con;
}

uint8_t ble_customer_con_get(void)
{
    logi("##ble_juplus_con_get,%d",ble_customer_con);
    return ble_customer_con;
}


#endif
