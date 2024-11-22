#ifndef _LCD_SEG_H_
#define _LCD_SEG_H_


#ifdef __cplusplus
 extern "C" {
#endif

#include "hal_gpio.h"


enum lcd_com_e {
    COM0 = 0,
    COM1,
    COM2,
    COM3,
    COM4,
};

enum com_level_e {
    COM_LEVEL_H = 0,
    COM_LEVEL_L,
};


typedef enum {
    // 电量状态
    Battery_Empty = 0,      // 电量空
    Battery_OneBar,         // 电量一格
    Battery_TwoBars,        // 电量两格
    Battery_ThreeBars,      // 电量三格

    // 锁状态
    Lock_Status,            // 锁状态

    // 按钮
    SET_Status,             // 设置状态
    GROP_Status,            // 组状态
    CH_Status,              // 通道状态

    // 信号状态
    Signal_ZeroBar,         // 信号零格
    Signal_OneBar,          // 信号一格
    Signal_TwoBars,         // 信号两格
    Signal_ThreeBars,       // 信号三格
    Signal_FourBars,        // 信号四格

    // 数码管显示
    Digit1_Display0,        // 数码管1显示0
    Digit1_Display1,        // 数码管1显示1
    Digit1_Display2,        // 数码管1显示2
    Digit1_Display3,        // 数码管1显示3
    Digit1_Display4,        // 数码管1显示4
    Digit1_Display5,        // 数码管1显示5
    Digit1_Display6,        // 数码管1显示6
    Digit1_Display7,        // 数码管1显示7
    Digit1_Display8,        // 数码管1显示8
    Digit1_Display9,        // 数码管1显示9

    Digit2_Display0,        // 数码管2显示0
    Digit2_Display1,        // 数码管2显示1
    Digit2_Display2,        // 数码管2显示2
    Digit2_Display3,        // 数码管2显示3
    Digit2_Display4,        // 数码管2显示4
    Digit2_Display5,        // 数码管2显示5
    Digit2_Display6,        // 数码管2显示6
    Digit2_Display7,        // 数码管2显示7
    Digit2_Display8,        // 数码管2显示8
    Digit2_Display9,        // 数码管2显示9

    Digit3_Display0,        // 数码管3显示0
    Digit3_Display1,        // 数码管3显示1
    Digit3_Display2,        // 数码管3显示2
    Digit3_Display3,        // 数码管3显示3
    Digit3_Display4,        // 数码管3显示4
    Digit3_Display5,        // 数码管3显示5
    Digit3_Display6,        // 数码管3显示6
    Digit3_Display7,        // 数码管3显示7
    Digit3_Display8,        // 数码管3显示8
    Digit3_Display9         // 数码管3显示9
} DisplayStatusEnum;



/*
    C0--C1--C2--C3--C4--S0--S1--S2--S3--S4--S5--S6--S7
    B5--B6--B7--B8--D0--B11-B12-B13-B0--B1--B2--B4--B3
*/
#define PIN_COM0        PIN_D6
#define PIN_COM1        PIN_D7
#define PIN_COM2        PIN_D8
#define PIN_COM3        PIN_D9
#define PIN_SEG0        PIN_D1
#define PIN_SEG1        PIN_D2
#define PIN_SEG2        PIN_D3
#define PIN_SEG3        PIN_B6
#define PIN_SEG4        PIN_B5
#define PIN_SEG5        PIN_B4
#define PIN_SEG6        PIN_B3
#define PIN_SEG7        PIN_B2
#define PIN_SEG8        PIN_B1
#define PIN_SEG9        PIN_B0
#define PIN_SEG10       PIN_D10

#define SEG_MASK_0      (1<<0)
#define SEG_MASK_1      (1<<1)
#define SEG_MASK_2      (1<<2)
#define SEG_MASK_3      (1<<3)
#define SEG_MASK_4      (1<<4)
#define SEG_MASK_5      (1<<5)
#define SEG_MASK_6      (1<<6)
#define SEG_MASK_7      (1<<7)
#define SEG_MASK_8      (1<<8)
#define SEG_MASK_9      (1<<9)
#define SEG_MASK_10     (1<<10)



#define LCD_COM0_INIT() {HAL_gpio_config(PIN_COM0 | PIN_DRI3 | PIN_FUNCINPUT | PIN_PULL_UP2K_DOWN2K);}
#define LCD_COM1_INIT() {HAL_gpio_config(PIN_COM1 | PIN_DRI3 | PIN_FUNCINPUT | PIN_PULL_UP2K_DOWN2K);}
#define LCD_COM2_INIT() {HAL_gpio_config(PIN_COM2 | PIN_DRI3 | PIN_FUNCINPUT | PIN_PULL_UP2K_DOWN2K);}
#define LCD_COM3_INIT() {HAL_gpio_config(PIN_COM3 | PIN_DRI3 | PIN_FUNCINPUT | PIN_PULL_UP2K_DOWN2K);}
#define LCD_SEG0_INIT() {HAL_gpio_config(PIN_SEG0 | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);}
#define LCD_SEG1_INIT() {HAL_gpio_config(PIN_SEG1 | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);}
#define LCD_SEG2_INIT() {HAL_gpio_config(PIN_SEG2 | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);}
#define LCD_SEG3_INIT() {HAL_gpio_config(PIN_SEG3 | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);}
#define LCD_SEG4_INIT() {HAL_gpio_config(PIN_SEG4 | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);}
#define LCD_SEG5_INIT() {HAL_gpio_config(PIN_SEG5 | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);}
#define LCD_SEG6_INIT() {HAL_gpio_config(PIN_SEG6 | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);}
#define LCD_SEG7_INIT() {HAL_gpio_config(PIN_SEG7 | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);}
#define LCD_SEG8_INIT() {HAL_gpio_config(PIN_SEG8 | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);}
#define LCD_SEG9_INIT() {HAL_gpio_config(PIN_SEG9 | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);}
#define LCD_SEG10_INIT() {HAL_gpio_config(PIN_SEG10 | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);}

#define LCD_COM0_H()    {HAL_gpio_value_set(PIN_COM0,1); HAL_gpio_func_set(PIN_COM0,PIN_FUNCOUTPUT);}
#define LCD_COM0_L()    {HAL_gpio_value_set(PIN_COM0,0);}
#define LCD_COM0_M()    {HAL_gpio_func_set(PIN_COM0,PIN_FUNCINPUT);}

#define LCD_COM1_H()    {HAL_gpio_value_set(PIN_COM1,1); HAL_gpio_func_set(PIN_COM1,PIN_FUNCOUTPUT);}
#define LCD_COM1_L()    {HAL_gpio_value_set(PIN_COM1,0);}
#define LCD_COM1_M()    {HAL_gpio_func_set(PIN_COM1,PIN_FUNCINPUT);}

#define LCD_COM2_H()    {HAL_gpio_value_set(PIN_COM2,1); HAL_gpio_func_set(PIN_COM2,PIN_FUNCOUTPUT);}
#define LCD_COM2_L()    {HAL_gpio_value_set(PIN_COM2,0);}
#define LCD_COM2_M()    {HAL_gpio_func_set(PIN_COM2,PIN_FUNCINPUT);}

#define LCD_COM3_H()    {HAL_gpio_value_set(PIN_COM3,1); HAL_gpio_func_set(PIN_COM3,PIN_FUNCOUTPUT);}
#define LCD_COM3_L()    {HAL_gpio_value_set(PIN_COM3,0);}
#define LCD_COM3_M()    {HAL_gpio_func_set(PIN_COM3,PIN_FUNCINPUT);}

#define LCD_SEG0_H()    {HAL_gpio_value_set(PIN_SEG0,1); HAL_gpio_func_set(PIN_SEG0,PIN_FUNCOUTPUT);}
#define LCD_SEG0_L()    {HAL_gpio_value_set(PIN_SEG0,0); HAL_gpio_func_set(PIN_SEG0,PIN_FUNCOUTPUT);}
#define LCD_SEG0_I()    {HAL_gpio_func_set(PIN_SEG0,PIN_FUNCINPUT);}
#define LCD_SEG0_O()    {HAL_gpio_func_set(PIN_SEG0,PIN_FUNCOUTPUT);}
#define LCD_SEG0_D()    {HAL_gpio_func_set(PIN_SEG0,PIN_FUNC15);}

#define LCD_SEG1_H()    {HAL_gpio_value_set(PIN_SEG1,1); HAL_gpio_func_set(PIN_SEG1,PIN_FUNCOUTPUT);}
#define LCD_SEG1_L()    {HAL_gpio_value_set(PIN_SEG1,0); HAL_gpio_func_set(PIN_SEG1,PIN_FUNCOUTPUT);}
#define LCD_SEG1_I()    {HAL_gpio_func_set(PIN_SEG1,PIN_FUNCINPUT);}
#define LCD_SEG1_O()    {HAL_gpio_func_set(PIN_SEG1,PIN_FUNCOUTPUT);}
#define LCD_SEG1_D()    {HAL_gpio_func_set(PIN_SEG1,PIN_FUNC15);}

#define LCD_SEG2_H()    {HAL_gpio_value_set(PIN_SEG2,1); HAL_gpio_func_set(PIN_SEG2,PIN_FUNCOUTPUT);}
#define LCD_SEG2_L()    {HAL_gpio_value_set(PIN_SEG2,0); HAL_gpio_func_set(PIN_SEG2,PIN_FUNCOUTPUT);}
#define LCD_SEG2_I()    {HAL_gpio_func_set(PIN_SEG2,PIN_FUNCINPUT);}
#define LCD_SEG2_O()    {HAL_gpio_func_set(PIN_SEG2,PIN_FUNCOUTPUT);}
#define LCD_SEG2_D()    {HAL_gpio_func_set(PIN_SEG2,PIN_FUNC15);}

#define LCD_SEG3_H()    {HAL_gpio_value_set(PIN_SEG3,1); HAL_gpio_func_set(PIN_SEG3,PIN_FUNCOUTPUT);}
#define LCD_SEG3_L()    {HAL_gpio_value_set(PIN_SEG3,0); HAL_gpio_func_set(PIN_SEG3,PIN_FUNCOUTPUT);}
#define LCD_SEG3_I()    {HAL_gpio_func_set(PIN_SEG3,PIN_FUNCINPUT);}
#define LCD_SEG3_O()    {HAL_gpio_func_set(PIN_SEG3,PIN_FUNCOUTPUT);}
#define LCD_SEG3_D()    {HAL_gpio_func_set(PIN_SEG3,PIN_FUNC15);}

#define LCD_SEG4_H()    {HAL_gpio_value_set(PIN_SEG4,1); HAL_gpio_func_set(PIN_SEG4,PIN_FUNCOUTPUT);}
#define LCD_SEG4_L()    {HAL_gpio_value_set(PIN_SEG4,0); HAL_gpio_func_set(PIN_SEG4,PIN_FUNCOUTPUT);}
#define LCD_SEG4_I()    {HAL_gpio_func_set(PIN_SEG4,PIN_FUNCINPUT);}
#define LCD_SEG4_O()    {HAL_gpio_func_set(PIN_SEG4,PIN_FUNCOUTPUT);}
#define LCD_SEG4_D()    {HAL_gpio_func_set(PIN_SEG4,PIN_FUNC15);}

#define LCD_SEG5_H()    {HAL_gpio_value_set(PIN_SEG5,1); HAL_gpio_func_set(PIN_SEG5,PIN_FUNCOUTPUT);}
#define LCD_SEG5_L()    {HAL_gpio_value_set(PIN_SEG5,0); HAL_gpio_func_set(PIN_SEG5,PIN_FUNCOUTPUT);}
#define LCD_SEG5_I()    {HAL_gpio_func_set(PIN_SEG5,PIN_FUNCINPUT);}
#define LCD_SEG5_O()    {HAL_gpio_func_set(PIN_SEG5,PIN_FUNCOUTPUT);}
#define LCD_SEG5_D()    {HAL_gpio_func_set(PIN_SEG5,PIN_FUNC15);}

#define LCD_SEG6_H()    {HAL_gpio_value_set(PIN_SEG6,1); HAL_gpio_func_set(PIN_SEG6,PIN_FUNCOUTPUT);}
#define LCD_SEG6_L()    {HAL_gpio_value_set(PIN_SEG6,0); HAL_gpio_func_set(PIN_SEG6,PIN_FUNCOUTPUT);}
#define LCD_SEG6_I()    {HAL_gpio_func_set(PIN_SEG6,PIN_FUNCINPUT);}
#define LCD_SEG6_O()    {HAL_gpio_func_set(PIN_SEG6,PIN_FUNCOUTPUT);}
#define LCD_SEG6_D()    {HAL_gpio_func_set(PIN_SEG6,PIN_FUNC15);}

#define LCD_SEG7_H()    {HAL_gpio_value_set(PIN_SEG7,1); HAL_gpio_func_set(PIN_SEG7,PIN_FUNCOUTPUT);}
#define LCD_SEG7_L()    {HAL_gpio_value_set(PIN_SEG7,0); HAL_gpio_func_set(PIN_SEG7,PIN_FUNCOUTPUT);}
#define LCD_SEG7_I()    {HAL_gpio_func_set(PIN_SEG7,PIN_FUNCINPUT);}
#define LCD_SEG7_O()    {HAL_gpio_func_set(PIN_SEG7,PIN_FUNCOUTPUT);}
#define LCD_SEG7_D()    {HAL_gpio_func_set(PIN_SEG7,PIN_FUNC15);}

#define LCD_SEG8_H()    {HAL_gpio_value_set(PIN_SEG8,1); HAL_gpio_func_set(PIN_SEG8,PIN_FUNCOUTPUT);}
#define LCD_SEG8_L()    {HAL_gpio_value_set(PIN_SEG8,0); HAL_gpio_func_set(PIN_SEG8,PIN_FUNCOUTPUT);}
#define LCD_SEG8_I()    {HAL_gpio_func_set(PIN_SEG8,PIN_FUNCINPUT);}
#define LCD_SEG8_O()    {HAL_gpio_func_set(PIN_SEG8,PIN_FUNCOUTPUT);}
#define LCD_SEG8_D()    {HAL_gpio_func_set(PIN_SEG8,PIN_FUNC15);}

#define LCD_SEG9_H()    {HAL_gpio_value_set(PIN_SEG9,1); HAL_gpio_func_set(PIN_SEG9,PIN_FUNCOUTPUT);}
#define LCD_SEG9_L()    {HAL_gpio_value_set(PIN_SEG9,0); HAL_gpio_func_set(PIN_SEG9,PIN_FUNCOUTPUT);}
#define LCD_SEG9_I()    {HAL_gpio_func_set(PIN_SEG9,PIN_FUNCINPUT);}
#define LCD_SEG9_O()    {HAL_gpio_func_set(PIN_SEG9,PIN_FUNCOUTPUT);}
#define LCD_SEG9_D()    {HAL_gpio_func_set(PIN_SEG9,PIN_FUNC15);}

#define LCD_SEG10_H()    {HAL_gpio_value_set(PIN_SEG10,1); HAL_gpio_func_set(PIN_SEG10,PIN_FUNCOUTPUT);}
#define LCD_SEG10_L()    {HAL_gpio_value_set(PIN_SEG10,0); HAL_gpio_func_set(PIN_SEG10,PIN_FUNCOUTPUT);}
#define LCD_SEG10_I()    {HAL_gpio_func_set(PIN_SEG10,PIN_FUNCINPUT);}
#define LCD_SEG10_O()    {HAL_gpio_func_set(PIN_SEG10,PIN_FUNCOUTPUT);}
#define LCD_SEG10_D()    {HAL_gpio_func_set(PIN_SEG10,PIN_FUNC15);}

/************************************************************************************************************/





/************************************************************************************************************/

void disp_lcd_seg_init(void);
void disp_lcd_seg_config(uint16_t seg_mask, uint8_t com_level, uint8_t com_num);
void disp_lcd_seg_update(void);
void disp_lcd_seg(void);
void displayUpdate(DisplayStatusEnum Update_disp, uint8_t status);
void disp_lcd_channel_update(int num);
void disp_lcd_volume_update(int num);

#ifdef __cplusplus
}
#endif


#endif

