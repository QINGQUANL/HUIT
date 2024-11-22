#ifndef _BT_HOST_API_H
#define _BT_HOST_API_H

#include "event.h"

#define BT_NAME_MAX         32

//#define BT_HSHF_EN                      1
//#define BT_A2DP_EN                      1
//#define BT_SPP_EN                       1
//#define BT_HID_EN                       0

#define BT_HSHF_3WCALL_EN               1
#define BT_HSHF_BAT_SHOW_EN             1
#define BT_VOL_CTRL_EN                  1   //use absolute volume
#define BT_VOL_CHANGE_EN                2   //use volume change

#define FT_HSF_BAT_SHOW                 0x01    //iphone battery level
#define FT_HSF_3WAY_CALL                0x02    //3way calling control
#define FT_HSF_IOS_RING                 0x04    //IOS incoming call ring
#define FT_HSF_VOICE_REC                0x08    //voice recognization
#define FT_A2DP_VOL_CTRL                0x01    //iphone volume control
#define FT_HSF_CODEC_NEGO               0x10    //codec negotiation

#define BT_SUPPORT_FT_HSF       ((FT_HSF_3WAY_CALL*BT_HSHF_3WCALL_EN) | (FT_HSF_BAT_SHOW*BT_HSHF_BAT_SHOW_EN))
#define BT_SUPPORT_FT_A2DP      (FT_A2DP_VOL_CTRL*BT_VOL_CTRL_EN)

enum prof_e
{
    PROF_A2DP       = 0x0007,   //bit0,1,2
    PROF_HANDSFREE  = 0x0008,   //bit3
    PROF_HEADSET    = 0x0010,   //bit4
    PROF_SPP        = 0x0020,   //bit5
    PROF_A          = 0x0180,   //bit7,8
    PROF_PNP        = 0x0200,   //bit9
    PROF_BLE        = 0x0400,
    PROF_TWS        = 0x0800,
    PROF_RFCOMM     = PROF_HEADSET|PROF_HANDSFREE|PROF_SPP,

    SDP_HSHF        = 0x1000,
    SDP_SPP         = 0x2000,
    SDP_PNP         = 0x4000,
    PROF_SDP        = SDP_HSHF|SDP_SPP|SDP_PNP,
};

//bt_get_status can get the status of BT
enum {
    BT_STATUS_OFF,                                 //0 bt mode disable
    BT_STATUS_INIT,                                //1 init
    BT_STATUS_IDLE,                                //2 not connect

    BT_STATUS_SLEEP,                               //3 low power

    BT_STATUS_DISCONNECTING,                       //4 disconnecting
    BT_STATUS_CONNECTING,                          //5 connecting

    BT_STATUS_CONNECTED,                           //6 connected
    BT_STATUS_PLAYING,                             //7 playing
    BT_STATUS_INCOMING,                            //8 incoming call ring
    BT_STATUS_OUTGOING,                            //9 outgoing calling
    BT_STATUS_CALLING,                             //10 calling

    BT_STATUS_CALL_TEST_START,                     //11 call test start
    BT_STATUS_CALL_TEST_END,                       //12 call test end
};

