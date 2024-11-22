#ifndef _APP_PMU_H_
#define _APP_PMU_H_
#include"hal_pmu.h"
/*
 * read 1 time per 400ms, total read 10 times, so update bat quantity percent 1 time per 4s
 * set period to 400ms can avoid vbat change suddenly when some load change, for example sdcard in/out
 *
 * when set BAT_QUANTITY_STEP_BIG to 1, will set the step to 25%, this case is for bluetooth Speaker, can save code space.
 * when set BAT_QUANTITY_STEP_BIG to 0, will set the step to 5%, this case is for bluetooth Headset, will use more code space.
 */
#define BAT_RECORD_TIME           8
#define BAT_RECORD_PERIOD         1000
#define BAT_QUANTITY_STEP_PERCENT 10
#define CV_TABLE_LEN              ((100 / BAT_QUANTITY_STEP_PERCENT) + 1)

typedef struct _VBUS_EVENT_CB_
{
    void (*vbus_out_cb)();  //VBUS OUT事件回调
}VBusCb;

#if CHARGING_BIN_CODE_UART
enum pm_valid_value_e {
    PM_NULL = 0x0,
    PM_POWERON = 0x30,  //cmd0
    PM_POWEROFF = 0x28, //cmd1
    PM_SHUTDOWN = 0x24, //cmd2
    PM_PAIR = 0x3c,     //cmd3
    PM_DUT = 0x22,      //cmd4
    PM_DISCOV = 0x3a,   //cmd5
    PM_RECOV = 0x36,    //cmd6
    PM_UNPAIR = 0x2e,   //cmd7
};
#define VBUS_OUT_TIMEOUT              1000000
#define VBUS_OUT_AUTO_POWERON         1
#else
enum pm_valid_value_e {
    PM_NULL = 0x0,
    PM_POWERON = 0x30,  //cmd0
    PM_POWEROFF = 0x28, //cmd1
    PM_SHUTDOWN = 0x24, //cmd2
    PM_PAIR = 0x3c,     //cmd3
    PM_DUT = 0x22,      //cmd4
    PM_DISCOV = 0x3a,   //cmd5
    PM_RECOV = 0x36,    //cmd6
    PM_UNPAIR = 0x2e,   //cmd7
};
#define VBUS_OUT_TIMEOUT              1000000
#define VBUS_OUT_AUTO_POWERON     0
#endif

#define UART_OUT_BIN_PON_THR  550  //ms,充电仓200ms轮询一次耳机电量
#define UART_OUT_BIN_POF_THR  2000 //ms,充电仓200ms轮询一次耳机电量

/* INTS */
#define PMU_INTS_VBUSIN                   (0x1 << 3)  //vbus in
#define PMU_INTS_VBUSOUT                  (0x1 << 4)  //vbus out

typedef struct pm_uart_frame
{
	volatile uint16_t header;//0x55aa
	volatile uint8_t cmd_id;
	volatile uint8_t left_or_right;
	volatile uint8_t length;
	volatile uint8_t data[48];//缓冲器从32->48  以适用通用PMU命令
	volatile uint8_t crc;
} pm_uart_frame_t;

#define PM_HEADER    0x55aa
#define PM_LEFT_EAR  0x11
#define PM_RIGHT_EAR 0x22

#define PM_STATE_HEADER 1
#define PM_STATE_CMD    2
#define PM_STATE_LR     3
#define PM_STATE_LEN    4
#define PM_STATE_DATA   5
#define PM_STATE_CRC    6
#define PM_STATE_ACK    7
#define PM_STATE_ACK_BH 8

enum pm_uart_cmd_e {
    UART_CMD_NULL = 0x0,
    UART_CMD_POWERON = 0x01,
    UART_CMD_POWEROFF = 0x02,
    UART_CMD_RESET = 0x03,
    UART_CMD_SHUTDOWN = 0x04,
    UART_CMD_PAIR_SUCCESS = 0x05,
    UART_CMD_PAIR_FAIL = 0x06,
    UART_CMD_INFO = 0x07,
    UART_CMD_BAT = 0x08,
    UART_CMD_DISCOV = 0x09,
    UART_CMD_RECOV = 0x0a,
    UART_CMD_DUT = 0x0b,
    UART_CMD_CAL = 0x0c,
    UART_CMD_UNPAIR = 0x0d,
    UART_CMD_UNPAIR_SUCCESS = 0x0e,
    UART_CMD_UNPAIR_FAIL = 0x0f,
    UART_CMD_GETMAC = 0x20,
    UART_CMD_SETMAC = 0x21,     //只是用于在配对时交互蓝牙地址，其实不是修改耳机蓝牙本来的地址，原来的地址还是会备份
    UART_CMD_GET_FREQ_CAL = 0x22,
    UART_CMD_PAIR = 0x23,
    UART_CMD_GET_NAME = 0x24,   //获取内存中的名字
    UART_CMD_SET_NAME = 0x25,   //修改固件蓝牙名字
    UART_CMD_CHANGE_MAC = 0x26, //修改固件蓝牙地址
    UART_CMD_SET_SN = 0x27,
    UART_CMD_SET_A_EN = 0x28,
    UART_CMD_SET_KWS = 0x29,
    UART_CMD_SET_LINK_ADDR = 0x2A,

