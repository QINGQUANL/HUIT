#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "karaoke_framework.h"
#include "disp.h"

#if LCD_SEG_EN
    static uint16_t seg_com0 = 0;
    static uint16_t seg_com1 = 0;
    static uint16_t seg_com2 = 0;
    static uint16_t seg_com3 = 0;


void disp_lcd_seg_init(void)
{
#if DISP_EN
    LCD_COM0_INIT();
    LCD_COM1_INIT();
    LCD_COM2_INIT();
    LCD_COM3_INIT();
    LCD_SEG0_INIT();
    LCD_SEG1_INIT();
    LCD_SEG2_INIT();
    LCD_SEG3_INIT();
    LCD_SEG4_INIT();
    LCD_SEG5_INIT();
    LCD_SEG6_INIT();
    LCD_SEG7_INIT();
    LCD_SEG8_INIT();
    LCD_SEG9_INIT();
    LCD_SEG10_INIT();
    displayUpdate(Battery_Empty,true);//电量
    displayUpdate(Lock_Status,false);//锁
    displayUpdate(CH_Status,true);
    displayUpdate(SET_Status,true);
    displayUpdate(GROP_Status,true);
    displayUpdate(Signal_FourBars,true);//信号
    disp_lcd_channel_update(00);//频道
    // disp_lcd_volume_update(0);//音量
#endif
}


AT_SRAM(.disp)
void disp_lcd_seg_config(uint16_t seg_mask, uint8_t com_level, uint8_t com_num) {
    // 设置 COM 口的显示级别
    if (com_level == COM_LEVEL_H) {
        switch (com_num) {
            case COM0:
                LCD_COM0_H();
                LCD_COM1_M();
                LCD_COM2_M();
                LCD_COM3_M();
                break;
            case COM1:
                LCD_COM0_M();
                LCD_COM1_H();
                LCD_COM2_M();
                LCD_COM3_M();
                break;
            case COM2:
                LCD_COM0_M();
                LCD_COM1_M();
                LCD_COM2_H();
                LCD_COM3_M();
                break;
            case COM3:
                LCD_COM0_M();
                LCD_COM1_M();
                LCD_COM2_M();
                LCD_COM3_H();
                break;
            default:
                break;
        }
    } else if (com_level == COM_LEVEL_L) {
        switch (com_num) {
            case COM0:
                LCD_COM0_L();
                LCD_COM1_M();
                LCD_COM2_M();
                LCD_COM3_M();
                break;
            case COM1:
                LCD_COM0_M();
                LCD_COM1_L();
                LCD_COM2_M();
                LCD_COM3_M();
                break;
            case COM2:
                LCD_COM0_M();
                LCD_COM1_M();
                LCD_COM2_L();
                LCD_COM3_M();
                break;
            case COM3:
                LCD_COM0_M();
                LCD_COM1_M();
                LCD_COM2_M();
                LCD_COM3_L();
                break;
            default:
                break;
        }
    }


    if (seg_mask & (1 << 0)) { // 检查是否需要点亮 SEG0
        if (com_level == COM_LEVEL_H) {
            LCD_SEG0_L();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG0_H();
        }
    }
    else if(!(seg_mask & (1 << 0)))
    {
        if (com_level == COM_LEVEL_H) {
            LCD_SEG0_H();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG0_L();
        }
    }


    if (seg_mask & (1 << 1)) { // 检查是否需要点亮 SEG1
        if (com_level == COM_LEVEL_H) {
            LCD_SEG1_L();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG1_H();
        }
    }
    else if(!(seg_mask & (1 << 1)))
    {
        if (com_level == COM_LEVEL_H) {
            LCD_SEG1_H();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG1_L();
        }
    }



    if (seg_mask & (1 << 2)) { // 检查是否需要点亮 SEG2
        if (com_level == COM_LEVEL_H) {
            LCD_SEG2_L();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG2_H();
        }
    }
    else if(!(seg_mask & (1 << 2)))
    {
        if (com_level == COM_LEVEL_H) {
            LCD_SEG2_H();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG2_L();
        }
    }


    if (seg_mask & (1 << 3)) { // 检查是否需要点亮 SEG3
        if (com_level == COM_LEVEL_H) {
            LCD_SEG3_L();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG3_H();
        }
    }
    else if(!(seg_mask & (1 << 3)))
    {
        if (com_level == COM_LEVEL_H) {
            LCD_SEG3_H();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG3_L();
        }
    }

    if (seg_mask & (1 << 4)) { // 检查是否需要点亮 SEG4
        if (com_level == COM_LEVEL_H) {
            LCD_SEG4_L();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG4_H();
        }
    }
    else if(!(seg_mask & (1 << 4)))
    {
        if (com_level == COM_LEVEL_H) {
            LCD_SEG4_H();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG4_L();
        }
    }


    if (seg_mask & (1 << 5)) { // 检查是否需要点亮 SEG5
        if (com_level == COM_LEVEL_H) {
            LCD_SEG5_L();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG5_H();
        }
    }
    else if(!(seg_mask & (1 << 5)))
    {
        if (com_level == COM_LEVEL_H) {
            LCD_SEG5_H();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG5_L();
        }
    }


    if (seg_mask & (1 << 6)) { // 检查是否需要点亮 SEG6
        if (com_level == COM_LEVEL_H) {
            LCD_SEG6_L();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG6_H();
        }
    }
    else if(!(seg_mask & (1 << 6)))
    {
        if (com_level == COM_LEVEL_H) {
            LCD_SEG6_H();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG6_L();
        }
    }


    if (seg_mask & (1 << 7)) { // 检查是否需要点亮 SEG7
        if (com_level == COM_LEVEL_H) {
            LCD_SEG7_L();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG7_H();
        }
    }
    else if(!(seg_mask & (1 << 7)))
    {
        if (com_level == COM_LEVEL_H) {
            LCD_SEG7_H();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG7_L();
        }
    }



    if (seg_mask & (1 << 8)) { // 检查是否需要点亮 SEG8
        if (com_level == COM_LEVEL_H) {
            LCD_SEG8_L();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG8_H();
        }
    }
    else if(!(seg_mask & (1 << 8)))
    {
        if (com_level == COM_LEVEL_H) {
            LCD_SEG8_H();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG8_L();
        }
    }


    if (seg_mask & (1 << 9)) { // 检查是否需要点亮 SEG9
        if (com_level == COM_LEVEL_H) {
            LCD_SEG9_L();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG9_H();
        }
    }
    else if(!(seg_mask & (1 << 9)))
    {
        if (com_level == COM_LEVEL_H) {
            LCD_SEG9_H();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG9_L();
        }
    }



    if (seg_mask & (1 << 10)) { // 检查是否需要点亮 SEG10
        if (com_level == COM_LEVEL_H) {
            LCD_SEG10_L();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG10_H();
        }
    }
    else if(!(seg_mask & (1 << 10)))
    {
        if (com_level == COM_LEVEL_H) {
            LCD_SEG10_H();
        } else if (com_level == COM_LEVEL_L) {
            LCD_SEG10_L();
        }
    }
}

