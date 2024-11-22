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
#include "hal_log.h"
#include "lcd_display.h"
#include "app_lcd_font.h"


// OAL_MUTEX OAL_mutex_create(void);
// OAL_RESULT OAL_mutex_destroy(OAL_MUTEX *mutex);
// OAL_RESULT OAL_mutex_lock(OAL_MUTEX *mutex);
// void OAL_mutex_unlock(OAL_MUTEX *mutex);


SystemStatus systemStatus;

static bool MENU_flag = 0;

extern void g24_Channel_select(uint8_t channel);
extern uint8_t am_record_volume_get(void);



#define LCD_TIME                    500                         //interval time, unit:ms
#define VOLUME_CLEANR_TIMER         5                         //更新完音量后延时关闭显示
OAL_TIMER LCDTimer;


/**
 * 初始化函数
 */
void lcd_display_init(void)
{
    /*disp*/
    systemStatus.menuSelection = MAIN_MENU;
    systemStatus.batteryLevel = BATTERY_FULL;
    systemStatus.noiseCancellationState = NOISE_CANCELLATION_ON;
    systemStatus.signalChannel = 0;
    systemStatus.signalStrength = SIGNAL_NONE;
    systemStatus.volumeLevel = VOLUME_BLACK;

    systemStatus.bluetoothState = BLUETOOTH_OFF;
    systemStatus.bluetoothDeviceCount = 0;
    systemStatus.ble_device_name_ptr[0] = NULL;

    logi("lcd_display_init");
    OAL_timer_create(&LCDTimer, "lcd_timer", OAL_ms_to_ticks(LCD_TIME),lcd_disp_cb, (void *)1, 1, OAL_TIMER_AUTO_ACTIVATE);
}

void lcd_bluemenu_init(void)
{
    systemStatus.ble_index[0].Select_Location_flag = 1;
    systemStatus.ble_index[1].Select_Location_flag = 0;
    systemStatus.ble_index[2].Select_Location_flag = 0;
    systemStatus.ble_index[3].Select_Location_flag = 0;
    systemStatus.ble_index[0].Ble_name_index = 0;
    systemStatus.ble_index[1].Ble_name_index = 1;
    systemStatus.ble_index[2].Ble_name_index = 2;
    systemStatus.ble_index[3].Ble_name_index = 3;
    systemStatus.ble_index[0].ble_updata = 0;
    systemStatus.ble_index[1].ble_updata = 0;
    systemStatus.ble_index[2].ble_updata = 0;
    systemStatus.ble_index[3].ble_updata = 0;
}

/**
 * 音量显示函数
 * 该函数用于在LCD屏幕上显示当前音量等级。
 * 音量等级从0到10，每个等级对应一个不同的图标。
 *
 * @param volume 要显示的音量等级，范围从0到10。
 */
