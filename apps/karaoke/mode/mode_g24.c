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
#include "g24.h"
#include "mode_g24.h"
#include "g24_api.h"

#include "ui.h"
#include "tws_api.h"
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
#include "sensor.h"
#include "anc_service.h"
#include "ble52_api.h"
#include "app_psm.h"
#include "psm_manager.h"
#include "app_audio_anc.h"
#include "kws_manager.h"
#include "air_api.h"
#include "mode_bt.h"
#include "sys_version.h"
#include "rfdss.h"
#include "soundcard.h"
#include "sbt_api.h"
#include "app_record.h"
#include "app_lcdc.h"
#include "app_lcd_font.h"
#include "lcd_display.h"

void g24_ui_initialize();
void g24_bt_e_rspdsb_set(bool tx_dis);
extern void g24_dongle_delete_link_process(void);

#if MODE_G24_EN

#if EVB_TX_DEMO || EVB_RX_DEMO || (G24_2T4_SHORT_PACKET_3TIMES || G24_2T4_WITH_MONO_MONITOR)
#define USER_FREQ_EN                1           //使用自定义频段
#else
#define USER_FREQ_EN                0           //使用自定义频段
#endif

#define G24_PAGE_CODE_TEST          0

OAL_THREAD g24_isr_bh_handler;
OAL_THREAD g24_lmp_handler;
OAL_THREAD g24_main_task_handler;
OAL_TIMER  wireless_scan_timer_handler;
OAL_THREAD IDUPDATA_task_handler;
uint32_t   g24_handsetID;   //dongle 和handset公用
uint32_t  master_mic_not_match_count;
uint32_t broadcast_code = 0;
uint32_t g24_channel_ID = 0;

g24_mode_t g24_mode;
OAL_SEM sem_g24_connect = NULL;
bool in_mode_g24;
bool mode_g24_exit_flag;

bool g24_2T4_group;             //0: 副芯片一组，1:主芯片一组
extern uint8_t sbt_phy_rate; //default 1

#if G24_PAGE_CODE_TEST
uint32_t g24_page_gen_code;
uint32_t g24_page_gen_code_get;
#endif


#if USER_FREQ_EN //使用RFPLL_config_6801.xlsx生成
short user_freq_int_list[86] = {
#if 0
	//from 2402M~2487
	0xc8, 0xc8, 0xc8, 0xc8, 0xc8, 0xc8, 0xc8, 0xc8, 0xc8, 0xc8, 0xc8, 0xc8,
	0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9, 0xc9,
	0xca, 0xca, 0xca, 0xca, 0xca, 0xca, 0xca, 0xca, 0xca, 0xca, 0xca, 0xca,
	0xcb, 0xcb, 0xcb, 0xcb, 0xcb, 0xcb, 0xcb, 0xcb, 0xcb, 0xcb, 0xcb, 0xcb,
	0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,
	0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd, 0xcd,
	0xce, 0xce, 0xce, 0xce, 0xce, 0xce, 0xce, 0xce, 0xce, 0xce, 0xce, 0xce,
	0xcf, 0xcf
#endif
#if 1
    //from 2302M~2387
    0xBF,0xBF,0xBF,0xBF,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,
    0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC1,0xC1,0xC1,0xC1,
    0xC1,0xC1,0xC1,0xC1,0xC1,0xC1,0xC1,0xC1,0xC2,0xC2,
    0xC2,0xC2,0xC2,0xC2,0xC2,0xC2,0xC2,0xC2,0xC2,0xC2,
    0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,0xC3,
    0xC3,0xC3,0xC4,0xC4,0xC4,0xC4,0xC4,0xC4,0xC4,0xC4,
    0xC4,0xC4,0xC4,0xC4,0xC5,0xC5,0xC5,0xC5,0xC5,0xC5,
    0xC5,0xC5,0xC5,0xC5,0xC5,0xC5,0xC6,0xC6,0xC6,0xC6,
    0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,
#endif
#if 0
    //from 2502M~2587
    0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD0,0xD1,0xD1,
    0xD1,0xD1,0xD1,0xD1,0xD1,0xD1,0xD1,0xD1,0xD1,0xD1,
    0xD2,0xD2,0xD2,0xD2,0xD2,0xD2,0xD2,0xD2,0xD2,0xD2,
    0xD2,0xD2,0xD3,0xD3,0xD3,0xD3,0xD3,0xD3,0xD3,0xD3,
    0xD3,0xD3,0xD3,0xD3,0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,
    0xD4,0xD4,0xD4,0xD4,0xD4,0xD4,0xD5,0xD5,0xD5,0xD5,
    0xD5,0xD5,0xD5,0xD5,0xD5,0xD5,0xD5,0xD5,0xD6,0xD6,
    0xD6,0xD6,0xD6,0xD6,0xD6,0xD6,0xD6,0xD6,0xD6,0xD6,
    0xD7,0xD7,0xD7,0xD7,0xD7,0xD7,
#endif
};
int user_freq_frac_list[86] = {
#if 0
    //from 2402M~2487
	0x000000, 0x02aaaa, 0x055555, 0x080000, 0x0aaaaa, 0x0d5555, 0x100000, 0x12aaaa, 0x155555,0x180000, 0x1aaaaa, 0x1d5555,
	0x000000, 0x02aaaa, 0x055555, 0x080000, 0x0aaaaa, 0x0d5555, 0x100000, 0x12aaaa, 0x155555,0x180000, 0x1aaaaa, 0x1d5555,
	0x000000, 0x02aaaa, 0x055555, 0x080000, 0x0aaaaa, 0x0d5555, 0x100000, 0x12aaaa, 0x155555,0x180000, 0x1aaaaa, 0x1d5555,
	0x000000, 0x02aaaa, 0x055555, 0x080000, 0x0aaaaa, 0x0d5555, 0x100000, 0x12aaaa, 0x155555,0x180000, 0x1aaaaa, 0x1d5555,
	0x000000, 0x02aaaa, 0x055555, 0x080000, 0x0aaaaa, 0x0d5555, 0x100000, 0x12aaaa, 0x155555,0x180000, 0x1aaaaa, 0x1d5555,
	0x000000, 0x02aaaa, 0x055555, 0x080000, 0x0aaaaa, 0x0d5555, 0x100000, 0x12aaaa, 0x155555,0x180000, 0x1aaaaa, 0x1d5555,
	0x000000, 0x02aaaa, 0x055555, 0x080000, 0x0aaaaa, 0x0d5555, 0x100000, 0x12aaaa, 0x155555,0x180000, 0x1aaaaa, 0x1d5555,
	0x000000, 0x02aaaa
#endif
#if 1
    //from 2302M~2387
    0x155555,0x180000,0x1AAAAA,0x1D5555,0x000000,0x02AAAA,0x055555,0x080000,0x0AAAAA,0x0D5555,
    0x100000,0x12AAAA,0x155555,0x180000,0x1AAAAA,0x1D5555,0x000000,0x02AAAA,0x055555,0x080000,
    0x0AAAAA,0x0D5555,0x100000,0x12AAAA,0x155555,0x180000,0x1AAAAA,0x1D5555,0x000000,0x02AAAA,
    0x055555,0x080000,0x0AAAAA,0x0D5555,0x100000,0x12AAAA,0x155555,0x180000,0x1AAAAA,0x1D5555,
    0x000000,0x02AAAA,0x055555,0x080000,0x0AAAAA,0x0D5555,0x100000,0x12AAAA,0x155555,0x180000,
    0x1AAAAA,0x1D5555,0x000000,0x02AAAA,0x055555,0x080000,0x0AAAAA,0x0D5555,0x100000,0x12AAAA,
    0x155555,0x180000,0x1AAAAA,0x1D5555,0x000000,0x02AAAA,0x055555,0x080000,0x0AAAAA,0x0D5555,
    0x100000,0x12AAAA,0x155555,0x180000,0x1AAAAA,0x1D5555,0x000000,0x02AAAA,0x055555,0x080000,
    0x0AAAAA,0x0D5555,0x100000,0x12AAAA,0x155555,0x180000,
#endif
#if 0
    //from 2502M~2587
    0x0AAAAA,0x0D5555,0x100000,0x12AAAA,0x155555,0x180000,0x1AAAAA,0x1D5555,0x000000,0x02AAAA,
    0x055555,0x080000,0x0AAAAA,0x0D5555,0x100000,0x12AAAA,0x155555,0x180000,0x1AAAAA,0x1D5555,
    0x000000,0x02AAAA,0x055555,0x080000,0x0AAAAA,0x0D5555,0x100000,0x12AAAA,0x155555,0x180000,
    0x1AAAAA,0x1D5555,0x000000,0x02AAAA,0x055555,0x080000,0x0AAAAA,0x0D5555,0x100000,0x12AAAA,
    0x155555,0x180000,0x1AAAAA,0x1D5555,0x000000,0x02AAAA,0x055555,0x080000,0x0AAAAA,0x0D5555,
    0x100000,0x12AAAA,0x155555,0x180000,0x1AAAAA,0x1D5555,0x000000,0x02AAAA,0x055555,0x080000,
    0x0AAAAA,0x0D5555,0x100000,0x12AAAA,0x155555,0x180000,0x1AAAAA,0x1D5555,0x000000,0x02AAAA,
    0x055555,0x080000,0x0AAAAA,0x0D5555,0x100000,0x12AAAA,0x155555,0x180000,0x1AAAAA,0x1D5555,
    0x000000,0x02AAAA,0x055555,0x080000,0x0AAAAA,0x0D5555,
#endif
};
#endif

