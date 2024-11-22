#ifndef _LTK1181_H_
#define _LTK1181_H_

#define SENSOR_LTK1181_TWI_ID        TWI_ID_01

#define Ltk1181_I2C_SlaveAddr 0x2C

#define toPsCtrl(PBITS, PMA) \
	(unsigned char)((PBITS << 2) | PMA)
#define toDriverCtrl(LEDDRV) \
	(unsigned char)((LEDDRV & 0x0F) | 0x20)
#define toInterruptCtrl(OBJIEN, PDATIEN, PIEN) \
	(unsigned char)((OBJIEN << 5) | (PDATIEN << 3) | PIEN)

#define Ltk1181_Default_PBITS			(unsigned char)0			//10 bit
#define Ltk1181_Default_PMA				(unsigned char)0			//MA1

#define Ltk1181_Default_PsPulseWidth		(unsigned char)8			//32uS

#define Ltk1181_Default_PsBurstCount		(unsigned char)4			//1C

#define Ltk1181_Default_LEDDRV			(unsigned char)0x08			//12mA

#define Ltk1181_Default_PS_BOOST_INT		(unsigned char)0			//No boost interrupt
#define Ltk1181_Default_PS_TRIG_MODE		(unsigned char)0			//Dynamic Mode
#define Ltk1181_Default_PPERS			(unsigned char)1			//Persistence 2

#define Ltk1181_Default_OBJIEN			(unsigned char)0
#define Ltk1181_Default_PDATIEN			(unsigned char)0
#define Ltk1181_Default_PIEN				(unsigned char)1

#define Ltk1181_Default_FrameTime		(unsigned short)200			//200 ms

#define Ltk1181_Default_Threshold_High	(unsigned short)500			//High threshold = 600
#define Ltk1181_Default_Threshold_Low	(unsigned short)380			//Low threshold = 200

#define Ltk1181_Default_CTC_Gain		(unsigned char)7			//Start from 1
#define Ltk1181_Default_CTC_Step		(unsigned char)10			//Default is zero

#define Ltk1181_Default_Auto_CTC_en		(unsigned char)0			//Default is disable			
#define Ltk1181_Default_CTC_en			(unsigned char)1			//Default is enable
#define Ltk1181_Default_Full_scale		(unsigned short)1023		// 10 bits Full scale
#define Ltk1181_CTC_G7_Step_Values		(unsigned char)65			//Gain 7 1 step
#define Ltk1181_CTC_G7_Values			(unsigned char)7			//Gain 7 1 step
#define Ltk1181_CTC_Step_limit			(unsigned char)45			
#define Ltk1181_CTC_Avg_Loop			(unsigned char)1



