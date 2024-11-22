#ifndef _BT_MANAGER_H_
#define _BT_MANAGER_H_

void bt_scan_enable_set(uint8_t enable);
uint8_t bt_scan_enable_get(void);
void bt_recon_phone_en_set(uint8_t en);
uint8_t bt_recon_phone_en_get();
void bt_manager_init();
void bt_manager_exit();


#endif
