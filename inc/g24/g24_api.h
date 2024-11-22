#ifndef _G24_API_H_
#define _G24_API_H_
#include "sys_inc.h"

enum {
    MONITOR_USER_CMD_HEARTBREAK = 0xAA,    //心跳包
    MONITOR_USER_CMD_DISCONNECT = 0xCC,    //断连前主动发给dongle的命令
};

enum g24_func_role {
    G24_FUNC_ROLE_HANDSET= 0,
    G24_FUNC_ROLE_DONGLE = 1,
};

enum g24_device_role_type {
    G24_DEVICE_ROLE_SLAVER = 0,
    G24_DEVICE_ROLE_MASTER = 1,
};


enum rf_group_type {
    GROUP_2_4G,
    GROUP_2_3G,
    GROUP_2_5G,
    GROUP_23_25G,
    GROUP_2_4G_FRONT,   //2400MHz-2440MHz, 仅用于初始化
    GROUP_2_4G_BACK,    //2442MHz-2483MHz, 仅用于初始化
    GROUP_NUM,
};

typedef enum
{
    /// idle state
    BT_IDLE=0,
    /// inquiry state
    BT_INQUIRY,
    /// page state
	BT_PAGE,
	/// connect poll state
	BT_CONNECT_POLL,
	/// connect task state
	BT_CONNECT_TASK,
}BT_STATE;

typedef struct {
    uint8_t device_role;
    uint8_t func_role;
    bool pa_en;
    uint32_t pair_code0;
    uint32_t pair_code1;
    uint8_t rssi_thrs;
    uint8_t reconnect_rssi_thrs;
    bool master_reconnect_no_rssi;
    uint32_t slaver_reconnect_no_rssi_time;
    uint32_t master_reconnect_time;
    bool master_reconnect_always;
    uint8_t master_inquiry_times;
    uint8_t master_page_times;
    uint32_t slaver_reconnect_time;
    uint8_t inquiry_maxpower;
    uint8_t connect_maxpower;
    bool page_replace_inquiry;
    bool rf_jump_en;
    bool uplink_2ch;
    bool dnlink_2ch;
    bool sbc_send_4times;
    bool dual_channel;
    bool dual_channel_ul_long_pkt;
    bool dual_audio_long_send_4times;
    bool dual_short_3times;
    bool dual_2T4_short_3times;
    bool afc_en;
    bool use_sbt_connect;
} g24_param_t;


typedef struct {
    void (*receive_cmd_proc)(uint8_t link, uint8_t* cmd, uint8_t max_len);
    void (*lm_rx_rssi_check_log)(uint8_t rssi_cal, bool is_match);
    void (*err_patck_check)(uint8_t link, uint16_t err, uint16_t rel_err, uint16_t total);
    void (*paircode_gen)(uint8_t link, uint32_t *datap);
    bool (*paircode_check)(uint8_t link, uint32_t *datap);
    bool (*found_pair_device)(uint8_t link);
    void (*rx_power_check)(uint8_t link, int8_t power);
    bool (*rx_rssi_con_en_get)(uint8_t link, int8_t rssi_cs, uint8_t *remote_addr);
    void (*monito_common_cmd_receive)(uint8_t link, uint32_t monitor_id, uint8_t user_cmd);
    void (*monitor_audio_receive_processed)(uint32_t master_mic_id);
    void (*monitor_dongle_set_master_mic_id)(uint32_t master_mic_id);//dongle设置主MIC ID的回调
    void (*broadcast_code_gen)(uint32_t* broadcast_code);
    void (*broadcast_code_receive)(uint32_t broadcast_code);

    void (*page_user_data_gen)(uint8_t link, uint32_t *datap, uint8_t max_len);
    bool (*page_user_data_rec)(uint8_t link, uint32_t *datap, bool *access_code_change, uint8_t max_len);
} g24_cb_t;

typedef struct
{
    uint32_t connect_timeout;
    bool trigger_connect;
    bool en;
    bool is_master;
    bool is_master_mic_tx_enable;  //dongle发送命令下拉设置这个变量
    bool is_dongle;
    bool no_handshake;
    bool no_handshake_none_continue;
    uint8_t audio_ch;
} g24_monitor_t;

//base
void comm_init(void);
void comm_deinit(void);
void g24_local_addr_get(uint8_t * addr);
void g24_local_addr_set(uint8_t * addr);


