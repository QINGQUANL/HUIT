#define LOG_LEV 4
#include "app_lcdc.h"
#include "hal_lcdc.h"
#include "hal_timestamp.h"

#if (LCDC_EN && (LCD_CHIP_SEL == LCD_CHIP_ST7789S))

void st7789s_lcd_init(void)
{
    logi("st7789s_lcd_init");

    HAL_lcdc_module_write_reg(0x11);
    HAL_mdelay(120);
    //--------------------------------------Display Setting------------------------------------------//
    HAL_lcdc_module_write_reg(0x36);
    HAL_lcdc_module_write_data(0x00,1);
    HAL_lcdc_module_write_reg(0x3a);
#if (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB666)
    HAL_lcdc_module_write_data(0x06,1);
#elif (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB565)
    HAL_lcdc_module_write_data(0x05,1);
#endif
    //--------------------------------ST7789S Frame rate setting----------------------------------//
    HAL_lcdc_module_write_reg(0xb2);
    HAL_lcdc_module_write_data(0x0c,1);
    HAL_lcdc_module_write_data(0x0c,1);
    HAL_lcdc_module_write_data(0x00,1);
    HAL_lcdc_module_write_data(0x33,1);
    HAL_lcdc_module_write_data(0x33,1);
    HAL_lcdc_module_write_reg(0xb7);
    HAL_lcdc_module_write_data(0x35,1);
    //---------------------------------ST7789S Power setting--------------------------------------//
    HAL_lcdc_module_write_reg(0xbb);
    HAL_lcdc_module_write_data(0x2b,1);
    HAL_lcdc_module_write_reg(0xc0);
    HAL_lcdc_module_write_data(0x2c,1);
    HAL_lcdc_module_write_reg(0xc2);
    HAL_lcdc_module_write_data(0x01,1);
    HAL_lcdc_module_write_reg(0xc3);
    HAL_lcdc_module_write_data(0x11,1);
    HAL_lcdc_module_write_reg(0xc4);
    HAL_lcdc_module_write_data(0x20,1);
    HAL_lcdc_module_write_reg(0xc6);
    HAL_lcdc_module_write_data(0x0f,1);
    HAL_lcdc_module_write_reg(0xd0);
    HAL_lcdc_module_write_data(0xa4,1);
    HAL_lcdc_module_write_data(0xa1,1);
    //--------------------------------ST7789S gamma setting---------------------------------------//
    HAL_lcdc_module_write_reg(0xe0);
    HAL_lcdc_module_write_data(0xd0,1);
    HAL_lcdc_module_write_data(0x00,1);
    HAL_lcdc_module_write_data(0x05,1);
    HAL_lcdc_module_write_data(0x0e,1);
    HAL_lcdc_module_write_data(0x15,1);
    HAL_lcdc_module_write_data(0x0d,1);
    HAL_lcdc_module_write_data(0x37,1);
    HAL_lcdc_module_write_data(0x43,1);
    HAL_lcdc_module_write_data(0x47,1);
    HAL_lcdc_module_write_data(0x09,1);
    HAL_lcdc_module_write_data(0x15,1);
    HAL_lcdc_module_write_data(0x12,1);
    HAL_lcdc_module_write_data(0x16,1);
    HAL_lcdc_module_write_data(0x19,1);
    HAL_lcdc_module_write_reg(0xe1);
    HAL_lcdc_module_write_data(0xd0,1);
    HAL_lcdc_module_write_data(0x00,1);
    HAL_lcdc_module_write_data(0x05,1);
    HAL_lcdc_module_write_data(0x0d,1);
    HAL_lcdc_module_write_data(0x0c,1);
    HAL_lcdc_module_write_data(0x06,1);
    HAL_lcdc_module_write_data(0x2d,1);
    HAL_lcdc_module_write_data(0x44,1);
    HAL_lcdc_module_write_data(0x40,1);
    HAL_lcdc_module_write_data(0x0e,1);
    HAL_lcdc_module_write_data(0x1c,1);
    HAL_lcdc_module_write_data(0x18,1);
    HAL_lcdc_module_write_data(0x16,1);
    HAL_lcdc_module_write_data(0x19,1);
    HAL_lcdc_module_write_reg(0x29);
#if LCD_USE_TE
    HAL_lcdc_module_write_reg(0x35);      //TE EN
    HAL_lcdc_module_write_data(0x00,1);   //mode 0
#endif
}

uint32_t st7789s_read_id(void)
{
    uint32_t id = 0;
    HAL_lcdc_module_read(0x04,1,(uint8_t*)&id,4);
    logi("04 ID = 0x%x",id);
    return id;
}
uint32_t st7789s_read_power_mode(void)
{
    uint32_t mode = 0;
    HAL_lcdc_module_read(0x0a,1,(uint8_t*)&mode,4);
    logi("power mode = 0x%x",mode);
    return mode;
}
uint32_t st7789s_tosleep(void)
{
   HAL_lcdc_module_write_reg(0x10);
   return 1;
}


uint32_t st7789s_weakup(void)
{
   HAL_lcdc_module_write_reg(0x11);
   HAL_mdelay(10);
   HAL_lcdc_module_write_reg(0x11);
   HAL_mdelay(10);
   return 1;
}
#endif