void lcd_display_volume()
{

    static uint8_t old_volume;//老数据存储
    static bool Updata_flag,clear_display;//更新标志位和清平标志位
    static uint16_t clear_volume_timer = 0; //倒计时计数

    uint16_t v_x,v_y,v_w,v_h;// 
    v_x = 10;
    v_y = 60;
    v_w = 15;
    v_h = 160;

    systemStatus.volumeLevel = (VolumeLevel)am_record_volume_get()/10;

    if(old_volume != systemStatus.volumeLevel)//判断有无更新
    {
        old_volume = systemStatus.volumeLevel;
        Updata_flag = 1;//标志位用以判断是否执行更新操作
        clear_volume_timer = 0;//清除音量标志位
        clear_display = 0;//清除音量标志位
    }
    else
    {
        if(clear_volume_timer < VOLUME_CLEANR_TIMER && clear_display == 0 && Updata_flag == 0)
        {
            OAL_sleep(1);
            clear_volume_timer++;//增加计数
        }
        else if(clear_volume_timer >= VOLUME_CLEANR_TIMER && clear_display == 0 && Updata_flag == 0)
        {
            clear_volume_timer = 0;
            clear_display = 1;//打开清除标志位
        }
    }

    if((clear_display == 0 && Updata_flag) || MENU_flag == 1)
    {
        switch (systemStatus.volumeLevel)
        {
            case VOLUME_0:
                lcd_show_picture(v_x,v_y,v_w,v_h,(uint8_t *)gImage_yinlian0,1);
                break;

            case VOLUME_1:
                lcd_show_picture(v_x,v_y,v_w,v_h,(uint8_t *)gImage_yinlian1,1);
                break;

            case VOLUME_2:
                lcd_show_picture(v_x,v_y,v_w,v_h,(uint8_t *)gImage_yinlian2,1);
                break;

            case VOLUME_3:
                lcd_show_picture(v_x,v_y,v_w,v_h,(uint8_t *)gImage_yinlian3,1);
                break;

            case VOLUME_4:
                lcd_show_picture(v_x,v_y,v_w,v_h,(uint8_t *)gImage_yinlian4,1);
                break;

            case VOLUME_5:
                lcd_show_picture(v_x,v_y,v_w,v_h,(uint8_t *)gImage_yinlian5,1);
                break;

            case VOLUME_6:
                lcd_show_picture(v_x,v_y,v_w,v_h,(uint8_t *)gImage_yinlian6,1);
                break;

            case VOLUME_7:
                lcd_show_picture(v_x,v_y,v_w,v_h,(uint8_t *)gImage_yinlian7,1);
                break;

            case VOLUME_8:
                lcd_show_picture(v_x,v_y,v_w,v_h,(uint8_t *)gImage_yinlian8,1);
                break;

            case VOLUME_9:
                lcd_show_picture(v_x,v_y,v_w,v_h,(uint8_t *)gImage_yinlian9,1);
                break;

            case VOLUME_10:
                lcd_show_picture(v_x,v_y,v_w,v_h,(uint8_t *)gImage_yinlian10,1);
                break;

            default:
                lcd_show_picture(v_x,v_y,v_w,v_h,(uint8_t *)gImage_yinlianwu,1);
                break;
        }
        Updata_flag = 0;//更新结束
    }
    else if(clear_display && Updata_flag == 0)
    {
        lcd_show_picture(v_x,v_y,v_w,v_h,(uint8_t *)gImage_black,1);
        clear_display = 0;
    }
}
/*信号显示*/
void lcd_display_signal(void)
{
    static SignalStrength old_signal;
    static bool signal_updata_flag = 1;
    if(old_signal != systemStatus.signalStrength)
    {
        old_signal = systemStatus.signalStrength;
        signal_updata_flag = 1;
    }

    if(signal_updata_flag == 1 || MENU_flag == 1)
    {
        signal_updata_flag = 0;
        switch(systemStatus.signalStrength)
        {
            case SIGNAL_NONE:
                #if G24_FUNC_ROLE
                    lcd_ground_color_set(BLUE,BLACK);
                    lcd_show_picture(8,0,47,47,(uint8_t *)gImage_jizhan,1);
                #else 
                    lcd_ground_color_set(GREEN,BLACK);
                    lcd_show_picture(0,0,60,47,(uint8_t *)gImage_duankai,1);
                #endif
                break;
            
            case SIGNAL_ONE_BAR:
                    lcd_show_picture(0,0,60,47,(uint8_t *)gImage_xinhao1,1);
                break;
            
            case SIGNAL_TWO_BARS:
                    lcd_show_picture(0,0,60,47,(uint8_t *)gImage_xinhao2,1);
                break;

            case SIGNAL_THREE_BARS:
                    lcd_show_picture(0,0,60,47,(uint8_t *)gImage_xinhao3,1);
                break;

            case SIGNAL_FULL:
                    lcd_show_picture(0,0,60,47,(uint8_t *)gImage_xinhao4,1);
                break;
        }
    }
}

/*蓝牙图标*/
void lcd_display_ble(void)
{
    static BluetoothState old_ble;
    static bool ble_updata_flag = 1;
    if(old_ble != systemStatus.bluetoothState)
    {
        old_ble = systemStatus.bluetoothState;
        ble_updata_flag = 1;
    }

    if(ble_updata_flag == 1 || MENU_flag == 1)
    {
        ble_updata_flag = 0;
        switch(systemStatus.bluetoothState)
        {
            case BLUETOOTH_OFF:
                UART2_BLE_ON();
                lcd_show_picture(70,0,50,50,(uint8_t *)gImage_lanyaguan,1);
                break;
            
            case BLUETOOTH_ON:
                UART2_BLE_OFF();
                lcd_show_picture(70,0,50,50,(uint8_t *)gImage_lanya,1);
                break;
        }
    }
}

