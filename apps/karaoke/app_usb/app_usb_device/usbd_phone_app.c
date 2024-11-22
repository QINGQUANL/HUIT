#define LOG_LEV 4
#include "sys_inc.h"
#include "hal.h"
#include "user_config.h"
#include "usbd_phone_app.h"

extern USBD_SetupReqTypedef setupReq;

AT_SRAM(.usbd_phone_app)
void smartlink_set_sound_mode(uint8_t* indata)
{
    logi("%s", __func__);
    printf_array(indata, setupReq.wLength);
}

AT_SRAM(.usbd_phone_app)
void smartlink_set_low_switch(uint8_t* indata)
{
    logi("%s", __func__);
    printf_array(indata, setupReq.wLength);
}

AT_SRAM(.usbd_phone_app)
void smartlink_set_aenc_level(uint8_t* indata)
{
    logi("%s", __func__);
    printf_array(indata, setupReq.wLength);
}

AT_SRAM(.usbd_phone_app)
void smartlink_set_gain(uint8_t* indata)
{
    logi("%s", __func__);
    printf_array(indata, setupReq.wLength);
}

AT_SRAM(.usbd_phone_app)
void smartlink_set_channel_track_mode(uint8_t* indata)
{
    logi("%s", __func__);
    printf_array(indata, setupReq.wLength);
}

AT_SRAM(.usbd_phone_app)
void smartlink_set_listener_vol(uint8_t* indata)
{
    logi("%s", __func__);
}

AT_SRAM(.usbd_phone_app)
void smartlink_set_sd_control(uint8_t* indata)
{
    logi("%s", __func__);
    printf_array(indata, setupReq.wLength);
}

AT_SRAM(.usbd_phone_app)
void smartlink_set_mic_vol(uint8_t* indata)
{
    logi("%s", __func__);
    printf_array(indata, setupReq.wLength);
}

AT_SRAM(.usbd_phone_app)
void smartlink_set_rx_bright(uint8_t* indata)
{
    logi("%s", __func__);
    printf_array(indata, setupReq.wLength);
}

AT_SRAM(.usbd_phone_app)
void smartlink_reset_connect(uint8_t* indata)
{
    logi("%s", __func__);
}

AT_SRAM(.usbd_phone_app)
void smartlink_set_date(uint8_t* indata)
{
    logi("%s", __func__);
}

AT_SRAM(.usbd_phone_app)
void smartlink_restore_factory(uint8_t* indata)
{
    logi("%s", __func__);
}


AT_SRAM(.usbd_phone_app)
void smartlink_get_bat(float* ret)
{
    logi("%s", __func__);
    ret[0] = 99.58;
    ret[1] = 50.46;
    ret[2] = 30.00;
    ret[3] = 38.00;
}

AT_SRAM(.usbd_phone_app)
void smartlink_get_signal_power(uint8_t* ret)
{
    logi("%s", __func__);
    ret[1] = 1;
    ret[2] = 2;
    ret[3] = 3;
    ret[4] = 1;
}

AT_SRAM(.usbd_phone_app)
void smartlink_get_vol(uint8_t* ret)
{
    logi("smartlink_get_vol");
    ret[1] = 10;
    ret[2] = 20;
    ret[3] = 30;
    ret[4] = 99;
}

AT_SRAM(.usbd_phone_app)
void smartlink_get_vol_mode(uint8_t* ret)
{
    logi("%s", __func__);
    ret[1] = 1;
    ret[2] = 0;
    ret[3] = 0;
    ret[4] = 1;
}

AT_SRAM(.usbd_phone_app)
void smartlink_get_aenc_level(uint8_t* ret)
{
    logi("%s", __func__);
    ret[1] = 3;
    ret[2] = 2;
    ret[3] = 1;
    ret[4] = 5;
}

AT_SRAM(.usbd_phone_app)
void smartlink_get_gain(uint8_t* ret)
{
    logi("%s", __func__);
    ret[1] = 3;
    ret[2] = 2;
    ret[3] = 1;
    ret[4] = 5;
}

AT_SRAM(.usbd_phone_app)
void smartlink_get_device_status(uint8_t* ret)
{
    logi("%s", __func__);
    ret[1] = 0;
    ret[2] = 1;
    ret[3] = 0;
    ret[4] = 1;
}

