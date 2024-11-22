#define LOG_LEV 5
#include "sys_inc.h"
#include "user_config.h"
#include "sensor.h"
#include "hal.h"
#include "bt.h"
#include "bt_audio_manager.h"


#if SENSOR_G_EN

bool sensor_g_online_sta;
uint8_t sensor_g_model;
#if (SENSOR_G_CLICK_SEL == 2)
uint8_t sensor_g_interval;      //两次间隔时间
uint8_t sensor_g_click_cnt;     //多击次数
#endif

/* 敏芯微MSA301 */
const uint8_t sensor_g_msa301_init_tbl[] = {
    //reg,value

    /*0x0F
    * [3:2]-RESOLUTION[1:0]: resolution of x/y/z axes, 00:14bit,
    * [1:0]-FS[1:0]: acceleration range of x/y/z axes,01:+/-4g,
    */
    0x0f,0x01,

    /*0x11
    * [7:6]-PWR_MODE:01: low power mode
    * [4:1]-LOW_POWER_BW[3:0]:1010-1111:500Hz
    */
    0x11,0x5e,

    /*0x10
    * [7]- X_AXIS_DIS: 0: enable,
    * [6]- Y_AXIS_DIS: 0: enable,
    * [5]- Z_AXIS_DIS: 0: enable,
    * [3:0]- ODR[3:0]: 1000: 250Hz,
    */
    0x10,0x08,

    /*0x20
    * [1]- INT1_OD: select output for INT1, 0: push-pull,
    * [0]- INT1_LVL: select active level for INT1, 0: low,
    */
    0x20,0x00,

    /*0x21
    * [7]- RESET_INT: reset or not, 0: not reset all latched interrupt,
    * [3:0]- LATCH_INT[3:0]: 0010: tempoary latched 500ms
    */
#if (SENSOR_G_CLICK_SEL == 0 || SENSOR_G_CLICK_SEL == 1)
    0x21,0x02,  //latched 500ms
#else
    0x21,0x00,  //non-latched
#endif

    /*0x16
    * [6]-ORIENT_INT_EN: orient interrupt, 0: disable,
    * [5]-S_TAP_INT_EN: single tap interrupt,  0: disable,
    * [4]-D_TAP_INT_EN: double tap interrupt, 1: enable,
    * [2]-ACTIVE_INT_EN_Z: active interrupt for the z axis, 0: disable,
    * [1]-ACTIVE_INT_EN_Y: active interrupt for the y axis, 0: disable,
    * [0]-ACTIVE_INT_EN_X: active interrupt for the x axis, 0: disable,
    */
#if (SENSOR_G_CLICK_SEL == 0 || SENSOR_G_CLICK_SEL == 2)
    0x16,0x20,  //single
#else
    0x16,0x10,  //double
#endif

    /*0x2A
    * [7]-TAP_QUIET: 0: tap quiet duration 30ms,
    * [6]-TAP_SHOCK: 0: tap shock duration 50ms,
    * [2:0]-TAP_DUR[2:0]: 100:250ms
    */
#if (SENSOR_G_CLICK_SEL == 0 || SENSOR_G_CLICK_SEL == 1)
    0x2a,0x04,  //250ms
#else
    0x2a,0x00,  //50ms
#endif

    /*0x2B
    * TAP_TH[4:0]: threshold of tap interrupt.
    * 62.5mg/LSB(2g range)
    * 125mg/LSB(4g range)
    * 250mg/LSB(8g range)
    * 500mg/LSB(16g range)
    */
    0x2b,0x0b,

    /*0x19
    * [6]-INT1_ORIENT: map orientation interrupt to INT1,  0: disable,
    * [5]-INT1_S_TAP: map single tap interrupt to INT1,  0: disable,
    * [4]-INT1_D_TAP: map double tap interrupt to INT1, 1: enable
    * [2]-INT1_ACTIVE: map active interrupt to INT1, 0: disable,
    * [0]-INT1_FREEFALL: map freefall interrupt to INT1, 0: disable,
    */
#if (SENSOR_G_CLICK_SEL == 0 || SENSOR_G_CLICK_SEL == 2)
    0x19,0x20,  //single
#else
    0x19,0x10,  //double
#endif
};

