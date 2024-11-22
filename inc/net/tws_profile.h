#ifndef _APP_TWS_H
#define _APP_TWS_H

//#ifdef TWS_SERVICE_SUPPORT

//#include "tws_service.h"
#include "sys_types.h"
#include "sltk_audio.h"

#define TWS_LOCAL_VERSION     (1)
#define TWS_VERSION_INVALID   (0xff)

#define UWS_SEG_RELAY 1
#define SNIFFER_UNCOMPLETE_PACKAGE_SUPPORT  0

typedef enum
{
    TWS_EAR_TYPE_LEFT = 0,
    TWS_EAR_TYPE_RIGHT,
}  tws_ear_type_e;

typedef enum
{
    APP_TWS_IDLE = 0,
    APP_TWS_CONNECTTING,
    APP_TWS_CONNECTED,
    APP_TWS_DISCONNECTTING,
    APP_TWS_DISCONNECTED,
    APP_TWS_RECONNECTTING,
}  tws_state_e;

typedef enum
{
    TWS_CHANNEL_SINGLE = 0,
    TWS_CHANNEL_STEREO,
    TWS_CHANNEL_LEFT,
    TWS_CHANNEL_RIGHT,
} tws_channel_type_e;

typedef enum
{
   TWS_INIT = 0,
   TWS_MASTER,
   TWS_SLAVE,
} tws_mode_e;

enum audio_cmd_e {
    TWS_AUDIO_CMD_NONE        = 0x00,
    TWS_AUDIO_RSP_NONE        = 0x01,

    TWS_AUDIO_CMD_A2DP_START  = 0x02,
    TWS_AUDIO_RSP_A2DP_START  = 0x03,

    TWS_AUDIO_CMD_A2DP_STOP   = 0x04,
    TWS_AUDIO_RSP_A2DP_STOP   = 0x05,

    //TWS_AUDIO_CMD_SCO_START  = 0x06,
    //TWS_AUDIO_RSP_SCO_START  = 0x07,

    TWS_AUDIO_CMD_SCO_STOP   = 0x08,
    TWS_AUDIO_RSP_SCO_STOP   = 0x09,

    TWS_AUDIO_CMD_PLAY_TONE   = 0x0A,
    TWS_AUDIO_RSP_PLAY_TONE   = 0x0B,

    TWS_AUDIO_CMD_STOP_TONE   = 0x0C,
    TWS_AUDIO_RSP_STOP_TONE   = 0x0D,

    TWS_AUDIO_CMD_PLAY_STATUS   = 0x0E, //unused
    TWS_AUDIO_RSP_PLAY_STATUS   = 0x0F, //unused

    TWS_AUDIO_CMD_SCO_UNMUTE  = 0x12,
    TWS_AUDIO_RSP_SCO_UNMUTE  = 0x13,

    TWS_AUDIO_CMD_SCO_SYNC  = 0x14,
    TWS_AUDIO_RSP_SCO_SYNC  = 0x15,

    TWS_AUDIO_CMD_MUSIC_SYNC  = 0x16,
    TWS_AUDIO_RSP_MUSIC_SYNC  = 0x17,

    TWS_AUDIO_CMD_CHANGE_MUSIC_VOLUME         = 0x18,
    TWS_AUDIO_RSP_CHANGE_MUSIC_VOLUME         = 0x19,

    TWS_AUDIO_CMD_NOTIFY_CHANGE_MUSIC_VOLUME = 0x1A,
    TWS_AUDIO_RSP_NOTIFY_CHANGE_MUSIC_VOLUME = 0x1B,

    TWS_AUDIO_CMD_CHANGE_CALL_VOLUME         = 0x1C,
    TWS_AUDIO_RSP_CHANGE_CALL_VOLUME         = 0x1D,

    TWS_AUDIO_CMD_NOTIFY_CHANGE_CALL_VOLUME = 0x1E,
    TWS_AUDIO_RSP_NOTIFY_CHANGE_CALL_VOLUME = 0x1F,

    TWS_AUDIO_CMD_MUSIC_PLL  = 0x20,
    TWS_AUDIO_RSP_MUSIC_PLL  = 0x21,

