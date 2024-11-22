#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_
//N handset
#include "usb_app_config.h"
/***************************************************************************
 *               product model config
 ***************************************************************************/
#define PRODUCT_MODEL                       "handset_board"

#define SL_VER4_BOARD                       0
#define MYME_M4_BOARD                       0
#define EVB_TX_DEMO                         0


/***************************************************************************
 *               scene mode config
 ***************************************************************************/
#define MODE_BT_EN                          0       //蓝牙模式
#define MODE_G24_EN                         1       //2.4G模式
#define MODE_MUSIC_EN                       0       //本地音乐模式
#define MODE_LINEIN_EN                      0       //音频输入模式
#define MODE_USBDEV_EN                      0       //USB从机模式
#define MODE_SPDIF_EN                       0       //光纤输入模式
#define MODE_POWEROFF_EN                    1       //软关机模式
#define MODE_CHARGE_EN                      1       //充电模式
#define MODE_BTUPDATE_EN                    0       //蓝牙升级固件模式
#define MODE_AFC_EN                         0       //AFC模式

/***************************************************************************
 *               audio config
 ***************************************************************************/
/* volume config */
#define VOLUME_STEP                         5       //加减音量的步进大小
#define VOLUME_DEFAULT                      100     //默认音量

#define AUDIO_OUT_DEVICE_SEL                1       //0:i2s; 1:audiocodec
#define AUDIO_IN_DEVICE_SEL                 1       //0:i2s; 1:audiocodec
#define AUDIO_DAC_OUT_CH_MASK             0x3     //音频从哪个PIN输出; bit0:HPL; bit1:HPR; bit2:HPX;


/* audio effects for playback */
#define AUDIO_PLAYBACK_EQ                   0       //音乐（播放）通路EQ功能
#define AUDIO_PLAYBACK_DRC                  0       //音乐（播放）通路DRC功能
//#define AUDIO_PLAYBACK_TSPS                 0       //音乐（播放）通路变调功能
#define AUDIO_PLAYBACK_VSS                  0       //音乐（播放）通路3D音效功能
//#define AUDIO_PLAYBACK_SPECTRUM             0       //音乐（播放）频谱
//#define AUDIO_PLAYBACK_DR                   0       //音乐（播放）延时功能
//#define AUDIO_PLAYBACK_TBVC                 0       //音乐（播放）等响功能
#define AUDIO_PLAYBACK_VOLCUT               0       //音乐（播放）消原声功能

/* audio effects for mic */
#define AUDIO_MIC_EQ                        0       //MIC通路EQ功能
#define AUDIO_MIC_DRC                       0       //MIC通路DRC功能
#define AUDIO_MIC_ECHO                      0       //MIC通路ECHO(回声)功能
#define AUDIO_MIC_REVERB                    0      //MIC通路REVERB(混响)功能
#define AUDIO_MIC_TSPS                      0       //MIC通路变调功能
#define AUDIO_MIC_AENC                      1       //MIC AENC降噪
#define AUDIO_MIC_NSBWE_AENC                0       //MIC NSBWE降噪  
#define AUDIO_MIC_FREQ_SHIFT                0       //MIC移频
#define AUDIO_MIC_VOCALENHANCE              0       //MIC人声增强

#define AUDIO_EFFECT_FDNREV_STEREO          1           //使用立体声混响

#define AUDIO_KARAOKE_USE_DUCKER            0       //闪避功能, handset不处理
#define AUDIO_G24_TX_ENCODER_SEL            1 		//TX编码器选择：0-sbc; 1-lc3
#define AUDIO_G24_RX_DECODER_SEL            1 		//RX解码器选择：0-sbc; 1-lc3
#define LC3_ENC_CH                          1
#define LC3_DEC_CH                          2
#define AUDIO_G24_TX_ENCODER_DISABLE        1//0
#define AUDIO_G24_RX_DECODER_DISABLE        0
#define AUDIO_G24_TX_ENCODER_NUM            1

/* DAC digital gain:-80~30; 0 is 0dB, 0.75dB/step */
#define AUDIO_DAC_GAIN                      0

/* HPL模拟增益(0:0dB, 1:-0.75dB, ..., 31:-23.25dB; -0.75dB/step) */
#define AUDIO_LPGA_GAIN                     0
/* HPR模拟增益(0:0dB, 1:-0.75dB, ..., 31:-23.25dB; -0.75dB/step) */
#define AUDIO_RPGA_GAIN                     0
/* HPX模拟增益(0:0dB, 1:-0.75dB, ..., 31:-23.25dB; -0.75dB/step) */
#define AUDIO_XPGA_GAIN                     0

/* DMIC(digital microphone) config */
#define AUDIO_DMICDATA0_EN                  0
#define AUDIO_DMICDATA1_EN                  0
#define AUDIO_DMIC_CLK_PIN_SEL              0       //0:PA5; 1:PB11; 2:PB12; 3:PB15; 4:PB17; 5:PB22; 6:PB25; 7:PB26; 8:PB28; 9:PD0; 10:PD2; 11:PD3; 12:PD5; 13:PD7; 14:PD9;
#define AUDIO_DMICDATA0_PIN_SEL             0       //0:PA4; 1:PB10; 2:PB16; 3:PB21; 4:PB27; 5:PB30; 6:PD2; 7:PD6; 8:PD10;
#define AUDIO_DMICDATA1_PIN_SEL             0       //0:PB13; 1:PB14; 2:PB18; 3:PB23; 4:PB25; 5:PB29; 6:PD1; 7:PD4; 8:PD8;

/* DMIC maximum digital gain:-80 ~ 30; 0 is 0dB, 0.75dB/step */
//可由audio_service_set_input_digital_gain接口调节数字增益
#define AUDIO_DMICDATA0F_DIGITAL_GAIN       0       //DMICDATA0 falling edge
#define AUDIO_DMICDATA0R_DIGITAL_GAIN       0       //DMICDATA0 rising edge
#define AUDIO_DMICDATA1F_DIGITAL_GAIN       -10     //DMICDATA1 falling edge
#define AUDIO_DMICDATA1R_DIGITAL_GAIN       0       //DMICDATA1 rising edge

/* AMIC maximum digital gain:-80 ~ 30; 0 is 0dB, 0.75dB/step */
//#define AUDIO_AMIC0_DIGITAL_GAIN            0
//#define AUDIO_AMIC1_DIGITAL_GAIN            0
//#define AUDIO_AMIC2_DIGITAL_GAIN            0

/* LINEIN maximum digital gain:-80 ~ 30; 0 is 0dB, 0.75dB/step */
//#define AUDIO_LINEIN0_DIGITAL_GAIN          0
//#define AUDIO_LINEIN1_DIGITAL_GAIN          0

/*
 * MIC0模拟增益(配置值范围:0~28): 0: 0dB; 1: 1.5dB; 2: 3dB; ...; 28: 42dB
 * (配置值加大一档增益增加1.5dB)
 */
#define AUDIO_AMIC0_ANALOG_GAIN             4
/*
 * MIC1模拟增益(配置值范围:0~28): 0: 0dB; 1: 1.5dB; 2: 3dB; ...; 28: 42dB
 * (配置值加大一档增益增加1.5dB)
 */
#define AUDIO_AMIC1_ANALOG_GAIN             4
/*
 * MIC2模拟增益(配置值范围:0~84): 0: 0dB; 1: 0.5dB; 2: 1dB; ...; 84: 42dB
 * (配置值加大一档增益增加0.5dB)
 */
#define AUDIO_AMIC2_ANALOG_GAIN             48

/*
 * AUX0模拟增益(配置值范围:0~24): 0: -24dB; 1: -22.5dB; ...; 24: 12dB
 * (配置值加大一档增益增加1.5dB)
  * AUX0输入有6dB衰减，可以通过增加AUDIO_AMIC0_ANALOG_GAIN补偿
  * AUX0 --> AUX0_ANALOG_GAIN --> 衰减6db -->  AUDIO_AMIC0_ANALOG_GAIN -->ADC0
 */
#define AUDIO_AUX0_ANALOG_GAIN              20

/*
 * AUX1模拟增益(配置值范围:0~24): 0: -24dB; 1: -22.5dB; ...; 24: 12dB
 * (配置值加大一档增益增加1.5dB)
 * AUX1输入有6dB衰减，可以通过增加AUDIO_AMIC1_ANALOG_GAIN补偿
 * AUX1 --> AUX1_ANALOG_GAIN --> 衰减6db -->  AUDIO_AMIC1_ANALOG_GAIN -->ADC1
 */
#define AUDIO_AUX1_ANALOG_GAIN              20

/*
 * AUX2模拟增益(配置值范围:0~24): 0: -24dB; 1: -22.5dB; ...; 24: 12dB
 * (配置值加大一档增益增加1.5dB)
 * AUX2输入有6dB衰减，可以通过增加AUDIO_AMIC2_ANALOG_GAIN补偿
 * 对应MICIN2输入增益，MICIN2 --> AUX2_ANALOG_GAIN --> 衰减6db -->  AMIC2_ANALOG_GAIN -->ADC2
 */
#define AUDIO_AUX2_ANALOG_GAIN              20


/*
 * Linein模拟增益（配置值范围:0~24): 0: -24dB; 1: -22.5dB; ...; 24: 12dB
 * (配置值加大一档增益增加1.5dB)
 * LININ0/1输入有6dB衰减，可以通过增加AUDIO_AMIC0_ANALOG_GAIN/AUDIO_AMIC1_ANALOG_GAIN补偿
 * LININ0/1 --> AUDIO_LINEIN0/1_ANALOG_GAIN --> 衰减6db -->  AUDIO_AMIC0/1_ANALOG_GAIN -->ADC0/1
 */
#define AUDIO_LINEIN0_ANALOG_GAIN           20
#define AUDIO_LINEIN1_ANALOG_GAIN           20

/*
 * analog mic connection mode: 0:differential mode; 1:single-end mode(MICINxP), 2:single-end mode(MICINx),3:single-end mode(MICINxP + MICINx)
 */
#define AUDIO_AMIC0_SINGLE_END              1
#define AUDIO_AMIC1_SINGLE_END              1
#define AUDIO_AMIC2_SINGLE_END              1

/*
 * linein connection mode: 0:differential mode; 1:single-end mode
 */
#define AUDIO_LINEIN0_SINGLE_END            1
#define AUDIO_LINEIN1_SINGLE_END            1

/*
 * Default input source config
 * 0:AMIC0(analog microphone 0); 1:AMIC1; 2:AMIC2;
 * 3:DMIC0F(digital microphone 0 falling edge);ADC0
 * 4:DMIC0R(digital microphone 0 rising edge);ADC1
 * 5:AMIC0/1; 6:AMIC0/2; 7:AMIC1/2;
 * 8:DMIC0F_R(digital microphone 0 falling and rising edge)
 * 9:linein0, 10:linein1, 11:linein0/1, 12:aux0, 13:aux1, 14:aux0/1,
 */
#define AUDIO_REC_INPUT_SEL                 2       // record and call input select
#define AUDIO_LINEIN_MODE_INPUT_SEL         14      // linein mode input select
#define AUDIO_FM_MODE_INPUT_SEL             5       // fm mode input select

