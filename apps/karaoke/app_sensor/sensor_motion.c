#define LOG_LEV 5
#include "sys_inc.h"
#include "hal.h"
#include "system.h"
#include "tws_profile.h"
#include "air_api.h"
#include "sensor.h"


#define MOTION_USE_SEM  1
#define DIS_GYRO        0

bool sensor_motion_online_sta;

bool volatile sensor_motion_task_exit;
bool sensor_motion_task_deleted;
OAL_THREAD sensor_motion_task_handle;
OAL_TIMER SensorMotionActiveTimerHandle;
uint8_t sensor_motion_active_timer_status;
uint8_t sensor_motion_enable_status;


#if MOTION_USE_SEM
OAL_SEM motion_sem;
#endif

#if SENSOR_MOTION_EN

void sensor_motion_init(void)
{
    twi_clk_set(SENSOR_MOTION_TWI_ID, 1);

    if (!sensor_motion_online()) {
        twi_clk_set(SENSOR_MOTION_TWI_ID, 0);
        return;
    }

    /* reset */
    sensor_motion_write(0x11, 0x01);
    HAL_mdelay(100);

    /* GYRO_FS_SEL:±500dps,
     * GYRO_ODR: 200Hz */
    sensor_motion_write(0x4F, 0x47);

    /* ACCEL_FS_SEL:±2g,
     * ACCEL_ODR:200Hz (LN mode) */
    sensor_motion_write(0x50, 0x67);

    /* ACCEL_UI_FILT_BW:BW=max(400Hz, ODR)/4
     * GYRO_UI_FILT_BW:BW=max(400Hz, ODR)/4 */
    sensor_motion_write(0x52, 0x66);//lowpass
    //sensor_motion_write(0x52, 0x11);//non-lowpass

    /* INT_ASYNC_RESET = 0 */
    sensor_motion_write(0x64, 0x00);

    /* GYRO_MODE:Places gyroscope in Low Noise (LN) Mode
     * ACCEL_MODE:Places accelerometer in Low Noise (LN) Mode */
    sensor_motion_write(0x4E, 0x0F);

    /* [4],RESET_DONE_INT1_EN,1: Reset done interrupt routed to INT1
     * [2],FIFO_THS_INT1_EN,1: FIFO threshold interrupt routed to INT1
     */
    sensor_motion_write(MPUREG_INT_SOURCE0, 0x14);

    /* [2],INT1_MODE,0: Pulsed mode
     * [1],INT1_DRIVE_CIRCUIT,0: Open drain
     * [0],INT1_POLARITY,0: Active low
     */
    sensor_motion_write(MPUREG_INT_CONFIG, 0x00);

    sensor_motion_fifo_init();
    sensor_motion_interrupt_init();

    sensor_motion_task_create();

    twi_clk_set(SENSOR_MOTION_TWI_ID, 0);
}

void sensor_motion_deinit(void)
{
    sensor_motion_task_delete();
    sensor_motion_online_sta = false;
    sensor_motion_active_timer_stop();

    HAL_gpio_irq_config(SENSOR_MOTION_INT_PIN_NUM, PIN_IRQ_DIS);
    HAL_gpio_config(SENSOR_MOTION_INT_PIN_NUM|PIN_FUNCDISABLE|PIN_PULL_DISABLE);
    HAL_gpio_config(SENSOR_MOTION_INT_PIN_NUM|PIN_FUNCDISABLE|PIN_PULL_DISABLE);
}

void sensor_motion_interrupt_init(void)
{
    HAL_gpio_pull_set(SENSOR_MOTION_INT_PIN_NUM, PIN_PULL_UP100K);
    HAL_gpio_func_set(SENSOR_MOTION_INT_PIN_NUM, PIN_FUNCEINT);
    HAL_gpio_irq_clk_config(SENSOR_MOTION_INT_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x01<<4));
    HAL_gpio_irq_config(SENSOR_MOTION_INT_PIN_NUM, PIN_IRQ_DIS);    /* 初始时不开中断，待连接上手机后再开 */
    //HAL_gpio_irq_config(SENSOR_MOTION_INT_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_F_EDGE);

    sensor_motion_enable_status = 0x00;
    sensor_motion_active_timer_status = 0x00;
}

