#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "ui.h"
#include "bt.h"
#include "bt_host_api.h"
#include "tws_profile.h"
#include "local_play_manager.h"
#include "key.h"
#include "bt_audio_manager.h"
#include "tws_api.h"
#include "app_pmu.h"
#include "sensor.h"
#include "sensor_proximity.h"
#include "audio_service.h"
#include "bt_audio_service.h"
#include "tone_play_manager.h"
#include "mode_manager.h"
#include "app_audio_anc.h"
#include "air_api.h"
#include "mode_bt.h"
#include "mode_g24.h"
#include "g24.h"
#include "g24_api.h"
#include "app_psm.h"
#include "psm_manager.h"
#include "key_ctrl_led.h"
#include "soundcard.h"
#include "audio_afx_config.h"
#include "charge.h"
#include "disp.h"
#include "amixer.h"
#include "usb_app_config.h"
#include "usbd_audio_app.h"
#include "usbd_msc_app.h"
#include "usbd_app.h"
#include "eq_controller_vss.h"
#include "spdif_play_track.h"
#include "usb_hid_app.h"
#include "app_lcd_font.h"
#include "lcd_display.h"

void usb_sys_module_init();
void on_pc_detect_timer_close();
void monitor_device_connect_set(bool is_connect);
void wireless_scan_timer_enable(bool enable);
g24_param_t *g24_param_get(void);
void usbd_gpio_sofware_detect_deinit();
void sbt_enable(bool en);
uint32_t g24_monitor_m_mic_id_get();

OAL_TIMER timer_g24_tx_enbale;
static uint8_t Lock_Status_flag = 0;


void timer_enbale_tx()
{
    bt_top_link_enable_set(0,TRUE);
}

void g24_ui_initialize()
{
#if G24_MONITOR_EN
#if (G24_FUNC_ROLE == 0) & G24_MONITOR_MASTER_EN    //handset
    if(timer_g24_tx_enbale == NULL) {
        OAL_timer_create(&timer_g24_tx_enbale, "enabletx", OAL_ms_to_ticks(2000), timer_enbale_tx,
            (void *)0, FALSE,OAL_TIMER_NO_ACTIVATE);
    }
#endif
#endif
}

AT_SRAM(.bt_24g_isr_comm_seg)
void g24_receive_cmd_proc(uint8_t link, uint8_t* cmd, uint8_t max_len)
{
    //logi("g24_receive_cmd_proc %x", cmd[0]);
    g24_mode_t *g24_mode = g24_mode_param_get();
    switch(cmd[0]) {
    case G24_CMD_DEL_LINKKEY:
        g24_mode->del_linkkey_req[link] = 1;
        event_put(KEY_EVENT_L | KEY_PLAY, NULL , 0);
        break;
    case G24_CMD_POWEROFF:
		mode_set(MODE_POWEROFF);
		break;
    }
}

