#ifndef _SOUNDCARD_H_
#define _SOUNDCARD_H_

#include "sys_inc.h"
#include "aeffect_echo.h"
#include "aeffect_fdnrev.h"
#include "aeffect_drc.h"
#include "aeffect_vss.h"
#include "aeffect_freqshift.h"
#include "aeffect_eq14.h"
#include "aeffect_tsps.h"
#include "aeffect_hldrc.h"

void usbplay_start(uint32_t fs);
void usbplay_write(void *buf, uint32_t samples);
void usbrec_read(void *buf, uint32_t bytes);
void i2susb_read(void *buf, uint32_t bytes);
bool is_usbplay_ready(void);

void soundcard_tracks_create(void);

void Lmic_listener_dry_vol_set(float vol_db);
float Lmic_listener_dry_vol_get(void);
void Lmic_listener_dry_vol_mute_set(uint8_t mute);
uint8_t Lmic_listener_dry_vol_mute_get(void);

void Lmic_wet_vol_set(float vol_db);
float Lmic_wet_vol_get(void);
void Lmic_wet_vol_mute_set(uint8_t mute);
uint8_t Lmic_wet_vol_mute_get(void);

void Lmic_play_dry_vol_set(float vol_db);
float Lmic_play_dry_vol_get(void);
void Lmic_play_dry_vol_mute_set(uint8_t mute);
uint8_t Lmic_play_dry_vol_mute_get(void);

bool Lmic_aenc_enable_get(void);
void Lmic_aenc_enable_set(uint8_t enable); //Local Mic降噪开关

void wrx_listener_dry_vol_set(float vol_db);
float wrx_listener_dry_vol_get(void);
void wrx_listener_dry_vol_mute_set(uint8_t mute);
uint8_t wrx_listener_dry_vol_mute_get(void);

void wrx_wet_vol_set(float vol_db);
float wrx_wet_vol_get(void);
void wrx_wet_vol_mute_set(uint8_t mute);
uint8_t wrx_wet_vol_mute_get(void);

void wrx_play_dry_vol_set(float vol_db);
float wrx_play_dry_vol_get(void);
void wrx_play_dry_vol_mute_set(uint8_t mute);
uint8_t wrx_play_dry_vol_mute_get(void);

void tone_listener_vol_set(float vol_db);
float tone_listener_vol_get(void);
void tone_listener_vol_mute_set(uint8_t mute);
uint8_t tone_listener_vol_mute_get(void);

void tone_play_vol_set(float vol_db);
float tone_play_vol_get(void);
void tone_play_vol_mute_set(uint8_t mute);
uint8_t tone_play_vol_mute_get(void);

void aux_listener_vol_set(float vol_db);
float aux_listener_vol_get(void);
void aux_listener_vol_mute_set(uint8_t mute);
uint8_t aux_listener_vol_mute_get(void);

void aux_play_vol_set(float vol_db);
float aux_play_vol_get(void);
void aux_play_vol_mute_set(uint8_t mute);
uint8_t aux_play_vol_mute_get(void);

void usbplay_listener_vol_set(float vol_db);
float usbplay_listener_vol_get(void);
void usbplay_listener_vol_mute_set(uint8_t mute);
uint8_t usbplay_listener_vol_mute_get(void);

void usbplay_play_vol_set(float vol_db);
float usbplay_play_vol_get(void);
void usbplay_play_vol_mute_set(uint8_t mute);
uint8_t usbplay_listener_vol_mute_get(void);
void am_usbplay_volume_set(uint8_t vol);

int am_playback_volume_set(uint8_t vol);
uint8_t am_playback_volume_get(void);
int am_playback_volume_down(void);
int am_playback_volume_up(void);

int am_record_volume_set(uint8_t vol);
uint8_t am_record_volume_get(void);
int am_record_volume_down(void);
int am_record_volume_up(void);

void afc_adaptive();    //自适应模式，开机仅能进入一次，结束后自动进入afc处理模式
void afc_proc();        //AFC处理模式
void afc_proc_close();   //AFC处理模式关
void duck_enbale_set(bool en);
bool duck_enbale_get(void);
void vss_config_set(const vss_tunningarea *setting);
void vss_enable_set(uint8_t enable);
void fdnrev_enable_set(uint8_t enable);
uint8_t fdnrev_enable_get(void);
void fdnrev_config_set(const fdnrev_pro_setting *setting);
void drc_enable_set(uint32_t page_id, uint8_t enable);
void drc_config_set(uint32_t page_id, uint8_t func_mask, const drc_subfunc_setting *setting);
void drc_comm_param_set(uint32_t page_id, const drc_comm_param_setting *setting);

void tsps_config_set(uint8_t tsps_enable, float alpha_ps, uint8_t atune_enable, uint8_t fc_enable, float alpha_fc);
void autotune_config_set(atune_setting_t *atune);
void fc_config_set(uint8_t fc_enable, float alpha_fc);
void echo_config_set(const echo_setting_t *setting);
void freqshift_config_set(const freqshift_subfunc_setting * setting);
void eq14_config_set(uint32_t page_id, const aeffect_eq_sec14_tunningarea * setting);

void hldrc_config_drc_set(uint32_t page_id, uint8_t pass, uint8_t func_mask, const hldrc_subfunc_setting *setting);
void hldrc_comm_param_set(uint32_t page_id, uint8_t pass, const hldrc_comm_param_setting *setting);
void hldrc_enable_set(uint32_t page_id, uint8_t enable);
void hldrc_fc_set(uint32_t page_id, int32_t fc);

#endif