void sensor_motion_interrupt_enable(uint8_t enable)
{
    if (enable) {
        sensor_motion_fifo_reset();
        HAL_gpio_irq_config(SENSOR_MOTION_INT_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_F_EDGE);
    } else {
        HAL_gpio_irq_config(SENSOR_MOTION_INT_PIN_NUM, PIN_IRQ_DIS);
    }
}

void sensor_motion_fifo_init(void)
{
    uint8_t reg_val;

    /* [7:6],FIFO_MODE,01: Stream-to-FIFO Mode
     */
    sensor_motion_write(MPUREG_FIFO_CONFIG, 0x40);

    /* [4],TMST_TO_REGS_EN,1:TMST_VALUE[19:0] read returns timestamp value
     * [3],TMST_RES,0:time stamp resolution is 1 μs.
     * [2],TMST_DELTA_EN,1:the time stamp field contains the measurement of time since the last occurrence of ODR.
     * [0],TMST_EN,Time Stamp register enable
     */
    sensor_motion_write(MPUREG_TMST_CONFIG, 0x35);

    /* [6],FIFO_COUNT_REC,0: FIFO count is reported in bytes
     * [5],FIFO_COUNT_ENDIAN,1: FIFO count is reported in Big Endian format (default)
     * [4],SENSOR_DATA_ENDIAN,1: Sensor data is reported in Big Endian format (default)
     */
    sensor_motion_write(MPUREG_INTF_CONFIG0, 0x30);

    reg_val = sensor_motion_read(MPUREG_INT_SOURCE0);
    sensor_motion_write(MPUREG_INT_SOURCE0, 0x0);       /* workaround 9108 */

    /* FIFO_WM[7:0]
     */
    sensor_motion_write(MPUREG_FIFO_CONFIG2, 0x40);

    /* FIFO_WM[11:8]
     */
    sensor_motion_write(MPUREG_FIFO_CONFIG3, 0x00);

    sensor_motion_write(MPUREG_INT_SOURCE0, reg_val);  /* workaround 9108 */

    /* [5],FIFO_WM_GT_TH,1:Trigger FIFO watermark interrupt on every ODR (DMA write) if FIFO_COUNT ≥ FIFO_WM_TH
     * [3],FIFO_TMST_FSYNC_EN,Must be set to 1 for all FIFO use cases when FSYNC is used
     * [2],FIFO_TEMP_EN,Enable temperature sensor packets to go to FIFO
     * [1],FIFO_GYRO_EN,Enable gyroscope packets to go to FIFO
     * [0],FIFO_ACCEL_EN,Enable accelerometer packets to go to FIFO
     */
    sensor_motion_write(MPUREG_FIFO_CONFIG1, 0x2f);

    /* [6],INT_TPULSE_DURATION,0: Interrupt pulse duration is 100μs. Use only if ODR < 4kHz.
     * [5],INT_TDEASSERT_DISABLE,0: The interrupt de-assertion duration is set to a minimum of 100μs. Use only if ODR < 4kHz.
     * [4],INT_ASYNC_RESET,User should change setting to 0 from default setting of 1, for proper INT1 and INT2 pin operation
     */
    sensor_motion_write(MPUREG_INT_CONFIG1, 0x00);      /* Extra FIFO config,for A1 */
}