    TWS_AUDIO_CMD_SAVE_LINKKEY  = 0x22,
    TWS_AUDIO_RSP_SAVE_LINKKEY  = 0x23,

    TWS_AUDIO_CMD_STATUS_PARAMS  = 0x24,
    TWS_AUDIO_RSP_STATUS_PARAMS  = 0x25,

    TWS_AUDIO_CMD_SCO_LOSS_INFO     = 0x26,
    TWS_AUDIO_RSP_SCO_LOSS_INFO     = 0x27,

    TWS_AUDIO_CMD_ANC_SWITCH        = 0x28,
    TWS_AUDIO_RSP_ANC_SWITCH        = 0x29,

    TWS_AUDIO_CMD_WAIT_SLAVE_AUDIO_IDLE = 0x2A,
    TWS_AUDIO_RSP_WAIT_SLAVE_AUDIO_IDLE = 0x2B,

    TWS_AUDIO_CMD_GET_AUDIO_IDLE_STATUS = 0x2C,
    TWS_AUDIO_RSP_GET_AUDIO_IDLE_STATUS = 0x2D,

    TWS_AUDIO_CMD_NOTIFY_PLAY_TONE   = 0x2E,
    TWS_AUDIO_RSP_NOTIFY_PLAY_TONE   = 0x2F,

    TWS_AUDIO_CMD_GAME_MODE        = 0x30,
    TWS_AUDIO_RSP_GAME_MODE        = 0x31,

    TWS_AUDIO_CMD_WAIT_BTAUDIO_IDLE = 0x32,
    TWS_AUDIO_RSP_WAIT_BTAUDIO_IDLE = 0x33,

    TWS_AUDIO_CMD_EQCONFIG_ID       = 0x34,
    TWS_AUDIO_RSP_EQCONFIG_ID       = 0x35,

    TWS_AUDIO_CMD_AZHMI_LONGHOLD    = 0x36,
    TWS_AUDIO_RSP_AZHMI_LONGHOLD    = 0x37,

    TWS_AUDIO_CMD_SCO_AENC_PRM      = 0x38,
    TWS_AUDIO_RSP_SCO_AENC_PRM      = 0x39,
    TWS_AUDIO_CMD_SCO_AENC_EXPRM    = 0x3A,
    TWS_AUDIO_RSP_SCO_AENC_EXPRM    = 0x3B,
    TWS_AUDIO_CMD_SCO_SPKEQ         = 0x3C,
    TWS_AUDIO_RSP_SCO_SPKEQ         = 0x3D,
    TWS_AUDIO_CMD_SCO_SPKDRC        = 0x3E,
    TWS_AUDIO_RSP_SCO_SPKDRC        = 0x3F,
    TWS_AUDIO_CMD_SCO_MICEQ         = 0x40,
    TWS_AUDIO_RSP_SCO_MICEQ         = 0x41,
    TWS_AUDIO_CMD_SCO_MICDRC        = 0x42,
    TWS_AUDIO_RSP_SCO_MICDRC        = 0x43,

    TWS_AUDIO_CMD_MUSIC_DUCKER      = 0x44,
    TWS_AUDIO_RSP_MUSIC_DUCKER      = 0x45,

    TWS_AUDIO_CMD_HTEX_MUTE         = 0x46,
    TWS_AUDIO_RSP_HTEX_MUTE         = 0x47,

    TWS_AUDIO_CMD_SENSOR_PROC_CNT_GET = 0x48,
    TWS_AUDIO_RSP_SENSOR_PROC_CNT_GET = 0x49,
    TWS_AUDIO_CMD_SYNC_VOLUME                   = 0x4A,
    TWS_AUDIO_RSP_SYNC_VOLUME                   = 0x4B,
    TWS_AUDIO_CMD_SLAVE_NOTIFY_CHANGE_VOLUME    = 0x4C,
    TWS_AUDIO_RSP_SLAVE_NOTIFY_CHANGE_VOLUME    = 0x4D,
    TWS_AUDIO_CMD_SEND_EVENT_INFO               = 0x4E,
    TWS_AUDIO_RSP_SEND_EVENT_INFO               = 0x4F,

