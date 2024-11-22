#define LOG_LEV 4//4

#include "sys_inc.h"
#include "user_config.h"
#include "hal_gpio.h"

#include "hal_dio.h"
#include "hal_pmu.h"
#include "event.h"
#include "hal_clk.h"
#include "bt_host_api.h"
#include "karaoke_framework.h"
#include "app_pmu.h"
#include "pmu_observer.h"
#include "disp.h"
#include "bt_api.h"
#include "bt.h"
#include "tws_profile.h"
#include "psm_manager.h"
#include "info_mem.h"
#include "app_gpio_irq.h"
#include "app_audio_anc.h"
#include "anc_service.h"
#include "air_api.h"
#include "ui.h"
#include "iodm_test.h"
#include "sensor.h"
#include "LTK1181.h"
#include "info_mem.h"
#include "hal.h"
#include "app_lcd_font.h"
#include "lcd_display.h"

//general variable
bool delay_poweroff_flag;
#if BAT_EXIST
uint32_t discon_time = 0xffffffff;
#endif

VBusCb* _VbusCb = NULL;

OAL_THREAD pmu_isr_bh_handler;

//vbus code variable
static bool bootup_vbus;

/* ----------------------------------------------------------------------
 * 函数功能:
 * 函数名称:
 * 参数: 无
 * 返回值:
 *-----------------------------------------------------------------------*/
void pmu_register_vbus_out_cb(VBusCb* vbusCb)
{
    _VbusCb = vbusCb;
}

#if VBUS_OUT_AUTO_POWERON
volatile static uint64_t ts_out;

void pmu_vbus_out_ts_set(bool in)
{
    if (in) {
        ts_out = UINT64_MAX;
    } else {
        ts_out = HAL_get_timestamp();
    }
}
#else
void pmu_vbus_out_ts_set(bool in)
{
}
#endif

OAL_TIMER DelayPoweroffTimer;
OAL_TIMER SeamlessTimer;
OAL_TIMER seamless_fail_timer;
#if SEAMLESS_EN
static uint8_t seamless_reason;
#endif

//bat variable
/*
 * coulomp: battery quantity
 * voltage: battery voltage
 */
#if BAT_EXIST
const uint16_t coulomp_voltage_table[CV_TABLE_LEN] = {
    BAT_LEVEL_0PERCENT_VOLT,
    BAT_LEVEL_10PERCENT_VOLT,
    BAT_LEVEL_20PERCENT_VOLT,
    BAT_LEVEL_30PERCENT_VOLT,
    BAT_LEVEL_40PERCENT_VOLT,
    BAT_LEVEL_50PERCENT_VOLT,
    BAT_LEVEL_60PERCENT_VOLT,
    BAT_LEVEL_70PERCENT_VOLT,
    BAT_LEVEL_80PERCENT_VOLT,
    BAT_LEVEL_90PERCENT_VOLT,
    BAT_LEVEL_100PERCENT_VOLT,
};

struct pmu_battery_info bat_info;
bool bat_quantity_inited = 0;
uint32_t bat_quantity_average;
uint32_t bat_quantity_total = 0;
uint8_t bat_quantity_percent = 0;
uint8_t bat_quantity_percent_old = 0;
uint32_t bat_quantity_average_old = 0;
uint8_t bat_quantity_record_cnt = 0;
uint16_t bat_low_warning_cnt = BAT_SW_LB_WARNING_PERIOD/BAT_RECORD_PERIOD - 1;
uint32_t charge_time;
uint32_t last_percent;
uint32_t update_time;

#if !RTC_EN
OAL_TIMER PmuTimerHandle;
#endif

#endif

#if STAGE_CHARGE_EN
OAL_TIMER BatfullTimer;
#endif

#if (G24_2T4_SHORT_PACKET_3TIMES || G24_2T4_WITH_MONO_MONITOR) && G24_2T4_DONGLE_MCHIP
uint8_t pmu_powerkey_us_flag = 0;           //按下powerkey VBAT导通时避免误触发事件
#else
uint8_t pmu_powerkey_us_flag = 1;
#endif

AT_SRAM(.pmu)
void pmu_event_parse(uint32_t event)
{
    if (event & PMU_EVENT_POWERKEY_US_CB) {
        event_put(PMU_EVENT_POWERKEY_US, NULL, 0);
        pmu_key_set(PMU_EVENT_POWERKEY_US);
        pmu_powerkey_us_flag = 1;
    }

    if (event & PMU_EVENT_POWERKEY_S_CB) {
        if(pmu_powerkey_us_flag) {
            event_put(PMU_EVENT_POWERKEY_SU, NULL, 0);
            pmu_key_set(PMU_EVENT_POWERKEY_SU);
        }
        
    }

    if (event & PMU_EVENT_POWERKEY_L_CB) {
        if(pmu_powerkey_us_flag) {
            event_put(PMU_EVENT_POWERKEY_L, NULL, 0);
            pmu_key_set(PMU_EVENT_POWERKEY_L);
        }
    }

    if (event & PMU_EVENT_CHARGE_IN_CB) {
        pmu_vbus_out_ts_set(1);
#if CHARGING_BIN_CODE_UART
        pmu_vbus_print_set(0);
#endif
        event_clear(PMU_EVENT_CHARGE_IN);
        event_put(PMU_EVENT_CHARGE_IN, NULL, 0);
    }

    if (event & PMU_EVENT_CHARGE_OUT_CB) {
        pmu_vbus_out_ts_set(0);
        event_clear(PMU_EVENT_CHARGE_OUT);
        event_put(PMU_EVENT_CHARGE_OUT, NULL, 0);
    }

    if (event & PMU_EVENT_BAT_LOW_CB) {
        event_put(PMU_EVENT_BAT_LOW, NULL, 0);
    }

    if (event & PMU_EVENT_HSW_OFF_CB) {
        event_put(PMU_EVENT_HSW_OFF, NULL, 0);
    }

    if (event & PMU_EVENT_HSW_ON_CB) {
        event_put(PMU_EVENT_HSW_ON, NULL, 0);
    }

    if (event & PMU_EVENT_VBUSUV_CB) {
        event_put(PMU_EVENT_VBUSUV, NULL, 0);
    }

    if (event & PMU_EVENT_BAT_FULL_CB) {
        event_put(PMU_EVENT_BAT_FULL, NULL, 0);
    }

    if (event & PMU_EVENT_BAT_TO_CB) {
        event_put(PMU_EVENT_BAT_TO, NULL, 0);
    }

    if (event & PMU_EVENT_MID_IN_CB) {
        event_clear(PMU_EVENT_VBUSMIDIN);
        event_put(PMU_EVENT_VBUSMIDIN, NULL, 0);
    }

    if (event & PMU_EVENT_MID_OUT_CB) {
        event_clear(PMU_EVENT_VBUSMIDOUT);
        event_put(PMU_EVENT_VBUSMIDOUT, NULL, 0);
    }
}

uint32_t app_pmu_cmd_cb(app_pmu_cmd cmd_id, uint32_t val)
{
    uint32_t ret = 0;
    switch (cmd_id) {
       case APP_PMU_CMD_TASK_DEINIT:
            pmu_task_deinit();
            break;
        case APP_PMU_CMD_VBUS_IS_IN:
            ret = pmu_vbus_is_in();
            break;
        case APP_PMU_CMD_BAT_LEVEL_GET:
#if BAT_EXIST
            ret = bat_quantity_percent;
#else
            ret = 0;
#endif
        default:
            break;
    }
    return ret;
}

void pmu_early_init(void)
{
    vf_table_t table[] = {
        VF_TABLE_0,
        VF_TABLE_1,
        VF_TABLE_2,
        VF_TABLE_3,
        VF_TABLE_4,
        VF_TABLE_5,
    };

    HAL_clk_set_vf_table(table);
    HAL_pmu_mutex_init();

#if CHARGING_BIN_MID_LEVEL
    HAL_pmu_enable_5kpd(false, true);
#else
    HAL_pmu_enable_5kpd(false, false);
#endif

#if CHARGING_BIN_CODE_UART
    HAL_gpio_func_set(PIN_B24, PIN_FUNCDISABLE); //disable
    pmu_uart_txrx_enable(0, 1);
    HAL_pmu_uarttx_driver(3);
    uart_isr_bh_init();
    HAL_uart_register_rx_callback(UART_ID_01, uart1_rx_data);
    logi("uart ready:%u", HAL_timestamp_to_ms(HAL_get_timestamp()));
#endif
}

