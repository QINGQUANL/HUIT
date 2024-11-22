#ifndef _APP_BLE_CUSTOMER_H_
#define _APP_BLE_CUSTOMER_H_
void ble_customer_init();
void ble_customer_exit(void);
void ble_customer_addr_init();
void ble_customer_name_init();
void ble_customer_adv_param_init();
void ble_customer_con_set(uint8_t con);
uint8_t ble_customer_con_get(void);
void ble_customer_send(uint8_t *buf, uint16_t size);
void ble_customer_adv_update_enable_addr(bool enable);
void ble_customer_ota_send(uint8_t *buf, uint16_t size);

#endif
