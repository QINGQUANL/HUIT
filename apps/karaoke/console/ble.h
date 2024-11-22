#ifndef _BLE_H_
#define _BLE_H_


// 蓝牙模块命令
#define CMD_LIST "listEND"
#define CMD_START "startEND"
#define CMD_LINK "linkEND"


#define UART2_BLE_TIME      500                         //interval time, unit:ms


// 蓝牙设备列表的最大长度和每个设备名称的最大长度
#define MAX_DEVICES 20
#define MAX_NAME_LENGTH 64


#define PIN_BLE_SWITCH   PIN_B6
#define UART2_BLE_INIT()    {HAL_gpio_config(PIN_BLE_SWITCH | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);} //蓝牙开关引脚

#define UART2_BLE_ON()    {HAL_gpio_value_set(PIN_BLE_SWITCH,1); HAL_gpio_func_set(PIN_BLE_SWITCH,PIN_FUNCOUTPUT);}
#define UART2_BLE_OFF()    {HAL_gpio_value_set(PIN_BLE_SWITCH,0); HAL_gpio_func_set(PIN_BLE_SWITCH,PIN_FUNCOUTPUT);}


void uart2_ble_init(void);
void uart2_ble_cb(void* xTimer);

void get_ble_list(void);
bool connect_to_ble(const char* device_name);
bool get_ble_connection_status();




#endif
