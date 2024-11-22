#define LOG_LEV 4
#include "app_lcdc.h"
#include "hal_lcdc.h"
#include "hal.h"


#if (LCDC_EN && (LCD_CHIP_SEL == LCD_CHIP_GC9A01))

void gc9a01_lcd_init(void)
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

uint32_t gc9a01_read_id(void)
{
    uint32_t id = 0;
    HAL_lcdc_module_read(0xDB,1,(uint8_t*)&id,4);
    logi("RD ID = 0x%x",id);//return 93
#if LCDC_INTERFACE
    id = (id&0Xff0000)>>16 |(id&0Xff00);           //0xXX019a00 dont match datasheet
#else
    id = (id&0Xff0000)>>8 |(id&0Xff000000)>>24;    //0x019a0000  match datasheet
#endif
    logi("DB ID = 0x%x",id);
    return id;
}

#endif

