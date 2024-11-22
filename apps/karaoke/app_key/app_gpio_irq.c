#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"

#include "gpio.h"
#include "device.h"
#include "key_io.h"
#include "hal_clk.h"
#include "hal_gpio.h"
#include "sleep.h"
#include "hal_power.h"
#include "sensor.h"
#include "app_key_config.h"
#include "app_device.h"
#include "hal_rtc.h"
#include "console.h"
#include "charge.h"
#include "disp.h"
#include "mode_g24.h"

uint8_t usb_signal_count = 0;
uint32_t usb_edge_irq_time;
void usbd_gpio_sofware_detect_init();
void usbd_gpio_sofware_detect_deinit();

#if (G24_2T4_SHORT_PACKET_3TIMES || G24_2T4_WITH_MONO_MONITOR)
extern bool wait_slaver;
#else
bool wait_slaver = 0;
#endif

AT_SRAM(.gpio)
void gpiob_cb(void)
{
    sleep_delay_set();

	if(wait_slaver){
		INTC_SetPriority(IRQn_GPIOB, 1);
#if (G24_2T4_SHORT_PACKET_3TIMES || G24_2T4_WITH_MONO_MONITOR)  && !G24_2T4_DONGLE_MCHIP
		//g24_2T4_master_slaver_sync_interrupt();
		g24_2T4_1TN_slaver_sync_interrupt();
#endif
	}else{
	INTC_SetPriority(IRQn_GPIOB, IRQn_GPIOB_PRIO);
	
#if SD_DETECT_EN
    if (HAL_gpio_irq_pending_get(SD_DETECT_PIN_NUM)) {
        HAL_gpio_irq_pending_clear(SD_DETECT_PIN_NUM);
    }
#endif

#if USB_DETECT_USE_SOFWARE
    if (HAL_gpio_irq_pending_get(PIN_B22)) {
        logi("p22 irq %d", usb_signal_count);
        HAL_gpio_irq_pending_clear(PIN_B22);
        usb_signal_count++;
        
        if(usb_signal_count > 2) {
            uint32_t tim = HAL_timestamp_to_ms(HAL_get_timestamp());
            if(tim - usb_edge_irq_time > 2000) {
                usb_edge_irq_time = tim ;
                usb_signal_count = 0;
            } else {
                if(usb_signal_count == 30) {
                    logi("pc in....");
                    event_put(USB_EVENT_SOFWWARE_USB_DETECT_PC_IN, 0, 0);
                }
                usb_edge_irq_time = tim ;
            }
        } else {
            usb_edge_irq_time = HAL_timestamp_to_ms(HAL_get_timestamp());
       }
    }
#endif

#if (KEY_IO_EN && KIO_GPIOB)
    kio_get_sta();
#endif

#if KEY_CODING_EN
    kcd_get_sta();
#endif

#if (KADC_SCAN_EN&&SLEEP_EN)
#if KADC0_EN
    if (HAL_gpio_irq_pending_get(KADC0_PIN_NUM)) {
        HAL_gpio_irq_pending_clear(KADC0_PIN_NUM);
    }
#endif
#if KADC1_EN
    if (HAL_gpio_irq_pending_get(KADC1_PIN_NUM)) {
        HAL_gpio_irq_pending_clear(KADC1_PIN_NUM);
    }
#endif
#if KADC2_EN
    if (HAL_gpio_irq_pending_get(KADC2_PIN_NUM)) {
        HAL_gpio_irq_pending_clear(KADC2_PIN_NUM);
    }
#endif
#if KADC3_EN
    if (HAL_gpio_irq_pending_get(KADC3_PIN_NUM)) {
        HAL_gpio_irq_pending_clear(KADC3_PIN_NUM);
    }
#endif
#if KADC4_EN
    if (HAL_gpio_irq_pending_get(KADC4_PIN_NUM)) {
        HAL_gpio_irq_pending_clear(KADC4_PIN_NUM);
    }
#endif
#if KADC5_EN
    if (HAL_gpio_irq_pending_get(KADC5_PIN_NUM)) {
        HAL_gpio_irq_pending_clear(KADC5_PIN_NUM);
    }
#endif
#if KADC6_EN
    if (HAL_gpio_irq_pending_get(KADC6_PIN_NUM)) {
        HAL_gpio_irq_pending_clear(KADC6_PIN_NUM);
    }
#endif
#if KADC7_EN
    if (HAL_gpio_irq_pending_get(KADC7_PIN_NUM)) {
        HAL_gpio_irq_pending_clear(KADC7_PIN_NUM);
    }
#endif
#if KADC8_EN
    if (HAL_gpio_irq_pending_get(KADC8_PIN_NUM)) {
        HAL_gpio_irq_pending_clear(KADC8_PIN_NUM);
    }
#endif
#if KADC9_EN
    if (HAL_gpio_irq_pending_get(KADC9_PIN_NUM)) {
        HAL_gpio_irq_pending_clear(KADC9_PIN_NUM);
    }
#endif
#endif

#if SENSOR_G_EN
    sensor_g_interrupt();
#endif
#if (SENSOR_PROXIMITY_EN && SENSOR_PROXIMITY_DEVICE_SEL == 1)
    sensor_proximity_interrupt();
#endif
#if SENSOR_FORCE_EN
    //sensor_force_interrupt(); //use kio
#endif
	}
}

