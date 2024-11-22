#define LOG_LEV 4
#include "app_lcdc.h"
#include "hal_lcdc.h"
#include "hal.h"

#if (LCDC_EN && (LCD_CHIP_SEL == LCD_CHIP_GC9307C))

void gc9307c_init_LCD(void)
{
    logi("gc9a01_lcd_init");
    
    HAL_lcdc_module_write_reg(0xfe);     
    HAL_lcdc_module_write_reg(0xfe);     
    HAL_lcdc_module_write_reg(0xef); 
    
    HAL_lcdc_module_write_reg(0x36);     
    HAL_lcdc_module_write_data(0x48, 1);     

    HAL_lcdc_module_write_reg(0x3a);     
    HAL_lcdc_module_write_data(0x05, 1);

    HAL_lcdc_module_write_reg(0x86);     
    HAL_lcdc_module_write_data(0x98, 1);   

    HAL_lcdc_module_write_reg(0x89);     //Dot inversion
    HAL_lcdc_module_write_data(0x03, 1);   

    HAL_lcdc_module_write_reg(0x8b);     
    HAL_lcdc_module_write_data(0x80, 1);   


    HAL_lcdc_module_write_reg(0x8d);     
    HAL_lcdc_module_write_data(0x22, 1);   

    HAL_lcdc_module_write_reg(0x8e);     
    HAL_lcdc_module_write_data(0x0f, 1);   

    HAL_lcdc_module_write_reg(0xe8);     
    HAL_lcdc_module_write_data(0x11, 1);   //11 22改，原0x12
    HAL_lcdc_module_write_data(0x64, 1);   //11 22改，原0x00

    HAL_lcdc_module_write_reg(0xff);     
    HAL_lcdc_module_write_data(0x62, 1);   

    HAL_lcdc_module_write_reg(0x99);     //VCOM
    HAL_lcdc_module_write_data(0x3e, 1); //1D  .06


    HAL_lcdc_module_write_reg(0x9d);     //MX, MY, RGB mode
    HAL_lcdc_module_write_data(0x4b, 1);   	//c0

    HAL_lcdc_module_write_reg(0xc3);     //MX, MY, RGB mode
    HAL_lcdc_module_write_data(0x02, 1);   	//c0
    HAL_lcdc_module_write_reg(0xc4);     
    HAL_lcdc_module_write_data(0x02, 1);    

    HAL_lcdc_module_write_reg(0xc9);     
    HAL_lcdc_module_write_data(0x0a, 1);   

    HAL_lcdc_module_write_reg(0xf0);     
    HAL_lcdc_module_write_data(0x84, 1);   
    HAL_lcdc_module_write_data(0x05, 1);   
    HAL_lcdc_module_write_data(0x08, 1);   
    HAL_lcdc_module_write_data(0x07, 1);   
    HAL_lcdc_module_write_data(0x04, 1);   
    HAL_lcdc_module_write_data(0x29, 1);   

    HAL_lcdc_module_write_reg(0xf2);     
    HAL_lcdc_module_write_data(0x44, 1);   
    HAL_lcdc_module_write_data(0x05, 1);   
    HAL_lcdc_module_write_data(0x08, 1);   
    HAL_lcdc_module_write_data(0x07, 1);   
    HAL_lcdc_module_write_data(0x04, 1);   
    HAL_lcdc_module_write_data(0x29, 1);   

    HAL_lcdc_module_write_reg(0xf1);     
    HAL_lcdc_module_write_data(0x43, 1);   
    HAL_lcdc_module_write_data(0x75, 1);   
    HAL_lcdc_module_write_data(0x78, 1);   
    HAL_lcdc_module_write_data(0x32, 1);   
    HAL_lcdc_module_write_data(0x33, 1);   
    HAL_lcdc_module_write_data(0x6f, 1);  

    HAL_lcdc_module_write_reg(0xf3);     
    HAL_lcdc_module_write_data(0x43, 1);   
    HAL_lcdc_module_write_data(0x75, 1);   
    HAL_lcdc_module_write_data(0x78, 1);   
    HAL_lcdc_module_write_data(0x32, 1);   
    HAL_lcdc_module_write_data(0x33, 1);   
    HAL_lcdc_module_write_data(0x6f, 1); 

    HAL_lcdc_module_write_reg(0x35);     
    HAL_lcdc_module_write_data(0x00, 1);   
    HAL_lcdc_module_write_reg(0x44);     
    HAL_lcdc_module_write_data(0x00, 1);  
    HAL_lcdc_module_write_data(0x0a, 1);  

    // HAL_lcdc_module_write_reg(0x80);     
    // HAL_lcdc_module_write_data(0x00, 1);   	
	
    // HAL_lcdc_module_write_reg(0xf7);     
    // HAL_lcdc_module_write_data(0x20, 1);
    // HAL_lcdc_module_write_data(0x3f, 1);  
    // HAL_lcdc_module_write_data(0x00, 1);
    // HAL_lcdc_module_write_data(0x00, 1);	 	   	 

    HAL_lcdc_module_write_reg(0x11);   
    HAL_mdelay(120); 
    
    HAL_lcdc_module_write_reg(0x29);     //Display on
    HAL_lcdc_module_write_reg(0x2c);     //Display on
}

void gc9307c_lcd_enter_sleep(void)
{
    logi("gc9307c_lcd_enter_sleep\r\n");
    HAL_lcdc_module_write_reg(0x28);   // 进入显示关闭模式
    HAL_mdelay(120);                   // 延时120毫秒
    HAL_lcdc_module_write_reg(0x10);   // 进入睡眠模式
    HAL_mdelay(50);                    // 延时50毫秒
    logi("gc9307c_lcd_enter_sleep_exit\r\n");
}

void gc9307c_lcd_exit_sleep(void)
{
    logi("gc9307c_lcd_exit_sleep\r\n");
    HAL_lcdc_module_write_reg(0x11);   // 退出睡眠模式
    HAL_mdelay(120);                   // 延时120毫秒
    HAL_lcdc_module_write_reg(0x29);   // 开启显示
    logi("gc9307c_lcd_exit_sleep_exit\r\n");
}

uint32_t gc9307c_read_id(void)
{
    logi("gc9307c_read_id\r\n");
    uint32_t id = 0;
    // 命令代码 0x04 用于读取显示识别信息
    HAL_lcdc_module_read(0x04, 1, (uint8_t*)&id, 4);
    logi("04 ID = 0x%x", id);
    logi("gc9307c_read_id_exit\r\n");
    return id;
}

uint32_t gc9307c_read_power_mode(void)
{
    logi("gc9307c_read_power_mode\r\n");
    uint32_t mode = 0;
    // 命令代码 0x09 用于读取显示状态
    HAL_lcdc_module_read(0x09, 1, (uint8_t*)&mode, 4);
    logi("power mode = 0x%x", mode);
    logi("gc9307c_read_power_mode_exit\r\n");
    return mode;
}

uint32_t gc9307c_tosleep(void)
{
    logi("gc9307c_tosleep\r\n");
   // 命令代码 0x10 用于进入睡眠模式
   HAL_lcdc_module_write_reg(0x10);
   logi("gc9307c_tosleep_exit\r\n");
   return 1;
}

uint32_t gc9307c_weakup(void)
{
    logi("gc9307c_weakup\r\n");
   // 命令代码 0x11 用于退出睡眠模式
   HAL_lcdc_module_write_reg(0x11);
   HAL_mdelay(10);
   HAL_lcdc_module_write_reg(0x11);
   HAL_mdelay(10);
   logi( "gc9307c_weakup_exit\r\n");
   return 1;
}

#endif