void g24_handset_delete_link_process(void)
{
    g24_during_delete_link_set(true);
    
    //logi("g24_handset_delete_link_process");
#if 1
    g24_mode_t *g24_mode = g24_mode_param_get();
    if(g24_mode->connect[0]) {
        if(!g24_mode->del_linkkey_req[0]) {                     //handset发起
            if(!g24_monitor_en_get()) {
                uint8_t cmd[2];
                cmd[0] = G24_CMD_DEL_LINKKEY;                   //handset发给dongle,让dongle断开
                cmd[1] = 0x00;
                g24_send_cmd(0, cmd, 2);
                HAL_msleep(100);
            }
            bt_top_clear_link_flg(0);
        }
        if(bt_top_task_status_get(0) >= BT_CONNECT_POLL) {      //handset断开
            bt_top_disconnect(0);
        }
    }
    if(!g24_mode->del_linkkey_req[0]) {                         //handset发起则清除配对信息
        g24_link_info_delete(true,false);

        //使用新的local_addr
        uint8_t addr[6];
        g24_addr_default_get(addr);
        g24_local_addr_set(addr);
        g24_addr_save(addr);

        //进去查询状态
        bt_top_clear_link_flg(0);
        if(bt_top_task_status_get(0) >= BT_PAGE) {
            g24_mid_stop_task(0);
        }
    }
    g24_mode->del_linkkey_req[0] = 0;
#endif

#if 0	//两端一起清
    uint8_t cmd[2];
    g24_mode_t *g24_mode = g24_mode_param_get();
    if(g24_mode->connect[0]) {
        if(!g24_mode->del_linkkey_req[0]) {
            cmd[0] = G24_CMD_DEL_LINKKEY;
            cmd[1] = 0x00;
            g24_send_cmd(0, cmd, 2);
            HAL_msleep(100);
        }
        if(bt_top_task_status_get(0) >= BT_CONNECT_POLL) {
            bt_top_disconnect(0);
        }
    }
    g24_link_info_delete(true,false);
#if !G24_DUAL_CHANNEL
    uint8_t addr[6];
    g24_addr_default_get(addr);
    g24_local_addr_set(addr);
    g24_addr_save(addr);
#endif
    bt_top_clear_link_flg(0);
    if(bt_top_task_status_get(0) >= BT_PAGE) {
        g24_mid_stop_task(0);
    }
    g24_mode->del_linkkey_req[0] = 0;
#endif

    g24_during_delete_link_set(false);

    //restart();
}


void bt_ui_process_do(uint32_t event, uint32_t params_len, uint8_t *params)
{
    //logi("bt_ui_process_do %x", event);
    switch(event){
        case KEY_EVENT_SU | KEY_NUM18:
#if AUDIO_PLAYBACK_VSS
            logi("3d enable %d", !vss_enable_get());
            vss_enable_set(!vss_enable_get());
#endif
            break;
        case KEY_EVENT_SU | KEY_NUM17:
            logi("key17..");
            //link_info_clean();  //清除linkkey,由应用决定是否调用
            bt_disconnect_direct();
            bt_set_scan(1,1);
            break;
        case KEY_EVENT_SU | KEY_NUM16:
            {
            //蓝牙低延时模式和普通模式切换，低延时55ms
            static bool swit = 0;
            am_btaudio_game_mode_switch(!swit);
            logi("game mode %d", am_btaudio_game_mode_get);
            swit =!swit;
            }
            break;
        case KEY_EVENT_SU | KEY_SW:
#if (AUDIO_MIC_AENC || AUDIO_MIC_NSBWE_AENC)
            //1 :AENC 0:bypass
            logi("aenc mode:%d\r\n",!am_btaudio_call_aenc_mode_get());
            am_btaudio_call_aenc_mode_set(!am_btaudio_call_aenc_mode_get());
#endif
        break;
        case KEY_EVENT_SU | KEY_MODE:
            logi("key sw");
#if MODE_G24_EN
                work_mode_set(WORK_MODE_SET_G24);
                link_info_save();
                restart();
#endif
            break;
        case KEY_EVENT_SU | KEY_NEXT:
        case KEY_EVENT_SU | KEY_NEXT_VOLUP:
            logi("bt next");
            //if (am_btaudio_bt_calling_state_get() == BT_CALLING_STATE_NULL) {
            //    bt_audio_forward();
            //}
            work_mode_set(WORK_MODE_SET_G24);
            restart();
            break;
        case KEY_EVENT_SU | KEY_PREV:
        case KEY_EVENT_SU | KEY_PREV_VOLDOWN:
            logi("bt pre");
            if (am_btaudio_bt_calling_state_get() == BT_CALLING_STATE_NULL) {
                logi("bt audio pre");
                bt_audio_backward();
            }
            break;

        case KEY_EVENT_SU | KEY_PLAY:
            logi("bt play");
            if (am_btaudio_bt_calling_state_get() == BT_CALLING_STATE_NULL) {
                audio_play_pause();
            } else {
                am_btaudio_calling_process();
            }
            break;
#if USB_DETECT_USE_SOFWARE
        case USB_EVENT_SOFWWARE_USB_DETECT_PC_IN:
            logi("sw usb detect pc in");
            usbd_gpio_sofware_detect_deinit();  
            //需要再次检测还需要重新调用usbd_gpio_sofware_detect_init
            break;
#endif
    }
}

