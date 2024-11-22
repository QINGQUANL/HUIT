
#ifndef _APP_LCD_GC9307C_H_
#define _APP_LCD_GC9307C_H_

void gc9307c_init_LCD(void);
void gc9307c_lcd_enter_sleep(void);
void gc9307c_lcd_exit_sleep(void);
uint32_t gc9307c_read_id(void);
uint32_t gc9307c_read_power_mode(void);
uint32_t gc9307c_tosleep(void);
uint32_t gc9307c_weakup(void);

#endif
