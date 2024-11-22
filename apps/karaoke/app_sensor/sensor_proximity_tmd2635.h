#ifndef _TMD2635_HAL_H_
#define _TMD2635_HAL_H_


#define NRF_SUCCESS 1
#define SENSOR_TMD2635_TWI_ID        TWI_ID_01

/* Register definitions */
#define TMD2635_ENABLE_REG            0x80 /* bit3 WEN, bit2 PEN, bit1 AEN, bit0 PON */
#define TMD2635_PRATE_REG             0x82 /* 88us */
#define TMD2635_PWTIME_REG            0xA9 /* 2.78ms, and 2.8ms*12 if WLONG */

#define TMD2635_PILTL_REG             0x88
#define TMD2635_PILTH_REG             0x89
#define TMD2635_PIHTL_REG             0x8A
#define TMD2635_PIHTH_REG             0x8B

#define TMD2635_PERS_REG              0x8C /* bit7:4 PPERS, bit3:0 APERS */
#define TMD2635_CFG0_REG              0x8D /* bit7:3 must be set to 01000b, bit2 WLONG, bit1 reserved, bit0 PPULSE_LEN_16x */
#define	TMD2635_PCFG0_REG             0x8E /* bit7:6 PULSE LEN, bit5:0 PPLUSES */
#define	TMD2635_PCFG1_REG             0x8F /* bit7:6 PGAIN, bit4:0 PLDRIVE 2mA, 28mA Max */

#define	TMD2635_REVID_REG             0x91
#define	TMD2635_ID_REG                0x92
#define	TMD2635_STATUS_REG            0x9B /* PHIGH|PLOW|PSAT|PINT|CINT|ZINT|PSAT_RELECTIVE|PSAT_AMBIENT */

#define	TMD2635_PDATAL_REG            0x9C
#define	TMD2635_PDATAH_REG            0x9D
#define TMD2635_REVID2_REG            0x9E
#define	TMD2635_CFG2_REG              0x9F
#define TMD2635_SOFTRST_REG           0xA8 /* bit0 SOFTRST */
#define	TMD2635_CFG8_REG              0xAA /* PD select */
#define	TMD2635_CFG3_REG              0xAB /* bit7 INT_READ_CLEAR, bit6:5 must be set to 00b, bit4 SAI, bit3:0 must be set to 0100b */
#define TMD2635_CFG4_REG              0xAC /* bit7:0 must be set to 0x3D */
#define TMD2635_CFG6_REG              0xAE /* bit6 APC Disable, bit5:0 must be set to 111111b */
#define	TMD2635_POFFSETL_REG          0xC0
#define	TMD2635_POFFSETH_REG          0xC1
#define	TMD2635_CALIB_REG             0xD7 /* bit5 ELEC_CALIB, bit0 START_CALIB */
#define	TMD2635_CALIBCFG_REG          0xD9 /* bit7:5 BINSRCH_TARGET, bit4 must be set to 1b, bit3 AUTO_OFFSET_ADJ, bit2:0 PROX_AVG */
#define	TMD2635_CALIBSTAT_REG         0xDC /* bit2 OFFSET_ADJUSTED, bit0 CALIB_FINISHED */
#define	TMD2635_INTENAB_REG           0xDD /* ASIEN|PSIEN|PIEN|AIEN|CIEN|ZIEN|RESERVE|RESERVE */
#define	TMD2635_FAC_L_REG             0xE5
#define	TMD2635_FAC_H_REG             0xE6
#define TMD2635_TEST3_REG             0xF9 /* Must be set to 0xC4 */

#define	PON                           (0x01 << 0)
#define	AEN                           (0x01 << 1)
#define	PEN_TMD                           (0x01 << 2)
#define	WEN                           (0x01 << 3)
#define	PWEN                          (0x01 << 4)
#define	PWDIS                         (0x01 << 0)


//TMD2635_ENABLE_REG @0x80
//#define	PON										(0x01 << 0)
//#define	PEN										(0x01 << 2)
//#define	PWEN									(0x01 << 4)