//g24 param & cmd
void g24_param_config(g24_param_t *param);
g24_param_t * g24_param_get(void);
void g24_cb_register(g24_cb_t *cb);
g24_cb_t *g24_cb_get(void);
void g24_send_cmd(uint8_t link, uint8_t* cmd, uint8_t len);         
uint8_t g24_cmd_max_len_get(void);									//获取最大命令长度，不同配置不一样


//afh & rf
uint8_t g24_afh_send_size_get(void);
void p24g_timer_afh_fer_entity(uint8_t Lid);
void g24_rf_tbl_set(short *freq_int, int *freq_frac);
void p24g_afh_rf_reset();
bool p24g_channel_map_set(uint32_t channel_mapl, uint32_t channel_mapm, uint16_t channel_maph, uint8_t min_chan_num);
bool afh_monitor_triger_one_get(uint8_t link);
void monitor_set_slave_id(uint8_t link, uint32_t id);
uint32_t monitor_handset_detect_stamp_get();
bool g24_is_monitor_master_mic_connected();
void monitor_user_cmd_send_set(uint8_t link,bool set, uint8_t user_cmd);
bool monitor_heartbreak_is_enable();
void monitor_device_connect_set(bool is_connect);
void monitor_heartbreak_enable(bool en);

//bt_top
uint8_t bt_top_task_status_get(uint8_t link);
void bt_top_disconnect(uint8_t link);
void bt_top_clear_link_flg(uint8_t link);
void bt_top_set_link_addr(uint8_t link, uint8_t *addr);          //设置回连地址
void bt_top_set_locallink_addr(uint8_t link);                    //设置local_addr等于回连地址
void bt_top_link_enable_set(uint8_t link, bool en);              //设置使能link连接流程
bool bt_top_link_enable_get(uint8_t link);                       //获取使能link连接流程	
void g24_bt_cnt_clear(void);


//middle level
void g24_mid_stop_task(uint8_t link);
uint8_t g24_mid_master_link_get(void);							 //获取slaver作为link0还是link1, 连接上时会更新


//bt_audio
void bt_audio_control_init(bool dongle_role);

//codec
void g24_put_bs_func_register(void (*put_bs_func)(uint8_t link, uint8_t *bs, uint32_t bytes, bool valid));
void g24_get_bs_func_register(uint8_t (*get_bs_func)(uint8_t link, uint8_t *bs, uint32_t bytes));
uint8_t g24_encode_sel_get(void);
void g24_tx_packet_size_set(uint8_t packet_size);
uint8_t g24_tx_packet_size_get(uint8_t link);
void g24_rx_packet_size_set(uint8_t packet_size);
uint8_t g24_rx_packet_size_get(uint8_t link);
uint8_t g24_packet_header_size_get(void);
uint8_t g24_packet_tail_size_get(void);
void g24_packet_tail_size_set(uint8_t size);
void g24_encode_sel_set(uint8_t type);
void g24_decode_sel_set(uint8_t type);

uint8_t g24_packet_rx_tail_size_get(void);
void g24_packet_rx_tail_size_set(uint8_t size);

//sbt
void g24_sbt_init(void);
void g24_sbt_rand_seed_set(uint32_t seed);
void g24_sbt_ctrl_chan_set(uint8_t ch0, uint8_t ch1, uint8_t ch2);


//monitor
void g24_monitor_init(void);
void g24_monitor_param_init(g24_monitor_t *param);
void g24_monitor_en_set(bool en);
bool g24_monitor_en_get(void);
void g24_monitor_master_addr_set(uint8_t *addr);
g24_monitor_t *g24_monitor_get(void);
void g24_monitor_master_mic_connect_set(bool master_connect);
bool g24_is_monitor_master_mic_connected();
bool g24_is_monitor_master_mic_tx_enable();
void g24_monitor_master_mic_tx_enanle(bool enable);
bool g24_is_monitor_mic_tx_enable();
bool g24_is_monitor_master_mic();

bool g24_master_mic_check();
uint32_t g24_monitor_m_mic_id_get();
void bt_paircode_set(uint32_t code0, uint32_t code1);
void monitor_user_cmd_send_set(uint8_t link,bool set, uint8_t use_cmd);
void g24_bt_e_rspdsb_set(bool tx_dis);
void g24_audio_set_monitor_id(uint32_t monitor_id);
void g24_audio_set_m_monitor_id(uint32_t monitor_id);
uint32_t g24_audio_get_m_monitor_id();
void g24_enable_no_handshake(bool noHandshake);
void g24_rf_deinit();
void comm_task_pause();

void g24_during_delete_link_set(bool work);

#endif