void sensor_motion_fifo_reset(void)
{
    twi_clk_set(SENSOR_MOTION_TWI_ID, 1);
    /* [1],FIFO_FLUSH,When set to 1, FIFO will get flushed. */
    sensor_motion_write(MPUREG_SIGNAL_PATH_RESET, 0x02);
    twi_clk_set(SENSOR_MOTION_TWI_ID, 0);
}
bool sensor_motion_online(void)
{
    uint8_t chip_id;

    /* 校验CHIP ID */
    for (uint32_t i=0;i<3;i++) {
        chip_id = sensor_motion_read(SENSOR_MOTION_REG_CHIP_ID);
        if (SENSOR_MOTION_CHIP_ID == chip_id) {
            logi("m-sensor is online");
            sensor_motion_online_sta = true;
            return sensor_motion_online_sta;
        }
        HAL_mdelay(100);
    }
    loge("m-sensor error,chip_id should be 0x20,but is %x", chip_id);
    sensor_motion_online_sta = false;
    //sensor_motion_online_sta = true;    //test
    return sensor_motion_online_sta;
}

AT_SRAM(.motion_proc)
bool sensor_motion_online_get(void)
{
    return sensor_motion_online_sta;
}

AT_SRAM(.motion_proc)
void sensor_motion_gyro_get(int16_t *data)
{
    uint8_t buf[6];
    uint8_t adr;

    if (!sensor_motion_online_get()) {
        return;
    }

    twi_clk_set(SENSOR_MOTION_TWI_ID, 1);
    adr = SENSOR_MOTION_REG_GYRO_DATA;
    twi_write(SENSOR_MOTION_TWI_ID, SENSOR_MOTION_TWI_ADDR, &adr, 1);
    twi_read(SENSOR_MOTION_TWI_ID, SENSOR_MOTION_TWI_ADDR, buf, 6);
    twi_clk_set(SENSOR_MOTION_TWI_ID, 0);

    data[0] = (((int16_t)buf[0]) << 8) | (int16_t)buf[1];
    data[1] = (((int16_t)buf[2]) << 8) | (int16_t)buf[3];
    data[2] = (((int16_t)buf[4]) << 8) | (int16_t)buf[5];
}

AT_SRAM(.motion_proc)
void sensor_motion_accel_get(int16_t *data)
{
    uint8_t buf[6];
    uint8_t adr;

    if (!sensor_motion_online_get()) {
        return;
    }

    twi_clk_set(SENSOR_MOTION_TWI_ID, 1);
    adr = SENSOR_MOTION_REG_ACCEL_DATA;
    twi_write(SENSOR_MOTION_TWI_ID, SENSOR_MOTION_TWI_ADDR, &adr, 1);
    twi_read(SENSOR_MOTION_TWI_ID, SENSOR_MOTION_TWI_ADDR, buf, 6);
    twi_clk_set(SENSOR_MOTION_TWI_ID, 0);

    data[0] = (((int16_t)buf[0]) << 8) | (int16_t)buf[1];
    data[1] = (((int16_t)buf[2]) << 8) | (int16_t)buf[3];
    data[2] = (((int16_t)buf[4]) << 8) | (int16_t)buf[5];
}

void sensor_motion_fifo_data_pro(short *gyr_data, short *acc_data, uint16_t *timestamp)
{
    uint8_t fifo_data[16];
    uint8_t adr;

    adr = MPUREG_FIFO_DATA;
    twi_write(SENSOR_MOTION_TWI_ID, SENSOR_MOTION_TWI_ADDR, &adr, 1);
    twi_read(SENSOR_MOTION_TWI_ID, SENSOR_MOTION_TWI_ADDR, fifo_data, 16);

    *timestamp = ((int16_t)fifo_data[14] << 8) | (int16_t)fifo_data[15];

    if (fifo_data[0] & FIFO_ACCEL_EN) {
        acc_data[0] = (int16_t)((fifo_data[1] << 8) | fifo_data[2]);
        acc_data[1] = (int16_t)((fifo_data[3] << 8) | fifo_data[4]);
        acc_data[2] = (int16_t)((fifo_data[5] << 8) | fifo_data[6]);
    }
    if (fifo_data[0] & FIFO_GYRO_EN) {
        gyr_data[0] = ((int16_t)fifo_data[7] << 8) | (int16_t)fifo_data[8];
        gyr_data[1] = ((int16_t)fifo_data[9] << 8) | (int16_t)fifo_data[10];
        gyr_data[2] = ((int16_t)fifo_data[11] << 8) | (int16_t)fifo_data[12];
    }

    /*
    static uint8_t temp_last;
    if (fifo_data[13] != 0x80) {
        *temp = fifo_data[13];
        temp_last = fifo_data[13];
    } else {
        *temp = temp_last;  //0x80 is invalid code for 1-byte temperature, use the last valid one
    }
    */
}


