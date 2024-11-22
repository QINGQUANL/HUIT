#ifndef _MODE_BT_H_
#define _MODE_BT_H_


void bt_siri_start(void);
void bt_siri_stop(void);
void spp_register_receive_data_handler(uint8_t (*func_handler)(uint8_t *ptr, uint8_t len));
void bt_name_info_get(void);



#endif
