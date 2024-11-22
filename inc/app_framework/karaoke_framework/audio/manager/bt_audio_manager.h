#ifndef _BT_AUDIO_MANAGER_H_
#define _BT_AUDIO_MANAGER_H_

#include "sys_inc.h"
#include "event.h"

enum game_mode_e {
    GAME_MODE_CLOSE,
    GAME_MODE_OPEN,
    GAME_MODE_OPEN_AFX,
};

typedef void (*proxy_func_t)(void);
typedef void (*proxy_func2_t)(uint8_t param);

void am_btaudio_volume_step_set(uint8_t volume_step);
uint8_t am_btaudio_volume_step_get(void);
uint8_t am_btaudio_music_volume_value_get(void);
void am_btaudio_music_volume_value_set(uint8_t btmusic_volume);
uint8_t am_btaudio_call_volume_value_get(void);
void am_btaudio_call_volume_value_set(uint8_t btcall_volume);
uint8_t am_btaudio_con_tone_played_get(void);
void am_btaudio_con_tone_played_set(uint8_t played);
uint8_t am_btaudio_discon_tone_played_get(void);
void am_btaudio_discon_tone_played_set(uint8_t played);
uint8_t am_btaudio_scene_get(void);
void am_btaudio_scene_set(uint8_t audio_scene);
uint8_t am_btaudio_calling_state_get(void);
void am_btaudio_calling_state_set(uint8_t calling_state);


void am_btaudio_vol_down(void);
void am_btaudio_vol_up(void);
void am_a2dp_gameafx_plugin_register(void);
void am_btaudio_game_mode_switch(uint8_t game_mode);
void am_btaudio_game_mode_fix(uint8_t game_mode);
uint8_t am_btaudio_game_mode_get(void);
bool am_btaudio_call_aenc_mode_get();
void am_btaudio_call_aenc_mode_set(bool en_call_aenc);
void am_btaudio_eqconfig_id_switch(uint8_t eqconfig_id);
void am_btaudio_eqconfig_id_fix(uint8_t eqconfig_id);
uint8_t am_btaudio_eqconfig_id_get(void);
void am_air_start(void);
void am_air_stop(void);

void am_btaudio_incoming_number_callback(uint8_t* num, uint8_t size);
void am_btaudio_bt_calling_state_set(uint8_t state);
uint8_t am_btaudio_bt_calling_state_get(void);
void am_btaudio_bt_calling_state_sync(void);
void am_btaudio_calling_process(void);
void am_btaudio_process_when_vbus_in(void);
void audio_aenc_demo_switch(void);
void audio_aenc_demo_clear(void);
void am_1mic_aenc_plugin_register(void);
void am_2mic_aenc_plugin_register(void);
void am_third_party_aenc_plugin_register(
    void (*on_open)(uint32_t fs, uint8_t mic_ch_count, uint32_t proc_samples),
    void (*on_close)(void),
    void (*on_proc)(int16_t *spk_in, int16_t *mic_in,
        int16_t *spk_out, int16_t *mic_out, int32_t proc_samples));

bool am_btaudio_a2dp_is_0data(void);
void am_btaudio_stop(void);
void am_btaudio_start(void);
void am_btaudio_task_init(void);
void am_btaudio_task_deinit(void);

void am_btauduio_disconnect_event_process(void);
void am_btauduio_connect_event_process(void);
void am_btaudio_proximity_event_process(void);
void am_btaudio_m_connected_event_process(void);
void am_btaudio_s_connected_event_process(void);
void am_btaudio_peer_disconnected_event_process(void);
void am_btaudio_tws_phone_disconnected_event_process(void);
void am_btaudio_event_process(sys_event_info_t *info);

void am_a2dp_space_audio_register(void);
void am_btaudio_space_audio_switch(uint8_t enable);
uint8_t am_btaudio_space_audio_status_get(void);

void audio_proxy_init(void);
void audio_proxy_deinit(void);
void audio_proxy_process(sys_event_info_t *info);
void am_btaudio_bat_balance_aujust(uint32_t cur_bat, uint32_t pair_bat, bool pair_suspen_stat);
void am_set_pair_kws_state(bool enable);
void am_btaudio_kws_state_reset(void);
#endif