typedef enum {
    BT_EVENT_NONE           = 0,
    BT_EVENT_DISCONNECT     = BT_EVENT | 0x1,   //disconncet
    BT_EVENT_LOSTCONNECT    = BT_EVENT | 0x2,   //far distance connect lost
    BT_EVENT_CONNECTED      = BT_EVENT | 0x3,   //connect success
    BT_EVENT_PLAY           = BT_EVENT | 0x4,   //start play
    BT_EVENT_STOP           = BT_EVENT | 0x5,   //stop play
    BT_EVENT_INCOMING       = BT_EVENT | 0x6,   //incoming event
    BT_EVENT_OUTGOING       = BT_EVENT | 0x7,   //outgoing event
    BT_EVENT_CALLING        = BT_EVENT | 0x8,   //calling
    BT_EVENT_INCOMING_END   = BT_EVENT | 0x9,   //incoming end event
    BT_EVENT_CALLING_END    = BT_EVENT | 0xA,   //calling end event
    BT_EVENT_BAT_EN         = BT_EVENT | 0xB,   //enable update battery level
    BT_EVENT_BAT_UP         = BT_EVENT | 0xC,   //update battery level
    BT_EVENT_VOL_DOWN       = BT_EVENT | 0xD,   //set volume down of device from phone
    BT_EVENT_VOL_UP         = BT_EVENT | 0xE,   //set volume up of device from phone
    BT_EVENT_ANSWERING      = BT_EVENT | 0xF,
    BT_EVENT_ANSWERING_END  = BT_EVENT | 0x10,
    BT_EVENT_SCO_CON        = BT_EVENT | 0x11,  //sco connected
    BT_EVENT_SCO_DISCON     = BT_EVENT | 0x12,  //sco disconnected
    BT_EVENT_SIRI_ENABLE    = BT_EVENT | 0x13,  //siri enable
    BT_EVENT_STOP_SIRI      = BT_EVENT | 0x14,  //stop siri
    BT_EVENT_START_SIRI     = BT_EVENT | 0x15,  //start siri from iphone
    BT_EVENT_SIRI_ERROR     = BT_EVENT | 0x16,  //siri error
    BT_EVENT_INCOMING_NUM   = BT_EVENT | 0x17,
    BT_EVENT_SAVE_LINKKEY   = BT_EVENT | 0x18,   //save linkkey
    BT_EVENT_OTA_UPDATE     = BT_EVENT | 0x19,   //update
    BT_EVENT_SYNC_LINKKEY   = BT_EVENT | 0x1A,   //sync linkkey with Master/left earphone
    BT_EVENT_SML_PEER_DONE  = BT_EVENT | 0x1B,   //seamless peer done
    BT_EVENT_PAIR           = BT_EVENT | 0x1C,
    BT_EVENT_BLE_OTA        = BT_EVENT | 0x1D,
    BT_EVENT_SIRI_ENABLE_OK = BT_EVENT | 0x1E,  //siri enable OK
    BT_EVENT_SIRI_DISABLE_OK = BT_EVENT | 0x1F,  //siri disable OK
    BT_EVENT_STACK_UPDATE   = BT_EVENT | 0x20,  //siri disable OK
    BT_EVENT_SPP_SEND       = BT_EVENT | 0x21,
    BT_EVENT_SIGLE_WAVE     = BT_EVENT | 0x22,
    BT_EVENT_UR_ENTER_DUT   = BT_EVENT | 0x23,
    BT_EVENT_CALLING_REFUSE = BT_EVENT | 0x24,   //calling refuse
    BT_EVENT_VOL_SET        = BT_EVENT | 0x25,   //volume set
    BT_EVENT_CALL_VOL_SET   = BT_EVENT | 0x26,   //call volume set
    BT_EVENT_ANC_SET        = BT_EVENT | 0x27,   //anc set
    BT_EVENT_AVCTP_PLAY     = BT_EVENT | 0x28,
    BT_EVENT_AVCTP_PAUSE    = BT_EVENT | 0x29,
    BT_EVENT_RING           = BT_EVENT | 0x2A,
    BT_EVENT_SIRI_END       = BT_EVENT | 0x2B,  //siri end
    BT_EVENT_SNIFF_ENTER    = BT_EVENT | 0x2C,
    BT_EVENT_SNIFF_EXIT     = BT_EVENT | 0x2D,
    BT_EVENT_CLEAN_LINK     = BT_EVENT | 0x2E,
    BT_EVENT_CON_DISP       = BT_EVENT | 0x2F,
    BT_EVENT_DIS_DISP       = BT_EVENT | 0x30,
    BT_EVENT_GAME_MODE_DISP       = BT_EVENT | 0x31,
    BT_EVENT_NOTIYF_DEFAULT_VOL   = TWS_EVENT | 0x32,
    BT_EVENT_TWS_DISCON_DIRECT    = TWS_EVENT | 0x33,
    BT_EVENT_PEER_SNIFF_ENTER     = BT_EVENT | 0x34,
    BT_EVENT_PEER_SNIFF_EXIT      = BT_EVENT | 0x35,
    BT_EVENT_STACK_LINK_UPDATE    = BT_EVENT | 0x36,
    BT_EVENT_MUSIC_VOL_SET        = BT_EVENT | 0x37,

    BT_EVENT_TWS_PEER_CONNECTED         = TWS_EVENT | 0x1,
    BT_EVENT_TWS_PHONE_CONNECTED        = TWS_EVENT | 0x2,
    BT_EVENT_TWS_CONNECT_PEER_TIMEOUT   = TWS_EVENT | 0x3,
    BT_EVENT_TWS_CONNECT_PHONE_TIMEOUT  = TWS_EVENT | 0x4,
    BT_EVENT_TWS_ROLE_CHANGED           = TWS_EVENT | 0x5,
    BT_EVENT_TWS_SERVICE_M_CONNECTED    = TWS_EVENT | 0x6,
    BT_EVENT_TWS_SERVICE_S_CONNECTED    = TWS_EVENT | 0x7,
    BT_EVENT_TWS_PEER_DISCONNECTED      = TWS_EVENT | 0x8,
    BT_EVENT_TWS_PHONE_DISCONNECTED     = TWS_EVENT | 0x9,
    BT_EVENT_TWS_SEAMLESS_FINISH        = TWS_EVENT | 0xa,
    BT_EVENT_TWS_SEAMLESS_SKIP          = TWS_EVENT | 0xb,
    BT_EVENT_TWS_SLAVE_VBUS_IN          = TWS_EVENT | 0xc,
    BT_EVENT_TWS_ENTER_SCAN_MODE        = TWS_EVENT | 0xd,
    BT_EVENT_TWS_STOP_RECONNECT         = TWS_EVENT | 0xe,
} bt_event_t;

