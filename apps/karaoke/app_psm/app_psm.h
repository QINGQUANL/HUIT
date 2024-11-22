#ifndef _APP_PSM_H_
#define _APP_PSM_H_


enum am_volume_info_e {
    AM_VOLUME_INFO_PLAYBACK,
    AM_VOLUME_INFO_RECORD,
    AM_VOLUME_INFO_MAX,
};

typedef struct {
    uint8_t volume[AM_VOLUME_INFO_MAX];
} am_volume_info_t;

enum {
    WORK_MODE_NONE = 0,
    WORK_MODE_SET_G24,
    WORK_MODE_SET_BT,
//    WORK_MODE_SET_USBDEV,
};

enum {
    USBAUDIO_MODE_STEREO,
    USBAUDIO_MODE_DOLBY,    
};

void g24_remote_addr_set(uint8_t *addr);
void g24_remote_addr_get(uint8_t *addr);
void g24_remote_addr_clear(void);
bool g24_info_need_save(void);
void g24_earphone_addr_get(uint8_t *bd_addr);
void g24_earphone_addr_and_linkkey_save(uint8_t *addr);
void g24_link_info_save(uint8_t link);
void g24_link_info_delete(uint8_t del_link0, uint8_t del_link1);
uint8_t g24_link_addr_get(uint8_t link, uint8_t *bd_addr, bool get_remote);
void g24_addr_save(uint8_t *addr);

void am_volume_all_info_get(uint8_t default_volume);
void am_volume_info_set(enum am_volume_info_e volume_index, uint8_t volume);
uint8_t am_volume_info_get(enum am_volume_info_e volume_index);
void am_volume_info_save(void);
bool nor_mp_dut_flag_get(void);

bool is_usb_update_flag_set();
void clear_usb_update_flag();
void set_usb_update_flag();
void g24_afc_param_get(uint16_t* afc_param);
void g24_afc_param_save(uint16_t afc_param);
void work_mode_set(uint8_t work_mode);
void work_mode_get(uint8_t* work_mode);
uint8_t psm_usb_audio_mode_get();
void psm_usb_audio_mode_set(uint8_t work_mode);
bool psm_is_rtc_time_has_set();
void psm_rtc_time_flag_set();
void psm_rtc_time_flag_clear();
uint32_t psm_get_master_mic_id();
void psm_save_master_mic_id(uint32_t mmic_id);
#endif