#define AUDIO_ANC_EN                        0
/*
 * ANC mic select:
 *  AUDIO_ANC_NONE, AUDIO_ANC_AMIC0, AUDIO_ANC_AMIC1, AUDIO_ANC_AMIC2,
 *  AUDIO_ANC_AUX0, AUDIO_ANC_AUX1, AUDIO_ANC_AUX2,
 *  AUDIO_ANC_LININ0, AUDIO_ANC_LININ1, AUDIO_ANC_LININ2,
 *  AUDIO_ANC_DMIC0F, AUDIO_ANC_DMIC0R, AUDIO_ANC_DMIC1F, AUDIO_ANC_DMIC1R,
 */
#define AUDIO_ANC_FF_MIC_SEL                AUDIO_ANC_AMIC2
#define AUDIO_ANC_FB_MIC_SEL                AUDIO_ANC_AMIC1
#define AUDIO_ANC_FF_MIC_ANALOG_GAIN        48
#define AUDIO_ANC_FB_MIC_ANALOG_GAIN        16

#define AUDIO_KARAOKE_EN                    1
#define AUDIO_KWS_EN                        0       //关键词唤醒功能

#define AUDIO_MUTE_EN                       0       //在充电仓里面mute住
#define AUDIO_PAUSE_WHEN_VBUS_IN_EN         0       //vbus in时暂停音乐
#define AUDIO_RECOVER_WHEN_RECONNECT_EN     1       //开机回连时恢复音乐播放
#define AUDIO_CALL_MAX_VOLUME_DECIBELS     -6       //通话最大音量对应衰减值（范围:-50dB ~ 0dB; 一般配置为0，如通话音量太大，则应做衰减)
#define AUDIO_MUSIC_MAX_VOLUME_DECIBELS     0       //音乐最大音量对应衰减值（范围:-50dB ~ 0dB; 一般配置为0，如音乐音量太大，则应做衰减)
#define AUDIO_INCOMING_RINGTONE_VOL_EN      1       //手机来电铃声音量独立控制： 0：来电铃声音量使用通话音量; 1：来电铃声音量由AUDIO_INCOMING_RINGTONE_VOL指定
#define AUDIO_INCOMING_RINGTONE_VOL        60       //手机来电铃声音量大小(范围:0~100)
#define AUDIO_ADMAENC_MIC_DISTANCE         14       //双MIC通话方案MIC的距离(mm);
#define AUDIO_ADMAENC_MIC_ANGLE            45       //双MIC连线和嘴的角度
#define AUDIO_ADMAENC_MIC_MODE              0       //MIC摆放方式; 0:序号小的MIC靠近嘴(如:使用MIC0，MIC2,则序号小的MIC为MIC0; 使用MIC1，MIC2,则序号小的MIC为MIC1); 1：序号大的MIC靠近嘴

#define AUDIO_KARAOKE_MIC_EN                1       //使能MIC音频通路
#define AUDIO_KARAOKE_LINEIN_EN             1       //使能LINEIN音频通路

/***************************************************************************
 *               bt config
 ***************************************************************************/
#define BT_BACKGROUND_EN                    0       //蓝牙后台（全模式支持蓝牙）功能
#define BT_PHONE_EN                         1       //蓝牙通话功能
#define BT_HID_EN                           0       //HID功能
#define BT_ADDR_USE_RANDOM                  1       //蓝牙设备地址使用随机值功能
#define BT_FCC_TEST_EN                      0       //FCC测试功能
#define BT_VOLUME_EN                        1       //0:关闭音量与手机同步功能; 1:打开音量与手机同步功能; 3:打开音量与手机同步功能并且支持耳机调节音量
#define BT_RECONNECT_SCAN_EN                0       //回连时可被搜索到功能
//#define BT_SIRI_EN                          1       //苹果设备的siri功能
#define BT_AIR_SEL                          0
#define APP_EQ_EN                           0
#define BT_DUT_EN                           0       //DUT测试,需要打开蓝牙模式

#define BT_IODM_TEST_MODE_EN                0       //通过仪器的UR发送测试命令
#define BT_DUT_UART_COM_NUM                 1       //DUT通信用的UR, 需要在UR那里选择UR的IO，波特率115200

#define BT_TWS_EN                           0       //TWS功能
#define BT_TWS_1LINK2                       0       //number of phone linked 0:1 1:2
#define BT_TWS_SCO_DATA_SEND_EN             1       //通话数据转发功能

#define BT_ADDR_DEFAULT                    {0x81,0x71,0xA2,0xA3,0xA4,0xA5} //蓝牙设备默认地址 [低地址-高地址]
#define BT_NAME_DEFAULT                     "handset"                          //蓝牙设备默认名字

//当BT_RFPOWER_FORCE_EN为1时，无论BT_MAXPOWER_SET_EN设置与否，均不起作用
#define BT_RFPOWER_FORCE_EN                 1       //RF发送功率固定,底层已经固定使用SBT方式连接，需要使用BT_RFPOWER_FORCE
#define BT_RFPOWER_FORCE_VAL                15//25      //RF发送功率固定的档位，范围：0~18，功率值：(-34+2*BT_RFPOWER_FORCE_VAL) dbm

#define BT_MAXPOWER_SET_EN                  1       //RF发送最大功率限制（在BT_RFPOWER_FORCE_EN是0时有效）
#define BT_MAXPOWER_SET_VAL                 15//25      //RF发送最大功率的档位，范围：0~18，功率值：(-34+2*BT_MAXPOWER_SET_VAL) dbm

#define G24_ADDR_DEFAULT                    {0xF1,0xF1,0xF1,0xF1,0xF1,0xF1} //2.4G设备默认地址 [低地址-高地址]
#define G24_ADDR_USE_RANDOM                 1       //2.4G设备地址使用随机值功能

#define G24_FUNC_ROLE                       0       //固定为0, 表示handset
#define G24_DEVICE_ROLE                     0//1       //0:slaver,1:master
#define G24_PAIR_CODE0                      0x12345678  //2.4G配对码0,code0+code1和检验
#define G24_PAIR_CODE1                      0x87654321  //2.4G配对码1,code0+code1和检验

#define G24_PA_EN                           1//0       //2.4G带PA
//satrt=================用于一拖一或待开发，暂不支持，请勿修改
#define G24_RSSI_THRS                       0//128     //2.4G配对距离限制，范围:0-138，值越大距离越近
#define G24_RECONNECT_RSSI_THRS             0//128     //2.4G配对距离限制，范围:0-138，值越大距离越近
#define G24_MASTER_RECONNECT_NO_RSSI        1       //master非正常断线回连距离不受限，时间与回连时间一致
#define G24_SLAVER_RECONNECT_NO_RSSI_TIME   60      //salver回连距离不受限时间，单位：秒，0表示一直受限制
#define G24_MASTER_RECONNECT_TIME           60      //回连时间，单位：秒
#define G24_MASTER_RECONNECT_ALWAYS         1       //master回连时间不受限制，用于handset做master且只连接同一设备
#define G24_MASTER_INQUIRY_TIMES            0xFF    //master断开后新设备查询次数，时间为 5*次数 秒,0xFF为一直
#define G24_MASTER_PAGE_TIMES               5       //master断开后新设备连接次数，时间为 5*次数 秒,0xFF为一直
#define G24_SLAVER_RECONNECT_TIME           0xFF    //slaver回连时间，单位：秒，slaver一直交替做inquiry scan/page scan，这里设置先page scan回连多少秒再做交替
#define G24_PAGE_REPLACE_INQUIRY            1       //查询时用page/page scan固定地址替代inquiry/inquiry scan
#define G24_RF_JUMP_EN                      0       //信号差时是否进行频段跳转，由dongle发起，handset此宏固定为0
#define G24_UPLINK_2CH                      0       //2.4G handset到dongle立体声(会增加CPU负载,上行只传输MIC建议使用单声道,该宏handset和dongle需同步设置)
#define G24_DNLINK_2CH                      1       //2.4G dongle到handset立体声
#define G24_SBC_SEND_4TIMES                 0       //2.4G上下行传输4次
//end===================


#define G24_DUAL_CHANNEL                    0//1       //2.4G 1个dongle对2个handset
#define G24_DUAL_CHANNEL_UL_LONG_PKT        0       //2.4G一拖二上行(handset到dongle)长包
#define G24_DUAL_AUDIO_LONG_PACKET_4TIMES   0       //使用音频通路实现的长包4次传输,延时增加20ms
#define G24_DUAL_SHORT_PACKET_3TIMES        1       //一拖二短包，协议层3次传输
#define G24_2T4_SHORT_PACKET_3TIMES         0//0       //一拖二短包应用于二拖四（RX 2个dongle芯片与4个handset通信）
#define G24_2T4_WITH_MONO_MONITOR			0//1

#define G24_2T4_HANDSET_SAME_FW             1       //handset使用同一个固件, 在连接阶段分配频段。只能用在主副芯片同一个RF表格，如果主芯片跑2.3G，副芯片跑2.5G则不能用
#define G24_2T4_HANDSET_MCHIP               1       //handset 二拖四主副选择，1：主，0：副，G24_2T4_HANDSET_SAME_FW为0时使用

#define G24_MONITOR_EN                      1//0       //dongle到handset单向一拖多监听功能
#define G24_MONITOR_MASTER_EN               0       //监听功能下是否是主MIC，只能有一个主MIC
#define G24_MONITOR_ONLINE_DETECT           0       //向dongle发送心跳包等,不是主MIC的时候才有用
#define G24_MONITOR_NO_HANDSHAKE            1       //监听无握手连接，master单向发送连接信息，slaver收到即可建立连接
#define G24_MONITOR_NONE_CONTINUE           0       //固定0
#define G24_MONITOR_AUDIO_CH                2//2       //一拖多传输音频声道数，单声道会增加传输次数

#if G24_2T4_SHORT_PACKET_3TIMES 
#undef	AUDIO_G24_TX_ENCODER_DISABLE
#undef	G24_DEVICE_ROLE
#undef	G24_DUAL_CHANNEL
#undef	G24_MONITOR_EN
#undef	G24_MONITOR_AUDIO_CH

#define AUDIO_G24_TX_ENCODER_DISABLE        1
#define G24_DEVICE_ROLE                     0       //0:slaver,1:master
#define G24_DUAL_CHANNEL                    0       //2.4G 1个dongle对2个handset
#define G24_MONITOR_EN                      1       //dongle到handset单向一拖多监听功能
#define G24_MONITOR_AUDIO_CH                2       //一拖多传输音频声道数，单声道会增加传输次数
#endif

#if G24_MONITOR_EN && G24_MONITOR_MASTER_EN == 0
#undef AUDIO_KARAOKE_MIC_EN
#define AUDIO_KARAOKE_MIC_EN 0
#endif

#if G24_MONITOR_EN & !G24_MONITOR_MASTER_EN
#undef AUDIO_G24_TX_ENCODER_DISABLE
#define AUDIO_G24_TX_ENCODER_DISABLE        1
#endif

#if G24_DUAL_CHANNEL
#undef G24_DEVICE_ROLE
#undef G24_PAGE_REPLACE_INQUIRY
#undef G24_MONITOR_EN
#define G24_DEVICE_ROLE                     0       //一拖二时handset固定做slaver
#define G24_PAGE_REPLACE_INQUIRY            0       //仅用在一拖一
#define G24_MONITOR_EN                      0

#if G24_DUAL_CHANNEL_UL_LONG_PKT
#undef AUDIO_G24_RX_DECODER_DISABLE
#undef G24_DUAL_SHORT_PACKET_3TIMES
#define AUDIO_G24_RX_DECODER_DISABLE        1       //长包不能接收音频数据
#define G24_DUAL_SHORT_PACKET_3TIMES        0
#endif