AT_SRAM(.motion_proc)
void sensor_motion_fifo_get(void)
{
    uint8_t adr;
    uint8_t data;
    uint16_t count_l,count_h;
    uint16_t packet_size;
    short gyro[3], accel[3];
    uint16_t ts;

    if (!sensor_motion_online_get()) {
        return;
    }

    twi_clk_set(SENSOR_MOTION_TWI_ID, 1);

    uint8_t reg_val = sensor_motion_read(MPUREG_INT_SOURCE0);
    sensor_motion_write(MPUREG_INT_SOURCE0, 0x0);

    adr = MPUREG_FIFO_COUNTL;
    twi_write(SENSOR_MOTION_TWI_ID, SENSOR_MOTION_TWI_ADDR, &adr, 1);
    twi_read(SENSOR_MOTION_TWI_ID, SENSOR_MOTION_TWI_ADDR, &data, 1);
    count_l = data;

    adr = MPUREG_FIFO_COUNTH;
    twi_write(SENSOR_MOTION_TWI_ID, SENSOR_MOTION_TWI_ADDR, &adr, 1);
    twi_read(SENSOR_MOTION_TWI_ID, SENSOR_MOTION_TWI_ADDR, &data, 1);
    count_h = data;

    //logi("fifo count:%d", (count_h<<8) + count_l);
    packet_size = ((count_h<<8) + count_l) / 16;
    if (packet_size > 4) {
        logi("motion packet size:%d", packet_size);
    }

    if (packet_size > 10) {
        sensor_motion_fifo_reset();
    } else {
        while (packet_size > 0) {
            sensor_motion_fifo_data_pro(gyro, accel, &ts);
#if DIS_GYRO
            motion_process(NULL, accel, 0.1f, 0.1f, 5000);
#else
            motion_process(gyro, accel, 0.1f, 0.1f, 5000);
#endif
            //logi("a:%d,%d,%d,%d", accel[0],accel[1],accel[2],ts);
            packet_size--;
        }
    }

    sensor_motion_write(MPUREG_INT_SOURCE0, reg_val);

    twi_clk_set(SENSOR_MOTION_TWI_ID, 0);
}

AT_SRAM(.motion_proc)
uint8_t sensor_motion_int_status_get(void)
{
    uint8_t data;

    twi_clk_set(SENSOR_MOTION_TWI_ID, 1);
    data = sensor_motion_read(MPUREG_INT_STATUS);
    twi_clk_set(SENSOR_MOTION_TWI_ID, 0);

    return data;
}

AT_SRAM(.motion_proc)
void sensor_motion_interrupt(void)
{
    if (HAL_gpio_irq_pending_get(SENSOR_MOTION_INT_PIN_NUM)) {
        HAL_gpio_irq_pending_clear(SENSOR_MOTION_INT_PIN_NUM);
        //logx("m^");

#if MOTION_USE_SEM
        if (NULL != motion_sem) {
            OAL_sem_post(&motion_sem);
        }
#else
        if (NULL != sensor_motion_task_handle) {
            OAL_thread_notify(sensor_motion_task_handle, 0, 0);
        }
#endif
    }
}

