#ifndef _SENSOR_BONE_VIBRATE_H_
#define _SENSOR_BONE_VIBRATE_H_


#define SENSOR_BV_TWI_ID                TWI_ID_01
#define SENSOR_BV_TWI_ADDR              (0x32>>1)

#define SENSOR_BV_CHIP_ID               0x20

//register
#define SENSOR_BV_REG_CHIP_ID           0x0F


void sensor_bv_init(void);
bool sensor_bv_online(void);
void sensor_bv_write(uint8_t adr, uint8_t value);
uint8_t sensor_bv_read(uint8_t adr);

#endif