void display_lcd_segment(uint16_t seg_com, uint8_t com_level, uint8_t com_numb) {
    if(com_numb == COM0){
            disp_lcd_seg_config(seg_com, COM_LEVEL_H, COM0);
            HAL_mdelay(5);
            disp_lcd_seg_config(seg_com, COM_LEVEL_L, COM0);
            HAL_mdelay(3);
    }
    else if(com_numb == COM1){
            disp_lcd_seg_config(seg_com, COM_LEVEL_H, COM1);
            HAL_mdelay(5);
            disp_lcd_seg_config(seg_com, COM_LEVEL_L, COM1);
            HAL_mdelay(3);
    }
    else if(com_numb == COM2){
            disp_lcd_seg_config(seg_com, COM_LEVEL_H, COM2);
            HAL_mdelay(5);
            disp_lcd_seg_config(seg_com, COM_LEVEL_L, COM2);
            HAL_mdelay(3);
    }
    else if(com_numb == COM3){
            disp_lcd_seg_config(seg_com, COM_LEVEL_H, COM3);
            HAL_mdelay(5);
            disp_lcd_seg_config(seg_com, COM_LEVEL_L, COM3);
            HAL_mdelay(3);
    }
}

// 更新屏幕显示内容
// @param Update_disp: 更新的内容
// @param status: 状态 1为显示 0为清除显示 只有部分有清除功能
// @return None
//
void displayUpdate(DisplayStatusEnum Update_disp, uint8_t status) {
    switch (Update_disp) {
        // 电量状态
        case Battery_Empty://空电量
            seg_com0 |= SEG_MASK_2; //电量外框
            seg_com1 &= ~SEG_MASK_0; //三格电量显示
            seg_com2 &= ~SEG_MASK_0; //一格电量显示
            seg_com3 &= ~SEG_MASK_2; //两格电量显示
            break;
        case Battery_OneBar:
            seg_com0 |= SEG_MASK_2; //电量外框
            seg_com1 &= ~SEG_MASK_0; //三格电量显示
            seg_com2 |= SEG_MASK_0; //一格电量显示
            seg_com3 &= ~SEG_MASK_2; //两格电量显示
            break;
        case Battery_TwoBars:
            seg_com0 |= SEG_MASK_2; //电量外框
            seg_com1 &= ~SEG_MASK_0; //三格电量显示
            seg_com2 |= SEG_MASK_0; //一格电量显示
            seg_com3 |= SEG_MASK_2; //两格电量显示
            break;
        case Battery_ThreeBars:
            seg_com0 |= SEG_MASK_2; //电量外框
            seg_com1 |= SEG_MASK_2; //三格电量显示
            seg_com2 |= SEG_MASK_2; //一格电量显示
            seg_com3 |= SEG_MASK_2; //两格电量显示
            break;

        // 锁状态
        case Lock_Status:
            if(status == true)
            {
                seg_com2 |= SEG_MASK_1; //锁显示
            }else{
                seg_com2 &= ~SEG_MASK_1; //锁清除
            }
            break;

        // 状态
        case SET_Status:
            if(status == true)
            {
                seg_com1 |= SEG_MASK_1; //SET显示
            }else{
                seg_com1 &= ~SEG_MASK_1; //SET清除
            }
            break;
        case GROP_Status:
            if(status == true)
            {
                seg_com3 |= SEG_MASK_3; //GROP显示
            }else{
                seg_com3 &= ~SEG_MASK_3; //GROP清除
            }
            break;
        case CH_Status:
            if(status == true)
            {
                seg_com3 |= SEG_MASK_5; //CH显示
            }else{
                seg_com3 &= ~SEG_MASK_5; //CH清除
            }
            break;

        // 信号状态
        case Signal_ZeroBar:
            seg_com3 |= SEG_MASK_0; //信号图标显示
            seg_com2 &= ~SEG_MASK_0; //一格信号显示
            seg_com1 &= ~SEG_MASK_0; //两格信号显示
            seg_com0 &= ~SEG_MASK_0; //三格信号显示
            seg_com0 &= ~SEG_MASK_1; //四格信号显示
            break;

        case Signal_OneBar:
            seg_com3 |= SEG_MASK_0; //信号图标显示
            seg_com2 |= SEG_MASK_0; //一格信号显示
            seg_com1 &= ~SEG_MASK_0; //两格信号显示
            seg_com0 &= ~SEG_MASK_0; //三格信号显示
            seg_com0 &= ~SEG_MASK_1; //四格信号显示
            break;
        case Signal_TwoBars:
            seg_com3 |= SEG_MASK_0; //信号图标显示
            seg_com2 |= SEG_MASK_0; //一格信号显示
            seg_com1 |= SEG_MASK_0; //两格信号显示
            seg_com0 &= ~SEG_MASK_0; //三格信号显示
            seg_com0 &= ~SEG_MASK_1; //四格信号显示
            break;
        case Signal_ThreeBars:
            seg_com3 |= SEG_MASK_0; //信号图标显示
            seg_com2 |= SEG_MASK_0; //一格信号显示
            seg_com1 |= SEG_MASK_0; //两格信号显示
            seg_com0 |= SEG_MASK_0; //三格信号显示
            seg_com0 &= ~SEG_MASK_1; //四格信号显示
            break;
        case Signal_FourBars:
            seg_com3 |= SEG_MASK_0; //信号图标显示
            seg_com2 |= SEG_MASK_0; //一格信号显示
            seg_com1 |= SEG_MASK_0; //两格信号显示
            seg_com0 |= SEG_MASK_0; //三格信号显示
            seg_com0 |= SEG_MASK_1; //四格信号显示
            break;

        // 数码管1显示
        case Digit1_Display0:
            seg_com3 |= SEG_MASK_4; //1号8位数码管1显示
            seg_com2 |= SEG_MASK_4; //1号8位数码管2显示
            seg_com0 |= SEG_MASK_4; //1号8位数码管3显示
            seg_com0 |= SEG_MASK_3; //1号8位数码管4显示
            seg_com1 |= SEG_MASK_3; //1号8位数码管5显示
            seg_com2 |= SEG_MASK_3; //1号8位数码管6显示
            seg_com1 &= ~SEG_MASK_4; //1号8位数码管7显示
            break;
        case Digit1_Display1:
            seg_com3 &= ~SEG_MASK_4; //1号8位数码管1显示
            seg_com2 |= SEG_MASK_4; //1号8位数码管2显示
            seg_com0 |= SEG_MASK_4; //1号8位数码管3显示
            seg_com0 &= ~SEG_MASK_3; //1号8位数码管4显示
            seg_com1 &= ~SEG_MASK_3; //1号8位数码管5显示
            seg_com2 &= ~SEG_MASK_3; //1号8位数码管6显示
            seg_com1 &= ~SEG_MASK_4; //1号8位数码管7显示
            break;
        case Digit1_Display2:
            seg_com3 |= SEG_MASK_4; //1号8位数码管1显示
            seg_com2 |= SEG_MASK_4; //1号8位数码管2显示
            seg_com0 &= ~SEG_MASK_4; //1号8位数码管3显示
            seg_com0 |= SEG_MASK_3; //1号8位数码管4显示
            seg_com1 |= SEG_MASK_3; //1号8位数码管5显示
            seg_com2 &= ~SEG_MASK_3; //1号8位数码管6显示
            seg_com1 |= SEG_MASK_4; //1号8位数码管7显示
            break;
        case Digit1_Display3:
            seg_com3 |= SEG_MASK_4; //1号8位数码管1显示
            seg_com2 |= SEG_MASK_4; //1号8位数码管2显示
            seg_com0 |= SEG_MASK_4; //1号8位数码管3显示
            seg_com0 |= SEG_MASK_3; //1号8位数码管4显示
            seg_com1 &= ~SEG_MASK_3; //1号8位数码管5显示
            seg_com2 &= ~SEG_MASK_3; //1号8位数码管6显示
            seg_com1 |= SEG_MASK_4; //1号8位数码管7显示
            break;
        case Digit1_Display4:
            seg_com3 &= ~SEG_MASK_4; //1号8位数码管1显示
            seg_com2 |= SEG_MASK_4; //1号8位数码管2显示
            seg_com0 |= SEG_MASK_4; //1号8位数码管3显示
            seg_com0 &= ~SEG_MASK_3; //1号8位数码管4显示
            seg_com1 &= ~SEG_MASK_3; //1号8位数码管5显示
            seg_com2 |= SEG_MASK_3; //1号8位数码管6显示
            seg_com1 |= SEG_MASK_4; //1号8位数码管7显示
            break;
        case Digit1_Display5:
            seg_com3 |= SEG_MASK_4; //1号8位数码管1显示
            seg_com2 &= ~SEG_MASK_4; //1号8位数码管2显示
            seg_com0 |= SEG_MASK_4; //1号8位数码管3显示
            seg_com0 |= SEG_MASK_3; //1号8位数码管4显示
            seg_com1 &= ~SEG_MASK_3; //1号8位数码管5显示
            seg_com2 |= SEG_MASK_3; //1号8位数码管6显示
            seg_com1 |= SEG_MASK_4; //1号8位数码管7显示
            break;
        case Digit1_Display6:
            seg_com3 |= SEG_MASK_4; //1号8位数码管1显示
            seg_com2 &= ~SEG_MASK_4; //1号8位数码管2显示
            seg_com0 |= SEG_MASK_4; //1号8位数码管3显示
            seg_com0 |= SEG_MASK_3; //1号8位数码管4显示
            seg_com1 |= SEG_MASK_3; //1号8位数码管5显示
            seg_com2 |= SEG_MASK_3; //1号8位数码管6显示
            seg_com1 |= SEG_MASK_4; //1号8位数码管7显示
            break;
        case Digit1_Display7:
            seg_com3 |= SEG_MASK_4; //1号8位数码管1显示
            seg_com2 |= SEG_MASK_4; //1号8位数码管2显示
            seg_com0 |= SEG_MASK_4; //1号8位数码管3显示
            seg_com0 &= ~SEG_MASK_3; //1号8位数码管4显示
            seg_com1 &= ~SEG_MASK_3; //1号8位数码管5显示
            seg_com2 &= ~SEG_MASK_3; //1号8位数码管6显示
            seg_com1 &= ~SEG_MASK_4; //1号8位数码管7显示
            break;
        case Digit1_Display8:
            seg_com3 |= SEG_MASK_4; //1号8位数码管1显示
            seg_com2 |= SEG_MASK_4; //1号8位数码管2显示
            seg_com0 |= SEG_MASK_4; //1号8位数码管3显示
            seg_com0 |= SEG_MASK_3; //1号8位数码管4显示
            seg_com1 |= SEG_MASK_3; //1号8位数码管5显示
            seg_com2 |= SEG_MASK_3; //1号8位数码管6显示
            seg_com1 |= SEG_MASK_4; //1号8位数码管7显示
            break;
        case Digit1_Display9:
            seg_com3 |= SEG_MASK_4; //1号8位数码管1显示
            seg_com2 |= SEG_MASK_4; //1号8位数码管2显示
            seg_com0 |= SEG_MASK_4; //1号8位数码管3显示
            seg_com0 |= SEG_MASK_3; //1号8位数码管4显示
            seg_com1 &= ~SEG_MASK_3; //1号8位数码管5显示
            seg_com2 |= SEG_MASK_3; //1号8位数码管6显示
            seg_com1 |= SEG_MASK_4; //1号8位数码管7显示
            break;

        // 数码管2显示
        case Digit2_Display0:
            seg_com3 |= SEG_MASK_6; //2号8位数码管1显示
            seg_com2 |= SEG_MASK_6; //2号8位数码管2显示
            seg_com0 |= SEG_MASK_6; //2号8位数码管3显示
            seg_com0 |= SEG_MASK_5; //2号8位数码管4显示
            seg_com1 |= SEG_MASK_5; //2号8位数码管5显示
            seg_com2 |= SEG_MASK_5; //2号8位数码管6显示
            seg_com1 &= ~SEG_MASK_6; //2号8位数码管7显示
            break;
        case Digit2_Display1:
            seg_com3 &= ~SEG_MASK_6; //2号8位数码管1显示
            seg_com2 |= SEG_MASK_6; //2号8位数码管2显示
            seg_com0 |= SEG_MASK_6; //2号8位数码管3显示
            seg_com0 &= ~SEG_MASK_5; //2号8位数码管4显示
            seg_com1 &= ~SEG_MASK_5; //2号8位数码管5显示
            seg_com2 &= ~SEG_MASK_5; //2号8位数码管6显示
            seg_com1 &= ~SEG_MASK_6; //2号8位数码管7显示
            break;
        case Digit2_Display2:
            seg_com3 |= SEG_MASK_6; //2号8位数码管1显示
            seg_com2 |= SEG_MASK_6; //2号8位数码管2显示
            seg_com0 &= ~SEG_MASK_6; //2号8位数码管3显示
            seg_com0 |= SEG_MASK_5; //2号8位数码管4显示
            seg_com1 |= SEG_MASK_5; //2号8位数码管5显示
            seg_com2 &= ~SEG_MASK_5; //2号8位数码管6显示
            seg_com1 |= SEG_MASK_6; //2号8位数码管7显示
            break;
        case Digit2_Display3:
            seg_com3 |= SEG_MASK_6; //2号8位数码管1显示
            seg_com2 |= SEG_MASK_6; //2号8位数码管2显示
            seg_com0 |= SEG_MASK_6; //2号8位数码管3显示
            seg_com0 |= SEG_MASK_5; //2号8位数码管4显示
            seg_com1 &= ~SEG_MASK_5; //2号8位数码管5显示
            seg_com2 &= ~SEG_MASK_5; //2号8位数码管6显示
            seg_com1 |= SEG_MASK_6; //2号8位数码管7显示
            break;
        case Digit2_Display4:
            seg_com3 &= ~SEG_MASK_6; //2号8位数码管1显示
            seg_com2 |= SEG_MASK_6; //2号8位数码管2显示
            seg_com0 |= SEG_MASK_6; //2号8位数码管3显示
            seg_com0 &= ~SEG_MASK_5; //2号8位数码管4显示
            seg_com1 &= ~SEG_MASK_5; //2号8位数码管5显示
            seg_com2 |= SEG_MASK_5; //2号8位数码管6显示
            seg_com1 |= SEG_MASK_6; //2号8位数码管7显示
            break;
        case Digit2_Display5:
            seg_com3 |= SEG_MASK_6; //2号8位数码管1显示
            seg_com2 &= ~SEG_MASK_6; //2号8位数码管2显示
            seg_com0 |= SEG_MASK_6; //2号8位数码管3显示
            seg_com0 |= SEG_MASK_5; //2号8位数码管4显示
            seg_com1 &= ~SEG_MASK_5; //2号8位数码管5显示
            seg_com2 |= SEG_MASK_5; //2号8位数码管6显示
            seg_com1 |= SEG_MASK_6; //2号8位数码管7显示
            break;
        case Digit2_Display6:
            seg_com3 |= SEG_MASK_6; //2号8位数码管1显示
            seg_com2 &= ~SEG_MASK_6; //2号8位数码管2显示
            seg_com0 |= SEG_MASK_6; //2号8位数码管3显示
            seg_com0 |= SEG_MASK_5; //2号8位数码管4显示
            seg_com1 |= SEG_MASK_5; //2号8位数码管5显示
            seg_com2 |= SEG_MASK_5; //2号8位数码管6显示
            seg_com1 |= SEG_MASK_6; //2号8位数码管7显示
            break;
        case Digit2_Display7:
            seg_com3 |= SEG_MASK_6; //2号8位数码管1显示
            seg_com2 |= SEG_MASK_6; //2号8位数码管2显示
            seg_com0 |= SEG_MASK_6; //2号8位数码管3显示
            seg_com0 &= ~SEG_MASK_5; //2号8位数码管4显示
            seg_com1 &= ~SEG_MASK_5; //2号8位数码管5显示
            seg_com2 &= ~SEG_MASK_5; //2号8位数码管6显示
            seg_com1 &= ~SEG_MASK_6; //2号8位数码管7显示
            break;
        case Digit2_Display8:
            seg_com3 |= SEG_MASK_6; //2号8位数码管1显示
            seg_com2 |= SEG_MASK_6; //2号8位数码管2显示
            seg_com0 |= SEG_MASK_6; //2号8位数码管3显示
            seg_com0 |= SEG_MASK_5; //2号8位数码管4显示
            seg_com1 |= SEG_MASK_5; //2号8位数码管5显示
            seg_com2 |= SEG_MASK_5; //2号8位数码管6显示
            seg_com1 |= SEG_MASK_6; //2号8位数码管7显示
            break;
        case Digit2_Display9:
            seg_com3 |= SEG_MASK_6; //2号8位数码管1显示
            seg_com2 |= SEG_MASK_6; //2号8位数码管2显示
            seg_com0 |= SEG_MASK_6; //2号8位数码管3显示
            seg_com0 |= SEG_MASK_5; //2号8位数码管4显示
            seg_com1 &= ~SEG_MASK_5; //2号8位数码管5显示
            seg_com2 |= SEG_MASK_5; //2号8位数码管6显示
            seg_com1 |= SEG_MASK_6; //2号8位数码管7显示
            break;

        // 数码管3显示
        case Digit3_Display0:
            seg_com3 |= SEG_MASK_1; //3号8位数码管1显示
            seg_com2 |= SEG_MASK_10; //3号8位数码管2显示
            seg_com0 |= SEG_MASK_10; //3号8位数码管3显示
            seg_com0 |= SEG_MASK_8; //3号8位数码管4显示
            seg_com1 |= SEG_MASK_7; //3号8位数码管5显示
            seg_com2 |= SEG_MASK_7; //3号8位数码管6显示
            seg_com2 &= ~SEG_MASK_8; //3号8位数码管7显示
            seg_com1 &= ~SEG_MASK_10; //3号8位数码管8显示
            seg_com3 &= ~SEG_MASK_7; //3号8位数码管9显示
            seg_com3 &= ~SEG_MASK_8; //3号8位数码管10显示
            seg_com3 &= ~SEG_MASK_10; //3号8位数码管11显示
            seg_com0 &= ~SEG_MASK_7; //3号8位数码管12显示
            seg_com1 &= ~SEG_MASK_8; //3号8位数码管13显示
            seg_com0 &= ~SEG_MASK_9; //3号8位数码管14显示
            break;
        case Digit3_Display1:
            seg_com3 &= ~SEG_MASK_1; //3号8位数码管1显示
            seg_com2 |= SEG_MASK_10; //3号8位数码管2显示
            seg_com0 |= SEG_MASK_10; //3号8位数码管3显示
            seg_com0 &= ~SEG_MASK_8; //3号8位数码管4显示
            seg_com1 &= ~SEG_MASK_7; //3号8位数码管5显示
            seg_com2 &= ~SEG_MASK_7; //3号8位数码管6显示
            seg_com2 &= ~SEG_MASK_8; //3号8位数码管7显示
            seg_com1 &= ~SEG_MASK_10; //3号8位数码管8显示
            seg_com3 &= ~SEG_MASK_7; //3号8位数码管9显示
            seg_com3 &= ~SEG_MASK_8; //3号8位数码管10显示
            seg_com3 &= ~SEG_MASK_10; //3号8位数码管11显示
            seg_com0 &= ~SEG_MASK_7; //3号8位数码管12显示
            seg_com1 &= ~SEG_MASK_8; //3号8位数码管13显示
            seg_com0 &= ~SEG_MASK_9; //3号8位数码管14显示
            break;
        case Digit3_Display2:
            seg_com3 |= SEG_MASK_1; //3号8位数码管1显示
            seg_com2 |= SEG_MASK_10; //3号8位数码管2显示
            seg_com0 &= ~SEG_MASK_10; //3号8位数码管3显示
            seg_com0 |= SEG_MASK_8; //3号8位数码管4显示
            seg_com1 |= SEG_MASK_7; //3号8位数码管5显示
            seg_com2 &= ~SEG_MASK_7; //3号8位数码管6显示
            seg_com2 |= SEG_MASK_8; //3号8位数码管7显示
            seg_com1 |= SEG_MASK_10; //3号8位数码管8显示
            seg_com3 &= ~SEG_MASK_7; //3号8位数码管9显示
            seg_com3 &= ~SEG_MASK_8; //3号8位数码管10显示
            seg_com3 &= ~SEG_MASK_10; //3号8位数码管11显示
            seg_com0 &= ~SEG_MASK_7; //3号8位数码管12显示
            seg_com1 &= ~SEG_MASK_8; //3号8位数码管13显示
            seg_com0 &= ~SEG_MASK_9; //3号8位数码管14显示
            break;
        case Digit3_Display3:
            seg_com3 |= SEG_MASK_1; //3号8位数码管1显示
            seg_com2 |= SEG_MASK_10; //3号8位数码管2显示
            seg_com0 |= SEG_MASK_10; //3号8位数码管3显示
            seg_com0 |= SEG_MASK_8; //3号8位数码管4显示
            seg_com1 &= ~SEG_MASK_7; //3号8位数码管5显示
            seg_com2 &= ~SEG_MASK_7; //3号8位数码管6显示
            seg_com2 |= SEG_MASK_8; //3号8位数码管7显示
            seg_com1 |= SEG_MASK_10; //3号8位数码管8显示
            seg_com3 &= ~SEG_MASK_7; //3号8位数码管9显示
            seg_com3 &= ~SEG_MASK_8; //3号8位数码管10显示
            seg_com3 &= ~SEG_MASK_10; //3号8位数码管11显示
            seg_com0 &= ~SEG_MASK_7; //3号8位数码管12显示
            seg_com1 &= ~SEG_MASK_8; //3号8位数码管13显示
            seg_com0 &= ~SEG_MASK_9; //3号8位数码管14显示
            break;
        case Digit3_Display4:
            seg_com3 &= ~SEG_MASK_1; //3号8位数码管1显示
            seg_com2 |= SEG_MASK_10; //3号8位数码管2显示
            seg_com0 |= SEG_MASK_10; //3号8位数码管3显示
            seg_com0 &= ~SEG_MASK_8; //3号8位数码管4显示
            seg_com1 &= ~SEG_MASK_7; //3号8位数码管5显示
            seg_com2 |= SEG_MASK_7; //3号8位数码管6显示
            seg_com2 |= SEG_MASK_8; //3号8位数码管7显示
            seg_com1 |= SEG_MASK_10; //3号8位数码管8显示
            seg_com3 &= ~SEG_MASK_7; //3号8位数码管9显示
            seg_com3 &= ~SEG_MASK_8; //3号8位数码管10显示
            seg_com3 &= ~SEG_MASK_10; //3号8位数码管11显示
            seg_com0 &= ~SEG_MASK_7; //3号8位数码管12显示
            seg_com1 &= ~SEG_MASK_8; //3号8位数码管13显示
            seg_com0 &= ~SEG_MASK_9; //3号8位数码管14显示
            break;
        case Digit3_Display5:
            seg_com3 |= SEG_MASK_1; //3号8位数码管1显示
            seg_com2 &= ~SEG_MASK_10; //3号8位数码管2显示
            seg_com0 |= SEG_MASK_10; //3号8位数码管3显示
            seg_com0 |= SEG_MASK_8; //3号8位数码管4显示
            seg_com1 &= ~SEG_MASK_7; //3号8位数码管5显示
            seg_com2 |= SEG_MASK_7; //3号8位数码管6显示
            seg_com2 |= SEG_MASK_8; //3号8位数码管7显示
            seg_com1 |= SEG_MASK_10; //3号8位数码管8显示
            seg_com3 &= ~SEG_MASK_7; //3号8位数码管9显示
            seg_com3 &= ~SEG_MASK_8; //3号8位数码管10显示
            seg_com3 &= ~SEG_MASK_10; //3号8位数码管11显示
            seg_com0 &= ~SEG_MASK_7; //3号8位数码管12显示
            seg_com1 &= ~SEG_MASK_8; //3号8位数码管13显示
            seg_com0 &= ~SEG_MASK_9; //3号8位数码管14显示
            break;
        case Digit3_Display6:
            seg_com3 |= SEG_MASK_1; //3号8位数码管1显示
            seg_com2 &= ~SEG_MASK_10; //3号8位数码管2显示
            seg_com0 |= SEG_MASK_10; //3号8位数码管3显示
            seg_com0 |= SEG_MASK_8; //3号8位数码管4显示
            seg_com1 |= SEG_MASK_7; //3号8位数码管5显示
            seg_com2 |= SEG_MASK_7; //3号8位数码管6显示
            seg_com2 |= SEG_MASK_8; //3号8位数码管7显示
            seg_com1 |= SEG_MASK_10; //3号8位数码管8显示
            seg_com3 &= ~SEG_MASK_7; //3号8位数码管9显示
            seg_com3 &= ~SEG_MASK_8; //3号8位数码管10显示
            seg_com3 &= ~SEG_MASK_10; //3号8位数码管11显示
            seg_com0 &= ~SEG_MASK_7; //3号8位数码管12显示
            seg_com1 &= ~SEG_MASK_8; //3号8位数码管13显示
            seg_com0 &= ~SEG_MASK_9; //3号8位数码管14显示
            break;
        case Digit3_Display7:
            seg_com3 |= SEG_MASK_1; //3号8位数码管1显示
            seg_com2 |= SEG_MASK_10; //3号8位数码管2显示
            seg_com0 |= SEG_MASK_10; //3号8位数码管3显示
            seg_com0 &= ~SEG_MASK_8; //3号8位数码管4显示
            seg_com1 &= ~SEG_MASK_7; //3号8位数码管5显示
            seg_com2 &= ~SEG_MASK_7; //3号8位数码管6显示
            seg_com2 &= ~SEG_MASK_8; //3号8位数码管7显示
            seg_com1 &= ~SEG_MASK_10; //3号8位数码管8显示
            seg_com3 &= ~SEG_MASK_7; //3号8位数码管9显示
            seg_com3 &= ~SEG_MASK_8; //3号8位数码管10显示
            seg_com3 &= ~SEG_MASK_10; //3号8位数码管11显示
            seg_com0 &= ~SEG_MASK_7; //3号8位数码管12显示
            seg_com1 &= ~SEG_MASK_8; //3号8位数码管13显示
            seg_com0 &= ~SEG_MASK_9; //3号8位数码管14显示
            break;
        case Digit3_Display8:
            seg_com3 |= SEG_MASK_1; //3号8位数码管1显示
            seg_com2 |= SEG_MASK_10; //3号8位数码管2显示
            seg_com0 |= SEG_MASK_10; //3号8位数码管3显示
            seg_com0 |= SEG_MASK_8; //3号8位数码管4显示
            seg_com1 |= SEG_MASK_7; //3号8位数码管5显示
            seg_com2 |= SEG_MASK_7; //3号8位数码管6显示
            seg_com2 |= SEG_MASK_8; //3号8位数码管7显示
            seg_com1 |= SEG_MASK_10; //3号8位数码管8显示
            seg_com3 &= ~SEG_MASK_7; //3号8位数码管9显示
            seg_com3 &= ~SEG_MASK_8; //3号8位数码管10显示
            seg_com3 &= ~SEG_MASK_10; //3号8位数码管11显示
            seg_com0 &= ~SEG_MASK_7; //3号8位数码管12显示
            seg_com1 &= ~SEG_MASK_8; //3号8位数码管13显示
            seg_com0 &= ~SEG_MASK_9; //3号8位数码管14显示
            break;
        case Digit3_Display9:
            seg_com3 |= SEG_MASK_1; //3号8位数码管1显示
            seg_com2 |= SEG_MASK_10; //3号8位数码管2显示
            seg_com0 |= SEG_MASK_10; //3号8位数码管3显示
            seg_com0 |= SEG_MASK_8; //3号8位数码管4显示
            seg_com1 &= ~SEG_MASK_7; //3号8位数码管5显示
            seg_com2 |= SEG_MASK_7; //3号8位数码管6显示
            seg_com2 |= SEG_MASK_8; //3号8位数码管7显示
            seg_com1 |= SEG_MASK_10; //3号8位数码管8显示
            seg_com3 &= ~SEG_MASK_7; //3号8位数码管9显示
            seg_com3 &= ~SEG_MASK_8; //3号8位数码管10显示
            seg_com3 &= ~SEG_MASK_10; //3号8位数码管11显示
            seg_com0 &= ~SEG_MASK_7; //3号8位数码管12显示
            seg_com1 &= ~SEG_MASK_8; //3号8位数码管13显示
            seg_com0 &= ~SEG_MASK_9; //3号8位数码管14显示
            break;

        default:

            break;
    }
}

void disp_lcd_channel_update(int num)
{
    int units_digit, tens_digit;

    // 个位数的提取方法：使用取模运算 %
    units_digit = num % 10;

    // 十位数的提取方法：先去掉个位数，再取模
    tens_digit = (num / 10) % 10;

    // 更新数码管显示
    displayUpdate((DisplayStatusEnum)(Digit3_Display0 + units_digit), true);    // 数码管1显示个位数
    displayUpdate((DisplayStatusEnum)(Digit2_Display0 + tens_digit), true);     // 数码管2显示十位数
}

void disp_lcd_volume_update(int num)
{
    displayUpdate((DisplayStatusEnum)(Digit1_Display0 + num), true); // 数码管3显示百位数
}




AT_SRAM(.disp)
void disp_lcd_seg_update(void)
{

}


AT_SRAM(.disp)
void disp_lcd_seg(void)
{
#if DISP_EN
    display_lcd_segment(seg_com0, COM_LEVEL_H, COM0);
    display_lcd_segment(seg_com1, COM_LEVEL_H, COM1);
    display_lcd_segment(seg_com2, COM_LEVEL_H, COM2);
    display_lcd_segment(seg_com3, COM_LEVEL_H, COM3);
#endif
}
#endif


