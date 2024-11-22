#define LOG_LEV 4
#include "sys_inc.h" // libsys.a
#include "oal.h" // liboal.a
#include "hal.h" //libhal.a
#include "sltk_mem.h"

#include "system.h" //libmuddleware.a
#include "mode_manager.h"
#include "console.h"
#include "user_config.h"
#include "bt_host_api.h"
#include "info_mem.h"
#include "tws_profile.h"
#include "bt_api.h"
#include "bt.h"
#include "ui.h"
#include "ble_profile.h"
#include "ble_api.h"
#include "tone_play_manager.h"
#include "bt_audio_manager.h"
#include "tws_api.h"
#include "bt_audio_service.h"
#include "app_pmu.h"
#include "mixer.h"
#include "event.h"
#include "disp.h"
#include "key.h"
#include "rtc.h"
#include "pms.h"
#include "sleep.h"
#include "anc_service.h"
#include "ble52_api.h"
#include "psm_manager.h"
#include "app_audio_anc.h"
#include "kws_manager.h"
#include "mode_bt.h"
#include "sys_version.h"
#include "bt_manager.h"
#include "app_psm.h"


#if MODE_BT_EN || BT_DUT_EN

#define BLE52_STAND_ALONE 1

bool ble_app_run;
bool mp_task_run = false;
bool bt_tws_task_run = false;

#if REC_POWEROFF_TIME
volatile uint64_t poweroff_time;
#endif


void bt_rf_power_force(uint8_t enable, uint8_t power)
{
    //rf_power_force(enable, power);//FIXME:chenjian
}


void bt_rf_maxpower_set(uint8_t enable, uint8_t power)
{
    if(enable==1){
        bt_rf_power_max_set(power);
    }
}


uint8_t bt_addr_default[] = BT_ADDR_DEFAULT;
uint8_t bt_name_default[] = BT_NAME_DEFAULT;

void bt_addr_init()
{
    uint8_t bt_addr[6] = {0};
    //nor_bt_addr_get(bt_addr);
    sys_info_read(BT_ADDR_INFO, bt_addr, 6);

    if (((bt_addr[0]==0x00)&&(bt_addr[1]==0x00)&&(bt_addr[2]==0x00)&&(bt_addr[3]==0x00)&&(bt_addr[4]==0x00)&&(bt_addr[5]==0x00)) || \
        ((bt_addr[0]==0xFF)&&(bt_addr[1]==0xFF)&&(bt_addr[2]==0xFF)&&(bt_addr[3]==0xFF)&&(bt_addr[4]==0xFF)&&(bt_addr[5]==0xFF))) {
            libc_memcpy(bt_addr, bt_addr_default, 6);
/* on FPGA platform, the trng is fake */
#if (SYS_PLATFORM == PLATFORM_CHIP)
#if BT_ADDR_USE_RANDOM
        //libc_memcpy(bt_addr, &random_num, 4);
        libc_memcpy(bt_addr, &bt_addr_default, 4);//temp
#endif
#endif
        logi("bt addr:%02x:%02x:%02x:%02x:%02x:%02x",
                bt_addr[5],bt_addr[4],bt_addr[3],bt_addr[2],bt_addr[1],bt_addr[0]);

    } else {
    }

    bt_addr_set(bt_addr);
}

void bt_name_init()
{
    uint8_t bt_name[32] = {0};
    uint8_t len;


    uint8_t offset;
    int ret;
    ret = sys_info_read(BT_NAME_INFO, bt_name, BT_NAME_MAX);

    if (ret < 0) {
        libc_memcpy(bt_name, bt_name_default, sizeof(bt_name_default));  //包含结束字符
        len = sizeof(bt_name_default) - 1;     //长度中不包含结束字符
    } else {
        len = strlen((char*)bt_name);
    }

    if (len == 0){
        libc_memcpy(bt_name, bt_name_default, sizeof(bt_name_default));  //包含结束字符
        len = sizeof(bt_name_default) - 1;     //长度中不包含结束字符
    }
    if (bt_testmode_get() == 1) {
        offset = len;
        bt_name[offset+0] = '-';
        bt_name[offset+1] = 'D';
        bt_name[offset+2] = 'U';
        bt_name[offset+3] = 'T';
        bt_name[offset+4] = '\0';
        len += 4;
    }
    bt_name_set(bt_name, len);

    logi("bt name init:%s len:%d", bt_name,len);
}

