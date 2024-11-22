#ifndef _USBD_PHONE_APP_H_
#define _USBD_PHONE_APP_H_

#include "setup.h"
#include "hal_ll_usb.h"

#define SL_PHONE_CMD    0xEE    //自定义的头

enum {
    SMARTLINK_GET_BAT_VOL = 0xE0,
    SMARTLINK_GET_SIGNAL_POWER = 0xE1,
    SMARTLINK_GET_VOL = 0xE2,
    SMARTLINK_GET_VOL_MODE = 0xE3,
    SMARTLINK_GET_AENC_LEVEL = 0xE4,
    SMARTLINK_GET_GAIN = 0xE5,
    SMARTLINK_GET_DEVICE_STATUS = 0xE6,
    SMARTLINK_GET_LISTENER_VOL = 0xE7,
    SMARTLINK_GET_SD_VOLUME = 0xE8,
    SMARTLINK_GET_VERSION = 0xE9,

    SMARTLINK_SET_SOUND_MODE = 0xA0,
    SMARTLINK_SET_LOW_SWITCH = 0xA1,
    SMARTLINK_SET_AENC_LEVEL = 0xA2,
    SMARTLINK_SET_GAIN = 0xA3,
    SMARTLINK_SET_CHANNEL_TRACK_MODE = 0xA4,
    SMARTLINK_SET_LISTENER_VOL = 0xA5,
    SMARTLINK_SET_SD_CONTROL = 0xA6,
    SMARTLINK_SET_MIC_VOLUME = 0xA7,
    SMARTLINK_SET_LCD_BRIGHT = 0xA8,
    SMARTLINK_SET_RESET_PAIR = 0xA9,
    SMARTLINK_SET_DATE      = 0xAA,
    SMARTLINK_SET_RESET_FACTORY = 0xAB,
};

void onep0_preprocess_set_data(uint8_t* indata);
void onep0_preprocess_get_data(uint8_t* ret);

#endif