#if G24_DUAL_SHORT_PACKET_3TIMES
#undef G24_DUAL_CHANNEL_UL_LONG_PKT
#undef G24_DUAL_AUDIO_LONG_PACKET_4TIMES
#define G24_DUAL_CHANNEL_UL_LONG_PKT        0
#define G24_DUAL_AUDIO_LONG_PACKET_4TIMES   0
#else
#undef G24_2T4_SHORT_PACKET_3TIMES
#define G24_2T4_SHORT_PACKET_3TIMES         0
#endif


#else  //一拖一

#if G24_MONITOR_EN
#undef G24_DEVICE_ROLE
#define G24_DEVICE_ROLE                     0       //0:slaver,1:master, 监听方案固定handset做slaver，dongle做master
#endif

#undef G24_DUAL_CHANNEL_UL_LONG_PKT
#undef G24_DUAL_AUDIO_LONG_PACKET_4TIMES
#undef G24_DUAL_SHORT_PACKET_3TIMES
#undef G24_2T4_SHORT_PACKET_3TIMES
#define G24_DUAL_CHANNEL_UL_LONG_PKT        0
#define G24_DUAL_AUDIO_LONG_PACKET_4TIMES   0
#define G24_DUAL_SHORT_PACKET_3TIMES        0
#define G24_2T4_SHORT_PACKET_3TIMES         0
#endif

#if G24_MONITOR_MASTER_EN
#undef AUDIO_G24_TX_ENCODER_DISABLE
#define AUDIO_G24_TX_ENCODER_DISABLE        1
#endif


#if G24_PA_EN
#define G24_INQUIRY_MAXPOWER_SET_VAL        8
#define G24_CONNECT_MAXPOWER_SET_VAL        14
#else
#define G24_INQUIRY_MAXPOWER_SET_VAL        22//16
#define G24_CONNECT_MAXPOWER_SET_VAL        25
#endif

/***************************************************************************
 *               ble config
 ***************************************************************************/
#define BLE_OTA_EN                          0
#define BLE_AIR_SEL                         0
#define BLE_TEST_EN                         0       //ble_test
#if (BLE_OTA_EN || BLE_AIR_SEL || BLE_TEST_EN)
#define BT_BLE_EN                           1       //BLE功能
#define BLE_ADDR_USE_RANDOM                 0                                   //BLE设备地址使用随机值功能
#define BLE_ADDR_DEFAULT_L                  {0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}     //BLE左设备默认地址
#define BLE_ADDR_DEFAULT_R                  {0xEA,0xEB,0xEC,0xED,0xEE,0xEF}     //BLE右设备默认地址
#define BLE_NAME_DEFAULT_L                  "SL-BLE-L"                          //BLE左设备默认名字
#define BLE_NAME_DEFAULT_R                  "SL-BLE-R"                          //BLE右设备默认名字
#else
#define BT_BLE_EN                           0
#endif

#define BT_BLE52_EN                         0       //BLE5.2功能


/***************************************************************************
 *               spp config
 ***************************************************************************/
#define BT_SPP_EN                           0       //SPP功能
#define BT_SPP_EQ_EN                        0       //eq调试
#define BT_SPP_APP_EN                       0       //android app

#if !BT_SPP_EN
#undef BT_SPP_EQ_EN
#define BT_SPP_EQ_EN                        0
#undef BT_SPP_APP_EN
#define BT_SPP_APP_EN                       0
#endif

#define BT_DEBUG_EN                         0       //bt debug port
#define SWD_DEBUG_EN                        0       //JTAG debug enabel PB22 PB23
#define BASE_BOARD_CONFIG                   0

/***************************************************************************
 *               music config
 ***************************************************************************/
#define MUSIC_FOLDER_SELECT_EN              0       //文件夹选择功能
#define MUSIC_FAST_SKIP_EN                  1       //快进快退功能
#define MUSIC_REC_FOLDER_IGNORE             1       //忽略录音文件夹
#define MUSIC_NUM_STEP10                    0       //上下首时文件编号加减10
#define MUSIC_MUTE_FOR_FAST                 1       //快进快退时静音
#define MUSIC_DEVICE_SWITCH                 1       //0：U盘或SD卡内循环播放；1：自动切换U盘/SD卡播放
#define MUSIC_METADATA                      0       //metadata(ID3)信息获取
#define MUSIC_DEV_OFFLINE_SWITCH_MODE       0

#define MUSIC_WAV                           1
#define MUSIC_MP3                           1
#define MUSIC_WMA                           0
#define MUSIC_FLAC                          0
#define MUSIC_AAC                           1
#define MUSIC_APE                           0
#define MUSIC_OGG                           0
#define MUSIC_SBC                           0


/*
 * 0：保存本地音乐播放信息（曲目、时间）到RTC RAM，在宏PMU_VCC_RTC_ALWAYS_ON是1时有效，且在电池不断电时有效；
 * 1：保存本地音乐播放信息（曲目）到flash，在每个文件开始播放时写操作；
 * 2：保存本地音乐播放信息（曲目、时间）到flash，在软关机时写操作。
 */
#define MUSIC_INFO_SAVE_MODE                0

#define RECORD_FUNC_EN                      0       //录音模式

/***************************************************************************
 *               record config
 ***************************************************************************/
#define RECORD_LINEIN_FUNC                  0        //linein模式下录音; 0:disable; 1:录linein; 2:录mic; 3:录mic和linein混音
#define RECORD_FM_FUNC                      0        //FM模式下录音; 0:disable; 1:录fm; 2:录mic; 3:录mic和fm混音
#define RECORD_STREAM_TYPE                  0        //录音数据流保存类型： 0:文件系统（U盘/SD卡）保存; 1: spi flash保存
#define RECORD_NEW_DEVICE_FIRST             1        //0:不自动切换最新插入的设备； 1:自动切换最新插入的设备
#define RECORD_AUTO_PLAY                    1        //0:不自动播放录音文件； 1:自动播放录音文件
#define RECORD_PLAY_LOOP                    1        //0:全部循环播放; 1:单曲循环播放
#define RECORD_FORMAT_SEL                   0        //录音编码格式： 0:adpcm; 1:mp2; 2:sbc
#define RECORD_FILE_DIR                     "record" //录音文件夹名
#define RECORD_FILE_PREFIX                  "rec"    //录音文件名前缀
#define RECORD_FILE_COUNT_BITS              5        //录音文件序号支持多少位（e.g 支持到万位则配5)
#define RECORD_DEV_OFFLINE_SWITCH_MODE      0


/***************************************************************************
 *               sd config
 ***************************************************************************/
#define SD_EN                               0       //SD卡
#define SD_DETECT_EN                        1       //SD检测
#define SD_4WIRE_EN                         0       //是否支持四线
#define SD_D0_PIN_SEL                       0       //0:PB0; 1:PB3;4;PB25
#define SD_D1_PIN_SEL                       4       //4:PB28
#define SD_D2_PIN_SEL                       4       //4:PB29
#define SD_D3_PIN_SEL                       4       //4:PB24
#define SD_CLK_PIN_SEL                      0      //0:PB1; 1:PB6; 4:PB26
#define SD_CMD_PIN_SEL                      0       //0:PB2; 1:PB4; 4:PB27
#define SD_CLKDIV                           4       //0:--; 1:24MHz; 2:12MHz; 4:6MHz; 8:3MHz; 16:1.5MHz; 32:750KHz; 64:375KHz;


#if !SD_EN
#undef SD_DETECT_EN
#define SD_DETECT_EN                        0
#endif

/***************************************************************************
 *               twi config
 ***************************************************************************/
#define TWI1_EN                             0       //TWI1模块
#define TWI1_SCL_PIN_SEL                    1       //0:PA4; 1:PB0; 2:PB13; 3:PB18; 4:PB22; 5:PB28; 6:PB30; 7:PC2; 8:PD1; 9:PD3; 10:PD7; 11:PD11; 12:PD12
#define TWI1_SDA_PIN_SEL                    4       //0:PA5; 1:PB2; 2:PB11; 3:PB12; 4:PB15; 5:PB19; 6:PB23; 7:PB29; 8:PC2; 9:PD0; 10:PD2; 11:PD6; 12:PD10; 13:PD12
#define TWI1_SD_SHARE_EN                    0       //TWI1与SD共用PB0/PB2
#if !TWI1_EN
#undef TWI1_SD_SHARE_EN
#define TWI1_SD_SHARE_EN                    0
#endif

#define TWI2_EN                             0       //TWI2模块
#define TWI2_SCL_PIN_SEL                    2       //0:PB4; 1:PB7; 2:PB8; 3:PB9; 4:PB16; 5:PB20; 6:PB26; 7:PC2; 8:PD4; 9:PD8;
#define TWI2_SDA_PIN_SEL                    2       //0:PB6; 1:PB8; 2:PB9; 3:PB10; 4:PB17; 5:PB21; 6:PB27; 7:PC2; 8:PD3; 9:PD5; 10:PD7; 11:PD10; 
#define TWI2_SD_SHARE_EN                    0       //TWI2与SD共用PB3/PB4
#define TWI2_DM_SHARE_EN                    0       //TWI2_SCL与USB_DM共用PB3
#if !TWI2_EN
#undef TWI2_SD_SHARE_EN
#undef TWI2_DM_SHARE_EN
#define TWI2_SD_SHARE_EN                    0
#define TWI2_DM_SHARE_EN                    0
#endif

#define TWI_IO_EN                           1       //软件利用gpio模拟twi
#define TWI_IO_SCL_PIN                      PIN_D5
#define TWI_IO_SDA_PIN                      PIN_D6

/***************************************************************************
 *               spi config
 ***************************************************************************/
#define SPI0_EN                             0       //SPI0模块
#define SPI0_CS_PIN_SEL                     0       //0:PB7
#define SPI0_CLK_PIN_SEL                    0       //0:PB0
#define SPI0_MOSI_PIN_SEL                   0       //0:PB1
#define SPI0_MISO_PIN_SEL                   0       //0:PB2

#define SPI1_EN                             0       //SPI1模块
#define SPI1_MODE_SEL                       1       //0:使用gpio软件模拟; 1:使用spi1硬件模块
#define SPI1_DIRECT_SEL                     0       //0:四线模式:CS/CLK/MOSI/MISO,支持读/写; 1:三线模式:CS/CLK/MOSI,支持读/写(spiflash的SO通过电阻接到SI)
#define SPI1_CS_PIN_SEL                     1       //0:PA15(fpga);1:PB2;2:PB6;3:PB13;4:PB14;5:PB16;6:PB19;7:PB25;8:PD1;9:PD2;10:PD7;11:PD9
#define SPI1_CLK_PIN_SEL                    2       //0:PA14(fpga);1:PA4;2:PB1;3:PB5;4:PB10;5:PB18;6:PB27;7:PD5;8:PD8;9:PD12
#define SPI1_MOSI_PIN_SEL                   2       //0:PA16(fpga);1:PA5;2:PB0;3:PB3;4:PB4;5:PB11;6:PB12;7:PB17;8:PB26;9:PD4;10:PD10
#define SPI1_MISO_PIN_SEL                   1       //0:PA17(fpga);1:PB3;2:PB10;3:PB12;4:PB16;5:PB19;6:PB24;7:PB28;8:PD3;9:PD6;10:PD11(仅在四线模式时用到)