typedef struct
{
    uint8_t dis_reason;
    uint8_t sniff_status;
    int8_t rssi;
} bt_dis_status_t;

typedef struct
{
    uint8_t bd_addr[6];
    uint16_t con_handle;
    uint8_t reason;
} bt_dis_param_t;

bool bt_is_connect(void);
void btstack_profile_set(uint16_t profile);
uint16_t btstack_profile_get(void);

#define hshfvol_get_hook(x) btstack_call_vol_get(x) //range in 0~15 by wxp

void bt_deep_sleep_enable(bool enable);
void bt_set_scan(bool scan, bool link);
uint8_t bt_phone_addr_get(uint8_t *phone_bdaddr);
uint8_t bt_disconnect_direct(void);
uint8_t bt_connect_link_n(void);
uint8_t bt_disconnect_phone_by_conhdl(uint16_t con_hdl);

void bt_reset(void);
uint16_t link_conhdl_get(void);
void link_conhdl_slave_set(uint16_t conhdl);

/********************************************************
 * A2DP
********************************************************/
void audio_play_time_hook(uint8_t *ptr);
void bt_audio_forward(void);
void bt_audio_backward(void);
void audio_play(void);
void audio_pause(void);
void audio_play_pause(void);
void audio_rewind(void);
void audio_fast_forward(void);
void audio_rewind_end(void);
void audio_fast_forward_end(void);
void bt_audio_volume_change(void);
uint8_t bt_playing_check();
uint8_t audio_mmi_play_pause_state_get(void);

void bt_volume_info_set(uint8_t *addr, uint8_t volume, bool sel_call);
uint8_t bt_volume_info_get(uint8_t *addr, bool sel_call);
void bt_volume_info_save(void);
uint8_t bt_volume_music_default_get(void);
bool new_linkey_flag_get(void);
void new_linkey_flag_clean(void);

uint8_t bt_a2dp_status_get(uint8_t index);
uint8_t bt_sco_status_get(void);
uint32_t bt_play_state_get(void);
void bt_play_state_set(uint8_t state);
uint8_t bt_a2dp_get_playing_addr(uint8_t *bd_addr);

bool tws_phone_enter_sniff_req(uint8_t link_id, uint16_t con_handle, uint16_t t, uint16_t to);
bool tws_phone_exit_sniff_req(void);
void tws_master_retx_sco_acl_create_link(void);
bool tws_peer_exit_sniff_req(void);
bool tws_peer_enter_sniff_req(void);
void btstack_controller_process();
void sniffer_data_update(uint8_t * buf, uint16_t len);

#define WORK_EDR        BIT(0)
#define WORK_BLE        BIT(1)

/*
 * default is N4010
 * MT8852B: 0x88522
 * N4010:0x40100
 */
extern void bt_test_equipment_set(uint32_t value);

void bt_name_set(uint8_t *name, uint8_t len);
void bt_name_get(uint8_t *name, uint8_t *len);
void bt_addr_set(uint8_t *addr);
void bt_addr_get(uint8_t *addr);
void bt_stop_active();
void bt_disconnect_phone(void);
void bt_disconnect_peer(void);
void bt_recon_abort(uint8_t bdaddr[]);
bool bt_recon_busy();
uint8_t bt_status_get(void);
void bt_app_status_set(uint8_t status);
uint8_t bt_app_status_get(void);
uint8_t seamless_process_status();
uint8_t tws_peer_judge_exit_sniff_req(void);
void tws_phone_judge_exit_sniff_req(void);

bt_dis_status_t* bt_dis_status_get();
uint8_t bt_profile_con_status_get();
uint8_t tws_get_connected_phone_num(void);
uint8_t bt_hfp_status_get(uint8_t index);
uint8_t bt_connect_do(uint8_t bdaddr[], uint16_t profile, uint8_t times, uint8_t scan_en);


#endif
