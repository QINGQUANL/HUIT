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
#include "usb_app_config.h"
#include "usbd_audio_app.h"
#include "usb_hid_app.h"
#include "soundcard.h"
#include "usbd_app.h"
#include "key_ctrl_led.h"
#include "audio_afx_config.h"
#include "charge.h"
#include "disp.h"
#include "amixer.h"
#include "usbh_msc_app.h"
#include "usb_sys.h"
#include "app_lcdc.h"
#include "audio_afx_config.h"
#include "g24_api.h"
#include "mode_g24.h"
#include "app_lcdc.h"
#include "lcd_display.h"

extern void g24_Channel_select_code(uint8_t channel);


uint8_t wrx_ctrl_sel;
#if G24_MONITOR_EN && G24_MONITOR_ONLINE_DETECT

typedef struct _monitor_check_info_
{
    uint32_t monitor_connect_id;
    uint32_t timestamp_ms;
}monitor_check_info;

#define MAX_MONITOR_NUM  16

//30秒超时时间
#define NO_HEARTBREAK_TIME  30 
uint16_t monitor_device_count;
monitor_check_info monitor_id_vector[MAX_MONITOR_NUM]; //定义一个ID数组，用于记录连接的设备
OAL_TIMER monitor_detect_timer = NULL;

uint32_t g24_check_monitor_number(uint32_t monitor_id)
{
    for(uint8_t i =0 ;i< MAX_MONITOR_NUM; i++) {
        if(monitor_id_vector[i].monitor_connect_id == monitor_id) {
            //更新时间戳
            monitor_id_vector[i].timestamp_ms = HAL_timestamp_to_ms(HAL_get_timestamp());
            return monitor_device_count;//已经存在了
        }
    }

    for(uint8_t i =0 ;i< MAX_MONITOR_NUM; i++) {
        if(monitor_id_vector[i].monitor_connect_id == 0) {
            monitor_id_vector[i].monitor_connect_id = monitor_id;
            //更新时间戳
            monitor_id_vector[i].timestamp_ms = HAL_timestamp_to_ms(HAL_get_timestamp());
            monitor_device_count++;
            break;
        }
    }
    return monitor_device_count;
}

bool g24_remove_monitor_id(uint32_t monitor_id)
{
    for(uint8_t i =0 ;i< MAX_MONITOR_NUM; i++) {
        if(monitor_id_vector[i].monitor_connect_id == monitor_id) {
            monitor_id_vector[i].monitor_connect_id = 0;
            monitor_id_vector[i].timestamp_ms = 0;
            monitor_device_count--;
            return true;
        } 
    }
    return false;
}

void monitor_detect_handset()
{
    if(monitor_device_count ) {
        uint32_t cur_time = HAL_timestamp_to_ms(HAL_get_timestamp());
        for(uint8_t i=0; i< MAX_MONITOR_NUM;i++ ){
            if(monitor_id_vector[i].monitor_connect_id != 0) {
                if((cur_time - monitor_id_vector[i].timestamp_ms) > NO_HEARTBREAK_TIME*1000) {
                    //超时时间内没有收到心跳包，说明断线了
                    //干点啥事呢??
                    event_put(G24_EVENT_MONITOR_USER_DISCON, &monitor_id_vector[i].monitor_connect_id, 4 );
                }
            }
        } 
    }
}

void g24_ui_initialize()
{
    monitor_device_count = 0;
    memset(monitor_id_vector, 0, MAX_MONITOR_NUM * sizeof(monitor_check_info));

    if(monitor_detect_timer == NULL) {
        //30秒检测一次
        OAL_timer_create(&monitor_detect_timer, "pcdetect", NO_HEARTBREAK_TIME*1000, monitor_detect_handset,
            (void *)0, TRUE,OAL_TIMER_AUTO_ACTIVATE);
    }
}

#else
void g24_ui_initialize()
{
}

#endif


extern void ep0_setup_new();

