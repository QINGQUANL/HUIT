#ifndef _HAL_PMU_LIB_H_
#define _HAL_PMU_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif

enum pmu_set {
    PMU_SET_DIS,
    PMU_SET_EN,
    PMU_SET_KEEP,
};

enum pmu_int_triger_mode {
    PMU_INT_FALLING_EDGE,
    PMU_INT_RISING_EDGE,
};

enum pmu_onoff_hsw_e {
    PMU_ONOFF,
    PMU_HSW,
};

enum pmu_onoff_state {
    PMU_ONOFF_DW,
    PMU_ONOFF_UP,
};

enum pmu_hsw_state {
    PMU_HSW_STATE_OFF,
    PMU_HSW_STATE_ON,
};

enum pmu_pwron_reason {
    PMU_VBUS_PON,
    PMU_ONOFFL_PON,
    PMU_ONOFFS_PON,
    PMU_ONOFFUS_PON,
    PMU_HSW_PON,
    PMU_ONOFFLRST_PON,
    PMU_RESTART_PON,
    PMU_SYSIN_PON,
};

enum pmu_pwroff_reason {
    PMU_SW_EN_POFF,
    PMU_ONOFFRST_POFF,
    PMU_RESTART_POFF,
    PMU_LB_PON,
    PMU_DCUV_PON,
    PMU_PWROK_PON,
    PMU_LDOUV_PON,
    PMU_VBUSOV_PON,
};

enum pmu_battery_charging_state {
    PMU_BAT_NO_CHARGE,
    PMU_BAT_CHARGING,
};

enum pmu_battery_exist_state {
    PMU_BAT_NON_EXIST,
    PMU_BAT_EXIST,
};

enum pmu_regu_state {
    PMU_REGU_GOOD,
    PMU_REGU_NOGOOD,
};

enum pmu_regu_mode {
    PMU_REGU_NORMAL,
    PMU_REGU_ALWAYSON,
};

enum pmu_regu_e {
    PMU_DCDC1,                // 0 VCC_CORE
    PMU_DCDC_MAX = PMU_DCDC1, // 0
    PMU_LDO1,                 // 1 VCC_RTC
    PMU_LDO2,                 // 2 VCC_IO
    PMU_LDO3,                 // 3 AVCC
    PMU_LDO4,                 // 4 1.2V reserved
    PMU_LDO_MAX = PMU_LDO4,   //4
    PMU_VCC_CORE_DCDC = PMU_DCDC1,
    PMU_VCC_RTC = PMU_LDO1,
    PMU_VCC_IO = PMU_LDO2,
    PMU_AVCC = PMU_LDO3,
    PMU_VCC_CORE_LDO = PMU_LDO4,
};

enum pmu_regu_cmd_e {
    PMU_REGU_ENABLE,
    PMU_REGU_DISABLE,
    PMU_REGU_SET_VOLTAGE,
    PMU_REGU_GET_VOLTAGE,
};

enum pmu_state_cmd_e {
    PMU_ENTER_POFF_MODE,
    PMU_ENTER_LP_MODE,
    PMU_ENTER_CAL_MODE,
    PMU_RESTART,
    PMU_RESTART_NO_WD,
};

enum pmu_time_set_e {
    PMU_POFF_KEEP_TIME,
    PMU_ONOFFUL_TIME,
    PMU_ONOFFL_TIME,
    PMU_ONOFFHL_TIME,
};

enum pmu_protect_cmd_e {
    PMU_PRO_PWROK,
    PMU_PRO_VBUS_OV_SHUT,
    PMU_PRO_VBUS_OV_DET,
    PMU_PRO_VBUS_UV_VOLT,
    PMU_PRO_VBUSID_WK_VOLT,
    PMU_PRO_VBUS_5KPD,
    PMU_PRO_BAT_LB_SHUT,
    PMU_PRO_BAT_SHUT_VOLT,
    PMU_PRO_BAT_OK_VOLT,
    PMU_PRO_BAT_LB_VOLT,
};