#define SPI_FLASH_TONE_SEL                  0       //0:内置spiflash; 1:外部spiflash（使用SPI0）; 2:外部spiflash（使用SPI1）
/* 外部spi-flash容量：SPI0支持最大1.5Mbyte，SPI1支持最大16Mbyte */
#define SPI_FLASH_SIZE                      (1*1024*1024) //外置flash容量(单位:byte)

/***************************************************************************
 *               timer config
 ***************************************************************************/
#define TIMER0_EN                           1       //定时器0模块
#define TIMER0_PERIOD                       5000    //us

#define TIMER1_EN                           0       //定时器1模块
#define TIMER1_PERIOD                       100000  //us

#define TIMER2_EN                           0       //定时器2模块
#define TIMER2_PERIOD                       1000  //us

#define TIMER3_EN                           0       //定时器3模块
#define TIMER3_PERIOD                       100000  //us

/***************************************************************************
 *               pmw config
 ***************************************************************************/
#define PWM_EN                              0       //PWM模块

#define PWM_CH0_EN                          0       //PWM通道0
#define PWM_CH1_EN                          0       //PWM通道1
#define PWM_CH2_EN                          0       //PWM通道2
#define PWM_CH3_EN                          0       //PWM通道3
#define PWM_CH4_EN                          0       //PWM通道4
#define PWM_CH5_EN                          0       //PWM通道5

#define PWM_CH0_PIN_SEL                     2       //0:PB0(fpga);1:PA4;2:PB4;3:PB19;4:PB25;5:PD0;6:PD11
#define PWM_CH1_PIN_SEL                     2       //0:PB1(fpga);1:PA5;2:PB7;3:PB14;4:PB20;5:PB24;6:PD1
#define PWM_CH2_PIN_SEL                     2       //0:PB2(fpga);1:PB2;2:PB8;3:PB21;4:PB22;5:PB27;6:PD5
#define PWM_CH3_PIN_SEL                     2       //0:PB3(fpga);1:PB9;2:PB17;3:PB23;4:PB28;5:PD6;6:PD8
#define PWM_CH4_PIN_SEL                     2       //0:PB0(fpga);1:PB6;2:PB15;3:PB29;4:PC2;5:PD3;6:PD9
#define PWM_CH5_PIN_SEL                     2       //0:PB1(fpga);1:PB11;2:PB18;3:PB30;4:PC2;5:PD2;6:PD10

/***************************************************************************
 *               i2s config
 ***************************************************************************/
#define I2S0_EN                             0       //I2S模块
#define IS20_ROLE                           0       //0:slave, 1:master
#define I2S0_BCLK_PIN_SEL                   5       //0:PB9; 1:PB14; 2:PB21; 3:PB22; 4:PB28; 5:PD1; 6:PD5; 7:PD8
#define I2S0_LRCK_PIN_SEL                   6       //0:PB7; 1:PB8; 2:PB13; 3:PB20; 4:PB27; 5:PD0; 6:PD2; 7:PD6; 8:PD7; 9:PD10
#define I2S0_TX_EN                          1
#define I2S0_TX_PIN_SEL                     6       //0:PB5; 1:PB10; 2:PB17; 3:PB23; 4:PB29; 5:PD1; 6:PD3
#define I2S0_RX_EN                          1
#define I2S0_RX_PIN_SEL                     4       //0:PB7; 1:PB12; 2:PB19; 3:PB26; 4:PD4; 5:PB6(I2S0_D5)
#define I2S0_MCLK_EN                        0
#define I2S0_MCLK_PIN_SEL                   1       //0:PB4; 1:PB6; 2:PB7; 3:PB9; 4:PB11; 5:PB18; 6:PB25; 7:PB30; 8:PD4; 9:PD6; 10:PD9;
#define I2S0_MCLK_FACTOR                    256     //mclk分频比（mclk频率是采样率的多少倍）
#define I2S0_FORMAT                         0       //0:标准I2S; 1:左对齐模式（left-justified）; 2:右对齐模式（right-justified）; 3:PCM long Frame; 4:PCM Short Frame
#define I2S0_SLOT_WIDTH                     1       //0:16; 1:32; (LRCK周期 = slot_num * slot_width * BCLK周期)
#define I2S0_SLOT_NUM                       2       //0:1-slot, 1:2-slot; 2:4-slot; 3:8-slot; 4:16-slot;(2ch 固定配成2-slot即可)
#define I2S0_SAMPLE_RESOLUTION              3       //采样精度（采样精度必须小于等于slot_width) 0:16bit; 1:20bit; 2:24bit; 3:32bit
#define I2S0_USE_WM8978                     0       //0:不使用WM8978芯片； 1：I2S接WM8978芯片，使用TWI1控制； 2：I2S接WM8978芯片，使用TWI2控制
#define I2S0_SLAVER_REC_EN                  0       //I2S从机录音
#define I2S0_SLAVER_PLAY_EN                 0       //I2S从机播放

/***************************************************************************
 *               spdif config
 ****************************************************************************/
#define SPDIF_TX_EN                         0       //SPDIF模块
#define SPDIF_TX_PIN_SEL                    1       //0:PB3; 1:PB5; 2:PB7; 3:PB8; 4:PB14; 5:PB24; 6:PB28
#define SPDIF_RX_EN                         0       //SPDIF模块
#define SPDIF_RX_PIN_SEL                    1       //0:PB4; 1:PB6; 2:PB7; 3:PB10; 4:PB15; 5:PB24; 6:PB29
#define SPDIF_COA_RX_PIN_SEL                0       //1: PD1 0: disable
/***************************************************************************
 *               uart config
 ***************************************************************************/
#define UART0_EN                            1       //UART0模块 用于打印
#define UART0_TXBAUDRATE                    3000000 //波特率
#define UART0_RXBAUDRATE                    3000000
#define UART0_SCLK_FREQ                     48000000
#define UART0_TX_PIN_SEL                    11       //0:PA0(FPGA); 1:PB2; 2:PB3; 3:PB4; 4:PB6; 5:PB8; 6:PB22; 7:PB25; 8:PB29; 9:PB30; 10:PD0; 11:PD5; 12:PD7; 13:PD9; 14:PD10; 15:PD11; 16:PD12
#define UART0_RX_PIN_SEL                    99       //0:PA1(FPGA); 1:PB1; 2:PB3; 3:PB5; 4:PB23; 5:PB24; 6:PB26; 7:PB29; 8:PD1; 9:PD6; 10:PD8; 11:PD9; 12:PD12

#define UART1_EN                            0     //UART1模块 用于充电仓通信
#define UART1_TXBAUDRATE                    115200  //波特率
#define UART1_RXBAUDRATE                    115200  //波特率
#define UART1_HWFLOWCTL                     UART_HWCONTROL_NONE //流控
#define UART1_SCLK_FREQ                     24000000
#define UART1_FIFO_EN                       1
#define UART1_DMA_EN                        0
#define UART1_CTS_PIN_SEL                   99      //99:null
#define UART1_RTS_PIN_SEL                   99      //99:null
#define UART1_TX_PIN_SEL                    12      //0:PA4; 1:PB0; 2:PB7; 3:PB9; 4:PB12; 5:PB18; 6:PB20; 7:PB23; 8:PB24; 9:PB26; 10:PB30; 11:PD2; 12:PD6; 13:PD7
#define UART1_RX_PIN_SEL                    11       //0:PA5; 1:PB0; 2:PB8; 3:PB13; 4:PB19; 5:PB21; 6:PB22; 7:PB24; 8:PB25; 9:PD1; 10:PD3; 11:PD5; 12:PD7

#define UART2_EN                            0       //UART2模块
#define UART2_TXBAUDRATE                    115200  //波特率
#define UART2_RXBAUDRATE                    115200
#define UART2_HWFLOWCTL                     UART_HWCONTROL_NONE //流控
#define UART2_SCLK_FREQ                     48000000
#define UART2_FIFO_EN                       1
#define UART2_DMA_EN                        0
#define UART2_CTS_PIN_SEL                   99       //0:PB13
#define UART2_RTS_PIN_SEL                   99       //0:PB12
#define UART2_TX_PIN_SEL                    99       //0:PA4; 1:PB9; 2:PB12; 3:PB14; 4:PB16; 5:PB20; 6:PB22; 7:PB24; 8:PB25; 9:PB27; 10:PD0; 11:PD1; 12:PD8
#define UART2_RX_PIN_SEL                    6        //0:PA5; 1:PB8; 2:PB10; 3:PB15; 4:PB17; 5:PB21; 6:PB23; 7:PB26; 8:PB30; 9:PD0; 10:PD10; 11:PD12

#define UART_DEBUG                          0       //用于打印的uart，0:uart0; 1:uart1; 2:uart2;

/***************************************************************************
 *               LCDC config
 ***************************************************************************/
#define LCDC_EN                             1//0

#define EVB_LCD_DEMO                        1

#define LCDC_INTERFACE                      1                //0:8080 mode,1:spi mode
#define LCDC_SPI_LINE_SEL                   0                //0:四线模式:CSX/SCL/DCX/SDA/(SDO); 1:三线模式:CSX/SCL/SDA/(SDO)
#define LCDC_SPI_INTERFACE                  0                //0:Interface I(SDA和SDO复用),1:Interface II(SDA和SDO独立)
#define LCDC_DMA_RGB_CHAGE                  0                //使用dma刷新时是否做大小端转换
#define LCD_USE_TE                          0                //使用TE触发写gram


#define LCDC_IM0_PIN_NUM                    (PIN_D12)
#define LCDC_IM1_PIN_NUM                    (PIN_B41)//1118
#define LCDC_IM2_PIN_NUM                    (PIN_B30)
#define LCD_RESET_PIN_NUM                   (PIN_D8)//(PIN_B0)
#define LCD_BACKLIGHT_PIN_NUM               (PIN_B3)///(PIN_D15)   //PIN_D15 for fgpa
#define LCD_TE_INT_PIN_NUM                  (PIN_B19)   //ty^2021.04.25

#if LCDC_INTERFACE                          //spi
#define LCDC_X0_SEL                         0             //0:DIS;1:PD0(fpga);2:PA4;3:PB14;4:PD11
#define LCDC_X1_SEL                         0             //0:DIS;1:PD1(fpga);2:PA5;3:PB15
#define LCDC_X2_SEL                         0             //0:DIS;1:PD2(fpga);2:PB16
#define LCDC_X3_SEL                         0             //0:DIS;1:PD3(fpga);2:PB17
#define LCDC_X4_SEL                         0             //0:DIS;1:PD4(fpga);2:PB18
#define LCDC_X5_SEL                         0             //0:DIS;1:PD5(fpga);2:PB19
#define LCDC_X6_SEL                         0//3          //0:DIS;1:PD6(fpga);2:PB20;3:PD1
#define LCDC_X7_SEL                         0//3          //0:DIS;1:PD7(fpga);2:PB21;3:PD2
#define LCDC_X8_SEL                         0//3          //0:DIS;1:PD8(fpga);2:PB6;3:PD3
#define LCDC_X9_SEL                         0             //0:DIS;1:PD9(fpga);2:PB5;3:PD4
#define LCDC_X10_SEL                        0             //0:DIS;1:PD10(fpga);2:PB4;3:PD5
#define LCDC_X11_SEL                        0             //0:DIS;1:PD11(fpga);2:PB3;3:PD6
#define LCDC_X12_SEL                        2//0          //0:DIS;1:PD12(fpga);2:PB2;3:PD7
#define LCDC_X13_SEL                        0             //0:DIS;1:PD13(fpga);2:PB1;3:PD8
#define LCDC_X14_SEL                        2//0          //0:DIS;1:PD14(fpga);2:PB0;3:PD9
#define LCDC_X15_SEL                        2//0          //0:DIS;1:PD15(fpga);2:PD10