    TWS_AUDIO_CMD_GET_KWS_TALK_STA              = 0x50,
    TWS_AUDIO_RSP_GET_KWS_TALK_STA              = 0x51,
    TWS_AUDIO_CMD_TALK_STA_UPDATE               = 0x52,
    TWS_AUDIO_RSP_TALK_STA_UPDATE               = 0x53,

    TWS_AUDIO_CMD_DUCK_SYNC                     = 0x54,
    TWS_AUDIO_RSP_DUCK_SYNC                     = 0x55,

    TWS_AUDIO_CMD_APP_TALK_STA_RESET            = 0x56,
    TWS_AUDIO_RSP_APP_TALK_STA_RESET            = 0x57,
};

#define TWS_RETRYACL_FLAG_MASK  (0xF << 4)
#define TWS_TONE_FLAG_MASK      (0xF)

enum tws_retry_acl_flag_e {
    TWS_RETRYACL_FLAG_NONE              = 0x00,
    TWS_RETRYACL_FLAG_KEEP_SYNC_FLAG    = 0x10,
};

enum tws_tone_flag_e {
    TWS_TONE_FLAG_NONE                  = 0x00,
    TWS_TONE_FLAG_CONNECTED             = 0x01,
    TWS_TONE_FLAG_DISCONNECTED          = 0x02,
    TWS_TONE_FLAG_PEER_CONNECTED        = 0x03,
    TWS_TONE_FLAG_PEER_DISCONNECTED     = 0x04,
};

enum tws_music_sync_type_e {
    TWS_MUSIC_SYNC_TYPE_NORMAL,
    TWS_MUSIC_SYNC_TYPE_FIRST_SYNC,
    TWS_MUSIC_SYNC_TYPE_FIRST_SYNC_END,
};

enum tws_music_sync_rsp_type_e {
    TWS_MUSIC_SYNC_RSP_TYPE_MISS_PACKET,
    TWS_MUSIC_SYNC_RSP_TYPE_PACKET_IS_LOSS,
    TWS_MUSIC_SYNC_RSP_TYPE_CMD_TIMEOUT,
    TWS_MUSIC_SYNC_RSP_TYPE_NOT_READY,
    TWS_MUSIC_SYNC_RSP_TYPE_TONE_PLAYING,
    TWS_MUSIC_SYNC_RSP_TYPE_OK,
};

enum tws_acmd_send_ret_e {
    TWS_ACMD_SEND_OK,
    TWS_ACMD_SEND_DISCON = -1,
    TWS_ACMD_SEND_PRMERR = -2,
    TWS_ACMD_SEND_ALLOCFAIL = -3,
    TWS_ACMD_SEND_RSPTIMEOUT = -4,
};

typedef struct
{
    uint8_t tws_local_version;
    uint8_t tws_remote_version;

    int16_t peer_device_music_volume;
    int16_t local_device_music_volume;

    int16_t peer_device_call_volume;
    int16_t local_device_call_volume;

    uint8_t phone_sbc_frame_num;
    uint8_t tws_state;

    uint8_t ear_type;
    tws_mode_e tws_role; // master or slave

    volatile bool  transfer_blocked;
    volatile bool  transfer_is_running;
    volatile bool  transfer_need_stop;
    volatile uint8_t remote_player_role;
    volatile uint8_t remote_player_state;

    /// Current bit offset
    uint16_t bit_off;
    uint16_t rxbit_off;

    // Role switch info
    //tws_master_info_t tws_master_info;
    uint8_t remote_phone_connected;
} tws_env_t;

typedef struct {
    play_timestamp_info_t play_info;
    uint32_t clkn;
    uint32_t finecnt;
    uint32_t sys_t;
    uint32_t l2cap_seq;
    uint8_t play_status;
} tws_master_time_info_t;

typedef struct {
    play_timestamp_info_t play_info;
    uint32_t clkn;
    uint32_t finecnt;
    uint32_t bofs;
    uint32_t sys_t;
} tws_slave_time_info_t;

typedef struct {
    volatile int64_t play;
    volatile uint32_t clkn;
    volatile uint32_t finecnt;
    volatile uint32_t bofs;
    volatile uint32_t sys_t;
} tws_sco_slave_sync_info_t;

typedef struct {
    uint32_t pll_freq;
    uint32_t clkn;
    uint32_t finecnt;
    uint32_t delay_us;
} tws_pll_info_t;