AT_SRAM(.bt_24g_isr_comm_seg)
bool is_in_mode_g24(void)
{
    return in_mode_g24;
}

AT_SRAM(.bt_24g_isr_comm_seg)
bool is_mode_g24_exit(void)
{
    return mode_g24_exit_flag;
}


AT_SRAM(.bt_24g_isr_comm_seg)
void g24_lm_rx_rssi_check_log(uint8_t rssi_cal, bool is_match)
{
    //logi("rssi_cal = %d, is_match = %d",rssi_cal,is_match);
}

AT_SRAM(.bt_24g_isr_comm_seg)
void g24_err_patck_check(uint8_t link, uint16_t err, uint16_t rel_err, uint16_t total)
{
    logi("[%d]  %d {%d} / %d",link,err, rel_err, total);
}

AT_SRAM(.bt_g24_isr_handset)
uint32_t g24_monitor_m_mic_id_get()
{
    return g24_handsetID;
}

//在即将连接上的时候发送信息处理
AT_SRAM(.bt_24g_isr_comm_seg)
void g24_paircode_gen(uint8_t link, uint32_t *datap)
{
#if G24_PAGE_CODE_TEST
    logi("###########g24_paircode_gen %x  ", g24_page_gen_code_get);
    *datap = g24_page_gen_code_get;
#endif

#if (G24_2T4_SHORT_PACKET_3TIMES ) && G24_2T4_HANDSET_SAME_FW	//G24_2T4_WITH_MONO_MONITOR
    if(G24_FUNC_ROLE == G24_FUNC_ROLE_DONGLE) {     //发送主副组信息
        *datap = g24_2T4_group;
        //logi("**pcode:%#x", *datap);
    }
#endif

#if G24_MONITOR_EN
#if (G24_FUNC_ROLE == 1) //dongle
        *datap = 0x00 | g24_audio_get_m_monitor_id() << 8;
#else   //handset

#endif  //(G24_FUNC_ROLE == G24_FUNC_ROLE_DONGLE) 
#endif  //G24_MONITOR_EN

}

AT_SRAM(.bt_24g_isr_monitor_check_seg)
bool g24_monitor_pair_check(uint8_t link, uint32_t *datap)
{
    logi("g24_monitor_connect_check............ %x", *datap);
#if 0
    /*
        1拖N时：
        当好几个handset，A,B,C先启动，dongle发的包handset全部收到，就会导致可能B,Chandset的包
        发到dongle没有成功，但由于前面一个Ahandset的包发成功了，dongle有会应，因此B,C能够受到
        dongle发给A的回应，因此,B,和C也认为已经发成功了，从而进入音频阶段；

        下面这个机制只能剔除掉大部分的连接，上面说的的情况是避免不了的。
        因此还需要标记住主MIC ID，用来通知当前主MIC是哪个，handset受到后，如果自己是主MIC
        但是ID不匹配，那么需要自己断开
    */
#if (G24_FUNC_ROLE == 0)    //handset
    if((*datap & 0xFF) == e_G24_MASTER_MIC_CONNECTED){  //主MIC
        logi("a master mic is discoverd..");
        g24_monitor_master_mic_connect_set(true);
#if G24_MONITOR_MASTER_EN
        master_mic_not_match_count = 0;
        g24_monitor_master_mic_tx_enanle(false);
        logi("refuse to connect dongle");
        if(bt_top_task_status_get(0) >= BT_PAGE) {
            g24_mid_stop_task(0);
        }
        bt_top_link_enable_set(0,FALSE);
        
        event_put(G24_EVENT_MONITOR_TX_ENABLE, 0, 0);
        return false;   //如果handset收到dongle握手数据并且dongle已经连接过主MIC，不再连了
#endif
        
    } else {
        logi("dongle have no master mic before, %x", *datap);
        g24_monitor_master_mic_connect_set(false);
#if G24_MONITOR_MASTER_EN
        g24_monitor_master_mic_tx_enanle(true);
#endif
    }
//end handset process
#else   //dongle
    logi("data rec %x", *datap);
    if((*datap & 0xFF) == e_G24_MIC_MASTER) {    //主MIC连接
        if(g24_is_monitor_master_mic_connected()) {
            //logi("dongle refuse another master mic's connection");
            return 0;   //如果dongle已经连接过主MIC，不再连了
        }
        if(g24_handsetID == 0) {
            g24_handsetID = *datap >> 8;
            //主MIC设置一下数据中ID，1拖N的主MIC需要每包数据带上自己的ID，
            //这样dongle才能确定当前匹配的MIC是否连接着
            g24_audio_set_m_monitor_id(g24_handsetID);
        }
        logi("master mic connect....%x ", g24_handsetID);
        g24_monitor_master_mic_connect_set(true);   //标记主MIC已经连接
    }else {
        logi("vice mic connect....");
    }
#endif
#endif
    return 1;
}

AT_SRAM(.bt_24g_isr_monitor_check2_seg)
bool g24_monitor_pair_check2(uint8_t link, uint32_t *datap)
{
#if G24_MONITOR_EN
#if (G24_FUNC_ROLE == 0) //handset
#if G24_MONITOR_MASTER_EN
    uint32_t dongles_master_mic_ID = *datap >> 8;
    logi("g24_monitor_pair_check2 %x", dongles_master_mic_ID);
    if((dongles_master_mic_ID == g24_handsetID) || (dongles_master_mic_ID == 0)) {
        return 1;
    } else{
        master_mic_not_match_count = 0;
        g24_monitor_master_mic_tx_enanle(false);
        logi("refuse to connect dongle");
        if(bt_top_task_status_get(0) >= BT_PAGE) {
            g24_mid_stop_task(0);
        }
        bt_top_link_enable_set(0,FALSE);
        
        event_put(G24_EVENT_MONITOR_TX_ENABLE, 0, 0);
        
        return 0;
    }
#else   //vice mic
    g24_monitor_master_mic_tx_enanle(false);

    uint32_t dongles_master_mic_ID = *datap >> 8;
    logi("#g24_monitor_pair_check2 %x", dongles_master_mic_ID);
    if(dongles_master_mic_ID != 0) {
        g24_monitor_master_mic_connect_set(true);
    } else {
        g24_monitor_master_mic_connect_set(false);
    }
#endif  //G24_MONITOR_MASTER_EN
//<<<<<-----------
#elif (G24_FUNC_ROLE == 1)  //dongle

#endif  //#if (G24_FUNC_ROLE == 0)

#endif
    return 1;
}


//在即将连接上的时候接收信息处理
AT_SRAM(.bt_24g_isr_comm_seg)
bool g24_paircode_check(uint8_t link, uint32_t *datap)
{
#if G24_PAGE_CODE_TEST
        if(g24_page_gen_code == *datap) {
            printf_debug("g24_paircode_check code OK :%#x\n", *datap);
            return true;
        } else {
            printf_debug("g24_paircode_check code ERR:%#x\n", *datap);
            return false;
        }
#endif

#if (G24_2T4_SHORT_PACKET_3TIMES ) && G24_2T4_HANDSET_SAME_FW	//|| G24_2T4_WITH_MONO_MONITOR
    if(G24_FUNC_ROLE == G24_FUNC_ROLE_HANDSET) {    //根据主副组信息设置不同频点
        g24_2T4_group = *datap;
        //logi("##pcode:%#x", g24_2T4_group);
        bool ret = 0;
        if(g24_2T4_group) {   //主组
            //ret = p24g_channel_map_set(0xFFFFFFFC, 0x0000003F, 0x0000, 27);        //2-37
            ret = p24g_channel_map_set(0x01FFFFFF, 0x00000000, 0x0000, 20);		//0-24
        } else {              //副组
            //ret = p24g_channel_map_set(0x00000000, 0xFFFFFE00, 0x1FFF, 27);        //41-76
            ret = p24g_channel_map_set(0x00000000, 0xFFC00000, 0x7FFF, 20);		 //54-78
        }
        if(ret == 0) {
            loge("map err");
        }
    }
#endif

#if G24_MONITOR_EN
    return g24_monitor_pair_check2(link, datap);
#endif

    return 1;   //check sucess
}