#if !LCDC_SPI_LINE_SEL                      //4 line
#undef LCDC_X9_SEL
#define LCDC_X9_SEL                         3           //0:DIS;1:PD9(fpga);2:PB5;3:PD4
#endif

#if LCDC_SPI_INTERFACE                      //interface II
#undef LCDC_X11_SEL
#define LCDC_X11_SEL                        3           //0:DIS;1:PD11(fpga);2:PB3;3:PD6
#endif

#else                                       //8080
#define LCDC_X0_SEL                         4           //0:DIS;1:PD0(fpga);2:PA4;3:PB14;4:PD11
#define LCDC_X1_SEL                         0           //0:DIS;1:PD1(fpga);2:PA5;3:PB15
#define LCDC_X2_SEL                         0           //0:DIS;1:PD2(fpga);2:PB16
#define LCDC_X3_SEL                         0           //0:DIS;1:PD3(fpga);2:PB17
#define LCDC_X4_SEL                         0           //0:DIS;1:PD4(fpga);2:PB18
#define LCDC_X5_SEL                         2           //0:DIS;1:PD5(fpga);2:PB19
#define LCDC_X6_SEL                         3           //0:DIS;1:PD6(fpga);2:PB20;3:PD1
#define LCDC_X7_SEL                         3           //0:DIS;1:PD7(fpga);2:PB21;3:PD2
#define LCDC_X8_SEL                         3           //0:DIS;1:PD8(fpga);2:PB6;3:PD3
#define LCDC_X9_SEL                         3           //0:DIS;1:PD9(fpga);2:PB5;3:PD4
#define LCDC_X10_SEL                        3           //0:DIS;1:PD10(fpga);2:PB4;3:PD5
#define LCDC_X11_SEL                        3           //0:DIS;1:PD11(fpga);2:PB3;3:PD6
#define LCDC_X12_SEL                        3           //0:DIS;1:PD12(fpga);2:PB2;3:PD7
#define LCDC_X13_SEL                        3           //0:DIS;1:PD13(fpga);2:PB1;3:PD8
#define LCDC_X14_SEL                        3           //0:DIS;1:PD14(fpga);2:PB0;3:PD9
#define LCDC_X15_SEL                        2           //0:DIS;1:PD15(fpga);2:PD10

#undef UART0_TX_PIN_SEL
#define UART0_TX_PIN_SEL                    1       //0:PA0(FPGA); 1:PB2; 2:PB3; 3:PB4; 4:PB6; 5:PB8; 6:PB22; 7:PB25; 8:PB29; 9:PB30; 10:PD0; 11:PD5; 12:PD7; 13:PD9; 14:PD10; 15:PD11; 16:PD12

#endif

/*
 LCDC IO MAPPING:
 0x0: CSX,   0x1: DCX,   0x2:WRX/SDA1,0x3: RDX,
 0x4: DAT[0],0x5: DAT[1],0x6: DAT[2], 0x7: DAT[3],
 0x8: DAT[4],0x9: DAT[5],0xA: DAT[6], 0xB: DAT[7],
 0xC: SCL,   0xD: SDA0,  0xE: SDO,    0xF: Disable
 */
#if LCDC_INTERFACE                          //spi
#define LCDC_X0_MAP                         0xF      //Disable
#define LCDC_X1_MAP                         0xF      //Disable
#define LCDC_X2_MAP                         0xF      //Disable
#define LCDC_X3_MAP                         0xF      //Disable
#define LCDC_X4_MAP                         0xF      //Disable
#define LCDC_X5_MAP                         0xF      //Disable
#define LCDC_X6_MAP                         0xF      //CSX       (sel PD1) 
#define LCDC_X7_MAP                         0xF      //SCL       (sel PD2)
#define LCDC_X8_MAP                         0xF      //SDA0      (sel PD3)
#define LCDC_X9_MAP                         0x0      //Disable
#define LCDC_X10_MAP                        0xF      //Disable
#define LCDC_X11_MAP                        0xF      //Disable
#define LCDC_X12_MAP                        0x1      //Disable
#define LCDC_X13_MAP                        0xF      //Disable
#define LCDC_X14_MAP                        0xC      //Disable
#define LCDC_X15_MAP                        0xD      //Disable

#if !LCDC_SPI_LINE_SEL                      //4 line
#undef LCDC_X9_MAP
#define LCDC_X9_MAP                         0x0     //DCX       (sel PD4)
#endif

#if LCDC_SPI_INTERFACE                      //interface II
#undef LCDC_X11_MAP
#define LCDC_X11_MAP                        0xE     //SDO       (sel PD6)
#endif

#else                                       //8080
#define LCDC_X0_MAP                         0x0     //CSX       (sel PD11)
#define LCDC_X1_MAP                         0xF     //Disable
#define LCDC_X2_MAP                         0xF     //Disable
#define LCDC_X3_MAP                         0xF     //Disable
#define LCDC_X4_MAP                         0xF     //Disable
#define LCDC_X5_MAP                         0x1     //DCX       (sel PB19)
#define LCDC_X6_MAP                         0x2     //WRX/SDA1  (sel PD1)
#define LCDC_X7_MAP                         0x3     //RDX       (sel PD2)
#define LCDC_X8_MAP                         0x4     //DAT[0]    (sel PD3)
#define LCDC_X9_MAP                         0x5     //DAT[1]    (sel PD4)
#define LCDC_X10_MAP                        0x6     //DAT[2]    (sel PD5)
#define LCDC_X11_MAP                        0x7     //DAT[3]    (sel PD6)
#define LCDC_X12_MAP                        0x8     //DAT[4]    (sel PD7)
#define LCDC_X13_MAP                        0x9     //DAT[5]    (sel PD8)
#define LCDC_X14_MAP                        0xA     //DAT[6]    (sel PD9)
#define LCDC_X15_MAP                        0xB     //DAT[7]    (sel PD10)

#endif

#if EVB_LCD_DEMO
#undef LCDC_EN
#define LCDC_EN                             1
#endif
/***************************************************************************
 *               tone config
 ***************************************************************************/
#define TONE_EN                                       1 //提示音总开关
#define TONE_VOLUME                                  50 //默认的提示音音量，如果某个提示音没有指定音量，则自动使用此音量
#define TONE_M_CONNECTED_NOTIFY_S_PLAY_CONNECTED_TONE 1 //单耳连接手机然后对耳连接上，则副耳是否需要播放连接提示音（0:不播放；1:播放）

#define TONE_WELCOME_EN                     0       //开机提示音
#define TONE_BT_MODE_EN                     1       //蓝牙模式提示音
#define TONE_G24_MODE_EN                    1       //2.4G模式提示音
#define TONE_MUSIC_MODE_EN                  0       //音乐模式提示音
#define TONE_CLOCK_MODE_EN                  0       //clock模式提示音
#define TONE_FM_MODE_EN                     0       //FM模式提示音
#define TONE_LINEIN_MODE_EN                 0       //linein模式提示音
#define TONE_PC_MODE_EN                     0       //PC模式提示音
#define TONE_POWER_OFF_EN                   0       //关机提示音
#define TONE_RECORDING_MODE_EN              0       //录音模式提示音
#define TONE_CONNECTED_EN                   1       //蓝牙连接提示音
#define TONE_DISCONNECTED_EN                1       //蓝牙断开连接提示音
#define TONE_PEER_CONNECTED_EN              0       //对耳连接提示音：0:disable; 1:使用对耳连接提示音； 2：使用蓝牙连接提示音
#define TONE_PEER_DISCONNECTED_EN           0       //对耳断开连接提示音：0:disable; 1:使用对耳断开提示音； 2：使用蓝牙断开连接提示音
#define TONE_INCOMING_NUM_EN                0       //来电报号提示音
#define TONE_CALL_EN                        2       //来电铃声: 0:disable; 1:使用本地来电提示音； 2：优先播手机的来电提示音，没有则播本地来电提示音
#define TONE_ALARM_EN                       0       //闹钟提示音
#define TONE_MAX_VOLUME_EN                  0       //最大音量提示音
#define TONE_MIN_VOLUME_EN                  0       //最小音量提示音
#define TONE_LOW_POWER_EN                   0       //低电量提示音(必须为wav(linear pcm))
#define TONE_BT_NEXT_EN                     0       //蓝牙模式下一首提示音
#define TONE_BT_PREV_EN                     0       //蓝牙模式上一首提示音
#define TONE_MUSIC_NEXT_EN                  0       //音乐模式下一首提示音
#define TONE_MUSIC_PREV_EN                  0       //音乐模式上一首提示音
#define TONE_KEY_EN                         0       //按键提示音(必须为wav(linear pcm))
#define TONE_GAME_MODE_OPEN_EN              1       //"开启游戏模式"提示音
#define TONE_GAME_MODE_OPEN_AFX_EN          1       //"开启游戏音效模式"提示音
#define TONE_GAME_MODE_CLOSE_EN             1       //"关闭游戏模式"提示音

//提示音音量
#define TONE_BLUETOOTH_MODE_VOL             60
#define TONE_G24_MODE_VOL                   60
#define TONE_CALL_VOL                       60
#define TONE_CONNECTED_VOL                  60
#define TONE_DISCONNECTED_VOL               60
#define TONE_LOW_POWER_VOL                  80
#define TONE_PEER_CONNECTED_VOL             60
#define TONE_PEER_DISCONNECTED_VOL          60
#define TONE_POWER_OFF_VOL                  60
#define TONE_WELCOME_VOL                    60
#define TONE_GAME_MODE_OPEN_VOL             60
#define TONE_GAME_MODE_OPEN_AFX_VOL         60
#define TONE_GAME_MODE_CLOSE_VOL            60

/***************************************************************************
 *               rtc config
 ***************************************************************************/
/*
 *  * 如果BAT_EXIT是1，请保持RTC_EN是1，基础系统用到了RTC的秒中断；
 * RTC模块的功耗特别低，不必在意模块常开。
 */
#define RTC_EN                              1       //RTC模块
#define RTC_CLOCK_EN                        0       //时钟
#define RTC_ALARM0_EN                       1       //闹钟0，可设置在一定时间（例如10秒）后闹铃响
#define RTC_ALARM1_EN                       0       //闹钟1，可设置在某个时间点（例如07:05:30）闹铃响，可分别配置周一/二/三/四/五/六/日

#define RTC_DCXO_CUR                        0x18
#define RTC_DCXO_LP_EN                      1
#define RTC_XO32K_EN                        0

/***************************************************************************
 *               key config
 ***************************************************************************/
#define KEY_AD_EN                           1       //AD按键
#define KEY_IO_EN                           0       //IO按键
#define KEY_IR_EN                           0       //红外遥控按键
#define KEY_TOUCH_EN                        0       //触摸按键
#define KEY_CODING_EN                       0       //编码开关旋钮
#define KEY_UART_EN                         0       //从UART接收按键
#define KEY_ONOFF_EN                        1       //ONOFF按键
#define KEY_DIG_EN                          0       //数字按键

