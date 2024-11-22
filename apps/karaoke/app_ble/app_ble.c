#define LOG_LEV 5
#include "sys_inc.h"
#include "user_config.h"
#include "ble_profile.h"
#include "ble_api.h"
#include "app_ble.h"
#include "mode_bt.h"
#include "air_api.h"

void ble_ota_addr_init();
void ble_ota_adv_param_init();
void ble_ota_name_init();


uint8_t app_ble_adv_status;

void app_ble_adv_stop(void)
{
    /* app adv stop,
     * air adv start。
     * 注意：(1)如果已经是air adv，就不要再切换。(2)切换后需要立即生效。
     */
    logi("%s", __func__);
    if (app_ble_adv_status_get()) {
        ble_linkplus_exit();
#if BLE_AIR_SEL
        if (air_en_get()==0x01) {
            ble_air_addr_reinit();
            ble_air_adv_param_init();
            ble_air_init();
        }
#endif
        app_ble_adv_status_set(0x00);
    }
}

void app_ble_adv_start(void)
{
    /* air adv stop,
     * app adv start
     */
    uint8_t addr[6];
    uint32_t rand;

    logi("%s", __func__);
    if (!ble_juplus_con_get()) {
        app_ble_adv_status_set(0x01);
#if BLE_AIR_SEL
        if (air_en_get()==0x01) {
            ble_air_exit();
        }
#endif
        rand = HAL_trng_get_data();
        addr[0] = (uint8_t)rand;
        addr[1] = (uint8_t)(rand>>8);
        addr[2] = (uint8_t)(rand>>16);
        addr[3] = (uint8_t)(rand>>24);
        addr[4] = addr[0] ^ addr[2];
        addr[5] = addr[1] ^ addr[3];
        addr[5] |= 0xc0;
        ble_ota_addr_set(addr);
        logx("app adv:");
        printf_array(addr, 6);
        ble_ota_name_init();
        ble_ota_adv_param_init();
        ble_ota_addr_type_set(RANDOM_ADDRESS);
        ble_linkplus_init();
    }
}

void app_ble_adv_status_set(uint8_t value)
{
    app_ble_adv_status = value;
}

uint8_t app_ble_adv_status_get(void)
{
    /* 0-app adv没有运行
     * 1-app adv已运行
     */
    return app_ble_adv_status;
}