//在page时候发送信息处理
AT_SRAM(.bt_24g_isr_comm_seg)
void g24_page_user_data_gen(uint8_t link, uint32_t *datap, uint8_t max_len)
{
#if G24_PAGE_CODE_TEST
    datap[0] = g24_page_gen_code;
    logi("########## g24_page_user_data_gen %x", datap[0]);
#endif
}

//在page的时候接收信息处理，返回0:不允许连接，返回1:允许连接，
AT_SRAM(.bt_24g_isr_comm_seg)
bool g24_page_user_data_rec(uint8_t link, uint32_t *datap, bool *access_code_change, uint8_t max_len)
{
#if G24_PAGE_CODE_TEST
    g24_page_gen_code_get =  datap[0];
    printf_debug("============datap rec= %#x\n", datap[0]);
    *access_code_change = true;
#endif
    return true;
}


uint32_t rx_power_cnt[2] = {0, 0};
int32_t rx_power_total[2] = {0, 0};

//连接上后的rx power检测，power dBm
AT_SRAM(.bt_24g_isr_comm_seg)
void g24_rx_power_check(uint8_t link, int8_t power)
{
    //此函数在中断处理，不能调用打印过于频繁或其他占用长时间操作
#if 1
    rx_power_total[link] += power;
	rx_power_cnt[link] ++;
	if(rx_power_cnt[link] == 5000){
		rx_power_total[link] = rx_power_total[link]/5000;
		//printf_debug("rx power[%d]:%d\n",link,rx_power_total[link]);
        logi("rx power[%d]:%d\n",link,rx_power_total[link]);

#if !G24_FUNC_ROLE 
#if LCD_SEG_EN//信号
        if(rx_power_total[link] > -30)
        {
            displayUpdate(Signal_FourBars,true);
        }
        else if(rx_power_total[link] > -50)
        {
            displayUpdate(Signal_ThreeBars,true);
        }
        else if(rx_power_total[link] > -70)
        {
            displayUpdate(Signal_TwoBars,true);
        }
        else if(rx_power_total[link] > -100)
        {
            displayUpdate(Signal_OneBar,true);
        }
#else
        if(rx_power_total[link] > -30)
        {
            systemStatus.signalStrength = SIGNAL_FULL;
            logi("gImage_xinhao4 \r\n");
        }
        else if(rx_power_total[link] > -50)
        {
            systemStatus.signalStrength = SIGNAL_THREE_BARS;
            logi("gImage_xinhao3 \r\n");
        }
        else if(rx_power_total[link] > -70)
        {
            systemStatus.signalStrength = SIGNAL_TWO_BARS;
            logi("gImage_xinhao2 \r\n");
        }
        else if(rx_power_total[link] > -100)
        {
            systemStatus.signalStrength = SIGNAL_ONE_BAR;
            logi("gImage_xinhao1 \r\n");
        }   

#endif
#endif

		rx_power_cnt[link] = 0;
		rx_power_total[link] = 0;
	}
#endif
}

//使用一拖二sbt连接配对阶段，参数传入当前rssi_cs dBm，remote_addr为对方设备6byte地址，要求返回是否允许连接
AT_SRAM(.bt_24g_isr_comm_seg)
bool g24_rx_rssi_con_en_get(uint8_t link, int8_t rssi_cs, uint8_t *remote_addr)
{
    //此函数在中断处理，不能调用打印过于频繁或其他占用长时间操作
#if G24_MONITOR_EN
#if G24_FUNC_ROLE == 0 //handset
    if(broadcast_code == ALL_PAIR)
	{
		return true;	
	}
#endif
#endif
#if 1
    if(g24_monitor_en_get()) {
        //slaver保存master地址，slaver发master的地址给master，master发master的地址给slaver，对比一样才允许连接
#if G24_DEVICE_ROLE     //master
        if(((remote_addr[0]==0x00)&&(remote_addr[1]==0x00)&&(remote_addr[2]==0x00)&&(remote_addr[3]==0x00)&&(remote_addr[4]==0x00)&&(remote_addr[5]==0x00)) || \
            ((remote_addr[0]==0xFF)&&(remote_addr[1]==0xFF)&&(remote_addr[2]==0xFF)&&(remote_addr[3]==0xFF)&&(remote_addr[4]==0xFF)&&(remote_addr[5]==0xFF))) {
            //logi("bypass");
            return true;   //没有回连地址则可以连接
        } else {           //slaver已经连接过这个master或其他master
            uint8_t addr[6];
            g24_local_addr_get(addr);
            if(memcmp(addr, remote_addr, 6) == 0) {
                return true;
            } else {
                return false;
            }
        }
#else
        uint8_t addr[6];
        g24_remote_addr_get(addr);
        if(((addr[0]==0x00)&&(addr[1]==0x00)&&(addr[2]==0x00)&&(addr[3]==0x00)&&(addr[4]==0x00)&&(addr[5]==0x00)) || \
            ((addr[0]==0xFF)&&(addr[1]==0xFF)&&(addr[2]==0xFF)&&(addr[3]==0xFF)&&(addr[4]==0xFF)&&(addr[5]==0xFF))) {
            //logi("bypass");
            return true;   //没有回连地址则可以连接
        } else {
            if(memcmp(addr, remote_addr, 6) == 0) {
                return true;
            } else {
                return false;
            }
        }
#endif
    }

    return true;
#else
    if(rssi_cs < -50) {     //该值需要根据距离测试而定
        return false;
    }
    return true;
#endif
}


AT_SRAM(.bt_24g_isr_comm_seg)
g24_mode_t *g24_mode_param_get(void)
{
    return &g24_mode;
}

void g24_rf_power_force(uint8_t enable, uint8_t power)
{
    rf_power_force(enable, power);//FIXME:chenjian
}


void g24_rf_maxpower_set(uint8_t enable, uint8_t power)
{
    if(enable==1){
        bt_rf_power_max_set(power);
    }
}

uint8_t g24_addr_default[] = G24_ADDR_DEFAULT;

void g24_addr_default_get(uint8_t *addr)
{
    libc_memcpy(addr, g24_addr_default, 6);
#if G24_ADDR_USE_RANDOM
    uint32_t random_num = HAL_trng_get_data();
    libc_memcpy(addr, &random_num, 4);
#endif
}

uint32_t monitor_detect_stamp;
void monitor_handset_detect_stamp_set()
{
    uint64_t timestamp = HAL_get_timestamp();
    uint32_t detect_timestamp = HAL_timestamp_to_ms(timestamp);
    logi("monitor device id %x", (uint32_t)timestamp);
    monitor_set_slave_id(0, timestamp); //用时间戳作为副MIC的ID
    monitor_detect_stamp = detect_timestamp% 100 * 100;  //10秒内
    if(monitor_detect_stamp < 3000) {
        monitor_detect_stamp += 3000;    //至少3秒才触发一次心跳
    }
    logi("monitor_detect_stamp %d .......", monitor_detect_stamp);
}

AT_SRAM(.handset_stamp)
uint32_t monitor_handset_detect_stamp_get()
{
    return monitor_detect_stamp;
}

#if (G24_2T4_SHORT_PACKET_3TIMES || G24_2T4_WITH_MONO_MONITOR)

volatile bool wait_slaver = true;

#if 1	//1TN+2T4三芯片同步机制
AT_SRAM(.bt_24g_isr_comm_seg)
void g24_2T4_1TN_slaver_sync_interrupt(void)
{
#if G24_FUNC_ROLE
#if !G24_2T4_DONGLE_MCHIP  //schip
#if G24_1TN_DONGLE_MCHIP   //1tn
	if (HAL_gpio_irq_pending_get(G24_1TN_TRIG_INPUT_PIN_NUM)) {
		 HAL_gpio_irq_pending_clear(G24_1TN_TRIG_INPUT_PIN_NUM);
		 
		 OAL_enter_critical_section();
		 if(!HAL_gpio_value_get(G24_1TN_TRIG_INPUT_PIN_NUM)) {
			 //清0 BT计数, 主副芯片需要同时设置以保证2.4G TX同步
			 g24_bt_cnt_clear();
			 
			 wait_slaver = false;
			 //HAL_gpio_config(PIN_B30 | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_HIGH);
			 //HAL_gpio_config(PIN_B30 | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_LOW);

			 HAL_gpio_irq_config(G24_1TN_TRIG_INPUT_PIN_NUM, PIN_IRQ_DIS);
			 //logi("1tn*****");
		 }
		 OAL_exit_critical_section();
	 }

#else
    if (HAL_gpio_irq_pending_get(G24_2T4_TRIG_INPUT_PIN_NUM)) {
    	HAL_gpio_irq_pending_clear(G24_2T4_TRIG_INPUT_PIN_NUM);
		OAL_enter_critical_section();
        if(!HAL_gpio_value_get(G24_2T4_TRIG_INPUT_PIN_NUM)) {
            //清0 BT计数, 主副芯片需要同时设置以保证2.4G TX同步
            g24_bt_cnt_clear();
			
			//HAL_gpio_config(PIN_B0 | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_HIGH);
			//HAL_gpio_config(PIN_B0 | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_LOW);
			wait_slaver = false;
            HAL_gpio_irq_config(G24_2T4_TRIG_INPUT_PIN_NUM, PIN_IRQ_DIS);
			//logi("2t4*****");
        }
		OAL_exit_critical_section();
		
    }
#endif
#endif
#endif
}