void g24_ui_process_do(uint32_t event, uint32_t params_len, uint8_t *params)
{
    static uint8_t chnmb;
    static bool ble_Status_flag = 0;
    static bool ble_link_flag = 0;//蓝牙连接标志
    static bool menu_Status_flag = 0;

    __maybe_unused uint8_t cmd[2];
    __maybe_unused uint8_t addr[6];
    g24_mode_t *g24_mode = g24_mode_param_get();

    //logi("g24_ui_process_do = %08x\n",event);

    switch(event) {
        case KEY_EVENT_SU | KEY_NUM18:
#if AUDIO_PLAYBACK_VSS
            logi("3d enable %d", !vss_enable_get());
            vss_enable_set(!vss_enable_get());
#endif
            break;
        case KEY_EVENT_SU | KEY_NUM17:
            logi("key17..");
            spdif_input_sel(!spdif_get_work_input());
            break;
        case KEY_EVENT_SU | KEY_NUM16:
            logi("key16\r\n");
            uint8_t cmd[2] = {G24_CMD_NEXT};
            g24_send_cmd(0,cmd ,2);
            break;
        case KEY_EVENT_SU | KEY_NUM15:
            //dolby 和立体声切换
            logi("key15");
            psm_usb_audio_mode_set(!psm_usb_audio_mode_get());
            work_mode_set(WORK_MODE_SET_G24);
            restart();
            break;
        case KEY_EVENT_SU | KEY_NUM14:
            logi("key14");
#if (BT_IODM_TEST_MODE_EN == 0) && BT_DUT_EN
        nor_mp_dut_flag_set(true);
        HAL_restart_pmu();
#endif
            break;
        case KEY_EVENT_SU | KEY_NUM13:
            logi("key13");
            usbd_hid_key_push(HID_KEY_VOL_UP);
            break;
        case KEY_EVENT_SU | KEY_NUM12:
            logi("key12");
            usbd_hid_key_push(HID_KEY_VOL_DOWN);
            break;
        case KEY_EVENT_SU | KEY_NUM11:
#if G24_MONITOR_EN && (G24_MONITOR_MASTER_EN  == 0)
#if G24_MONITOR_ONLINE_DETECT
            //通知dongle，一个handset即将主动断开
            monitor_user_cmd_send_set(0, true, MONITOR_USER_CMD_DISCONNECT);
            monitor_heartbreak_enable(false);   //停止发送心跳包
            HAL_mdelay(500);    //延时让命令发送出去
            mode_set(MODE_POWEROFF);
#endif
#endif
            break;
        case KEY_EVENT_SU | KEY_NUM10:
            {
            logi("rf shutdown");
            void g24_rf_shutdown();
            void g24_rf_turnon();

            static bool open = false;
            if(!open) {
                g24_rf_shutdown();
            } else {
                g24_rf_turnon();
            }
            open = !open;
            }
            break;
        case KEY_EVENT_SU | KEY_MODE:
            logi("key mode ");
            work_mode_set(WORK_MODE_SET_BT);
            restart();
            break;
#if USB_DEVICE_EN
    case USB_EVENT_CABLE_IN:
        usb_sys_module_init();
#if USB_DEVICE_TYPE == USB_DEVICE_TYPE_CARDREADER   //读卡器
        usbd_cardreader_initialize();
#else
        OAL_sleep(1000);
        usbd_audio_app_init();
#endif        
        break;

    case USB_EVENT_CABLE_OUT:
        on_pc_detect_timer_close();
        usbd_deinitialize();
        break;
        
    case USB_ENUM_SETUP_TRANSACTION:    //处理USB中断发过来的SETUP相关事件
        standard_setup();
        break;

    case USB_EVENT_PCM_VOLUME:
        if(!usbd_is_target_maybe_ps4()) {
            usbdev_volume_set_process();
        }
        break;

    case USB_EVENT_MIC_VOLUME:           //set mic volume here
        break;
#endif  //USB_DEVICE_EN

    case KEY_EVENT_SU | KEY_PREV://按键-
        if(g24_get_channel_ID() > 0 && !Lock_Status_flag)
        {
            chnmb = g24_get_channel_ID() - 1;
            systemStatus.signalChannel = chnmb;    
            logi("Channel down %d\n",chnmb);
        }
        logi("KEY_EVENT_SU   KEY_PREV\r\n");
        break;
    case KEY_EVENT_SU | KEY_PREV_VOLDOWN://音量+
        if(systemStatus.menuSelection == MAIN_MENU)
        {
            logi("rec vol down\r\n");
            am_record_volume_down();
        }
        else if(systemStatus.menuSelection == BLUETOOTH_MENU)//蓝牙菜单
        {
            if(systemStatus.ble_index[3].Select_Location_flag == 1)//到底部
            {
                if(systemStatus.ble_index[3].Ble_name_index < systemStatus.bluetoothDeviceCount)//当处于最底下并且没有到达最大值时
                {
                    for(int i = 0;i < 4;i++)
                    {
                        systemStatus.ble_index[i].Ble_name_index++;
                        systemStatus.ble_index[i].ble_updata = 1;
                    }
                }
            }
            else
            {
                for(int i = 0;i < 4;i++)
                {
                    if(systemStatus.ble_index[i].Select_Location_flag == 1)
                    {
                        systemStatus.ble_index[i].Select_Location_flag = 0;
                        systemStatus.ble_index[i+1].Select_Location_flag = 1;
                        break;
                    }
                }
            }
            
        }
        break;

    case KEY_EVENT_SU | KEY_NEXT://按键+
            if(g24_get_channel_ID() < 99 && !Lock_Status_flag)
        {
            chnmb = g24_get_channel_ID() + 1;
            systemStatus.signalChannel = chnmb;     
            logi("Channel down %d\n",chnmb);
        }
        logi("KEY_EVENT_SU   KEY_PREV\r\n");
        break;
    case KEY_EVENT_SU | KEY_NEXT_VOLUP://音量-
        if(systemStatus.menuSelection == MAIN_MENU)
        {
            logi("rec vol up\r\n");
            am_record_volume_up();
        }
        else if(systemStatus.menuSelection == BLUETOOTH_MENU)//蓝牙菜单
        {
            if(systemStatus.ble_index[0].Select_Location_flag == 1)//到底部
            {
                if(systemStatus.ble_index[0].Ble_name_index > 0)//当处于最顶上并且没有到达最大值时
                {
                    for(int i = 0;i < 4;i++)
                    {
                        systemStatus.ble_index[i].Ble_name_index--;
                        systemStatus.ble_index[i].ble_updata = 1;
                    }
                }
            }
            else
            {
                for(int i = 0;i < 4;i++)
                {
                    if(systemStatus.ble_index[i].Select_Location_flag == 1)
                    {
                        systemStatus.ble_index[i].Select_Location_flag = 0;
                        systemStatus.ble_index[i-1].Select_Location_flag = 1;
                        break;
                    }
                }
            }
            
        }

        break;

    //case KEY_EVENT_SU | KEY_PLAY:
        //cmd[0] = G24_CMD_PLAY;
        //cmd[1] = 0x00;
        //logi("PP\r\n");
        //g24_send_cmd(0, cmd, 2);
    //    break;

    case KEY_EVENT_L | KEY_PREV_VOLDOWN://菜单切换
        if(menu_Status_flag == 0)
        {
            if(systemStatus.menuSelection == MAIN_MENU && systemStatus.bluetoothState == BLUETOOTH_ON)
            {
                get_ble_list();//获取蓝牙列表
                for(int i = 0;i < MAX_DEVICES;i++)
                {
                    systemStatus.ble_device_link_status[i] = Unselected;//清除连接状态显示
                }
                systemStatus.menuSelection = BLUETOOTH_MENU;
            }
            else
                systemStatus.menuSelection = MAIN_MENU;

            menu_Status_flag = 1;
            logi("ble_menu\r\n");
        }
        break;

    case KEY_EVENT_LU | KEY_PREV_VOLDOWN://菜单切换
        if(menu_Status_flag == 1)
        {
            menu_Status_flag = 0;
        }
        break;


    case KEY_EVENT_L | KEY_NEXT://蓝牙开关
        if(ble_Status_flag == 0 && systemStatus.menuSelection == MAIN_MENU)
        {
            if(systemStatus.bluetoothState == BLUETOOTH_OFF)
            {
                systemStatus.bluetoothState = BLUETOOTH_ON;
                // get_ble_list();
            }
            else
                systemStatus.bluetoothState = BLUETOOTH_OFF;

            ble_Status_flag = 1;
            logi("ble_Status\r\n");
        }
        else if(ble_link_flag == 0 && systemStatus.menuSelection == BLUETOOTH_MENU)//蓝牙菜单 连接蓝牙
        {
            Bleinfo bleinfo;
            get_bluetooth_device_name(&bleinfo);
            connect_to_ble(bleinfo.Ble_name);//连接当前蓝牙(获取当前蓝牙名称)
            BLE_Connect_set(Connecting,bleinfo.Ble_name_index);//显示连接中
            if(get_ble_connection_status())
            {
                //蓝牙连接成功
                BLE_Connect_set(Connected,bleinfo.Ble_name_index);//显示连接中
            }
            else
            {
                //蓝牙连接失败
                BLE_Connect_set(ConnectionFailed,bleinfo.Ble_name_index);//显示连接中
            }
        }
        break;

    case KEY_EVENT_LU | KEY_NEXT://蓝牙开关
        if(ble_Status_flag == 1 && systemStatus.menuSelection == MAIN_MENU)
        {
            ble_Status_flag = 0;    
        }
        else if(ble_link_flag == 1 && systemStatus.menuSelection == BLUETOOTH_MENU)//蓝牙菜单 连接蓝牙
        {
            ble_link_flag = 0;
        }
        break;


#if RECORD_FUNC_EN == 0
    case KEY_EVENT_M | KEY_M | (4<<8):
    case KEY_EVENT_L | KEY_PLAY:
        logi("delete linkkey\r\n");
        g24_handset_delete_link_process();
        break;
#endif
    case KEY_EVENT_H | KEY_PREV:
    case KEY_EVENT_H | KEY_PREV_VOLDOWN:
        cmd[0] = G24_CMD_VOL_DN;
        cmd[1] = 0x00;
        logi("VOL_DN\r\n");
        g24_send_cmd(0, cmd, 2);
        break;

    case KEY_EVENT_H | KEY_NEXT:
    case KEY_EVENT_H | KEY_NEXT_VOLUP:
        cmd[0] = G24_CMD_VOL_UP;
        cmd[1] = 0x00;
        logi("VOL_UP\r\n");
        g24_send_cmd(0, cmd, 2);
        break;

    case KEY_EVENT_SU | KEY_M:
    case KEY_EVENT_SU | KEY_SW:
    case KEY_EVENT_SU | KEY_PLAY:
        {
#if (AUDIO_MIC_AENC || AUDIO_MIC_NSBWE_AENC)
            logi("aenc sw:%d\r\n",!Lmic_aenc_enable_get());
            Lmic_aenc_enable_set(!Lmic_aenc_enable_get());
#if MYME_M4_BOARD
            key_led_set_status(led4_green, Lmic_aenc_enable_get());
#endif
#if EVB_TX_DEMO
            if(Lmic_aenc_enable_get()) {
                AENC_DISP_ON();
            } else {
                AENC_DISP_OFF();
            }
#endif

#endif
            /*
#if AUDIO_MIC_REVERB
            uint8_t fdnrev_enable = fdnrev_enable_get();
            fdnrev_enable = mic_fdnrev_enable_set(!fdnrev_enable);
            logi("fdnrev sw:%d\r\n",fdnrev_enable);
#if MYME_M4_BOARD
            key_led_set_status(led4_green, fdnrev_enable);
#endif
#endif
            */
        }
        break;

#if USB_AUDIO_EN
    case USB_EVENT_PCM_VOLUME:
        /*
        if(USBD_volume_control_pipe_enable()) {
            usbdev_volume_set_process();
        }
        */
        break;

    case USB_EVENT_MIC_VOLUME:           //set mic volume here
        break;
#endif

    case G24_EVENT_SLAVE_SAVE_BDADDR:
#if G24_PAGE_REPLACE_INQUIRY
        if(!g24_monitor_en_get()) {                       //一拖多监听slaver不回连
            g24_local_addr_get(addr);
            bt_top_set_link_addr(0,addr);                 //set reconnect addr
            g24_link_info_save(0);
        }
#endif
        logi("save bdaddr\r\n");
        break;

    case G24_EVENT_MASTER_SAVE_LNK:
        logi("##save linkkey\r\n");
        break;
    case G24_EVENT_DISCONNECT:
    {
        uint8_t link = *params;
        //g24_link_addr_get(0, addr, false);
        //bt_top_set_link_addr(0,addr);
        g24_mode->connect[link] = false;
        if(g24_monitor_en_get()) {
            bt_top_clear_link_flg(link);                   //重新走第一次连接流程
        }
        logi("disconnect g24\r\n");
#if LCD_SEG_EN
        displayUpdate(Signal_ZeroBar,true);//无信号显示
#endif
        systemStatus.signalStrength = SIGNAL_NONE;
#if MYME_M4_BOARD
         key_led_set_g24_connect_status(link, 0);
#endif
#if (DISP_EN && STATE_BT_DIS_EN)
         disp_state(STATE_BT_DIS);
#endif

#if  POWER_LPC_MODE_EN
#if (G24_FUNC_ROLE == 0)
        monitor_device_connect_set(false);
        wireless_scan_timer_enable(true);
        amixer_dac_switch(0);
#endif
#endif

        break;
    }
    case G24_EVENT_CONNECT:
    {
        logi("event connect");
#if G24_MONITOR_EN && (G24_MONITOR_MASTER_EN == 0)
        g24_set_not_match_msg_processed(true);
#if G24_MONITOR_ONLINE_DETECT
        void monitor_handset_detect_stamp_set();
        monitor_handset_detect_stamp_set();
        monitor_heartbreak_enable(true);
#endif
#endif

#if POWER_LPC_MODE_EN
        low_power_wakup_clear();
#endif
        __maybe_unused uint8_t link = *params;
        g24_mode->connect[link] = true;
        /*
        if (G24_DEVICE_ROLE == G24_DEVICE_ROLE_SLAVER) {
            g24_local_addr_get(addr);
            bt_top_set_link_addr(0,addr);                 //set reconnect addr
        }
        */
        logi("g24 connect link %d\r\n", link);
#if MYME_M4_BOARD
        key_led_set_g24_connect_status(link, 1);
#endif
#if !G24_PAGE_REPLACE_INQUIRY
        g24_link_info_save(link);
#else
        if(g24_monitor_en_get()) {                        //一拖多监听slaver保存master地址用于连接允许判断
            g24_link_info_save(link);
        }
#endif
#if (DISP_EN && STATE_BT_CON_EN)
        disp_state(STATE_BT_CON);
#endif

#if  POWER_LPC_MODE_EN

#if (G24_FUNC_ROLE == 0)
        wireless_scan_timer_enable(false);
        event_clear(SYSTEM_EVENT_WIRELESS_SCAN_EN);
#endif
        monitor_device_connect_set(true);
        amixer_dac_switch(1);
#endif
		break;
    }
     case EQ_EVENT_SET_EQ_DATA_ISR:
#if AUDIO_MIC_REVERB
        mic_fdnrev_refresh();
#endif
        break;
    case USB_EVENT_TEST_HOST_IS_PS4:
        HAL_mdelay(500);    //延时才能检测得到是否是PS4
        logi("ps4 connect %d ....", usbd_is_target_maybe_ps4());
        break;
#if G24_MONITOR_EN
#if (G24_FUNC_ROLE == 0) & G24_MONITOR_MASTER_EN    //handset
    case G24_EVENT_MONITOR_TX_ENABLE:
        if(timer_g24_tx_enbale) {
            OAL_timer_activate(&timer_g24_tx_enbale, OAL_TIMER_AUTO_ACTIVATE);
        }
        break;
#endif
#endif
#if POWER_LPC_MODE_EN & (G24_FUNC_ROLE == 0)
    case SYSTEM_EVENT_WIRELESS_SCAN_EN:
        logi("wireless scan en %d", params_len);
        g24_param_t* g24Param = g24_param_get();
        if(params_len == 1) {
            INTC_EnableIRQ(IRQn_SBT);
            bt_top_link_enable_set(0,TRUE);			//使能link0连接流程
            bt_top_link_enable_set(1,TRUE);

            if(g24Param->use_sbt_connect) {
                sbt_enable(true);
            }
        } else if(params_len == 0){
            bt_top_link_enable_set(0,FALSE);
            bt_top_link_enable_set(1,FALSE);

            if(g24Param->use_sbt_connect) {
                sbt_enable(false);
            }
            //g24_bt_e_rspdsb_set(1);     //TX关闭
        }
        break;

    case G24_EVENT_LPC_ENTER:
        HAL_msleep(100);
        amixer_dac_switch(0);
        logi("stop");
    break;
#endif 
#if G24_MONITOR_EN & G24_MONITOR_MASTER_EN
    case G24_EVENT_MONITOR_FORCE_DISCONNECT:
        //强制删除KEY并退出连接, 这个根据实际需求来，断开连接时一定要的
        g24_handset_delete_link_process();
        break;
    case G24_EVENT_MONITOR_MIC_ID_NOT_MATCH:
        {
            logi("m_mic_id , force disconnect...");
            bt_top_link_enable_set(0,FALSE);
            g24_handset_delete_link_process();
            g24_set_not_match_msg_processed(true);
            event_put(G24_EVENT_MONITOR_TX_ENABLE, 0, 0);
        }
        break;
#endif
#if KADC3_EN
    case KEY_VOL0 | KEY_EVENT_S:
            am_record_volume_set(0);
            disp_lcd_volume_update(am_record_volume_get()/10);//音量
            logi("##############   %d   ##############",am_record_volume_get());
        break;

    case KEY_VOL1 | KEY_EVENT_S:
            am_record_volume_set(10);
            disp_lcd_volume_update(am_record_volume_get()/10);//音量
            logi("##############   %d   ##############",am_record_volume_get());
        break;
    
    case KEY_VOL2 | KEY_EVENT_S:
            am_record_volume_set(20);
            disp_lcd_volume_update(am_record_volume_get()/10);//音量
            logi("##############   %d   ##############",am_record_volume_get());
        break;

    case KEY_VOL3 | KEY_EVENT_S:
            am_record_volume_set(30);
            disp_lcd_volume_update(am_record_volume_get()/10);//音量
            logi("##############   %d   ##############",am_record_volume_get());
        break;

    case KEY_VOL4 | KEY_EVENT_S:
            am_record_volume_set(40);
            disp_lcd_volume_update(am_record_volume_get()/10);//音量
            logi("##############   %d   ##############",am_record_volume_get());
        break;

    case KEY_VOL5 | KEY_EVENT_S:
            am_record_volume_set(50);
            disp_lcd_volume_update(am_record_volume_get()/10);//音量
            logi("##############   %d   ##############",am_record_volume_get());  
        break;
    case KEY_VOL6 | KEY_EVENT_S:
            am_record_volume_set(60);
            disp_lcd_volume_update(am_record_volume_get()/10);//音量
            logi("##############   %d   ##############",am_record_volume_get());
        break;
    case KEY_VOL7 | KEY_EVENT_S:
            am_record_volume_set(70);
            disp_lcd_volume_update(am_record_volume_get()/10);//音量
            logi("##############   %d   ##############",am_record_volume_get());
        break;
    case KEY_VOL8 | KEY_EVENT_S:
            am_record_volume_set(80);
            disp_lcd_volume_update(am_record_volume_get()/10);//音量
            logi("##############   %d   ##############",am_record_volume_get());
        break;
    case KEY_VOL9 | KEY_EVENT_S:
            am_record_volume_set(90);
            disp_lcd_volume_update(am_record_volume_get()/10);//音量
            logi("##############   %d   ##############",am_record_volume_get());
        break;
#endif  
    }
}

