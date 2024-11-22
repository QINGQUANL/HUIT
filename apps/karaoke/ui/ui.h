#ifndef _UI_H_
#define _UI_H_




void ui_init(void);
uint8_t ui_anc_mode_config_get(void);
void ui_anc_mode_switch(uint32_t event);
void g24_receive_cmd_proc(uint8_t link, uint8_t* cmd, uint8_t max_len);
void ui_process(uint32_t event, uint32_t params_len, uint8_t *params);
void g24_ui_process_do(uint32_t event, uint32_t params_len, uint8_t *params);
void bt_ui_process_do(uint32_t event, uint32_t params_len, uint8_t *params);
void ui_process_do(uint32_t event);
void proximity_ui_init(void);
void proximit_ui_sync_stat(void);
void ui_poweroff(void);

void ui_play_when_sensor_p_ht(void);
void ui_play_pause(uint8_t play_status);
void auto_monkey_test(void);
uint8_t get_mokey_test_state(void);
void set_mokey_test(uint8_t enable);
void ui_space_audio_switch(void);
uint8_t ui_sensor_play_stat_get(void);
void ui_sensor_play_stat_set(uint8_t stat);
uint8_t ui_sensor_pause_stat_get(void);
void ui_sensor_pause_stat_set(uint8_t stat);
void usb_epaudio_init(void);
void usb_epaudio_init_new(void);
void afc_ui_process_do(uint32_t event, uint32_t params_len, uint8_t *params);
void app_audio_set_dac_gain(int8_t gain);
void app_audio_set_lpga_gain(int8_t gain);
void app_audio_set_rpga_gain(int8_t gain);
void app_audio_set_xpga_gain(int8_t gain);
void app_audio_set_digital_mic0_gain(int8_t gain);
void app_audio_set_digital_mic1_gain(int8_t gain);
void app_audio_set_digital_mic2_gain(int8_t gain);
void app_audio_set_analog_mic0_gain(int8_t gain);
void app_audio_set_analog_mic1_gain(int8_t gain);
void app_audio_set_analog_mic2_gain(int8_t gain);
void app_audio_set_analog_linein0_gain(int8_t gain);
void app_audio_set_analog_linein1_gain(int8_t gain);
void app_audio_set_analog_aux0_gain(int8_t gain);
void app_audio_set_analog_aux1_gain(int8_t gain);
void app_audio_set_analog_aux2_gain(int8_t gain);
void amixer_spdif_volume_set(int8_t volume);
void amixer_localmic_volume_set(int8_t volume);
void amixer_wirelessmic_volume_set(int8_t volume);
void amixer_usb_audio_volume_set(int8_t volume);
void amixer_i2s_volume_set(int8_t volume);
void amixer_tone_volume_set(int8_t volume);
void amixer_aux_volume_set(int8_t volume);
void amixer_play_set(int8_t volume);
void g24_rf_power_set(int8_t power);

#endif
