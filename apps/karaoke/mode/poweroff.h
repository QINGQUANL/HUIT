#ifndef __MODE_POWEROFF_H__
#define __MODE_POWEROFF_H__

void mode_poweroff(void);
void mode_poweroff_enter(void);
void mode_poweroff_loop(void);
void mode_poweroff_exit(void);

void poweroff_event_process(uint32_t msg);



#endif

