#ifndef __MODE_CHARGE_H__
#define __MODE_CHARGE_H__

#include "event.h"

void restart(void);
void mode_charge(void);
void mode_charge_enter(void);
void mode_charge_loop(void);
void mode_charge_exit(void);

void charge_event_process(sys_event_info_t *info);
void charge_register_event_handler(void (*handler)(uint32_t event));
bool charge_mode_sta_get(void);
void charge_mode_sta_set(bool enter);

#endif