/*降噪图标*/
void lcd_display_noise(void)
{
    static NoiseCancellationState old_noise;
    static bool noise_updata_flag = 1;
    if(old_noise != systemStatus.noiseCancellationState)
    {
        old_noise = systemStatus.noiseCancellationState;
        noise_updata_flag = 1;
    }

    if(noise_updata_flag == 1 || MENU_flag == 1)
    {
        noise_updata_flag = 0;
        switch(systemStatus.noiseCancellationState)
        {
            case NOISE_CANCELLATION_OFF:
                lcd_show_picture(130,0,50,50,(uint8_t *)gImage_kaijiangzao,1);
                break;
            
            case NOISE_CANCELLATION_ON:
                lcd_show_picture(130,0,50,50,(uint8_t *)gImage_kaijiangzao,1);
                break;
        }
    }
}

/*电池显示*/
void lcd_display_battery(void)
{
    static BatteryLevel old_battery;
    static bool battery_updata_flag = 1;
    if(old_battery != systemStatus.batteryLevel)
    {
        old_battery = systemStatus.batteryLevel;
        battery_updata_flag = 1;
    }

    if(battery_updata_flag == 1 || MENU_flag == 1)
    {
        battery_updata_flag = 0;
        switch(systemStatus.batteryLevel)
        {
            case BATTERY_ONE_BAR:
                lcd_show_picture(190,0,50,50,(uint8_t *)gImage_didian,1);
                break;
            
            case BATTERY_TWO_BARS:
                lcd_show_picture(190,0,50,50,(uint8_t *)gImage_bandian,1);
                break;

            case BATTERY_THREE_BARS:
                lcd_show_picture(190,0,50,50,(uint8_t *)gImage_dianchi,1);
                break;

            case BATTERY_FULL:
                lcd_show_picture(190,0,50,50,(uint8_t *)gImage_mandian,1); 
                break;
        }
    }
}


/*蓝牙列表*/

char test_buff[10][50] = {"AAAAA","BBBBB","CCCCC","DDDDD","EEEEE","FFFFF","GGGGG"};
#define TEST_MAX    7

void lcd_list_display(void)
{
    // systemStatus.bluetoothDeviceCount = TEST_MAX;

    static bool ble_old_flag[4] = {0};

    for(int i = 0;i < 4;i++)
    {
        if(ble_old_flag[i] != systemStatus.ble_index[i].Select_Location_flag)
        {
            ble_old_flag[i] = systemStatus.ble_index[i].Select_Location_flag;
            systemStatus.ble_index[i].ble_updata = 1;
        }
    }

    // for(int i = 0;i < systemStatus.bluetoothDeviceCount;i++)
    // {
    //     systemStatus.ble_device_name_ptr[i] = test_buff[i];
    // }
//列表显示 

    for(int i = 0;i < 4;i++)
    {
        if(systemStatus.ble_index[i].ble_updata)
        {
            if(systemStatus.ble_index[i].Select_Location_flag)
            {
                lcd_show_picture(5,3+(i*60),230,53,(uint8_t *)gImage_BleSelect,1); 
            }
            else
            {
                lcd_show_picture(5,3+(i*60),230,53,(uint8_t *)gImage_BleNoselect,1); 
            }
            for(int j = 0;j < 4;j++)
            {
                if(systemStatus.ble_index[j].Select_Location_flag)
                {
                    lcd_ground_color_set(WHITE,BLACK);
                    lcd_show_string(50, 22+(j*60), 240, 30, 16, (uint8_t *)systemStatus.ble_device_name_ptr[systemStatus.ble_index[j].Ble_name_index], 1);
                    switch(systemStatus.ble_device_link_status[systemStatus.ble_index[j].Ble_name_index])
                    {
                        case Unselected:
                            lcd_show_string(180, 24+(j*60), 240, 30, 12, (uint8_t *)" ", 1);
                            break;

                        case Connecting:
                            lcd_show_string(180, 24+(j*60), 240, 30, 12, (uint8_t *)"...", 1);
                            break;

                        case Connected:
                            lcd_ground_color_set(GREEN,BLACK);
                            lcd_show_string(180, 24+(j*60), 240, 30, 12, (uint8_t *)"OK", 1);
                            break;

                        case ConnectionFailed:
                            lcd_ground_color_set(RED,BLACK);
                            lcd_show_string(180, 24+(j*60), 240, 30, 12, (uint8_t *)"ERROR", 1);
                            break;
                    }
                }
                else
                {
                    lcd_ground_color_set(BLACK,BLACK);
                    lcd_show_string(50, 22+(j*60), 240, 30, 16, (uint8_t *)systemStatus.ble_device_name_ptr[systemStatus.ble_index[j].Ble_name_index], 1);
                    switch(systemStatus.ble_device_link_status[systemStatus.ble_index[j].Ble_name_index])
                    {
                        case Unselected:
                            lcd_show_string(180, 24+(j*60), 240, 30, 12, (uint8_t *)" ", 1);
                            break;

                        case Connecting:
                            lcd_show_string(180, 24+(j*60), 240, 30, 12, (uint8_t *)"...", 1);
                            break;

                        case Connected:
                            lcd_ground_color_set(GREEN,BLACK);
                            lcd_show_string(180, 24+(j*60), 240, 30, 12, (uint8_t *)"OK", 1);
                            break;

                        case ConnectionFailed:
                            lcd_ground_color_set(RED,BLACK);
                            lcd_show_string(180, 24+(j*60), 240, 30, 12, (uint8_t *)"ERROR", 1);
                            break;
                    }
                }
            }
            systemStatus.ble_index[i].ble_updata = 0; 
        }
    }
}