AT_SRAM(.bt_24g_isr_comm_seg)
void g24_receive_cmd_proc(uint8_t link, uint8_t* cmd, uint8_t max_len)
{
    //logi("g24_receive_cmd_proc %x", cmd[0]);
    g24_mode_t *g24_mode = g24_mode_param_get();
    switch(cmd[0]) {
        case G24_CMD_PREV:
            event_put(KEY_EVENT_SU | KEY_PREV, NULL, 0);
            break;
        case G24_CMD_NEXT:
            event_put(KEY_EVENT_SU | KEY_NEXT, NULL, 0);
            break;
        case G24_CMD_PLAY:
            event_put(KEY_EVENT_SU | KEY_PLAY, NULL, 0);
            break;
        case G24_CMD_VOL_DN:
            event_put(KEY_EVENT_H | KEY_PREV, NULL, 0);
            break;
        case G24_CMD_VOL_UP:
            event_put(KEY_EVENT_H | KEY_NEXT, NULL, 0);
            break;
        case G24_CMD_DEL_LINKKEY:
            g24_mode->del_linkkey_req[link] = 1;
            event_put(KEY_EVENT_L | KEY_PLAY, NULL, 0);
            break;
        case G24_CMD_POWEROFF:
#if !G24_MASTER_RECONNECT_ALWAYS && !G24_DUAL_CHANNEL
            if(G24_DEVICE_ROLE == G24_DEVICE_ROLE_MASTER) {
                //logi("Get poweroff\n");
                event_put(G24_EVENT_POWEROFF_REQ, NULL, 0);
            }
#endif
            break;
        }
}

void g24_DEL_dongle(void)
{
    uint8_t cmd[2];
    uint8_t delete_link[2] = {0,0};
    g24_mode_t *g24_mode = g24_mode_param_get();
    if(!g24_mode->del_linkkey_req[0] && !g24_mode->del_linkkey_req[1]) {    //dongle发起
        cmd[0] = G24_CMD_DEL_LINKKEY;                                       //dongle发给handset,让handset断开
        cmd[1] = 0x00;
        g24_send_cmd(wrx_ctrl_sel, cmd, 2);
        HAL_msleep(100);
        
        delete_link[wrx_ctrl_sel] = 1;   //disconnect wrx_ctrl_sel ch

        bt_top_clear_link_flg(wrx_ctrl_sel);
        logi("##############bt_top_task_status_get(wrx_ctrl_sel) = %d",bt_top_task_status_get(wrx_ctrl_sel) );
        if(bt_top_task_status_get(wrx_ctrl_sel) >= BT_CONNECT_POLL) {      //dongle主动断开
            bt_top_disconnect(wrx_ctrl_sel);
        }
    } else {                                                               //handset发起
        if(g24_mode->del_linkkey_req[0]) {
            g24_mode->del_linkkey_req[0] = 0;
            if(bt_top_task_status_get(0) >= BT_CONNECT_POLL) {             //dongle被请求断开
                bt_top_disconnect(0);
            }
        }
        logi("##############bt_top_task_status_get(0) = %d",bt_top_task_status_get(0) );
        g24_link_info_delete(delete_link[0],delete_link[1]);
    }
}