#if DISP_EN

#if LED_DIODE_EN

volatile bool bat_full_when_charging = false;

/*------------------------------------------------------------------------------
*函数功能: 灯光行为控制
*参数1：各种状态下，灯光常亮，关闭，闪烁等
*-------------------------------------------------------------------------------*/

void disp_state_led_diode_common(enum disp_state_e state)
{
    g24_mode_t *g24_mode = g24_mode_param_get();
    
    //disp priority:bat low > charging > connect/disconnect
    if (pmu_bat_is_low()) {
       disp_led_flash_set(LED_RED,200,200,0xffffffff);
    } else if (pmu_bat_is_charging()) {
        printf_debug("**pmu_bat_is_charging");
        if ((state == STATE_BAT_FULL) || (bat_full_when_charging == true)) {
            if(charge_mode_sta_get()) {
                printf_debug("**LED_RED ON");
                disp_led_onoff_set(LED_RED, 0);
            }
            bat_full_when_charging = true;
        } else {
            if(charge_mode_sta_get()) {
                printf_debug("**LED_RED ON");
                disp_led_onoff_set(LED_RED, 1);
            }
        }
    } else {
        disp_led_flash_set(LED_RED,1000,1000,0xffffffff);
    }

    if(is_in_mode_g24()) {
        if (g24_mode->connect[0]) {
            disp_led_onoff_set(LED_BLUE, 1);
        }  else {
            disp_led_flash_set(LED_BLUE,1000,1000,0xffffffff);
        }
    }

    if (state == STATE_UNCHARGEING) {
        bat_full_when_charging = false;
    }
}