/* 士兰微SC7A20 */
const uint8_t sensor_g_sc7a20_init_tbl[] = {
    //reg,value

    /* odr 200Hz */
    0x20,0x6f,

    /* fds -->开启高通滤波器(滤掉地球G)(一定要开启，否则阈值要超过1G，而且动作也要超过1G) */
    0x21,0x0c,

    /* range bdu  0x20--0xA8 */
    0x23,0x90,

    /* 使能FIFO */
    0x24,0x40,

    /* stream 模式 */
    0x2e,0x80,

    /* selects active level low for pin INT 正常是高电平，有效的时候是低电平 */
    0x25,0x02,

    /* 设定中断阈值(触发阈值)          默认0x28 */
    0x3a,0x2a,

    /* click 有效窗口 */
    0x3b,0x05,

    /* click 中断持续时间 */
    0x3c,0x10,

    /* 使能单击，前一次中断和后一次中断的保持时间(1就是保持1个ODR，2就是2个ODR(比如10HZ，2就是每次中断保持200mS，200mS期间的中断不响应)) */
    0x38,0x15,

    /* 使能敲击中断 */
    0x22,0x80,
};

/* 明皓DA270 */
const uint8_t sensor_g_da270_init_tbl[] = {
    //reg,value

    /*0x0F
    * [3:2]-RESOLUTION[1:0]: resolution of x/y/z axes, 00:14bit,
    * [1:0]-FS[1:0]: acceleration range of x/y/z axes,01:+/-4g,
    */
    0x0f,0x01,

    /*0x11
    * [7]-PWR_MODE:0: normal mode
    * [2:1]-BW[1:0]:10:100Hz
    * [0]-Autosleep_en:0:working the current ODR state all the way
    */
    0x11,0x3c,

    /*0x10
    * [7]- X_AXIS_DIS: 0: enable,
    * [6]- Y_AXIS_DIS: 0: enable,
    * [5]- Z_AXIS_DIS: 0: enable,
    * [3:0]- ODR[3:0]: 1000: 250Hz,
    */
    0x10,0x08,

    /*0x20
    * [1]- INT1_OD: select output for INT1, 0: push-pull,
    * [0]- INT1_LVL: select active level for INT1, 0: low,
    */
    0x20,0x00,

    /*0x21
    * [7]- RESET_INT: reset or not, 0: not reset all latched interrupt,
    * [3:0]- LATCH_INT[3:0]: 0010: tempoary latched 500ms
    */
#if (SENSOR_G_CLICK_SEL == 0 || SENSOR_G_CLICK_SEL == 1)
    0x21,0x02,  //latched 500ms
#else
    0x21,0x00,  //non-latched
#endif

    /*0x16
    * [6]-ORIENT_INT_EN: orient interrupt, 0: disable,
    * [5]-S_TAP_INT_EN: single tap interrupt,  0: disable,
    * [4]-D_TAP_INT_EN: double tap interrupt, 1: enable,
    * [2]-ACTIVE_INT_EN_Z: active interrupt for the z axis, 0: disable,
    * [1]-ACTIVE_INT_EN_Y: active interrupt for the y axis, 0: disable,
    * [0]-ACTIVE_INT_EN_X: active interrupt for the x axis, 0: disable,
    */
#if (SENSOR_G_CLICK_SEL == 0 || SENSOR_G_CLICK_SEL == 2)
    0x16,0x20,  //single
#else
    0x16,0x10,  //double
#endif

    /*0x2A
    * [7]-TAP_QUIET: 0: tap quiet duration 30ms,
    * [6]-TAP_SHOCK: 0: tap shock duration 50ms,
    * [2:0]-TAP_DUR[2:0]: 100:250ms
    */
#if (SENSOR_G_CLICK_SEL == 0 || SENSOR_G_CLICK_SEL == 1)
    0x2a,0x04,  //250ms
#else
    0x2a,0x00,  //50ms
#endif

    /*0x2B
    * TAP_TH[4:0]: threshold of tap interrupt.
    * 62.5mg/LSB(2g range)
    * 125mg/LSB(4g range)
    * 250mg/LSB(8g range)
    * 500mg/LSB(16g range)
    */
    0x2b,0x0d,

    /*0x19
    * [6]-INT1_ORIENT: map orientation interrupt to INT1,  0: disable,
    * [5]-INT1_S_TAP: map single tap interrupt to INT1,  0: disable,
    * [4]-INT1_D_TAP: map double tap interrupt to INT1, 1: enable
    * [2]-INT1_ACTIVE: map active interrupt to INT1, 0: disable,
    * [0]-INT1_FREEFALL: map freefall interrupt to INT1, 0: disable,
    */
#if (SENSOR_G_CLICK_SEL == 0 || SENSOR_G_CLICK_SEL == 2)
    0x19,0x20,  //single
#else
    0x19,0x10,  //double
#endif
};