enum pmu_wakeupsrc_e {
    PMU_HSWPOS_WAKEUPSRC = 1 << 0,//HSW POS
    PMU_ONOFFL_WAKEUPSRC = 1 << 1,
    PMU_SYSIN_WAKEUPSRC = 1 << 2,
    PMU_ONOFFUL_RESET_EN = 1 << 3,
    PMU_ONOFFUS_WAKEUPSRC = 1 << 4,
    PMU_VBUSIN_WAKEUPSRC = 1 << 5,//VBUS IN
    PMU_ONOFFS_WAKEUPSRC = 1 << 6,
    PMU_RESTART_EN = 1 << 7,
    PMU_HSWON_WAKEUPSRC = 1 << 8,
    PMU_RECHARGE_WAKEUPSRC = 1 << 9,
    PMU_VBUSOUT_WAKEUPSRC = 1 << 10,
    PMU_ONOFFHL_WAKEUPSRC = 1 << 11,
    PMU_VBUSMIDOUT_WAKEUPSRC = 1 << 12,
    PMU_VBUSMIDIN_WAKEUPSRC = 1 << 13,
    PMU_VBUSPPR_WAKEUPSRC = 1 << 14,
};

enum pmu_intsrc_e {
    PMU0_ONOFFS_INTSRC = 1 << 0,
    PMU0_ONOFFL_INTSRC = 1 << 1,
    PMU0_ONOFFUS_INTSRC = 1 << 2,
    PMU0_VBUSIN_INTSRC = 1 << 3,
    PMU0_VBUSOUT_INTSRC = 1 << 4,
    PMU0_LB_INTSRC = 1 << 5,
    PMU0_HSWOFF_INTSRC = 1 << 6,
    PMU0_HSWON_INTSRC = 1 << 7,
    PMU0_VBUSUV_INTSRC = 1 << 8,
    PMU0_BATFULL_INTSRC = 1 << 9,
    PMU0_BATTO_INTSRC = 1 << 10,

    PMU1_BATADC_INTSRC = 1 << 0,
    PMU1_BATFULL_INTSRC = 1 << 1,
    PMU1_VBUSPPR_INTSRC = 1 << 2,
    PMU1_VBUSIN_INTSRC = 1 << 3,
    PMU1_VBUSOUT_INTSRC = 1 << 4,
    PMU1_LB_INTSRC = 1 << 5,
    PMU1_MIDOUT_INTSRC = 1 << 6,
    PMU1_MIDIN_INTSRC = 1 << 7,
    PMU1_KEYDOWN_INTSRC = 1 << 8,
    PMU1_KEYUP_INTSRC = 1 << 9,
    PMU1_ONOFFHL_INTSRC = 1 << 10,
    PMU1_CLICK_INTSRC = 1 << 11,
    PMU1_ONOFFS_INTSRC = 1 << 12,
    PMU1_ONOFFL_INTSRC = 1 << 13,
    PMU1_ONOFFUS_INTSRC = 1 << 14,
    PMU1_RES1_INTSRC = 1 << 15,
    PMU1_VBUSUV_INTSRC = 1 << 16,
    PMU1_BATTO_INTSRC = 1 << 17,
    PMU1_HSWOFF_INTSRC = 1 << 18,
    PMU1_HSWON_INTSRC = 1 << 19,
};

enum inpmu_wakeupintsrc_e {
    INPMU_BT_WAKEUPSRC = 1 << 0,
    INPMU_ADCKEY_WAKEUPSRC = 1 << 1,
    INPMU_SEC_WAKEUPSRC = 1 << 2,
    INPMU_TKC_WAKEUPSRC = 1 << 3,
    INPMU_ALARM_WAKEUPSRC = 1 << 4,
    INPMU_GPIOB_WAKEUPSRC = 1 << 5,
    INPMU_GPIOC_WAKEUPSRC = 1 << 6,
    INPMU_IRRX_WAKEUPSRC = 1 << 7,
    INPMU_TIMER0_WAKEUPSRC = 1 << 8,
    INPMU_TIMER1_WAKEUPSRC = 1 << 9,
    INPMU_TIMER2_WAKEUPSRC = 1 << 10,
    INPMU_TIMER3_WAKEUPSRC = 1 << 11,
    INPMU_RBUS_WAKEUPSRC = 1 << 12,
    INPMU_DKC_WAKEUPSRC = 1 << 13,
    INPMU_TKC_SLD_WAKEUPSRC = 1 << 14,
    INPMU_USB_WAKEUPSRC = 1 << 15,
    INPMU_GPIOD_WAKEUPSRC = 1 << 16,
    INPMU_GPIOA_WAKEUPSRC = 1 << 17,
};

