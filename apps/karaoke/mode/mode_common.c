#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "bt_host_api.h"
#include "bt.h"

#include "hal.h"
#include "middleware.h"
#include "karaoke_framework.h"

#include "audio_player.h"
#include "local_play_manager.h"
#include "tws_profile.h"
#include "ble_api.h"
#include "app_pmu.h"
#include "disp.h"
#include "app_rtc.h"
#include "bt_api.h"
#include "bt.h"
#include "tws_profile.h"
#include "air_api.h"
#include "mode_common.h"
#include "ui.h"
#include "g24.h"
#include "g24_api.h"

uint8_t bat_quantity;
uint8_t bat_quantity_last;
bool powerkey_us_flag = false;
bool bt_bat_update_enable = false;

bool send_sw = true;
void Lmic_g24_tx_dry_vol_mute_set(uint8_t mute);
void aux_g24_tx_vol_mute_set(uint8_t mute);
void tone_g24_tx_vol_mute_set(uint8_t mute);


extern void hci_con_release_all(void);
extern void seamless_phone_connected_set(uint8_t phone);
extern void seamless_kick_set();


void mode_common_app_process(uint32_t event)
{
        switch(event)
        {

#if 0//SD_EN
            case DEVICE_EVENT_SD_IN:
                {
                __maybe_unused uint8_t ret;
                __maybe_unused bool fs_scan;
                if (mode_sta_get() == MODE_MUSIC) {
#if MODE_MUSIC_EN
                    ret = local_play_manager_play_new_device(DEV_SDCARD, PLAY_SELECT, 0, true);
                    if (ret < 0) {
                        if (ret == PLAYER_RET_DEV_OFFLINE) {
                            fs_scan = false;
                        } else {
                            fs_scan = true;
                        }
                        local_play_manager_play_new_device(DEV_UDISK, PLAY_SELECT, 0, fs_scan);
                    }
#endif
                } else if (mode_sta_get() != MODE_USBDEV) {
#if MODE_DISKUPDATE_EN
                    mode_set(MODE_DISKUPDATE);
#else
                    mode_set(MODE_RECORD);
#endif
                    mode_switch_reason_set(SW_SDCARD);
                }
               }
                break;

            case DEVICE_EVENT_SD_OUT:
#if MODE_MUSIC_EN
                if ((MODE_MUSIC== mode_sta_get())
                    && (AP_STATUS_PLAYING != player_get_status())
                    && (DEV_SDCARD == device_get_num())) {
                    if (local_play_manager_play_new_device(DEV_UDISK, PLAY_SELECT, 0, true) < 0) {
                        device_set_num(DEV_NONE);
                        mode_switch_auto();
                    }
                }
#endif
                break;

            case DEVICE_EVENT_SD_FAIL:
                logi("sd fail");
                sd_detect_set(SD_DETACH_ENABLE);
                break;
#endif

    case PMU_EVENT_INT:
    case PMU_EVENT_VBUSMIDIN:
    case PMU_EVENT_VBUSMIDOUT:
    case PMU_EVENT_CHARGE_IN:
    case PMU_EVENT_CHARGE_OUT:
    case PMU_EVENT_BAT_FULL:
#if 0
    case PM_CODE_EVENT | PM_POWERON:
    case PM_CODE_EVENT | PM_POWEROFF:
    case PM_CODE_EVENT | PM_SHUTDOWN:
    case PM_CODE_EVENT | PM_DUT:
    case PM_CODE_EVENT | PM_PAIR:
    case PM_CODE_EVENT | PM_UNPAIR:
    case PM_CODE_EVENT | PM_RECOV:
#endif
        pmu_event_process(event);
        break;

    case SYSTEM_EVENT_SECOND:
#if BAT_EXIST
#if RTC_EN
        pmu_timer_cb(NULL);
#endif
#endif
#if KEY_ONOFF_EN
        if (key_onoff_state_get() && HAL_pmu_get_onoff_state()) {
            key_onoff_state_set(false);
        }
#endif
#if CACHE_EN
#if CACHE_MONITOR_EN
        cache_monior_timer();
#endif
#endif

#if RTC_CLOCK_EN
       rtc_time_display();
#endif
        break;


#if BAT_EXIST
    case BT_EVENT_BAT_EN:
        bat_quantity = pmu_get_bat_quantity_percent() / 10;
        //some phone can't support the 10, only support 0~9
        if (bat_quantity) {
            bat_quantity--;
        }
#if MODE_BT_EN
        if(mode_sta_get() == MODE_BT) {
            hsp_hfp_sent_battery_to_phone(bat_quantity);
        }
#endif
        bt_bat_update_enable = true;
        break;

    case BT_EVENT_BAT_UP:
#if BT_AIR_SEL
        if (air_param_phone_type_get()) {
            break;
        }
#endif
        bat_quantity = pmu_get_bat_quantity_percent() / 10;
        //some phone can't support the 10, only support 0~9
        if (bat_quantity) {
            bat_quantity--;
        }
        if (bt_bat_update_enable == true) {
#if MODE_BT_EN
            if(mode_sta_get() == MODE_BT) {
                hsp_hfp_sent_battery_to_phone(bat_quantity);
            }
#endif
        }
        break;
#endif

#if 0
    case KEY_EVENT_SU | KEY_MODE:
        logi("-->>mode key");
#if MODE_KEY_ONOFF
        pmu_delay_poweroff_abort();
        event_put(PM_CODE_EVENT | PM_POWEROFF, NULL, 0);
#else
        mode_switch();
        mode_switch_reason_set(SW_KEY);
#endif
        break;
#endif

#if LINEIN_DETECT_EN
    case DEVICE_EVENT_LINE_IN:
        mode_set(MODE_LINEIN);
        break;
#endif

    case PMU_EVENT_BAT_LOW_WRN:
    {
        const char *tone_name;
        uint8_t tone_vol;
        //logi("lb wrn");
        #if (DISP_EN && STATE_BAT_LOW_EN)
        disp_state(STATE_BAT_LOW);
        #endif
        #if (DISP_EN)
        disp_bat_low_digital(true);
        #endif
        if(am_sys_tone_info_get(SYS_TONE_TYPE_LOW_POWER, &tone_name, &tone_vol)) {
            am_tone_play(tone_name, 1, 0, tone_vol, 0, 0);
        }
        break;
    }

    case PMU_EVENT_BAT_LOW_SHUT:
        mode_switch_reason_set(SW_LB);
        mode_set_force(MODE_POWEROFF);
        break;

    case PMU_EVENT_BAT_UNLOW:
#if (DISP_EN && STATE_BAT_UNLOW_EN)
        disp_state(STATE_BAT_UNLOW);
#endif
        break;

    case PMU_EVENT_POWERKEY_US:
        logi("PMU_EVENT_POWERKEY_US");
        powerkey_us_flag = true;
        break;

    case PMU_EVENT_POWERKEY_SU:
        logi("PMU_EVENT_POWERKEY_SU");
        /*
		//发送 静音/播放 切换
        if(G24_FUNC_ROLE == G24_FUNC_ROLE_HANDSET) {
            if(send_sw) {
                send_sw = false;
                Lmic_g24_tx_dry_vol_mute_set(true);        //有线MIC干声到2.4G发送
                aux_g24_tx_vol_mute_set(true);             //音乐(aux)到2.4G发送
                tone_g24_tx_vol_mute_set(true);            //提示音到2.4G发送
            } else {
                send_sw = true;
                Lmic_g24_tx_dry_vol_mute_set(false);        //有线MIC干声到2.4G发送
                aux_g24_tx_vol_mute_set(false);             //音乐(aux)到2.4G发送
                tone_g24_tx_vol_mute_set(false);            //提示音到2.4G发送
            }
        } 
        */
        break;

    case KEY_EVENT_L | KEY_POWER:
    case PMU_EVENT_POWERKEY_L:
         logi("PMU_EVENT_POWERKEY_L\n");
#if (KEY_ONOFF_EN && (KEY_FUNC_LL_EN || KEY_FUNC_LU_EN))
#else
        if (powerkey_us_flag) {
            /*for avoid enter to poweroff immediately after poweron when long press powerkey.*/
            powerkey_us_flag = false;
#if MODE_G24_EN && !G24_MASTER_RECONNECT_ALWAYS
            if(mode_sta_get() == MODE_G24) {
                if((G24_DEVICE_ROLE == G24_DEVICE_ROLE_SLAVER) && (G24_FUNC_ROLE == G24_FUNC_ROLE_HANDSET)) {
                    uint8_t cmd[2];
                    cmd[0] = G24_CMD_POWEROFF;
                    cmd[1] = 0x00;
                    logi("notify Poweroff\r\n");
                    g24_send_cmd(0, cmd, 2);
                    HAL_msleep(250);        //wait cmd send
                }
            }
#endif
            mode_switch_reason_set(SW_KEY);
            logi("long key shutdown");
            pmu_code_event_process(PM_CODE_EVENT | PM_SHUTDOWN);
        }
#endif
        break;

        case SYSTEM_EVENT_POWEROFF:
#if BT_BLE_EN
        if (!ble_ota_status_get())
#endif
        {
            mode_set_force(MODE_POWEROFF);
        }
        break;

#if 0
       case SYSTEM_EVENT_CON_DIS_TIMEOUT:
#if BLE_OTA_EN
        if (ble_ota_status_get()) {
            break;
        }
#endif
        tws_need_recon_phone_set(0);   /* 关闭回连 */
#if BT_SNIFF_EN
        bt_sniff_enter();
#endif
#if (POWER_OFF_AUTO_WHEN_ANC_DIS == 0)
        mode_switch_reason_set(SW_DISCON);
        if (tws_role_get() != TWS_SLAVE) {
            ui_poweroff();
        }
#endif
        break;
#endif

#if (RTC_ALARM0_EN || RTC_ALARM1_EN)
    case RTC_EVENT_ALM0:
    case RTC_EVENT_ALM1:
        logi("alarm.");
        break;
#endif

#if 0
    case SYSTEM_EVENT_VERSION_ERROR_TIMER_STOP:
#if BT_TWS_EN
        tws_version_timer_stop();
#endif
        break;
#endif
    }
}

uint8_t bat_quantity_get(void)
{
#if DEBUG_EVB_NOBAT
    if (HAL_pmu_is_charging()) {
        //充电中
        return 8;
    } else
#endif
    {
        return bat_quantity;
    }
}

void common_mode_init(void)
{

    common_register_event_handler(mode_common_app_process);

}