void pmu_late_init(void)
{
    uint32_t int_bits;
    uint16_t wakesrc_bits;
    struct pmu_user_config config;

    //如果bt MAC地址已经烧写，证明已经进入产品阶段，而不是PCBA阶段,
    //PCBA不需要充电仓控制，而需要直接上电
#if BT_TWS_MP_MODE_EN
    bool dio0_value, flash_value;

    HAL_dio_func_set(DIO_ID0, DIO_FUNC_INPUT);
    HAL_dio_func_set(DIO_ID0, DIO_PULL_UP_100K);

    dio0_value = HAL_dio_value_get(DIO_ID0);
    HAL_dio_func_set(DIO_ID0, DIO_FUNC_DISABLE);
    HAL_dio_pull_set(DIO_ID0, DIO_PULL_DISABLE);

    flash_value = is_nor_bootup_flag_set();
    BOOL mp_enter_dut = is_mp_enter_udt_mode();
    extern void set_ke_mp_mode_en(BOOL mp_mode_en);
    set_ke_mp_mode_en(!(flash_value || mp_enter_dut));
    if ((dio0_value == flash_value) && (mp_enter_dut == 0) ) {
        bootup_vbus = 0;//code
    } else {
        bootup_vbus = 1;//vbus
    }
    logi("dio0:%u, flash:%u, use %s", dio0_value, flash_value, bootup_vbus?"vbus":"code");
#else
    bootup_vbus = 0;
#endif
    config.core_use_dcdc = PMU_CORE_USE_DCDC;
    config.coreldo_en = PMU_CORELDO_EN;
    config.coreldo_lp_mode = PMU_CORELDO_LP_MODE;
    config.vcc_rtc_alwayson = PMU_VCC_RTC_ALWAYS_ON;
    config.vccio_volt = PMU_VCC_IO_VOLT;
    config.avcc_volt = PMU_AVCC_VOLT;
#if BAT_EXIST
    config.poweroff_current = PMU_POWEROFF_CURRENT;
#else
    config.poweroff_current = 2;
#endif
    config.hsw_sel = PMU_HSW_SEL;
    config.hsw_mode = PMU_HSW_MODE;
    config.nor_sec_erase = PMU_NOR_SEC_ERASE_PRO_EN;
    logi("use dcdc:%u", config.core_use_dcdc);
    HAL_pmu_module_init(config);

#if !STARTUP_TUNING
    if (HAL_pmu_is_use_hsw()) {
        HAL_pmu_hsw_reset_enable(true);
    }
#endif

    wakesrc_bits = 0;
#if PMU_SYSIN_POWERON_EN
    wakesrc_bits |= PMU_SYSIN_WAKEUPSRC;
#endif
#if PMU_VBUSIN_POWERON_EN
    wakesrc_bits |= PMU_VBUSIN_WAKEUPSRC;
#endif
#if PMU_VBUSOUT_POWERON_EN
    HAL_pmu_set_mid_out_trig_mode(PMU_MID_OUT_EDGE);
    wakesrc_bits |= PMU_VBUSOUT_WAKEUPSRC;
#endif
#if PMU_VBUSPPR_POWERON_EN
    wakesrc_bits |= PMU_VBUSPPR_WAKEUPSRC;
#endif
#if PMU_VBUSMID_IN_POWERON_EN
    wakesrc_bits |= PMU_VBUSMIDIN_WAKEUPSRC;
#endif
#if PMU_VBUSMID_OUT_POWERON_EN
    wakesrc_bits |= PMU_VBUSMIDOUT_WAKEUPSRC;
#endif
#if PMU_ULTRA_SHORT_PRESS_POWERON_EN
    wakesrc_bits |= PMU_ONOFFUS_WAKEUPSRC;
#endif
#if PMU_SHORT_PRESS_POWERON_EN
    wakesrc_bits |= PMU_ONOFFS_WAKEUPSRC;
#endif
#if PMU_LONG_PRESS_POWERON_EN
    wakesrc_bits |= PMU_ONOFFL_WAKEUPSRC;
#endif
#if PMU_HYPERLONG_PRESS_POWERON_EN
    wakesrc_bits |= PMU_ONOFFHL_WAKEUPSRC;
#endif
#if PMU_ULTRA_LONG_PRESS_POWERON_EN
    wakesrc_bits |= PMU_ONOFFUL_RESET_EN;
#endif
#if PMU_HSWON_POWERON_EN
    wakesrc_bits |= PMU_HSWON_WAKEUPSRC;
#endif
#if PMU_HSWPOS_POWERON_EN
    wakesrc_bits |= PMU_HSWPOS_WAKEUPSRC;
#endif
#if PMU_RECHARGE_POWERON_EN
    wakesrc_bits |= PMU_RECHARGE_WAKEUPSRC;
#endif
    HAL_pmu_poweron_wakeupsrc_init(wakesrc_bits);

#if PMU_RECHARGE_POST_VOLT != 4000
    HAL_PMU_SetPostVolt(PMU_RECHARGE_POST_VOLT);
#endif

#if 0
    HAL_click_init();
#endif

#if CHARGING_BIN_MID_LEVEL
    int_bits = PMU1_VBUSIN_INTSRC | PMU1_VBUSOUT_INTSRC | PMU1_MIDIN_INTSRC | PMU1_MIDOUT_INTSRC;
#else
    int_bits = PMU1_VBUSIN_INTSRC | PMU1_VBUSOUT_INTSRC;
#endif

#if BAT_EXIST
    int_bits |= PMU1_BATFULL_INTSRC | PMU1_BATTO_INTSRC;
#endif
    if (HAL_pmu_is_use_hsw()) {
        int_bits |= PMU1_HSWOFF_INTSRC | PMU1_HSWON_INTSRC;
    } else {
        int_bits |= PMU1_ONOFFUS_INTSRC | PMU1_ONOFFS_INTSRC | PMU1_ONOFFL_INTSRC | PMU1_ONOFFHL_INTSRC | PMU1_CLICK_INTSRC;
    }
    HAL_pmu_int_init(int_bits);

//#if !STARTUP_TUNING
    if (!HAL_pmu_is_use_hsw()) {
        HAL_pmu_set_time(PMU_ONOFFL_TIME, PMU_LONG_PRESS_TIME);
        HAL_pmu_set_time(PMU_ONOFFHL_TIME, PMU_HYPERLONG_PRESS_TIME);
        HAL_pmu_set_time(PMU_ONOFFUL_TIME, PMU_ULTRA_LONG_PRESS_TIME);
    }
    HAL_pmu_set_time(PMU_POFF_KEEP_TIME, 0);
//#endif

#if PMU_POWEROFF_WHEN_BAT_FIRST_IN
    if (!bootup_vbus && HAL_pmu_is_vbat_pon()) {
        logi("poweroff after vbat in poweron");
        //pmu_set_poweron_wakeupsrc(PMU_VBUS_WAKEUPSRC, false);
        if (HAL_pmu_is_use_hsw_otp()) {
            HAL_pmu_hsw_reset_enable(false);
            HAL_pmu_sel_onoff_hsw(PMU_HSW);
            while (HAL_pmu_get_hsw_state() == PMU_HSW_STATE_OFF) {
                ;
            }
        }
        HAL_set_rc_cal_paras(RC_CAL_PRD_BASE, RC_CAL_PRD, RC_CAL_NUM, RC_CAL_CON_MODE, RC_POWEROFF_CAL_EN);
        HAL_pmu_vbat_adc_enable(true);
        pmu_info_bat_level_set(0xff);
        enter_poweroff_mode();
    }
#else
    if (!bootup_vbus && HAL_pmu_is_vbat_pon()) {
        pmu_info_bat_level_set(0xff);
    }
#endif

#if PMU_NOR_SEC_ERASE_PRO_EN
    HAL_pmu_set_protect(PMU_PRO_BAT_LB_VOLT, false, PMU_NOR_SEC_ERASE_PRO_VOLT);
#endif
    pmu_bat_init();
#if SLEEP_EN
    HAL_rtc_prcm_init();
#endif

#if !STARTUP_TUNING ////FIXME:WXP 正常运行时仍然打开pmu lp mode会导致dcdc纹波大，需要后面打开看波形，确认没有问题再打开
    HAL_pmu_lowpower_set(false);
#endif

    HAL_pwron_off_reason_parse();

    app_pmu_resiger_callback(app_pmu_cmd_cb);
}

void pmu_reset_time_set(uint32_t ms)
{
    HAL_pmu_set_time(PMU_POFF_KEEP_TIME, ms);
}

void pmu_bat_init(void)
{
#if BAT_EXIST
    struct pmu_battery_cfg bat_cfg;

    bat_cfg.sub_charge_volt = 3300;
    bat_cfg.sub_charge_curt = PMU_SUBCHARGE_CURRENT;//电池容量的十分之一
    bat_cfg.main_charge_volt = PMU_CHARGE_VOLT;
    bat_cfg.main_charge_volt_tuning = PMU_CHARGE_VOLT_TUNING;
    bat_cfg.bat_full_volt_tuning = PMU_BAT_FULL_VOLT_TUNING;
    bat_cfg.main_charge_curt = PMU_CHARGE_CURRENT;
    bat_cfg.charge_time_en = PMU_SET_EN;
    bat_cfg.vbat_adc_en = PMU_SET_EN;
    bat_cfg.charge_en = PMU_SET_KEEP;
    bat_cfg.mcharge_min_en = PMU_CHARGE_MIN_EN;
    bat_cfg.avg_lpf_num = 16; //16/32/64/128
    bat_cfg.vilp_en = PMU_SET_EN;
    bat_cfg.r_vil = 4600;
#if BAT_VOLT_DET
    bat_cfg.iref_adj_bias = 7;
#else
    bat_cfg.iref_adj_bias = 0;
#endif

    HAL_pmu_set_bat_cfg(&bat_cfg);

#if (BAT_HW_LB_SHUT_VOLT != 2700)
    HAL_pmu_set_protect(PMU_PRO_BAT_SHUT_VOLT, true, BAT_HW_LB_SHUT_VOLT);
#endif
#if (BAT_HW_OK_VOLT != 3100)
    HAL_pmu_set_protect(PMU_PRO_BAT_OK_VOLT, true, BAT_HW_OK_VOLT);
#endif
#if BAT_HW_LB_SHUT_EN
    //HAL_pmu_set_protect(PMU_PRO_BAT_LB_SHUT, true, 0);//默认是enable
#else
    HAL_pmu_set_protect(PMU_PRO_BAT_LB_SHUT, false, 0);
#endif

    HAL_mdelay(20);
    HAL_pmu_get_bat_info(&bat_info);

#if (PMU_EXIST == 0)
    bat_info.vbat = 4200;
#endif

    //bat_quantity_average = bat_info.vbat;
    //bat_quantity_percent = pmu_voltage_to_percent(bat_info.vbat);
    //bat_quantity_percent_old = bat_quantity_percent;
    //logd("percent:%u%%", bat_quantity_percent);
    //logd("vbat:%u", bat_info.vbat);
    pmu_bat_quantity_init();
#endif
}

void pmu_bat_deinit(void)
{
#if BAT_EXIST
    if (mode_switch_reason_get() == SW_LB) {
        logi("lb set bat level");
        pmu_info_bat_level_set(0);
    } else {
        pmu_info_bat_level_set(pmu_get_bat_quantity_percent());
    }
#endif
}

void pmu_task_init(void)
{

#if BAT_EXIST
    //pmu_bat_quantity_init();
#if !RTC_EN
    //creat timer for battery management
    if (PmuTimerHandle == 0) {
        OAL_timer_create(&PmuTimerHandle, "PmuTimer", OAL_ms_to_ticks(BAT_RECORD_PERIOD),
            pmu_timer_cb, (void *)1, 1, OAL_TIMER_AUTO_ACTIVATE);
    }
#endif
#endif

    if (DelayPoweroffTimer == 0) {
        OAL_timer_create(&DelayPoweroffTimer, "DPTimer", OAL_ms_to_ticks(2000),
            delaypoweroff_timer_cb, (void *)1, 0, OAL_TIMER_NO_ACTIVATE);
    }
#if SEAMLESS_EN
#if BT_SNIFF_EN
    if (SeamlessTimer == 0) {
        OAL_timer_create(&SeamlessTimer, "SeamlessTimer", OAL_ms_to_ticks(5000),
            seamless_timer_cb, (void *)1, 0, OAL_TIMER_NO_ACTIVATE);//主耳用
    }

    if (seamless_fail_timer == NULL) {
        OAL_timer_create(&seamless_fail_timer, "SeamlessFail", OAL_ms_to_ticks(5100),
            seamless_fail_cb, (void *)1, 0, OAL_TIMER_NO_ACTIVATE);//副耳用
    }
#else
    if (SeamlessTimer == 0) {
        OAL_timer_create(&SeamlessTimer, "SeamlessTimer", OAL_ms_to_ticks(2000),
            seamless_timer_cb, (void *)1, 0, OAL_TIMER_NO_ACTIVATE);//主耳用
    }

    if (seamless_fail_timer == NULL) {
        OAL_timer_create(&seamless_fail_timer, "SeamlessFail", OAL_ms_to_ticks(2100),
            seamless_fail_cb, (void *)1, 0, OAL_TIMER_NO_ACTIVATE); //副耳用
    }
#endif
#endif

#if STAGE_CHARGE_EN
    if (BatfullTimer == 0) {
        OAL_timer_create(&BatfullTimer, "BatfullTimer", OAL_ms_to_ticks(500),
            batfull_timer_cb, (void *)1, 0, OAL_TIMER_NO_ACTIVATE);
    }
#endif

    HAL_pmu_vbus_state_init();
}

//it use in update code,so it must be put in com_sys section
void pmu_task_deinit(void)
{
#if BAT_EXIST
#if !RTC_EN
    if (PmuTimerHandle != NULL) {
        //xTimerStop(PmuTimerHandle, 10);
        OAL_timer_activate(&PmuTimerHandle, OAL_TIMER_NO_ACTIVATE);
    }

#endif
#endif

    HAL_pmu_mutex_deinit();
}

void pmu_deinit(void)
{
    pmu_bat_deinit();
    pmu_task_deinit();
}

void pmu_isr_bh_init(void)
{
    uint32_t event;
    
#if PMU_HSW_SEL    
    HAL_pmu_hsw_enable(PMU_HSW_SEL);    
#endif

    pmu_isr_bh_handler = OAL_thread_create(pmu_isr_bh, NULL, "pmu_isr_bh", PMU_ISR_BH_TASK_PRIO, PMU_ISR_BH_TASK_STACK_SIZE);
    event = HAL_pmu_send_startup_int_event(PMU_INTS_VBUSIN|PMU_INTS_VBUSOUT);
    pmu_event_parse(event);
}

