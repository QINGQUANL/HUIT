#define LOG_LEV 5
#include "sys_inc.h"
#include "user_config.h"
#include "sensor.h"
#include "hal.h"


bool sensor_bv_online_sta;

void sensor_bv_init(void)
{
    uint8_t value;

    if (sensor_bv_online()) {
        sensor_bv_online_sta = true;
    } else {
        sensor_bv_online_sta = false;
        return;
    }

    /*0x2F(AXES_CTRL_REG)
    * [7]:AXISZ_EN,Z-axis enable.1: enabled
    * [6]:AXISY_EN,Y-axis enable.1: enabled
    * [5]:AXISX_EN,X-axis enable.1: enabled
    * [0]:ODR_AUTO_EN,ODR AUTO enable.1: ODR and BCLK auto enabled
    */
    sensor_bv_write(0x2F, 0xE1);

    /*0x2E(TDM_CTRL_REG)
    * [7]:TDM enable. (1: TDM off)
	* [6]:TDM delayed configuration. (1: TDM delayed configuration)
	* [5]:TDM data valid. (1: data valid on the falling edge of BCLK)
	* [4]:TDM mapping. (0: AXEX --> SLOT0; AXEY --> SLOT1; AXEZ --> SLOT2;)
	* [3]:reserved
	* [2:1]:WCLK_fq [0:1],TDM clock frequencies. (01: WCLK = 16 kHz). ODR_AUTO_EN(0x2F[0]) is 1,bypassing
	* [0]:reserved
    */
    sensor_bv_write(0x2E, 0xE2);

    /*0x24(TDM_cmax[11:8])
    * TDM_cmax[11:8] MSB
    */
    sensor_bv_write(0x24, 0x00);

    /*0x25(TDM_cmax[7:0])
    * TDM_cmax[7:0] LSB
    */
    sensor_bv_write(0x25, 0x40);

    /*0x26(CTRL_REG)
    * [5]:Device in disabled mode.(0: Normal mode)
    */
    value = sensor_bv_read(0x26);
    value &= ~0x20;
    sensor_bv_write(0x26, value);

    /*0x2E(TDM_CTRL_REG)
    * [7]:TDM enable. (0: TDM on)
    */
    value = sensor_bv_read(0x2E);
    value &= ~0x80;
    sensor_bv_write(0x2E, value);
}

bool sensor_bv_online(void)
{
    uint8_t chip_id;

    /* 校验CHIP ID
    * 0x0F(WHO_AM_I)
    */
    for (uint32_t i=0;i<3;i++) {
        chip_id = sensor_bv_read(SENSOR_BV_REG_CHIP_ID);
        if (SENSOR_BV_CHIP_ID == chip_id) {
            return true;
        }
        HAL_mdelay(100);
    }

    loge("sensor bone vibrate error,chip_id should be 0x20,but is %x", chip_id);
    return false;
}

void sensor_bv_write(uint8_t adr, uint8_t value)
{
    uint8_t data[2];

    data[0] = adr;
    data[1] = value;
    twi_write(SENSOR_BV_TWI_ID, SENSOR_BV_TWI_ADDR, data, 2);
}

uint8_t sensor_bv_read(uint8_t adr)
{
    uint8_t value;

    twi_write(SENSOR_BV_TWI_ID, SENSOR_BV_TWI_ADDR, &adr, 1);
    twi_read(SENSOR_BV_TWI_ID, SENSOR_BV_TWI_ADDR, &value, 1);

    return value;
}


