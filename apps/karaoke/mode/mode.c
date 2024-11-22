
#define LOG_LEV 4
#include "sys_inc.h" // libsys.a
#include "oal.h" // liboal.a
#include "hal.h" //libhal.a
#include "sltk_mem.h"

#include "system.h" //libmuddleware.a
#include "user_config.h"

#include "mode_manager.h"
#include "console.h"
#include "mode.h"
#include "bt.h"

#include "hal.h"
#include "middleware.h"
#include "karaoke_framework.h"

#include "local_play_manager.h"
#include "app_audio.h"
#include "bt_api.h"
#include "bt.h"
#include "g24.h"
#include "bt_update.h"
#include "bt_host_api.h"
#include "app_device.h"
#include "mode_common.h"
#include "mode_bt.h"
#include "mode_g24.h"
#include "g24_api.h"
#include "app_pmu.h"
#include "disp.h"
#include "sensor_proximity.h"
#include "twi.h"
#include "app_psm.h"
#include "app_lcdc.h"
#include "mode_idle.h"

void mode_update_firmware();
void mode_fcc();
void mode_afc();
void g24_addr_init();

mode_info_t mode_support[] = {

#if MODE_BT_EN || BT_DUT_EN
    {MODE_BT, mode_bt},
#endif

#if MODE_G24_EN
    {MODE_G24, mode_g24},
#endif

#if MODE_IDLE_EN
    {MODE_IDLE, mode_idle},
#endif

#if MODE_USBDEV_EN
    {MODE_USBDEV, mode_usbdev},
#endif

#if MODE_MUSIC_EN
    {MODE_MUSIC, mode_music },

#endif

    //NOTE:normal scene mode will insert in before the MODE_SCENE_MAX
    {MODE_SCENE_MAX, NULL},

#if MODE_CHARGE_EN
    {MODE_CHARGE, mode_charge},
#endif

#if MODE_POWEROFF_EN
    {MODE_POWEROFF, mode_poweroff},
#endif


#if MODE_BTUPDATE_EN
    {MODE_BTUPDATE, mode_btupdate},
#endif

#if BT_FCC_TEST_EN
    {MODE_FCC, mode_fcc},
#endif

#if MODE_AFC_EN
    {MODE_AFC, mode_afc},
#endif

     {MODE_RESET, NULL},
    //NOTE:system scene mode will insert in bettwin the MODE_SCENE_MAX and the MODE_SYSTEM_MAX
    {MODE_SYSTEM_MAX, NULL},
};

#define MODE_TOTAL_NUM  (sizeof(mode_support)/sizeof(mode_support[0]))