void bt_profile_init()
{
    uint16_t bt_profile_select = PROF_A2DP;
#if BT_PHONE_EN
    bt_profile_select |= PROF_HANDSFREE;
#endif
#if BT_SPP_EN
    bt_profile_select |= PROF_SPP;
#endif
#if BT_HID_EN
    bt_profile_select |= 0x60;
#endif
#if BT_BLE_EN
    bt_profile_select |= PROF_BLE;
#endif
#if BT_TWS_EN
    bt_profile_select |= PROF_TWS;
#endif
#if BT_AIR_SEL
    bt_profile_select |= air_profile_set();
#endif
    btstack_profile_set(bt_profile_select);
}


void bt_debug_port_config(void)
{

#if (BASE_BOARD_CONFIG == 1)
         *((uint32_t*)0x400810c0) &= 0xf0f000ff;
         *((uint32_t*)0x400810c0) |= 0x010dd100; //PD3,PD4
         *((uint32_t *)(0x40021040)) |= 0xc0008006;
         *((uint32_t*)0x40081400) = 0x80000002;
#else
        *((uint32_t*)0x40081048) &= 0x00ffffff;
        *((uint32_t*)0x40081048) |= 0xdd000000; //PB22,23
        *((uint32_t *)(0x40021040)) |= 0xc0008000;
        *((uint32_t*)0x40081400) = 0x80000002;

#endif
#if 0 //for sl6801 ver
        *((uint32_t*)0x40081040) &= 0xffffffff;
        *((uint32_t*)0x40081044) &= 0xffffffff;
        *((uint32_t*)0x40081048) &= 0xff00ffff;
        *((uint32_t*)0x40081040) |= 0x00000000;
        *((uint32_t*)0x40081044) |= 0x00000000;
        *((uint32_t*)0x40081048) |= 0x00dd0000; //PB20,21
        *((uint32_t*)0x40020050) = 0xa3;
        *((uint32_t*)0x40081400) = 0x80000002;
#endif
}

extern uint32_t mode_bt_cnt, m_con_cnt, m_con_cnt_2, m_con_cnt_5, m_con_cnt_10, s_con_cnt, s_con_cnt_2, s_con_cnt_5, s_con_cnt_10, phone_con_cnt, phone_con_cnt_5, phone_con_cnt_10;

bool bt_onoff_sta;

void bt_onoff_set(bool onoff, uint8_t step)
{
    if(onoff) {
        if(!bt_scan_enable_get()) {
            hci_write_scan_enable(0x03);
            bt_scan_enable_set(1);
            bt_recon_phone_en_set(1);
        }
    } else {
        if(step == 0) {
            if(bt_con_flag){
               if(bt_disconnect_direct()){
                    for(uint8_t wcnt=0;wcnt<100;wcnt++){
                        if(bt_connect_link_n()==0){
                            logi("wait dis %d.", wcnt);
                            break;
                        }else{
#if BT_SNIFF_EN
                            HAL_msleep(30);
#else
                            HAL_msleep(10);
#endif
                        }
                    }
                } else {
                    hci_write_scan_enable(0);
                    bt_scan_enable_set(0);
                }
            } else {
                hci_write_scan_enable(0);
                bt_scan_enable_set(0);
            }
        } else if(step == 1) {
            hci_write_scan_enable(0);
            bt_scan_enable_set(0);
        }
        
    }
}

