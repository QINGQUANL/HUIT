
#ifndef _HAL_PMS_H_
#define _HAL_PMS_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys_types.h"


enum pm_prevent_sleep
{
    PS_AUDIO_PLAY   = 1 << 0,
    PS_AUDIO_RECORD = 1 << 1,
    PS_AUDIO_ANC    = 1 << 2,
    PS_AUDIO_KWS    = 1 << 3,
    PS_BT_INCOMING  = 1 << 4,
    PS_BTC          = 1 << 5,
    PS_APP_PROCESS  = 1 << 6,
    PS_BTH          = 1 << 7,
    PS_UART         = 1 << 8,
    PS_MODE         = 1 << 9,
    PS_AUDIO_I2S    = 1 << 10,
};


void pm_prevent_sleep_set(enum pm_prevent_sleep prv_slp_bit);
void pm_prevent_sleep_clear(enum pm_prevent_sleep prv_slp_bit);
uint32_t pm_prevent_sleep_get(void);
uint8_t pm_prevent_sleep_delay_get(void);
void pm_prevent_sleep_delay_set(uint8_t sec);
void pm_prevent_sleep_delay_inc(uint8_t sec);
void pm_prevent_sleep_delay_dec(uint8_t sec);
void pm_prevent_sleep_delay_clear(void);
void set_poweron_time(uint32_t time);
uint32_t get_poweron_time(void);

#ifdef __cplusplus
}
#endif

#endif /* _HAL_PMS_H_ */