void g24_2T4_master_slaver_sync(void)
{
    //logi("g24_2T4_master_slaver_sync\n");
#if G24_FUNC_ROLE  //dongle

#if G24_2T4_DONGLE_MCHIP
#if G24_1TN_DONGLE_MCHIP
	HAL_gpio_config(G24_1TN_TRIG_INPUT_PIN_NUM |PIN_FUNCINPUT | PIN_PULL_DISABLE);
	HAL_gpio_config(G24_2T4_TRIG_INPUT_PIN_NUM |PIN_FUNCINPUT | PIN_PULL_DISABLE);
	HAL_gpio_config(G24_1TN_TRIG_OUTPUT_PIN_NUM | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_HIGH);  //拉高output让slaver准备
	HAL_gpio_config(G24_2T4_TRIG_OUTPUT_PIN_NUM | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_HIGH);  //拉高output让slaver准备

	OAL_enter_critical_section();
	while(wait_slaver) {

		if(HAL_gpio_value_get(G24_1TN_TRIG_INPUT_PIN_NUM) && 
			HAL_gpio_value_get(G24_2T4_TRIG_INPUT_PIN_NUM)) 
		{	
			wait_slaver = false;
			//HAL_udelay(4);
			HAL_gpio_config(G24_2T4_TRIG_OUTPUT_PIN_NUM | PIN_FUNCOUTPUT | PIN_DRI3 | PIN_LOW);
			HAL_gpio_config(G24_1TN_TRIG_OUTPUT_PIN_NUM | PIN_FUNCOUTPUT | PIN_DRI3 | PIN_LOW);  //拉低output让slaver清0计数
			//HAL_gpio_config(PIN_D10 | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_HIGH);
 			//HAL_gpio_config(PIN_D10 | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_LOW);
			g24_bt_cnt_clear();

			//logi("clear****");
			
			break;
		}  
    }
	OAL_exit_critical_section();
#endif	//	if/else G24_1TN_DONGLE_MCHIP
#else	//	else G24_2T4_DONGLE_MCHIP (slave_1tn/2t4 chip)

#if G24_1TN_DONGLE_MCHIP	//	if (1tn_chip existence)
		HAL_gpio_func_set(G24_1TN_TRIG_INPUT_PIN_NUM, PIN_FUNCEINT);
		HAL_gpio_irq_clk_config(G24_1TN_TRIG_INPUT_PIN_NUM, (0x00<<8)|(0x1<<7)|(0x00<<4));//时钟，防抖
		HAL_gpio_irq_config(G24_1TN_TRIG_INPUT_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_F_EDGE);
		HAL_gpio_config(G24_1TN_TRIG_OUTPUT_PIN_NUM | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_HIGH);  //拉高output应答master		
		uint32_t ts = HAL_timestamp_to_ms(HAL_get_timestamp());
		while(!HAL_gpio_value_get(G24_1TN_TRIG_INPUT_PIN_NUM)) {
        if((HAL_timestamp_to_ms(HAL_get_timestamp()) - ts) > 3000) {
            break;
        	}
   		}
		
#else
		HAL_gpio_func_set(G24_2T4_TRIG_INPUT_PIN_NUM, PIN_FUNCEINT);
		HAL_gpio_irq_clk_config(G24_2T4_TRIG_INPUT_PIN_NUM, (0x00<<8)|(0x1<<7)|(0x00<<4));//时钟，防抖
		HAL_gpio_irq_config(G24_2T4_TRIG_INPUT_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_F_EDGE);
		HAL_gpio_config(G24_2T4_TRIG_OUTPUT_PIN_NUM | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_HIGH);  //拉高output应答master		
		uint32_t ts = HAL_timestamp_to_ms(HAL_get_timestamp());
		while(!HAL_gpio_value_get(G24_2T4_TRIG_INPUT_PIN_NUM)) {
        if((HAL_timestamp_to_ms(HAL_get_timestamp()) - ts) > 3000) {
            break;
        	}
   		}
#endif	//	if/else G24_1TN_DONGLE_MCHIP (1tn_chip existence)
#endif  //  if/else G24_2T4_DONGLE_MCHIP (m/s chip)
#endif	//	if G24_FUNC_ROLE (dongle)
}

#else	//只有2T4时同步机制
AT_SRAM(.bt_24g_isr_comm_seg)
void g24_2T4_master_slaver_sync_interrupt(void)
{
#if G24_2T4_DONGLE_MCHIP  //dongle mchip
	
    if (HAL_gpio_irq_pending_get(G24_2T4_TRIG_INPUT_PIN_NUM)) {
        HAL_gpio_irq_pending_clear(G24_2T4_TRIG_INPUT_PIN_NUM);
		OAL_enter_critical_section();
        if(HAL_gpio_value_get(G24_2T4_TRIG_INPUT_PIN_NUM)) {
            //清0 BT计数, 主副芯片需要同时设置以保证2.4G TX同步
            g24_bt_cnt_clear();
            //HAL_gpio_config(PIN_D10 | PIN_FUNCOUTPUT | PIN_DRI1 |PIN_HIGH);
			//HAL_gpio_config(PIN_D10 | PIN_FUNCOUTPUT | PIN_DRI1 |PIN_LOW);
            wait_slaver = false;

            HAL_gpio_irq_config(G24_2T4_TRIG_INPUT_PIN_NUM, PIN_IRQ_DIS);
        }
			OAL_exit_critical_section();
    }
#endif
}

void g24_2T4_master_slaver_sync(void)
{
#if G24_FUNC_ROLE  //dongle

#if G24_2T4_DONGLE_MCHIP
    HAL_gpio_func_set(G24_2T4_TRIG_INPUT_PIN_NUM, PIN_FUNCEINT);
    HAL_gpio_irq_clk_config(G24_2T4_TRIG_INPUT_PIN_NUM, (0x00<<8)|(0x1<<7)|(0x00<<4));
    HAL_gpio_irq_config(G24_2T4_TRIG_INPUT_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_R_EDGE);
    HAL_gpio_config(G24_2T4_TRIG_OUTPUT_PIN_NUM | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_HIGH);  //拉高output让slaver清0计数
    uint32_t ts = HAL_timestamp_to_ms(HAL_get_timestamp());
    while(wait_slaver) {
        if((HAL_timestamp_to_ms(HAL_get_timestamp()) - ts) > 3000) {
            HAL_gpio_irq_config(G24_2T4_TRIG_INPUT_PIN_NUM, PIN_IRQ_DIS);
            break;
        }
    }
#else
    HAL_gpio_config(G24_2T4_TRIG_INPUT_PIN_NUM |PIN_FUNCINPUT | PIN_PULL_DISABLE);
    uint32_t ts = HAL_timestamp_to_ms(HAL_get_timestamp());
    while(!HAL_gpio_value_get(G24_2T4_TRIG_INPUT_PIN_NUM)) {
        if((HAL_timestamp_to_ms(HAL_get_timestamp()) - ts) > 3000) {
            break;
        }
    }
	OAL_enter_critical_section();
    HAL_gpio_config(G24_2T4_TRIG_OUTPUT_PIN_NUM | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_HIGH);  //拉高output让master清0计数

    //清0 BT计数, 主副芯片需要同时设置以保证2.4G TX同步
    g24_bt_cnt_clear();
	//HAL_gpio_config(PIN_B0 | PIN_FUNCOUTPUT | PIN_DRI1 |PIN_HIGH);
	//HAL_gpio_config(PIN_B0 | PIN_FUNCOUTPUT | PIN_DRI1 |PIN_LOW);
	OAL_exit_critical_section();
#endif

#endif
}
#endif