//TMD2635_PERS_REG @0x8C
#define	PPERS_MASK                    (0x0F << 0)

//TMD2635_CFG0_REG @0x8D
//#define PPULSE_LEN_16X                (0x01 << 0)
#define PWLONG                        (0x01 << 3)
#define CFG0_RESERVED                 (0x01 << 6)

//TMD2635_PCFG0_REG @0x8E
#define PPULSE_MASK                   (0x3F << 0)
#define PGAIN_MASK                    (0x03 << 6)
#define PGAIN_1X                      (0x00 << 6)
#define PGAIN_2X                      (0x01 << 6)
#define PGAIN_4X                      (0x02 << 6)
#define PGAIN_8X                      (0x03 << 6)

//TMD2635_PCFG1_REG @0x8F
#define PLDRIVE_MASK                  (0x0F << 0)
#define PPULSE_LEN_MASK               (0x07 << 5)
#define PPLEN_1US                     (0x00 << 5)
#define PPLEN_2US                     (0x01 << 5)
#define PPLEN_4US                     (0x02 << 5)
#define PPLEN_8US                     (0x03 << 5)
#define PPLEN_12US                    (0x04 << 5)
#define PPLEN_16US                    (0x05 << 5)
#define PPLEN_24US                    (0x06 << 5)
#define PPLEN_32US                    (0x07 << 5)

//TMD2635_REVID_REG @0x91
#define REV_ID_MASK                   (0x07 << 0)

//TMD2635_ID_REG @0x92
#define ID_MASK                       (0x3F << 2)

//TMD2635_STATUS_REG @0x9B
#define PSAT_AMBIENT                  (0x01 << 0)
#define PSAT_REFLECTIVE               (0x01 << 1)
#define ZINT                          (0x01 << 2)
#define CINT                          (0x01 << 3)
#define PINT                          (0x01 << 4)
#define PSAT                          (0x01 << 5)
#define PLOW                          (0x01 << 6)
#define PHIGH                         (0x01 << 7)

//TMD2635_REVID2_REG @0xA6
#define REV_ID2_MASK                  (0x0F << 0)

//TMD2635_SOFTRST_REG @0xA8
#define SOFTRST                       (0x01 << 0)

//TMD2635_CFG8_REG @0xAA
#define PDSELECT_MASK                 (0x03 << 0)
#define PD_NO                         (0x00 << 0)
#define PD_FAR                        (0x01 << 0)
#define PD_NEAR                       (0x02 << 0)
#define PD_BOTH                       (0x03 << 0)

//TMD2635_CFG3_REG @0xAB
#define CFG3_RESERVED1                (0x04 << 0)
#define SAI                           (0x01 << 4)
#define CFG3_RESERVED2                (0x00 << 5)
#define INT_READ_CLEAR                (0x01 << 7)

//TMD2635_CFG4_REG @0xAC
#define CFG4_RESERVED                 (0x3D << 0)

//TMD2635_CFG6_REG @0xAE
#define CFG6_RESERVED                 (0x3F << 0)
#define APC_DISABLE                   (0x01 << 6)

//TMD2635_FILTER_REG @0xB3
#define PMAVG_MASK                    (0x03 << 0)
#define PMAVG_DIS                     (0x00 << 0)
#define PMAVG_2                       (0x01 << 0)
#define PMAVG_4                       (0x02 << 0)
#define PMAVG_8                       (0x03 << 0)

//TMD2635_CALIB_REG @0xD7
#define START_OFFSET_CALIB            (0x01 << 0)
#define ELECTRICAL_CALIBRATION        (0x01 << 5)

