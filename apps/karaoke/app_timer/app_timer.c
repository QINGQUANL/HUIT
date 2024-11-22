#define LOG_LEV 4
#include "sys_inc.h"
#include "hal.h"
#include "user_config.h"
#include "hal_power.h"
#include "key.h"
#include "hal_timer.h"
#include "hal_timestamp.h"
#include "timer.h"
#include "app_key.h"
#include "app_key_config.h"
#include "device.h"
#include "app_pwm.h"
#include "g24_api.h"
#include "key_ctrl_led.h"
#include "mode_g24.h"

#if MOTOR_EN
#include "motor.h"
#endif
#include "sensor.h"
#include "app_pmu.h"
#include "mode_manager.h"
#include "ui.h"

#define IRQn_TIMER0_PRIO            7
#define IRQn_TIMER1_PRIO            10
#define IRQn_TIMER2_PRIO            10
#define IRQn_TIMER3_PRIO            10

volatile uint32_t cnt_t0;
extern uint32_t   g24_handsetID;
extern uint32_t   broadcast_code;
#define CNT_PAIR	200		//duration of all pairing>>200----1000ms

uint32_t g24_get_sbc_number();
AT_SRAM(.g24_mmic_online_check)
bool g24_master_mic_check()
{
#if G24_MONITOR_EN && (G24_FUNC_ROLE == 1)  //dongle
    g24_param_t *g24_param_ptr = g24_param_get();
    if(g24_param_ptr->func_role == 1) { //only dongle
        static uint32_t s_master_mic_count = 0;
        static uint8_t discon_count = 0;
        //ֻ����Ͽ����ɣ���Ϊ���ӵ�ʱ���֪���ǲ�����MIC��
        uint32_t master_mic_check_count = g24_get_sbc_number();
        if(g24_is_monitor_master_mic_connected()) {
            if(s_master_mic_count != master_mic_check_count) {
                //˵����������
                s_master_mic_count = master_mic_check_count;
                discon_count = 0;
             } else {
                discon_count++;
                logi("discon_count %d, %d, %d", s_master_mic_count, master_mic_check_count, discon_count);
                if(discon_count == G24_MONITOR_MASTER_DISCON_TIME && master_mic_check_count != 0) {
                    master_mic_check_count =0;
                    s_master_mic_count = 0;
                    g24_monitor_master_mic_connect_set(false);
                    g24_audio_set_m_monitor_id(0);
                    logi("master mic discon###");
                    discon_count = 0;
                    return false;
                }
             }
        } 
    }
#endif
    return 1;
}


AT_SRAM(.timer)
void timer0_cb(void)
{
    pmu_hw_timer();

#if (KEY_IO_EN || KEY_IR_EN || (KEY_AD_EN && KADC_SCAN_EN) || KEY_CODING_EN || KEY_ONOFF_EN)
    if (device_startup_get())
        key_scan();
#endif


#if (SENSOR_G_EN && SENSOR_G_CLICK_SEL == 2)
    sensor_g_scan();
#endif

    cnt_t0++;
    if (cnt_t0 % 100 == 0) {
#if MOTOR_EN
        motor_timer_cb();
#endif
    }

#if MODE_G24_EN
    if(mode_sta_get() == MODE_G24) {
        if (cnt_t0 % 200 == 0) {
            p24g_timer_afh_fer_entity(0);
#if G24_DUAL_CHANNEL
            p24g_timer_afh_fer_entity(1);
#endif
        }

#if G24_MONITOR_EN && (G24_FUNC_ROLE == 1)  //dongle only
        if(cnt_t0 % 200 == 0) { //1000ms check once
            if(!g24_master_mic_check()) {
                g24_handsetID = 0;
            }
        }
#endif
    }

#if G24_FUNC_ROLE == 0  //handset
#if G24_MONITOR_EN && (G24_MONITOR_MASTER_EN == 0)
#if G24_MONITOR_ONLINE_DETECT
    if(monitor_heartbreak_is_enable() && (mode_get_sta() == MODE_G24) ) {
        g24_mode_t *g24_mode = g24_mode_param_get();
        if(g24_mode->connect[0]) {
            uint32_t stamp = monitor_handset_detect_stamp_get();
            if(stamp) {
                if(cnt_t0 % (stamp/5) == 0) {
                    if(!g24_is_monitor_master_mic_connected()) {
                        logi("heartbreak trig one......");
                        monitor_user_cmd_send_set(0, TRUE, MONITOR_USER_CMD_HEARTBREAK);
                    }
                }
            }
        }
    }
#endif
#endif
#elif G24_FUNC_ROLE == 1    // dongle

#endif
#endif

#if DEBUG_RUNTIME_PRINT
    if (cnt_t0 % 600 == 0) {
        //3000ms
        event_put(EVENT_DEBUG, NULL, 0);
    }
#endif

#if G24_MONITOR_EN && G24_FUNC_ROLE == 1    // dongle
    static uint16_t kick_count = 0;
    if(broadcast_code == ALL_PAIR)
    {
        kick_count++;
        if(kick_count % CNT_PAIR == 0)
        {
            kick_count = 0;
            broadcast_code = 0x00;
        }
    }
#endif
}