void g24_dongle_delete_link_process(void)
{
    uint8_t cmd[2];
    uint8_t delete_link[2] = {0,0};
    g24_mode_t *g24_mode = g24_mode_param_get();

    if(bt_top_task_status_get(1) >= BT_PAGE)
    {
        g24_mid_stop_task(1);
    }
    g24_DEL_dongle();
    if(g24_monitor_en_get()) {
        //1拖多不支持删除linkey，只有handset保存linkey
        return;
    }

    g24_during_delete_link_set(true);

#if 1
    logi("##############g24_mode->connect[0] = %d",g24_mode->connect[0]);
    if(g24_mode->connect[0] 
#if G24_DUAL_CHANNEL
    || g24_mode->connect[1]
#endif
        ) {
        if(!g24_mode->del_linkkey_req[0] && !g24_mode->del_linkkey_req[1]) {    //dongle发起
            cmd[0] = G24_CMD_DEL_LINKKEY;                                       //dongle发给handset,让handset断开
            cmd[1] = 0x00;
            g24_send_cmd(wrx_ctrl_sel, cmd, 2);
            HAL_msleep(100);

            delete_link[wrx_ctrl_sel] = 1;   //disconnect wrx_ctrl_sel ch

            bt_top_clear_link_flg(wrx_ctrl_sel);
            logi("##############bt_top_task_status_get(wrx_ctrl_sel) = %d",bt_top_task_status_get(wrx_ctrl_sel) );
            if(bt_top_task_status_get(wrx_ctrl_sel) >= BT_CONNECT_POLL) {      //dongle主动断开
                bt_top_disconnect(wrx_ctrl_sel);
            }
        } else {                                                               //handset发起
            if(g24_mode->del_linkkey_req[0]) {
                g24_mode->del_linkkey_req[0] = 0;
                if(bt_top_task_status_get(0) >= BT_CONNECT_POLL) {             //dongle被请求断开
                    bt_top_disconnect(0);
                }
            }
            logi("##############bt_top_task_status_get(0) = %d",bt_top_task_status_get(0) );
#if G24_DUAL_CHANNEL
            if(g24_mode->del_linkkey_req[1]) {
                g24_mode->del_linkkey_req[1] = 0;
                if(bt_top_task_status_get(1) >= BT_CONNECT_POLL) {
                    bt_top_disconnect(1);
                }
            }
#endif
        }
    }else {
        delete_link[wrx_ctrl_sel] = 1;   //disconnect wrx_ctrl_sel ch
    }
    g24_link_info_delete(delete_link[0],delete_link[1]);
    if(delete_link[0]) {
        if (G24_DEVICE_ROLE == G24_DEVICE_ROLE_SLAVER) {
            //使用新的local_addr
            uint8_t addr[6];
            g24_addr_default_get(addr);
            g24_local_addr_set(addr);
            g24_addr_save(addr);
        }
        //进去查询状态
        bt_top_clear_link_flg(0);
        if(bt_top_task_status_get(0) >= BT_PAGE) {
            g24_mid_stop_task(0);
        }
        g24_mode->del_linkkey_req[0] = 0;
    }
#if G24_DUAL_CHANNEL
    if(delete_link[1]) {
        bt_top_clear_link_flg(1);
        if(bt_top_task_status_get(1) >= BT_PAGE) {
            g24_mid_stop_task(1);
        }
        g24_mode->del_linkkey_req[1] = 0;
    }
#endif
#endif


#if 0 //两端ch0 & 1一起清
    if(g24_mode->connect[0] 
#if G24_DUAL_CHANNEL
    || g24_mode->connect[1]
#endif
        ) {
        if(!g24_mode->del_linkkey_req[0] && !g24_mode->del_linkkey_req[1]) {
            cmd[0] = G24_CMD_DEL_LINKKEY;
            cmd[1] = 0x00;
            g24_send_cmd(0, cmd, 2);
#if G24_DUAL_CHANNEL
            g24_send_cmd(1, cmd, 2);
#endif
            HAL_msleep(100);
            delete_link[0] = 1;   //disconnect 0 & 1
            delete_link[1] = 1;
        } else {
            delete_link[0] = g24_mode->del_linkkey_req[0];
            delete_link[1] = g24_mode->del_linkkey_req[1];
        }
        if(delete_link[0] && bt_top_task_status_get(0) >= BT_CONNECT_POLL) {
            bt_top_disconnect(0);
        }
#if G24_DUAL_CHANNEL
        if(delete_link[1] && bt_top_task_status_get(1) >= BT_CONNECT_POLL) {
            bt_top_disconnect(1);
        }
#endif
    }else {
        delete_link[0] = 1;   //disconnect 0 & 1
        delete_link[1] = 1;
    }
    g24_link_info_delete(delete_link[0],delete_link[1]);
    if(delete_link[0]) {
        bt_top_clear_link_flg(0);
        if(bt_top_task_status_get(0) >= BT_PAGE) {
            g24_mid_stop_task(0);
        }
        g24_mode->del_linkkey_req[0] = 0;
    }
#if G24_DUAL_CHANNEL
    if(delete_link[1]) {
        bt_top_clear_link_flg(1);
        if(bt_top_task_status_get(1) >= BT_PAGE) {
            g24_mid_stop_task(1);
        }
        g24_mode->del_linkkey_req[1] = 0;
    }
#endif
#endif

    g24_during_delete_link_set(false);
    //restart();
}