//TMD2635_CALIBCFG_REG @0xD9
#define PROX_AVG_MASK                 (0x07 << 0)
#define PROX_DATA_AVG_DISABLE         (0x00 << 0)
#define PROX_DATA_AVG_2               (0x01 << 0)
#define PROX_DATA_AVG_4               (0x02 << 0)
#define PROX_DATA_AVG_8               (0x03 << 0)
#define PROX_DATA_AVG_16              (0x04 << 0)
#define PROX_DATA_AVG_32              (0x05 << 0)
#define PROX_DATA_AVG_64              (0x06 << 0)
#define PROX_DATA_AVG_128             (0x07 << 0)
#define AUTO_OFFSET_ADJ               (0x01 << 3)
#define CALIBCFG_RESERVED             (0x01 << 4)
#define BINSRCH_TARGET_MASK           (0x07 << 5)
#define BINSRCH_TARGET_3              (0x00 << 5)
#define BINSRCH_TARGET_7              (0x01 << 5)
#define BINSRCH_TARGET_15             (0x02 << 5)
#define BINSRCH_TARGET_31             (0x03 << 5)
#define BINSRCH_TARGET_63             (0x04 << 5)
#define BINSRCH_TARGET_127            (0x05 << 5)
#define BINSRCH_TARGET_255            (0x06 << 5)
#define BINSRCH_TARGET_511            (0x07 << 5)

//TMD2635_CALIBSTAT_REG @0xDC
#define CALIB_FINISHED                (0x01 << 0)

//TMD2635_INTENAB_REG @0xDD
#define ZIEN                          (0x01 << 1)
#define CIEN                          (0x01 << 2)
#define PSIEN                         (0x01 << 3)
#define PIEN_TMD                          (0x01 << 4)
#define PIM                           (0x01 << 5)

//Configration calculations
#define ATIME_PER_STEP_X100           278
#define ATIME_MS(ms)                  (uint8_t)(((uint32_t)ms*100 + ((uint32_t)ATIME_PER_STEP_X100 >> 1))/(uint32_t)ATIME_PER_STEP_X100 - 1)
#define PTIME_PER_STEP                88
#define PTIME_US(us)                  (uint8_t)(((uint32_t)us + ((uint32_t)PTIME_PER_STEP >> 1))/PTIME_PER_STEP - 1)
#define PWTIME_PER_STEP_X100           278
#define PWTIME_MS(ms)                  (uint8_t)(((uint32_t)ms*100 + ((uint32_t)PWTIME_PER_STEP_X100 >> 1))/(uint32_t)PWTIME_PER_STEP_X100 - 1)
#define PROX_PERSIST(p)               (uint8_t)((p & 0x0F) << 0)
#define PPULSES(c)                    (uint8_t)(c - 1)
#define PLDRIVE_MA(ma)                (uint8_t)((ma-2))

/* For ALS/Color auto gain setting */
#define MAX_ALS_VALUE                 0xFFFF
#define	MIN_ALS_VALUE                 3
#define	GAIN_SWITCH_LEVEL             200

#define PROXIMITY_EVENT_TYPE_NEAR     0
#define PROXIMITY_EVENT_TYPE_FAR      1

/* Proximity thresholds */
#define AMS_PROX_THRESH_NEAR          200  /* unit of ADC count */
#define AMS_PROX_THRESH_FAR           100  /* unit of ADC count */
#define AMS_PROX_THRESH_VERY_NEAR     200 /* unit of ADC count */
#define AMS_PROX_THRESH_CONTAMINATED  100 /* unit of ADC count */
#define AMS_PROX_MAX_VALUE            255

/* Max offset limitation, should <= 150 */
#define TMD2635_MAX_OFFSET            30

/* Time */
#define TMD2635_OFF_TO_IDLE_MS        10  //ms
#define TMD2635_POLLING_MS            100
#define TMD2635_DELAY_AFTER_PON       1
#define TMD2635_MAX_CALIB_TIME        30

#define TMD2635_WHOAMI_VALUE          0x10
#define TMD2635_PARTNO_MASK           0xFC

/* Proximity parameters */
#define TMD2635_PROX_MAX_VALUE        255
#define TMD2635_VERY_NEAR_THRESHOLD   250
#define TMD2635_THRESH_CONTAMINATED   150
#define	TMD2635_CONTAMINATED_COUNT    16
#define TMD2635_VERY_NEAR_COUNT       4
#define	TMD2635_DATA_JITTER_TH        3

