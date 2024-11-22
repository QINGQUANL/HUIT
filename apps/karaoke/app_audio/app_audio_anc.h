#ifndef _APP_ANC_H__
#define _APP_ANC_H__

void anc_mode_init(void);
void anc_mode_sel_set(uint8_t sel);
uint8_t anc_mode_sel_get(void);
void anc_or_siri_sel_set(uint8_t sel_right, uint8_t sel_left);
uint8_t anc_or_siri_sel_right_get(void);
uint8_t anc_or_siri_sel_left_get(void);
void anc_mode_param_set(uint8_t mode);
uint8_t anc_mode_param_get(void);
void anc_mode_param_store(uint8_t mode);

void anc_mode_set(uint8_t mode);
void anc_mode_set_process(void);
void anc_mode_switch(void);
void anc_mode_suspend(void);
void anc_mode_resume(void);

void anc_play_tone(uint8_t mode);
void anc_param_set(uint8_t *param);
void anc_param_sync(void);
void anc_mode_2in_param_set(uint8_t mode);
void anc_mode_1in_param_set(uint8_t mode);
uint8_t anc_mode_1in_param_get(void);
uint8_t anc_mode_2in_param_get(void);
void anc_mode_fix(uint8_t mode);
void poweroff_anc_mode_param_set(void);
void poweron_anc_mode_param_get(void);
void anc_mode_timer_start(void);
void anc_mode_timer_stop(void);

void anc_mode_switch_do(void);

#endif