AT_SRAM(.pmu)
void pmu_irq_proc(void)
{
    uint8_t irq_cb_num;

    irq_cb_num = HAL_pmu_irq_proc();
    if (irq_cb_num == IRQ_NOTIFY_CB) {
        if (pmu_isr_bh_handler != NULL) {
            OAL_thread_notify(pmu_isr_bh_handler, 0, 0);
        }
    }
}

AT_SRAM(.pmu)
void pmu_handler(void)
{
    uint32_t event;
    HAL_pmu_handler(&event);
    pmu_event_parse(event);
}

void* pmu_isr_bh(void *pvParameters)
{
    while (1) {
        OAL_thread_notify_wait(0x0, 0xffffffff, NULL, OAL_WAIT_FOREVER);
        pmu_handler();
         if(OAL_SUCCESS == OAL_thread_wait_join(pmu_isr_bh_handler, 0))
            break;
    }
    return NULL;
}

uint8_t pmu_info_bat_level_get(void)
{
    int ret;
    uint8_t bat_level;

    ret = sys_info_read(BAT_LEVEL, &bat_level, 1);
    if (ret < 0) {
        loge("can't find bat level in psm:%d-%u", ret, bat_level);
        bat_level = 0xff;
    }
    logi("get bat level:%u", bat_level);

    return bat_level;
}

void pmu_info_bat_level_set(uint8_t bat_level)
{
    uint8_t level = bat_level;
    __maybe_unused int ret;

    ret = sys_info_write(BAT_LEVEL, &level, 1);
    logi("set bat level:%d-%u", ret, level);
}

#if BAT_EXIST
void pmu_bat_quantity_init(void)
{
    uint8_t percent;
    uint8_t percent_vbat;
    uint8_t percent_diff;

    percent = pmu_info_bat_level_get();
    //invalid psm info, poweroff mode will save the lastest bat level,
    //if restart by watchdog or restart cmd will query bat voltage
    pmu_info_bat_level_set(0xff);

    if (percent == 0xff) {
        HAL_pmu_get_bat_info(&bat_info);
        bat_quantity_average = bat_info.vbat;
    } else {
        if (percent > 100) {
            percent = 100;
        }

        HAL_pmu_get_bat_info(&bat_info);
        percent_vbat = pmu_voltage_to_percent(bat_info.vbat);
        if(percent_vbat > percent) {
            percent_diff = percent_vbat - percent;
        } else {
            percent_diff = percent - percent_vbat;
        }

        if(percent_diff <= 10) {     //使用保存的电量
            bat_info.vbat = pmu_percent_to_voltage(percent);
        }
        
        bat_quantity_average = bat_info.vbat;
    }
    bat_quantity_average_old = bat_quantity_average;
    bat_quantity_total = bat_quantity_average << 3;//8次
    logi("bat_quantity_average:%u", bat_quantity_average);
    bat_quantity_percent = pmu_voltage_to_percent(bat_quantity_average);
    bat_quantity_percent_old = bat_quantity_percent;
    logi("statup:Vbat:%u, Vavg:%u, Per:%u", bat_info.vbat, bat_quantity_average, bat_quantity_percent);
    bat_info.bat_low = false;
    bat_info.bat_low_shut = false;
    bat_low_warning_cnt = BAT_SW_LB_WARNING_PERIOD/BAT_RECORD_PERIOD - 1;
    bat_quantity_inited = 1;
    //pmu_bat_quantity_process();
}

uint8_t pmu_voltage_to_percent(uint32_t volt)
{
    uint32_t i, percent;
    uint32_t step, base;

    // 遍历电压表，找到第一个大于等于当前电压的索引
    for (i = 0; i < CV_TABLE_LEN; i++) {
        if (volt <= coulomp_voltage_table[i])
            break;
    }

    // 处理电压小于第一个阈值的情况
    if (i == 0) {
        if (volt >= BAT_SW_LB_SHUT_VOLT) {
            // 计算当前电压与最低关机电压之间的步长和基准电压
            step = (coulomp_voltage_table[i] - BAT_SW_LB_SHUT_VOLT) / 10;
            base = BAT_SW_LB_SHUT_VOLT;
        } else { // 如果电压小于3000，百分比为0%
            step = 1;
            base = volt;
        }
    } 
    // 处理电压在第一个和第九个阈值之间的情况
    else if ((i >= 1) && (i <= 9)) {
        step = (coulomp_voltage_table[i] - coulomp_voltage_table[i - 1]) / 10;
        base = coulomp_voltage_table[i - 1];
    } 
    // 处理电压大于最后一个阈值的情况
    else {
        step = 1;
        base = volt;
    }

    // 确保步长不为0
    if (step == 0) {
        step = 1;
        base = volt;
    }

    // 计算百分比
    percent = (BAT_QUANTITY_STEP_PERCENT * i) + (volt - base) / step;
    // 限制百分比不超过100
    if (percent > 100) {
        percent = 100;
    }

#if LCD_SEG_EN
    // 根据计算出的百分比显示电量
    if (percent == 0) {
        displayUpdate(Battery_Empty, true); // 显示无电量
    } else if (percent <= 33) {
        displayUpdate(Battery_OneBar, true); // 显示一格电量
    } else if (percent <= 66) {
        displayUpdate(Battery_TwoBars, true); // 显示二格电量
    } else {
        displayUpdate(Battery_ThreeBars, true); // 显示三格电量
    }
#elif LCDC_EN
    // // 根据计算出的百分比显示电量
    // if (percent == 0) {
    //     displayUpdate(Battery_Empty, true); // 显示无电量
    // } else if (percent <= 33) {
    //     displayUpdate(Battery_OneBar, true); // 显示一格电量
    // } else if (percent <= 66) {
    //     displayUpdate(Battery_TwoBars, true); // 显示二格电量
    // } else {
    //     displayUpdate(Battery_ThreeBars, true); // 显示三格电量
    // }
#endif
    // 记录调试信息
    // logv("volt:%u, i:%u, step:%u, base:%u, per:%u", volt, i, step, base, percent);
    // logi("volt:%u, i:%u, step:%u, base:%u, per:%u", volt, i, step, base, percent);
    //oops("volt:%u, i:%u, step:%u, base:%u, per:%u", volt, i, step, base, percent);

    // 返回电池电量百分比
    return (uint8_t)percent;
}


uint32_t pmu_percent_to_voltage(uint32_t percent)
{
    uint32_t i, j, voltage;

    i = percent / 10;
    j = percent % 10;

    if (percent == 0) {
        voltage = BAT_SW_LB_SHUT_VOLT;
    } else if ((percent > 0) && (percent <= 9)) {
        voltage = (coulomp_voltage_table[0] - BAT_SW_LB_SHUT_VOLT) / 10 * j + BAT_SW_LB_SHUT_VOLT;
    } else if ((percent >= 10) && (percent <= 99)) {
        voltage = (coulomp_voltage_table[i] - coulomp_voltage_table[i - 1]) / 10 * j + coulomp_voltage_table[i - 1];
    } else {
        voltage = 4250;
    }

    // logi("per:%u, i:%u, j:%u, volt:%u", percent, i, j, voltage);
    //oops("per:%u, i:%u, j:%u, volt:%u", percent, i, j, voltage);

    return voltage;
}


uint8_t pmu_get_bat_quantity_percent(void)
{
#if BAT_EXIST
    if (bat_quantity_inited) {
        return bat_quantity_percent;
    } else {
        HAL_pmu_get_bat_info(&bat_info);
        return pmu_voltage_to_percent(bat_info.vbat);;
    }
#else
    return 0;
#endif
}


volatile uint32_t cnt_pre = 0;
uint32_t shutdown_pre = 0;

#if STAGE_CHARGE_EN

/*
bool HAL_pmu_stage_charge_set(uint32_t *current, uint32_t cutoff_current)
{
	if (*current != cutoff_current) {
    	HAL_pmu_set_r_vstop(0);
		if (*current > cutoff_current) {
	    	*current -= 8000;
	    	HAL_pmu_set_bat_current(*current);
		} else {
			*current = cutoff_current;
	    	HAL_pmu_set_bat_current(*current);
		}
		logi("stage charge current:%d",*current);
		return true;
	} else {
		return false;
	}
}
*/

uint32_t stage_charge_curt = PMU_CHARGE_CURRENT;
uint32_t cutoff_current = CUTOFF_CURRENT;

void batfull_timer_cb(void* xTimer)
{
    if(HAL_pmu_get_bat_full_deb()) {
        logi("batfull_timer,stage_charge:%d",stage_charge_curt);
        if (HAL_pmu_stage_charge_set(&stage_charge_curt,cutoff_current)) {
            OAL_timer_activate(&BatfullTimer, OAL_TIMER_AUTO_ACTIVATE);
        } else {
            HAL_pmu_set_bat_current(PMU_CHARGE_CURRENT);
        	stage_charge_curt = PMU_CHARGE_CURRENT;
	        if (mode_sta_get() == MODE_CHARGE) {
    	        logi("stage charge batfull");
                HAL_pmu_charge_enable(false);
#if CHARGING_BIN && (CHARGING_BIN_MID_LEVEL || CHARGING_BIN_CODE)

#else
                HAL_pmu_set_poweron_wakeupsrc(PMU_VBUSIN_WAKEUPSRC, false);
                //HAL_pmu_vbus_wakeup_hardware_ctrl(DISABLE);         //关闭关机后硬件自动恢复默认的VBUSIN wakeup
                mode_switch_reason_set(SW_CHARGE);
                mode_set_force(MODE_POWEROFF);
#endif
            }
       }
   }
}

void stage_charge_curt_set(uint32_t cur)
{
    stage_charge_curt = cur;
}

#endif