typedef struct {
    uint8_t phone_address[6];
    uint8_t linkkey[16];
} linkkey_info_t;

#define BT_TWS_TASK_PRIO  8
#define BT_TWS_TASK_STACK_SIZE  (256)

void tws_create_connection_to_slave(void);

void tws_transmit_task(void *para);

uint32_t get_bt_clk(void);

int tws_send_audio_common_cmd(uint8_t audio_cmd, uint8_t *params,
    uint8_t params_len, uint32_t wait_timeout_ms, uint32_t try_count);

void tws_send_audio_common_rsp(uint8_t audio_rsp, uint8_t *params,
    uint8_t params_len);
uint8_t get_tws_audio_ctrl_packet_receive_count(uint8_t audio_ctrl);
void tws_manager_app_cmd_send(uint8_t app_cmd, uint8_t *params, uint8_t params_len);
void tws_app_cmd_param_send(uint32_t *event,uint8_t *params, uint8_t params_len);



typedef void (*audio_cmd_receive_func)(void *params, uint32_t params_len);
void tws_audio_cmd_receive_register(uint32_t cmd, audio_cmd_receive_func func);

void on_tws_send_event_info(void *_params, uint32_t params_len);
int tws_send_event_info_to_peer(uint32_t event, const void *params, uint32_t params_len,
    uint32_t wait_timeout_ms, uint32_t try_count);

void tws_transfer_task_notify(void);
void tws_transfer_task_wait(void);
void tws_master_audio_start(void);

tws_env_t *tws_env_ptr_get(void);

extern tws_env_t tws_env;

void tws_service_phone_in_info(uint8_t info_type,uint8_t num_size, uint8_t *buf);

void tws_master_send_status_params(void);

uint8_t tws_get_anc_working_mode(void);
uint32_t tws_get_anc_switch_sys_t(void);
void tws_master_send_anc_working_mode(uint8_t working_mode);
void on_tws_slave_receive_anc_working_mode(void *_params, uint32_t params_len);
void tws_master_send_anc_working_mode_suspend(uint8_t suspend);
void on_tws_slave_receive_game_mode(void *_params, uint32_t params_len);
void tws_send_game_mode(uint8_t enable);
void tws_send_eqconfig_id(uint8_t eqconfig_id);
void on_tws_slave_receive_eqconfig_id(void *_params, uint32_t params_len);
void on_tws_slave_receive_wait_slave_audio_idle(void *_params, uint32_t params_len);
void on_tws_slave_receive_get_audio_idle_status(void *_params, uint32_t params_len);
void on_tws_slave_receive_rsp_get_audio_idle_status(void *_params, uint32_t params_len);

void on_tws_slave_music_receive_a2dp_start_params(void *_params, uint32_t params_len);
void on_tws_slave_music_receive_a2dp_stop_params(void *_params, uint32_t params_len);
void on_tws_slave_sco_receive_sco_stop_params(void *_params, uint32_t params_len);
void on_tws_slave_received_tone_sync_info(void *_params, uint32_t params_len);
void on_tws_master_received_tone_sync_info(void *_params, uint32_t params_len);
void on_tws_slave_received_tone_stop_info(void *_params, uint32_t params_len);
void on_tws_slave_received_status_params(void *_params, uint32_t params_len);

void on_tws_master_receive_sco_unmute_clkn(void *_params, uint32_t params_len);

void on_tws_receive_change_music_volume(void *_params, uint32_t params_len);
void on_tws_receive_notify_change_music_volume(void *_params, uint32_t params_len);
void on_tws_receive_change_call_volume(void *_params, uint32_t params_len);
void on_tws_receive_notify_change_call_volume(void *_params, uint32_t params_len);
void on_tws_receive_wait_btaudio_idle(void *_params, uint32_t params_len);
void on_tws_receive_rsp_wait_btaudio_idle(void *_params, uint32_t params_len);
void on_tws_music_azhmi_longhold_receive(void *_params, uint32_t params_len);
void on_tws_music_azhmi_longhold_rsp_proc(void *_params, uint32_t params_len);