void bt_onoff_sw(void)
{
    bt_onoff_sta = !bt_onoff_sta;
    bt_onoff_set(bt_onoff_sta, 0);
    logi("bt_onoff_sta %d", bt_onoff_sta);
}

void mode_bt_app_process(sys_event_info_t *info)
{
#if WATCHDOG_EN
    HAL_watchdog_refresh(NULL);
#endif

    uint32_t event = info->event;
    switch(event) {
        case BT_EVENT_TWS_PHONE_DISCONNECTED:
            {
                uint32_t reason;
                memcpy(&reason ,info->params,4);
                logi("!!dis reason 0x%x",reason);
                pmu_set_discon_time(rtc_get_second());
            }
            break;

        case BT_EVENT_TWS_PHONE_CONNECTED:
            pmu_set_discon_time(0xffffffff);
            break;

        case BT_EVENT_CONNECTED:
            logi("bt con");
            link_info_save();
            break;

        case BT_EVENT_DISCONNECT:
            logi("bt discon");
            if (am_incoming_tone_playing_status_get()) {
                am_incoming_tone_stop(0, 0);
            }
            if(!bt_onoff_sta) {
                bt_onoff_set(bt_onoff_sta, 1);
            }
            break;
            
        default:
            ui_process(event, info->params_len, info->params);
            break;
    }
}

void ble_customer_init();

void ble_app_init()
{
#if BLE_AIR_SEL
    if (air_en_get()==0x01) {
        ble_air_version_set(BLE_AIR_SEL);
        ble_air_init();
    } else {
    #if BLE_APP_EN
        ble_linkplus_init();
    #endif
    }
#elif BLE_APP_EN
    ble_linkplus_init();
#elif BLE_TEST_EN
    ble_test_init();
#elif BLE_CUSTOMER_APP_EN
    ble_customer_init();
#endif
}


void mode_bt_init()
{
    uint16_t trimOriCap = nor_freq_drift_get();
    rf_drift_set(trimOriCap);   //在bt_frontend_init设置频偏值

    bt_frontend_init(1); //tws:1 //Yp3:0
    bt_rf_power_force(BT_RFPOWER_FORCE_EN, BT_RFPOWER_FORCE_VAL);
    bt_rf_maxpower_set(BT_MAXPOWER_SET_EN, BT_MAXPOWER_SET_VAL);

    bt_name_init();
    bt_addr_init();
    bt_profile_init();
    bt_register_event_handler(mode_bt_app_process);
}

void mode_bt_enter(void)
{
    logi("mode_bt_enter  version= %d>>", sys_ver);
#if BT_DUT_EN && !MODE_BT_EN
    //rf_para_dft();//FIXME:chenjian
    bt_ll_init();
#endif

#if DEBUG_AUTOTEST_LOG_EN
    mode_bt_cnt++;
#endif

    set_poweron_time(HAL_timestamp_to_ms(HAL_get_timestamp()));

#if TEST_EQUIPMENT_SEL
    bt_test_equipment_set(0x88522);
#endif
#if BT_SNIFF_EN
    bt_sniff_init();
    sleep_prevent_clear(PS_APP_PROCESS);
#else
    sleep_prevent_set(PS_APP_PROCESS);
#endif
    bt_app_status_set(0x00);

    set_cpu_low_freq_delay_time(5000); //5s后才允许CPU设置成低频率

	mode_bt_init();

    if(nor_mp_dut_flag_get()){
        logi("dut test mode en +++++++++++");
        nor_mp_dut_flag_set(0);
        bt_testmode_set(1);
    }

#if (TONE_EN && TONE_BT_MODE_EN)
    if (mode_switch_reason_get() != SW_MODE_RESET) {
        am_tone_play(SYS_TONE_BTMODE_NAME, 0, 0, TONE_BLUETOOTH_MODE_VOL, 0, 0);
    }
#endif

    am_btaudio_task_init();
#if (AUDIO_RECOVER_WHEN_RECONNECT_EN == 0)
    bt_play_state_set(0);
#endif
    bt_volume_feature(BT_VOLUME_EN);

    btstack_init();
    bt_manager_init();

#if (DISP_EN && STATE_BT_PAIR_EN)
    disp_state(STATE_BT_PAIR);
#endif

#if BAT_EXIST
    pmu_set_discon_time(rtc_get_second());
#endif

#if BT_DEBUG_EN
    bt_debug_port_config();
#endif
    bt_onoff_sta = true;

#if BLE_CUSTOMER_APP_EN //用户定制 app
    void ble_customer_addr_init();
    void ble_customer_name_init();
    void ble_customer_adv_param_init();

    ble_customer_addr_init();
    ble_customer_name_init();
    ble_customer_adv_param_init();

    void ble_app_init();

    ble_init();
    ble_app_init();
#endif
    
    logi("<<mode_bt_enter");
}