void pmu_timer_cb(void* xTimer)
{
#if (G24_2T4_SHORT_PACKET_3TIMES || G24_2T4_WITH_MONO_MONITOR) && G24_FUNC_ROLE			//副芯片不处理电量计算
    if(!G24_2T4_DONGLE_MCHIP) {
        return;
    }
#endif

    bool bat_update = 0;
    RTC_TimeTypeDef rtc;
    uint32_t current_time;

#if (PMU_EXIST == 0)
    bat_info.vbat = 4200;
#else
    HAL_pmu_get_bat_info(&bat_info);
#endif

    if (charge_mode_sta_get()) {
        bat_quantity_average = bat_info.vbat;
        bat_quantity_total = bat_quantity_average << 3;//8次
        //pmu_bat_quantity_process();
    } else {
        //plugin_vbat_filter(&vbat);
        //默认的取平均值算法.
        bat_quantity_total = bat_quantity_total - bat_quantity_average + bat_info.vbat; //均值
        bat_quantity_average = bat_quantity_total >> 3; //8次
    }

    if (pmu_voltage_to_percent(bat_quantity_average_old) != pmu_voltage_to_percent(bat_quantity_average)) {
        //update bt bat level
        if (bat_quantity_average > bat_quantity_average_old) {
            if ((charge_mode_sta_get() || HAL_pmu_is_charging()) && \
                ((bat_quantity_average - bat_quantity_average_old) > BAT_UPDATE_THRESHOLD)) {
                bat_update = 1;
        }
        } else {
            if (!charge_mode_sta_get() && \
                (bat_quantity_average_old - bat_quantity_average) > BAT_UPDATE_THRESHOLD) {
                bat_update = 1;
            }
        }
    }

    if (bat_update) {
        uint32_t time;
        uint32_t old_percent, cur_percent, time_diff_percent, volt_diff_percent, diff_percent;
        bool percent_inc;

        rtc_time_wait_update();
        HAL_rtc_get_time(&rtc);
        current_time = rtc.hour * 3600 + rtc.minute * 60 + rtc.second;

        if (current_time >= update_time) {
            time = current_time - update_time;
        } else {
            time = 3600 * 24 + current_time - update_time;
        }
        old_percent = pmu_voltage_to_percent(bat_quantity_average_old);
        cur_percent = pmu_voltage_to_percent(bat_quantity_average);
        if (cur_percent > old_percent) {
            volt_diff_percent = cur_percent - old_percent;
            percent_inc = 1;
        } else {
            volt_diff_percent = old_percent - cur_percent;
            percent_inc = 0;
        }
        if ((charge_mode_sta_get() || HAL_pmu_is_charging()) && \
                (bat_quantity_average > bat_quantity_average_old)) {
            //time_diff_percent = (time * PMU_CHARGE_CURRENT) / (36 * BAT_CAPACITY);
            //time_diff_percent = (time * PMU_CHARGE_CURRENT * 5) / (36 * BAT_CAPACITY * 4); //FIXME:WXP
            time_diff_percent = (time * PMU_CHARGE_CURRENT * 3) / (36 * BAT_CAPACITY * 2); //FIXME:WXP
        } else {
            time_diff_percent = (time * 60000) / (36 * BAT_CAPACITY);
        }

        if (volt_diff_percent > time_diff_percent) {
            diff_percent = time_diff_percent;
        } else {
            diff_percent = volt_diff_percent;
        }
        if (percent_inc) {
            bat_quantity_percent = bat_quantity_percent_old + diff_percent;
        } else {
            bat_quantity_percent = bat_quantity_percent_old - diff_percent;
        }
        // logi("\n->Vold:%u, Vcur:%u, Vdiff:%u, inc:%u, Tdiff:%u, Ctime:%u, Otime:%u, Time:%u, Per:%u",
        //     old_percent, cur_percent, volt_diff_percent, percent_inc, time_diff_percent, current_time, update_time, time, bat_quantity_percent);

        if (bat_quantity_percent != bat_quantity_percent_old) {
            update_time = current_time;
#if MODE_BT_EN
            event_put(BT_EVENT_BAT_UP, NULL, 0);
#endif
        }

        bat_quantity_percent_old = bat_quantity_percent;
        bat_quantity_average_old = pmu_percent_to_voltage(bat_quantity_percent);
    }
    // logi("\nvbat:%u-%u-%u-%u%%, low:%u", bat_info.vbat, bat_quantity_average, bat_quantity_average_old, bat_quantity_percent, bat_info.bat_low);
    //bat_quantity_percent = (bat_quantity_average - BAT_SW_LB_SHUT_VOLT) / ((4350 - BAT_SW_LB_SHUT_VOLT) / 100);

    if (!bootup_vbus && !HAL_pmu_is_charging() && (mode_get_sta() != MODE_CHARGE) && !MODE_KEY_ONOFF && \
        (bat_info.bat_low == true) && (bat_info.bat_low_shut == false) && \
        bat_quantity_average && (bat_quantity_average <= BAT_SW_LB_SHUT_VOLT)) {
        logi("lb sw shut");
        bat_info.bat_low_shut = true;
        event_put(PMU_EVENT_BAT_LOW_SHUT, NULL, 0);
    } else if (!HAL_pmu_is_charging() && bat_quantity_average_old && (bat_quantity_average_old <= BAT_SW_LB_WRN_VOLT)) {
        bat_info.bat_low = true;
        bat_low_warning_cnt++;
        if (bat_low_warning_cnt == BAT_SW_LB_WARNING_PERIOD/BAT_RECORD_PERIOD) {
#ifndef BAT_SW_LB_WRN_NOLOOP
            bat_low_warning_cnt = 0;
#endif
            logi("lb wrn");
            event_put(PMU_EVENT_BAT_LOW_WRN, NULL, 0);
        }
#ifdef BAT_SW_PRE_SHUT_WRN
        if (bat_quantity_average <= BAT_SW_PRE_SHUT_WRN) {
            if(!shutdown_pre) {
                am_tone_play(SYS_TONE_VOL_SHUT_PRE, 1, 0, 50, 0, 0);
                shutdown_pre = 1;
            }
        }
#endif
    } else if (bat_info.bat_low) {
        bat_info.bat_low = false;
        shutdown_pre = 0;
        bat_low_warning_cnt = BAT_SW_LB_WARNING_PERIOD/BAT_RECORD_PERIOD - 1;
        logv("unlow:%u", bat_low_warning_cnt);
        event_put(PMU_EVENT_BAT_UNLOW, NULL, 0);
    }
    disp_bat_low_digital(bat_info.bat_low);

#if POWER_OFF_AUTO_WHEN_CON_DIS
        logv("discon_time:%u-%u", discon_time, rtc_get_second());
        if (discon_time != 0xffffffff) {
            if ((rtc_get_second() - discon_time) > POWER_OFF_AUTO_PERIOD) {
                logi("con dis timeout");
                if (HAL_pmu_is_use_hsw()) {
                    event_put(PMU_EVENT_HSW_OFF, NULL, 0);
                } else {
                    event_put(SYSTEM_EVENT_CON_DIS_TIMEOUT, NULL, 0);
                }
                discon_time = 0xffffffff;
            }
        }
#endif

#if AUDIO_KWS_EN
    cnt_pre++;
     if (cnt_pre % 60 == 0) {
        event_put(SYSTEM_EVENT_SYNC_BAT_PRECENT, NULL, 0);
    }
#endif

#if STAGE_CHARGE_EN
    if(HAL_pmu_get_bat_full_deb()) {
	    if (HAL_pmu_stage_charge_set(&stage_charge_curt,cutoff_current)) {
			OAL_timer_activate(&BatfullTimer, OAL_TIMER_AUTO_ACTIVATE);
	    } else {
	        HAL_pmu_set_bat_current(PMU_CHARGE_CURRENT);
        	stage_charge_curt = PMU_CHARGE_CURRENT;
	        if (mode_sta_get() == MODE_CHARGE) {
    	        logi("stage charge batfull");
                HAL_pmu_charge_enable(false);
#if CHARGING_BIN && (CHARGING_BIN_MID_LEVEL || CHARGING_BIN_CODE)
                
#else
                HAL_pmu_set_poweron_wakeupsrc(PMU_VBUSIN_WAKEUPSRC, false);
                //HAL_pmu_vbus_wakeup_hardware_ctrl(DISABLE);         //关闭关机后硬件自动恢复默认的VBUSIN wakeup
                mode_switch_reason_set(SW_CHARGE);
                mode_set_force(MODE_POWEROFF);
#endif
            }
	    }
    }
#endif

}
#endif

bool pmu_bat_is_exist(void)
{
    return BAT_EXIST;
}

//use in disp, must in sram
bool pmu_bat_is_low(void)
{
#if BAT_EXIST
    return bat_info.bat_low;
#else
    return false;
#endif
}

//use in disp, must in sram
bool pmu_bat_is_charging(void)
{
#if BAT_EXIST
    return HAL_pmu_is_charging();
#else
    return false;
#endif
}

bool pmu_vbus_is_in(void)
{
#if PMU_VBUS_FAKE_OUT
    return 0;
#else
    return (HAL_pmu_get_vbus_state() == PMU_VBUS_IN);
#endif
}

bool pmu_is_enter_charge_mode(void)
{
#if BAT_EXIST
    return HAL_pmu_is_vbus_pon();
#else
    return false;
#endif
}

bool pmu_poweroff_is_hswoff(void)
{
    if (HAL_pmu_is_use_hsw() && mode_switch_reason_get() == SW_HSW) {
        return true;
    } else {
        return false;
    }
}

AT_SRAM(.pmu)
void pmu_key_set(uint32_t event)
{
    key_onoff_process(event);
}

#if SEAMLESS_EN
void seamless_reason_set(uint8_t reason)
{
    seamless_reason = reason;
}

uint8_t seamless_reason_get(void)
{
    return seamless_reason;
}

void pmu_seamlesstimer_start(void)
{
    if (SeamlessTimer != NULL) {
        OAL_timer_activate(&SeamlessTimer, OAL_TIMER_AUTO_ACTIVATE);
    }
}

void pmu_seamlesstimer_stop(void)
{
    if (SeamlessTimer != NULL) {
        OAL_timer_activate(&SeamlessTimer, OAL_TIMER_NO_ACTIVATE);
    }
}

void seamless_timer_cb(void* xTimer)
{
    //主耳用
    logw("seamless 2s timeout");
    event_put(BT_EVENT_TWS_SEAMLESS_SKIP, NULL, 0);
}

//for seamless switch failed, the slave eraphone connect to phone
void seamless_fail_cb(void* xTimer)
{
    //副耳用
    logw("seamless failed 2.1s timeout");
    event_put(SYSTEM_EVENT_MODE_RESET, NULL, 0);
}
#endif

bool pmu_bootup_vbus(void)
{
    return bootup_vbus;
}

AT_SRAM(.pmu_auto_poweron)
void pmu_vbus_scan(void)
{
#if VBUS_OUT_AUTO_POWERON
        if (((mode_get_sta() == MODE_CHARGE) || (pmu_delay_poweroff_flag() == true)) && \
            (pmu_vbus_is_in() == 0) && \
            (ts_out != UINT64_MAX) && \
            ((HAL_get_timestamp() - ts_out) >= VBUS_OUT_TIMEOUT)) {
                ts_out = UINT64_MAX;
                logi("vbus out auto poweron");
                sleep_delay_set();
                event_unique_put(PM_CODE_EVENT | PM_POWERON, NULL, 0);
        }
#endif
}


void delaypoweroff_timer_cb(void *xTimer)
{
    logd("delay poweroff timeout");
    mode_set_force(MODE_CHARGE);
    event_clear(SYSTEM_EVENT_MODE_CHANGE);
    event_put(SYSTEM_EVENT_MODE_CHANGE, NULL, 0);
}

void pmu_delay_poweroff(void)
{
    if (delay_poweroff_flag == false) {
        logi("delay poff");
        if (DelayPoweroffTimer != NULL) {
            OAL_timer_activate(&DelayPoweroffTimer, OAL_TIMER_NO_ACTIVATE);
            OAL_timer_activate(&DelayPoweroffTimer, OAL_TIMER_AUTO_ACTIVATE);
        }
        delay_poweroff_flag = true;
    }
}

void pmu_delay_poweroff_end(void)
{
    if (delay_poweroff_flag == true) {
        logi("delay poff end");
        if (DelayPoweroffTimer != NULL) {
            OAL_timer_activate(&DelayPoweroffTimer, OAL_TIMER_NO_ACTIVATE);
        }
        mode_set_force(MODE_CHARGE);
        delay_poweroff_flag = false;
    }
}

void pmu_delay_poweroff_abort(void)
{
    if (delay_poweroff_flag == true) {
        logi("delay poff abort");
        if (DelayPoweroffTimer != NULL) {
            OAL_timer_activate(&DelayPoweroffTimer, OAL_TIMER_NO_ACTIVATE);
        }
        delay_poweroff_flag = false;
    }
}

AT_SRAM(.pmu)
bool pmu_delay_poweroff_flag(void)
{
    return delay_poweroff_flag;
}