AT_SRAM(.timer)
void timer1_cb(void)
{
   // logi("%s: %u.", __func__, HAL_timestamp_to_ms(HAL_get_timestamp()));
#if (SENSOR_PROXIMITY_EN && (((SENSOR_PROXIMITY_DEVICE_SEL == 1) && (SENSOR_PROXIMITY_DETECT_MODE == 0))))
      sensor_proximity_lt_timer();
#endif
#if LED_PWM_EN
    app_pwm_timer_cb();
#endif

}

AT_SRAM(.timer)
void timer2_cb(void)
{
#if MYME_M4_BOARD
    key_led_timer_proc();
#endif
    //logi("%s: %u.", __func__, HAL_timestamp_to_ms(HAL_get_timestamp()));
}

AT_SRAM(.timer)
void timer3_cb(void)
{
   // logi("%s: %u.", __func__, HAL_timestamp_to_ms(HAL_get_timestamp()));
}

void app_timer_init(void)
{
#if (TIMER0_EN || MOTOR_EN || WATCHDOG_EN || KEY_IO_EN || KEY_IR_EN || \
    (KADC0_EN && !KADC0_IRQ) || (KADC1_EN && !KADC1_IRQ) || (KADC2_EN && !KADC2_IRQ) || \
    (KADC3_EN && !KADC3_IRQ) || (KADC4_EN && !KADC4_IRQ) || (KADC5_EN && !KADC5_IRQ) || \
    (KADC6_EN && !KADC6_IRQ) || (KADC7_EN && !KADC7_IRQ) || (KADC8_EN && !KADC8_IRQ) || (KADC9_EN && !KADC9_IRQ))

    timer_init(TIMER0, TIMER_CLK_HOSC, IRQn_TIMER0_PRIO, TIMER0_PERIOD, true, timer0_cb);

#if WATCHDOG_EN
    HAL_watchdog_init(WATCHDOG_TIMEOUT);
#endif

#endif
#if TIMER1_EN
    logi("timer1 init. ");
    timer_init(TIMER1, TIMER_CLK_HOSC, IRQn_TIMER1_PRIO, TIMER1_PERIOD, true, timer1_cb);
#endif
#if TIMER2_EN
    logi("timer2 init.");
    timer_init(TIMER2, TIMER_CLK_HOSC, IRQn_TIMER2_PRIO, TIMER2_PERIOD, true, timer2_cb);
#endif
#if TIMER3_EN
    logi("timer3 init.");
    timer_init(TIMER3, TIMER_CLK_HOSC, IRQn_TIMER3_PRIO, TIMER3_PERIOD, true, timer3_cb);
#endif

}