void sensor_motion_enable_proc(void)
{
    static uint8_t enable_last = 0;
    uint8_t enable_cur;
    uint8_t enable_tmp;

    if (tws_role_get() != TWS_SLAVE) {
        if (tws_peer_con_status_get()) {
            if (tws_phone_con_status_get()) {
                if (bt_sniff_phone_get() == 0) {
                #if BT_AIR_SEL
                    if (air_param_phone_type_get()) {
                        enable_tmp = 1;
                    } else {
                        /* android */
                        enable_tmp = 0;
                    }
                #else
                    enable_tmp = 1;
                #endif
                } else {
                    /* sniff中 */
                    enable_tmp = 0;
                }
            } else {
                /* 未连接手机 */
                enable_tmp = 0;
            }
        } else {
            /* 对耳未连接 */
            if (sensor_motion_enable_status & SENSOR_MOTION_ENABLE_STU) {
                /* 入耳姿态检测 */
                enable_tmp = 1;
            } else {
                enable_tmp = 0;
            }
        }
    } else {
#if SPACE_AUDIO_EN
        enable_tmp = 1;
#else
        /* 副 */
        if (sensor_motion_enable_status & SENSOR_MOTION_ENABLE_STU) {
            /* 副耳上入耳姿态检测 */
            enable_tmp = 1;
        } else {
            enable_tmp = 0;
        }
#endif
    }

    if (enable_tmp) {
        if (sensor_motion_enable_status & SENSOR_MOTION_ENABLE_MASK) {
            enable_cur = 1;
        } else {
            enable_cur = 0;
        }
    } else {
        enable_cur = 0;
    }

    logi("sensor m en:%x,%x", enable_cur, sensor_motion_enable_status);
    if (enable_cur != enable_last) {
        if (enable_cur == 0) {
            sensor_motion_enable_status = 0;
        }
        sensor_motion_interrupt_enable(enable_cur);
        enable_last = enable_cur;
    }
}

void sensor_motion_enable_set(uint8_t enable)
{
    sensor_motion_enable_status |= enable;
    sensor_motion_enable_proc();
}

void sensor_motion_enable_clear(uint8_t enable)
{
    sensor_motion_enable_status &= ~enable;
    sensor_motion_enable_proc();
}

void sensor_motion_write(uint8_t adr, uint8_t value)
{
    uint8_t data[2];

    data[0] = adr;
    data[1] = value;
    twi_write(SENSOR_MOTION_TWI_ID, SENSOR_MOTION_TWI_ADDR, data, 2);
}

uint8_t sensor_motion_read(uint8_t adr)
{
    uint8_t value;

    twi_write(SENSOR_MOTION_TWI_ID, SENSOR_MOTION_TWI_ADDR, &adr, 1);
    twi_read(SENSOR_MOTION_TWI_ID, SENSOR_MOTION_TWI_ADDR, &value, 1);

    return value;
}

void sensor_motion_active_timer_cb(void* xTimer)
{
    if (sensor_motion_active_timer_status == 0x01) {
        sensor_motion_active_timer_status = 0x02;
    }
    if (motion_body_stu_get()) {
        event_unique_put(SENSOR_EVENT_P_HT, NULL, 0);
    }
}

void sensor_motion_active_timer_start(void)
{
    if (NULL == SensorMotionActiveTimerHandle) {
        OAL_timer_create(&SensorMotionActiveTimerHandle, "sensor motion active timer", OAL_ms_to_ticks(100),
            sensor_motion_active_timer_cb, (void *)1, 1, OAL_TIMER_AUTO_ACTIVATE);
    } else {
        OAL_timer_activate(&SensorMotionActiveTimerHandle, OAL_TIMER_AUTO_ACTIVATE);
    }

    sensor_motion_active_timer_status = 0x01;
}

void sensor_motion_active_timer_stop(void)
{
    if (NULL != SensorMotionActiveTimerHandle) {
        OAL_timer_activate(&SensorMotionActiveTimerHandle, OAL_TIMER_NO_ACTIVATE);
    }
}