void pmu_enter_dut_mode(uint32_t reason)
{
    pmu_delay_poweroff_end();
    logi("dut");
    bt_testmode_set(1);
    mode_switch_reason_set(reason);
    mode_set_force(MODE_RESET);
}

void pmu_code_event_process(uint32_t event)
{
    switch (event) {
    case PM_CODE_EVENT | PM_POWERON:
        pmu_delay_poweroff_end();
#if BLE_AIR_SEL
        air_poweron();
#endif
        if (mode_get_sta() == MODE_CHARGE) {
            /*
            logi("poweron");
            bt_testmode_set(0);
            mode_switch_reason_set(SW_CHARGING_BIN);
            mode_set_force(MODE_BT);
            */
            logi("poweron");
            restart();
        }
        break;

    case PM_CODE_EVENT | PM_POWEROFF:
        if (mode_get_sta() != MODE_CHARGE) {
            logi("poweroff");
            if (bt_testmode_get() == 0) {
#if BLE_AIR_SEL
                if (air_poweroff(0x00)) {
                    break;
                }
#endif
                mode_switch_reason_set(SW_CHARGING_BIN);
#if SEAMLESS_EN
                if ((seamless_phone_connected_get() == 1) && (tws_peer_con_status_get() == 1)) {
                    //对耳已连接、手机已连接、主耳
                    seamless_reason_set(SEAMLESS_POWEROFF);
                    event_put(SYSTEM_EVENT_SEAMLESS, NULL, 0);
                } else if (tws_peer_con_status_get() == 0) {
                    //对耳没有连接，无论手机是否连接，延时1s关机
                    pmu_delay_poweroff();
                } else {
                    //对耳连接，手机没有连接，直接关机
                    mode_set_force(MODE_CHARGE);
                }
#else
                mode_set_force(MODE_CHARGE);
#endif //SEAMLESS_EN
            } else if (bt_testmode_get() == 1) { //exit dut mode will restart to reset all value
                restart();
            }
        }
        break;

    case PM_CODE_EVENT | PM_SHUTDOWN:
        logi("shutdown");
#if BLE_AIR_SEL
        if (mode_get_sta() != MODE_CHARGE) {
            if (air_poweroff(0x01)) {
                logi("air break");
                break;
            }
        }
#endif
        pmu_delay_poweroff_end();
#if CHARGING_BIN_CODE
        if (mode_get_sta() == MODE_CHARGE) {
            mode_switch_reason_set(SW_BATFULL);
        }
#endif

#if SEAMLESS_EN
        if ((mode_get_sta() != MODE_CHARGE) &&
            (seamless_phone_connected_get() == 1) &&
            (tws_peer_con_status_get() == 1)) {
            seamless_reason_set(SEAMLESS_SHUTDOWN);
            event_put(SYSTEM_EVENT_SEAMLESS, NULL, 0);
        } else {
            mode_set_force(MODE_POWEROFF);
        }
#else
        mode_set_force(MODE_POWEROFF);
#endif //SEAMLESS_EN

#if BLE_AIR_SEL
        if (mode_get_sta() != MODE_CHARGE) {
            ble_air_when_poweroff();
        }
#endif
        break;

    case PM_CODE_EVENT | PM_DUT:
        pmu_enter_dut_mode(SW_CHARGING_BIN);
        break;

    case PM_CODE_EVENT | PM_PAIR:
#if CHARGING_BIN_CODE
        pmu_delay_poweroff_end();
        if (mode_get_sta() == MODE_CHARGE) {
            logi("pair");
            //bt_freqcal_mode_set(1);
            mode_switch_reason_set(SW_CHARGING_BIN);
            mode_set_force(MODE_RESET);
        }
#else
        pmu_delay_poweroff_end();
        logi("pair");
        //bt_freqcal_mode_set(1);
        mode_switch_reason_set(SW_CHARGING_BIN);
        mode_set_force(MODE_RESET);
#endif
        break;

    case PM_CODE_EVENT | PM_UNPAIR:
#if CHARGING_BIN_CODE
        pmu_delay_poweroff_end();
        if (mode_get_sta() == MODE_CHARGE) {
            logi("unpair");
            //bt_freqcal_mode_set(2);
            mode_switch_reason_set(SW_CHARGING_BIN);
            mode_set_force(MODE_RESET);
        }
#else
        pmu_delay_poweroff_end();
        logi("unpair");
        //bt_freqcal_mode_set(2);
        mode_switch_reason_set(SW_CHARGING_BIN);
        mode_set_force(MODE_RESET);
#endif
        break;
    }
}

void pmu_set_discon_time(uint32_t time)
{
#if POWER_OFF_AUTO_WHEN_CON_DIS
#if BAT_EXIST
    discon_time = time;
#endif
#if POWER_OFF_AUTO_WHEN_ANC_DIS && AUDIO_ANC_EN
    if ((time != 0xffffffff) && (anc_mode_param_get() == ANC_WORKING_MODE_DIS)) {
        anc_mode_timer_start();
    } else {
        anc_mode_timer_stop();
}
#endif
#endif
}

bool pmu_ear_in_case_state_get(void)
{
    bool state;

#if CHARGING_BIN_CODE_UART
    state = pmu_ear_is_in_charging_bin();
#else
    state = HAL_pmu_vbus_state_get();
#endif

    return state;
}

#if CHARGING_BIN
void pmu_charging_bin_process(uint32_t event)
{
#if CHARGING_BIN_CODE //有开关机码
    pmu_charging_bin_code(event);
#else
    pmu_charging_bin_noncode(event);
#endif
}

#if CHARGING_BIN_CODE //有开关机码
void pmu_charging_bin_code(uint32_t event)
{
    pmu_charging_bin_code_uart(event);
}

#if CHARGING_BIN_CODE_UART //充电仓uart通信功能有时候只用来配对和解配，当uart用来开关机时才需要这个函数
void pmu_charging_bin_code_uart(uint32_t event)
{
    switch(event) {
        case PMU_EVENT_VBUSMIDIN:
            logi("-vbus mid in");
            sleep_prevent_set(PS_UART);
            break;

        case PMU_EVENT_VBUSMIDOUT:
            logi("-vbus mid out");
            sleep_prevent_clear(PS_UART);
            break;

        case PMU_EVENT_CHARGE_IN:
        case PMU_EVENT_CHARGE_OUT:
#if STAGE_CHARGE_EN
            HAL_pmu_set_bat_current(PMU_CHARGE_CURRENT);
            stage_charge_curt_set(PMU_CHARGE_CURRENT);
#endif
            if (pmu_vbus_is_in()) {
                logi("-vbus in");
                //logi("-vbus in:%x-%x-%x", HAL_pmu_read(0x4d), HAL_pmu_read(0x3), HAL_pmu_read(0x17));
                pmu_ear_in_charging_bin_detect();
                //sleep_prevent_clear(PS_UART);
                HAL_pmu_charge_enable(true);
#if AUDIO_MUTE_EN
                audio_service_set_mute_mode(pmu_vbus_is_in());
#endif
#if (DISP_EN && STATE_CHARGEING_EN)
                disp_state(STATE_CHARGEING);
#endif
            } else {
                logi("-vbus out");
                //logi("-vbus out:%x-%x-%x", HAL_pmu_read(0x4d), HAL_pmu_read(0x3), HAL_pmu_read(0x17));
                sleep_delay_set();
                //sleep_prevent_set(PS_UART);
#if (DISP_EN && STATE_UNCHARGEING_EN)
              disp_state(STATE_UNCHARGEING);
#endif
#if AUDIO_MUTE_EN
                audio_service_set_mute_mode(pmu_vbus_is_in());
#endif
                HAL_pmu_charge_enable(false);
                HAL_msleep(100);
                //scene_mode.switch_reason = SW_CHARGE;
                //mode_set_force(MODE_POWEROFF);
                //disp_state(STATE_UNCHARGEING);
                //restart();
            }
            break;

        case PMU_EVENT_BAT_FULL:
            logi("-bat full");
#if (DISP_EN && STATE_BAT_FULL_EN)
            disp_state(STATE_BAT_FULL);
#endif
            break;

        case PM_CODE_EVENT | PM_POWERON:
        case PM_CODE_EVENT | PM_POWEROFF:
        case PM_CODE_EVENT | PM_SHUTDOWN:
        case PM_CODE_EVENT | PM_DUT:
        case PM_CODE_EVENT | PM_PAIR:
        case PM_CODE_EVENT | PM_UNPAIR:
        case PM_CODE_EVENT | PM_RECOV:
            pmu_code_event_process(event);
            break;
    }
}
#endif //CHARGING_BIN_CODE_UART

#else
void pmu_charging_bin_noncode(uint32_t event)
{
#if CHARGING_BIN_MID_LEVEL
    pmu_charging_bin_mid(event);
#else
    pmu_charging_bin_5V(event);
#endif
}
#endif

