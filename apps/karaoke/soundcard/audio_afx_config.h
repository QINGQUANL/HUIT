#ifndef AUDIO_AFX_H
#define AUDIO_AFX_H

#include "sys_inc.h"

void audioeffect_eq14_register();
void audioeffect_vss_register(void);
void audioeffect_fdnrev_register(void);
void audioeffect_drc_register(void);
void audioeffect_tsps_register(void);
void audioeffect_echo_register(void);
void audioeffect_freqshift_register(void);
void audioeffect_register(void);
void audioeffect_enable(void);
void audioeffect_fc_register(void);
void audioeffect_hldrc_register(void);

void mic_eq14_config(void);
void music_eq14_config(void);
void vss_config(void);
void fdnrev_config(void);
void mic_fdnrev_refresh(void);
bool mic_fdnrev_enable_set(uint8_t enable);
void mic_drc_natural_noise_gate_config(void);
void music_drc_natural_noise_gate_config(void);
void freqshift_config(void);
void music_drc_lt_gate_config();
void mic_tsps_female_config(void);
void mic_tsps_male_config(void);
void mic_tsps_robot_config(void);
void mic_tsps_kid_config(void);
void mic_tsps_vc_config(void);
void mic_tsps_female_config_close(void);
void mic_tsps_male_config_close(void);
void mic_tsps_robot_config_close(void);
void mic_tsps_kid_config_close(void);
void mic_tsps_vc_config_close(void);
void mic_autotune_config(void);
#endif
