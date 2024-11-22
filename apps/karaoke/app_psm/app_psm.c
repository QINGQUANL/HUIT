#define LOG_LEV 4
#include "sys_inc.h"
#include "hal.h"
#include "sys_libc_common.h"
#include "info_mem.h"
#include "pms.h"
#include "psm_manager.h"
#include "user_config.h"
#include "g24_api.h"
#include "app_psm.h"


g24_link_info_t g24_info;
am_volume_info_t am_volume_info;
uint8_t am_default_volume;
uint8_t g24_remote_addr[6];
const uint8_t invalid_addr1[6] = {0,0,0,0,0,0};
const uint8_t invalid_addr2[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

void g24_remote_addr_set(uint8_t *addr)
{
#if G24_MONITOR_EN && !G24_DEVICE_ROLE
    memcpy(g24_remote_addr, addr, 6);
    g24_monitor_master_addr_set(g24_remote_addr);
#endif
}

AT_SRAM(.app_psm)
void g24_remote_addr_get(uint8_t *addr)
{
    memcpy(addr, g24_remote_addr, 6);
}

void g24_remote_addr_clear(void)
{
#if G24_MONITOR_EN && !G24_DEVICE_ROLE
    memset(g24_remote_addr, 0xff, 6);
    g24_monitor_master_addr_set(g24_remote_addr);
#endif
}

bool is_wireless_addr_valid(uint8_t* addr)
{
    if(memcmp(addr, invalid_addr1 , 6) == 0){
        return 0;
    }

    if(memcmp(addr, invalid_addr2 , 6) == 0){
        return 0;
    }

    return 1;
}

static int cmp_g24_link_info(void)
{
    g24_link_info_t g24_info_temp;
    sys_info_read(G24_ADDR_INFO, g24_info_temp.earphone_addr[0].addr, BT_ADDR_SIZE);
    sys_info_read(G24_LINK_INFO, g24_info_temp.g24_link[0].addr, BT_ADDR_SIZE);

    if ((memcmp(g24_info.earphone_addr[0].addr, g24_info_temp.earphone_addr[0].addr, 6) == 0)
        && (memcmp(g24_info.g24_link[0].addr, g24_info_temp.g24_link[0].addr, 6) == 0)) {
        logi("Same\r\n");
        return 1;
    }

    return 0;
}

bool g24_info_need_save(void)
{
    if (cmp_g24_link_info()) {
        logi("don't save");
        return false;
    }

    return true;
}

void g24_earphone_addr_get(uint8_t *bd_addr)
{
    sys_info_read(G24_ADDR_INFO, g24_info.earphone_addr[0].addr, BT_ADDR_SIZE);
    /*第一次启动的时候原则上2.4G和蓝牙的地址是一样的
    工具在量产时只写了蓝牙地址，G24的地址应该是为空的，当读取2.4G地址，为空的时候，
    就将蓝牙地址当做2.4G地址并保存
    */
    if(is_wireless_addr_valid(g24_info.earphone_addr[0].addr) == 0) {
        sys_info_read(BT_ADDR_INFO, g24_info.earphone_addr[0].addr, BT_ADDR_SIZE);
        if(is_wireless_addr_valid(g24_info.earphone_addr[0].addr)) {
            sys_info_write(G24_ADDR_INFO, g24_info.earphone_addr[0].addr, BT_ADDR_SIZE);
        }
    }
    
    memcpy(bd_addr, g24_info.earphone_addr[0].addr, BT_ADDR_SIZE);
}

void g24_earphone_addr_and_linkkey_save(uint8_t *addr)
{
    sys_info_read(G24_ADDR_INFO, g24_info.earphone_addr[0].addr, BT_ADDR_SIZE);
    sys_info_read(G24_LINK_INFO, g24_info.g24_link[0].addr, BT_ADDR_SIZE);
    if ((memcmp(g24_info.earphone_addr[0].addr, addr, BT_ADDR_SIZE) == 0)
        && (memcmp(g24_info.g24_link[0].addr, addr, BT_ADDR_SIZE) == 0)) {
        logi("S save already\r\n");
        return;
    }
    memcpy(g24_info.earphone_addr[0].addr, addr, BT_ADDR_SIZE);
    memcpy(g24_info.g24_link[0].addr, addr, BT_ADDR_SIZE);
    sys_info_write(G24_ADDR_INFO, g24_info.earphone_addr[0].addr, BT_ADDR_SIZE);
    sys_info_write(G24_LINK_INFO, g24_info.g24_link[0].addr, BT_ADDR_SIZE);
}

void g24_addr_save(uint8_t *addr)
{
    uint8_t addr_temp[6];
    sys_info_read(G24_ADDR_INFO, addr_temp, BT_ADDR_SIZE);
    if(memcmp(addr_temp, addr, BT_ADDR_SIZE) == 0) {
        logi("g24_addr same\r\n");
        return;
    }
    sys_info_write(G24_ADDR_INFO, addr, BT_ADDR_SIZE);
}

extern void bt_top_get_link_addr(uint8_t link, uint8_t *addr);
void g24_link_info_save(uint8_t link)
{
    uint8_t addr[6];
    bt_top_get_link_addr(link, addr);
    g24_remote_addr_set(addr);
    if(memcmp(g24_info.g24_link[link].addr, addr, BT_ADDR_SIZE) == 0){
        return;
    }
    logi("g24_link_info_save %d", link);
    logi("%02x:%02x:%02x:%02x:%02x:%02x",addr[5],addr[4],addr[3],addr[2],addr[1],addr[0]);
    //update
    memcpy(g24_info.g24_link[link].addr, addr, BT_ADDR_SIZE);
    //save
    sys_info_write(G24_LINK_INFO, g24_info.g24_link[0].addr, 2*BT_ADDR_SIZE);
}


void g24_link_info_delete(uint8_t del_link0, uint8_t del_link1)
{
    logi("delete link");
    if(del_link0) {
       memset(&g24_info.g24_link[0], 0xff, BT_ADDR_SIZE);
       g24_remote_addr_clear();
    }
    if(del_link1) {
       memset(&g24_info.g24_link[1], 0xff, BT_ADDR_SIZE);
    }
    if(del_link0 || del_link1) {
       sys_info_write(G24_LINK_INFO, g24_info.g24_link[0].addr, 2*BT_ADDR_SIZE);
    }
}

uint8_t g24_link_addr_get(uint8_t link, uint8_t *bd_addr, bool get_remote)
{
    if(g24_monitor_en_get() && !get_remote) {		//一拖多只用于获取remote addr
        return 0;
    }

    uint8_t num = 0;
    uint8_t i;

    logi("g24 link get %d:", link);
    sys_info_read(G24_LINK_INFO, g24_info.g24_link[0].addr, 2*BT_ADDR_SIZE);

    memcpy(bd_addr, g24_info.g24_link[link].addr, BT_ADDR_SIZE);
    logi("%x:%x:%x:%x:%x:%x", bd_addr[0],bd_addr[1],bd_addr[2],bd_addr[3],bd_addr[4],bd_addr[5]);

    for(i=0;i<BT_ADDR_SIZE;i++)
    {
       num += (bd_addr[i]==0xff);
    }

    if(num==6){
       return 0;
    }

    num = 0;
    for(i=0;i<BT_ADDR_SIZE;i++)
    {
       num += (bd_addr[i]==0x00);
    }

    if(num==6){
       return 0;
    }

    return 1;
}


void am_volume_all_info_default_set(uint8_t default_volume)
{
    am_default_volume = default_volume;
    //logi("%s-:%u", __func__, default_volume);
    memset((void *)&am_volume_info, 0, sizeof(am_volume_info));
    for(uint8_t i=0;i<AM_VOLUME_INFO_MAX;i++) {
        am_volume_info.volume[i] = default_volume;
    }
}


void am_volume_all_info_get(uint8_t default_volume)
{
    int ret;
    am_default_volume = default_volume;

    ret = sys_info_read(AM_VOL_INFO, &am_volume_info, sizeof(am_volume_info));
    if(ret >= 0) {
        logi("###playback_volume:%d\n",am_volume_info.volume[AM_VOLUME_INFO_PLAYBACK]);
        logi("###record_volume:%d\n",am_volume_info.volume[AM_VOLUME_INFO_RECORD]);
    } else {
        logi("&&&NO volume:%d\n",ret);
    }

    if (ret < 0) {
        am_volume_all_info_default_set(default_volume);
        ret = sys_info_write(AM_VOL_INFO, &am_volume_info, sizeof(am_volume_info));
        if (ret < 0) {
            loge("g24 vol write fail");
        } else {
            ret = sys_info_read(AM_VOL_INFO, &am_volume_info, sizeof(am_volume_info));
            if (ret < 0) {
                loge("g24 vol read fail");
            }
        }
    }
}

void am_volume_info_set(enum am_volume_info_e volume_index, uint8_t volume)
{
    am_volume_info.volume[volume_index] = volume;
}

uint8_t am_volume_info_get(enum am_volume_info_e volume_index)
{
    return am_volume_info.volume[volume_index];
}

void am_volume_info_save(void)
{
    int ret;
    am_volume_info_t am_volume_info_temp;
    sys_info_read(AM_VOL_INFO, &am_volume_info_temp, sizeof(am_volume_info_temp));

    if (memcmp(&am_volume_info_temp, &am_volume_info,sizeof(am_volume_info)) == 0) {
        logi("am_volume_info Same\r\n");
        return;
    }

    ret = sys_info_write(AM_VOL_INFO, &am_volume_info, sizeof(am_volume_info));
    if (ret < 0) {
        loge("g24 vol write fail");
    }
    logi("***playback_volume:%d\n",am_volume_info.volume[AM_VOLUME_INFO_PLAYBACK]);
    logi("***record_volume:%d\n",am_volume_info.volume[AM_VOLUME_INFO_RECORD]);
}

bool nor_mp_dut_flag_get(void)
{
#if BT_DUT_EN
    return (is_nor_mp_dut_flag_set());
#else
    return 0;
#endif
}

void g24_afc_param_get(uint16_t* afc_param)
{
    sys_info_read(G24_AFC_PARAM, (uint8_t*)afc_param, 2);    
}

void g24_afc_param_save(uint16_t afc_param)
{
    sys_info_write(G24_AFC_PARAM, (uint8_t*)&afc_param, 2);    
}

void work_mode_set(uint8_t work_mode)
{
    sys_info_write(WORK_MODE, &work_mode, 1);
}

void work_mode_get(uint8_t* work_mode)
{
    *work_mode = WORK_MODE_NONE;
    sys_info_read(WORK_MODE, work_mode, 1);
}

uint8_t psm_usb_audio_mode_get()
{
#if  USB_AUDIO_DOLBY_SUPPORT_EN
    uint8_t work_mode = USBAUDIO_MODE_STEREO;
    sys_info_read(USB_AUDIO_WORK_MODE, &work_mode, 1);
    return work_mode;
#else
    return USBAUDIO_MODE_STEREO;
#endif
}

void psm_usb_audio_mode_set(uint8_t work_mode)
{
    sys_info_write(USB_AUDIO_WORK_MODE, &work_mode, 1);
}

bool psm_is_rtc_time_has_set()
{   
    uint8_t rtc_time_flag;
    sys_info_read(RTC_TIME_FLAG, &rtc_time_flag, 1);
    return rtc_time_flag == 0x55;
}

void psm_rtc_time_flag_set()
{
    uint8_t flag = 0x55;
    sys_info_write(RTC_TIME_FLAG, &flag, 1);
}

void psm_rtc_time_flag_clear()
{
    uint8_t flag = 0;
    sys_info_write(RTC_TIME_FLAG, &flag, 1);
}

uint32_t psm_get_master_mic_id()
{
    uint32_t master_mic_id = 0;
    sys_info_read(MASTER_MIC_ID, &master_mic_id, 4);
    return master_mic_id;
}


void psm_save_master_mic_id(uint32_t mmic_id)
{
    sys_info_write(MASTER_MIC_ID, &mmic_id, 4);
}

