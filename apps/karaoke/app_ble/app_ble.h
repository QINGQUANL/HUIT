#ifndef _APP_BLE_H_
#define _APP_BLE_H_


void app_ble_adv_stop(void);
void app_ble_adv_start(void);
void app_ble_adv_status_set(uint8_t value);
uint8_t app_ble_adv_status_get(void);

void ble_air_addr_reinit(void);
void ble_air_adv_param_init(void);
void ble_air_exit(void);
uint32_t HAL_trng_get_data(void);


#endif
