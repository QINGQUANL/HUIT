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
#include "app_audio_anc.h"
#include "anc_service.h"
#include "air_api.h"
#include "mode_bt.h"
#include "event.h"
#include "disp.h"
#include "hal.h"
#include "app_psm.h"
#include "app_record.h"
#include "atunning_common.h"

uint8_t ui_poweroff_flag;
uint8_t ui_sensor_play_start = 0 ; //用于判断是否有sensor play 触发
uint8_t ui_sensor_pause_start = 0 ; //用于判断是否有sensor pause 触发



void ui_init(void)
{
#if AUDIO_ANC_EN
    anc_mode_init();
#endif
    ui_poweroff_flag = 0;
}

void ui_poweroff(void)
{
    uint32_t event;

    if (tws_role_get() != TWS_SLAVE) {
#if (TONE_EN && TONE_POWER_OFF_EN)
        am_tws_tone_play(SYS_TONE_POWER_OFF_NAME, TONE_POWER_OFF_VOL, 1);
#endif


        if (tws_peer_con_status_get()) {
            event = UI_EVENT_POWEROFF;
            tws_manager_app_cmd_send(TWS_APP_CMD_EVENT, (uint8_t *)&event, sizeof(event));
        }
        HAL_msleep(200);
#if (DISP_EN && STATE_POWEROFF_EN)
        disp_state_sync(STATE_POWEROFF,200);
#endif

        event_put(PM_CODE_EVENT | PM_SHUTDOWN, NULL, 0);
        ui_poweroff_flag = 1;
    } else {
        event_put(PM_CODE_EVENT | PM_SHUTDOWN, NULL, 0);
        ui_poweroff_flag = 1;
    }
}

void ui_voice_recognition(void)
{
#if BT_SIRI_EN
    if (bt_siri_enable_get()) {
        /* 停止SIRI */
        logi("siri stop");
        bt_siri_stop();
    } else {
        /* 开始SIRI */
        logi("siri start");
        bt_siri_start();
    }
#endif
}



void usb_eq_hw_process(uint32_t params_len,uint8_t* params)
{
    eq_data* eq = (eq_data*)params;
    eq_data_type* p = (eq_data_type *)&eq->fill.ctx;
    logi("type %d, %d",eq->item_type, (int32_t)p->data);

    switch(eq->item_type) {
        case e_EQTOOL_AUDIO_DAC_GAIN:
            app_audio_set_dac_gain((int8_t)p->data);
            break;
        case e_EQTOOL_AUDIO_LPGA_GAIN:
            app_audio_set_lpga_gain((int8_t)p->data);
            break;
        case e_EQTOOL_AUDIO_RPGA_GAIN:
            app_audio_set_lpga_gain((int8_t)p->data);
            break;
        case e_EQTOOL_AUDIO_XPGA_GAIN:
            app_audio_set_xpga_gain((int8_t)p->data);
            break;
        case e_EQTOOL_AUDIO_AMIC0_DIGITAL_GAIN:
            app_audio_set_digital_mic0_gain((int8_t)p->data);
            break;
        case e_EQTOOL_AUDIO_AMIC1_DIGITAL_GAIN:
            app_audio_set_digital_mic1_gain((int8_t)p->data);
            break;
        case e_EQTOOL_AUDIO_AMIC2_DIGITAL_GAIN:
            app_audio_set_digital_mic2_gain((int8_t)p->data);
            break;
        case e_EQTOOL_AUDIO_AMIC0_ANALOG_GAIN:
            app_audio_set_analog_mic0_gain((int8_t)p->data);
            break;
        case e_EQTOOL_AUDIO_AMIC1_ANALOG_GAIN:
            app_audio_set_analog_mic1_gain((int8_t)p->data);
            break;
        case e_EQTOOL_AUDIO_AMIC2_ANALOG_GAIN:
            app_audio_set_analog_mic2_gain((int8_t)p->data);
            break;
        case e_EQTOOL_AUDIO_LINEIN0_ANALOG:
            app_audio_set_analog_linein0_gain((int8_t)p->data);
            break;
        case e_EQTOOL_AUDIO_LINEIN1_ANALOG:
            app_audio_set_analog_linein1_gain((int8_t)p->data);
            break;
        case e_EQTOOL_AUDIO_AUX0_ANALOG_GAIN:
            app_audio_set_analog_aux0_gain((int8_t)p->data);
            break;
        case e_EQTOOL_AUDIO_AUX1_ANALOG_GAIN:
            app_audio_set_analog_aux1_gain((int8_t)p->data);
            break;
        case e_EQTOOL_AUDIO_AUX2_ANALOG_GAIN:
            app_audio_set_analog_aux2_gain((int8_t)p->data);
            break;
        case e_EQTOOL_MIXVOLUME_SPDIF:
            amixer_spdif_volume_set((int8_t)p->data);
            break;
        case e_EQTOOL_MIXVOLUME_LOCAL_MIC:
            amixer_localmic_volume_set((int8_t)p->data);
            break;
        case e_EQTOOL_MIXVOLUME_WIRELESS_MIC:
            amixer_wirelessmic_volume_set((int8_t)p->data);
            break;
        case e_EQTOOL_MIXVOLUME_USB_AUDIO:
            amixer_usb_audio_volume_set((int8_t)p->data);
            break;
        case e_EQTOOL_MIXVOLUME_I2S:
            amixer_i2s_volume_set((int8_t)p->data);
            break;
        case e_EQTOOL_MIXVOLUME_TONE:
            amixer_tone_volume_set((int8_t)p->data);
            break;
        case e_EQTOOL_MIXVOLUME_AUX:
            amixer_aux_volume_set((int8_t)p->data);
            break;
        case e_EQTOOL_MODE_G24_PLAY_VOLUME:
            amixer_play_set((int8_t)p->data);
            break;
        case e_EQTOOL_RF_POWER_VAL:
            g24_rf_power_set((int8_t)p->data);
            break;
    }
    
}