void on_tws_receive_sco_aenc_prm(void *_params, uint32_t params_len);
void on_tws_receive_sco_aenc_exprm(void *_params, uint32_t params_len);
void on_tws_receive_sco_spkeq(void *_params, uint32_t params_len);
void on_tws_receive_sco_spkdrc(void *_params, uint32_t params_len);
void on_tws_receive_sco_miceq(void *_params, uint32_t params_len);
void on_tws_receive_sco_micdrc(void *_params, uint32_t params_len);
void on_tws_music_ducker_receive(void *_params, uint32_t params_len);

void on_tws_receive_loss_info(void *_params, uint32_t params_len);
void on_tws_slave_sco_receive_sync_info(void *_params, uint32_t params_len);
void on_tws_slave_sco_receive_sync_rsp(void *_params, uint32_t params_len);

void on_tws_slave_music_receive_sync_info(void *_params, uint32_t params_len);
void on_tws_master_music_receive_sync_rsp(void *_params, uint32_t params_len);
void on_tws_slave_music_receive_pll_info(void *_params, uint32_t len);

void on_cmd_sensor_proc_cnt_get(void *_params, uint32_t params_len);
void on_rsp_sensor_proc_cnt_get(void *_params, uint32_t params_len);
uint32_t tws_sensor_proc_cnt_get(void);

void tws_slave_report_audio_idle_status(void);

uint8_t tws_con_state_get(void);
void tws_master_acl_start(void);
void tws_master_acl_stop(void);

void tws_master_get_time_info(tws_master_time_info_t *info, void *atrack);
void tws_slave_get_time_info(tws_slave_time_info_t *info, void *atrack);
uint32_t get_bt_delta_us(uint32_t m_clkn, uint32_t m_finecnt,
    uint32_t s_clkn, uint32_t s_finecnt, uint32_t s_bitoffset);

#define TWS_DATA_PACKET_MAX_SZ  (679)
#define TRANSFER_BUFFER_SIZE     (1024)

extern uint8_t transfer_buffer[TRANSFER_BUFFER_SIZE];
extern uint8_t transfer_buffer2[TRANSFER_BUFFER_SIZE];


//void tws_con_init(void);

void tws_create_con_to_peer(uint32_t timeout);
void tws_con_slave_start();
//uint8_t tws_con_get_role();
void tws_peer_addr_set(uint8_t *bt_addr);
void tws_peer_addr_get(uint8_t *bt_addr);
void tws_dump_peer_addr();
uint8_t tws_create_con_to_phone(uint8_t phone_tog, uint32_t timeout);

uint8_t tws_con_state_get(void);
uint8_t tws_role_get(void);
void tws_con_disconnect_slave(void);

void tws_ear_type_set(uint8_t left_ear);
uint8_t tws_ear_type_get(void);

void tws_last_role_set(uint8_t role);
uint8_t tws_last_role_get();
void tws_peer_con_status_set(uint8_t connected);
uint8_t tws_peer_con_status_get();
void tws_phone_con_status_set(uint8_t connected);
uint8_t tws_phone_con_status_get();
void tws_slave_phone_con_status_set(uint8_t connected);
uint8_t tws_slave_phone_con_status_get();
void tws_sco_con_state_set(uint8_t connected);
uint8_t tws_sco_con_state_get();
void tws_phone_disconnected_clear();
uint8_t tws_need_recon_phone_get();
void tws_need_recon_phone_set(uint8_t recon);
uint8_t tws_need_recon_peer_get();
void tws_need_recon_peer_set(uint8_t recon);
void tws_scan_enable_set(uint8_t enable);
uint8_t tws_scan_enable_get();
void tws_phone_disconnect_reason_set(uint8_t reason);
uint8_t tws_phone_disconnect_reason_get(void);
void tws_slave_phone_disconnect_reason_set(uint8_t reason);
uint8_t tws_slave_phone_disconnect_reason_get(void);
void tws_profile_init(void);

void tws_peer_afh_set_ignore(uint8_t set_flag);
void tws_peer_afh_set_flag(uint8_t set_flag);
uint8_t tws_peer_afh_get_flag(void);
void tws_peer_afh_set_waiting(uint8_t waiting);
uint8_t tws_peer_afh_get_waiting(void);
void tws_peer_afh_send(void);
void tws_peer_afh_set_handler(uint8_t mode,uint8_t htbl,uint8_t *master_clk, uint8_t *map, uint8_t set_flg);
void tws_peer_afh_init(void);
void tws_peer_afh_reset(void);
void tws_sniffer_afh_reset();