#if CHARGING_BIN_MID_LEVEL
void pmu_charging_bin_mid(uint32_t event)
{
    switch(event) {

        case PMU_EVENT_VBUSMIDIN:
            logi("-vbus mid in:%x-%x-%x", HAL_pmu_read(0x4d), HAL_pmu_read(0x3), HAL_pmu_read(0x17));
            break;

        case PMU_EVENT_VBUSMIDOUT:
            logi("-vbus mid out:%x-%x-%x", HAL_pmu_read(0x4d), HAL_pmu_read(0x3), HAL_pmu_read(0x17));
            break;

        case PMU_EVENT_CHARGE_IN:
        case PMU_EVENT_CHARGE_OUT:
#if STAGE_CHARGE_EN
            HAL_pmu_set_bat_current(PMU_CHARGE_CURRENT);
            stage_charge_curt_set(PMU_CHARGE_CURRENT);
#endif
            if (pmu_vbus_is_in()) {
                logi("-vbus in:%x-%x-%x", HAL_pmu_read(0x4d), HAL_pmu_read(0x3), HAL_pmu_read(0x17));
                HAL_pmu_charge_enable(true);
                pmu_code_event_process(PM_CODE_EVENT | PM_SHUTDOWN);
#if AUDIO_MUTE_EN
              audio_service_set_mute_mode(pmu_vbus_is_in());
#endif
#if (DISP_EN && STATE_CHARGEING_EN)
             disp_state(STATE_CHARGEING);
#endif
            } else {
                logi("-vbus out:%x-%x-%x", HAL_pmu_read(0x4d), HAL_pmu_read(0x3), HAL_pmu_read(0x17));
#if (DISP_EN && STATE_UNCHARGEING_EN)
                 disp_state(STATE_UNCHARGEING);
#endif
#if AUDIO_MUTE_EN
                audio_service_set_mute_mode(pmu_vbus_is_in());
#endif
                HAL_pmu_charge_enable(false);
                if (mode_get_sta() == MODE_CHARGE) {
                    if (CHARGING_BIN_MID_LEVEL_THRD) {
                        HAL_msleep(CHARGING_BIN_MID_LEVEL_THRD); //delay xms for vbus 0V to middle level 2.7V
                    }
                    if (HAL_pmu_get_vbus_state() == PMU_VBUS_MID) { //vbus level high, bat full in charging bin
                        uint32_t sleep_cnt = 0;
                        while((HAL_pmu_get_vbus_state() == PMU_VBUS_MID) && (sleep_cnt < 100)) {
                            HAL_msleep(10);
                            sleep_cnt++;
                        }
                        if (sleep_cnt > 80) {
                            logi("mid level bat full");
                            //HAL_pmu_enable_5kpd(true, false);
                            pmu_code_event_process(PM_CODE_EVENT | PM_SHUTDOWN);
                            mode_switch_reason_set(SW_BATFULL);
                            //把vbus out和vbus mid out的触发方式改为电平方式，避免在掉电的过程中产生vbus mid out无法开机
                            HAL_pmu_set_mid_out_trig_mode(PMU_MID_OUT_LEVEL);
#if (DISP_EN && STATE_BAT_FULL_EN)
                            disp_state(STATE_BAT_FULL);
#endif
                        } else if (HAL_pmu_get_vbus_state() == PMU_VBUS_OUT) {
                            logi("out charging bin");
                            pmu_code_event_process(PM_CODE_EVENT | PM_POWERON);
                        }
                    } else if (HAL_pmu_get_vbus_state() == PMU_VBUS_IN) {
                        logi("vbus in after vbus out");
                    } else {
                        logi("out charging bin");
                        //HAL_pmu_enable_5kpd(false, true);
                        pmu_code_event_process(PM_CODE_EVENT | PM_POWERON);
                    }
                } else {
                    pmu_code_event_process(PM_CODE_EVENT | PM_POWERON);
                }
            }
            break;

        case PMU_EVENT_BAT_FULL:
            logi("-bat full");
#if (DISP_EN && STATE_BAT_FULL_EN)
            //disp_state(STATE_BAT_FULL); //中间电平方案，不用耳机的满电检测进行灯显，直接使用充电仓的满电检测就可以
#endif
            break;

        case PM_CODE_EVENT | PM_POWERON:
        case PM_CODE_EVENT | PM_POWEROFF:
        case PM_CODE_EVENT | PM_SHUTDOWN:
        case PM_CODE_EVENT | PM_DUT:
        case PM_CODE_EVENT | PM_PAIR:
        case PM_CODE_EVENT | PM_UNPAIR:
        case PM_CODE_EVENT | PM_RECOV:
            pmu_code_event_process(event);
            break;
    }
}
#else
void pmu_charging_bin_5V(uint32_t event)
{
    switch(event) {
        case PMU_EVENT_INT:
            logi("-int");
            pmu_handler();
            break;

        case PMU_EVENT_CHARGE_IN:
        case PMU_EVENT_CHARGE_OUT:
#if STAGE_CHARGE_EN
            HAL_pmu_set_bat_current(PMU_CHARGE_CURRENT);
            stage_charge_curt_set(PMU_CHARGE_CURRENT);
#endif
            if (pmu_vbus_is_in()) {
                logi("-vbus in");
                HAL_pmu_charge_enable(true);
                pmu_code_event_process(PM_CODE_EVENT | PM_POWEROFF);
#if AUDIO_MUTE_EN
                audio_service_set_mute_mode(pmu_vbus_is_in());
#endif
#if (DISP_EN && STATE_CHARGEING_EN)
                disp_state(STATE_CHARGEING);
#endif
            } else {
                logi("-vbus out");
#if (DISP_EN && STATE_UNCHARGEING_EN)
                disp_state(STATE_UNCHARGEING);
#endif
#if AUDIO_MUTE_EN
                audio_service_set_mute_mode(pmu_vbus_is_in());
#endif
                HAL_pmu_charge_enable(false);
                if (mode_get_sta() == MODE_CHARGE) {
                    pmu_code_event_process(PM_CODE_EVENT | PM_POWERON);
                }
            }
            break;

        case PMU_EVENT_BAT_FULL:

#if !STAGE_CHARGE_EN
            logi("-bat full");
#if (DISP_EN && STATE_BAT_FULL_EN)
            disp_state(STATE_BAT_FULL);
#endif
            mode_switch_reason_set(SW_BATFULL);
            pmu_code_event_process(PM_CODE_EVENT | PM_SHUTDOWN);
            //HAL_pmu_charge_enable(false);
            //HAL_pmu_write(0x17, 0);//disable adc/charge/charge timeout/main charge mini
            //pmu_regu_enable(PMU_AVCC, 0);
            //pmu_set_poweron_wakeupsrc(PMU_VBUSIN_WAKEUPSRC, false);
            //scene_mode.switch_reason = SW_CHARGE;
            //mode_set_force(MODE_POWEROFF);
#if (DISP_EN && STATE_BAT_FULL_EN)
            disp_state(STATE_BAT_FULL);
#endif
#endif
            break;

        case PM_CODE_EVENT | PM_POWERON:
        case PM_CODE_EVENT | PM_POWEROFF:
        case PM_CODE_EVENT | PM_SHUTDOWN:
        case PM_CODE_EVENT | PM_DUT:
        case PM_CODE_EVENT | PM_PAIR:
        case PM_CODE_EVENT | PM_UNPAIR:
        case PM_CODE_EVENT | PM_RECOV:
            pmu_code_event_process(event);
            break;
    }
}
#endif
#else
void pmu_noncharging_bin_process(uint32_t event)
{
    switch(event) {
        case PMU_EVENT_INT:
            logi("-int");
            pmu_handler();
            break;

        case PMU_EVENT_CHARGE_IN:
        case PMU_EVENT_CHARGE_OUT:

#if STAGE_CHARGE_EN
            HAL_pmu_set_bat_current(PMU_CHARGE_CURRENT);
            stage_charge_curt_set(PMU_CHARGE_CURRENT);
#endif

            if (pmu_vbus_is_in()) {
                logi("-vbus in");
#if (G24_2T4_SHORT_PACKET_3TIMES || G24_2T4_WITH_MONO_MONITOR) && G24_FUNC_ROLE
                if(G24_2T4_DONGLE_MCHIP) {          //主芯片开充电，副芯片不开充电
                    HAL_pmu_charge_enable(true);
                }
#else
                HAL_pmu_charge_enable(true);
#endif

#if AUDIO_MUTE_EN
                audio_service_set_mute_mode(pmu_vbus_is_in());
#endif
#if (DISP_EN && STATE_CHARGEING_EN)
                disp_state(STATE_CHARGEING);
#endif
            } else {
                logi("-vbus out");
                HAL_pmu_charge_enable(false);
                if(_VbusCb) {
                    if(_VbusCb->vbus_out_cb) {
                        _VbusCb->vbus_out_cb();
                    }
                }
#if (DISP_EN)
                disp_state(STATE_UNCHARGEING);
#endif
#if AUDIO_MUTE_EN
                audio_service_set_mute_mode(pmu_vbus_is_in());
#endif
                if(mode_sta_get() == MODE_CHARGE) {
                    mode_switch_reason_set(SW_CHARGE);
                    mode_set_force(MODE_POWEROFF);
                }
            }
            break;
        case PMU_EVENT_BAT_FULL:

#if !STAGE_CHARGE_EN
            logi("-bat full");
#if (DISP_EN && STATE_BAT_FULL_EN)
            disp_state(STATE_BAT_FULL);
#endif
            if(mode_sta_get() == MODE_CHARGE) {
                HAL_pmu_set_poweron_wakeupsrc(PMU_VBUSIN_WAKEUPSRC, false);
                //HAL_pmu_vbus_wakeup_hardware_ctrl(DISABLE);         //关闭关机后硬件自动恢复默认的VBUSIN wakeup
                mode_switch_reason_set(SW_CHARGE);
                mode_set_force(MODE_POWEROFF);
            }
#endif

            break;

        case PM_CODE_EVENT | PM_POWERON:
        case PM_CODE_EVENT | PM_POWEROFF:
        case PM_CODE_EVENT | PM_SHUTDOWN:
        case PM_CODE_EVENT | PM_DUT:
        case PM_CODE_EVENT | PM_PAIR:
        case PM_CODE_EVENT | PM_UNPAIR:
        case PM_CODE_EVENT | PM_RECOV:
            pmu_code_event_process(event);
            break;
    }
}
#endif

void pmu_event_process(uint32_t event)
{
#if CHARGING_BIN
    pmu_charging_bin_process(event);
#else
    pmu_noncharging_bin_process(event);
#endif
}

#if 1//CHARGING_BIN_CODE_UART
OAL_THREAD uart_isr_bh_handler;
pm_uart_frame_t pm_uart_frame;
uint8_t pm_data_cnt;
volatile uint8_t pm_state;
uint8_t left_addr[6];
uint8_t charging_bin_bat_info;
volatile bool charging_bin_bat_init_state;
volatile bool earphone_in_charging_bin; //耳机是否在充电仓里面
volatile uint32_t earphone_in_charging_bin_ts;

void pmu_ear_in_charging_bin_detect(void)
{
    pmu_vbus_out_ts_set(pmu_vbus_is_in());
    earphone_in_charging_bin_ts = HAL_timestamp_to_ms(HAL_get_timestamp());
    //if (mode_get_sta() == MODE_BT) {
        if (earphone_in_charging_bin == false) {
            logi("\n---in cb---\n");
            sleep_prevent_set(PS_UART);
            event_put(PMU_EVENT_IN_CHARGING_BIN, NULL, 0);
        }
    //}
    earphone_in_charging_bin = true;
}


bool pmu_ear_is_in_charging_bin(void)
{
    return earphone_in_charging_bin;
}

//bit0~bit6:bat level(0~100), bit7:charging state, 0:uncharging, 1:charging

uint8_t pmu_get_charging_bin_bat_quantity(void)
{
    if (pmu_charging_bin_bat_init_state_get()) {
        return charging_bin_bat_info&0x7f;
    } else {
        return 0xff;
    }
}


bool pmu_get_charging_bin_bat_charging_state(void)
{
    return (charging_bin_bat_info>>7)&0x1;
}


void pmu_charging_bin_bat_init_state_set(bool state)
{
    charging_bin_bat_init_state = state;
}


bool pmu_charging_bin_bat_init_state_get(void)
{
    /* 上电初始化为false
     * 在获取到仓电量后改为true
     */
    return charging_bin_bat_init_state;
}


uint8_t crc8_maxim(uint8_t *buff, uint8_t length)
{
    uint8_t i;
    uint8_t crc = 0; //initial value

    while (length--) {
        crc ^= *buff++; //crc ^= *buff; buff++;
        for(i = 0; i < 8; i++) {
            if(crc & 1)
                crc = (crc >> 1) ^ 0x8C; //0x8C = reverse 0x31
            else
                crc >>= 1;
        }
    }
    return crc;
}

#define UART_ISR_BH_TASK_STACK_SIZE (128*2)
#define UART_ISR_BH_TASK_PRIO       3

void* uart_isr_bh(void *pvParameters)
{
    pmu_charging_bin_bat_init_state_set(false);
    while (1) {
        OAL_thread_notify_wait(0x0, 0xffffffff, NULL, OAL_WAIT_FOREVER);
        uart1_rx_ack();
    }
    return NULL;
}

void uart_isr_bh_init(void)
{
    printf_debug("uart_isr_bh_init\n");
    uart_isr_bh_handler = OAL_thread_create(uart_isr_bh, NULL, "uart_isr_bh", UART_ISR_BH_TASK_PRIO, UART_ISR_BH_TASK_STACK_SIZE);
}

void pmu_uart_txrx_enable(bool tx_en, bool rx_en)
{

    HAL_pmu_uart_txrx_enable(tx_en,rx_en);
	if ((tx_en == 1) && (rx_en == 0)) {
		HAL_gpio_func_set(PIN_B24, PIN_FUNC2); //tx
	} else if ((tx_en == 0) && (rx_en == 1)) {
		HAL_gpio_func_set(PIN_B24, PIN_FUNC3); //rx
	} else {
		HAL_gpio_func_set(PIN_B24, PIN_FUNCDISABLE); //disable
	}
    HAL_udelay(86);
}

