#ifndef _KEY_ONOFF_H_
#define _KEY_ONOFF_H_


void key_onoff_init(void);
void key_onoff_state_set(uint32_t state);
uint32_t key_onoff_state_get(void);
void key_onoff_process(uint32_t event);
void key_onoff_scan(void);


#endif