enum pmu_irq_cb_e {
    CHARGE_IN_CB,
    CHARGE_OUT_CB,
    IRQ_NOTIFY_CB,
    IRQ_NOTIFY_MAX,
 };

 enum pmu_event_callback_e {
    PMU_EVENT_POWERKEY_NULL,
    PMU_EVENT_POWERKEY_US_CB = 1 << 0,
    PMU_EVENT_POWERKEY_S_CB = 1 << 1,
    PMU_EVENT_POWERKEY_L_CB = 1 << 2,
    PMU_EVENT_CHARGE_IN_CB = 1 << 3,
    PMU_EVENT_CHARGE_OUT_CB = 1 << 4,
    PMU_EVENT_BAT_LOW_CB = 1 << 5,
    PMU_EVENT_HSW_OFF_CB = 1 << 6,
    PMU_EVENT_HSW_ON_CB = 1 << 7,
    PMU_EVENT_VBUSUV_CB = 1 << 8,
    PMU_EVENT_BAT_FULL_CB = 1 << 9,
    PMU_EVENT_BAT_TO_CB = 1 << 10,
    PMU_EVENT_MID_IN_CB = 1 << 11,
    PMU_EVENT_MID_OUT_CB = 1 << 12,
 };

 enum pmu_vbus_state {
    PMU_VBUS_OUT,
    PMU_VBUS_IN,
    PMU_VBUS_MID,
};

enum pmu_vbus_mid_state {
    PMU_VBUS_MID_OUT,
    PMU_VBUS_MID_IN,
};

enum pmu_mid_out_trig_mode {
    PMU_MID_OUT_EDGE,
    PMU_MID_OUT_LEVEL,
};


typedef struct {
    uint16_t slave_addr;
    uint16_t dcdc_dec_volt;
    uint16_t coreldo_dec_volt;
    bool low_power_enable;
    bool dcdc_dec_volt_enable;
    bool coreldo_en;
    bool coreldo_lp_mode;
    uint8_t irqno;
    enum pmu_regu_e rc_cal_regu;
} PMU_InitTypeDef;

struct pmu_user_config {
    uint16_t vccio_volt;
    uint16_t avcc_volt;
    bool core_use_dcdc;
    bool coreldo_en;
    bool coreldo_lp_mode;
    bool poweroff_current;
    bool vcc_rtc_alwayson;
    bool hsw_sel;
    bool hsw_mode;
    bool nor_sec_erase;
};

struct pmu_battery_info {
    uint16_t vbat;          /* bat voltage */
    bool bat_low;           /* bat low power */
    bool bat_low_shut;      /* bat low power shut */
};

struct pmu_battery_cfg {
    bool charge_en;             /* enable bat charge  */
    bool vbat_adc_en;           /* enable vbat adc  */
    bool charge_time_en;        /* enable bat charge full count time */
    uint8_t mcharge_min_en;        /* enable min current charging after main charge full */
    bool vilp_en;               /* input under voltage cycle enable */
    uint8_t main_charge_volt;  /* main charge constant voltage setting */
    uint8_t main_charge_volt_tuning;
    uint8_t bat_full_volt_tuning;
    uint32_t sub_charge_volt;   /* sub charge constant voltage setting(mV) */
    uint32_t sub_charge_curt;   /* sub charge constant current setting(mA)  */
    uint32_t main_charge_curt;  /* main charge constant current setting(uA) */
    uint32_t avg_lpf_num;       /* average filter num */
    uint32_t r_vil;             /* vout under voltage cycle setting(mV) */
    uint32_t iref_adj_bias;     /* IREF current output bias */
};


void HAL_pmu_pre_init(bool use_dcdc, uint32_t dcdc_volt);
void HAL_pmu_mutex_init(void);
void HAL_pmu_mutex_deinit(void);

