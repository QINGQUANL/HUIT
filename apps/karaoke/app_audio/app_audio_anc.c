#define LOG_LEV     4

#include "sys_inc.h"
#include "hal.h"
#include "user_config.h"
#include "sltk_audio.h"
#include "bt_audio_service.h"
#include "bt_host_api.h"
#include "tws_api.h"
#include "bt_api.h"
#include "bt_audio_manager.h"
#include "anc_service.h"
#include "event.h"
#include "ui.h"
#include "oal.h"
#include "app_audio_anc.h"
#include "psm_manager.h"
#include "info_mem.h"
#include "tws_profile.h"
#include "sensor.h"
#include "sensor_proximity.h"
#include "bt.h"
#include "tone_play_manager.h"
#include "air_api.h"
#include "sleep.h"


#if AUDIO_ANC_EN
uint8_t anc_mode_cur;           /* 当前模式 */
uint8_t anc_mode_1in;           /* 一只入耳时模式 */
uint8_t anc_mode_2in;           /* 两只入耳时模式 */
uint8_t anc_mode_sel;           /* 选择降噪、通透、关闭的组合 */
uint8_t phone_anc_mode;         /* 手机请求设置的模式 */


/*------------------------------------------------------------------------------
*函数功能: ANC mode初始化 设置总共支持哪些模式
*-------------------------------------------------------------------------------*/
void anc_mode_init(void)
{
    anc_mode_sel = (1<<ANC_WORKING_MODE_ANC) | (1<<ANC_WORKING_MODE_HEARING);   /* 降噪+通透 */
}

#if POWER_OFF_AUTO_WHEN_ANC_DIS
OAL_TIMER AncTimerHandle;

/*------------------------------------------------------------------------------
*函数功能: ANC timer超时关机
*-------------------------------------------------------------------------------*/
void anc_mode_timer_cb(void* xTimer)
{
    if (tws_role_get() != TWS_SLAVE) {
        logi("anc dis timeout");
        event_put(SYSTEM_EVENT_POWEROFF, NULL, 0);

        if (tws_role_get() == TWS_MASTER) {
            uint32_t event = SYSTEM_EVENT_POWEROFF;
            tws_manager_app_cmd_send(TWS_APP_CMD_EVENT, (uint8_t*)&event, 4);
        }
    }
}

void anc_mode_timer_start(void)
{
    logi("anc_mode_timer_start");
    if (AncTimerHandle == NULL) {
        OAL_timer_create(&AncTimerHandle, "AncTimer", OAL_ms_to_ticks(POWER_OFF_AUTO_PERIOD*1000),
            anc_mode_timer_cb, (void *)1, 0, OAL_TIMER_AUTO_ACTIVATE);
    } else {
       OAL_timer_activate(&AncTimerHandle, OAL_TIMER_AUTO_ACTIVATE);
    }
}

void anc_mode_timer_stop(void)
{
    logi("anc_mode_timer_stop");
    if (AncTimerHandle != NULL) {
       OAL_timer_activate(&AncTimerHandle, OAL_TIMER_NO_ACTIVATE);
    }
}
#endif

/*------------------------------------------------------------------------------
*函数功能: 设置anc总共支持模式，副耳同步主耳设置使用
*参数1：工作模式
*-------------------------------------------------------------------------------*/
void anc_mode_sel_set(uint8_t sel)
{
    /* 噪声控制选择，左、右保持相同 */
    anc_mode_sel = sel;
    anc_param_sync();
}

/*------------------------------------------------------------------------------
*函数功能: 获取anc总共支持模式
*返回值：工作模式
*-------------------------------------------------------------------------------*/
uint8_t anc_mode_sel_get(void)
{
    return anc_mode_sel;
}

/*------------------------------------------------------------------------------
*函数功能: 关机时anc当前工作模式存入flash
*参数：
*-------------------------------------------------------------------------------*/
void poweroff_anc_mode_param_set(void)
{

    logi("anc mode flash save");
    uint8_t mode[3];
    mode[0] = anc_mode_cur;

#if SENSOR_PROXIMITY_EN
    mode[1] = anc_mode_1in;
    mode[2] = anc_mode_2in;
#endif
    sys_info_write(ANC_MODE_SEL, mode, 3);

}