/*------------------------------------------------------------------------------
*函数功能: ui灯光控制
*参数1：state 表示什么状态下的灯光控制，开关机，连接及断开等
*-------------------------------------------------------------------------------*/

AT_SRAM(.disp)
void ui_led_diode_process(enum disp_state_e state)
{
    printf_debug("##disp_stat:%d\n",state);
    switch(state) {
    //system
    case STATE_POWERON:
    case STATE_RESET:
        disp_led_delayoff_set(LED_BLUE,2000);
        break;

#if MODE_POWEROFF_EN
    //power off
    case STATE_POWEROFF:
        disp_led_onoff_set(LED_RED, 0);
        disp_led_onoff_set(LED_BLUE, 0);
        break;
#endif

    //charge
#if MODE_CHARGE_EN
    case STATE_UNCHARGEING:
    case STATE_BAT_LOW:
    case STATE_BAT_UNLOW:
    case STATE_CHARGEING:
    case STATE_BAT_FULL:
        disp_state_led_diode_common(state);
        break;
#endif

    //2.4g
#if MODE_G24_EN 
    //case STATE_BT_PAIR:
    case STATE_BT_CON:
    case STATE_BT_DIS:
    case STATE_BT_PEER_CON:
    case STATE_BT_PEER_DIS:
        disp_state_led_diode_common(state);
        break;
#endif

    default:
        break;
    }
}
#endif
#endif


void ui_process_do(uint32_t event)
{
}