bool g24_2T4_channel_setting(void)
{
//G24_1TN_DONGLE_MCHIP不在2t4函数中设置，在正常流程单独用宏控制
//主副和一拖多分三段频段，以减少干扰
#if G24_2T4_HANDSET_SAME_FW
    bool ret = true;
    g24_sbt_ctrl_chan_set(0, 39, 78);           //设置查询/连接阶段使用的信道, 2M带宽
    //主副组分开两段，并剔除了sbt影响到的信道 0/1, 38/39/40, 77/78
    if(G24_FUNC_ROLE == G24_FUNC_ROLE_DONGLE) { //连接阶段，副芯片使用不同的随机码，减少跟主芯片撞上
#if G24_2T4_DONGLE_MCHIP
        logi("-------- master chip --------");
        ret = p24g_channel_map_set(0x01FFFFFF, 0x00000000, 0x0000, 20);				//2-24
        g24_sbt_rand_seed_set(0x0);
        g24_2T4_group = true;
#else
        logi("--------2t4 slaver chip --------");
        ret = p24g_channel_map_set(0x00000000, 0xFFC00000, 0x7FFF, 20);        //54-76
        g24_sbt_rand_seed_set(0x1);
        g24_2T4_group = false;

#endif
    }
    
#else
    //主副组分开两段，并剔除了sbt影响到的信道 0/1, 38/39/40, 77/78
#if G24_2T4_DONGLE_MCHIP || G24_2T4_HANDSET_MCHIP
    ret = p24g_channel_map_set(0x01FFFFFF, 0x00000000, 0x0000, 20);				//2-24
    if(G24_FUNC_ROLE == G24_FUNC_ROLE_DONGLE) {         //连接阶段，副芯片使用不同的随机码，减少跟主芯片撞上
        g24_sbt_rand_seed_set(0x0);
    }
#else
    ret = p24g_channel_map_set(0x00000000, 0xFFC00000, 0x7FFF, 20);        //54-76
    if(G24_FUNC_ROLE == G24_FUNC_ROLE_DONGLE) {         //连接阶段，副芯片使用不同的随机码，减少跟主芯片撞上
        g24_sbt_rand_seed_set(0x01);
    }
#endif

#endif
    return ret;
}
#endif

void g24_addr_init(void)
{
    uint8_t addr[6];
    g24_earphone_addr_get(addr);
    if (((addr[0]==0x00)&&(addr[1]==0x00)&&(addr[2]==0x00)&&(addr[3]==0x00)&&(addr[4]==0x00)&&(addr[5]==0x00)) || \
        ((addr[0]==0xFF)&&(addr[1]==0xFF)&&(addr[2]==0xFF)&&(addr[3]==0xFF)&&(addr[4]==0xFF)&&(addr[5]==0xFF))) {
        libc_memcpy(addr, g24_addr_default, 6);
#if G24_ADDR_USE_RANDOM
        uint32_t random_num = HAL_trng_get_data();

        libc_memcpy(addr, &random_num, 4);
        logi("use random addr");
#endif
    }
#if !G24_DUAL_CHANNEL
    if(mode_sta_get() == MODE_G24) {
        uint8_t link_addr[6];
        if(g24_monitor_en_get()) {
#if !G24_DEVICE_ROLE                                    //slaver获取对方地址(回连地址)
            g24_remote_addr_clear();
            if(g24_link_addr_get(0,link_addr, true)) {
                g24_remote_addr_set(link_addr);

                g24_remote_addr_get(link_addr);
                logi("***********g24_remote_addr:");
                printf_array(link_addr, 6);
            }
#endif
        } else {
            if(g24_link_addr_get(0,link_addr, false)) {
                memcpy(addr, link_addr, 6);
                logi("use link addr:");
                logi("%02x:%02x:%02x:%02x:%02x:%02x",addr[5],addr[4],addr[3],addr[2],addr[1],addr[0]);
            }
        }
    }
#endif

    g24_local_addr_set(addr);
    g24_addr_save(addr);

#if G24_PAGE_CODE_TEST
    g24_page_gen_code = HAL_trng_get_data();
    printf_debug("###g24_page_gen_code = %#x\n", g24_page_gen_code);
#endif
    //logi("g24 addr:%02x:%02x:%02x:%02x:%02x:%02x",
    //    addr[5],addr[4],addr[3],addr[2],addr[1],addr[0]);
}

void g24_task_init()
{

    if(sem_g24_connect == NULL) {
        sem_g24_connect = OAL_sem_create_binary();
    }
    if (sem_g24_connect == NULL) {
        loge("g24 sem err\r\n");
    }
    g24_mode.connect[0] = false;
#if G24_DUAL_CHANNEL
    g24_mode.connect[1] = false;
#endif

    g24_addr_init();

    if (G24_DEVICE_ROLE == G24_DEVICE_ROLE_MASTER) {
        logi("-------- master role --------");
    } else {
#if G24_1TN_DONGLE_MCHIP
        logi("--------1tn slave role --------");
#else
		logi("--------2t4 slave role --------");
#endif
    }
#if (EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
     //usb_dev_enter();
#endif

	// bt config
	//dongle_role: 0-Dongle is Master role (handset need set the value to 0) ;
	//            1-Dongle is Slave role (handset need set the value to 1)
    if(G24_FUNC_ROLE == G24_FUNC_ROLE_DONGLE) {
        g24_init(!G24_DEVICE_ROLE);
        logi("-------- dongle --------");
    } else {
        g24_init(G24_DEVICE_ROLE);
        logi("-------- handset -------");
    }

    // comm init
    g24_lm_init();
    comm_init();

#if (G24_2T4_SHORT_PACKET_3TIMES || G24_2T4_WITH_MONO_MONITOR)
    g24_2T4_master_slaver_sync();
#endif
}

bool g24_is_comm_task_pause();
void *g24_task_main(void *pvParameters)
{
    logi("g24_task_main ######");
    
    while(1) {
        if(g24_is_comm_task_pause()){
            HAL_msleep(50);
            //logi("pause ... ");
            continue;
        }
        g24_task_init();
        task_comm();
    }
    
    logi("thread exit .################################ ");
    return NULL;

}


AT_SRAM(.bt_proc)
static uint32_t app_service_observer(uint32_t cookie, uint32_t cmd, uint32_t val,  void *data)
{
    uint32_t ret = OBSERVER_OK;

    switch (cmd) {
    case APP_SERVICE_OBSERVER_AIR_PARAM_GET:
        break;

    case APP_SERVICE_OBSERVER_AIR_SIRI_CONFIG_GET:
        break;

    case APP_SERVICE_OBSERVER_AIR_SIRI_STATUS_CHANGE:
        break;

    //case APP_SERVICE_OBSERVER_SENSOR_MOTION_GET:
    //    break;

    case APP_SERVICE_OBSERVER_MUSIC_PLAY_STATUS_CHANGE:
        break;

    default:
        break;
    }

    return ret;
}

void mode_g24_app_process(sys_event_info_t *info)
{
#if WATCHDOG_EN
    HAL_watchdog_refresh(NULL);
#endif

    uint32_t event = info->event;
    switch(event) {
    default:
        ui_process(event, info->params_len, info->params);
        break;
    }
}

#if POWER_LPC_MODE_EN & (G24_FUNC_ROLE == 0)
bool monitor_device_is_connected();
AT_SRAM(.wirelessscan)
void wireless_scan_en_timer_cb(void* time)
{
    //handset定时打开RF连接
    static uint8_t cnt_t0 = 0;
    if(!monitor_device_is_connected()) {
        if(cnt_t0 % 2 ) {  //
            event_put(SYSTEM_EVENT_WIRELESS_SCAN_EN, 0 , 1);    //开启连接
        }else { 
            event_put(SYSTEM_EVENT_WIRELESS_SCAN_EN, 0 , 0);    //关闭连接
        }
    }
    cnt_t0++;
}

AT_SRAM(.wirelessscan)
void wireless_scan_timer_enable(bool enable)
{
    logi("wireless_scan_timer_enable %d");
    if(wireless_scan_timer_handler) {
        OAL_timer_activate(&wireless_scan_timer_handler, !enable);   //0 :ACTICE 1: stop
    }
}

#endif

