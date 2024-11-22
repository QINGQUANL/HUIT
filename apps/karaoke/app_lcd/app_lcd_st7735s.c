#define LOG_LEV 4
#include "app_lcdc.h"
#include "hal_lcdc.h"
#include "hal_timestamp.h"

#if (LCDC_EN && (LCD_CHIP_SEL == LCD_CHIP_ST7735S))

void st7735s_lcd_init(void)
{
    logi("st7735s_lcd_init");
    HAL_lcdc_module_write_reg(0x01);
    HAL_mdelay(120);
    HAL_lcdc_module_write_reg(0x11);
    HAL_mdelay(240);
    HAL_lcdc_module_write_reg(0xb1);
    HAL_lcdc_module_write_data(0x05,1);
    HAL_lcdc_module_write_data(0x3c,1);
    HAL_lcdc_module_write_data(0x3c,1);
    HAL_lcdc_module_write_reg(0xb4);
    HAL_lcdc_module_write_data(0x02,1);
    HAL_lcdc_module_write_reg(0xb4);
    HAL_lcdc_module_write_data(0x03,1);

    HAL_lcdc_module_write_reg(0xc0);
    HAL_lcdc_module_write_data(0x28,1);
    HAL_lcdc_module_write_data(0x08,1);
    HAL_lcdc_module_write_data(0x84,1);
    HAL_lcdc_module_write_reg(0xc1);
    HAL_lcdc_module_write_data(0xc0,1);
    HAL_lcdc_module_write_reg(0xc2);
    HAL_lcdc_module_write_data(0x0d,1);
    HAL_lcdc_module_write_data(0x00,1);
    HAL_lcdc_module_write_reg(0xc3);
    HAL_lcdc_module_write_data(0x8d,1);
    HAL_lcdc_module_write_data(0x2a,1);
    HAL_lcdc_module_write_reg(0xc4);
    HAL_lcdc_module_write_data(0x8d,1);
    HAL_lcdc_module_write_data(0xee,1);
    HAL_lcdc_module_write_reg(0xc5);
    HAL_lcdc_module_write_data(0x16,1);
    //--------------------------------------ST7735R Gamma Sequence-----------------------------------//
    HAL_lcdc_module_write_reg(0xe0);
    HAL_lcdc_module_write_data(0x04,1);
    HAL_lcdc_module_write_data(0x22,1);
    HAL_lcdc_module_write_data(0x07,1);
    HAL_lcdc_module_write_data(0x0a,1);
    HAL_lcdc_module_write_data(0x2e,1);
    HAL_lcdc_module_write_data(0x30,1);
    HAL_lcdc_module_write_data(0x25,1);
    HAL_lcdc_module_write_data(0x2a,1);
    HAL_lcdc_module_write_data(0x28,1);
    HAL_lcdc_module_write_data(0x26,1);
    HAL_lcdc_module_write_data(0x2e,1);
    HAL_lcdc_module_write_data(0x3a,1);
    HAL_lcdc_module_write_data(0x00,1);
    HAL_lcdc_module_write_data(0x01,1);
    HAL_lcdc_module_write_data(0x03,1);
    HAL_lcdc_module_write_data(0x13,1);

    HAL_lcdc_module_write_reg(0xe1);
    HAL_lcdc_module_write_data(0x04,1);
    HAL_lcdc_module_write_data(0x16,1);
    HAL_lcdc_module_write_data(0x06,1);
    HAL_lcdc_module_write_data(0x0d,1);
    HAL_lcdc_module_write_data(0x2d,1);
    HAL_lcdc_module_write_data(0x26,1);
    HAL_lcdc_module_write_data(0x23,1);
    HAL_lcdc_module_write_data(0x27,1);
    HAL_lcdc_module_write_data(0x27,1);
    HAL_lcdc_module_write_data(0x25,1);
    HAL_lcdc_module_write_data(0x2d,1);
    HAL_lcdc_module_write_data(0x3b,1);
    HAL_lcdc_module_write_data(0x00,1);
    HAL_lcdc_module_write_data(0x01,1);
    HAL_lcdc_module_write_data(0x04,1);
    HAL_lcdc_module_write_data(0x13,1);
    //--------------------------------------ST7735R Gamma Sequence-----------------------------------//
    HAL_lcdc_module_write_reg(0x3a);
    HAL_lcdc_module_write_data(0x05,1);
    HAL_lcdc_module_write_reg(0x29);
    HAL_lcdc_module_write_reg(0x36);
    HAL_lcdc_module_write_data(0xc8,1);
    HAL_lcdc_module_write_reg(0x2a);
    HAL_lcdc_module_write_data(0x00,1);
    HAL_lcdc_module_write_data(0x00,1);
    HAL_lcdc_module_write_data(0x00,1);
    HAL_lcdc_module_write_data(0x7f,1);
    HAL_lcdc_module_write_reg(0x2b);
    HAL_lcdc_module_write_data(0x00,1);
    HAL_lcdc_module_write_data(0x00,1);
    HAL_lcdc_module_write_data(0x00,1);
    HAL_lcdc_module_write_data(0x9f,1);
    HAL_lcdc_module_write_reg(0x2c);
}