uint8_t sensor_motion_proc_with_proximity(uint32_t event)
{
    uint8_t ret = 0;

    switch (event) {
    case SENSOR_EVENT_P_HT:
        if (pmu_ear_in_case_state_get()) {
            logi("vbus in,break");
            ret = 0;
            break;
        }

        if (sensor_motion_active_timer_status == 0x00) {
            /* 未启动timer */
            sensor_motion_enable_set(SENSOR_MOTION_ENABLE_STU);
            sensor_motion_active_timer_start();
            ret = 1;
        } else if (sensor_motion_active_timer_status == 0x01) {
            /* 已启动timer，时间未到 */
            ret = 1;
        } else if (sensor_motion_active_timer_status == 0x02) {
            /* 已启动timer，时间已到 */
            if (motion_body_stu_get()) {
                logi("stu ok");
                sensor_motion_enable_clear(SENSOR_MOTION_ENABLE_STU);
                sensor_motion_active_timer_stop();
                sensor_motion_active_timer_status = 0x00;
                ret = 0;
            } else {
                logi("stu fail");
                ret = 1;
            }
        }
        break;

    case SENSOR_EVENT_P_LT:
        sensor_motion_enable_clear(SENSOR_MOTION_ENABLE_STU);
        sensor_motion_active_timer_stop();
        sensor_motion_active_timer_status = 0x00;
        ret = 0;
        break;

    default:
        ret = 0;
    }

    return ret;
}

void sensor_motion_test(void)
{
    int16_t read_buf[3];

    sensor_motion_gyro_get(read_buf);
    logi("gyro:%d, %d, %d", read_buf[0],read_buf[1],read_buf[2]);

    sensor_motion_accel_get(read_buf);
    logi("accel:%d, %d, %d", read_buf[0],read_buf[1],read_buf[2]);

    logi("\r");
}

AT_SRAM(.motion_proc)
void* sensor_motion_task(void *para)
{
    __maybe_unused uint8_t data;
    float g_bias[3];
    float a_bias[3];
    for (uint8_t i=0;i<3;i++) {
        g_bias[i]=0;
        a_bias[i]=0;
    }

    Motion_Filter_init(200.0f, g_bias, a_bias);

    while (1) {
        if (sensor_motion_task_exit) {
            break;
        }
#if MOTION_USE_SEM
        OAL_sem_wait(&motion_sem);
#else
        OAL_thread_notify_wait(0x0, 0xffffffff, NULL, OAL_WAIT_FOREVER);
#endif
        if (sensor_motion_task_exit) {
            break;
        }

        data = sensor_motion_int_status_get();
        //logi("int status:%x", data);
        if (data & (0x01<<2)) {
            /* the FIFO buffer reaches the threshold value. */
            sensor_motion_fifo_get();
        }

        if(OAL_SUCCESS == OAL_thread_wait_join(sensor_motion_task_handle, 0)) {
           break;
        }
    }

    sensor_motion_task_deleted = true;
    OAL_thread_delete_self(sensor_motion_task_handle);
#if MOTION_USE_SEM
    if (NULL != motion_sem) {
        OAL_sem_destroy(&motion_sem);
        motion_sem = NULL;
    }
#endif

    return NULL;
}

void sensor_motion_task_create(void)
{
    sensor_motion_task_exit = false;
    sensor_motion_task_deleted = false;
#if MOTION_USE_SEM
    motion_sem = OAL_sem_create_binary();
#endif
    sensor_motion_task_handle = OAL_thread_create(sensor_motion_task, NULL, "task motion", 1, 256);
}

void sensor_motion_task_delete(void)
{
    if (NULL != sensor_motion_task_handle) {
        sensor_motion_task_exit = true;
#if MOTION_USE_SEM
        if (NULL != motion_sem) {
            OAL_sem_post(&motion_sem);
        }
#else
        OAL_thread_notify(sensor_motion_task_handle, 0, 0);
#endif
        while (!sensor_motion_task_deleted) {
            OAL_sleep(5);
        }
    }
    /* wait idle task free TCB */
    wait_idle_running_a_period(10000);
    sensor_motion_task_handle = NULL;
}
#endif