typedef struct Ltk1181_Reg__ {
	/*
		0x00 InterruptFlag
	*/
	union {
		unsigned char InterruptFlag;
		struct {
			unsigned char PIF : 1;		//PS Interrupt Flag
			unsigned char : 2;
			unsigned char DATIF : 1;	//PDAT Ready Flag
			unsigned char : 1;
			unsigned char PHTH : 1;		//PS High Threshold Flag
			unsigned char PLTF : 1;		//PS Low Threshold Flag
			unsigned char PORIF : 1;	//Power On Reset Flag
		};
	};

	/*
		0x01 ErrorFlag
	*/
	union {
		unsigned char ErrorFlag;
		struct {
			unsigned char PERR : 1;		//PDAT Not Valid Flag
			unsigned char : 7;
		};
	};

	/*
		0x1E PID
	*/
	unsigned char PID;

	/*
		0x1F VID
	*/
	unsigned char VID;

	/*
		0x40 PsCtrl

		0x40[3:2] PBITS

			PDAT valid bit width setting.

		0x40[1:0] PMA

			PDAT average numbers.

			Proximity data are added and averaged as valid PDAT depending on PMA setting.

			No averaging is done if PMA is set as zero (default).

			PDATs are stored internally before number of average is
			reached. The through-put of PDAT will be divided by average
			number. When complete, flag of valid PDAT will be asserted.
	*/
	union {
		unsigned char PsCtrl;
		struct {
			unsigned char PMA : 2;
			unsigned char PBITS : 2;
			unsigned char : 4;
		};
	};

	/*
		0x42 PsPulseWidth

		PS pulse width setting which is the duration of one VCSEL
		emitting.
	*/
	union {
		unsigned char PsPulseWidth;
		unsigned char PPW;
	};

	/*
		0x43 PsBurstCount

		PS burst count setting which is cycles of VCSEL emitting in
		one proximity data sensing.
	*/
	union {
		unsigned char PsBurstCount;
		unsigned char PBC;
	};

	/*
		0x44 DriverCtrl

		0x44[3:0] LEDDRV

		VCSEL driving current setting. 
	
		9mA is recommended.
	*/
	union {
		unsigned char DriverCtrl;
		struct {
			unsigned char LEDDRV : 4;
			unsigned char : 4;
		};
	};

	/*
		0x45 PsIntCtrl

		0x45[5] PS_BOOST_INT

			PS distance event boost mode select. 
			When PS_BOOST_INT is set, if one PDAT meets distance event trigger 
			condition, next PDAT will be sensed immediately regardless of frame rate. 
	
			The purpose is to speed up distance event interrupt asserting time.
	
			Whenever distance event interrupt asserts, the next PDAT 
			will be sensed with normal frame rate.
		
		0x45[4] PS_TRIG_MODE

			PS distance event trigger mode select. 

		0x45[3:0] PPERS

			PS distance event persistence number. 

			PS distance event will 	be triggered if distance condition 
			is met with number of consecutive PDAT exceeds number of persistence. 
	
			If one PDAT does not meet distance event condition, the number 
			will be re-counted from zero.
	*/
	union {
		unsigned char PsIntCtrl;
		struct {
			unsigned char PPERS : 4;
			unsigned char PS_TRIG_MODE : 1;
			unsigned char PS_BOOST_INT : 1;
			unsigned char : 2;
		};
	};

	/*
		0x46 CTC_Gain

		0x46[3:0] CTC_GAIN
		PS crosstalk (CT) cancellation system gain setting.

		The CT will be calculated with combination of CTC_GAIN and CTC_Step.
	*/
	union {
		unsigned char CTC_Gain;
		struct {
			unsigned char CTC_GAIN : 4;
			unsigned char : 4;
		};
	};

	/*
		0x47 CTC_Ctrl

		0x47[5] CTC_AUTO

			Crosstalk automatic cancellation.

			This bit is valid only if CTC_EN is set to 1.

			Crosstalk will be cancelled automatically when PEN is set to one.
		
		0x47[4] CTC_EN

			Enable crosstalk cancellation system.

			When CTC_AUTO is not set. Crosstalk cancellation will be done
			with offset manually set by CTC_GAIN and CTC_Step.

			When CTC_AUTO is set, automatic crosstalk cancellation is done
			and CTC_GAIN/CTC_Step are ignored.
	*/
	union {
		unsigned char CTC_Ctrl;
		struct {
			unsigned char : 4;
			unsigned char CTC_EN : 1;
			unsigned char CTC_AUTO : 1;
			unsigned char : 2;
		};
	};
	
	/*
		0xD9 CTC_Step

		0xD9[5:0] CTC_Step

		The crosstalk cancellation value.

		This field contributes the base unit that will be multiplied
		with CTC_GAIN as offset to be subtracted by crosstalk cancellation system.
	*/
	union {
		unsigned char CTC_Step;
		struct {
			unsigned char CTC_STEP : 6;
			unsigned char : 2;
		};
	};
	/*
		0x80 SensorCtrl

		0x80[7] ONESHOT

			When ONESHOT is set, measurement will start once if 0x00[4] DATIF is written to 0.

			0x80[6] IRDCLR does not affect the behavior of measurement starting.

		0x80[6] IRDCLR

			This bit activates interrupt read clear.
			If IRDCLR is set, the interrupt flag will be cleared whenever 0x00 InterruptFlag is read.

		0x80[0] PEN

			This bit activates the PS function.
	*/
	union {
		unsigned char SensorCtrl;
		struct {
			unsigned char PEN : 1;
			unsigned char : 5;
			unsigned char IRDCLR : 1;
			unsigned char ONESHOT : 1;
		};
	};

	/*
		0x81 InterruptCtrl

		0x81[5] OBJIEN

			Enable the Object status interrupt flag to INT pin.

		0x81[3] PDATIEN

			Enable the PDAT ready interrupt flag to INT pin.

		0x81[0] PIEN

			Enable the distance event interrupt flag to INT pin.
	*/
	union {
		unsigned char InterruptCtrl;
		struct {
			unsigned char PIEN : 1;
			unsigned char : 2;
			unsigned char PDATIEN : 1;
			unsigned char : 1;
			unsigned char OBJIEN : 1;
			unsigned char : 2;
		};
	};

	/*
		0x84 SoftwareReset

		0x84[7:0] SoftReset

		Write 0xA5 to do software reset.
		I2C communication should be waiting for 5 milliseconds after doing SoftReset.
	*/
	union {
		unsigned char SoftwareReset;
		unsigned char SoftReset;
	};

	#define Ltk1181_Do_SoftReset		(unsigned char)0xA5

	unsigned char for_align_no_use;

	/*
		0x02 PDAT_L
		0x03 PDAT_H
	*/
	unsigned short PDAT;

	/*
		0x4A PsBaseLine_L
		0x4B PsBaseLine_H

		Baseline that will be subtracted from measurement as PDAT.
	*/
	unsigned short PsBaseLine;

	/*
		0x4C PsThresholdLow_L
		0x4D PsThresholdLow_H

		Low threshold value for distance event.
	*/
	unsigned short PsThresholdLow;

	/*
		0x4C PsThresholdHigh_L
		0x4D PsThresholdHigh_H

		High threshold value for distance event.
	*/
	unsigned short PsThresholdHigh;

	/*
		0x88 FrameTimeL
			[7:0] FTL
		0x89 FrameTimeH
			[3:0] FTH

		12-bit PS Frame Time.

		Frame time is the period of consecutive proximity measurement.

		When set to zero, new proximity measurement will start immediately after one valid PDAT is ready.

		1 count = 1 millisecond
	*/
	union {
		unsigned short FrameTime;
		struct {
			unsigned short FT : 12;
			unsigned short : 4;
		};
	};
}Ltk1181_Reg_;