void tws_process_clkn_handle(void);
void tws_start_ble_ota(void);
void tws_start_enc(void);
void tws_service_cmd_start_enc_handler(void);
void tws_service_rsp_start_enc_handler(uint8_t *buf);
void tws_peer_enc_reset(void);


//////////////////////////////////////////////////////////////////////////////////


typedef enum {
    TWS_CMD_TWS_START       = 0x00,
    TWS_RSP_TWS_START       = 0x01,

    TWS_CMD_DIS_TWS         = 0x02,
    TWS_RSP_DIS_TWS         = 0x03,

    TWS_CMD_TWS_EVENT       = 0x0A,
    TWS_RSP_TWS_EVENT       = 0x0B,

    TWS_CMD_TWS_VERSION     = 0x0C,
    TWS_RSP_TWS_VERSION     = 0x0D,

    TWS_CMD_SHARE_MASTER_INFO_TO_SALVE = 0x0E,
    TWS_RSP_SHARE_MASTER_INFO_TO_SALVE = 0x0F,

    TWS_CMD_ROLE_SWITCH     = 0x10,
    TWS_RSP_ROLE_SWITCH     = 0x11,

    TWS_CMD_CONN_INFO       = 0x12,
    TWS_RSP_CONN_INFO       = 0x13,

    TWS_CMD_SCO_START       = 0x14,
    TWS_RSP_SCO_START       = 0x15,

    TWS_CMD_SCO_STOP        = 0x16,
    TWS_RSP_SCO_STOP        = 0x17,

    TWS_CMD_ACL_START       = 0x1C,
    TWS_RSP_ACL_START       = 0x1D,

    TWS_CMD_ACL_STOP        = 0x1E,
    TWS_RSP_ACL_STOP        = 0x1F,

    TWS_CMD_RSSI_INFO       = 0x38,
    TWS_RSP_RSSI_INFO       = 0x39,

    TWS_CMD_PHONE_IN_INFO   = 0x3E,
    TWS_RSP_PHONE_IN_INFO   = 0x3F,

    TWS_CMD_AUDIO_COMMON    = 0x46,
    TWS_RSP_AUDIO_COMMON    = 0x47,

    TWS_CMD_SEAMLESS_INFO   = 0x48,
    TWS_RSP_SEAMLESS_INFO   = 0x49,

    TWS_CMD_AUDIO_CTL_INFO  = 0x4A,
    TWS_RSP_AUDIO_CTL_INFO  = 0x4B,

    TWS_CMD_AUDIO_LMP_INFO  = 0x4C,
    TWS_RSP_AUDIO_LMP_INFO  = 0x4D,

	TWS_CMD_AUDIO_A2DP_INFO  = 0x4E,
    TWS_RSP_AUDIO_A2DP_INFO  = 0x4F,
    // transimitted by LMP
    TWS_CMD_ACLTF_BITMAP    = 0xC0,
    TWS_RSP_ACLTF_BITMAP    = 0xC1,

    TWS_CMD_SET_AFH_TEST    = 0xC2,
    TWS_RSP_SET_AFH_TEST    = 0xC3,

    TWS_CMD_SET_AFH_FORCE   = 0xC4,
    TWS_RSP_SET_AFH_FORCE   = 0xC5,

    TWS_CMD_SEAMLESS_REQ    = 0xC6,
    TWS_RSP_SEAMLESS_REQ    = 0xC7,

    TWS_CMD_SET_AFH_INFO    = 0xC8,
    TWS_RSP_SET_AFH_INFO    = 0xC9,

    TWS_CMD_OTA_UPDATE      = 0xCA,
    TWS_CMD_OTA_LOW_POWER   = 0xCB,

    TWS_CMD_SET_PEER_AFH    = 0xCC,
    TWS_RSP_SET_PEER_AFH    = 0xCD,

    TWS_CMD_APP_COMMON      = 0xCE,
    TWS_RSP_APP_COMMON      = 0xCF,

    TWS_CMD_START_ENC       = 0xD0,
    TWS_RSP_START_ENC       = 0xD1,

	TWS_CMD_ACLTF_BITMAP2   = 0xD2,
    TWS_RSP_ACLTF_BITMAP2   = 0xD3,

    TWS_CMD_PAIR            = 0xD4,

    TWS_CMD_START_BLE_OTA   = 0xD5,

    TWS_CMD_MASS_PRODUCTION_TEST = 0xD6,
    TWS_RSP_MASS_PRODUCTION_TEST = 0xD7,

    TWS_CMD_MP_TEST_PEER = 0xD8,
    TWS_RSP_MP_TEST_PEER = 0xD9,

    TWS_CMD_MP_HFP_AT_PEER = 0xDA,
    TWS_RSP_MP_HFP_AT_PEER = 0xDB,

    TWS_CMD_ENCRYPT_KEY_UPDATE = 0xDC,

    TWS_CMD_UI_TEST = 0xDE,
    TWS_RSP_UI_TEST = 0xDF,

    TWS_CMD_PEER_AFH_NEGO    = 0xE0,
    TWS_RSP_PEER_AFH_NEGO    = 0xE1,
} tws_control_packet_type_e;

