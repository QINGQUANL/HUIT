#ifndef _AIR_API_H_
#define _AIR_API_H_


void air_app_init(void);

uint8_t air_poweron(void);
uint8_t air_poweroff(uint8_t mode);

uint8_t air_bat_partner_get(void);
uint8_t air_param_phone_type_get(void);
uint8_t air_param_link_type_get(void);
uint8_t air_param_anc_or_siri_sel_left_get(void);
uint8_t air_param_anc_or_siri_sel_right_get(void);
uint8_t air_param_proximity_config_get(void);
uint8_t air_param_hey_siri_config_get(void);
uint8_t air_param_ver2_sel_left_get(void);
uint8_t air_param_ver2_sel_right_get(void);
uint8_t air_param_app_get(uint8_t param_id);
void air_param_set_m2s(uint8_t *param);

uint16_t air_profile_set(void);
void air_anc_send_data(uint8_t mode);
void air_wear_update(void);
void air_siri_set(uint8_t enable);
void air_when_exit(void);

void ble_air_addr_init(void);
void ble_air_adv_param_init(void);
void ble_air_exit(void);
void ble_air_version_set(uint8_t ver);

void ble_air_when_poweroff(void);
uint8_t air_spp_data_receive(uint8_t *buf, uint8_t len);


#endif

