#ifndef _BT_API_H_
#define _BT_API_H_


typedef struct {
    uint8_t   test_control;	 // 0: stop; 1: start
    uint8_t   freq_type;     // 0: hop;  1: hop
    uint8_t   channel_idx;   // 0~79
    uint8_t   tx_power;      // 0~32
    uint8_t   rx_gain;       // 0~32
    uint8_t   rx_gain_auto;
    uint8_t   role_idx;      // 0:slave 1:master -> node_idx
    uint8_t   tx_type;       // packe type
    uint16_t  tx_pkt_len;    // 0~1024
    uint8_t   rx_type;       // packe type
    uint16_t  rx_pkt_len;    // 0~1024
    uint8_t   payload_type;  // 0: 0x0;  1:0xff  2:0x55  3:0xf0:  4:prbs9
    uint8_t   tx_edr;
    uint8_t   rx_edr;
} fcc_control_t;
extern fcc_control_t fcc_control;

struct hci_control {
    volatile uint8_t event_rcv_num;
    volatile uint8_t event_proc_num;

    volatile uint8_t acl_rcv_num;
    volatile uint8_t last_acl_rcv_num;
    volatile uint8_t acl_proc_num;
    volatile uint8_t last_acl_proc_num;

    volatile uint32_t sco_rcv_num;
    volatile uint32_t sco_proc_num;
};

enum active_status_e
{
    BT_HOST_ACTIVE,
    BT_HOST_INACTIVE,
    BT_CONTROLOR_ACTIVE,
    BT_CONTROLOR_INACTIVE,
    BT_RFC_ACTIVE,
    BT_RFC_INACTIVE,
};

enum con_status_e
{
    PHONE_CONNECT = 0,
    PHONE_DISCONNECT,
    PHONE_CONNECT_TIMEOUT,
    PEER_CONNECT,
    PEER_DISCONNECT,
    PEER_CONNECT_TIMEOUT,
};

void btstack_init(void);
void btstack_exit(void);
void ble_init(void);
void ble_deinit(void);
void bt_frontend_init(uint8_t type);
void rf_drift_set(uint32_t cap);
void bt_frontend_init_incharge();
void rf_para_dft();
void btdm_config(void);
void rf_config(uint8_t cal_enable);
uint8_t bt_get_hf_codec_type(void);
uint8_t bt_sco_status_get_by_idx(uint8_t index);

void get_efuse_rfpara(uint32_t *efuse_para,uint8_t len);
void bt_initial_exchange_memory(void);
void bt_ll_init();

//enable 1 to force power
// power value:   0~18
// power in dbm: (-34+2*power) dbm
void rf_power_force(uint8_t enable, uint8_t power);
void bt_rf_power_max_set(uint8_t rf_power_max_val);
void rf_txpwr_force(uint8_t link_id, uint8_t value);
// To set RF freq offset,
// 7 bits unsigned valid, default value is 0x60,
// freq offset is in the same direction as value.
void rfdss_freq_offset_set(uint8_t fre_value);

void phone_rssi_set(uint16_t rssi);
uint16_t phone_rssi_get(void);

void bt_testmode_set(uint8_t enable);
uint8_t bt_testmode_get(void);
uint8_t bt_autotest_seamless_get();
uint8_t bt_autotest_recon_get();
void seamless_abort_set();
void seamless_sco_deinit(uint8_t lid);
void seamless_deinit(void);
void bt_testmode_set(uint8_t enable);
uint8_t bt_testmode_get(void);

bool bt_sleep(void);
bool btble_sleep(void);
void uws_rsp_disable(uint8_t link, uint8_t disable);
void co_alloc_buf_statistic(uint8_t *tx_cnt_o,uint8_t *tx_cnt0_o,uint8_t *tx_cnt1_o,uint8_t *rx_cnt_o);
void sniff_req_phone_clear();
uint8_t sniff_req_phone_get(uint8_t *link_id, uint16_t *con_handle, uint16_t *t_sniff,uint16_t *sniff_to);
void bt_testbqb_set(uint8_t enable);
uint8_t bt_testbqb_get(void);

uint8_t rf_txpwr_get(uint8_t link_id);
int16_t rf_rssi_get(uint8_t Lid);

void spp_printf(const char *fmt, ...);
uint32_t tws_get_clkn(void);


void bt_sco_tx_rf_enable();
void bt_sco_tx_rf_disable();
uint8_t bt_sco_tx_rf_enable_get();
void bt_sniffer_sco_get(uint8_t **tx_ptr,uint8_t *txlen);
void bt_addr_init();
void a2dp_support_sbc_only_set(uint8_t enable);

uint8_t bt_create_con_to_phone(uint8_t phone_tog, uint32_t timeout);

void tws_acl_extend_data_tx_enable();
void tws_acl_extend_data_tx_disable();
uint8_t tws_acl_extend_data_tx_get();

uint8_t ld_acl_extend_get(uint8_t linkid);
uint8_t ld_acl_extend_enable(uint8_t linkid);
uint8_t ld_acl_extend_disable(uint8_t linkid);

#endif