//获取当前的蓝牙名字
bool get_bluetooth_device_name(Bleinfo * bleinfo) {
    
    // 检查索引是否有效
    for(int i = 0;i <= 4;i++)
    {
        if(systemStatus.ble_index[i].Select_Location_flag)
        {
            bleinfo->Ble_name = (char *)systemStatus.ble_device_name_ptr[systemStatus.ble_index[i].Ble_name_index];
            bleinfo->Ble_name_index = systemStatus.ble_index[i].Ble_name_index;
            return 1;
        }
    }
    return 0;
}

//设置当前蓝牙连接显示状态
void BLE_Connect_set(Blueuistaus blueuistaus,uint8_t index)
{
    for(int i = 0;i < 4;i++)
    {
        if(systemStatus.ble_index[i].Select_Location_flag)
        {
            // uart2_printf("index = %d\n",index);
            systemStatus.ble_device_link_status[index] = blueuistaus;
            systemStatus.ble_index[i].ble_updata = 1;
        }
    }
}

void lcd_disp_cb(void* xTimer)
{
    while(1)
    {
        if(systemStatus.menuSelection == MAIN_MENU)
        {
            /*首次进入先清屏*/
            if(MENU_flag == 1)
            {
                systemStatus.ble_device_name_ptr[0] = NULL;
                lcd_clear(BLACK);
            }
                

            /*信号显示*/
            lcd_display_signal();
            /*蓝牙图标*/
            lcd_display_ble();
            /*降噪图标*/
            lcd_display_noise();
            /*电池显示*/
            lcd_display_battery();
            /*音量显示*/
            lcd_display_volume();
            /*通道显示*/
            g24_Channel_select(systemStatus.signalChannel);

            MENU_flag = 0;
        }
        else if(systemStatus.menuSelection == BLUETOOTH_MENU)
        {
            /*首次进入先清屏*/
            if(MENU_flag == 0)
            {
                lcd_bluemenu_init();

                lcd_clear(WHITE);
                lcd_show_picture(5,3,230,53,(uint8_t *)gImage_BleSelect,1);
                lcd_show_picture(5,63,230,53,(uint8_t *)gImage_BleNoselect,1);
                lcd_show_picture(5,123,230,53,(uint8_t *)gImage_BleNoselect,1);
                lcd_show_picture(5,183,230,53,(uint8_t *)gImage_BleNoselect,1);
            }
            if(systemStatus.ble_device_name_ptr[0] != NULL)
            {
                lcd_list_display();
            }
            MENU_flag = 1;
        }
    }
}


