#define LOG_LEV 4
#include "app_lcdc.h"
#include "hal_lcdc.h"
#include "hal_timestamp.h"

#if (LCDC_EN && (LCD_CHIP_SEL == LCD_CHIP_ST7789V2_IG03))

void st7789v2_lcd_init(void)
{
    logi("st7789v2_lcd_init");

    HAL_lcdc_module_write_reg(0x36);
    HAL_lcdc_module_write_data(0x00,1);

    HAL_lcdc_module_write_reg(0x3A);
#if (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB666)
    HAL_lcdc_module_write_data(0x06,1);
#elif (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB565)
    HAL_lcdc_module_write_data(0x05,1);
#endif

    HAL_lcdc_module_write_reg(0xB2);
    HAL_lcdc_module_write_data(0x0B,1);
    HAL_lcdc_module_write_data(0x0B,1);
    HAL_lcdc_module_write_data(0x00,1);
    HAL_lcdc_module_write_data(0x33,1);
    HAL_lcdc_module_write_data(0x33,1);

    HAL_lcdc_module_write_reg(0xB7);
    HAL_lcdc_module_write_data(0x11,1);

    HAL_lcdc_module_write_reg(0xBB);
    HAL_lcdc_module_write_data(0x35,1);

    HAL_lcdc_module_write_reg(0xC0);
    HAL_lcdc_module_write_data(0x2C,1);

    HAL_lcdc_module_write_reg(0xC2);
    HAL_lcdc_module_write_data(0x01,1);

    HAL_lcdc_module_write_reg(0xC3);
    HAL_lcdc_module_write_data(0x0D,1);

    HAL_lcdc_module_write_reg(0xC4);
    HAL_lcdc_module_write_data(0x20,1);//VDV, 0x20:0v

    HAL_lcdc_module_write_reg(0xC6);
    HAL_lcdc_module_write_data(0x13,1);//0x13:60Hz   

    HAL_lcdc_module_write_reg(0xD0);
    HAL_lcdc_module_write_data(0xA4,1);
    HAL_lcdc_module_write_data(0xA1,1);

    HAL_lcdc_module_write_reg(0xD6);
    HAL_lcdc_module_write_data(0xA1,1);//sleep in后，gate输出为GND

    HAL_lcdc_module_write_reg(0xE0);
    HAL_lcdc_module_write_data(0xF0,1);
    HAL_lcdc_module_write_data(0x04,1);
    HAL_lcdc_module_write_data(0x0A,1);
    HAL_lcdc_module_write_data(0x0A,1);
    HAL_lcdc_module_write_data(0x08,1);
    HAL_lcdc_module_write_data(0x25,1);
    HAL_lcdc_module_write_data(0x27,1);
    HAL_lcdc_module_write_data(0x33,1);
    HAL_lcdc_module_write_data(0x3D,1);
    HAL_lcdc_module_write_data(0x38,1);
    HAL_lcdc_module_write_data(0x14,1);
    HAL_lcdc_module_write_data(0x14,1);
    HAL_lcdc_module_write_data(0x25,1);
    HAL_lcdc_module_write_data(0x2A,1);

    HAL_lcdc_module_write_reg(0xE1);
    HAL_lcdc_module_write_data(0xF0,1);
    HAL_lcdc_module_write_data(0x05,1);
    HAL_lcdc_module_write_data(0x08,1);
    HAL_lcdc_module_write_data(0x07,1);
    HAL_lcdc_module_write_data(0x06,1);
    HAL_lcdc_module_write_data(0x02,1);
    HAL_lcdc_module_write_data(0x26,1);
    HAL_lcdc_module_write_data(0x32,1);
    HAL_lcdc_module_write_data(0x3D,1);
    HAL_lcdc_module_write_data(0x3A,1);
    HAL_lcdc_module_write_data(0x16,1);
    HAL_lcdc_module_write_data(0x16,1);
    HAL_lcdc_module_write_data(0x26,1);
    HAL_lcdc_module_write_data(0x2C,1);

    HAL_lcdc_module_write_reg(0xE4);
    HAL_lcdc_module_write_data(0x25,1);
    HAL_lcdc_module_write_data(0x00,1);
    HAL_lcdc_module_write_data(0x00,1);//当gate没有用完时，bit4(TMG)设为0

    HAL_lcdc_module_write_reg(0x21);

    HAL_lcdc_module_write_reg(0x11);

    HAL_mdelay(120); 

    HAL_lcdc_module_write_reg(0x29);  
}

uint32_t st7789v2_read_id(void)
{
    uint32_t id = 0;
    HAL_lcdc_module_read(0xD3,1,(uint8_t*)&id,4);
    logi("RD ID = 0x%x",id);
#if LCDC_INTERFACE
    id = (id&0Xff0000)>>16 |(id&0Xff00);        
#else
    id = (id&0Xff0000)>>8 |(id&0Xff000000)>>24; 
#endif
    logi("D3 ID = 0x%x",id);
    return id;
}

#endif

