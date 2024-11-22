/*
1.接收的原始数据需要在末尾带上 END 用来结束
2.发送命令  (获取蓝牙列表)  <1>MCU发送list  <2>等待蓝牙模块回应列表长度  例如:5(目前检索到了五个蓝牙)  <3>MCU发送start <4>蓝牙模块开始发送列表内容 单次发送一个蓝牙名称 MCU单次接收完成回应OK至蓝牙模块
            (连接蓝牙)      <1>MCU发送link  <2>等待蓝牙模块回应 link <3>发送蓝牙名字 <4>等待蓝牙模块连接并回应 OK or ERROR  
            (蓝牙连接状态)  <1>MCU发送get  <2>等待蓝牙模块回应OK or ERROR

*/
#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "console.h"
#include "oal.h"
#include "ring_buffer.h"
#include "hal_dma.h"
#include "hal_uart.h"
#include "hal_gpio.h"
#include "stdarg.h"
#include "stdio.h"
#include "ble.h"
#include "app_lcdc.h"
#include "lcd_display.h"



// 蓝牙设备列表
char ble_devices[MAX_DEVICES][MAX_NAME_LENGTH]; // 存储蓝牙设备名称的数组

OAL_TIMER UART2BLETimer;


void uart2_ble_init(void)
{
    UART2_BLE_INIT();

    OAL_timer_create(&UART2BLETimer, "ble_timer", OAL_ms_to_ticks(UART2_BLE_TIME),uart2_ble_cb, (void *)1, 1, OAL_TIMER_AUTO_ACTIVATE);
    
}

//UART2带阻塞属性的读取数据
char* uart2_ble_read()
{
    char* data = NULL;
    while(!data)
    {
        OAL_sleep(100);
        data = uart2_read();
    }
    return data;
}

// 发送命令到蓝牙模块
void send_command(const char* cmd) {
    uart2_printf("%s\n", cmd);
}

// 获取蓝牙设备列表
void get_ble_list(void) {
    send_command(CMD_LIST); // 发送list命令
    OAL_sleep(10);
    char* response = uart2_ble_read(); // 等待蓝牙模块回应列表长度
    // uart2_printf("%s\n",response);
    if (response != NULL && strlen(response) > 0) {
        systemStatus.bluetoothDeviceCount = atoi(response); // 使用atoi将字符串转换为整数
        memset(ble_devices, 0, sizeof(ble_devices)); // 清空设备列表数组
        send_command(CMD_START); // 发送start命令
        OAL_sleep(10);
        for (int i = 0; i < systemStatus.bluetoothDeviceCount && i < MAX_DEVICES; ++i) { // 根据长度循环获取，但不超过数组最大长度
            char* device_name = uart2_ble_read(); // 接收蓝牙名称
            OAL_sleep(10);
            if (device_name != NULL && strlen(device_name) > 0) {
                strncpy(ble_devices[i], device_name, strlen(device_name)); // 存储设备名称到数组
                ble_devices[i][strlen(device_name)] = '\0'; // 确保字符串以空字符结尾
                uart2_printf("OKEND\n"); // 回应OK至蓝牙模块
            }
            systemStatus.ble_device_name_ptr[i] = ble_devices[i];
        }

        // for (int i = 0; i < MAX_DEVICES; i++) {//系统存储
        //     systemStatus.ble_device_name_ptr[i] = ble_devices[i];
        // }

        for (int i = 0; i < MAX_DEVICES && strlen(ble_devices[i]) > 0; ++i) {
            // uart2_printf("Bluetooth device %d: %s\n", i + 1, ble_devices[i]);
        }
    } else {
        // 错误处理
        uart2_printf("Failed to get BLE list length.\n");
    }
}

// 连接到蓝牙设备
bool connect_to_ble(const char* device_name) {
    send_command(CMD_LINK); // 发送link命令
    char* response = uart2_ble_read(); // 等待蓝牙模块回应
    if (strcmp(response, "link") == 0) {
        OAL_sleep(10);
        uart2_printf("%s\n", device_name); // 发送蓝牙名字
        response = uart2_ble_read(); // 等待蓝牙模块连接并回应
        OAL_sleep(10);
        send_command(response);
        return strcmp(response, "OK") == 0; // 返回连接结果
    } else {
        return false; // 返回连接失败
    }
}

// 获取蓝牙连接状态
bool get_ble_connection_status() {
    send_command("getEND"); // 发送get命令
    OAL_sleep(10);
    char* response = uart2_ble_read(); // 等待蓝牙模块回应
    OAL_sleep(10);
    return strcmp(response, "OK") == 0; // 返回连接状态
}

void uart2_ble_cb(void* xTimer)
{
    while(1)
    {
        OAL_sleep(1000);
    }
}