/*------------------------------------------------------------------------------
*函数功能: 开机时从flash读取anc模式
*参数：
*-------------------------------------------------------------------------------*/
void poweron_anc_mode_param_get(void)
{

    uint8_t anc_mode[3];
    sys_info_read(ANC_MODE_SEL, &anc_mode, 3);
    if (anc_mode[0] > ANC_WORKING_MODE_HEARING) {
        anc_mode[0] = ANC_WORKING_MODE_ANC;
    }
    anc_mode_param_set(anc_mode[0]);

#if SENSOR_PROXIMITY_EN
    if (anc_mode[1] > ANC_WORKING_MODE_HEARING) {
        anc_mode[1] = ANC_WORKING_MODE_DIS;
    }
    anc_mode_1in_param_set(anc_mode[1]);

    if (anc_mode[2] > ANC_WORKING_MODE_HEARING) {
        anc_mode[2] = ANC_WORKING_MODE_ANC;
    }
    anc_mode_2in_param_set(anc_mode[2]);
 #endif
}

/*------------------------------------------------------------------------------
*函数功能: 设置anc当前工作模式
*参数：工作模式
*-------------------------------------------------------------------------------*/
void anc_mode_param_set(uint8_t mode)
{
    anc_mode_cur = mode;
    anc_param_sync();

#if POWER_OFF_AUTO_WHEN_ANC_DIS
    bool start = false;

    if (mode == ANC_WORKING_MODE_DIS) {
        if (tws_role_get() != TWS_SLAVE) {
            if (!tws_phone_con_status_get()) {
                start = true;
            }
        }  else {
            if (!tws_slave_phone_con_status_get()) {
                start = true;
            }
        }
    } else {
        start = false;
    }

    if (start) {
        anc_mode_timer_start();
    } else {
        anc_mode_timer_stop();
    }
#endif
}

/*------------------------------------------------------------------------------
*函数功能: 获取anc当前工作模式
*返回值：工作模式
*-------------------------------------------------------------------------------*/
uint8_t anc_mode_param_get(void)
{
    return anc_mode_cur;
}

/*------------------------------------------------------------------------------
*函数功能: anc工作模式写入flash，主耳使用
*返回值：工作模式
*-------------------------------------------------------------------------------*/
void anc_mode_param_store(uint8_t mode)
{
    if (tws_role_get() != TWS_SLAVE) {
        anc_mode_param_set(mode);

#if SENSOR_PROXIMITY_EN
        uint8_t p_sta = sensor_proximity_peer_sta_get();
        if ((SENSOR_P_DOUBLE_1IN_1OUT == p_sta) || \
            (SENSOR_P_SINGLE_IN == p_sta)) {
            anc_mode_1in_param_set(mode);
        } else if (SENSOR_P_DOUBLE_2IN == p_sta) {
            anc_mode_2in_param_set(mode);
        }
#endif
    }
}

#if SENSOR_PROXIMITY_EN
/*------------------------------------------------------------------------------
*函数功能: 单边入耳时，anc工作模式设置
*参数：工作模式
*-------------------------------------------------------------------------------*/
void anc_mode_1in_param_set(uint8_t mode)
{
    anc_mode_1in = mode;
    anc_param_sync();
    //info_set(INFO_ANC_SEL_1IN, (void *)&anc_mode_1in, 1);
   // sys_info_write(ANC_MODE_SEL_1IN, &anc_mode_1in, 1);
}

uint8_t anc_mode_1in_param_get(void)
{
    return anc_mode_1in;
}

/*------------------------------------------------------------------------------
*函数功能: 双边入耳时，anc工作模式设置
*参数：工作模式
*-------------------------------------------------------------------------------*/
void anc_mode_2in_param_set(uint8_t mode)
{
    anc_mode_2in = mode;
    anc_param_sync();
    //info_set(INFO_ANC_SEL_2IN, (void *)&anc_mode_2in, 1);fix me lcj
    //sys_info_write(ANC_MODE_SEL_2IN, &anc_mode_2in, 1);
}

uint8_t anc_mode_2in_param_get(void)
{
    return anc_mode_2in;
}
#endif


/*------------------------------------------------------------------------------
*函数功能: 双耳参数同步，主耳->副耳
*
*-------------------------------------------------------------------------------*/
void anc_param_sync(void)
{
    uint8_t param[4];

    /*
    * 0 - anc_mode_sel
    * 1 - anc_mode_cur
    * 2 - anc_mode_1in
    * 3 - anc_mode_2in
    */
    if (tws_role_get() == TWS_MASTER) {
        libc_memset(param, 0x00, sizeof(param));
        param[0] = anc_mode_sel_get();
        param[1] = anc_mode_param_get();
#if SENSOR_PROXIMITY_EN
        param[2] = anc_mode_1in_param_get();
        param[3] = anc_mode_2in_param_get();
#endif
        tws_manager_app_cmd_send(TWS_APP_CMD_ANC_SYNC, param, sizeof(param));
    }
}