    UART_CMD_FW_UPDATE = 0x80,
    UART_CMD_GET_UPDATE_STA = 0x81,
    UART_CMD_FW_PARA = 0x82,
    UART_CMD_FW_DATA = 0x83,
    UART_CMD_SET_BT_ARRD = 0x86, //设置蓝牙地址
    UART_CMD_VBUS_PRINT = 0x87,  //使能vbus打印
    UART_CMD_MOKEY_TEST = 0x88,  //monkey test

    //公共命令，写参数需要包括一个KEY和VALUE，读参数则不需要VALUE,中间需要0x00间隔
    //返回的时候耳机需要把KEY一起发过来
    UART_CMD_COMMON_WRITE   =   0xA0,
    UART_CMD_COMMON_READ    =   0xA1,
};

enum seamless_reason_e {
    SEAMLESS_POWEROFF,
    SEAMLESS_SHUTDOWN,
};

void delaypoweroff_timer_cb(void* xTimer);
void pmu_seamlesstimer_start(void);
void pmu_seamlesstimer_stop(void);
void seamless_timer_cb(void* xTimer);
void seamless_fail_cb(void* xTimer);
void seamless_reason_set(uint8_t reason);
uint8_t seamless_reason_get(void);

void pmu_early_init(void);
void pmu_late_init(void);
void pmu_deinit(void);
void pmu_handler(void);
void pmu_irq_proc(void);
void* pmu_isr_bh(void *pvParameters);
bool pmu_is_enter_charge_mode(void);
void pmu_task_init(void);
void pmu_task_deinit(void);
void pmu_bat_quantity_init(void);
void pmu_bat_init(void);
uint8_t pmu_voltage_to_percent(uint32_t volt);
uint32_t pmu_percent_to_voltage(uint32_t percent);
uint8_t pmu_get_bat_quantity_percent(void);
bool pmu_bat_is_exist(void);
bool pmu_bat_is_low(void);
bool pmu_bat_is_charging(void);
bool pmu_vbus_is_in(void);
void pmu_isr_bh_init(void);
void pmu_prohibit(bool prohibit);
void pmu_timer_cb(void* xTimer);
void pmu_vbus_scan(void);
bool pmu_bootup_vbus(void);
void pmu_event_parse(uint32_t event);



void pmu_delay_poweroff(void);
void pmu_delay_poweroff_end(void);
void pmu_delay_poweroff_abort(void);
bool pmu_delay_poweroff_flag(void);




void pmu_set_discon_time(uint32_t time);
void pmu_code_event_process(uint32_t event);
void pmu_pm_code_process(bool in);
void pmu_key_set(uint32_t event);

void pmu_seamlesstimer_stop(void);
void pmu_seamlesstimer_start(void);

bool pmu_ear_in_case_state_get(void);
bool pmu_ear_is_in_charging_bin(void);
void pmu_event_process(uint32_t event);
void pmu_charging_bin_code(uint32_t event);
void pmu_noncharging_bin_process(uint32_t event);
void pmu_charging_bin_code(uint32_t event);
void pmu_charging_bin_code_vbus(uint32_t event);
void pmu_charging_bin_code_uart(uint32_t event);
void pmu_charging_bin_noncode(uint32_t event);
void pmu_charging_bin_mid(uint32_t event);
void pmu_charging_bin_5V(uint32_t event);
void pmu_noncharging_bin_process(uint32_t event);
void pmu_ear_in_charging_bin_detect(void);
uint8_t pmu_get_charging_bin_bat_quantity(void);
bool pmu_get_charging_bin_bat_charging_state(void);
bool pmu_charging_bin_bat_init_state_get(void);
void uart1_rx_ack(void);
void pmu_uart_txrx_enable(bool tx_en, bool rx_en);
void uart_isr_bh_init(void);
void uart1_rx_data(uint32_t data);
void pmu_hw_timer(void);
void pmu_enter_dut_mode(uint32_t reason);
uint8_t pmu_info_bat_level_get(void);
void pmu_info_bat_level_set(uint8_t bat_level);
void pmu_vbus_print_set(bool enable);
void pmu_vbus_spdif_set(bool enable);
void pmu_reset_time_set(uint32_t ms);
void batfull_timer_cb(void* xTimer);
void stage_charge_curt_set(uint32_t cur);
void pmu_disp_function(void);

#endif


