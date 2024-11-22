#ifndef _SENSOR_MOTION_H_
#define _SENSOR_MOTION_H_

#include "user_config.h"


enum sensor_motion_enable_e {
    SENSOR_MOTION_ENABLE_CMD            = (1<<0),   /* 在cmd要求时置1 */
    SENSOR_MOTION_ENABLE_PLAY           = (1<<1),   /* 在播放音乐时置1 */
    SENSOR_MOTION_ENABLE_STU            = (1<<2),   /* 在入耳姿态检测时置1*/
    SENSOR_MOTION_ENABLE_MASK   = (SENSOR_MOTION_ENABLE_CMD | SENSOR_MOTION_ENABLE_PLAY | SENSOR_MOTION_ENABLE_STU),
};


#define SENSOR_MOTION_TWI_ID                TWI_ID_01
#define SENSOR_MOTION_TWI_ADDR              0x68

#define SENSOR_MOTION_CHIP_ID               0x20

//register
#define SENSOR_MOTION_REG_CHIP_ID           0x75
#define SENSOR_MOTION_REG_ACCEL_DATA        0x1F
#define SENSOR_MOTION_REG_GYRO_DATA         0x25

/* ----------------------------------------------------------------------------
 * Device Register map
 *
 * Next macros defines address for all icm406xx registers as listed by device
 * datasheet.
 * Macros name is MPUREG_<REGISTER_NAME> with REGISTER_NAME being the name of
 * the corresponding register in datasheet.
 * Note that macro name is MPUREG_<REGISTER_NAME>_Bx with x being the bank
 * number for registers that are in bank 1 and 2 (suffix is ommitted for
 * bank0 registers)
 * ---------------------------------------------------------------------------- */

/* Bank 0 */
#define MPUREG_DEVICE_CONFIG      0x11
#define MPUREG_CHIP_CONFIG        MPUREG_DEVICE_CONFIG // Retro-compatibility
#define MPUREG_DRIVE_CONFIG       0x13
#define MPUREG_INT_CONFIG         0x14
#define MPUREG_FIFO_CONFIG        0x16
#define MPUREG_TEMP_DATA0_UI      0x1D
#define MPUREG_ACCEL_DATA_X0_UI   0x1F
#define MPUREG_GYRO_DATA_X0_UI    0x25
#define MPUREG_TMST_FSYNCH        0x2B
#define MPUREG_TMST_FSYNC1        MPUREG_TMST_FSYNCH // Retro-compatibility
#define MPUREG_INT_STATUS         0x2D
#define MPUREG_FIFO_COUNTH        0x2E
#define MPUREG_FIFO_BYTE_COUNT1   MPUREG_FIFO_COUNTH // Retro-compatibility
#define MPUREG_FIFO_COUNTL        0x2F
#define MPUREG_FIFO_BYTE_COUNT2   MPUREG_FIFO_COUNTL // Retro-compatibility
#define MPUREG_FIFO_DATA          0x30
#define MPUREG_APEX_DATA0         0x31
#define MPUREG_APEX_DATA1         0x32
#define MPUREG_APEX_DATA2         0x33
#define MPUREG_APEX_DATA3         0x34
#define MPUREG_APEX_DATA4         0x35
#define MPUREG_APEX_DATA5         0x36
#define MPUREG_INT_STATUS2        0x37
#define MPUREG_INT_STATUS3        0x38
#define MPUREG_SIGNAL_PATH_RESET  0x4B
#define MPUREG_INTF_CONFIG0       0x4C
#define MPUREG_INTF_CONFIG1       0x4D
#define MPUREG_PWR_MGMT_0         0x4E
#define MPUREG_GYRO_CONFIG0       0x4F
#define MPUREG_ACCEL_CONFIG0      0x50
#define MPUREG_GYRO_CONFIG1       0x51
#define MPUREG_ACCEL_GYRO_CONFIG0 0x52
#define MPUREG_ACCEL_CONFIG1      0x53
#define MPUREG_TMST_CONFIG        0x54
#define MPUREG_APEX_CONFIG0       0x56
#define MPUREG_SMD_CONFIG         0x57
#define MPUREG_FIFO_CONFIG1       0x5F
#define MPUREG_FIFO_CONFIG2       0x60
#define MPUREG_FIFO_CONFIG3       0x61
#define MPUREG_FSYNC_CONFIG       0x62
#define MPUREG_INT_CONFIG0        0x63
#define MPUREG_INT_CONFIG1        0x64
#define MPUREG_INT_SOURCE0        0x65
#define MPUREG_INT_SOURCE1        0x66
#define MPUREG_INT_SOURCE2        0x67
#define MPUREG_INT_SOURCE3        0x68
#define MPUREG_INT_SOURCE4        0x69
#define MPUREG_INT_SOURCE5        0x6A
#define MPUREG_FIFO_LOST_PKT0     0x6C
#define MPUREG_SELF_TEST_CONFIG   0x70
#define MPUREG_WHO_AM_I           0x75
#define MPUREG_REG_BANK_SEL       0x76