bool mic_to_usb_flag = true;
void Lmic_listener_dry_vol_mute_set(uint8_t mute);
void mic_wet_listener_vol_mute_set(uint8_t mute);
void on_pc_detect_timer_close();
// extern uint8_t LCD_g24_get_channel_ID(void);

void g24_ui_process_do(uint32_t event, uint32_t params_len, uint8_t *params)
{ 
    static uint8_t chnmb = 0;
    static uint8_t Lock_Status_flag = 0;
    static bool ble_Status_flag = 0;
    static bool menu_Status_flag = 0;
    static bool ble_link_flag = 0;//蓝牙连接标志

    // uint16_t numb = 0;
    __maybe_unused uint8_t cmd[2];
    __maybe_unused uint8_t addr[6];
    g24_mode_t *g24_mode = g24_mode_param_get();
    switch(event) {
#if G24_MONITOR_EN 
#if G24_MONITOR_ONLINE_DETECT
    // HANDSET发掉线消息dongle不一定能够收到，需要结合心跳包一起来判断
    case G24_EVENT_HEARTBREAK_RECEIVE:
        {
        uint32_t monitor_id = *(uint32_t*)params;
        g24_check_monitor_number(monitor_id);
        logi("G24_EVENT_HEARTBREAK_RECEIVE, id %x ,num %d", monitor_id, monitor_device_count);
        }
        break;
    case G24_EVENT_MONITOR_USER_DISCON:
        {
        uint32_t monitor_id = *(uint32_t*)params;
        g24_remove_monitor_id(monitor_id);
        //可以将发射关闭，启动一个定时器，过段时间再起来，看看是否会有设备连接
        //g24_bt_e_rspdsb_set(1);     //TX关闭
        logi("G24_EVENT_MONITOR_USER_DISCON,id %x, num %d", monitor_id, monitor_device_count);
        }
        break;
#endif
#endif

#if USB_DEVICE_EN
    case USB_EVENT_CABLE_IN:
        logi("event cable in");
        OAL_sleep(1000);
        logi("event cable in ++++ ");
        usb_sys_module_init();
        usbd_audio_app_init();
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
#endif

#if USB_HOST_EN
    case USB_EVENT_UDISK_SIGNAL_IN:
        usb_sys_module_init();
        usbh_disk_process();
        break;
    case USB_EVENT_UDISK_ARRIVAL:
        break;
    case USB_EVENT_UDISK_REMOVAL:
        usb_sys_module_deinit();
        break;
#endif

    case KEY_EVENT_M | KEY_M | (2<<8):
        duck_enbale_set(!duck_enbale_get());
        logi("duck :%d",duck_enbale_get());
        break;

    case KEY_EVENT_M | KEY_M | (3<<8):
		//导播声音到USB上行开关切换
        if(mic_to_usb_flag) {
            mic_to_usb_flag = false;
            logi("mic_to_usb_flag 0");
            Lmic_listener_dry_vol_mute_set(true);      //有线MIC干声到监听
            mic_wet_listener_vol_mute_set(true);       //MIC(Lmic+wrx)湿声到监听
        } else {
            mic_to_usb_flag = true;
            logi("mic_to_usb_flag 1");
            Lmic_listener_dry_vol_mute_set(false);      //有线MIC干声到监听
            mic_wet_listener_vol_mute_set(false);       //MIC(Lmic+wrx)湿声到监听
        }
        break;

    case KEY_EVENT_SU | KEY_PREV://按键-
        if(g24_get_channel_ID() > 0 && !Lock_Status_flag)
        {
            chnmb = g24_get_channel_ID() - 1;
            systemStatus.signalChannel = chnmb;    
            logi("Channel down %d\n",chnmb);
        }
        break;
    case KEY_EVENT_SU | KEY_PREV_VOLDOWN://音量-
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

#if AUDIO_MIC_AFC
        logi("afc adaptive");
        afc_adaptive();
#else
        logi("rec vol down\r\n");
        am_record_volume_down();
#endif

        break;
    case KEY_EVENT_SU | KEY_SW:
    case KEY_EVENT_SU | KEY_MODE:
         if(mode_sta_get() == MODE_BT) {
#if MODE_G24_EN
            work_mode_set(WORK_MODE_SET_G24);
            restart();
#endif
        } else if(mode_sta_get() == MODE_G24) {
#if MODE_BT_EN
            work_mode_set(WORK_MODE_SET_BT);
            restart();
#endif
        }
        break;
    case KEY_EVENT_SU | KEY_NUM18:
        logi("key num18");
#if (BT_IODM_TEST_MODE_EN == 0) && BT_DUT_EN
        nor_mp_dut_flag_set(true);
        HAL_restart_pmu();
#endif
        break;
    case KEY_EVENT_SU | KEY_NUM17:
        {
#if G24_MONITOR_EN == 0 //1拖N不支持
            logi("key17");
            void g24_rf_turnon();
            void g24_rf_shutdown();

            static bool rf_en = 1;
            if(rf_en) {
                g24_rf_shutdown();
            } else {
                g24_rf_turnon();
            }
            rf_en = !rf_en;
#endif
        }
        break;
    case KEY_EVENT_SU | KEY_NUM16:
        logi("key16\r\n");
        uint8_t cmd[2] = {G24_CMD_NEXT};
        g24_send_cmd(0,cmd ,2);
        break;
    case KEY_EVENT_SU | KEY_NEXT://按键+
        if(g24_get_channel_ID() < 99 && !Lock_Status_flag)
        {
            chnmb = g24_get_channel_ID() + 1;
            systemStatus.signalChannel = chnmb;   
            logi("Channel UP %d\n",chnmb);
        }
        break;
    case KEY_EVENT_SU | KEY_NEXT_VOLUP://音量+
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
        
    case KEY_EVENT_SU | KEY_NUM15:
		{
#if G24_MONITOR_EN
		    void g24_force_all_pair(void);
	        g24_force_all_pair();						//Transmitter activated for code pairing.
#endif
        }
		break;

	case KEY_EVENT_SU  | KEY_NUM14:
		cmd[0] = G24_CMD_POWEROFF;			//All paired handsets are turned off.
		cmd[1] = 0x00;
		g24_send_cmd(0, cmd, 2);
		break;
#if 0   //需要支持单载波测试则将其打开
    case BT_EVENT_SIGLE_WAVE:
        {
        g24_rf_shutdown();
        extern void rfdss_tone(unsigned int fre);
        rfdss_tone(2441);
        break;
        }
#endif
    //case KEY_EVENT_SU | KEY_PLAY:
    //    logi("dev P/P");
    //    usbd_hid_key_push(HID_KEY_PLAY_PAUSE);
    //    break;

    case KEY_EVENT_H | KEY_PREV://一键对频
        // numb = 500;
        // g24_allpair_send(ALL_PAIR_2);
        // while(numb--);
        g24_allpair_send(g24_get_channel_ID());
        logi("DOWN");
        break;
    case KEY_EVENT_H | KEY_PREV_VOLDOWN:
        logi("dev vol down");
        if(!usbd_is_target_maybe_ps4()) {
            usbd_hid_key_push(HID_KEY_VOL_DOWN);
        } else {
            am_playback_volume_down();
        }
        break;

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
    case KEY_EVENT_H | KEY_NEXT_VOLUP:
        logi("dev vol up");
        if(!usbd_is_target_maybe_ps4()) {
            usbd_hid_key_push(HID_KEY_VOL_UP);
        } else {
            am_playback_volume_up();
        }
        break;

    case KEY_EVENT_M | KEY_M | (4<<8):
        wrx_ctrl_sel = 0;
        logi("delete linkkey 0\r\n");
        g24_dongle_delete_link_process();
        break;

    case KEY_EVENT_M | KEY_M | (5<<8):
        wrx_ctrl_sel = 1;
        logi("delete linkkey 1\r\n");
        g24_dongle_delete_link_process();
        break;
        
    case KEY_EVENT_L | KEY_PLAY:
        logi("delete linkkey cur\r\n");
        g24_dongle_delete_link_process();
        break;

    case KEY_EVENT_SU | KEY_M:
    //case KEY_EVENT_SU | KEY_SW:
        {
#if AUDIO_MIC_REVERB
            uint8_t fdnrev_enable = fdnrev_enable_get();
            fdnrev_enable = mic_fdnrev_enable_set(!fdnrev_enable);
            logi("fdnrev sw:%d\r\n",fdnrev_enable);
#if MYME_M4_BOARD
            key_led_set_status(led4_green, fdnrev_enable);
#endif
#if EVB_RX_DEMO
            if(fdnrev_enable) {
                REVERB_DISP_ON();
            } else {
                REVERB_DISP_OFF();
            }
#endif
#endif
        }
        break;

	case KEY_EVENT_L | KEY_SW:
        wrx_ctrl_sel = !wrx_ctrl_sel;
        logi("wrx_ctrl_sel:%d\r\n",wrx_ctrl_sel);
        break;

    case G24_EVENT_SLAVE_SAVE_BDADDR:
        logi("save bdaddr\r\n");
        break;

    case G24_EVENT_MASTER_SAVE_LNK:
        logi("##save linkkey\r\n");
        break;

    case G24_EVENT_DISCONNECT:
    {
        uint8_t link = *params;
        g24_mode->connect[link] = false;
        logi("disconnect g24\r\n");
        systemStatus.signalStrength = SIGNAL_NONE;
#if MYME_M4_BOARD
        key_led_set_g24_connect_status(link, 0);
#endif
#if (DISP_EN && STATE_BT_DIS_EN)
        disp_state(STATE_BT_DIS);
#endif

#if LCDC_EN && EVB_LCD_DEMO                        //清空能量条
        lcd_mic_eng_flush(link);
#endif
        break;
    }

    case G24_EVENT_MONITOR_START:
        logi("G24_EVENT_MONITOR_START\r\n");
    case G24_EVENT_CONNECT:
    case G24_EVENT_MONITOR_CONNECT:                 //连上监听设备
    {
        uint8_t link = *params;
        g24_mode->connect[link] = true;
        logi("g24 connect link %d\r\n", link);

        if(!g24_monitor_en_get()) {
#if !G24_DUAL_CHANNEL
            g24_local_addr_get(addr);
            bt_top_set_link_addr(link,addr);                 //set reconnect addr
#endif
            g24_link_info_save(link);
        }

#if MYME_M4_BOARD
        key_led_set_g24_connect_status(link, 1);
#endif
#if (DISP_EN && STATE_BT_CON_EN)
        disp_state(STATE_BT_CON);
#endif
#if LCDC_EN && EVB_LCD_DEMO 
        lcd_link_status_flush(link);
#endif

#if  POWER_LPC_MODE_EN
        monitor_device_connect_set(true);
#endif
		break;
    }

    case G24_EVENT_POWEROFF_REQ:
        logi("G24_EVENT_POWEROFF_REQ\n");
        bt_top_clear_link_flg(0);                           //clear flag and will discover new device
        bt_top_disconnect(0);
        break;
    case EQ_EVENT_SET_EQ_DATA_ISR:
#if AUDIO_MIC_REVERB
        mic_fdnrev_refresh();
#endif
        break;

#if LCDC_EN && EVB_LCD_DEMO 
    case G24_EVENT_ENG0_UPDATE:
        lcd_mic_eng_flush(0);
        break;

    case G24_EVENT_ENG1_UPDATE:
        lcd_mic_eng_flush(1);
        break;

    case G24_EVENT_LINK0_STA_UPDATE:
        lcd_link_status_flush(0);
        break;

    case G24_EVENT_LINK1_STA_UPDATE:
        lcd_link_status_flush(1);
        break;
#endif  

    }
}
bool bt_sco_disconnect(void);
bool bt_sco_connect(void);

