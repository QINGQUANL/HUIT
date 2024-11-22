//#define LOG_LEV 5
#include "sys_inc.h"
#include "user_config.h"
#include "sensor.h"
#include "tws_profile.h"
#include "hal.h"

#if SENSOR_FORCE_EN

bool sensor_force_online_sta;
#if (SENSOR_FORCE_DEVICE_SEL == 1) //DF100
OAL_TIMER SensorForceResetTimerHandle;
#define SENSOR_FORCE_RESET_TIME     4000
#endif

#if (SENSOR_FORCE_DEVICE_SEL == 0)
const uint8_t sensor_f_af3068_init_tbl[] = {
    //reg,value
    0x00,0xb5,
    0x01,0x50,
    0x07,0x02,
    0x08,0x02,
    0x09,0xb0,
    0x0b,0x03,
    0x0c,0x06,
    0x0d,0x45,
    0x0e,0x93,
    0x0f,0x01,
};

#elif (SENSOR_FORCE_DEVICE_SEL == 1) //DF100
const uint8_t sensor_f_af100_init_left_tbl[] = {
    //left ear
    //reg,value
    0x00,0xb5,
    0x01,0x40,
    0x06,0x85,
    0x07,0x07,
    0x08,0x00,
    0x09,0xb0,
    0x0a,0x40,
    0x0b,0x03,
    0x0c,0x0e,
    0x0d,0x00,
    0x0e,0x11,
    0x0f,0x01,
};

const uint8_t sensor_f_af100_init_right_tbl[] = {
    //right ear
    //reg,value
    0x00,0xb5,
#if (SENSOR_FORCE_PARM_SEL == 0)
    0x01,0x50,
#elif (SENSOR_FORCE_PARM_SEL == 1)
    0x01,0x40,
#endif
    0x06,0x85,
    0x07,0x05,
    0x08,0x00,
    0x09,0xb0,
    0x0a,0x40,
    0x0b,0x03,
    0x0c,0x0a,
    0x0d,0x00,
    0x0e,0x11,
    0x0f,0x01,
};
#endif



void sensor_force_init(void)
{

#if SENSOR_TEST_EN
    sensor_force_online_sta = true;
#else

    if (sensor_force_online()) {
        sensor_force_online_sta = true;
    } else {
        sensor_force_online_sta = false;
        return;
    }

#if 0 //use the key_io
    /* int gpio初始化，function设置为EINT，开内部上拉，开下降沿中断 */
    pin_set_pull(SENSOR_FORCE_INT_PIN_NUM, SENSOR_FORCE_INT_PIN_PULL);
    pin_set_func(SENSOR_FORCE_INT_PIN_NUM, PIN_FUNCEINT);
    pin_irq_clk_config(SENSOR_FORCE_INT_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
    pin_irq_config(SENSOR_FORCE_INT_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_F_EDGE);
#endif

#if (SENSOR_FORCE_DEVICE_SEL == 0)
    /* 寄存器初始化 */
    for (uint8_t i=0;i<sizeof(sensor_f_af3068_init_tbl);i=i+2) {
        sensor_force_write(sensor_f_af3068_init_tbl[i], sensor_f_af3068_init_tbl[i+1]);
    }

#elif (SENSOR_FORCE_DEVICE_SEL == 1) //DF100
    if (tws_ear_type_get() == TWS_EAR_TYPE_LEFT) { //左耳
        for (uint8_t i=0;i<sizeof(sensor_f_af100_init_left_tbl);i=i+2) {
            sensor_force_write(sensor_f_af100_init_left_tbl[i], sensor_f_af100_init_left_tbl[i+1]);
        }
    } else {
        for (uint8_t i=0;i<sizeof(sensor_f_af100_init_right_tbl);i=i+2) {
            sensor_force_write(sensor_f_af100_init_right_tbl[i], sensor_f_af100_init_right_tbl[i+1]);
        }
    }

        if (NULL == SensorForceResetTimerHandle) {
            OAL_timer_create(&SensorForceResetTimerHandle, "SensorForceResetTimer", OAL_ms_to_ticks(SENSOR_FORCE_RESET_TIME),
                sensor_force_reset_timer_cb, (void *)1, 0, OAL_TIMER_NO_ACTIVATE);
    }
#endif

#endif
}

void sensor_force_deinit(void)
{
#if (SENSOR_FORCE_DEVICE_SEL == 1) //DF100
    sensor_force_reset_timer_stop();
#endif
}

bool sensor_force_online(void)
{
    uint8_t chip_id;

    /* 校验CHIP ID */
    for (uint32_t i=0;i<3;i++) {
        chip_id = sensor_force_read(SENSOR_FORCE_REG_CHIP_ID);
        if (SENSOR_FORCE_CHIP_ID == chip_id) {
            logi("force-sensor is online");
            return true;
        }
        HAL_mdelay(100);
    }

    loge("force-sensor error,chip_id should be 0x11,but is %x", chip_id);
    return false;
}

#if (SENSOR_FORCE_DEVICE_SEL == 1) //DF100

void sensor_force_reset(void)
{
    sensor_force_write(0x0f, 0x01);
}

AT_SRAM(.sensor)
void sensor_force_reset_timer_start(void)
{
     if (NULL != SensorForceResetTimerHandle) {
        OAL_timer_activate(&SensorForceResetTimerHandle, OAL_TIMER_AUTO_ACTIVATE);
    }
}

AT_SRAM(.sensor)
void sensor_force_reset_timer_stop(void)
{
    if (NULL != SensorForceResetTimerHandle) {
        OAL_timer_activate(&SensorForceResetTimerHandle, OAL_TIMER_NO_ACTIVATE);
    }
}

void sensor_force_reset_timer_cb(void* xTimer)
{
    sensor_force_reset();
}
#endif

void sensor_force_write(uint8_t adr, uint8_t value)
{
    uint8_t data[2];

    data[0] = adr;
    data[1] = value;
#if TWI_IO_EN
    twi_io_write(SENSOR_FORCE_TWI_ADDR, data, 2);
#else
    twi_write(SENSOR_FORCE_TWI_ID, SENSOR_FORCE_TWI_ADDR, data, 2);
#endif
}


uint8_t sensor_force_read(uint8_t adr)
{
    uint8_t value;

#if TWI_IO_EN
    twi_io_write(SENSOR_FORCE_TWI_ADDR, &adr, 1);
    twi_io_read(SENSOR_FORCE_TWI_ADDR, &value, 1);
#else
    twi_write(SENSOR_FORCE_TWI_ID, SENSOR_FORCE_TWI_ADDR, &adr, 1);
    twi_read(SENSOR_FORCE_TWI_ID, SENSOR_FORCE_TWI_ADDR, &value, 1);
#endif

    return value;
}

#endif