extern BOOL write_flash_ews_left_or_right(BOOL is_ews_left);

static bool pmu_vbus_print_sta;
void pmu_vbus_print_set(bool enable)
{
    if ((enable == 1) && (pmu_vbus_print_sta == 0)) {
        logi("vbus print enable");
        disable_irq();
        pmu_uart_txrx_enable(1, 0);
        HAL_gpio_func_set(PIN_B24, PIN_FUNC7); //UART2 TX
        pmu_vbus_print_sta = 1;
        enable_irq();
    } else if ((enable == 0) && (pmu_vbus_print_sta == 1)) {
        logi("vbus print disable");
        disable_irq();
        pmu_uart_txrx_enable(0, 1);
        pmu_vbus_print_sta = 0;
        enable_irq();
    }
}

static bool pmu_vbus_spdif_sta;
void pmu_vbus_spdif_set(bool enable)
{
    if ((enable == 1) && (pmu_vbus_spdif_sta == 0)) {
        logi("vbus spdif enable");
        //disable_irq();
        HAL_pmu_uart_txrx_enable(1, 0);
        HAL_pmu_uarttx_driver(3);
        HAL_gpio_func_set(PIN_B24, PIN_FUNC10); //SPDIF TX
        pmu_vbus_print_sta = 1;
        pmu_vbus_spdif_sta = 1;

        //enable_irq();
    } else if ((enable == 0) && (pmu_vbus_spdif_sta == 1)) {
        logi("vbus spdif disable");
        //disable_irq();

        //set uart rx mode
        pmu_uart_txrx_enable(0, 1);
        pmu_vbus_print_sta = 0;
        pmu_vbus_spdif_sta = 0;
        //enable_irq();
    }
}

void uart1_rx_ack(void)
{
        uint8_t i;
        uint8_t val;
        uint8_t *frame = (uint8_t *)&pm_uart_frame;

    if (pm_state == PM_STATE_ACK_BH) {
        sleep_delay_set();
        pmu_ear_in_charging_bin_detect();
        if (pm_uart_frame.cmd_id == UART_CMD_POWERON) { //开盖
            event_put(PM_CODE_EVENT | PM_POWERON, NULL, 0);
        } else if (pm_uart_frame.cmd_id == UART_CMD_POWEROFF) { //关盖
            event_put(PM_CODE_EVENT | PM_POWEROFF, NULL, 0);
        }  else if (pm_uart_frame.cmd_id == UART_CMD_RESET) { //复位
            logi("restart");
            pmu_info_bat_level_set(pmu_get_bat_quantity_percent());
            pmu_uart_txrx_enable(0, 0);
            HAL_restart_pmu();
        } else if (pm_uart_frame.cmd_id == UART_CMD_SHUTDOWN) { //掉电
            event_put(PM_CODE_EVENT | PM_SHUTDOWN, NULL, 0);
        } else if (pm_uart_frame.cmd_id == UART_CMD_DUT) { //DUT模式
            event_put(PM_CODE_EVENT | PM_DUT, NULL, 0);
        } else if (pm_uart_frame.cmd_id == UART_CMD_BAT) { //告知充电仓电量，查询耳机电量
            //bit0~bit6:bat level(0~100), bit7:charging state, 0:uncharging, 1:charging
            //charging and uncharging state change
            if ((charging_bin_bat_info & (1 << 7)) && ((pm_uart_frame.data[0] & (1 << 7)) == 0)) {
                event_put(PMU_EVENT_CHARGING_BIN_UNCHARGING, NULL, 0);
            } else if (((charging_bin_bat_info & (1 << 7)) == 0) && (pm_uart_frame.data[0] & (1 << 7))) {
                event_put(PMU_EVENT_CHARGING_BIN_CHARGING, NULL, 0);
            }

            //charging bin bat quantity change
            if ((charging_bin_bat_info & 0x7f) != (pm_uart_frame.data[0] & 0x7f)) {
                logi("cb bat:%u", pm_uart_frame.data[0] & 0x7f);
                event_unique_put(PMU_EVENT_CHARGING_BIN_BAT_UPDATE, NULL, 0);
            }
            charging_bin_bat_info = pm_uart_frame.data[0];
            pmu_charging_bin_bat_init_state_set(true);
            val = pmu_get_bat_quantity_percent();
            if ((val == 100) && HAL_pmu_is_bat_full()) {
                pm_uart_frame.data[0] = 100;
            } else if ((val == 100) && !HAL_pmu_is_bat_full()) {
                pm_uart_frame.data[0] = 99;
            } else {
                pm_uart_frame.data[0] = val;
            }
#if CHARGING_BIN_AUTO_PAIR
            pm_uart_frame.data[1] = nor_tws_paired_get();
            pm_uart_frame.length = 2;
#endif
            //logi("ep bat:%u-%u-%u-%u", val, HAL_pmu_is_bat_full(), pm_uart_frame.data[0], pm_uart_frame.data[1]);
        } else if (pm_uart_frame.cmd_id == UART_CMD_DISCOV) { //可发现
            pm_uart_frame.data[0] = 0; //成功
            event_put(PM_CODE_EVENT | PM_DISCOV, NULL, 0);
        } else if (pm_uart_frame.cmd_id == UART_CMD_GETMAC) {//获取本地mac地址
            //memcpy((uint8_t *)pm_uart_frame.data, bt_local_addr, 6);
            //bt_addr_get((uint8_t *)pm_uart_frame.data);
            sys_info_read(BT_ADDR_INFO, (uint8_t *)pm_uart_frame.data, 6);
            logi("get addr:%02x:%02x:%02x:%02x:%02x:%02x",
                    pm_uart_frame.data[5],pm_uart_frame.data[4],pm_uart_frame.data[3],
                    pm_uart_frame.data[2],pm_uart_frame.data[1],pm_uart_frame.data[0]);
            pm_uart_frame.length = 6;
        } else if (pm_uart_frame.cmd_id == UART_CMD_SETMAC) {//设置对耳mac地址
            //ews_set_peer_addr((uint8_t *)pm_uart_frame.data);
            logi("set addr:%02x:%02x:%02x:%02x:%02x:%02x",
                    pm_uart_frame.data[5],pm_uart_frame.data[4],pm_uart_frame.data[3],
                    pm_uart_frame.data[2],pm_uart_frame.data[1],pm_uart_frame.data[0]);
            if (pm_uart_frame.left_or_right == 0x22) {
                memcpy(left_addr, (uint8_t *)pm_uart_frame.data, 6);
                }
                pm_uart_frame.data[0] = 0; //设置地址成功
                pm_uart_frame.length = 1;
        } else if(pm_uart_frame.cmd_id == UART_CMD_SET_LINK_ADDR){
            logi("link info write, %d", pm_uart_frame.length);
            if(pm_uart_frame.length > 12) {
                pm_uart_frame.length = 12;
            }
            printf_array((uint8_t*)pm_uart_frame.data, pm_uart_frame.length);
            
            sys_info_write(G24_LINK_INFO, (uint8_t*)pm_uart_frame.data, pm_uart_frame.length);
            pm_uart_frame.data[0] = 0;
            pm_uart_frame.length = 1;
        }else if (pm_uart_frame.cmd_id == UART_CMD_PAIR_SUCCESS) {//tws配对指令，对耳交换地址成功
            pm_uart_frame.data[0] = nor_tws_pair((pm_uart_frame.left_or_right == 0x11)?1:0, left_addr);
            pm_uart_frame.length = 1;
#if CHARGING_BIN_AUTO_PAIR
            //pmu_uart_txrx_enable(0, 0);
            //restart_pmu();
#endif
#if (SENSOR_EN&&SENSOR_PROXIMITY_EN && (SENSOR_PROXIMITY_DEVICE_SEL == 3 ||SENSOR_PROXIMITY_DEVICE_SEL == 4))
               uint8_t step;
#if (SENSOR_PROXIMITY_DEVICE_SEL == 4)
            if (sensor_proximity_get_select() == LTK1181)
#endif
            {
                if(Ltk1181_calibration(&step)==0){
                 pm_uart_frame.data[1] = 1;
                 pm_uart_frame.data[2] = step;
                 pm_uart_frame.length = 3;
                }
            }
#endif
        } else if (pm_uart_frame.cmd_id == UART_CMD_PAIR_FAIL) {//单耳配对指令，对耳交换地址失败
            //配对失败该做什么？
         } else if (pm_uart_frame.cmd_id == UART_CMD_UNPAIR) {//解配指令
            pm_uart_frame.data[0] = nor_tws_unpair((pm_uart_frame.left_or_right == 0x11)?1:0);
            pm_uart_frame.length = 1;
        } else if (pm_uart_frame.cmd_id == UART_CMD_UNPAIR_SUCCESS) {//解配成功
            pmu_info_bat_level_set(pmu_get_bat_quantity_percent());
            pmu_uart_txrx_enable(0, 0);
            HAL_restart_pmu();
        } else if (pm_uart_frame.cmd_id == UART_CMD_UNPAIR_FAIL) {//解配失败
            //解配失败该做什么？
        } else if (pm_uart_frame.cmd_id == UART_CMD_CAL) {//校准频偏
            //logi("bt_freqcal_mode_set");
          //  bt_freqcal_mode_set(1);FIX ME LCJ
            event_put(PMU_EVENT_CAL_FREQ, NULL, 0);
        } else if (pm_uart_frame.cmd_id == UART_CMD_GET_FREQ_CAL) {//解配失败
            int *pdata = (int *)&pm_uart_frame.data[1];
            //extern bool get_freq_offset(int *offset); //FIXME:WXP
            extern unsigned int rfdss_fre_cal(uint32_t trimData, uint16_t xoCurrent );
            uint16_t trimOriCap = nor_freq_drift_get();
            uint32_t trimCap;
            logi("begin drift %x +++++++",trimOriCap);
            if(trimOriCap  < 256) {
                trimCap =  rfdss_fre_cal(trimOriCap,RTC_DCXO_CUR);
            } else {
                trimCap =  rfdss_fre_cal(0xFFFFFFFF,RTC_DCXO_CUR);
            }

            if (trimCap < 0x100/*get_freq_offset(&offset)*/) {
                extern int16_t rdsss_get_fre_offset();
                int16_t offset = rdsss_get_fre_offset();
                logi("trim  res cap: %x - offset: %x-------------",trimCap, offset);

                pm_uart_frame.data[0] = 0;
                *pdata = offset;
                pm_uart_frame.length = 5;

                extern void nor_freq_drift_set(uint16_t value);
                nor_freq_drift_set(trimCap);
            } else {
                logi("trim  fail");
                pm_uart_frame.data[0] = 1;
                pm_uart_frame.length = 1;
            }
        } else if (pm_uart_frame.cmd_id == UART_CMD_GET_NAME) {//获取蓝牙名字
            uint8_t len;
            uint8_t *name = NULL;
            name  = (uint8_t *)libc_malloc(32);
            if (name == NULL) {
                loge("malc err");
            } else {
                bt_name_get(name, &len);
                if (len > 32) {
                    len = 32;
                }
                 logi("**bt_get_name:%s",name);
                memcpy((void *)&pm_uart_frame.data[0], name, len);
                pm_uart_frame.length = (len < 32) ? len : 32;
                libc_free(name);
            }
        } else if (pm_uart_frame.cmd_id == UART_CMD_SET_NAME) {//设置蓝牙名字
            uint8_t len;
            uint8_t *name = NULL;
            name  = (uint8_t *)libc_malloc(32);
            if (name == NULL) {
                loge("malc err");
            } else {
				len = pm_uart_frame.length;
				if (len > 32) {
                    len = 32;
                }
                memcpy(name, (void *)&pm_uart_frame.data[0], len);
				logi("name:%s len:%d", name,len);
			    pm_uart_frame.data[0] = nor_tws_name_set(name, len);
                pm_uart_frame.length = 1;
                libc_free(name);
            }
        } else if (pm_uart_frame.cmd_id == UART_CMD_RECOV) {//恢复出厂设置
#if CHARGING_BIN_AUTO_PAIR
            pm_uart_frame.data[0] = 0;
            pm_uart_frame.length = 1;
#endif
        } else if (pm_uart_frame.cmd_id == UART_CMD_FW_UPDATE) {//重启进入BootLoader
#if BAT_EXIST
            HAL_pmu_get_bat_info(&bat_info);
            if (bat_info.vbat > 3500) {
                HAL_pmu_uart_update_flag_set(1);
                pm_uart_frame.data[0] = 0; //0:success, 1:fail, 2:low bat
            } else {
                pm_uart_frame.data[0] = 2; //0:success, 1:fail, 2:low bat
            }
            pm_uart_frame.length = 1;
#endif
        } else if (pm_uart_frame.cmd_id == UART_CMD_SET_BT_ARRD) {//设置主耳地址
            logi("set_bt_addr:");
            printf_array((void *)&pm_uart_frame.data[0], 6);
            sys_info_write(BT_ADDR_INFO, (void *)&pm_uart_frame.data[0], 6);
            pm_uart_frame.data[0] = 0;
            pm_uart_frame.length = 1;
        }

#if BT_AIR_SEL
        else if (pm_uart_frame.cmd_id == UART_CMD_SET_SN) {
            logi("uart cmd set sn %s",pm_uart_frame.data);
            uint8_t len;
            uint8_t *sn = NULL;
            sn = (uint8_t *)libc_malloc(12);
            if (sn == NULL) {
                loge("malc err");
            } else {
                len = pm_uart_frame.length;
                if (len > 12) {
                    len = 12;
                }
                memcpy(sn, (void *)&pm_uart_frame.data[0], len);
                //logx("sn:");
                //printf_array(sn, 12);
                sys_info_write(SN_INFO, (void *)&pm_uart_frame.data[0], 12);
                pm_uart_frame.data[0] = 0;
                pm_uart_frame.length = 1;
                libc_free(sn);
            }
        }
        else if(pm_uart_frame.cmd_id == UART_CMD_SET_A_EN) {
            logi("cmd set a_en ----- %d",pm_uart_frame.data[0]);
            sys_info_write(EN_INFO, (void *)&pm_uart_frame.data[0], 1);
            pm_uart_frame.data[0] = 0;
            pm_uart_frame.length = 1;
        } else if(pm_uart_frame.cmd_id == UART_CMD_SET_KWS) {
            sys_info_write(KWS_INFO, (void *)&pm_uart_frame.data[0], 1);
            pm_uart_frame.data[0] = 0;
            pm_uart_frame.length = 1;
        }
#endif
        else if(pm_uart_frame.cmd_id == UART_CMD_COMMON_WRITE) {
/* 规则
 key: 一定是文本， 以0x00结束
 --控制,保留      ------- key -------------  结束  --- ++++++ val 数据 +++++++++
0x00 , 0x00, 'b','t','_', n','a','m','e',0x00, 'l','w','q','t','e','s','t'

*/
            logi("++++++++++++++++++++++++++++++++++");
            uint8_t keyLen = strlen((char*)pm_uart_frame.data + 2) + 1;
            char* key = (char*)pm_uart_frame.data + 2;
            logi("key %s,len :%d, valLen %d",key, keyLen,pm_uart_frame.length - keyLen - 2);
            printf_array((char*)key + keyLen , pm_uart_frame.length -2 - keyLen);
            sys_info_write((char*)key,(void *)&pm_uart_frame.data[keyLen + 2], pm_uart_frame.length - 2 - keyLen);
            pm_uart_frame.length = 1;
        } else if(pm_uart_frame.cmd_id == UART_CMD_COMMON_READ) {
            char* key = (char*)pm_uart_frame.data + 2;
            sys_info_read(key, (void *)&pm_uart_frame.data[0], 32);
            pm_uart_frame.length = 32;
        }


#if CHARGING_BIN_MID_LEVEL
        //if (mode_get_sta() == MODE_BT) {
            HAL_pmu_enable_5kpd(false, false);
        //}
#endif
        pmu_uart_txrx_enable(1, 0);
        for (i = 0; i < (5 + pm_uart_frame.length); i++) {
            HAL_uart_tx(UART_ID_01, frame[i]);
            //logi("%x", frame[i]);
        }
        pm_uart_frame.crc = crc8_maxim(frame, 5 + pm_uart_frame.length);
        HAL_uart_tx(UART_ID_01, pm_uart_frame.crc);
        //logi("%x", pm_uart_frame.crc);
        if (UART_CMD_BAT != pm_uart_frame.cmd_id) {
            logi("\n-->>cmd:%x\n", pm_uart_frame.cmd_id);
        }
        pmu_uart_txrx_enable(0, 1);
#if CHARGING_BIN_MID_LEVEL
        if (mode_get_sta() == MODE_BT) {
            HAL_pmu_enable_5kpd(false, true);
        } else {
            HAL_pmu_enable_5kpd(true, false);
        }
#endif
#if CHARGING_BIN_AUTO_PAIR
        if ((pm_uart_frame.cmd_id == UART_CMD_PAIR_SUCCESS) || (pm_uart_frame.cmd_id == UART_CMD_RECOV) || pm_uart_frame.cmd_id == UART_CMD_SET_BT_ARRD) {//可发现
            if (pm_uart_frame.cmd_id == UART_CMD_RECOV) {
                nor_tws_unpair((pm_uart_frame.left_or_right == 0x11)?1:0);
            }
            pmu_info_bat_level_set(pmu_get_bat_quantity_percent());
            pmu_uart_txrx_enable(0, 0);
            HAL_restart_pmu();
        }
#endif
        if ((pm_uart_frame.cmd_id == UART_CMD_FW_UPDATE) && (pm_uart_frame.data[0] == 0)) {
            pmu_info_bat_level_set(pmu_get_bat_quantity_percent());
            pmu_uart_txrx_enable(0, 0);
            HAL_restart_pmu();
        }

        if ((pm_uart_frame.cmd_id == UART_CMD_VBUS_PRINT)) {
            pmu_vbus_print_set(1);
        }

        pm_state = PM_STATE_HEADER;
        pm_uart_frame.header = 0;
    }
}