AT_SRAM(.usbd_phone_app)
void smartlink_get_listener_vol(uint8_t* ret)
{
    logi("%s", __func__);
    ret[1] = 99;
    ret[2] = 50;
    ret[3] = 60;
    ret[4] = 70;
}

AT_SRAM(.usbd_phone_app)
void smartlink_get_sd_volume(uint32_t* ret)
{
    logi("%s", __func__);
    uint8_t type = setupReq.wValue >> 8;
    logi("type %d", type);
    if(type == 0) { //录音时长
        ret[0] = 60*30; //30分钟
        ret[1] = 60*30; //30分钟
        ret[2] = 60*30;
        ret[3] = 60*30;
    } else if(type == 1) {
        ret[0] = 0x1F00000; //31G
        ret[1] = 0x1F00000; //31G
        ret[2] = 0x1F00000; //31G
        ret[3] = 0x1F00000; //31G
    } else if(type == 2) {
        ret[0] = 0x20000; //2G
        ret[1] = 0x20000; //2G
        ret[2] = 0x20000; //2G
        ret[3] = 0x20000; //2G
    }
}

AT_SRAM(.usbd_phone_app)
void smartlink_get_version(uint8_t* ret)
{
    logi("%s", __func__);
    memcpy(ret, "app demo V1.0.0",  15);  //最大32BYTE
}

AT_SRAM(.usbd_phone_app)
void onep0_preprocess_set_data(uint8_t* indata)
{
    uint8_t subCmd = setupReq.wValue & 0xff;
    switch(subCmd) {
    case SMARTLINK_SET_SOUND_MODE:
 		smartlink_set_sound_mode(indata);
		break;
        
	case SMARTLINK_SET_LOW_SWITCH:
		smartlink_set_low_switch(indata);
		break;

	case SMARTLINK_SET_AENC_LEVEL:
		smartlink_set_aenc_level(indata);
		break;
	case SMARTLINK_SET_GAIN:
		smartlink_set_gain(indata);
		break;

	case SMARTLINK_SET_CHANNEL_TRACK_MODE:
		smartlink_set_channel_track_mode(indata);
		break;

	case SMARTLINK_SET_LISTENER_VOL:
		smartlink_set_listener_vol(indata);
		break;

	case SMARTLINK_SET_SD_CONTROL:
		smartlink_set_sd_control(indata);
		break;

	case SMARTLINK_SET_MIC_VOLUME:
		smartlink_set_mic_vol(indata);
		break;
		
	case SMARTLINK_SET_LCD_BRIGHT:
		smartlink_set_rx_bright(indata);
		break;	

	case SMARTLINK_SET_RESET_PAIR:
		smartlink_reset_connect(indata);
		break;
		
	case SMARTLINK_SET_DATE:
		smartlink_set_date(indata);
		break;	

	case SMARTLINK_SET_RESET_FACTORY:
		smartlink_restore_factory(indata);
		break;
    }
}

AT_SRAM(.usbd_phone_app)
void onep0_preprocess_get_data(uint8_t* ret)
{
    logi("ret %x, %x", ret, &ret[0]);
    uint8_t subCmd = setupReq.wValue & 0xff;
	switch(subCmd){
		case SMARTLINK_GET_BAT_VOL:
			smartlink_get_bat((float*)ret);
			break;
		case SMARTLINK_GET_SIGNAL_POWER:
			smartlink_get_signal_power(ret);
			break;
		case SMARTLINK_GET_VOL:
			smartlink_get_vol(ret);
			break;
        case SMARTLINK_GET_VOL_MODE:
			smartlink_get_vol_mode(ret);
			break;
        case SMARTLINK_GET_AENC_LEVEL:
			smartlink_get_aenc_level(ret);
			
			break;
        case SMARTLINK_GET_GAIN:
			smartlink_get_gain(ret);
			break;
        case SMARTLINK_GET_DEVICE_STATUS:
			smartlink_get_device_status(ret);
			break;
        case SMARTLINK_GET_LISTENER_VOL:
			smartlink_get_listener_vol(ret);
			break;
        case SMARTLINK_GET_SD_VOLUME:
            smartlink_get_sd_volume((uint32_t*)ret);
            break;
        case SMARTLINK_GET_VERSION:
			smartlink_get_version(ret);
			break;
	}
}