void st7735s_spi_lcd_init(void)
{
    HAL_lcdc_module_write_reg(0x11);     //Sleep out

    HAL_mdelay(200);              //Delay 120ms

    HAL_lcdc_module_write_reg(0xB1);     
    HAL_lcdc_module_write_data(0x05, 1);   
    HAL_lcdc_module_write_data(0x3C, 1);   
    HAL_lcdc_module_write_data(0x3C, 1);   

    HAL_lcdc_module_write_reg(0xB2);     
    HAL_lcdc_module_write_data(0x05, 1);   
    HAL_lcdc_module_write_data(0x3C, 1);   
    HAL_lcdc_module_write_data(0x3C, 1);   

    HAL_lcdc_module_write_reg(0xB3);     
    HAL_lcdc_module_write_data(0x05, 1);   
    HAL_lcdc_module_write_data(0x3C, 1);   
    HAL_lcdc_module_write_data(0x3C, 1);   
    HAL_lcdc_module_write_data(0x05, 1);   
    HAL_lcdc_module_write_data(0x3C, 1);   
    HAL_lcdc_module_write_data(0x3C, 1);   

    HAL_lcdc_module_write_reg(0xB4);     //Dot inversion
    HAL_lcdc_module_write_data(0x03, 1);   

    HAL_lcdc_module_write_reg(0xC0);     
    HAL_lcdc_module_write_data(0x0E, 1);   
    HAL_lcdc_module_write_data(0x0E, 1);   
    HAL_lcdc_module_write_data(0x04, 1);   

    HAL_lcdc_module_write_reg(0xC1);     
    HAL_lcdc_module_write_data(0xC5, 1);   

    HAL_lcdc_module_write_reg(0xC2);     
    HAL_lcdc_module_write_data(0x0d, 1);   
    HAL_lcdc_module_write_data(0x00, 1);   

    HAL_lcdc_module_write_reg(0xC3);     
    HAL_lcdc_module_write_data(0x8D, 1);   
    HAL_lcdc_module_write_data(0x2A, 1);   

    HAL_lcdc_module_write_reg(0xC4);     
    HAL_lcdc_module_write_data(0x8D, 1);   
    HAL_lcdc_module_write_data(0xEE, 1);   

    HAL_lcdc_module_write_reg(0xC5);     //VCOM
    HAL_lcdc_module_write_data(0x06, 1); //1D  .06


    HAL_lcdc_module_write_reg(0x36);     //MX, MY, RGB mode
    HAL_lcdc_module_write_data(0xc8, 1);   	//c0

    HAL_lcdc_module_write_reg(0x3A);     //MX, MY, RGB mode
    HAL_lcdc_module_write_data(0x55, 1);   	//c0


    HAL_lcdc_module_write_reg(0xE0);     
    HAL_lcdc_module_write_data(0x0b, 1);   
    HAL_lcdc_module_write_data(0x17, 1);   
    HAL_lcdc_module_write_data(0x0a, 1);   
    HAL_lcdc_module_write_data(0x0d, 1);   
    HAL_lcdc_module_write_data(0x1a, 1);   
    HAL_lcdc_module_write_data(0x19, 1);   
    HAL_lcdc_module_write_data(0x16, 1);   
    HAL_lcdc_module_write_data(0x1d, 1);   
    HAL_lcdc_module_write_data(0x21, 1);   
    HAL_lcdc_module_write_data(0x26, 1);   
    HAL_lcdc_module_write_data(0x37, 1);   
    HAL_lcdc_module_write_data(0x3c, 1);   
    HAL_lcdc_module_write_data(0x00, 1);   
    HAL_lcdc_module_write_data(0x09, 1);   
    HAL_lcdc_module_write_data(0x05, 1);   
    HAL_lcdc_module_write_data(0x10, 1);   

    HAL_lcdc_module_write_reg(0xE1);     
    HAL_lcdc_module_write_data(0x0c, 1);   
    HAL_lcdc_module_write_data(0x19, 1);   
    HAL_lcdc_module_write_data(0x09, 1);   
    HAL_lcdc_module_write_data(0x0d, 1);   
    HAL_lcdc_module_write_data(0x1b, 1);   
    HAL_lcdc_module_write_data(0x19, 1);   
    HAL_lcdc_module_write_data(0x15, 1);   
    HAL_lcdc_module_write_data(0x1d, 1);   
    HAL_lcdc_module_write_data(0x21, 1);   
    HAL_lcdc_module_write_data(0x26, 1);   
    HAL_lcdc_module_write_data(0x39, 1);   
    HAL_lcdc_module_write_data(0x3E, 1);   
    HAL_lcdc_module_write_data(0x00, 1);   
    HAL_lcdc_module_write_data(0x09, 1);   
    HAL_lcdc_module_write_data(0x05, 1);   
    HAL_lcdc_module_write_data(0x10, 1);   

    HAL_mdelay(200);
    HAL_lcdc_module_write_reg(0x29);     //Display on
}


uint32_t st7735s_read_id(void)
{
    uint32_t id = 0;
    HAL_lcdc_module_read(0x04,1,(uint8_t*)&id,4);
    logi("04 ID = 0x%x",id);
    return id;
}
uint32_t st7735s_read_power_mode(void)
{
    uint32_t mode = 0;
    HAL_lcdc_module_read(0x0a,1,(uint8_t*)&mode,4);
    logi("power mode = 0x%x",mode);
    return mode;
}
uint32_t st7735s_tosleep(void)
{
   HAL_lcdc_module_write_reg(0x10);
   return 1;
}


uint32_t st7735s_weakup(void)
{
   HAL_lcdc_module_write_reg(0x11);
   HAL_mdelay(10);
   HAL_lcdc_module_write_reg(0x11);
   HAL_mdelay(10);
   return 1;
}
#endif