/*
* 按键功能
*/
#define KEY_FUNC_S_EN                       1       //short
#define KEY_FUNC_U_EN                       1       //up
#define KEY_FUNC_SU_EN                      1       //short up
#define KEY_FUNC_L_EN                       1       //long
#define KEY_FUNC_LU_EN                      1       //long up
#define KEY_FUNC_H_EN                       1       //hold
#define KEY_FUNC_LL_EN                      0       //long long (1s/2s/3s/...)
#define KEY_FUNC_M_EN                       1       //multiple click

/*
* 按键时间
*/
#define KEY_TIME_SHORT                      25      //short去抖时间(单位ms,要求5的倍数)
#define KEY_TIME_UP                         25      //up去抖时间(单位ms,要求5的倍数)
#define KEY_TIME_LONG                       750     //long时间(单位ms,要求5的倍数)
#define KEY_TIME_HOLD                       150     //hold时间(单位ms,要求5的倍数


/***************************************************************************
 *               keyadc config
 ***************************************************************************/
#define KADC0_EN                            0       //PB30
#define KADC1_EN                            1       //PD0
#define KADC2_EN                            0       //PD3
#define KADC3_EN                            0       //PD4
#define KADC4_EN                            0       //PD5
#define KADC5_EN                            0       //PD6
#define KADC6_EN                            0       //PD9
#define KADC7_EN                            0       //PB3
#define KADC8_EN                            0       //PB5
#define KADC9_EN                            0       //PB6


/*
* 0: 查询方式（硬件上需确保adc输入电压小于VCCIO，不可以直接上拉到VCCIO）;
* 1: 中断方式;
*/
#define KADC0_IRQ                           0
#define KADC1_IRQ                           0
#define KADC2_IRQ                           0
#define KADC3_IRQ                           0
#define KADC4_IRQ                           0
#define KADC5_IRQ                           0
#define KADC6_IRQ                           0
#define KADC7_IRQ                           0
#define KADC8_IRQ                           0
#define KADC9_IRQ                           0

/*
* 0: 按键;
* 1: 电位器旋钮;
*/
#define KADC0_KNOB                          0
#define KADC1_KNOB                          0
#define KADC2_KNOB                          0
#define KADC3_KNOB                          0
#define KADC4_KNOB                          0
#define KADC5_KNOB                          0
#define KADC6_KNOB                          0
#define KADC7_KNOB                          0
#define KADC8_KNOB                          0
#define KADC9_KNOB                          0

/***************************************************************************
 *               keyio config
 ***************************************************************************/
#define KIO_GPIOB                           0       //使用PB组GPIO
#define KIO_GPIOC                           0       //使用PC组GPIO
#define KIO_GPIOD                           1       //使用PD组GPIO

#define KIO_M_EN                            0       //耳机上1个按键实现多功能（包括单击、多击、长按）

//#define PIN_KIO1                            (PIN_D0)

#if KIO_M_EN
#define PIN_KIO0                            (PIN_D4) //force sensor 中断检测并做按键处理

#undef KEY_FUNC_L_EN
#undef KEY_FUNC_LU_EN
#undef KEY_FUNC_H_EN
#undef KEY_FUNC_LL_EN
#undef KEY_FUNC_M_EN
#define KEY_FUNC_L_EN                       0
#define KEY_FUNC_LU_EN                      0
#define KEY_FUNC_H_EN                       0
#define KEY_FUNC_LL_EN                      1
#define KEY_FUNC_M_EN                       1
#endif

#if !KEY_IO_EN
#undef KIO_GPIOB
#undef KIO_GPIOC
#undef KIO_GPIOD

#define KIO_GPIOB                           0
#define KIO_GPIOC                           0
#define KIO_GPIOD                           0

#endif

/***************************************************************************
 *               digital key config
 ***************************************************************************/
#define KDIG0_EN                            0
#define KDIG1_EN                            0
#define KDIG2_EN                            0

#define KDIG0_SEL                           2        //0:PC2;1:PD0;2:PD12
#define KDIG1_SEL                           3        //0:PB3;1:PB30;2:PC2;3:PD10
#define KDIG2_SEL                           3        //0:PB4;1:PB5;2:PC2;3:PD8

/***************************************************************************
 *               irrx config
 ***************************************************************************/
#define IRRX_PIN_SEL                        0       //0:PB30:--; 1:PD12; 2:PC2;

/***************************************************************************
 *               disp config
 ***************************************************************************/
#define DISP_EN                             0       //显示
#define LED_DIGITAL_EN                      0       //数码管
#define LED_DIODE_EN                        0       //单个二极管
#define LCD_SEG_EN                          0       //段码液晶屏

//LED_DIGITAL:
#define LEDC_X0_SEL                         0           //0:DIS;1:PD0(fpga);2:PA4;3:PB14;4:PD11
#define LEDC_X1_SEL                         0           //0:DIS;1:PD1(fpga);2:PA5;3:PB15
#define LEDC_X2_SEL                         0           //0:DIS;1:PD2(fpga);2:PB16
#define LEDC_X3_SEL                         0           //0:DIS;1:PD3(fpga);2:PB17
#define LEDC_X4_SEL                         0           //0:DIS;1:PD4(fpga);2:PB18
#define LEDC_X5_SEL                         0           //0:DIS;1:PD5(fpga);2:PB19
#define LEDC_X6_SEL                         0           //0:DIS;1:PD6(fpga);2:PB20;3:PD1
#define LEDC_X7_SEL                         0           //0:DIS;1:PD7(fpga);2:PB21;3:PD2
#define LEDC_X8_SEL                         3           //0:DIS;1:PD8(fpga);2:PB6;3:PD3
#define LEDC_X9_SEL                         3           //0:DIS;1:PD9(fpga);2:PB5;3:PD4
#define LEDC_X10_SEL                        3           //0:DIS;1:PD10(fpga);2:PB4;3:PD5
#define LEDC_X11_SEL                        3           //0:DIS;1:PD11(fpga);2:PB3;3:PD6
#define LEDC_X12_SEL                        3           //0:DIS;1:PD12(fpga);2:PB2;3:PD7
#define LEDC_X13_SEL                        3           //0:DIS;1:PD13(fpga);2:PB1;3:PD8
#define LEDC_X14_SEL                        3           //0:DIS;1:PD14(fpga);2:PB0;3:PD9
#define LEDC_X15_SEL                        0           //0:DIS;1:PD15(fpga);2:PD10

/*
 LEDC IO MAPPING:
 0x0~0x7:LEDC_D0~LEDC_D7,0x8: Disable
 */
#define LEDC_X0_MAP                         0x8
#define LEDC_X1_MAP                         0x8
#define LEDC_X2_MAP                         0x8
#define LEDC_X3_MAP                         0x8
#define LEDC_X4_MAP                         0x8
#define LEDC_X5_MAP                         0x8
#define LEDC_X6_MAP                         0x8
#define LEDC_X7_MAP                         0x8
#define LEDC_X8_MAP                         0x0
#define LEDC_X9_MAP                         0x1
#define LEDC_X10_MAP                        0x2
#define LEDC_X11_MAP                        0x3
#define LEDC_X12_MAP                        0x4
#define LEDC_X13_MAP                        0x5
#define LEDC_X14_MAP                        0x6
#define LEDC_X15_MAP                        0x8

//LED_DIODE:
#define LED_RED_EN                          0
#define LED_RED_PIN_NUM                     (PIN_D10)
#define LED_RED_PIN_ACTIVE_STATE            3       //0-输出低电平；1-输出高电平；2-下拉2K；3-上拉2K

#define LED_BLUE_EN                         0
#define LED_BLUE_PIN_NUM                    (PIN_B0)
#define LED_BLUE_PIN_ACTIVE_STATE           3       //0-输出低电平；1-输出高电平；2-下拉2K；3-上拉2K

#define STATE_POWERON_EN                    1       //开机灯光
#define STATE_POWEROFF_EN                   1       //关机灯光

//charge
#define STATE_CHARGEING_EN                  1       //充电灯光
#define STATE_UNCHARGEING_EN                1       //停止充电灯光
#define STATE_BAT_FULL_EN                   1       //满电灯光
#define STATE_BAT_LOW_EN                    1       //低电灯光
#define STATE_BAT_UNLOW_EN                  1       //不低电灯光

//bt
#define STATE_BT_PAIR_EN                    0       //配对灯光
#define STATE_BT_CON_EN                     1       //连接灯光
#define STATE_BT_DIS_EN                     1       //断开灯光
#define STATE_BT_PEER_CON_EN                0       //对耳连接灯光
#define STATE_BT_PEER_DIS_EN                0       //对耳断开灯光

#define STATE_BT_CALLING_EN                 0       //通话灯光
#define STATE_BT_CALLING_END_EN             0       //通话结束灯光
#define STATE_BT_INCOMING_EN                0       //来电灯光

#define STATE_CLEAN_LINK_EN                 0       //恢复出厂灯光
#define STATE_ENTER_DUT_EN                  0       //DUT模式灯光

#define STATE_BT_PLAY_EN                    0       //播放音乐灯光
#define STATE_BT_PAUSE_EN                   0       //播放暂停灯光

#define STATE_DISP_SUSPEN_EN                0       //灯光显示挂起
#define STATE_DISP_RESUME_EN                0       //灯光显示恢复

#define STATE_DISP_SLEEP_EN                 0       //睡眠待机灯光
#define STATE_DISP_WAKEUP_EN                0       //唤醒灯光


//LCD_SEG:
#define LCD_SEG_REUSE                       0       //复用做输入检测功能

#if !DISP_EN
#undef LED_DIGITAL_EN
#undef LED_DIODE_EN
#undef LCD_SEG_EN
#define LED_DIGITAL_EN                      0
#define LED_DIODE_EN                        0
#define LCD_SEG_EN                          1
#endif

/***************************************************************************
 *               motor config
 ***************************************************************************/
#define MOTOR_EN                            0

/***************************************************************************
 *               sensor config
 ***************************************************************************/
#define SENSOR_EN                           0

#define SENSOR_G_EN                         0       //Digital Triaxial Accelerometer,用于敲击检测
#define SENSOR_G_DEVICE_SEL                 0       //0-MSA301 //1-SC7A20
#define SENSOR_G_CLICK_SEL                  1       //0-单击; 1-双击; 2-多击
#define SENSOR_G_INT_PIN_NUM                (PIN_B8)

#define SENSOR_PROXIMITY_EN                 0       //Proximity Sensor,用于佩戴检测
#define SENSOR_PROXIMITY_DEVICE_SEL         1       //0-PA22409001; 1-HY2751
#if (SENSOR_PROXIMITY_EN == 1)
#if (SENSOR_PROXIMITY_DEVICE_SEL == 1)
#define SENSOR_PROXIMITY_DETECT_MODE        0       //0:loop mode, 1:interrupt
#if (SENSOR_PROXIMITY_DETECT_MODE == 0)
#undef TIMER1_EN
#define TIMER1_EN                           1
#endif
#endif
#endif
#define SENSOR_PROXIMITY_INT_PIN_NUM        (PIN_D3)
#define SENSOR_PROXIMITY_CTL_PIN_NUM        (PIN_B23)
#define SENSOR_PROXIMITY_NO_PLAY_WHEN_LT_TO 0       //出耳超时不再播放

#define SENSOR_BONE_VIBRATE_EN              0       //MEMS digital output motion sensor,骨振动传感器,用于采集语音

