#ifndef _APP_LCD_ST7735S_H_
#define _APP_LCD_ST7735S_H_

void st7735s_lcd_init(void);
void st7735s_spi_lcd_init(void);
uint32_t st7735s_read_id(void);
uint32_t st7735s_read_power_mode(void);
uint32_t st7735s_tosleep(void);
uint32_t st7735s_weakup(void) ;
#endif