uint8_t HAL_pmu_read(uint8_t addr);
uint8_t HAL_pmu_write(uint8_t addr, uint8_t data);
void HAL_pmu_set_poweroff_cur(uint8_t poweroff_current);
void HAL_pmu_set_r_vstop(uint8_t r_vstop);
bool HAL_pmu_get_bat_full_deb(void);
void HAL_pmu_set_regu_tlen(enum pmu_regu_e regu, bool enable);
void HAL_pmu_set_ldo1_mode(enum pmu_regu_mode mode);
void HAL_pmu_set_voltage(enum pmu_regu_e regu, uint32_t val);
uint32_t pmu_get_voltage(enum pmu_regu_e regu);
void HAL_pmu_regu_enable(enum pmu_regu_e regu, bool enable);
void HAL_pmu_set_core_voltage(uint32_t val);
void HAL_pmu_set_bat_cfg(struct pmu_battery_cfg *cfg);
void HAL_pmu_set_bat_current(uint32_t main_charge_curt);
void HAL_pmu_set_time(enum pmu_time_set_e time_type, uint32_t ms);
void HAL_pmu_set_protect(enum pmu_protect_cmd_e cmd, bool enable, uint32_t data);
void HAL_pmu_get_bat_info(struct pmu_battery_info *info);
void HAL_pmu_get_bat_volt(struct pmu_battery_info *info);
void HAL_pmu_set_poweron_wakeupsrc(enum pmu_wakeupsrc_e wakeup_src, bool enable);
void HAL_pmu_vbus_wakeup_hardware_ctrl(bool enable);
void HAL_pmu_poweron_wakeupsrc_init(uint16_t wakeup_src);
void HAL_pmu_set_wakeup(bool enable);
void HAL_pmu_int_init(enum pmu_intsrc_e int_src);
void HAL_pmu_set_int(enum pmu_intsrc_e int_src, bool enable);
bool HAL_pmu_get_int(enum pmu_intsrc_e int_src);
bool HAL_pmu_is_low_bat_direct(void);
bool HAL_pmu_is_use_hsw(void);
bool HAL_pmu_is_use_hsw_otp(void);
void HAL_pmu_sel_onoff_hsw(enum pmu_onoff_hsw_e sel);
void HAL_pmu_hsw_reset_enable(bool enable);
bool HAL_pmu_is_charging(void);
void HAL_pmu_lowpower_set(bool enable);
void HAL_pmu_clear_all_pending(void);
uint32_t HAL_pmu_send_startup_int_event(uint32_t ints);
void HAL_restart_pmu(void);
bool HAL_pmu_is_bat_full(void);
void HAL_pmu_bat_full_sta_set(bool full);

void HAL_pmu_vbat_adc_enable(bool enable);
void HAL_PMU_SetPostVolt(uint16_t volt);

enum pmu_hsw_state HAL_pmu_get_hsw_state(void);
enum pmu_onoff_state HAL_pmu_get_onoff_state(void);
void HAL_pmu_set_bat_charge_cur(uint8_t cur);
void HAL_click_init(void);
bool HAL_pmu_core_use_dcdc(void);
bool HAL_pmu_nor_sec_can_erase(void);
uint8_t HAL_pmu_irq_proc(void);
void HAL_pmu_handler(uint32_t *event);
void HAL_pmu_set_inpmu_wakeup_intsrc(enum inpmu_wakeupintsrc_e  inwakeup_src, bool enable);
void HAL_pmu_set_inpmu_clear_allwakeupsrc_pending(void);
void HAL_pmu_set_state_ctl(enum pmu_state_cmd_e cmd);
void HAL_pmu_poweroff_gpioc_lat(void);

void HAL_pmu_module_init(struct pmu_user_config config);
bool HAL_pmu_is_vbat_pon(void);
bool HAL_pmu_is_vbus_pon(void);
bool HAL_pmu_is_restart_pon(void);
void HAL_pmu_charge_enable(bool enable);
void HAL_pmu_vbus_state_process(bool in);
void HAL_pmu_vbus_timer(void);
void HAL_pmu_vbus_state_init(void);
bool HAL_pmu_vbus_state_get(void);
enum pmu_vbus_state HAL_pmu_get_vbus_state(void);
void HAL_pmu_set_iref(bool charge_mode);
void HAL_pmu_uart_update_flag_set(bool update);
enum pmu_vbus_state HAL_pmu_get_vbus_state(void);
enum pmu_vbus_mid_state HAL_pmu_get_vbus_mid_state(void);
void HAL_pmu_set_mid_out_trig_mode(enum pmu_mid_out_trig_mode mode);
void HAL_pmu_enable_5kpd(bool hw_auto, bool sw_enable);
void HAL_pmu_uart_txrx_enable(bool tx_en, bool rx_en);
void HAL_pmu_uarttx_driver(uint8_t level);
bool HAL_vbus_level(void);

void HAL_pmu_sd_update_flag_set(BOOL update);   //传入0则清除，1则设置
void HAL_pmu_pc_update_flag_set(BOOL update);             //传入0则清除，1则设置
BOOL is_pmu_sd_update_flag_set();
BOOL is_pmu_pc_update_flag_set();
void HAL_pwron_off_reason_parse(void);
bool HAL_pmu_is_lk_pon(void);
void HAL_pmu_hsw_enable(BOOL en);
bool HAL_pmu_stage_charge_set(uint32_t *current, uint32_t cutoff_current);

#ifdef __cplusplus
}
#endif

#endif


