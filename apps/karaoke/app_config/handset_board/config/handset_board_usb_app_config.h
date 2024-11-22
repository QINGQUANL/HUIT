#ifndef _USB_APP_CONFIG_H_
#define _USB_APP_CONFIG_H_

#include "usb_app_def.h"
/*
截止2022-7-12，SPEAKER + MIC模式中，MIC如果设置成立体声，一些手机的兼容性就会变差
比如大部分的oppo手机，MIC立体声下，手机自带的录像功能无法录进去;
而只有立体声麦克风的时候反而没有这个问题
*/
#define USB_DEVICE_EN                       0

#define USB_DEVICE_TYPE   USB_DEVICE_TYPE_AUDIO

#define USB_MODULE_ALWAYS_ON                0   //没有USB检测，USB模块始终处于工作状态
#define USB_DETECT_USE_SOFWARE              0   //蓝牙模式下USB检测打开这个宏

#define USB_PHONE_APP_EN                    0   //使能手机/主机APP通信接口
#define USB_AUDIO_DOLBY_SUPPORT_EN          0   //speaker是否支持虚拟7.1
//以下功能只能选择一个
#define USB_AUDIO_MFI_EN                    0
#define USB_EQ_EN                           0   //这个选项和USB_AUDIO_MFI_EN互斥
#define USB_AENC_TOOLS_EN                   0   //
#define USB_ADDITIONAL_HID                  0   //额外的HID接口，可用于自定义数据

//以下功能只能选择一个，下面几项通过EP3实现，复用音频的HID通路
#define USB_EP3_HID_TYPE    USB_HID_AUDIO_CTRL

//选择一个USB AUDIO的模式，单MIC还是具有播放功能
#define USBD_MODE       USB_MODE_SPEAKER_MIC            //选择一个模式

#define USBD_MIC_SAMPLEBIT  USB_MIC_SAMPLE_BITS16       //选择16,24,32中的一种 
//录音选择是立体声还是单声道
#define USB_MIC_CHANNEL     USB_MIC_CHANNEL_MONO    

#define USB_SPEAKER_MUTE_EN                 1   //1:播放使能静音控制
#define USB_SPEAKER_VOLUME_EN               1   //1:播放使能音量控制
#define USB_MICROPHONE_MUTE_EN              1   //1:使能麦克风的静音控制
#define USB_MICROPHONE_VOLUME_EN            0   //1:使能麦克风的音量控制

#define USB_OUTPUT_TYPE OUTPUT_TYPE_HEADPHONE   //选择耳机类型
#define USB_INPUT_TYPE  INPUT_TYPE_MICROPHONE     //选择麦克风类型

//选择麦克风支持的采样率
#define USB_MIC_SR_44100_EN         0   //MIC支持44100
#define USB_MIC_SR_48000_EN         1   //MIC支持48000
#define USB_MIC_SR_96000_EN         0   //MIC支持96000
#define USB_MIC_SR_192000_EN        0   //MIC支持192000

#if USBD_MODE == USB_MODE_MIC
#define USB_SPECIAL_SUPPORT         0   //存在多个可切换输入源场景下的支持
#endif

/*
无需使用PD0即可进入升级模式支持，USB必须支持HID的音频控制功能
需要专门的PC软件
*/
#define USB_PC_NO_PD0_UPDATE_EN     0


//下面的选项根据上面的选择自动选入，不要随意改变
#define USB_MIC_SAMPLE_NUMBER       (USB_MIC_SR_44100_EN + USB_MIC_SR_48000_EN + USB_MIC_SR_96000_EN + USB_MIC_SR_192000_EN)

#if USB_MIC_SR_192000_EN
#define MIC_BASE_DATA_NUM 192     //基础数据量
#elif USB_MIC_SR_96000_EN
#define MIC_BASE_DATA_NUM 96
#elif USB_MIC_SR_48000_EN
#define MIC_BASE_DATA_NUM 48
#else
#define MIC_BASE_DATA_NUM 48
#endif

#if USB_SPECIAL_SUPPORT
#ifdef USB_MIC_CHANNEL
#undef USB_MIC_CHANNEL
#define USB_MIC_CHANNEL     USB_MIC_CHANNEL_STERO
#endif
#endif

#if USB_MIC_CHANNEL == USB_MIC_CHANNEL_MONO
    #define MIC_INPUT_CHANNALS      1
#else 
    #define MIC_INPUT_CHANNALS      2
#endif

#define AUDIO_MIC_MPS               (MIC_BASE_DATA_NUM * USBD_MIC_SAMPLEBIT * MIC_INPUT_CHANNALS)

#define IT_AUDIO_DEFAULT    0
#define IT_USBAUDIO_SPEAKER      1
#define IT_USBAUDIO_MICROPHONE   2
#define IT_USBAUDIO_HID          3

#define IT_USBMIC_MICROPHONE   1
#define IT_USBMIC_HID          2


#if USB_EQ_EN || USB_AUDIO_MFI_EN || USB_AENC_TOOLS_EN || USB_ADDITIONAL_HID
#define IT_MSC          4
#else
#define IT_MSC          0xFF
#endif

#if USBD_MODE == USB_MODE_DATA_COLLECTOR
#undef USB_MODULE_ALWAYS_ON
#define USB_MODULE_ALWAYS_ON  1
#endif

#endif
