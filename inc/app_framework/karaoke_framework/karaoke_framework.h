/*****************************************************************************
* Project:
* All project
* Copyright (C) 2020, SHENJU Ltd.
* All Rights Reserved.
* Description:
* Author:
* yangyang
****************************************************************************/
#ifndef _APP_TWS_FRAME_H_
#define _APP_TWS_FRAME_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mode_manager.h"
#include "charge.h"
#include "music.h"
#include "record.h"
#include "key.h"
#include "spi.h"
#include "pwm.h"
#include "rtc.h"
#include "sd.h"
#include "device.h"
#include "timer.h"
#include "sleep.h"
#include "lcdc.h"
#include "tone_play_manager.h"
#include "pmu_observer.h"
#include "psm_manager.h"

#define TWS_1LINK2_EN                       1
//#define TWO_PHONE_CAN_ENTER_SNIFF           0
//#define TWO_PHONE_SNIFF_INDEPENDENT_CTRL    0
    
//#if TWS_1LINK2_EN
//#define TWS_1LINK2_RECON_2PHONE_EN          0
//#define TWS_1LINK2_PLAYING_SCAN_EN          1
//#endif

#ifdef __cplusplus
}
#endif
#endif
