#ifndef _APP_LCD_ILI9488_H_
#define _APP_LCD_ILI9488_H_

enum ILI9488_operating_mode_e {
    ILI9488_OPERATING_8BIT           = 0x3,
    ILI9488_OPERATING_3LINE_SPI      = 0x5,
    ILI9488_OPERATING_4LINE_SPI      = 0x7,
};

void ili9488_lcd_init(void);
uint32_t ili9488_read_id(void);


#endif