#define SENSOR_FORCE_EN                     0       //Force touch,用于压感按键
#define SENSOR_FORCE_DEVICE_SEL             1       //0-AF3068 1-DF100
#define SENSOR_FORCE_INT_PIN_NUM            (PIN_D4)

/***************************************************************************
 *               pmu startup mode config
***************************************************************************/
/*
 * 0: ONOFF按键开机/板上硬开关/板上软开关
 * 1: ONOFF高开机（HSW）
 */
#define PMU_POWER_ON_MODE                   0

/***************************************************************************
 *               pmu config
 ***************************************************************************/
#define PMU_CORE_USE_DCDC                   1       //HS系列使用DCDC给CORE供电，WS/CA系列使用LDO给CORE供电
#define PMU_CORELDO_EN                      1
#define PMU_CORELDO_LP_MODE                 2

/* 开关模式设置 */
#define PMU_HSW_SEL                         0       //0:软开关, 1:硬开关
#define PMU_HSW_MODE                        0       //0:低开机，1:高开机

/* 开机唤醒源设置 */
#define PMU_SYSIN_POWERON_EN                1       //使能系统首次上电开机
#define PMU_VBUSIN_POWERON_EN               1       //使能VBUS插入开机
#define PMU_VBUSOUT_POWERON_EN              0       //使能VBUS拔出开机
#define PMU_VBUSPPR_POWERON_EN              0       //使能VBUS私有协议开机
#define PMU_VBUSMID_IN_POWERON_EN           0       //使能VBUS中间电平插入开机
#define PMU_VBUSMID_OUT_POWERON_EN          0       //使能VBUS中间电平拔出开机
#define PMU_ULTRA_SHORT_PRESS_POWERON_EN    0       //使能超短按开机
#define PMU_SHORT_PRESS_POWERON_EN          0       //使能短按开机
#define PMU_LONG_PRESS_POWERON_EN           1       //使能长按开机
#define PMU_HYPERLONG_PRESS_POWERON_EN      0       //使能二级长按开机
#define PMU_ULTRA_LONG_PRESS_POWERON_EN     1       //使能超长按开机
#define PMU_HSWON_POWERON_EN                0       //使能高开机
#define PMU_HSWPOS_POWERON_EN               0       //使能上升沿开机
#define PMU_RECHARGE_POWERON_EN             0       //使能回充开机

#define PMU_POWEROFF_WHEN_BAT_FIRST_IN      0       //使能第一次插入电池时关机
#define PMU_ULTRA_LONG_PRESS_TIME           8000    //设置超长按时长，死机时超长按可以复位，单位:ms; 只能选择6000/8000/10000/12000ms
#define PMU_LONG_PRESS_TIME                 1000    //设置长按时长，长按开机或者长按关机，单位:ms; 只能选择1000/2000/3000/4000ms
#define PMU_HYPERLONG_PRESS_TIME            3000    //设置二级长按时长，二级长按时长是基于长按，假如长按3s，二级长按3s，那么6s后会产生二级长按中断，单位:ms; 只能选择1000/2000/3000/4000ms
#define PMU_CHARGE_MIN_EN                   0       //充满后使用充电电流最小档充电
#define PMU_BAT_FULL_VOLT_TUNING            2       //满电电压微调，满电电压一般跟恒压值一致，可以在恒压值基础上递减，0:-0mV，1:-7mV，2:-14mV，3:-21mV
#define PMU_CHARGE_VOLT                     0       //设置充电恒压值，0:4.2V，1:4.3V，2:4.35V，3:4.4V
#define PMU_CHARGE_VOLT_TUNING              2       //充电恒压值微调，在上面恒压值的基础上叠加，0:0mV，1:15mV，2:30mV，3:45mV
#define PMU_CHARGE_CURRENT                  200000  //设置充电电流，是充电时芯片耗电和充到电池的电流总和，单位:uA; base：4000uA, step:8000uA;
#define PMU_SUBCHARGE_CURRENT               10      //设置涓流充电电流，单位:mA;
#define PMU_POWEROFF_CURRENT                0       //设置关机电流，没有电池的方案关机电流要设置为2，有电池的方案关机电流要设置为0
#define PMU_VBUSIN_RESTART                  0       //使能VBUS插入后重启，常用于耳机方案
#define PMU_VCC_RTC_ALWAYS_ON               0       //设置RTC是否常在电，如果需要RTC时间在关机时继续走，应该设置RTC为常在电
#define PMU_VCC_IO_VOLT                     3300    //设置VCC-IO电压，单位:mV，2700~3400mV，步进:100mV，会影响外设GPIO电压
#define PMU_RECHARGE_POST_VOLT              4000    //回充的电压阈值，只有4000,4050,4100,4180这四个配置

//当AVCC < VCCIO时，同一ADC的模拟通路存在一定串扰，同一ADC的IO底层会配置成disbale func,
//AVCC等于VCCIO可减少串扰，底层也不会设置IO，AVCC提高需要同时确保关机电压大于AVCC
#define PMU_AVCC_VOLT                       3000    //设置AVCC电压，单位:mV， 2700~3400mV， 步进:100mV，会影响音频最大音量
#define PMU_NOR_SEC_ERASE_PRO_EN            0       //使能NOR扇区擦除保护，默认不使能，如果在车机或者硬开关会出现突然掉电的产品，则需要使能
#define PMU_NOR_SEC_ERASE_PRO_VOLT          2900    //设置NOR扇区擦除保护电压，低于这个电压，不会对NOR进行擦除，单位:mV，2900/3000/3100/3300mV
#define PMU_VBUS_FAKE_OUT                   0       //就算vbus in也假装是vbus out，满足开发板的特殊需求

#define STAGE_CHARGE_EN                     0       //使能阶梯充电
#define CUTOFF_CURRENT                      24000   //单位:uA; base：4000uA, step:4000uA，一般为C/10

/***************************************************************************
 *               battery config
 ***************************************************************************/
#define BAT_EXIST                           1       //使用内部PMU管理电池
#define BAT_VOLT_DET                        1       //使能内部PMU电池电池电压检测
#define BAT_CAPACITY                        250000  //电池容量，单位：uAh
#define BAT_UPDATE_THRESHOLD                2      //变化大于2mV才会更新

#if MYME_M4_BOARD
#undef BAT_EXIST
#define BAT_EXIST                           1
#endif

#define BAT_LEVEL_0PERCENT_VOLT             3400 //  0%:(3200~3400]
#define BAT_LEVEL_10PERCENT_VOLT            3464 // 10%:(3400~3464]
#define BAT_LEVEL_20PERCENT_VOLT            3527 // 20%:(3434~3527]
#define BAT_LEVEL_30PERCENT_VOLT            3588 // 30%:(3527~3588]
#define BAT_LEVEL_40PERCENT_VOLT            3645 // 40%:(3588~3645]
#define BAT_LEVEL_50PERCENT_VOLT            3728 // 50%:(3645~3728]
#define BAT_LEVEL_60PERCENT_VOLT            3824 // 60%:(3728~3824]
#define BAT_LEVEL_70PERCENT_VOLT            3916 // 70%:(3824~3916]
#define BAT_LEVEL_80PERCENT_VOLT            4014 // 80%:(3916~4014]
#define BAT_LEVEL_90PERCENT_VOLT            4076 // 90%:(4014~4076]
#define BAT_LEVEL_100PERCENT_VOLT           4200 //100%:(4076~4200]

#if !BAT_EXIST
#undef MODE_CHARGE_EN
#define MODE_CHARGE_EN                      0       //如果没有电池，则不需要使能充电模式
#undef BAT_VOLT_DET
#define BAT_VOLT_DET                        0       //如果没有电池，则不需要使能电池电压检测功能
#endif

#define BAT_HW_LB_SHUT_EN                   0       //使能硬件低电关机
#define BAT_HW_LB_SHUT_VOLT                 2700    //设置硬件低电关机电压，只能选择2700/2800/2900/3100mV
#define BAT_HW_OK_VOLT                      2900    //设置硬件允许开机电压，只能选择2900/3000/3100/3300mV
#define BAT_SW_LB_WRN_VOLT                  3500    //设置软件低电提醒电压，单位:mV，2900~4400mV, 步进:100mV
#define BAT_SW_LB_SHUT_VOLT                 3400    //设置软件低电关机电压，单位:mV，2900~4400mV, 步进:100mV
#define BAT_SW_LB_WARNING_PERIOD            30000   //设置软件低电提醒周期，单位:ms, 步进:1000ms

//Twait = 20min
//Tcal = 82ms
#define RC_CAL_PRD_BASE                     60000
#define RC_CAL_PRD                          4
#define RC_CAL_NUM                          1023
#define RC_CAL_CON_MODE                     1
#define RC_POWEROFF_CAL_EN                  1
#if ((MODE_CLOCK_EN == 0) || (BAT_EXIST == 0))
#undef RC_CAL_CON_MODE
#undef RC_POWEROFF_CAL_EN
#define RC_CAL_CON_MODE                     0
#define RC_POWEROFF_CAL_EN                  0
#endif

/***************************************************************************
 *               charging bin config
 ***************************************************************************/
#define CHARGING_BIN                        0       //使能充电仓功能, 0:不使用充电仓， 1：使用充电仓
#define CHARGING_BIN_CODE                   0       //使能充电仓功能, 0:充电仓不支持开关机码， 1：充电仓支持开关机码
#define CHARGING_BIN_CODE_UART              0
#define CHARGING_BIN_MID_LEVEL              0       //支持中间电平的充电仓
#define CHARGING_BIN_MID_LEVEL_THRD         0       //中间电平判断阈值，单位：ms，0：不等待。
#define MODE_KEY_ONOFF                      0       //使能按键(mode key)触发开关机功能
#define CHARGING_BIN_AUTO_PAIR              0       //充电仓内自动配对

#if CHARGING_BIN && CHARGING_BIN_MID_LEVEL
#undef PMU_VBUSOUT_POWERON_EN
#undef PMU_VBUSMID_IN_POWERON_EN
#undef PMU_VBUSMID_OUT_POWERON_EN
#define PMU_VBUSOUT_POWERON_EN              1       //使能VBUS拔出开机
#define PMU_VBUSMID_IN_POWERON_EN           1       //使能VBUS中间电平插入开机
#define PMU_VBUSMID_OUT_POWERON_EN          1       //使能VBUS中间电平拔出开机
#endif


/***************************************************************************
 *               seamless config
 ***************************************************************************/
#define SEAMLESS_EN                         0       //使能无缝切换功能

/***************************************************************************
 *               power/sleep config
 ***************************************************************************/
#define POWER_LPC_MODE_EN                   0       //低功耗模式，没有连接前RF每隔一段时间才起来
#define WIRELESS_PEER_TIME                  3000    //ms, 连接前, 3秒开启连接, 3秒关闭

#define SLEEP_EN                            0       //使能睡眠
#if EQ_ALL_MODE_SUPPORT
#undef SLEEP_EN
#define SLEEP_EN                            0
#endif

#define SLEEP_DELAY                         3       //睡眠延时，当允许睡眠时延迟10s才进入睡眠，例如暂停本地播放10s后进入睡眠
#define TIME_BEFORE_WFI                     3       //进入WFI阈值，单位:ms

#define POWER_OFF_DELAY                     0       //关机延时，单位:ms