AT(.pmu_uart_isr_sram_seg)
void uart1_rx_data(uint32_t data)
{
    //uint8_t i;
    uint8_t crc;
    uint8_t *frame = (uint8_t *)&pm_uart_frame;

    //logi("rx:%x\n",  data);
    if ((data == 0x55) && (pm_uart_frame.header == 0x0)) {
        pm_uart_frame.header = 0x55;
    } else if ((data == 0xaa) && (pm_uart_frame.header == 0x55)) {
        pm_uart_frame.header = 0xaa55;
        pm_uart_frame.length = 0;
        pm_state = PM_STATE_CMD;
        //logi("header");
    } else if (pm_state == PM_STATE_CMD) {
        //logi("rxcmd:%x", data);
        pm_uart_frame.cmd_id = data;
        pm_state = PM_STATE_LR;
    } else if (pm_state == PM_STATE_LR) {
        //logi("lr");
        pm_uart_frame.left_or_right = data;
        pm_state = PM_STATE_LEN;
    } else if (pm_state == PM_STATE_LEN) {
        //logi("len");
        pm_uart_frame.length = (data < 32) ? data : 32;
        if (pm_uart_frame.length == 0) {
            pm_state = PM_STATE_CRC;
        } else {
            pm_state = PM_STATE_DATA;
            pm_data_cnt = 0;
        }
    } else if (pm_state == PM_STATE_DATA) {
        //logi("data");
        if (pm_data_cnt < pm_uart_frame.length) {

            if (pm_data_cnt < 32) {
                pm_uart_frame.data[pm_data_cnt++] = data;
            } else {
                pm_data_cnt = 0;
            }
        }
        if (pm_data_cnt == pm_uart_frame.length) {
            pm_state = PM_STATE_CRC;
        }
    } else if (pm_state == PM_STATE_CRC) {
        pm_uart_frame.crc = data;
        #if 0
        logi("crc-%u", 5 + pm_uart_frame.length);
        for (i = 0; i < 5; i++) {
            logi("%x", frame[i]);
        }
        #endif
        crc = crc8_maxim(frame, 5 + pm_uart_frame.length);

            if (pm_uart_frame.crc == crc) {
            pm_state = PM_STATE_ACK;
            //logi("crc correct cmd:%x", pm_uart_frame.cmd_id);
        } else {
            pm_state = PM_STATE_HEADER;
            pm_uart_frame.header = 0;
            //logi("crc error cmd:%x-%x-%x", pm_uart_frame.cmd_id, pm_uart_frame.crc, crc);
        }
    }
    if (pm_state == PM_STATE_ACK) {
		pm_state = PM_STATE_ACK_BH;
		if (uart_isr_bh_handler != NULL) {
             OAL_thread_notify(uart_isr_bh_handler, 0, 0);
		}
    }
}
#endif

AT(.pmu_sram_seg)
void pmu_hw_timer(void)
{
#if CHARGING_BIN_CODE
    pmu_vbus_scan();
#endif
    HAL_pmu_vbus_timer();

#if CHARGING_BIN_CODE_UART
        if (pmu_vbus_is_in()) {
            pmu_ear_in_charging_bin_detect();
        } else if ((earphone_in_charging_bin == true) && \
            ((HAL_timestamp_to_ms(HAL_get_timestamp()) - earphone_in_charging_bin_ts) >= ((mode_get_sta() == MODE_BT)?UART_OUT_BIN_PON_THR:UART_OUT_BIN_POF_THR))) {
                logi("\n---out cb---\n");
                sleep_prevent_clear(PS_UART);
                event_put(PMU_EVENT_OUT_CHARGING_BIN, NULL, 0);
                earphone_in_charging_bin = false;
        }
#endif
}

BOOL is_pmu_sd_update_flag_set();
void update_from_sd_flag_clear();
void update_from_sd_flag_set();

//AT_SRAM(.usb_update)
bool is_usb_update_flag_set()
{
    return is_pmu_sd_update_flag_set();
}

//AT_SRAM(.usb_update)
void clear_usb_update_flag()
{
    HAL_pmu_sd_update_flag_set(false);
}

//AT_SRAM(.usb_update)
void set_usb_update_flag()
{
    HAL_pmu_sd_update_flag_set(true);
}

#if LCDC_EN
void pmu_disp_function(void)
{
    if(bat_info.vbat >= 4000){
        systemStatus.batteryLevel = BATTERY_FULL;  
    }else if(bat_info.vbat < 4000 && bat_info.vbat >= 3700){
        systemStatus.batteryLevel = BATTERY_THREE_BARS; 
    }else if(bat_info.vbat < 3700 && bat_info.vbat >= 3500){
        systemStatus.batteryLevel = BATTERY_TWO_BARS; 
    }else if(bat_info.vbat < 3500){
        systemStatus.batteryLevel = BATTERY_ONE_BAR; 
    }
}
#endif