/* If greater than this value, we think it is unreasonable */
#define TMD2635_MAX_POFFSET           240

///////////////////////////////////////////////////
typedef struct tmd2635_reg_setting {
	uint8_t reg;
	uint8_t value;
} tmd2635_reg_setting;

typedef enum {
 	 TMD2635_ALS           = 0x01,
	 TMD2635_PROX		  = 0x02,
} tmd2635_sensor_type;

typedef struct tmd2635_prox_info
{
	uint16_t                raw;
	uint8_t                 nearby;
} tmd2635_prox_info;

/** Private state. */
typedef struct tmd2635_instance_state
{
	/* Sensor to be published */
    uint8_t                 address;
	uint8_t                 publish_sensors;

	/* Proximity HW config details*/
	tmd2635_prox_info       prox_info;

	bool                    instance_is_ready_to_configure;
	bool                    timer_is_active;
	bool                    is_dri;
	bool                    first_prox;
	bool                    prox_to_report;

	uint8_t					near_threshold;
	uint8_t					far_threshold;
	bool                    is_prox_dri;
} tmd2635_instance_state;

/******************* Function Declarations ***********************************/

/**
 * Gets Who-Am-I register for the sensor.
 *
 * @param[i] scp_service   handle to synch COM port service
 * @param[i] port_handle   handle to synch COM port
 * @param[o] buffer        who am I value read from HW
 *
 * @return uint32_t
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
uint32_t tmd2635_get_who_am_i(tmd2635_instance_state *state, uint8_t *buffer);


/**
 * Reset TMD2635 sensor to default.
 *
 * @param[i] state         state of sensor instance
 * @param[i] sensor        bit mask for sensors to handle
 *
 * @return uint32_t
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 * @return none
 */
uint32_t tmd2635_reset_device(tmd2635_instance_state *state,
                                    tmd2635_sensor_type sensor);

/**
 * Modify TMD2635 ENABLE register.
 *
 * @param[i] state         state of sensor instance
 * @param[i] mask          mask of the bits which will be modified
 * @param[i] val           the value to be written into the mask bits
 *
 * @return none
 */
uint32_t tmd2635_modify_enable(tmd2635_instance_state *state,
                                     uint8_t mask,
                                     uint8_t val);

/**
 * Modify TMD2635 INTENABLE register.
 *
 * @param[i] state         state of sensor instance
 * @param[i] mask          mask of the bits which will be modified
 * @param[i] val           the value to be written into the mask bits
 *
 * @return none
 */
uint32_t tmd2635_modify_intenab(tmd2635_instance_state *state,
                                      uint8_t mask,
                                      uint8_t val);

/**
 * TMD2635 config start calibration.
 *
 * @param[i] state         state of sensor instance
 *
 * @return none
 */
uint32_t tmd2635_start_calibration(tmd2635_instance_state *state);

/**
 * Get proximity interrupt persistence from register.
 *
 * @param[i] state         state of sensor instance
 * @param[i] persistence   store the interrupt persistence of proximity sensor
 *
 * @return none
 */
uint32_t tmd2635_get_prox_pers(tmd2635_instance_state *state, uint8_t *persistence);

/**
 * Write proximity interrupt persistence setting to register.
 *
 * @param[i] state         state of sensor instance
 * @param[i] persistence   interrupt persistence for proximity sensor
 *
 * @return none
 */
uint32_t tmd2635_set_prox_pers(tmd2635_instance_state *state, uint8_t persistence);

/**
 * Update Proximity interrupt threshold registers.
 *
 * @param[i] state                 Instance state
 * @param[i] high_thresh           High threshold
 * @param[i] low_thresh            Low threshold
 *
 * @return none
 */
uint32_t tmd2635_update_prox_threshold(tmd2635_instance_state *state,
                                                uint8_t high_thresh, uint8_t low_thresh);