void* id_updata_Task(void *params)
{
#if !G24_FUNC_ROLE
    uint16_t id = 0;
    static bool id_flag = 0;
    while(1)
    {
        if(g24_mode.connect[0] == false) 
        {
            //logi("broadcast_code = %d\n g24_get_channel_ID() = %d\n",broadcast_code,g24_get_channel_ID());
            id = broadcast_code;
            if(id == ALL_PAIR_2)
            {
                id_flag = 1;
            }
            if(id != ALL_PAIR_2 && id_flag == 1)
            {
                g24_Channel_select(id);
                id_flag = 0;
            }
            OAL_sleep(100);
        }
        else
        {
            OAL_sleep(500);
        }
    }
#else 
    while(1)
    {
        //logi("broadcast_code = %d\n",broadcast_code);
        if(broadcast_code != ALL_PAIR_2)
        {
            OAL_sleep(500);
            logi("broadcast_code = %d\n",broadcast_code);
            if(broadcast_code != ALL_PAIR_2)
            {
                broadcast_code = ALL_PAIR_2;
            }
        }
        OAL_sleep(100);
    }
#endif
    return NULL;
}

AT_SRAM(.bt_24g_isr_comm_seg)
bool g24_found_pair_device(uint8_t link)
{
#if POWER_LPC_MODE_EN & (G24_FUNC_ROLE == 0)
    //logi("g24_found_pair_device");
    //wireless_scan_timer_enable(false);
    logi("g24_found_pair_device");
#endif

#if 0
    if(g24_is_monitor_master_mic_connected()) {
        logi("master mic have connect");
        return 0;   //do not link this device
    }
#endif    

    return 1;   //success
}


void mode_g24_init()
{
#if G24_MONITOR_EN && G24_MONITOR_MASTER_EN
#if (G24_FUNC_ROLE == 0)    //handset
    g24_handsetID = psm_get_master_mic_id();
    if((g24_handsetID == 0) || (g24_handsetID == 0xFFFFFFFF)) {
        g24_handsetID = (uint32_t)HAL_get_timestamp() & 0xFFFFFF;
        psm_save_master_mic_id(g24_handsetID);  //保存主MIC ID
    }

    g24_audio_set_m_monitor_id(g24_handsetID);
    logi("g24_handsetID %x .....", g24_handsetID);
#endif
#endif
    
    g24_register_event_handler(mode_g24_app_process);
    observer_resiger_callback((obclbk)app_service_observer, OBSERVER_APP_SERVICE);
}

AT_SRAM(.monitor_heartbreak)
void g24_monito_common_cmd_receive(uint8_t link, uint32_t monitor_id, uint8_t user_cmd)
{
    if(user_cmd == MONITOR_USER_CMD_HEARTBREAK) {
        //logi("heartbreak.");
        event_put(G24_EVENT_HEARTBREAK_RECEIVE, &monitor_id, 4);
    } else if(user_cmd == MONITOR_USER_CMD_DISCONNECT) {
        //logi("user disconnect");
        event_put(G24_EVENT_MONITOR_USER_DISCON, &monitor_id, 4);
    }
}

bool b_g24_not_match_msg_processed = true;
void g24_set_not_match_msg_processed(bool bset)
{
    b_g24_not_match_msg_processed = bset;
}

AT_SRAM(.mmic_id_check)
bool g24_is_not_match_msg_processed()
{
    return b_g24_not_match_msg_processed;
}

void g24_bt_e_rspdsb_set(bool tx_dis);
AT_SRAM(.handset_mmic_id_check)
void monitor_handset_audio_receive_processed(uint32_t mmic_id)
{
#if G24_MONITOR_EN && G24_MONITOR_MASTER_EN
    if(mmic_id != g24_handsetID && (g24_handsetID != 0) && (mmic_id != 0) ) {
        master_mic_not_match_count++;
        if(master_mic_not_match_count > 10 && b_g24_not_match_msg_processed) {
            logi("m id not match, %x %x", g24_handsetID, mmic_id);
            b_g24_not_match_msg_processed = false;
            event_put(G24_EVENT_MONITOR_MIC_ID_NOT_MATCH, (void*)&mmic_id , 4);
            return ;
        }
    } else {
        if(master_mic_not_match_count) {
            master_mic_not_match_count = 0;
            logi("not match to match");
        }
    }
    //logi("enable tx %x %x",mmic_id, g24_handsetID);
    if(!g24_is_monitor_master_mic_tx_enable()) {
        g24_bt_e_rspdsb_set(0); //打开TX
    }
    g24_monitor_master_mic_tx_enanle(true);
    
#endif
}

AT_SRAM(.dongle_mmic_id_check)
void monitor_dongle_audio_receive_processed(uint32_t mmic_id)
{

}

AT_SRAM(.bt_24g_isr_comm_seg)
void g24_broadcast_code_gen(uint32_t* broadcastcode)
{
#if G24_MONITOR_EN
//dongle生成，handset接收
#if G24_MONITOR_EN & (G24_FUNC_ROLE == 1)  //dongle
    *broadcastcode = broadcast_code;
#endif
    // broadcast_code =  ALL_PAIR_2;
    //logi("g24_broadcast_code_gen %x", *broadcastcode);
#endif
}


AT_SRAM(.bt_24g_isr_comm_seg)
void g24_broadcast_code_receive(uint32_t broadcastcode)
{
#if G24_MONITOR_EN
#if G24_MONITOR_EN & (G24_FUNC_ROLE == 0) //handset
    //logi("broadcastcode %x", broadcastcode);
    if(g24_mode.connect[0] == false) {   //未连接的时候
        broadcast_code = broadcastcode;
    }
#endif
#endif
}

#if G24_MONITOR_EN
void g24_force_all_pair(void)
{
	broadcast_code = ALL_PAIR;
}
#endif

AT_SRAM(.mmic_id_set_cb)
void monitor_dongle_set_master_mic_id(uint32_t mmic_id)
{
    logi("monitor_dongle_set_master_mic_id %x", mmic_id);
}


extern uint8_t sbt_phy_rate;
void app_g24_monitor_init(void)
{
    g24_monitor_t monitor_param;
    monitor_param.is_master                  = G24_MONITOR_MASTER_EN;
    monitor_param.is_dongle                  = G24_FUNC_ROLE;
    monitor_param.no_handshake               = G24_MONITOR_NO_HANDSHAKE;
    monitor_param.no_handshake_none_continue = G24_MONITOR_NONE_CONTINUE;
    monitor_param.audio_ch                   = G24_MONITOR_AUDIO_CH;
    g24_monitor_param_init(&monitor_param);
    g24_monitor_en_set(ENABLE);
    sbt_phy_rate = 0;
}