/*------------------------------------------------------------------------------
*函数功能: 设置anc参数，副耳收到主耳消息后使用
*参数：主耳发过来的消息
*-------------------------------------------------------------------------------*/
void anc_param_set(uint8_t *param)
{
    /* 只在副耳上处理 */
    anc_mode_sel_set(param[0]);
    anc_mode_param_set(param[1]);
#if SENSOR_PROXIMITY_EN
    anc_mode_1in_param_set(param[2]);
    anc_mode_2in_param_set(param[3]);
#endif
}

/*------------------------------------------------------------------------------
*函数功能: 设置anc工作模式，推送BT_EVENT_ANC_SET 消息
*参数：工作模式
*-------------------------------------------------------------------------------*/
void anc_mode_set(uint8_t mode)
{
    /* 手机指定噪声控制模式
     * mode的值是0x01/0x02/0x03 */
    mode -= 1;
    logi("anc working mode:%d", mode);

    phone_anc_mode = mode;

    event_unique_put(BT_EVENT_ANC_SET, NULL, 0);
}


/*------------------------------------------------------------------------------
*函数功能: 设置anc工作模式实际操作，接收到BT_EVENT_ANC_SET 消息
*
*-------------------------------------------------------------------------------*/
void anc_mode_set_process(void)
{
    uint8_t mode;

#if BT_SNIFF_EN
    bt_sniff_suspend(true);
#endif

    mode = phone_anc_mode;
    tws_master_send_anc_working_mode(mode);
    as_anc_working_mode_set(mode);
    anc_mode_param_store(mode);
    anc_play_tone(mode);
}

/*------------------------------------------------------------------------------
*函数功能: anc 模式依次切换
*
*-------------------------------------------------------------------------------*/
void anc_mode_switch(void)
{
    sys_event_info_t info;
    proxy_func_t func;

    info.event = AUDIO_ANC_EVENT_PROXY;
    func = anc_mode_switch_do;
    memcpy(info.params, &func, sizeof(func));
    info.params_len = sizeof(func);
    audio_proxy_process(&info);
}

void anc_mode_switch_do(void)
{
    /* 循环切换噪声控制模式 */
    uint8_t config;
    uint8_t cur_mode;
    uint8_t next_working_mode;

    set_cpu_low_freq_delay_time(3000);

    config = ui_anc_mode_config_get();

    if ((1<<ANC_WORKING_MODE_DIS) == config) {
        logi("%s,config=%d\n",__func__,config);
        return;
    }

#if BT_SNIFF_EN
    bt_sniff_suspend(true);
#endif

    cur_mode = as_anc_working_mode_get();
    next_working_mode = ANC_WORKING_MODE_DIS;

    logi("cur_mode:%x,config:%x", cur_mode,config);
    switch (cur_mode) {
#if (AUDIO_ANC_LOOP == 1)
    case ANC_WORKING_MODE_DIS:
        if (config & (1<<ANC_WORKING_MODE_HEARING)) {
            next_working_mode = ANC_WORKING_MODE_HEARING;
        } else {
            next_working_mode = ANC_WORKING_MODE_ANC;
        }
        break;

    case ANC_WORKING_MODE_ANC:
        if (config & (1<<ANC_WORKING_MODE_DIS)) {
            next_working_mode = ANC_WORKING_MODE_DIS;
        } else {
            next_working_mode = ANC_WORKING_MODE_HEARING;
        }
        break;

    case ANC_WORKING_MODE_HEARING:
        if (config & (1<<ANC_WORKING_MODE_ANC)) {
            next_working_mode = ANC_WORKING_MODE_ANC ;
        } else {
            next_working_mode =  ANC_WORKING_MODE_DIS;
        }
        break;

#else
    case ANC_WORKING_MODE_DIS:
        if (config & (1<<ANC_WORKING_MODE_ANC)) {
            next_working_mode = ANC_WORKING_MODE_ANC;
        } else {
            next_working_mode = ANC_WORKING_MODE_HEARING;
        }
        break;

    case ANC_WORKING_MODE_ANC:
        if (config & (1<<ANC_WORKING_MODE_HEARING)) {
            next_working_mode = ANC_WORKING_MODE_HEARING;
        } else {
            next_working_mode = ANC_WORKING_MODE_DIS;
        }
        break;

    case ANC_WORKING_MODE_HEARING:
        if (config & (1<<ANC_WORKING_MODE_DIS)) {
            next_working_mode = ANC_WORKING_MODE_DIS;
        } else {
            next_working_mode = ANC_WORKING_MODE_ANC;
        }
        break;
#endif

    default:
        break;
    }

    logi("anc working mode:%d", next_working_mode);

#if BT_AIR_SEL
    if (tws_role_get() != TWS_SLAVE) {
        air_anc_send_data(next_working_mode+1);
    }
#endif
    tws_master_send_anc_working_mode(next_working_mode);
    as_anc_working_mode_set(next_working_mode);
    anc_play_tone(next_working_mode);
    anc_mode_param_store(next_working_mode);
}