void mode_init(void)
{
    mode_register(mode_support, MODE_TOTAL_NUM);
    __mode__common();
#if (SENSOR_EN && SENSOR_PROXIMITY_EN && (SENSOR_PROXIMITY_DEVICE_SEL == 2||SENSOR_PROXIMITY_DEVICE_SEL == 4))
#if (TWI1_EN || TWI2_EN)
     logi("init twi");
     twi_init();
#endif
    sensor_proximity_mode_init();
#if (TWI1_EN || TWI2_EN)
     logi("deinit twi");
     twi_deinit();
#endif

#endif

#if BT_FCC_TEST_EN
    mode_set_force(MODE_FCC);
#else

#if CHARGING_BIN
    uint8_t work_mode;
    work_mode_get(&work_mode);
    if(work_mode != 0 && !MODE_AFC_EN) {
        if(work_mode == WORK_MODE_SET_G24) {
#if MODE_G24_EN
            mode_set_force(MODE_G24);
#endif    
        } else if(work_mode == WORK_MODE_SET_BT) {
#if MODE_BT_EN        
            mode_set_force(MODE_BT);
#endif            
        } 
        //清除保存的模式记录
        work_mode_set(WORK_MODE_NONE);
    }

#if CHARGING_BIN_CODE
    if (pmu_bootup_vbus()) {
        mode_set_force(MODE_BT);
    } else {
        mode_set_force(MODE_CHARGE);
    }
#else
    if (pmu_bat_is_charging()) {
        mode_set_force(MODE_CHARGE);
    } else {
#if MODE_G24_EN 
        mode_set_force(MODE_G24);
#elif MODE_BT_EN
        mode_set_force(MODE_BT);
#elif MODE_USBDEV_EN
        mode_set_force(MODE_USBDEV_EN);
#else 
        mode_set_force(MODE_G24);
#endif
    }
#if 0//CHARGING_BIN_MID_LEVEL
    if (HAL_pmu_get_vbus_mid_state()) {
        mode_set_force(MODE_CHARGE);
    }
#endif//CHARGING_BIN_MID_LEVEL
#endif//CHARGING_BIN_CODE

    if(nor_mp_dut_flag_get()) {
        mode_set_force(MODE_BT);
    }

    if(mode_sta_get() == MODE_BT) {
#if MODE_BT_EN        
        logi("load bt code--");
        as_overlay_code_load(OVERLAY_TYPE_BT);
#endif        
    } else if(mode_sta_get() == MODE_G24) {
#if MODE_G24_EN        
        logi("load g24 code --");
        as_overlay_code_load(OVERLAY_TYPE_G24);
#endif
    } else if(mode_sta_get() == MODE_USBDEV) {
#if MODE_USBDEV_EN
        logi("load usbdev code --");
        as_overlay_code_load(OVERLAY_TYPE_USBDEV);
#endif
    }

#else   //#if CHARGING_BIN

//务必在确认G24模式或者BT模式加载G24 overlap代码
    uint8_t work_mode;
    work_mode_get(&work_mode);
    if(work_mode != 0 && !MODE_AFC_EN) {
        if(work_mode == WORK_MODE_SET_G24) {
#if MODE_G24_EN
            mode_set_force(MODE_G24);
            logi("load g24 code --");
            as_overlay_code_load(OVERLAY_TYPE_G24);
#endif    
        } else if(work_mode == WORK_MODE_SET_BT) {
#if MODE_BT_EN        
            mode_set_force(MODE_BT);
            logi("load bt code--");
            as_overlay_code_load(OVERLAY_TYPE_BT);
#endif            
        }
        work_mode_set(WORK_MODE_NONE);
    } else {
        //----->>>
#if MODE_G24_EN
    if(!nor_mp_dut_flag_get()) {
        mode_set_force(MODE_G24);
        logi("load g24 code ------");
        as_overlay_code_load(OVERLAY_TYPE_G24);
    } else {
        logi("mode bt dut set---");
        mode_set_force(MODE_BT);
        as_overlay_code_load(OVERLAY_TYPE_BT);
    }
#elif MODE_BT_EN
    mode_set_force(MODE_BT);
    logi("load bt code ------");

    as_overlay_code_load(OVERLAY_TYPE_BT);
#elif MODE_AFC_EN
    mode_set_force(MODE_AFC);
#endif

    }
//<<<--------------------------

#endif//CHARGING_BIN

#endif//BT_FCC_TEST_EN

#if MODE_BT_EN
    //rf_para_dft();//FIXME:chenjian
    if(mode_sta_get() == MODE_BT) {
        bt_ll_init();
    }
#endif

#if POWER_OFF_FIRST_EN
    bool startup_flag;
    startup_flag = startup_is_first();
#endif

    mode_switch_reason_set(SW_INIT);
    mode_last_sta_set(MODE_NULL);

    am_volume_all_info_get(VOLUME_DEFAULT);

    if (pmu_is_enter_charge_mode()) {
        mode_set_force(MODE_CHARGE);
        return;
    }

#if (G24_2T4_SHORT_PACKET_3TIMES || G24_2T4_WITH_MONO_MONITOR)
    if(G24_FUNC_ROLE == G24_FUNC_ROLE_DONGLE) {
#if G24_2T4_DONGLE_MCHIP
        if (pmu_vbus_is_in()) {
            mode_set_force(MODE_CHARGE);
            return;
        }
#else
        if (pmu_vbus_is_in()) {
            while(1) {
                sys_event_info_t info;
                event_get(info.params, &info.params_len, true);
                HAL_msleep(10);
            }
        }
#endif
    }
#endif

#if POWER_OFF_FIRST_EN
     if (startup_flag) {
       //scene_mode->switch_reason = SW_STARTUP_FIRST;
        mode_switch_reason_set(SW_STARTUP_FIRST);
        mode_set_force(MODE_POWEROFF);
    } else
#endif
     {
     
#if LCDC_EN && EVB_LCD_DEMO                        //显示开机图片
        lcd_poweron_show(0);
        HAL_mdelay(30);
        LCD_BACKLIGHT_ON();
#endif

#if (DISP_EN && STATE_POWERON_EN)
        disp_state(STATE_POWERON);
#endif
        app_audio_init();
#if (TONE_EN && TONE_WELCOME_EN)
        am_tone_play(SYS_TONE_WELCOME_NAME, 0, 0, TONE_WELCOME_VOL, 0, 0);//播放开机提示音
#else
        {
            uint32_t start_time = HAL_timestamp_to_ms(HAL_get_timestamp());
            while(HAL_timestamp_to_ms(HAL_get_timestamp()) - start_time < 50);
        }
#endif

      }

    device_startup_set(1);
#if MODE_G24_EN
    if(mode_get_sta() == MODE_G24) {
        g24_monitor_init();
#if G24_MONITOR_EN
        app_g24_monitor_init();
#endif
    }
#endif
#if BT_DUT_EN
    if(mode_sta_get() == MODE_G24) {
        g24_addr_init();
    } else {
    }
#endif
}

extern char OVERLAY_SRAM_ADDR;
extern char __load_start_g24_sec, __load_stop_g24_sec;
extern char __load_start_bt_sec, __load_stop_bt_sec;
//extern char __load_start_usbdev_sec, __load_stop_usbdev_sec;

void as_overlay_code_load(enum overlay_type_e overlay_type)
{
#if 1
    uint32_t total_size = (uint32_t)(&__load_stop_g24_sec - &__load_start_g24_sec);
    uint32_t ram_addr = (uint32_t)&OVERLAY_SRAM_ADDR;
    uint32_t flash_addr = BASE_FLASH_ADDR + (uint32_t)&__load_start_g24_sec;

    switch (overlay_type) {
    case OVERLAY_TYPE_BT:
        total_size = (uint32_t)(&__load_stop_bt_sec - &__load_start_bt_sec);
        ram_addr = (uint32_t)&OVERLAY_SRAM_ADDR;
        flash_addr = BASE_FLASH_ADDR + (uint32_t)&__load_start_bt_sec;
        break;
    case OVERLAY_TYPE_USBDEV:
        //total_size = (uint32_t)(&__load_stop_usbdev_sec - &__load_start_usbdev_sec);
        //ram_addr = (uint32_t)&OVERLAY_SRAM_ADDR;
        //flash_addr = BASE_FLASH_ADDR + (uint32_t)&__load_start_usbdev_sec;
        break;

    default:
        break;
    }

    while (total_size) {
        uint32_t r_bytes = total_size;//MIN(OVERLAY_CODE_LOAD_BLOCK_MAX_SIZE, total_size);
        HAL_norflash_read(ram_addr, flash_addr, r_bytes);
        ram_addr += r_bytes;
        flash_addr += r_bytes;
        total_size -= r_bytes;
    }
#endif    
}