void g24_rf_init()
{
    g24_param_t param;
    uint32_t pair_code0 = G24_PAIR_CODE0;
    uint32_t pair_code1 = G24_PAIR_CODE1;
    if(g24_monitor_en_get()) {                  //跑一拖多监听流程使用不同配对码，区别于常规形态
        pair_code0 = G24_PAIR_CODE1;//主机
        pair_code1 = G24_PAIR_CODE0;
    }

    param.device_role                   = G24_DEVICE_ROLE;
    param.func_role                     = G24_FUNC_ROLE;
    param.pa_en                         = G24_PA_EN;
    param.pair_code0                    = pair_code0;
    param.pair_code1                    = pair_code1;
    param.rssi_thrs                     = G24_RSSI_THRS;
    param.reconnect_rssi_thrs           = G24_RECONNECT_RSSI_THRS;
    param.master_reconnect_no_rssi      = G24_MASTER_RECONNECT_NO_RSSI;
    param.slaver_reconnect_no_rssi_time = G24_SLAVER_RECONNECT_NO_RSSI_TIME;
    param.master_reconnect_time         = G24_MASTER_RECONNECT_TIME;
    param.master_reconnect_always       = G24_MASTER_RECONNECT_ALWAYS;
    param.master_inquiry_times          = G24_MASTER_INQUIRY_TIMES;
    param.master_page_times             = G24_MASTER_PAGE_TIMES;
    param.slaver_reconnect_time         = G24_SLAVER_RECONNECT_TIME;
    param.inquiry_maxpower              = G24_INQUIRY_MAXPOWER_SET_VAL;
    param.connect_maxpower              = G24_CONNECT_MAXPOWER_SET_VAL;
    param.page_replace_inquiry          = G24_PAGE_REPLACE_INQUIRY;
    param.rf_jump_en                    = G24_RF_JUMP_EN;
    param.uplink_2ch                    = G24_UPLINK_2CH;
    param.dnlink_2ch                    = G24_DNLINK_2CH;
    param.sbc_send_4times               = G24_SBC_SEND_4TIMES;
    param.dual_channel                  = G24_DUAL_CHANNEL;
    param.dual_channel_ul_long_pkt      = G24_DUAL_CHANNEL_UL_LONG_PKT;
    param.dual_audio_long_send_4times   = G24_DUAL_AUDIO_LONG_PACKET_4TIMES;
    param.dual_short_3times             = G24_DUAL_SHORT_PACKET_3TIMES;
    param.dual_2T4_short_3times         = G24_2T4_SHORT_PACKET_3TIMES;
    param.afc_en                        = false;
    param.use_sbt_connect               = true;         //一拖二和一拖多监听方案必须使能，一拖一看需求配置

    g24_param_config(&param);

    logi("pair_code0 = %d, pair_code1 = %d", pair_code0, pair_code1);

    uint8_t usbAudioMode = psm_usb_audio_mode_get();
    if(usbAudioMode == USBAUDIO_MODE_DOLBY) {
        // dolby模式不支持无线连接
        logi("device work in dolby mode, not support wireless");
        bt_top_link_enable_set(0,FALSE);            //使能link0连接流程
        bt_top_link_enable_set(1,FALSE);            //使能link1连接流程
    } else {
        bt_top_link_enable_set(0,ENABLE);           //使能link0连接流程
        bt_top_link_enable_set(1,ENABLE);           //使能link1连接流程
   }
    short *freq_int_list = freq_int_list_get();
    int *freq_frac_list = freq_frac_list_get();
#if USER_FREQ_EN
    freq_int_list = user_freq_int_list;
    freq_frac_list = user_freq_frac_list;
#endif
    g24_rf_tbl_set(freq_int_list,freq_frac_list);   //设置使用的RF freq表
    //设置使用的信道, 从低到高(channel_mapl--channel_mapm--channel_maph),每bit一个信道，
    //min_chan_num为最小信道数，必须满足: 19 <= min_chan_num < (信道总数-3)
    bool ret = 0;
#if G24_2T4_SHORT_PACKET_3TIMES
    ret = g24_2T4_channel_setting();
#else 
    if(g24_monitor_en_get()) {
        g24_sbt_ctrl_chan_set(1, 39, 78);                                   //设置查询/连接阶段使用的信道, 2M带宽
#if G24_1TN_DONGLE_MCHIP
		logi("--------NEW: 1tn slaver chip --------");
#endif
#if 0//USER_FREQ_EN
        ret = p24g_channel_map_set(0x0003fffC, 0xf8000000, 0x1fff, 25);     //剔除0, 1, 38, 39, 40, 77, 78用于连接，外加剔除2320-2360
#else
#if G24_2T4_WITH_MONO_MONITOR
        ret = p24g_channel_map_set(0xFE000000, 0x003FFE3F, 0x0000, 20);        //25-53
#else
		ret = p24g_channel_map_set(0xfffffffC, 0xfffffe3f, 0x1fff, 30);     //剔除0, 1, 38, 39, 40, 77, 78用于连接
#endif
#endif

    } else {
#if 0//USER_FREQ_EN
        ret = p24g_channel_map_set(0x0003ffff, 0xf8000000, 0x7fff, 29);     //剔除剔除2320-2360
#else
        ret = p24g_channel_map_set(0xffffffff, 0xffffffff, 0x7fff, 30);
    }
#endif
#endif
    if(ret == 0) {
        loge("p24g_channel_map_set ERR");
        return;
    }

    uint16_t trimOriCap = nor_freq_drift_get();
#if (G24_2T4_SHORT_PACKET_3TIMES || G24_2T4_WITH_MONO_MONITOR)
    if(G24_FUNC_ROLE == G24_FUNC_ROLE_DONGLE) {
#if !G24_2T4_DONGLE_MCHIP
        trimOriCap = 0x00;              //副芯片去掉内部电容
#endif
    }
#endif
    rf_drift_set(trimOriCap);                       //在bt_frontend_init设置频偏值
    bt_frontend_init(0);
    g24_rf_power_force(BT_RFPOWER_FORCE_EN, BT_RFPOWER_FORCE_VAL);
    g24_rf_maxpower_set(BT_MAXPOWER_SET_EN, BT_MAXPOWER_SET_VAL);

#if USER_FREQ_EN
    p24g_afh_rf_reset();
#endif
    
#if G24_PA_EN
    HAL_gpio_func_set(PIN_D11, PIN_FUNC13);//RXEN
    HAL_gpio_func_set(PIN_D12, PIN_FUNC13);//TXEN
    uint32_t reg_val = readl(0x40021040);
    reg_val |= BIT(15);
    reg_val &=~(0x3F);
    reg_val |= 0x0A & 0x3F;
    writel(reg_val, 0x40021040);
    *((uint32_t*)0x40081400) = 0x80000002;
#elif 0//!MYME_M4_BOARD //debug 用于2.4G TX/RX调试，占用PD3、PD4，若引脚用于其他功能请屏蔽
     *((uint32_t*)0x400810c0) &= 0xf0f000ff;
     *((uint32_t*)0x400810c0) |= 0x010dd100; //PD3,PD4
     *((uint32_t *)(0x40021040)) |= 0xc0008006;
     *((uint32_t*)0x40081400) = 0x80000002;
#endif

    if(g24_isr_bh_handler == NULL) {
        logi("task_bt_isr create&&&&&&&&&&&&&&&&&&&&");
        g24_isr_bh_handler = OAL_thread_create(task_bt_isr, 0, "bt_isr_bh",
            BT_ISR_BH_TASK_PRIO, BT_ISR_BH_TASK_STACK_SIZE);

            logi("thread create#########################");
    }
    //g24_lmp_handler = OAL_thread_create(task_bt_lmp, 0, "bt_lmp",
    //                        G24_LMP_TASK_PRIO, BT_ISR_BH_TASK_STACK_SIZE);
    if(g24_main_task_handler == NULL) {
        g24_main_task_handler = OAL_thread_create(g24_task_main, 0, "g24 main",
            G24_MAIN_TASK_PRIO, BT_ISR_BH_TASK_STACK_SIZE);
    }
    logi("heapsize%d %d",__LINE__, OAL_heap_freespace_get());
    g24_sbt_init();
    logi("heapsize%d %d",__LINE__, OAL_heap_freespace_get());
    in_mode_g24 = 1;

}

void mode_g24_enter(void)
{
    logi("mode_g24_enter role:%d version= %d>>", G24_FUNC_ROLE, sys_ver);
	mode_g24_init();
    
    g24_cb_t g24_cb;
    memset(&g24_cb,0x00, sizeof(g24_cb));
#if G24_MONITOR_EN
    g24_cb.broadcast_code_gen           = g24_broadcast_code_gen;
    g24_cb.broadcast_code_receive       = g24_broadcast_code_receive;
#endif    
    g24_cb.receive_cmd_proc             = g24_receive_cmd_proc;
    g24_cb.lm_rx_rssi_check_log         = g24_lm_rx_rssi_check_log;
    g24_cb.err_patck_check              = g24_err_patck_check;
    g24_cb.paircode_gen                 = g24_paircode_gen;
    g24_cb.paircode_check               = g24_paircode_check;
    g24_cb.rx_power_check               = g24_rx_power_check;
    g24_cb.rx_rssi_con_en_get           = g24_rx_rssi_con_en_get;
    g24_cb.found_pair_device            = g24_found_pair_device;
    g24_cb.monito_common_cmd_receive    = g24_monito_common_cmd_receive;
    g24_cb.page_user_data_gen           = g24_page_user_data_gen;
    g24_cb.page_user_data_rec           = g24_page_user_data_rec;
#if G24_MONITOR_EN
#if (G24_FUNC_ROLE == 1) //dongle
    g24_cb.monitor_audio_receive_processed = monitor_dongle_audio_receive_processed;
#elif (G24_FUNC_ROLE == 0)//handset
    g24_cb.monitor_audio_receive_processed = monitor_handset_audio_receive_processed;
#endif
#endif

#if G24_MONITOR_EN && (G24_FUNC_ROLE == 1)  //DONGLE
    g24_cb.monitor_dongle_set_master_mic_id = monitor_dongle_set_master_mic_id;
#endif
        //打开耳放
    HP_AMP_ON_DELAY();
    HP_AMP_ON();

#if (TONE_EN && TONE_G24_MODE_EN)
    if (mode_switch_reason_get() != SW_MODE_RESET) {
        am_tone_play(SYS_TONE_WELCOME_NAME, 0, 0, TONE_G24_MODE_VOL, 0, 0);
    }
#endif

    g24_cb_register(&g24_cb);
    g24_rf_init();

#if (DISP_EN && STATE_UNCHARGEING_EN)
    disp_state(STATE_UNCHARGEING);
#endif

#if (DISP_EN && STATE_BT_DIS_EN)
    disp_state(STATE_BT_DIS);
#endif

#if RECORD_FUNC_EN
    record_enter();
#endif
#if POWER_LPC_MODE_EN & (G24_FUNC_ROLE == 0)
    if (wireless_scan_timer_handler == 0) {
        OAL_timer_create(&wireless_scan_timer_handler, "wirescan", OAL_ms_to_ticks(WIRELESS_PEER_TIME),
            wireless_scan_en_timer_cb, (void *)1, 1, OAL_TIMER_AUTO_ACTIVATE);
    }

#endif

    if(IDUPDATA_task_handler == NULL)
    {
        IDUPDATA_task_handler = OAL_thread_create(id_updata_Task, 0, "idupdata", G24_IDUPDATA_TASK_PRIO, G24_IDUPDATA_TASK_SIZE);
    }

    g24_ui_initialize();

    logi("<<mode_g24_enter");

}

