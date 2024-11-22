#ifndef _SENSOR_FORCE_H_
#define _SENSOR_FORCE_H_

#include "user_config.h"


#if (SENSOR_FORCE_DEVICE_SEL == 0)
#define SENSOR_FORCE_TWI_ID                 TWI_ID_01
#define SENSOR_FORCE_TWI_ADDR               (0x46)

#define SENSOR_FORCE_INT_PIN_PULL           PIN_PULL_UP100K

#define SENSOR_FORCE_CHIP_ID                0x11

//register
#define SENSOR_FORCE_REG_CHIP_ID            0x80

#elif (SENSOR_FORCE_DEVICE_SEL == 1) //DF100
#define SENSOR_FORCE_TWI_ID                 TWI_ID_01
#define SENSOR_FORCE_TWI_ADDR               (0x4A)

#define SENSOR_FORCE_INT_PIN_PULL           PIN_PULL_UP100K
#define SENSOR_FORCE_CHIP_ID                0x20
#define SENSOR_FORCE_REG_CHIP_ID            0x80

#endif

void sensor_force_init(void);
void sensor_force_deinit(void);
bool sensor_force_online(void);
void sensor_force_reset(void);
void sensor_force_reset_timer_start(void);
void sensor_force_reset_timer_stop(void);
void sensor_force_reset_timer_cb(void* xTimer);
void sensor_force_interrupt(void);
void sensor_force_process(void);
void sensor_force_write(uint8_t adr, uint8_t value);
uint8_t sensor_force_read(uint8_t adr);


#endif