void bt_ui_process_do(uint32_t event, uint32_t params_len, uint8_t *params)
{
    //logi("bt_ui_process_do %x", event);
    switch(event){
        case KEY_EVENT_SU | KEY_NUM18:
            //mode_set_force(MODE_IDLE);
            {
                static bool aaa = 0;
                if(aaa == 0) {
                    bt_sco_disconnect();
                } else {
                    bt_sco_connect();
                }
                aaa = !aaa;
            }
            break;
        case KEY_EVENT_SU | KEY_SW:
            logi("key sw");
            if(mode_sta_get() == MODE_BT) {
#if MODE_G24_EN
                work_mode_set(WORK_MODE_SET_G24);
                link_info_save();
                restart();
#endif
            } 
            break;
        case KEY_EVENT_SU | KEY_NEXT:
        case KEY_EVENT_SU | KEY_NEXT_VOLUP:
            logi("bt next");
            if (am_btaudio_bt_calling_state_get() == BT_CALLING_STATE_NULL) {
                bt_audio_forward();
            }
            break;
        case KEY_EVENT_SU | KEY_VOL_UP:
            am_btaudio_vol_up();
            break;
        
        case KEY_EVENT_SU | KEY_PREV:
        case KEY_EVENT_SU | KEY_PREV_VOLDOWN:
            logi("bt pre");
            if (am_btaudio_bt_calling_state_get() == BT_CALLING_STATE_NULL) {
                logi("bt audio pre");
                bt_audio_backward();
            }
            break;
        case KEY_EVENT_SU | KEY_VOL_DOWN:
            am_btaudio_vol_down();
            break;

        case KEY_EVENT_SU | KEY_PLAY:
            logi("bt play");
            if (am_btaudio_bt_calling_state_get() == BT_CALLING_STATE_NULL) {
                audio_play_pause();
            } else {
                am_btaudio_calling_process();
            }
            
            break;
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
        if ((state == STATE_BAT_FULL) || (bat_full_when_charging == true)) {
            if(charge_mode_sta_get()) {
                disp_led_onoff_set(LED_RED, 0);
            }
            bat_full_when_charging = true;
        } else {
            if(charge_mode_sta_get()) {
                disp_led_onoff_set(LED_RED, 1);
            }
        }
    } else {
        disp_led_flash_set(LED_RED,1000,1000,0xffffffff);
    }

    if(is_in_mode_g24()) {
        if(g24_mode->connect[0] && g24_mode->connect[1]) {
            disp_led_onoff_set(LED_BLUE, 1);
        } else if ((g24_mode->connect[0] && !g24_mode->connect[1]) ||
            (!g24_mode->connect[0] && g24_mode->connect[1])) {
            disp_led_flash_set(LED_BLUE,200,200,0xffffffff);
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



