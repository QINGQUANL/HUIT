#ifndef _USB_APP_DEF_H_
#define _USB_APP_DEF_H_

#define USB_MODE_SPEAKER_MIC                0     //播放+录音
#define USB_MODE_MIC                        1     //单独的USB MIC
#define USB_MODE_DATA_COLLECTOR             2     //数据收集模式，配合相关软件使用
#define USB_MODE_TWO_AUDIO_CONFIG           3     //同时支持'单独MIC'以及'播放+录音' 的配置

#define USB_DEVICE_TYPE_AUDIO               1
#define USB_DEVICE_TYPE_CARDREADER          2

#define USB_HID_AUDIO_CTRL                  0       //音频控制HID
#define USB_HID_CUSTOM                      1       //通用数据收发HID接口

//USB MIC的属性控制
#define USB_MIC_CHANNEL_MONO                1
#define USB_MIC_CHANNEL_STERO               2   //不少手机对立体声的audio模式支持不好
#define USB_MIC_CHANNEL_4CHANNEL            4   //4通路USB MIC
#define USB_MIC_CHANNEL_6CHANNEL            6   //6通路USB MIC
#define USB_MIC_CHANNEL_8CHANNEL            8   //8通路USB MIC

//采样位数，只能选择 16， 24， 32 几种，8位不支持
#define USB_MIC_SAMPLE_BITS16               2
#define USB_MIC_SAMPLE_BITS24               3
#define USB_MIC_SAMPLE_BITS32               4
// ---------------------------------------------------
#define OUTPUT_TYPE_SPEAKER                 0x0301  //喇叭
#define OUTPUT_TYPE_HEADPHONE               0x0302  //耳机
#define OUTPUT_TYPE_DISPLAY_AUDIO           0x0303  //VR设备
#define OUTPUT_TYPE_DESKTOP_SPEAKER         0x0304  //桌面喇叭
#define OUTPUT_TYPE_ROOM_SPEAKER            0x0305  //大功率喇叭
#define OUTPUT_TYPE_COM_SPEAKER             0x0306  //通信喇叭
#define OUTPUT_TYPE_LOW_FRE_SPEAKER         0x0307  //低频喇叭

// ---------------------------------------------------
#define INPUT_TYPE_MICROPHONE               0x0201  //通用麦克风
#define INPUT_TYPE_DESKTOPMICROPHONE        0x0202  //桌面麦克风
#define INPUT_TYPE_PERSONAL_MIC             0x0203  //头戴式耳机中麦克风或者领夹麦克风

#endif

