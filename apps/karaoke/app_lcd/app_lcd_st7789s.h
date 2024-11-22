#ifndef _APP_LCD_ST7789S_H_
#define _APP_LCD_ST7789S_H_

void st7789s_lcd_init(void);
uint32_t st7789s_read_id(void);
uint32_t st7789s_read_power_mode(void);
uint32_t st7789s_tosleep(void);
uint32_t st7789s_weakup(void) ;
#endif