void common_process(uint32_t event,uint32_t params_len,uint8_t* params)
{
    switch(event)
    {
    case KEY_EVENT_SU | KEY_REC:        //录音
        if(device_sdcard_sta_get() == DEV_OK) {
            
        }
        break;
    
    case USB_EVENT_UPDATE_COMMAND_REC:  //升级
        disable_irq();
        set_usb_update_flag();
        HAL_restart_pmu();
        break;
    case USB_EVENT_BUS_RESET:
        usb_epaudio_init_new();
        break;
#if USB_EQ_EN
    case EQ_EVENT_HW_CHANGE:
        logi("EQ_EVENT_HW_CHANGE");
        usb_eq_hw_process(params_len, params);
        break;
#endif
    default:
#if RECORD_FUNC_EN        
        record_event_process(event);
#endif
        break;
    }
}


void ui_play_pause(uint8_t play_status)
{
    logi("!!ui_play_pause:%d  1:play,2:pause",play_status);
    if (0x01 == play_status) {
        /* play */
#if SENSOR_PROXIMITY_EN
        sensor_proximity_ht_need_play_set_and_sync(false);
#if SENSOR_PROXIMITY_NO_PLAY_WHEN_LT_TO
        sensor_proximity_lt_timeout_sta_set_and_sync(false);
#endif
#endif
#if (DISP_EN && STATE_BT_PLAY_EN)
    disp_state_sync(STATE_BT_PLAY,200);
#endif
    } else if (0x02 == play_status) {
        /* pause */
#if (DISP_EN && STATE_BT_PAUSE_EN)
    disp_state_sync(STATE_BT_PAUSE,200);
#endif

#if SENSOR_PROXIMITY_EN

        if(ui_sensor_play_stat_get()) {
            ui_sensor_play_stat_set(0);//清除play辅助标记
        }
        if (ui_sensor_pause_stat_get()){
            ui_sensor_pause_stat_set(0);//清除pause辅助标记
            if (!sensor_proximity_ht_need_play_get()) {
                logi("sensor_pause_reset_ht_play_flag");
                sensor_proximity_ht_need_play_set_and_sync(true);
            }
        }
 #endif
    }
}

#if (SENSOR_PROXIMITY_EN)

void ui_play_when_sensor_p_ht(void)
{
#if SENSOR_PROXIMITY_NO_PLAY_WHEN_LT_TO
    if (!sensor_proximity_lt_timeout_sta_get())
#endif
    {
        if (sensor_proximity_ht_need_play_get()) {
            sensor_proximity_ht_need_play_set_and_sync(false);
            logi("sensor play");
            audio_play();
            ui_sensor_play_stat_set(1);
        }
    }
}


uint8_t ui_sensor_play_stat_get(void)
{
    return ui_sensor_play_start;
}

void ui_sensor_play_stat_set(uint8_t stat)
{
    logi("!ui_sensor_play_stat_set:%d",stat);
    ui_sensor_play_start = stat;
}

