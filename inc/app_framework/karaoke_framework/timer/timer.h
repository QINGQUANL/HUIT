#ifndef _TIMER_H_
#define _TIMER_H_

void timer_init(uint32_t id, uint8_t clk_src, uint32_t int_priority, uint32_t period_in_us, bool repeat, void(*timer_cb)(void));

#endif