void anc_mode_fix_do(uint8_t mode)
{
    /* 耳机指定噪声控制模式 */
    logi("++%s,mode=0x%x\n",__func__,mode);
    if (tws_role_get() != TWS_SLAVE) {
        if (mode == as_anc_working_mode_get()) {
            return;
        }

#if BT_SNIFF_EN
        bt_sniff_suspend(true);
#endif

#if BT_AIR_SEL
        air_anc_send_data(mode+1);
#endif
        tws_master_send_anc_working_mode(mode);
        as_anc_working_mode_set(mode);
        anc_mode_param_store(mode);
    }
}

/*------------------------------------------------------------------------------
*函数功能: anc 模式调整，指定工作模式，主耳使用
*参数：工作模式
*-------------------------------------------------------------------------------*/
void anc_mode_fix(uint8_t mode)
{
    sys_event_info_t info;
    proxy_func2_t func;

    info.event = AUDIO_ANC_EVENT_PROXY;
    func = anc_mode_fix_do;
    memcpy(info.params, &func, sizeof(func));
    info.params[sizeof(func)] = mode;
    info.params_len = sizeof(func) + sizeof(mode);
    audio_proxy_process(&info);
}


void anc_mode_suspend_do(void)
{
#if BT_SNIFF_EN
    bt_sniff_suspend(true);
#endif

    tws_master_send_anc_working_mode_suspend(1);
    as_anc_working_mode_suspend();
}

void anc_mode_suspend(void)
{
    sys_event_info_t info;
    proxy_func_t func;

    info.event = AUDIO_ANC_EVENT_PROXY;
    func = anc_mode_suspend_do;
    memcpy(info.params, &func, sizeof(func));
    info.params_len = sizeof(func);
    audio_proxy_process(&info);
}

void anc_mode_resume_do(void)
{
#if BT_SNIFF_EN
    bt_sniff_suspend(true);
#endif

    tws_master_send_anc_working_mode_suspend(0);
    as_anc_working_mode_resume();
}

void anc_mode_resume(void)
{
    sys_event_info_t info;
    proxy_func_t func;

    info.event = AUDIO_ANC_EVENT_PROXY;
    func = anc_mode_resume_do;
    memcpy(info.params, &func, sizeof(func));
    info.params_len = sizeof(func);
    audio_proxy_process(&info);
}

/*------------------------------------------------------------------------------
*函数功能: anc 模式切换播放提示音
*参数：工作模式
*-------------------------------------------------------------------------------*/

void anc_play_tone(uint8_t mode)
{
    switch (mode) {
#if TONE_CLOSE_EN
    case ANC_WORKING_MODE_DIS:
        am_tws_tone_play(SYS_TONE_CLOSE_NAME, TONE_CLOSE_VOL, (TWS_INIT == tws_role_get()) ? 0 : 1);
        break;
#endif

#if TONE_NOISE_EN
    case ANC_WORKING_MODE_ANC:
        am_tws_tone_play(SYS_TONE_NOISE_NAME, TONE_NOISE_VOL, (TWS_INIT == tws_role_get()) ? 0 : 1);
        break;
#endif

#if TONE_PASS_THOSE_EN
    case ANC_WORKING_MODE_HEARING:
        am_tws_tone_play(SYS_TONE_PASS_THOSE_NAME, TONE_PASS_THOSE_VOL, (TWS_INIT == tws_role_get()) ? 0 : 1);
        break;
#endif

    default:
        break;
    }
}

#endif

