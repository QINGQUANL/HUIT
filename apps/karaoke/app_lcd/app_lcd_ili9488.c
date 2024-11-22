#define LOG_LEV 4
#include "app_lcdc.h"
#include "hal_lcdc.h"

#if (LCDC_EN && (LCD_CHIP_SEL == LCD_CHIP_ILI9488))

void ili9488_lcd_init(void)
{
    logi("ili9488_lcd_init");

#if LCDC_INTERFACE
    HAL_lcdc_module_write_reg(0xB0);                                 //interface mode control
#if LCDC_SPI_INTERFACE
    HAL_lcdc_module_write_data(0x00,1);                              //0xb0 bit 7 is SDA_EN,0 is interface2,1 is interface1
#else
    HAL_lcdc_module_write_data(0x80,1);
#endif
#endif

    HAL_lcdc_module_write_reg(0xE0);                   //Positive Gamma Control
    HAL_lcdc_module_write_data(0x00,1);
    HAL_lcdc_module_write_data(0x07,1);
    HAL_lcdc_module_write_data(0x0f,1);
    HAL_lcdc_module_write_data(0x0D,1);
    HAL_lcdc_module_write_data(0x1B,1);
    HAL_lcdc_module_write_data(0x0A,1);
    HAL_lcdc_module_write_data(0x3c,1);
    HAL_lcdc_module_write_data(0x78,1);
    HAL_lcdc_module_write_data(0x4A,1);
    HAL_lcdc_module_write_data(0x07,1);
    HAL_lcdc_module_write_data(0x0E,1);
    HAL_lcdc_module_write_data(0x09,1);
    HAL_lcdc_module_write_data(0x1B,1);
    HAL_lcdc_module_write_data(0x1e,1);
    HAL_lcdc_module_write_data(0x0f,1);

    HAL_lcdc_module_write_reg(0xE1);                   //Negative Gamma Control
    HAL_lcdc_module_write_data(0x00,1);
    HAL_lcdc_module_write_data(0x22,1);
    HAL_lcdc_module_write_data(0x24,1);
    HAL_lcdc_module_write_data(0x06,1);
    HAL_lcdc_module_write_data(0x12,1);
    HAL_lcdc_module_write_data(0x07,1);
    HAL_lcdc_module_write_data(0x36,1);
    HAL_lcdc_module_write_data(0x47,1);
    HAL_lcdc_module_write_data(0x47,1);
    HAL_lcdc_module_write_data(0x06,1);
    HAL_lcdc_module_write_data(0x0a,1);
    HAL_lcdc_module_write_data(0x07,1);
    HAL_lcdc_module_write_data(0x30,1);
    HAL_lcdc_module_write_data(0x37,1);
    HAL_lcdc_module_write_data(0x0f,1);

    HAL_lcdc_module_write_reg(0xC0);                   //Power Control 1
    HAL_lcdc_module_write_data(0x10,1);
    HAL_lcdc_module_write_data(0x10,1);

    HAL_lcdc_module_write_reg(0xC1);                   //Power Control 2
    HAL_lcdc_module_write_data(0x41,1);

    HAL_lcdc_module_write_reg(0xC5);                   //VCOM Control
    HAL_lcdc_module_write_data(0x00,1);
    HAL_lcdc_module_write_data(0x22,1);
    HAL_lcdc_module_write_data(0x80,1);

    //HAL_lcdc_module_write_reg(0x36); 
    //HAL_lcdc_module_write_data(0x48,1);

    HAL_lcdc_module_write_reg(0x3A);                   //Interface Pixel Format,RGB 666,ILI9488 spi don't support RGB565
#if (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB666)
    HAL_lcdc_module_write_data(0x66,1);
#elif (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB565)
    HAL_lcdc_module_write_data(0x55,1);
#endif

    //HAL_lcdc_module_write_reg(0XB0);
    //lcdc_write_dat(0x80,1);               //0xb0 bit 7 is SDA_EN,0 is interface2,1 is interface1
    HAL_lcdc_module_write_reg(0xB1);                   //Frame Rate Control
    HAL_lcdc_module_write_data(0xB0,1);
    HAL_lcdc_module_write_data(0x11,1);
    HAL_lcdc_module_write_reg(0xB4);                   //Display Inversion Control
    HAL_lcdc_module_write_data(0x02,1);
    HAL_lcdc_module_write_reg(0xB6);                   //Display Function Control
    HAL_lcdc_module_write_data(0x02,1);
    HAL_lcdc_module_write_data(0x02,1);

    HAL_lcdc_module_write_reg(0xB7);                   //Entry Mode Set
    HAL_lcdc_module_write_data(0xC6,1);

    HAL_lcdc_module_write_reg(0xE9);                   //Set Image Function
    HAL_lcdc_module_write_data(0x00,1);

    HAL_lcdc_module_write_reg(0xF7);                   //Adjust Control 3
    HAL_lcdc_module_write_data(0xA9,1);
    HAL_lcdc_module_write_data(0x51,1);
    HAL_lcdc_module_write_data(0x2C,1);
    HAL_lcdc_module_write_data(0x82,1);

    HAL_lcdc_module_write_reg(0x11);                   //Sleep OUT
    HAL_mdelay(120);//delay_nop(120); 
    HAL_lcdc_module_write_reg(0x29);                   //Display ON
}

uint32_t ili9488_read_id(void)
{
    uint32_t id = 0;
#if LCDC_INTERFACE
    HAL_lcdc_module_spi_dummy_cycle_set(0);
    /*
     * single命令，必须加lcdc_spi_cs_hold和lcdc_spi_cs_de_select,dummy_cyc必须设置为0
     * 0xFB 命令一次只读1个param
     */
    HAL_lcdc_module_spi_cs_hold();
    for(uint8_t i=0;i<4;i++) {
        HAL_lcdc_module_write_reg(0xFB);                     //SPI Read Command Setting,SPI_READ_EN
        HAL_lcdc_module_write_data(0x80+3-i,1);
        if(i==3) {
            HAL_lcdc_module_spi_cs_de_select();
        }
        HAL_lcdc_module_write_reg(0xD3);                    //Read ID4
        id |= HAL_lcdc_module_read_data(1)<<(8*i);
        logi("id%d = 0x%x",i,id);
    }
    id &= 0xffffff;
    logi("ID = 0x%x",id);
    HAL_lcdc_module_write_reg(0xFB);                        //SPI Read Command Setting,SPI_READ_DIS
    HAL_lcdc_module_write_data(0x00,1);
    
    HAL_lcdc_module_spi_dummy_cycle_set(1);
#else
    HAL_lcdc_module_write_reg(0xD3);                        //Read ID4
    for(uint8_t i=0;i<4;i++) {
        id |= HAL_lcdc_module_read_data(1)<<(8*(3-i));
    }
    id &= 0xffffff;
    logi("ID = 0x%x",id);
#endif
    return id;
}

#endif

