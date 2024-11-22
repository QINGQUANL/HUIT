#ifndef _LCD_DISPLAY_H_
#define _LCD_DISPLAY_H_

#include "ble.h"

typedef enum {
    MAIN_MENU,     // 主菜单
    BLUETOOTH_MENU, // 蓝牙菜单
    MENU_3         // 菜单3
} MenuSelection;

typedef enum {
    SIGNAL_NONE,   // 无信号
    SIGNAL_ONE_BAR, // 一格信号
    SIGNAL_TWO_BARS, // 两格信号
    SIGNAL_THREE_BARS, // 三格信号
    SIGNAL_FULL // 四格信号
} SignalStrength;

typedef enum {
    BLUETOOTH_OFF, // 蓝牙关
    BLUETOOTH_ON   // 蓝牙开
} BluetoothState;

typedef enum {
    NOISE_CANCELLATION_OFF, // 降噪关
    NOISE_CANCELLATION_ON   // 降噪开
} NoiseCancellationState;

typedef enum {
    BATTERY_ONE_BAR,  // 一格电池
    BATTERY_TWO_BARS, // 两格电池
    BATTERY_THREE_BARS, // 三格电池
    BATTERY_FULL      // 四格电池
} BatteryLevel;

typedef enum {
    VOLUME_0 = 0,  // 音量0
    VOLUME_1,  // 音量1
    VOLUME_2,  // 音量2
    VOLUME_3,  // 音量3
    VOLUME_4,  // 音量4
    VOLUME_5,  // 音量5
    VOLUME_6,  // 音量6
    VOLUME_7,  // 音量7
    VOLUME_8,  // 音量8
    VOLUME_9,  // 音量9
    VOLUME_10,  // 音量10
    VOLUME_BLACK //无显示
} VolumeLevel;


typedef enum {
    BLUETOOTH_DISPLAY_LOCATION_1, // 蓝牙显示位置1
    BLUETOOTH_DISPLAY_LOCATION_2, // 蓝牙显示位置2
    BLUETOOTH_DISPLAY_LOCATION_3,  // 蓝牙显示位置3
    BLUETOOTH_DISPLAY_LOCATION_4  // 蓝牙显示位置3
} BluetoothDisplayLocation;

typedef enum {
    Unselected,    // 未选择
    Connecting,   // 连接中
    Connected,    // 连接成功
    ConnectionFailed // 连接失败
} Blueuistaus;

typedef struct{
    uint8_t Ble_name_index;
    bool Select_Location_flag; //蓝色框选中标志
    bool ble_updata;
    uint8_t blue_staus;
    // Blueuistaus blue_ui_staus;
} Ble_index;


typedef struct{
    char *Ble_name;//当前数组存储的名字
    uint8_t Ble_name_index; //当前名字在数组中的位置
} Bleinfo;

// 定义包含所有内容的结构体
typedef struct {
    MenuSelection menuSelection; // 菜单选择
    SignalStrength signalStrength; // 信号强度
    NoiseCancellationState noiseCancellationState; // 降噪状态
    BatteryLevel batteryLevel; // 电池电量
    uint8_t signalChannel; // 信号通道
    VolumeLevel volumeLevel;// 音量等级

    BluetoothState bluetoothState; // 蓝牙开关
    uint8_t bluetoothDeviceCount;         // 蓝牙设备数量
    char *ble_device_name_ptr[MAX_DEVICES];            //指向存储蓝牙名字的指针
    Blueuistaus ble_device_link_status[MAX_DEVICES];          //蓝牙连接状态
    BluetoothDisplayLocation   bluetoothDisplayLocation;    //蓝牙显示位置
    Ble_index ble_index[4];            //蓝牙显示四个位置的名字以及是否选择     
} SystemStatus;

extern SystemStatus systemStatus;




void lcd_display_init(void);

void lcd_display_volume(void);//音量显示


void lcd_disp_cb(void* xTimer);

bool get_bluetooth_device_name(Bleinfo * bleinfo);

void BLE_Connect_set(Blueuistaus blueuistaus,uint8_t index);


#endif