/* Bank 1 */
#define MPUREG_GYRO_CONFIG_STATIC2_B1 0x0B
#define MPUREG_GYRO_CONFIG_STATIC3_B1 0x0C
#define MPUREG_GYRO_CONFIG_STATIC4_B1 0x0D
#define MPUREG_GYRO_CONFIG_STATIC5_B1 0x0E
#define MPUREG_XG_ST_DATA_B1          0x5F
#define MPUREG_YG_ST_DATA_B1          0x60
#define MPUREG_ZG_ST_DATA_B1          0x61
#define MPUREG_TMST_VAL0_B1           0x62
#define MPUREG_INTF_CONFIG4_B1        0x7A
#define MPUREG_INTF_CONFIG5_B1        0x7B
#define MPUREG_INTF_CONFIG6_B1        0x7C

/* Bank 2 */
#define MPUREG_ACCEL_CONFIG_STATIC2_B2 0x03
#define MPUREG_ACCEL_CONFIG_STATIC3_B2 0x04
#define MPUREG_ACCEL_CONFIG_STATIC4_B2 0x05
#define MPUREG_ACCEL_CONFIG_STATIC0_B2 0x39
#define MPUREG_XA_ST_DATA_B2           0x3B
#define MPUREG_YA_ST_DATA_B2           0x3C
#define MPUREG_ZA_ST_DATA_B2           0x3D
/* Only accessible from AUX1 */
#define MPUREG_OIS1_CONFIG1_B2         0x44
#define MPUREG_OIS1_CONFIG2_B2         0x45
#define MPUREG_OIS1_CONFIG3_B2         0x46
#define MPUREG_ACCEL_DATA_X0_OIS1_B2   0x49
#define MPUREG_GYRO_DATA_X0_OIS1_B2    0x4F
#define MPUREG_INT_STATUS_OIS1_B2      0x57
/* End of Only accessible from AUX1 */
/* Only accessible from AUX2 */
#define MPUREG_OIS2_CONFIG1_B2         0x59
#define MPUREG_OIS2_CONFIG2_B2         0x5A
#define MPUREG_OIS2_CONFIG3_B2         0x5B
#define MPUREG_ACCEL_DATA_X0_OIS2_B2   0x5E
#define MPUREG_GYRO_DATA_X0_OIS2_B2    0x64
#define MPUREG_INT_STATUS_OIS2_B2      0x6C
/* End of Only accessible from AUX2 */

/* Bank 4 */
#define MPUREG_APEX_CONFIG1_B4    0x40
#define MPUREG_APEX_CONFIG2_B4    0x41
#define MPUREG_APEX_CONFIG3_B4    0x42
#define MPUREG_APEX_CONFIG4_B4    0x43
#define MPUREG_APEX_CONFIG5_B4    0x44
#define MPUREG_APEX_CONFIG6_B4    0x45
#define MPUREG_APEX_CONFIG7_B4    0x46
#define MPUREG_APEX_CONFIG8_B4    0x47
#define MPUREG_APEX_CONFIG9_B4    0x48
#define MPUREG_ACCEL_WOM_X_THR_B4 0x4A
#define MPUREG_ACCEL_WOM_Y_THR_B4 0x4B
#define MPUREG_ACCEL_WOM_Z_THR_B4 0x4C
#define MPUREG_INT_SOURCE6_B4     0x4D
#define MPUREG_INT_SOURCE7_B4     0x4E
#define MPUREG_INT_SOURCE8_B4     0x4F
#define MPUREG_INT_SOURCE9_B4     0x50
#define MPUREG_INT_SOURCE10_B4    0x51
#define MPUREG_OFFSET_USER_0_B4   0x77
#define MPUREG_OFFSET_USER_1_B4   0x78
#define MPUREG_OFFSET_USER_2_B4   0x79
#define MPUREG_OFFSET_USER_3_B4   0x7A
#define MPUREG_OFFSET_USER_4_B4   0x7B
#define MPUREG_OFFSET_USER_5_B4   0x7C
#define MPUREG_OFFSET_USER_6_B4   0x7D
#define MPUREG_OFFSET_USER_7_B4   0x7E
#define MPUREG_OFFSET_USER_8_B4   0x7F

#define FIFO_ACCEL_EN           0x40
#define FIFO_GYRO_EN            0x20


void sensor_motion_init(void);
void sensor_motion_deinit(void);
void sensor_motion_interrupt_init(void);
void sensor_motion_interrupt(void);
void sensor_motion_fifo_init(void);
void sensor_motion_fifo_reset(void);
bool sensor_motion_online(void);
void sensor_motion_gyro_get(int16_t *data);
void sensor_motion_accel_get(int16_t *data);
void sensor_motion_enable_proc(void);
void sensor_motion_enable_set(uint8_t enable);
void sensor_motion_enable_clear(uint8_t enable);
void sensor_motion_write(uint8_t adr, uint8_t value);
uint8_t sensor_motion_read(uint8_t adr);
void sensor_motion_active_timer_cb(void* xTimer);
void sensor_motion_active_timer_start(void);
void sensor_motion_active_timer_stop(void);
uint8_t sensor_motion_proc_with_proximity(uint32_t event);
void sensor_motion_test(void);
void motion_process(int16_t* gyro_t, int16_t* accel_t, float alpha_0, float alpha_1, uint32_t delta_us);
int motion_body_stu_get(void);

void* sensor_motion_task(void *para);
void sensor_motion_task_create(void);
void sensor_motion_task_delete(void);

void Motion_Filter_init(float sample_rate, float g_bias[3], float a_bias[3]);
uint8_t bt_sniff_phone_get(void);


#endif