unsigned char Ltk1181_Set_PsCtrl(unsigned char PBITS, unsigned char PMA);
unsigned char Ltk1181_Get_PsCtrl(void);
unsigned char Ltk1181_Set_PsPulseWidth(unsigned char PPW);
unsigned char Ltk1181_Get_PsPulseWidth(void);
unsigned char Ltk1181_Set_PsBurstCount(unsigned char PBC);
unsigned char Ltk1181_Get_PsBurstCount(void);
unsigned char Ltk1181_Set_DriverCtrl(unsigned char LEDDRV);
unsigned char Ltk1181_Get_DriverCtrl(void);
unsigned char Ltk1181_Set_PsIntCtrl(unsigned char PS_BOOST_INT, unsigned char PS_TRIG_MODE, unsigned char PPERS);
unsigned char Ltk1181_Get_PsIntCtrl(void);
unsigned char Ltk1181_Set_CTC_Gain(unsigned char CTC_GAIN);
unsigned char Ltk1181_Get_CTC_Gain(void);
unsigned char Ltk1181_Set_CTC_Ctrl(unsigned char CTC_AUTO, unsigned char CTC_EN);
unsigned char Ltk1181_Get_CTC_Ctrl(void);
unsigned char Ltk1181_Set_CTC_Step(unsigned char CTC_STEP);
unsigned char Ltk1181_Get_CTC_Step(void);
unsigned char Ltk1181_Set_PsBaseLine(unsigned short PsBaseLine);
unsigned short Ltk1181_Get_PsBaseLine(void);
unsigned char Ltk1181_Set_PsThresholdLow(unsigned short PsThresholdLow);
unsigned short Ltk1181_Get_PsThresholdLow(void);
unsigned char Ltk1181_Set_PsThresholdHigh(unsigned short PsThresholdHigh);
unsigned short Ltk1181_Get_PsThresholdHigh(void);
unsigned char Ltk1181_Set_SensorCtrl(unsigned char ONESHOT, unsigned char IRDCLR, unsigned char PEN);
unsigned char Ltk1181_Get_SensorCtrl(void);
unsigned char Ltk1181_Set_InterruptCtrl(unsigned char OBJIEN, unsigned char PDATIEN, unsigned char PIEN);
unsigned char Ltk1181_Get_InterruptCtrl(void);
unsigned char Ltk1181_SoftReset(void);
unsigned char Ltk1181_Set_FrameTime(unsigned short FT);
unsigned short Ltk1181_Get_FrameTime(void);

unsigned char Ltk1181_Initial(unsigned char withReset);
unsigned char Ltk1181_Clear_Interrupt(void);
unsigned char Ltk1181_Reload_RegiterMap(void);
void Ltk1181_Get_RegisterMap(Ltk1181_Reg_* data);
unsigned char Ltk1181_Enable_PS(unsigned char enable);
unsigned short Ltk1181_Read_PDAT(void);
unsigned char Ltk1181_Read_PDAT_with_Check_DATIF(unsigned short* PDAT);
unsigned char Ltk1181_Enable_Auto_CT(unsigned char enable);
unsigned char Ltk1181_Who_am_I(void);
unsigned char Ltk1181_calibration(uint8_t *step);



void Ltk11181_resume(void);


typedef struct ltk1181_prox_info
{
	uint16_t                raw;
	uint8_t                 nearby;
} ltk1181_prox_info;

typedef struct ltk1181_instance_state
{
	/* Sensor to be published */
    unsigned char           address;
	unsigned char           publish_sensors;

	/* Proximity HW config details*/
	ltk1181_prox_info       prox_info;

	bool                    instance_is_ready_to_configure;
	bool                    timer_is_active;
	bool                    is_dri;
	bool                    first_prox;
	bool                    prox_to_report;

	unsigned char			near_threshold;
	unsigned char			far_threshold;
	bool                    is_prox_dri;
} ltk1181_instance_state;


extern unsigned char MCU_I2C_Write(unsigned char devid, unsigned char reg, unsigned char* data, unsigned char num);
extern unsigned char MCU_I2C_Read(unsigned char devid, unsigned char reg, unsigned char* data, unsigned char num);
extern void MCU_Wait_Millisecond(unsigned int millisecond);
void Ltk1181_get_datat_cb(void);
void Ltk1181_suspend(void);
void Ltk1181_sensor_deinit(void);
void Ltk1181_calibration_store_flash(uint8_t flag, uint8_t step);
void Ltk1181_calibration_get_flash(uint8_t *flag, uint8_t *step);


#endif	//_LTK1181_H_

