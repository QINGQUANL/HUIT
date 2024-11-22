#ifndef _SENSOR_G_H_
#define _SENSOR_G_H_


#define SENSOR_G_TWI_ID                 TWI_ID_01
#define SENSOR_G_INT_PIN_PULL           PIN_PULL_UP100K

#define SENSOR_G_M_CLICK_INTERVAL       (400/5)                 //400ms

#if (SENSOR_G_DEVICE_SEL == 0) //MSA301
#define SENSOR_G_TWI_ADDR               (0x4e>>1)

#define SENSOR_G_CHIP_ID                0x13

//register
#define SENSOR_G_REG_CHIP_ID            0x01
#define SENSOR_G_REG_MOTION_INTERRUPT   0x09
#define SENSOR_G_REG_POWER_MODE         0x11

#define SENSOR_G_MOTION_S_TAP_INT       (0x01<<5)
#define SENSOR_G_MOTION_D_TAP_INT       (0x01<<4)

#elif (SENSOR_G_DEVICE_SEL == 1) //SC7A20
#define SENSOR_G_TWI_ADDR               0x19
#define SENSOR_G_CHIP_ID                0x11

//register
#define SENSOR_G_REG_CHIP_ID            0x0f

#elif (SENSOR_G_DEVICE_SEL == 2) //DA270
#define SENSOR_G_TWI_ADDR               0x27
#define SENSOR_G_CHIP_ID                0x13

//register
#define SENSOR_G_REG_CHIP_ID            0x01
#define SENSOR_G_REG_POWER_MODE         0x11

#elif (SENSOR_G_DEVICE_SEL == 9)
#define SENSOR_G_SC7A20_TWI_ADDR        0x19
#define SENSOR_G_SC7A20_CHIP_ID         0x11
#define SENSOR_G_SC7A20_REG_CHIP_ID     0x0f

#define SENSOR_G_DA270_TWI_ADDR         0x27
#define SENSOR_G_DA270_CHIP_ID          0x13
#define SENSOR_G_DA270_REG_CHIP_ID      0x01
#define SENSOR_G_DA270_REG_POWER_MODE   0x11
#endif

void sensor_g_init(void);
void sensor_g_deinit(void);
bool sensor_g_online(void);
void sensor_g_interrupt(void);
void sensor_g_process(void);
void sensor_g_scan(void);
void sensor_g_write(uint8_t twi_adr, uint8_t adr, uint8_t value);
uint8_t sensor_g_read(uint8_t twi_adr, uint8_t adr);

#endif