/**
 * Procee sensor data.
 *
 * @param[i] instance   Sensor Instance
 */
void tmd2635_process_sensor_data(tmd2635_instance_state *state);

/**
 * TMD2635 proximity do factory calibration.
 *
 * @param[i] instance   Sensor Instance
 *
 * @return SNS_RC_SUCCESS if success
 */
uint32_t tmd2635_prox_factory_calibration(tmd2635_instance_state *state);


void tmd2635_set_config_param(tmd2635_instance_state *state);
void tmd2635_prox_enable(tmd2635_instance_state *state, bool dri);
void tmd2635_get_prox_raw(tmd2635_instance_state *state);
//uint32_t tmd2635_device_set_default(tmd2635_instance_state *state);

uint32_t tmd2635_device_set_default(tmd2635_instance_state *state);
void tmd2635_get_datat_cb(void);
void tmd2635_sensor_init(void);
bool tmd2635_sensor_online(void);
void tmd2635_sensor_deinit(void);
void tmd2635_start(void);
void tmd2635_suspend(void);
void tmd2635_resume(void);



#define TMD2635_REG_ENABLE						(0x80)
#define TMD2635_REG_PRATE_PROX_TIME				(0x82)
#define TMD2635_REG_PILTL_LOW_THRESHOLD_LOW		(0x88)
#define TMD2635_REG_PILTH_LOW_THRESHOLD_HIGH	(0x89)
#define TMD2635_REG_PIHTL_HIGH_THRESHOLD_LOW	(0x8A)
#define TMD2635_REG_PIHTH_HIGH_THRESHOLD_HIGH	(0x8B)
#define TMD2635_REG_PERS_INT_FILTERS			(0x8C)
#define TMD2635_REG_CFG0_CONFIG0				(0x8D)
#define TMD2635_REG_PCFG0_PROX_CONFIG0			(0x8E)
#define TMD2635_REG_PCFG1_PROX_CONFIG1			(0x8F)

#define TMD2635_REG_REVID_REVISION_ID			(0x91)
#define TMD2635_REG_ID_DEVICE_ID				(0x92)
#define TMD2635_REG_STATUS_DEVICE_STATUS		(0x9B)
#define TMD2635_REG_PDATAL_PROX_ADC_LOW			(0x9C)
#define TMD2635_REG_PDATAH_PROX_ADC_HIGH		(0x9D)

#define TMD2635_REG_REVID2_REVISION_ID2			(0xA6)
#define TMD2635_REG_SOFTRST_SOFT_RESET			(0xA8)
#define TMD2635_REG_PWTIME_PROX_WAIT_TIME		(0xA9)
#define TMD2635_REG_CFG8_CONFIG8				(0xAA)
#define TMD2635_REG_CFG3_CONFIG3				(0xAB)
#define TMD2635_REG_CFG6_CONFIG6				(0xAE)

#define TMD2635_REG_PFILTER_PROX_FILTER			(0xB3)

#define TMD2635_REG_POFFSETL_PROX_OFFSET_LOW	(0xC0)
#define TMD2635_REG_POFFSETH_PROX_OFFSET_HIGH	(0xC1)

#define TMD2635_REG_CALIB_PROX_OFFSET_CALBRATE	(0xD7)
#define TMD2635_REG_CALIBCFG_OFFSET_CAL_CTRL	(0xD9)
#define TMD2635_REG_CALIBSTAT_OFFSET_CAL_STATUS	(0xDC)
#define TMD2635_REG_INTENAB_INT_ENABLES			(0xDD)

#define TMD2635_REG_FAC_L_FACTORY_DATA_LOW		(0xE5)
#define TMD2635_REG_FAC_H_FACTORY_DATA_HIGH		(0xE6)
#define TMD2635_REG_TEST9						(0xE9)




//TMD2635_PERS_REG @0x8C
#define	PPERS_MASK								(0x0F << 0)