#define MAX_AUDIO_COMMON_CTRL_PARAMS_LEN (256)
typedef struct
{
    uint8_t     ctl_packet_type; // should be TWS_CMD_AUDIO_COMMON
    uint8_t     audio_ctrl;
    uint8_t     params_len;
    uint8_t     params[MAX_AUDIO_COMMON_CTRL_PARAMS_LEN];
} __attribute__ ((__packed__)) audio_common_ctrl_t;

//TODO: 需要初始化tws_process_loop_hook & tws_audio_data_callback_hook
extern void (*tws_process_loop_hook)(void);
extern void (*tws_audio_data_callback_hook)(uint8_t *buf, uint16_t len);

uint32_t tws_get_clkn(void);
uint16_t bt_finecnt_getf(void);
uint16_t tws_get_bit_offset(uint8_t LinkId);
void tws_get_cryptk_key(uint8_t LinkId, uint8_t *crypt_en, uint16_t *cryptk_key);
uint32_t tws_get_bt_clock_offset(uint8_t LinkId);
void s_sniff_clock_update(uint32_t clkn, uint32_t clockoffset,uint16_t bitoffset);
int32_t tws_service_send_data_packet(uint8_t *buf, uint16_t len, bool sco_data, uint32_t delay);
void s_sniffer_cryptk_update(uint8_t cryptk_en, uint16_t *cryptk_key);
void bt_set_hf_codec_type(uint8_t type);
uint8_t tws_service_send_control_packet(uint8_t *buf, uint16_t len, uint32_t delay);
void tws_service_sco_stop(void);
void tws_service_sco_start(uint8_t codec, uint8_t volume, uint8_t play_tone,
    uint8_t tone_num, uint8_t tone_flag, uint8_t tone_volume, uint32_t audiopll, uint32_t tone_delay_ms,
    uint32_t clkn, uint32_t finecnt, uint8_t sync, uint16_t sync_packet);
uint8_t tws_service_get_peer_device_music_volume(void);
uint8_t tws_service_get_peer_device_call_volume(void);
typedef void (*tws_data_handler_cb_t)(uint8_t *buf, uint16_t len);
void tws_service_register_data_handler_cb(tws_data_handler_cb_t callback);
void tws_process_wait(void);
void tws_control_process(void);
void tws_service_acl_start(uint8_t *audio_params, uint16_t audio_params_len);
void tws_master_sco_link_start(void);
uint8_t avdtp_write_media_data(uint8_t *payload, uint16_t media_packet_len);
void ble_find_debug_port_set(uint8_t port,uint8_t time_last);
extern void btsched_set_last_l2cap(uint16_t seq);
extern void btsched_reset_data(uint16_t seq);
extern void btsched_acl_get_audio_bitmap(uint8_t loss_num,uint16_t start_seq);

void tws_process_register_callback(void (*process_loop_hook)(void));


uint8_t playing_link_need_reset(void);
void playing_link_reset(void);
void playing_addr_get(uint8_t *addr);