uint8_t ui_sensor_pause_stat_get(void)
{
    return ui_sensor_pause_start;
}

void ui_sensor_pause_stat_set(uint8_t stat)
{
    ui_sensor_pause_start = stat;
}

#endif


void ui_process(uint32_t event, uint32_t params_len, uint8_t *params)
{
    //logi("ui  event:%x", event);
    if(mode_sta_get() == MODE_G24) {
#if MODE_G24_EN
        g24_ui_process_do(event, params_len, params);
#endif
    } else if (mode_sta_get() == MODE_AFC) {
#if MODE_AFC_EN
        afc_ui_process_do(event, params_len, params);
#endif
    } else if (mode_sta_get() == MODE_BT) {
#if MODE_BT_EN    
        bt_ui_process_do(event, params_len, params);
#endif
    } else {
        ui_process_do(event);
    }
    common_process(event, params_len, params);
}


#if SENSOR_PROXIMITY_EN
static uint8_t ear_in; //bit0:local, bit1:remote
static bool last_play_sta;
static bool ignore_play_sta;

void proximity_ui_init(void)
{
    ear_in = 0;
    last_play_sta = 0;
    ignore_play_sta = 0;
}

/*------------------------------------------------------------------------------
*函数功能: 主耳主动向副耳获取sensor 入耳状态
*
*-------------------------------------------------------------------------------*/

void proximit_ui_sync_stat(void)
{
   uint8_t param;
   param = sensor_proximity_ps_sta_get();
   tws_manager_app_cmd_send(TWS_APP_CMD_SENSOR_SYNC, &param, 1);
}

#endif


#if AUDIO_ANC_EN


/*------------------------------------------------------------------------------
*函数功能: 获取anc工作模式配置
*
*-------------------------------------------------------------------------------*/
uint8_t ui_anc_mode_config_get(void)
{
    /* 获取anc_mode_sel的配置 */
    uint8_t config = 0;
    if (false == sensor_proximity_is_enable()) {
        /* 无佩戴检测 */
        config = (1<<ANC_WORKING_MODE_ANC) | (1<<ANC_WORKING_MODE_HEARING)| (1<<ANC_WORKING_MODE_DIS);
    } else {
        uint8_t sensor_p = sensor_proximity_peer_sta_get();
        logi("sensor p:%x", sensor_p);
#if BT_AIR_SEL
        if (tws_phone_con_status_get() && air_param_phone_type_get()) {
            if ((SENSOR_P_DOUBLE_2OUT == sensor_p) || (SENSOR_P_SINGLE_OUT == sensor_p)) {
                //logi("2out");
                /* 2只出耳，关闭 */
                config = (1<<ANC_WORKING_MODE_DIS);
            } else if ((SENSOR_P_DOUBLE_1IN_1OUT == sensor_p) || (SENSOR_P_SINGLE_IN == sensor_p)) {
                /* 1只入耳，通透-关闭 */
                //logi("1in");
                config = (1<<ANC_WORKING_MODE_DIS) | (1<<ANC_WORKING_MODE_HEARING);
            } else if (SENSOR_P_DOUBLE_2IN == sensor_p) {
                /* 2只入耳，根据配置切换 */
                //logi("2in");
                config = anc_mode_sel_get();
            }
        } else
#endif
        {
            if ((SENSOR_P_DOUBLE_2OUT == sensor_p) || (SENSOR_P_SINGLE_OUT == sensor_p)) {
                logi("2out");
                /* 2只出耳，关闭 */
                config = (1<<ANC_WORKING_MODE_DIS);
            } else if ((SENSOR_P_DOUBLE_1IN_1OUT == sensor_p) || (SENSOR_P_SINGLE_IN == sensor_p)) {
                /* 1只入耳，通透-关闭 */
                logi("1in");
                config = (1<<ANC_WORKING_MODE_DIS) | (1<<ANC_WORKING_MODE_HEARING);
            } else if (SENSOR_P_DOUBLE_2IN == sensor_p) {
                /* 2只入耳，通透-关闭-降噪 */
                logi("2in");
                config = (1<<ANC_WORKING_MODE_DIS) | (1<<ANC_WORKING_MODE_ANC) | (1<<ANC_WORKING_MODE_HEARING);
            }
        }
    }
    logi("ui_anc_mode_config_get :0x%x\n",config);
    return config;
}

/*------------------------------------------------------------------------------
*函数功能: 根据出入耳情况，调整anc工作模式
*参数：消息事件
*-------------------------------------------------------------------------------*/

