#ifndef _MODE_AFC_H_
#define _MODE_AFC_H_
#include "user_config.h"


void mode_afc(void);
void mode_afc_enter(void);
void mode_afc_loop(void);
void mode_afc_exit(void);



void afc_register_event_handler(void (*handler)(sys_event_info_t *info));

#endif
