#ifndef _MODE_SLEEP_H_
#define _MODE_SLEEP_H_

#ifdef __cplusplus
 extern "C" {
#endif
#include "pms.h"

typedef void (*sleep_app_cb)(void);

typedef struct {
    bool sleep_enable;
    uint8_t sleep_delay;
    sleep_app_cb sleep_en_app_cb;
    sleep_app_cb sleep_dis_app_cb;
} sleep_app_ctl;

uint8_t sleep_delay_get(void);
void sleep_delay_inc(uint8_t sec);
void sleep_delay_dec(uint8_t sec);
void sleep_prevent_set(enum pm_prevent_sleep prevent);
void sleep_prevent_clear(enum pm_prevent_sleep prevent);
uint32_t sleep_prevent_get(void);
void sleep_delay_set(void);

//user interfaces
uint32_t btble_enter_sleep_mode(void);
uint32_t enter_poweroff_mode(void);

void sleep(void);
void sleep_register_app_callback(bool sleep_en, uint8_t sleep_delay, void (*sleep_en_cb)(void), void(*sleep_dis_en)(void));

#ifdef __cplusplus
}
#endif


#endif