AT_SRAM(.gpio)
void gpioc_cb(void)
{
    sleep_delay_set();

#if (KEY_IO_EN && KIO_GPIOC)
    kio_get_sta();
#endif

#if PMU_EXIST
    pmu_irq_proc();
#endif
}

AT_SRAM(.gpio)
void gpiod_cb(void)
{
    sleep_delay_set();
#if (KEY_IO_EN && KIO_GPIOD)
    kio_get_sta();
#endif

#if (SENSOR_PROXIMITY_EN && SENSOR_PROXIMITY_DETECT_MODE == 1)
    sensor_proximity_interrupt();
#endif

#if SENSOR_MOTION_EN
    sensor_motion_interrupt();
#endif
#if SENSOR_G_EN
    sensor_g_interrupt();
#endif


}

#if SLEEP_EN
uint32_t rtc_time;

AT_SRAM(.sleep1)
void sleep_wakeup_enable(void)
{
#if SLEEP_EN
#if KADC0_EN
    #if KADC0_IRQ
        HAL_kadc_wakeup_set(KADC_ID_00, true);
    #else
        HAL_gpio_wakeup_set(KADC0_PIN_NUM, true);
        HAL_gpio_irq_config(KADC0_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
    #endif
#endif
#if KEY_AD_EN && KADC1_EN
    #if KADC1_IRQ
        HAL_kadc_wakeup_set(KADC_ID_01, true);
    #else
        HAL_gpio_wakeup_set(KADC1_PIN_NUM, true);
        HAL_gpio_irq_config(KADC1_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
    #endif
#endif
#if KEY_AD_EN && KADC2_EN
    #if KADC2_IRQ
        HAL_kadc_wakeup_set(KADC_ID_02, true);
    #else
        HAL_gpio_wakeup_set(KADC2_PIN_NUM, true);
        HAL_gpio_irq_config(KADC2_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
    #endif
#endif
#if KEY_AD_EN && KADC3_EN
    #if KADC3_IRQ
        HAL_kadc_wakeup_set(KADC_ID_03, true);
    #else
        HAL_gpio_wakeup_set(KADC3_PIN_NUM, true);
        HAL_gpio_irq_config(KADC3_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
    #endif
#endif
#if KEY_AD_EN && KADC4_EN
    #if KADC4_IRQ
        HAL_kadc_wakeup_set(KADC_ID_04, true);
    #else
        HAL_gpio_wakeup_set(KADC4_PIN_NUM, true);
        HAL_gpio_irq_config(KADC4_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
    #endif
#endif
#if KEY_AD_EN &&KADC5_EN
    #if KADC5_IRQ
        HAL_kadc_wakeup_set(KADC_ID_05, true);
    #else
        HAL_gpio_wakeup_set(KADC5_PIN_NUM, true);
        HAL_gpio_irq_config(KADC5_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
    #endif
#endif
#if KEY_AD_EN &&KADC6_EN
    #if KADC6_IRQ
        HAL_kadc_wakeup_set(KADC_ID_06, true);
    #else
        HAL_gpio_wakeup_set(KADC6_PIN_NUM, true);
        HAL_gpio_irq_config(KADC6_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
    #endif
#endif
#if KEY_AD_EN &&KADC7_EN
    #if KADC7_IRQ
        HAL_kadc_wakeup_set(KADC_ID_07, true);
    #else
        HAL_gpio_wakeup_set(KADC7_PIN_NUM, true);
        HAL_gpio_irq_config(KADC7_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
    #endif
#endif
#if KEY_AD_EN &&KADC8_EN
    #if KADC8_IRQ
        HAL_kadc_wakeup_set(KADC_ID_08, true);
    #else
        HAL_gpio_wakeup_set(KADC8_PIN_NUM, true);
        HAL_gpio_irq_config(KADC8_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
    #endif
#endif
#if KEY_AD_EN &&KADC9_EN
    #if KADC9_IRQ
        HAL_kadc_wakeup_set(KADC_ID_09, true);
    #else
        HAL_gpio_wakeup_set(KADC9_PIN_NUM, true);
        HAL_gpio_irq_config(KADC9_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
    #endif
#endif


#if KEY_IO_EN
#ifdef PIN_KIO0
    HAL_gpio_wakeup_set(PIN_KIO0, true);
#endif
#ifdef PIN_KIO1
    HAL_gpio_wakeup_set(PIN_KIO1, true);
#endif
#ifdef PIN_KIO2
    HAL_gpio_wakeup_set(PIN_KIO2, true);
#endif
#ifdef PIN_KIO3
    HAL_gpio_wakeup_set(PIN_KIO3, true);
#endif
#ifdef PIN_KIO4
    HAL_gpio_wakeup_set(PIN_KIO4, true);
#endif
#ifdef PIN_KIO5
    HAL_gpio_wakeup_set(PIN_KIO5, true);
#endif
#ifdef PIN_KIO6
    HAL_gpio_wakeup_set(PIN_KIO6, true);
#endif
#ifdef PIN_KIO7
    HAL_gpio_wakeup_set(PIN_KIO7, true);
#endif
#endif

#if (SYS_PLATFORM == PLATFORM_CHIP)
    HAL_pmu_set_wakeup(true);
#endif

#if RTC_EN
    HAL_rtc_enable_wakeup(0x7);
#endif

#if KEY_IR_EN
    HAL_clk_set_src(CLK_IR, CLK_LOSC);
    HAL_irrx_set_wakeup(true);
#endif

#if SD_DETECT_EN && SD_EN
    sd_set_wakeup(true);
#endif

#if KEY_DIG_EN
#if KDIG0_EN
    HAL_clk_set_src(CLK_DKC, CLK_LOSC);
    HAL_kdig_wakeup_set(KDIG_CH0,KDIG_DOWN_WKUP_EN|KDIG_UP_WKUP_EN|KDIG_HOLD_WKUP_EN|KDIG_LONG_WKUP_EN|KDIG_MC_WKUP_EN,ENABLE);
#endif
#if KDIG1_EN
    HAL_clk_set_src(CLK_DKC, CLK_LOSC);
    HAL_kdig_wakeup_set(KDIG_CH1,KDIG_DOWN_WKUP_EN|KDIG_UP_WKUP_EN|KDIG_HOLD_WKUP_EN|KDIG_LONG_WKUP_EN|KDIG_MC_WKUP_EN,ENABLE);
#endif
#if KDIG2_EN
    HAL_clk_set_src(CLK_DKC, CLK_LOSC);
    HAL_kdig_wakeup_set(KDIG_CH2,KDIG_DOWN_WKUP_EN|KDIG_UP_WKUP_EN|KDIG_HOLD_WKUP_EN|KDIG_LONG_WKUP_EN|KDIG_MC_WKUP_EN,ENABLE);
#endif
#endif
    HAL_audiopll_sleep();
    uart_suspend(1);
    //swd_suspend(1);
#endif

}

AT_SRAM(.sleep1)
void sleep_wakeup_disable(void)
{
#if SLEEP_EN
    RTC_TimeTypeDef rtc;

    HAL_rtc_get_time(&rtc);
    rtc_time = rtc.hour * 3600 + rtc.minute * 60 + rtc.second;

    //swd_suspend(0);
    uart_suspend(0);
#if KEY_AD_EN && KADC0_EN
    #if KADC0_IRQ
        HAL_kadc_wakeup_set(KADC_ID_00, false);
    #else
        HAL_gpio_irq_config(KADC0_PIN_NUM, PIN_IRQ_DIS);
        HAL_gpio_wakeup_set(KADC0_PIN_NUM, false);
    #endif
#endif
#if KEY_AD_EN && KADC1_EN
    #if KADC1_IRQ
        HAL_kadc_wakeup_set(KADC_ID_01, false);
    #else
        HAL_gpio_irq_config(KADC1_PIN_NUM, PIN_IRQ_DIS);
        HAL_gpio_wakeup_set(KADC1_PIN_NUM, false);
    #endif
#endif
#if KEY_AD_EN && KADC2_EN
    #if KADC2_IRQ
        HAL_kadc_wakeup_set(KADC_ID_02, false);
    #else
        HAL_gpio_irq_config(KADC2_PIN_NUM, PIN_IRQ_DIS);
        HAL_gpio_wakeup_set(KADC2_PIN_NUM, false);
    #endif
#endif
#if KEY_AD_EN && KADC3_EN
    #if KADC3_IRQ
        HAL_kadc_wakeup_set(KADC_ID_03, false);
    #else
        HAL_gpio_irq_config(KADC3_PIN_NUM, PIN_IRQ_DIS);
        HAL_gpio_wakeup_set(KADC3_PIN_NUM, false);
    #endif
#endif
#if KEY_AD_EN && KADC4_EN
    #if KADC4_IRQ
        HAL_kadc_wakeup_set(KADC_ID_04, false);
    #else
        HAL_gpio_irq_config(KADC4_PIN_NUM, PIN_IRQ_DIS);
        HAL_gpio_wakeup_set(KADC4_PIN_NUM, false);
    #endif
#endif
#if KEY_AD_EN && KADC5_EN
    #if KADC5_IRQ
        HAL_kadc_wakeup_set(KADC_ID_05, false);
    #else
        HAL_gpio_irq_config(KADC5_PIN_NUM, PIN_IRQ_DIS);
        HAL_gpio_wakeup_set(KADC5_PIN_NUM, false);
    #endif
#endif
#if KEY_AD_EN && KADC6_EN
    #if KADC6_IRQ
        HAL_kadc_wakeup_set(KADC_ID_06, false);
    #else
        HAL_gpio_irq_config(KADC6_PIN_NUM, PIN_IRQ_DIS);
        HAL_gpio_wakeup_set(KADC6_PIN_NUM, false);
    #endif
#endif
#if KEY_AD_EN && KADC7_EN
    #if KADC7_IRQ
        HAL_kadc_wakeup_set(KADC_ID_07, false);
    #else
        HAL_gpio_irq_config(KADC7_PIN_NUM, PIN_IRQ_DIS);
        HAL_gpio_wakeup_set(KADC7_PIN_NUM, false);
    #endif
#endif
#if KEY_AD_EN && KADC8_EN
    #if KADC8_IRQ
        HAL_kadc_wakeup_set(KADC_ID_08, false);
    #else
        HAL_gpio_irq_config(KADC8_PIN_NUM, PIN_IRQ_DIS);
        HAL_gpio_wakeup_set(KADC8_PIN_NUM, false);
    #endif
#endif
#if KEY_AD_EN && KADC9_EN
    #if KADC9_IRQ
        HAL_kadc_wakeup_set(KADC_ID_09, false);
    #else
        HAL_gpio_irq_config(KADC9_PIN_NUM, PIN_IRQ_DIS);
        HAL_gpio_wakeup_set(KADC9_PIN_NUM, false);
    #endif
#endif

#if KEY_IO_EN
#ifdef PIN_KIO0
    HAL_gpio_wakeup_set(PIN_KIO0, false);
#endif
#ifdef PIN_KIO1
    HAL_gpio_wakeup_set(PIN_KIO1, false);
#endif
#ifdef PIN_KIO2
    HAL_gpio_wakeup_set(PIN_KIO2, false);
#endif
#ifdef PIN_KIO3
    HAL_gpio_wakeup_set(PIN_KIO3, false);
#endif
#ifdef PIN_KIO4
    HAL_gpio_wakeup_set(PIN_KIO4, false);
#endif
#ifdef PIN_KIO5
    HAL_gpio_wakeup_set(PIN_KIO5, false);
#endif
#ifdef PIN_KIO6
    HAL_gpio_wakeup_set(PIN_KIO6, false);
#endif
#ifdef PIN_KIO7
    HAL_gpio_wakeup_set(PIN_KIO7, false);
#endif
#endif

#if (SYS_PLATFORM == PLATFORM_CHIP)
    HAL_pmu_set_wakeup(false);
#endif

#if RTC_EN
    HAL_rtc_disable_wakeup(0x7);
#endif

#if KEY_IR_EN
    HAL_clk_set_src(CLK_IR, CLK_32K_HOSC);
    HAL_irrx_set_wakeup(false);
#endif

#if SD_DETECT_EN && SD_EN
    sd_set_wakeup(false);
#endif

#if KEY_DIG_EN
#if KDIG0_EN
    HAL_clk_set_src(CLK_DKC, CLK_32K_HOSC);
    HAL_kdig_wakeup_set(KDIG_CH0,KDIG_DOWN_WKUP_EN|KDIG_UP_WKUP_EN|KDIG_HOLD_WKUP_EN|KDIG_LONG_WKUP_EN|KDIG_MC_WKUP_EN,DISABLE);
#endif
#if KDIG1_EN
    HAL_clk_set_src(CLK_DKC, CLK_32K_HOSC);
    HAL_kdig_wakeup_set(KDIG_CH1,KDIG_DOWN_WKUP_EN|KDIG_UP_WKUP_EN|KDIG_HOLD_WKUP_EN|KDIG_LONG_WKUP_EN|KDIG_MC_WKUP_EN,DISABLE);
#endif
#if KDIG2_EN
    HAL_clk_set_src(CLK_DKC, CLK_32K_HOSC);
    HAL_kdig_wakeup_set(KDIG_CH2,KDIG_DOWN_WKUP_EN|KDIG_UP_WKUP_EN|KDIG_HOLD_WKUP_EN|KDIG_LONG_WKUP_EN|KDIG_MC_WKUP_EN,DISABLE);
#endif
#endif

    if (charge_mode_sta_get()) {
        event_put(SYSTEM_EVENT_EXIT_SLEEP_MODE, NULL, 0);
    }
#endif

}
#endif

void rtc_time_wait_update(void)
{
#if SLEEP_EN
    RTC_TimeTypeDef rtc;
    uint32_t rtc_cur;

    do {
        HAL_rtc_get_time(&rtc);
        rtc_cur = rtc.hour * 3600 + rtc.minute * 60 + rtc.second;
    } while(rtc_cur == rtc_time);
#endif
}

void app_gpio_irq_en(void)
{
#if (MODE_BT_EN || SD_DETECT_EN || KIO_GPIOB || KEY_CODING_EN || (KADC_SCAN_EN&&SLEEP_EN) \
    || SENSOR_G_EN || SENSOR_PROXIMITY_EN  || ((G24_2T4_SHORT_PACKET_3TIMES || G24_2T4_WITH_MONO_MONITOR) && !G24_2T4_DONGLE_MCHIP))
    gpiob_irq_en(gpiob_cb);
#endif

#if (PMU_EXIST || KIO_GPIOC)
    gpioc_irq_en(gpioc_cb);
#endif

#if (KIO_GPIOD || SENSOR_PROXIMITY_EN||SENSOR_G_EN )
    gpiod_irq_en(gpiod_cb);
#endif


 #if SLEEP_EN
    sleep_register_app_callback(SLEEP_EN, SLEEP_DELAY, sleep_wakeup_enable, sleep_wakeup_disable);
 #else
    sleep_register_app_callback(SLEEP_EN, SLEEP_DELAY, NULL, NULL);
 #endif
}

void usbd_gpio_sofware_detect_init()
{
    HAL_gpio_config(PIN_B22 | PIN_FUNCEINT | PIN_PULL_UP2K);
    HAL_gpio_irq_clk_config(PIN_B22, (0x1<<8)|(1<<7)|(0x07<<4));
    HAL_gpio_irq_config(PIN_B22, PIN_IRQ_EN | PIN_IRQ_F_EDGE);
    app_gpio_irq_en();
}

void usbd_gpio_sofware_detect_deinit()
{
    HAL_gpio_irq_config(PIN_B22, PIN_IRQ_DIS);
    HAL_gpio_config(PIN_B22 | PIN_FUNCINPUT);
}



