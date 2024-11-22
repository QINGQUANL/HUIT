#ifndef _BT_H_
#define _BT_H_

#include "event.h"

#define REC_POWEROFF_TIME 1

enum bt_calling_state_e {
    BT_CALLING_STATE_NULL,
    BT_CALLING_STATE_INCOMING,      //来电中
    BT_CALLING_STATE_OUTGOING,      //去电中
    BT_CALLING_STATE_ON,            //通话中
};

typedef struct _BTBOX_UR_CMD_
{
	uint8_t start0;
	uint8_t start1;
	uint8_t start2;
    uint8_t channel;
	uint8_t cmd;
	uint8_t data[9];
    uint8_t crc16L;
    uint8_t crc16H;
}EwsUrCmd;


typedef struct {
    volatile uint8_t status;
} bt_mode_t;

typedef struct _S_ANC_CMD_
{
    char channel;
    char command;
    char control;
    char data[6];
}__attribute__((packed)) s_anc_cmd;

extern bool bt_con_flag;

void bt_sniff_init(void);
void bt_sniff_deinit(void);
void bt_sniff_suspend(bool wait);
void bt_sniff_resume(void);
void bt_sniff_enter(void);
void bt_sniff_exit(void);
void bt_sniff_exit_wait(void);
void bt_sniff_resume_state_set(bool state);
bool bt_sniff_resume_state_get(void);
void bt_wait_exit_sniff(void);

void mode_bt(void);
void mode_bt_enter(void);
void mode_bt_loop(void);
void mode_bt_exit(void);

void bt_rf_power_force(uint8_t enable, uint8_t power);
void bt_rf_maxpower_set(uint8_t enable, uint8_t power);
void bt_answering_enter(void);
void bt_event_process(sys_event_info_t *info);
void bt_event_process_10s(void);
void bt_deal_notice_msg(uint32_t msg);

void btstack_hfp_process(void);
uint8_t btstack_hfp_get_call_status(void);
void btstack_hfp_reject_incoming_call(void);
void btstack_hfp_redial_last_number(void);
void btstack_hfp_swap_hangup_call(void);
void btstack_hfp_swap_calls(void);
void btstack_hfp_iphone_calls(void);
void btstack_hfp_iphone_call_siri(void);
void btstack_hfp_iphone_end_siri(void);
void btstack_hfp_answer_2nd_call_do(void);

void bth_init(void);
void bth_exit(void);
void* task_bth(void *pvParameters);
void btc_init(void);
void btc_exit(void);
void* task_btc(void *pvParameters);
void *bt_isr_bh(void *pvParameters);
void bt_disable(void);
uint8_t bt_siri_enable_get(void);
void bt_siri_start(void);
void bt_siri_stop(void);

void hsp_hfp_sent_battery_to_phone(uint8_t batval);
void hci_write_scan_enable(uint8_t scan_enable);
bool hci_create_connect_cancel(uint8_t * bd_addr);
void gap_advertisements_enable(int enabled);

void spp_app_init();
void spp_app_exit();
void spp_app_yh_init();
void spp_app_yh_exit();

uint8_t bt_connect_do(uint8_t bdaddr[], uint16_t profile, uint8_t times, uint8_t scan_en);
void bt_volume_feature(uint8_t feature);
void bt_call_volume_change(void);
uint8_t bt_connect(uint8_t times, uint8_t scan_en);
void freq_drift_save(uint8_t value);
void bt_tws_pair(void);
void ble_find_stop(bool clear);
void ble_find_var_reset();
void bt_seamless_peer_done();
uint8_t seamless_phone_connected_get();
uint8_t seamless_peer_sniff_get();
void seamless_app_prevent_set(int8_t enable,uint32_t delay_ms,int8_t reason);
void ble_find_set_full_flag(uint8_t flag);

uint8_t bt_autotest_chargemode_do(void);
uint8_t bt_autotest_btcon_do(void);
void tws_scan_state_enter();
void tws_auto_scan_mode_set(bool auto_scan);

uint8_t bt_testmode_get(void);

void restart(void);

void debug_update_adv_data(void* xTimer);

void bt_register_event_handler(void (*event_handler)(sys_event_info_t *info));
void bt_tws_dut_mode_init(void);
void app_bt_reconnect_phone(void);
void bt_air_sel_set(uint32_t value);
uint32_t bt_air_sel_get(void);
void link_info_save(void);

#endif