//TMD2635_CFG0_REG @0x8D
#define NO_PWLONG								(0x00 << 3)
#define PWLONG									(0x01 << 3)
#define CFG0_RESERVED							(0x01 << 6)

//TMD2635_PCFG0_REG @0x8E
#define PPULSE_MASK								(0x3F << 0)
#define PGAIN_MASK								(0x03 << 6)
#define PGAIN_1X								(0x00 << 6)
#define PGAIN_2X								(0x01 << 6)
#define PGAIN_4X								(0x02 << 6)
#define PGAIN_8X								(0x03 << 6)

//TMD2635_PCFG1_REG @0x8F
#define PLDRIVE_MASK							(0x0F << 0)
#define PLDRIVE_2MA								(0x00)
#define PLDRIVE_3MA								(0x01)
#define PLDRIVE_4MA								(0x02)
#define PLDRIVE_5MA								(0x03)
#define PLDRIVE_6MA								(0x04)
#define PLDRIVE_7MA								(0x05)
#define PLDRIVE_8MA								(0x06)
#define PLDRIVE_9MA								(0x07)
#define PLDRIVE_10MA							(0x08)
#define PPULSE_LEN_MASK							(0x07 << 5)
#define PPLEN_1US								(0x00 << 5)
#define PPLEN_2US								(0x01 << 5)
#define PPLEN_4US								(0x02 << 5)
#define PPLEN_8US								(0x03 << 5)
#define PPLEN_12US								(0x04 << 5)
#define PPLEN_16US								(0x05 << 5)
#define PPLEN_24US								(0x06 << 5)
#define PPLEN_32US								(0x07 << 5)

//TMD2635_STATUS_REG @0x9B
#define PSAT_AMBIENT							(0x01 << 0)
#define PSAT_REFLECTIVE							(0x01 << 1)
#define ZINT									(0x01 << 2)
#define CINT									(0x01 << 3)
#define PINT									(0x01 << 4)
#define PSAT									(0x01 << 5)
#define PLOW									(0x01 << 6)
#define PHIGH									(0x01 << 7)

//TMD2635_SOFTRST_REG @0xA8
#define SOFTRST									(0x01 << 0)

//TMD2635_CFG8_REG @0xAA
#define PDSELECT_MASK							(0x03 << 0)
#define PD_NO									(0x00 << 0)
#define PD_FAR									(0x01 << 0)
#define PD_NEAR									(0x02 << 0)
#define PD_BOTH									(0x03 << 0)

//TMD2635_CFG3_REG @0xAB
//#define CFG3_RESERVED1							(0x01 << 2)
#define SAI										(0x01 << 4)
#define INT_READ_CLEAR							(0x01 << 7)

//TMD2635_CFG6_REG @0xAE
#define CFG6_RESERVED							(0x3F << 0)
#define APC_ENABLE								(0x00)
#define APC_DISABLE								(0x01 << 6)

//TMD2635_FILTER_REG @0xB3
#define PMAVG_MASK								(0x03 << 0)
#define PMAVG_DIS								(0x00 << 0)
#define PMAVG_2									(0x01 << 0)
#define PMAVG_4									(0x02 << 0)
#define PMAVG_8									(0x03 << 0)

//TMD2635_CALIB_REG @0xD7
#define START_OFFSET_CALIB						(0x01 << 0)
#define PRATE_IGNORED							(0x00)
#define PRATE_APPLIED							(0x01 << 4)
#define ELECTRICAL_OPTICAL_CALIBRATION			(0x00)
#define ELECTRICAL_CALIBRATION					(0x01 << 5)
#define NO_HARDWARE_AVG							(0x00)
#define HARDWARE_AVG							(0x01 << 7)