#define POWER_OFF_FIRST_EN                  0       //第一次开机，马上关机，如果电池接到VBUS，需要使能该功能，该功能依赖于PMU_VCC_RTC_ALWAYS_ON
#if (PMU_VCC_RTC_ALWAYS_ON == 0)                    //如果RTC关机会掉电，则需要同时关闭 POWER_OFF_FIRST_EN
#undef POWER_OFF_FIRST_EN
#define POWER_OFF_FIRST_EN                  0
#endif

#define POWER_OFF_AUTO_WHEN_CON_DIS         0       //使能蓝牙断开自动关机，当蓝牙断开一段时间就会自动关机
#define POWER_OFF_AUTO_WHEN_ANC_DIS         0       //降噪模式切换到关闭，且蓝牙断开，一段时间就会自动关机
#define POWER_OFF_AUTO_PERIOD               180     //蓝牙断开自动关机时间阈值，单位:s，e.g.300s = 5min


/***************************************************************************
                 watchdog config
 ***************************************************************************/
#define WATCHDOG_EN                 1               //使用看门狗，如果出现挂住会重启系统
#define WATCHDOG_TIMEOUT            16              //单位:秒

/***************************************************************************
 *               cpu freq config
 ***************************************************************************/
#define OVER_FREQ_EN                        0       //CPU频率超过192M时，需要使能超频功能，12M步进，最大不要超过288M

#if !AUDIO_G24_RX_DECODER_DISABLE
#if (LC3_ENC_CH == 2) && (LC3_DEC_CH == 2)
#undef OVER_FREQ_EN
#define OVER_FREQ_EN                        1

#define DVFS_AMIXER_FREQ					{288000000, 288000000}
#else
#define DVFS_AMIXER_FREQ					{192000000, 192000000}
#endif  //#if (LC3_ENC_CH....

#else
#define DVFS_AMIXER_FREQ                    {96000000, 96000000}
#endif
#define DVFS_EFFECTS_FREQ                   {38400000, 38400000}
#define DVFS_TONEMIXER_FREQ                 {96000000, 96000000}
#define DVFS_A2DPAUDIO_FREQ                 {48000000, 48000000}
#define DVFS_SCOAUDIO_FREQ                  {38400000, 38400000}
#define DVFS_SCOAENC_FREQ                   {64000000, 96000000}
#define DVFS_SCOADMAENC_FREQ                {192000000, 192000000}
#define DVFS_USBAUDIO_FREQ                  {192000000, 192000000}
#define DVFS_LINEINAUDIO_FREQ               {192000000, 192000000}
#define DVFS_LOCAL_ENCODE_WAV_FREQ          {192000000, 192000000}
#define DVFS_LOCAL_ENCODE_MP3_FREQ          {192000000, 192000000}
#define DVFS_LOCAL_DECODE_WAV_FREQ          {192000000, 192000000}
#define DVFS_LOCAL_DECODE_MP3_FREQ          {192000000, 192000000}
#define DVFS_DEFAULT_FREQ                   {38400000, 38400000}

#if OVER_FREQ_EN
#undef PMU_CORELDO_EN
#define PMU_CORELDO_EN                      0
#endif

#if PMU_CORELDO_EN
#define VF_TABLE_0                          {32000000, 920}
#define VF_TABLE_1                          {38400000, 920}
#define VF_TABLE_2                          {64000000, 920}
#define VF_TABLE_3                          {96000000, 970}
#define VF_TABLE_4                          {128000000, 1050}
#define VF_TABLE_5                          {192000000, 1050}
#else
#define VF_TABLE_0                          {32000000, 1200}
#define VF_TABLE_1                          {38400000, 1200}
#define VF_TABLE_2                          {64000000, 1200}
#define VF_TABLE_3                          {96000000, 1200}
#define VF_TABLE_4                          {128000000, 1200}
#if (LC3_ENC_CH == 2) && (LC3_DEC_CH == 2)
#define VF_TABLE_5                          {288000000, 1200}
#else
#define VF_TABLE_5                          {192000000, 1200}
#endif
#endif

/***************************************************************************
 *               eq auto load config
 ***************************************************************************/
#define CPU_LOAD_EN                         0       //使能CPU负载统计功能
#define CPU_LOAD_PERIOD                     1000    //设置统计周期


#if APP_EQ_EN
#define EQ_LOAD_EN                          0
#else
#define EQ_LOAD_EN                          1
#endif
#define EQ_LOAD_SHOW_STATUS                 0
#define EQ_LOAD_PERIOD                      2000

/***************************************************************************
 *               system config
 ***************************************************************************/
#define SYS_HEAP_SIZE                      ((size_t) (290*1024))

#if BT_DUT_EN
#undef SYS_HEAP_SIZE
#define SYS_HEAP_SIZE                      ((size_t) (285*1024))
#endif

/***************************************************************************
 *               user board config
 ***************************************************************************/

#if MYME_M4_BOARD

#define MYME_G63PRO                         1

#if MYME_G63PRO
#undef KEY_IO_EN
#define KEY_IO_EN                           1       //IO按键

#undef KIO_GPIOD
#define KIO_GPIOD                           1

#undef PIN_KIO0
#define PIN_KIO0                            (PIN_D1)
#endif

#undef TIMER2_EN
#define TIMER2_EN                           1


#define HP_AMP_EN_PIN_NUM                   (PIN_B5)
#define HP_AMP_ON()                         HAL_gpio_config(HP_AMP_EN_PIN_NUM | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_HIGH)
#define HP_AMP_OFF()                        HAL_gpio_config(HP_AMP_EN_PIN_NUM | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_LOW)
#define HP_AMP_ON_DELAY()                   HAL_msleep(300)

#define G24_DISP_CON()                      //HAL_gpio_config(PIN_B1 | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_LOW);HAL_gpio_config(PIN_B2 | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_HIGH)
#define G24_DISP_DISCON()                   //HAL_gpio_config(PIN_B1 | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_LOW);HAL_gpio_config(PIN_B2 | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_LOW)
#else

#define HP_AMP_ON()
#define HP_AMP_OFF()
#define HP_AMP_ON_DELAY()
#define G24_DISP_CON()
#define G24_DISP_DISCON()
#endif


#if EVB_TX_DEMO

#define BT_POWER_PIN_NUM                    (PIN_B3)
#define BT_POWER_ON()                       HAL_gpio_config(BT_POWER_PIN_NUM | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_HIGH)
#define BT_POWER_OFF()                      HAL_gpio_config(BT_POWER_PIN_NUM | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_LOW)

#define BT_EN_PIN_NUM                       (PIN_B5)
#define BT_EN_ON()                          HAL_gpio_config(BT_EN_PIN_NUM | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_HIGH)
#define BT_EN_OFF()                         HAL_gpio_config(BT_EN_PIN_NUM | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_LOW)

#define BT_DISP_PIN_NUM                     (PIN_B1)
#define BT_DISP_ON()                        HAL_gpio_pull_set(BT_DISP_PIN_NUM, PIN_PULL_UP2K);\
                                            HAL_gpio_func_set(BT_DISP_PIN_NUM, PIN_FUNCINPUT)
#define BT_DISP_OFF()                       HAL_gpio_config(BT_DISP_PIN_NUM | PIN_FUNCDISABLE)

#define AENC_DISP_PIN_NUM                   (PIN_B2)
#define AENC_DISP_ON()                      HAL_gpio_pull_set(AENC_DISP_PIN_NUM, PIN_PULL_UP2K);\
                                            HAL_gpio_func_set(AENC_DISP_PIN_NUM, PIN_FUNCINPUT)
#define AENC_DISP_OFF()                     HAL_gpio_config(AENC_DISP_PIN_NUM | PIN_FUNCDISABLE)

#endif

/***************************************************************************
 *               EQ 工具选项
 ***************************************************************************/
 //硬件
#define EQTOOL_AUDIO_PGA_GAIN_EN            1   //EQ工具模拟增益使能
#define EQTOOL_AUDIO_AUX_ANALOG_GAIN_EN     1   //AUX模拟增益
#define EQTOOL_AUDIO_LINEIN_ANALOG_GAIN     1   //linein模拟增益

//混音的音量
#define EQTOOL_MIXVOLUME_SPDIF              1   //SPDIF混音音量
#define EQTOOL_MIXVOLUME_USB_AUDIO          1   //USB AUDIO混音音量
#define EQTOOL_MIXVOLUME_I2S                1   //I2S的混音音量
#define EQTOOL_MIXVOLUME_LOCAL_MIC          1   //本地MIC音量
#define EQTOOL_MIXVOLUME_WIRELESS_MIC       1   //无线MIC的音量
#define EQTOOL_MIXVOLUME_TONE               1   //提示音的音量
#define EQTOOL_MIXVOLUME_AUX                1   //AUX混音到输出的音量

//模式的默认音量
#define EQTOOL_MODE_G24_PLAY_VOLUME         1   //第一次启动时默认的G24播放音量
//#define EQTOOL_MODE_BT_PLAY_VOLUME          0   //第一次启动时默认的蓝牙播放音量
//#define EQTOOL_MODE_USB_PLAY_VOLUME         0   //第一次启动时默认的蓝牙播放音量

//rf的发射功率
#define EQTOOL_RF_POWER_VAL                 1   //RF发射功率

#if I2S0_EN == 0
#undef EQTOOL_MIXVOLUME_I2S
#define EQTOOL_MIXVOLUME_I2S 0
#endif

#if SPDIF_RX_EN == 0
#undef EQTOOL_MIXVOLUME_SPDIF
#define EQTOOL_MIXVOLUME_SPDIF 0
#endif

//下面自动生成, 不要改动
#define EQ_TOOL_FUNC0   AUDIO_PLAYBACK_EQ | (AUDIO_PLAYBACK_DRC << 1) | (AUDIO_PLAYBACK_VSS << 3) | (AUDIO_PLAYBACK_VOLCUT << 7) |  \
    (AUDIO_MIC_EQ << 8) | (AUDIO_MIC_DRC << 9) | (AUDIO_MIC_ECHO << 10) | (AUDIO_MIC_REVERB << 11) |   \
    (AUDIO_MIC_TSPS << 12) | (AUDIO_MIC_AENC << 13) | (AUDIO_MIC_NSBWE_AENC << 14) | (AUDIO_MIC_FREQ_SHIFT << 15)   |      \
    (AUDIO_MIC_VOCALENHANCE << 16)
    
#define EQ_TOOL_FUNC1  CPU_LOAD_EN | (EQTOOL_AUDIO_PGA_GAIN_EN << 1) |  \
     (AUDIO_REC_INPUT_SEL << 4) | (AUDIO_LINEIN_MODE_INPUT_SEL << 8)

#define EQ_TOOL_FUNC2   EQTOOL_MIXVOLUME_SPDIF | (EQTOOL_MIXVOLUME_USB_AUDIO << 1) | (EQTOOL_MIXVOLUME_I2S << 2) | (EQTOOL_MIXVOLUME_LOCAL_MIC << 3) \
    | (EQTOOL_MIXVOLUME_WIRELESS_MIC << 4) | (EQTOOL_MIXVOLUME_TONE << 5) | (EQTOOL_MIXVOLUME_AUX << 6)

#define  EQ_TOOL_FUNC3  EQTOOL_MODE_G24_PLAY_VOLUME
#define  EQ_TOOL_FUNC4  EQTOOL_RF_POWER_VAL

#define  EQ_TOOL_FUNC5  0//undefine
#define  EQ_TOOL_FUNC6  0
#define  EQ_TOOL_FUNC7  0
#endif