void mode_bt_exit(void)
{
#if REC_POWEROFF_TIME
    poweroff_time = HAL_get_timestamp();
#endif
    logi("mode_bt_exit>>");

#if BT_SNIFF_EN
    sleep_prevent_set(PS_APP_PROCESS);
#endif
#if DEBUG_AUTOTEST_LOG_EN
	logi("mode_bt %d, m_con: %d, %d, %d, %d, s_con: %d, %d, %d, %d, phone_con: %d, %d, %d", mode_bt_cnt, m_con_cnt, m_con_cnt_2, m_con_cnt_5, m_con_cnt_10, s_con_cnt, s_con_cnt_2, s_con_cnt_5, s_con_cnt_10, phone_con_cnt, phone_con_cnt_5, phone_con_cnt_10);
#endif
    bt_app_status_set(0x01);
    am_btaudio_stop();
#if BT_SNIFF_EN
    bt_sniff_deinit();
#endif

    mode_last_sta_set(MODE_BT);
    if (mode_get_sta() == MODE_RESET) {
        mode_set_force(MODE_BT);
        mode_switch_reason_set(SW_MODE_RESET);
    }
    am_btaudio_task_deinit();
#if AUDIO_ANC_EN
    as_anc_working_mode_set(ANC_WORKING_MODE_DIS);
#endif

    if(bt_disconnect_direct()){
        for(uint8_t wcnt=0;wcnt<100;wcnt++){
            if(bt_connect_link_n()==0){
                logi("wait dis %d.", wcnt);
                break;
            }else{
#if BT_SNIFF_EN
                HAL_msleep(30);
#else
                HAL_msleep(10);
#endif
            }
        }
    }else{
        HAL_msleep(50);
    }
    bt_disable();
    bt_manager_exit();
    btstack_exit();

    bt_con_flag = false;

    as_btaudio_sco_audio_irdata_store();
    event_clear(SYSTEM_EVENT_MODE_RESET);

#if BAT_EXIST
    pmu_set_discon_time(0xffffffff);
#endif
    bt_app_status_set(0x00);

    //关机前清掉事件，防止事件遗留到下一次开机过程中
    event_group_clear(BT_EVENT);
    event_group_clear(TWS_EVENT);
    event_group_clear(SYSTEM_EVENT);
    set_cpu_low_freq_delay_time(0);

    logi("mode_bt_exit<<");
#if REC_POWEROFF_TIME
    logi("poweroff take:%ums", HAL_timestamp_to_ms(HAL_get_timestamp() - poweroff_time));
#endif
    HAL_msleep(400);    //等待其他线程退出
    logi("exit sys heap %d- audio %d",OAL_heap_freespace_get(), audio_engine_get_avaheapsize());
}

void mode_bt(void)
{
    logi("bt mode.");
    mode_bt_enter();
    mode_bt_loop();
    mode_bt_exit();
}

#else

void mode_bt_exit(void)
{
}

#endif