void ui_anc_mode_switch(uint32_t event)
{
    __maybe_unused uint8_t sensor_p_sta_cur;
    logi("%s,event=0x%x",__func__,event);

    switch (event) {
    case BT_EVENT_SML_PEER_DONE:
    case BT_EVENT_TWS_PEER_DISCONNECTED:
    #if SENSOR_PROXIMITY_EN
        /* 同上次状态 */
        sensor_p_sta_cur = sensor_proximity_peer_sta_get();
        if (SENSOR_P_SINGLE_IN == sensor_p_sta_cur) {
            anc_mode_fix(anc_mode_1in_param_get());
        } else if (SENSOR_P_SINGLE_OUT == sensor_p_sta_cur) {
            anc_mode_fix(ANC_WORKING_MODE_DIS);
        }
    #endif
        break;

    case SENSOR_EVENT_P_HT:
    case SENSOR_EVENT_P_HT | SENSOR_TWS:
    case SENSOR_EVENT_P_LT:
    case SENSOR_EVENT_P_LT | SENSOR_TWS:
    case SENSOR_EVENT_P_SYNC_STATE:
        /* 获取佩戴状态，不判断手机是否连接，不判断“自动人耳检测”是否关闭 */
    #if SENSOR_PROXIMITY_EN
        sensor_p_sta_cur = sensor_proximity_peer_sta_get();
        if (SENSOR_P_DOUBLE_2IN == sensor_p_sta_cur) {
            /* 同上次状态 */
            anc_mode_fix(anc_mode_2in_param_get());
            if (sensor_proximity_ht_need_play_get() && (event == SENSOR_EVENT_P_SYNC_STATE) && bt_play_state_get() != 1) {
                logi("**sensor sync ht play");
                event_put(SENSOR_EVENT_P_HT,NULL,0);
            }
        } else if ((SENSOR_P_DOUBLE_1IN_1OUT == sensor_p_sta_cur) || \
        (SENSOR_P_SINGLE_IN == sensor_p_sta_cur)) {
             /* 同上次状态 */
            anc_mode_fix(anc_mode_1in_param_get());
        } else if ((SENSOR_P_DOUBLE_2OUT == sensor_p_sta_cur) || \
        (SENSOR_P_SINGLE_OUT == sensor_p_sta_cur)) {
            /* 降噪关闭 */
            anc_mode_fix(ANC_WORKING_MODE_DIS);
        }
    #endif
        break;
    }
}
#endif


/*------------------------------------------------------------------------------
*函数功能: space  模式切换播放提示音
*参数：工作模式
*-------------------------------------------------------------------------------*/
#if AUDIO_SPACE_MODE_EN

void space_play_tone(uint8_t enable)
{

    if (enable) {
#if TONE_SPACE_MODE_OPEN_EN
        if ((tws_role_get() == TWS_MASTER) || (tws_role_get() == TWS_SLAVE)) {
            am_tws_tone_play(SYS_TONE_SPACE_MODE_OPEN_NAME, TONE_GAME_MODE_OPEN_VOL,1);//对耳播放提示音
        } else {
            am_tone_play(SYS_TONE_SPACE_MODE_OPEN_NAME, 1, 0, TONE_GAME_MODE_OPEN_VOL, 0, 0);//单耳播放提示音
        }
#endif
    } else {
#if TONE_SPACE_MODE_CLOSE_EN
        if ((tws_role_get() == TWS_MASTER) || (tws_role_get() == TWS_SLAVE)) {
            am_tws_tone_play(SYS_TONE_SPACE_MODE_CLOSE_NAME, TONE_SPACE_MODE_CLOSE_VOL,1);//对耳播放提示音
        } else {
            am_tone_play(SYS_TONE_SPACE_MODE_CLOSE_NAME, 1, 0, TONE_SPACE_MODE_CLOSE_VOL, 0, 0);//单耳播放提示音
        }
#endif
    }
}


void ui_space_audio_switch(void)
{
    uint8_t param;
    param = am_btaudio_space_audio_status_get();
    if(param) {
        logi("space  1 -> 0");
    } else {
        logi("space  0 -> 1");
    }
    param = !param;
    space_play_tone(param);

    auto_set_cpux_freq();
    tws_manager_app_cmd_send(TWS_APP_CMD_SPACE_AUDIO_SYNC, &param, 1);
    am_btaudio_space_audio_switch(param);
}
#endif