void bt_sniffer_data_rx_report_register_callback(void (*hook)(uint32_t *rxbuf,uint8_t valid));
void ld_sco_manage_ctrl_entity_register_callback(void (*hook)(uint8_t *tx_ptr, uint8_t txlen, uint16_t clkn));

void tws_master_receive_play_status_rsp(uint8_t sta);

void tws_audio_process(void);

void tws_register_app_cmd_handler(void (*handler)(uint8_t *buf));
void tws_register_audio_cmd_handler(void (*handler)(uint8_t *buf));

//for uws
void uws_uncomplete_acl_buf_save_do();
extern void uws_fragment_info_s_set(uint8_t fragment_buf_start, uint16_t fragment_size, uint16_t total_size,uint8_t llid);
extern void uws_last_complete_packettime_set(uint8_t role, uint16_t conn_handle,uint8_t *bufptr);
extern uint16_t uws_uncomplete_get_fragment_info_totalsize(uint8_t index, uint8_t para_num);
extern uint16_t uws_uncomplete_get_hci_buf_seq(uint8_t index);
extern uint8_t *uws_uncomplete_get_hci_buf_ptr(uint8_t index);
extern void uws_btdis_close();
extern void uws_btdis_close_all();
extern void uws_peerdis_close();
uint8_t uws_get_init_mode();
extern uint8_t uws_mode_switch_set(uint8_t enable);
uint32_t uws_and_mode_set_finished(void);
extern uint8_t uws_get_mode();
void uws_recriver_data_register_callback(
    uint8_t (*uws_hook)(uint8_t *buf, uint16_t len, uint16_t seq)
    );
uint8_t uws_fragment_buf_num_get();

//#define MAX_ACL_LEN 88
#define MAX_ACL_LEN 64

#define A2DP_BS_PER_FRAME_MAX 7
#define SBC_INSERT_THD 100
struct a2dp_bs_seq_record_s {
    uint16_t seq_num;
    uint16_t cid;
    uint8_t per_frame_max;
    uint8_t per_frame_avg;
    uint16_t l2cap_seq_pre;
    uint16_t skip_count;
    uint8_t  insert_flg;
};
extern struct a2dp_bs_seq_record_s a2dp_bs_seq_record; // only one link record now
extern volatile uint8_t btmusic_inited;

extern volatile uint8_t slave_audio_is_idle;
extern uint8_t media_codec_type;
extern uint8_t phone_link_id;
extern bool bt_con_flag;
extern uint8_t afh_flag;
extern uint8_t bitmap_wait2send;
extern bool tws_process_task_exit;
extern uint32_t debug_avdtp_receive_data_sniffer_cnt;
extern volatile uint16_t monitor_media_fragment_size;
extern volatile uint16_t monitor_media_total_size;
extern uint8_t monitor_media_buf[900];
extern uint16_t data_sniffer_cid;
extern uint16_t media_data_sniffer_cid;
extern uint32_t l2_recv;
extern uint32_t l2_total;
extern uint32_t l2_trans;
extern uint16_t l2cap_seq_trans_req;
extern uint32_t debug_avdtp_receive_data_cnt;
extern uint8_t avdtp_play_state_err_cnt;
extern uint32_t debug_s_sniff_data_rx_report_loss_cnt;
extern uint32_t debug_sco_rxerr_count_sniffer;
extern uint8_t debug_msbc_num;
extern volatile uint32_t phone_sco_intant;
extern uint8_t s_sniff_m2s_data_len;
extern uint32_t debug_s_sniff_data_rx_report_normal_cnt;
extern uint32_t debug_sco_rx_count_sniffer;
extern uint32_t debug_ld_sco_manage_ctrl_entity_normal_cnt;
extern uint32_t debug_ld_sco_manage_ctrl_entity_loss_cnt;
extern uint32_t debug_sco_rxerr_count;
extern uint32_t debug_sco_rx_count;
extern uint8_t fragment_num;
extern uint8_t fragment_buf_used;
extern uint16_t debug_a2dp_dec_cnt;
extern int16_t debug_dac_data;
extern uint32_t debug_sco_process_cnt;
extern uint32_t debug_a2dp_bs_remain_ms;

int8_t choose_phone_to_recon(uint8_t from);

//#endif
#endif