void g24_isr_close();
void sbt_enable(bool en);
void g24_bt_br_edr_core_reset();

void g24_rf_deinit()
{
    //comm_deinit();
    g24_isr_close();
    sbt_enable(false);
    g24_bt_br_edr_core_reset();

#if  0
    if (sem_g24_connect) {
        OAL_sem_destroy(&sem_g24_connect);
        sem_g24_connect = NULL;
    }
#endif

#if 1

#if 0
    if(g24_isr_bh_handler != NULL) {
        OAL_thread_delete(g24_isr_bh_handler);
        logi("thread delete****************");
        g24_isr_bh_handler = NULL;
    }
#endif
#if 0
    if(g24_lmp_handler != NULL) {
        OAL_thread_delete(g24_lmp_handler);
        g24_lmp_handler = NULL;
    }
#endif
#if 0
    if(g24_main_task_handler != NULL) {
        OAL_thread_delete(g24_main_task_handler);
        g24_main_task_handler = NULL;
    }
#endif
#endif
}

void mode_g24_exit(void)
{
    mode_g24_exit_flag = 1;

    HAL_msleep(100);            //延时让2.4G发送空数据，避免远端有杂音
    
    logi("mode_g24_exit>>");
    HP_AMP_OFF();

    mode_last_sta_set(MODE_G24);

    //关机前清掉事件，防止事件遗留到下一次开机过程中
    event_group_clear(BT_EVENT);
    event_group_clear(TWS_EVENT);
    event_group_clear(SYSTEM_EVENT);
    event_group_clear(G24_EVENT);
    
    //comm_deinit();
    g24_rf_deinit();
    
    in_mode_g24 = 0;
    
#if RECORD_FUNC_EN
    record_exit();
#endif    
    logi("mode_g24_exit<<");
}

void mode_g24(void)
{
    logi("g24 mode.");
    mode_g24_enter();
    #if !G24_FUNC_ROLE
    g24_handset_delete_link_process();
    #endif
    mode_g24_loop();
    mode_g24_exit();
}

void g24_rf_shutdown()
{
    comm_task_pause();
    //禁用连接
    bt_top_link_enable_set(0,FALSE);			//使能link0连接流程
    bt_top_link_enable_set(1,FALSE);			//使能link1连接流程
    //先断开当前连接
    g24_mode.del_linkkey_req[0] = 0;
    if(bt_top_task_status_get(0) >= BT_CONNECT_POLL) {             //dongle被请求断开
        bt_top_disconnect(0);
    }
#if G24_DUAL_CHANNEL
    g24_mode.del_linkkey_req[1] = 0;
    if(bt_top_task_status_get(1) >= BT_CONNECT_POLL) {
        bt_top_disconnect(1);
    }
#endif
    g24_rf_deinit();
    //void g24_bt_br_edr_core_reset();
    //g24_bt_br_edr_core_reset();
    void rf_close(void);
    rf_close();   //直接切断整个RF时钟

    logi("heapsize%d %d",__LINE__, OAL_heap_freespace_get());
}

void comm_task_resume();
void g24_rf_init();

void g24_rf_turnon()
{
    g24_rf_init();
    logi("heapsize%d %d",__LINE__, OAL_heap_freespace_get());
    comm_task_resume();
}

void g24_rf_power_set(int8_t power)
{
    if(power >25){
        return;
    }
    g24_rf_power_force(BT_RFPOWER_FORCE_EN, power);
    g24_rf_maxpower_set(BT_MAXPOWER_SET_EN, power);
}

#else

void g24_addr_init(void)
{
}

#endif
void g24_Channel_select_code(uint8_t channel)
{
    g24_param_t param;
    uint32_t pair_code0 = G24_PAIR_CODE0 + channel;
    uint32_t pair_code1 = G24_PAIR_CODE1 + channel;
    if(g24_monitor_en_get()) {                  //跑一拖多监听流程使用不同配对码，区别于常规形态
        pair_code0 = G24_PAIR_CODE1 + channel;//主机
        pair_code1 = G24_PAIR_CODE0 + channel;
    }
    
    param.device_role                   = G24_DEVICE_ROLE;
    param.func_role                     = G24_FUNC_ROLE;
    param.pa_en                         = G24_PA_EN;
    param.pair_code0                    = pair_code0;
    param.pair_code1                    = pair_code1;
    param.rssi_thrs                     = G24_RSSI_THRS;
    param.reconnect_rssi_thrs           = G24_RECONNECT_RSSI_THRS;
    param.master_reconnect_no_rssi      = G24_MASTER_RECONNECT_NO_RSSI;
    param.slaver_reconnect_no_rssi_time = G24_SLAVER_RECONNECT_NO_RSSI_TIME;
    param.master_reconnect_time         = G24_MASTER_RECONNECT_TIME;
    param.master_reconnect_always       = G24_MASTER_RECONNECT_ALWAYS;
    param.master_inquiry_times          = G24_MASTER_INQUIRY_TIMES;
    param.master_page_times             = G24_MASTER_PAGE_TIMES;
    param.slaver_reconnect_time         = G24_SLAVER_RECONNECT_TIME;
    param.inquiry_maxpower              = G24_INQUIRY_MAXPOWER_SET_VAL;
    param.connect_maxpower              = G24_CONNECT_MAXPOWER_SET_VAL;
    param.page_replace_inquiry          = G24_PAGE_REPLACE_INQUIRY;
    param.rf_jump_en                    = G24_RF_JUMP_EN;
    param.uplink_2ch                    = G24_UPLINK_2CH;
    param.dnlink_2ch                    = G24_DNLINK_2CH;
    param.sbc_send_4times               = G24_SBC_SEND_4TIMES;
    param.dual_channel                  = G24_DUAL_CHANNEL;
    param.dual_channel_ul_long_pkt      = G24_DUAL_CHANNEL_UL_LONG_PKT;
    param.dual_audio_long_send_4times   = G24_DUAL_AUDIO_LONG_PACKET_4TIMES;
    param.dual_short_3times             = G24_DUAL_SHORT_PACKET_3TIMES;
    param.dual_2T4_short_3times         = G24_2T4_SHORT_PACKET_3TIMES;
    param.afc_en                        = false;
    param.use_sbt_connect               = true;         //一拖二和一拖多监听方案必须使能，一拖一看需求配置

    g24_param_config(&param);

}

//handset读取广播信息
uint32_t g24_allpair_receive(void)
{
    return broadcast_code;
}
//dongle发送广播信息
void g24_allpair_send(uint32_t broadcastcode)
{
    broadcast_code = broadcastcode;
}


char buffer[10] = {0}; // 创建一个足够大的字符数组来存储格式化后的字符串

void g24_Channel_select(uint8_t channel)
{
    static uint8_t old_ID;
    bool channel_flag;
    if(old_ID != channel)
    {
        old_ID = channel;
        channel_flag = 1;
    }
    else{
        channel_flag = 0;
    }
    g24_channel_ID = channel;

    // 手动构建字符串 "CH"
    buffer[0] = 'C';
    buffer[1] = 'H';
    buffer[2] = '0' + (g24_channel_ID / 10);  // 计算十位
    buffer[3] = '0' + (g24_channel_ID % 10);  // 计算个位
    buffer[4] = '\0';  // 字符串结束符

#if (G24_FUNC_ROLE) //dongle
    if(channel_flag)
    {
        g24_Channel_select_code(g24_channel_ID);//通道选择
        g24_dongle_delete_link_process();
    }
#if LCDC_EN
    lcd_show_string(60, 90, 130, 60, 60, (uint8_t *)buffer, 0);
    // logi("buffer = %s\r\n",buffer);
#endif
    // disp_lcd_channel_update(g24_channel_ID);//频道显示

#elif (!G24_FUNC_ROLE)//handset
    if(channel_flag)
    {
        g24_Channel_select_code(g24_channel_ID);//通道选择
        g24_handset_delete_link_process();
    }
#if LCDC_EN
    lcd_show_string(60, 90, 130, 60, 60, (uint8_t *)buffer, 0);
    // logi("buffer = %s\r\n",buffer);
#endif
    // disp_lcd_channel_update(g24_channel_ID);//频道显示
#endif
}

uint32_t g24_get_channel_ID(void)
{
    return g24_channel_ID;
}