//TMD2635_CALIBCFG_REG @0xD9
#define PROX_AVG_MASK							(0x07 << 0)
#define PROX_DATA_AVG_DISABLE					(0x00 << 0)
#define PROX_DATA_AVG_2							(0x01 << 0)
#define PROX_DATA_AVG_4							(0x02 << 0)
#define PROX_DATA_AVG_8							(0x03 << 0)
#define PROX_DATA_AVG_16						(0x04 << 0)
#define PROX_DATA_AVG_32						(0x05 << 0)
#define PROX_DATA_AVG_64						(0x06 << 0)
#define PROX_DATA_AVG_128						(0x07 << 0)
#define NO_AUTO_OFFSET_ADJ						(0x00 << 3)
#define AUTO_OFFSET_ADJ							(0x01 << 3)
#define CALIBCFG_RESERVED						(0x01 << 4)
#define BINSRCH_TARGET_MASK						(0x07 << 5)
#define BINSRCH_TARGET_3						(0x00 << 5)
#define BINSRCH_TARGET_7						(0x01 << 5)
#define BINSRCH_TARGET_15						(0x02 << 5)
#define BINSRCH_TARGET_31						(0x03 << 5)
#define BINSRCH_TARGET_63						(0x04 << 5)
#define BINSRCH_TARGET_127						(0x05 << 5)
#define BINSRCH_TARGET_255						(0x06 << 5)
#define BINSRCH_TARGET_511						(0x07 << 5)

//TMD2635_CALIBSTAT_REG @0xDC
#define CALIB_FINISHED							(0x01 << 0)

//TMD2635_INTENAB_REG @0xDD
#define ZIDIS									(0x00 << 1)
#define ZIEN									(0x01 << 1)
#define CIDIS									(0x00 << 2)
#define CIEN									(0x01 << 2)
#define PSIDIS									(0x00 << 3)
#define PSIEN									(0x01 << 3)
#define PIDIS									(0x00 << 4)
#define PIM_LEVEL_BASED							(0x00 << 5)
#define PIM_STATE_BASED							(0x01 << 5)


//The following are the init array :
//{
//	{TMD2635_REG_ENABLE,					PON},
//	{TMD2635_REG_PRATE_PROX_TIME,			0x1F},
//	{TMD2635_REG_PILTL_LOW_THRESHOLD_LOW,	0x00},
//	{TMD2635_REG_PILTH_LOW_THRESHOLD_HIGH,	0x04},
//	{TMD2635_REG_PIHTL_HIGH_THRESHOLD_LOW,	0x00},
//	{TMD2635_REG_PIHTH_HIGH_THRESHOLD_HIGH,	0x06},
//	{TMD2635_REG_PERS_INT_FILTERS,			0x00},
//	{TMD2635_REG_CFG0_CONFIG0,				CFG0_RESERVED|NO_PWLONG},
//	{TMD2635_REG_PCFG0_PROX_CONFIG0,		PGAIN_4X|15},
//	{TMD2635_REG_PCFG1_PROX_CONFIG1,		PPLEN_8US|PLDRIVE_2MA},
//	{TMD2635_REG_PWTIME_PROX_WAIT_TIME,		0x00},
//	{TMD2635_REG_CFG8_CONFIG8,				PD_NEAR},
//	{TMD2635_REG_CFG3_CONFIG3,				INT_READ_CLEAR|SAI|CFG3_RESERVED1},
//	{TMD2635_REG_CFG6_CONFIG6,				APC_ENABLE|CFG6_RESERVED},
//	{TMD2635_REG_PFILTER_PROX_FILTER,		PMAVG_2},
//	{TMD2635_REG_CALIBCFG_OFFSET_CAL_CTRL,	BINSRCH_TARGET_15|CALIBCFG_RESERVED|AUTO_OFFSET_ADJ|PROX_DATA_AVG_4},
//	{TMD2635_REG_INTENAB_INT_ENABLES,		PIM_STATE_BASED|PIEN|PSIDIS|CIEN|ZIDIS},
//	{TMD2635_REG_ENABLE,					PON|PEN},
//	{TMD2635_REG_CALIB_PROX_OFFSET_CALBRATE,HARDWARE_AVG|ELECTRICAL_OPTICAL_CALIBRATION|PRATE_APPLIED|START_OFFSET_CALIB}
//}
 //
#endif