void sensor_g_init(void)
{
#if SENSOR_TEST_EN
    sensor_g_online_sta = true;
#else

    if (sensor_g_online()) {
        sensor_g_online_sta = true;
    } else {
        sensor_g_online_sta = false;
        return;
    }

    /* int gpio初始化，function设置为EINT，开内部上拉，开下降沿中断 */
    HAL_gpio_pull_set(SENSOR_G_INT_PIN_NUM, SENSOR_G_INT_PIN_PULL);
    HAL_gpio_func_set(SENSOR_G_INT_PIN_NUM, PIN_FUNCEINT);
    HAL_gpio_irq_clk_config(SENSOR_G_INT_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
    HAL_gpio_irq_config(SENSOR_G_INT_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_F_EDGE);

    /* 寄存器初始化 */
#if (SENSOR_G_DEVICE_SEL == 0)      //MSA301
    for (uint8_t i=0;i<sizeof(sensor_g_msa301_init_tbl);i=i+2) {
        sensor_g_write(SENSOR_G_TWI_ADDR, sensor_g_msa301_init_tbl[i], sensor_g_msa301_init_tbl[i+1]);
    }
#elif (SENSOR_G_DEVICE_SEL == 1)    //SC7A20
    for (uint8_t i=0;i<sizeof(sensor_g_sc7a20_init_tbl);i=i+2) {
        sensor_g_write(SENSOR_G_TWI_ADDR, sensor_g_sc7a20_init_tbl[i], sensor_g_sc7a20_init_tbl[i+1]);
    }
#elif (SENSOR_G_DEVICE_SEL == 2)    //DA270
    for (uint8_t i=0;i<sizeof(sensor_g_da270_init_tbl);i=i+2) {
        sensor_g_write(SENSOR_G_TWI_ADDR, sensor_g_da270_init_tbl[i], sensor_g_da270_init_tbl[i+1]);
    }
#elif (SENSOR_G_DEVICE_SEL == 9)
    if (sensor_g_model == 1) {
        for (uint8_t i=0;i<sizeof(sensor_g_sc7a20_init_tbl);i=i+2) {
            sensor_g_write(SENSOR_G_SC7A20_TWI_ADDR, sensor_g_sc7a20_init_tbl[i], sensor_g_sc7a20_init_tbl[i+1]);
        }
    } else if (sensor_g_model == 2) {
        for (uint8_t i=0;i<sizeof(sensor_g_da270_init_tbl);i=i+2) {
            sensor_g_write(SENSOR_G_DA270_TWI_ADDR, sensor_g_da270_init_tbl[i], sensor_g_da270_init_tbl[i+1]);
        }
    }
#endif

#endif

#if (SENSOR_G_CLICK_SEL == 2)
    sensor_g_interval = 0;
    sensor_g_click_cnt = 0;
#endif
}

void sensor_g_deinit(void)
{
    uint8_t reg;
    HAL_gpio_irq_config(SENSOR_G_INT_PIN_NUM, PIN_IRQ_DIS);

#if (SENSOR_G_DEVICE_SEL == 0)      //MSA301
    reg = sensor_g_read(SENSOR_G_TWI_ADDR, SENSOR_G_REG_POWER_MODE);
    reg |= 0xC0;                                    //suspend mode
    sensor_g_write(SENSOR_G_TWI_ADDR, SENSOR_G_REG_POWER_MODE, reg);
#elif (SENSOR_G_DEVICE_SEL == 1)    //SC7A20
#elif (SENSOR_G_DEVICE_SEL == 2)    //DA270
    reg = sensor_g_read(SENSOR_G_TWI_ADDR, SENSOR_G_REG_POWER_MODE);
    reg |= 0x80;                                    //suspend mode
    sensor_g_write(SENSOR_G_TWI_ADDR, SENSOR_G_REG_POWER_MODE, reg);
#elif (SENSOR_G_DEVICE_SEL == 9)
    if (sensor_g_model == 1) {
    } else if (sensor_g_model == 2) {
        reg = sensor_g_read(SENSOR_G_DA270_TWI_ADDR, SENSOR_G_DA270_REG_POWER_MODE);
        reg |= 0x80;                                 //suspend mode
        sensor_g_write(SENSOR_G_DA270_TWI_ADDR, SENSOR_G_DA270_REG_POWER_MODE, reg);
    }
#endif
}

bool sensor_g_online(void)
{
    uint8_t chip_id;
    uint8_t i;

    /* 校验CHIP ID */
#if ((SENSOR_G_DEVICE_SEL == 0) || (SENSOR_G_DEVICE_SEL == 1) || (SENSOR_G_DEVICE_SEL == 2))
    for (i=0;i<3;i++) {
        chip_id = sensor_g_read(SENSOR_G_TWI_ADDR, SENSOR_G_REG_CHIP_ID);
		if (SENSOR_G_CHIP_ID == chip_id) {
            logi("g-sensor is online");
            return true;
        }
        HAL_mdelay(10);
    }
    loge("g-sensor error,chip_id is %x", chip_id);
    return false;

#elif (SENSOR_G_DEVICE_SEL == 9)
        sensor_g_model = 0;

        for (i=0;i<3;i++) {
            chip_id = sensor_g_read(SENSOR_G_SC7A20_TWI_ADDR, SENSOR_G_SC7A20_REG_CHIP_ID);
            //logi("sc7a20 id[%x]", chip_id);
            if (SENSOR_G_SC7A20_CHIP_ID == chip_id) {
                //logi("g-sensor sc7a20 is online");
                sensor_g_model = 1;
                return true;
            }
            HAL_mdelay(10);
        }

        for (i=0;i<3;i++) {
            chip_id = sensor_g_read(SENSOR_G_DA270_TWI_ADDR, SENSOR_G_DA270_REG_CHIP_ID);
            //logi("da270 id[%x]", chip_id);
            if (SENSOR_G_DA270_CHIP_ID == chip_id) {
                //logi("g-sensor da270 is online");
                sensor_g_model = 2;
                return true;
            }
            HAL_mdelay(10);
        }
        return false;
#endif
}

AT_SRAM(.sensor)
void sensor_g_interrupt(void)
{
    if (HAL_gpio_irq_pending_get(SENSOR_G_INT_PIN_NUM)) {
        HAL_gpio_irq_pending_clear(SENSOR_G_INT_PIN_NUM);
        if (sensor_g_online_sta) {
            if (HAL_pmu_vbus_state_get()) {
                //在vbus in中，无效
                logi("sensor g,vbus in,return");
                return;
            }
#if SENSOR_PROXIMITY_EN
            if (sensor_proximity_ps_sta_get() == false) {
                //摘下状态
                logi("sensor g,lt,return");
                return;
            }
#endif
            sensor_g_process();
        }
    }
}

AT_SRAM(.sensor)
void sensor_g_process(void)
{

#if (SENSOR_G_CLICK_SEL == 0)
    /* 单击 */
    event_put(SENSOR_EVENT_G | (1<<8), NULL, 0);
#elif (SENSOR_G_CLICK_SEL == 1)
    /* 双击 */
    event_put(SENSOR_EVENT_G | (2<<8), NULL, 0);
#elif (SENSOR_G_CLICK_SEL == 2)
    /* 多击 */
    sensor_g_interval = SENSOR_G_M_CLICK_INTERVAL;
    sensor_g_click_cnt++;
#endif

#if BT_SNIFF_EN
    if (am_btaudio_bt_calling_state_get() == BT_CALLING_STATE_NULL) {
        /* 通话时无需主动退出sniff，手机会要求退出 */
        bt_sniff_suspend(false);
    }
#endif
}

#if (SENSOR_G_CLICK_SEL == 2)
AT_SRAM(.sensor)
void sensor_g_scan(void)
{
    /* 多击相关 */
    if (sensor_g_interval > 0) {
        sensor_g_interval--;
    }
    if (sensor_g_interval == 1) {
        event_put(SENSOR_EVENT_G | (sensor_g_click_cnt<<8), NULL, 0);
        sensor_g_click_cnt = 0;
    }
}
#endif

void sensor_g_write(uint8_t twi_adr, uint8_t adr, uint8_t value)
{
    uint8_t data[2];

    data[0] = adr;
    data[1] = value;

#if TWI_IO_EN
    twi_io_write(twi_adr, data, 2);
#else
    twi_write(SENSOR_G_TWI_ID, twi_adr, data, 2);
#endif
}

uint8_t sensor_g_read(uint8_t twi_adr, uint8_t adr)
{
    uint8_t value;

#if TWI_IO_EN
    twi_io_write(twi_adr, &adr, 1);
    twi_io_read(twi_adr, &value, 1);
#else
    twi_write(SENSOR_G_TWI_ID, twi_adr, &adr, 1);
    twi_read(SENSOR_G_TWI_ID, twi_adr, &value, 1);
#endif
    return value;
}

#endif
