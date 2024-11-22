#define LOG_LEV 5
#include "sys_inc.h"
#include "user_config.h"
#include "hal.h"
#include "sensor.h"
#include "sys_exception.h"
#include "LTK1181.h"
#include "psm_manager.h"
#include "info_mem.h"

#if (SENSOR_PROXIMITY_EN && (SENSOR_PROXIMITY_DEVICE_SEL == 3 || SENSOR_PROXIMITY_DEVICE_SEL == 4))

static ltk1181_instance_state ltk1181_state;
ltk1181_instance_state *p_ltk1181_state = &ltk1181_state;
OAL_THREAD sensor_proximity_task_handle;
bool volatile sensor_proximity_task_exit;
OAL_SEM sensor_proximity_sem;
bool sensor_proximity_task_deleted;

uint8_t ear_in_flag;
uint8_t ear_in_state;


OAL_TIMER earStateTimer;



enum Ltk1181_Reg_Addr_ {
	Reg_InterruptFlag,
	Reg_ErrorFlag,
	Reg_PDAT_L,
	Reg_PDAT_H,
	Reg_PID = 0x1E,
	Reg_VID,
	Reg_PsCtrl = 0x40,
	Reg_PsPulseWidth = 0x42,
	Reg_PsBurstCount,
	Reg_DriverCtrl,
	Reg_PsIntCtrl,
	Reg_CTC_Gain,
	Reg_CTC_Ctrl,
	Reg_PsBaseLine_L = 0x4A,
	Reg_PsBaseLine_H,
	Reg_PsThresholdLow_L,
	Reg_PsThresholdLow_H,
	Reg_PsThresholdHigh_L,
	Reg_PsThresholdHigh_H,
	Reg_SensorCtrl = 0x80,
	Reg_InterruptCtrl,
	Reg_SoftwareReset = 0x84,
	Reg_FrameTime_L = 0x88,
	Reg_FrameTime_H,
	Reg_CTC_Step = 0xD9
}Ltk1181_Reg_Addr;

Ltk1181_Reg_ Ltk1181_Reg = {
	.InterruptFlag = 0xC0,
	.ErrorFlag = 0x00,
	.PDAT = 0,
	.PID = 0x01,
	.VID = 0x02,
	.PsCtrl = 0x00,
	.PsPulseWidth = 0x01,
	.PsBurstCount = 0x01,
	.DriverCtrl = 0x20,
	.PsIntCtrl = 0x01,
	.CTC_Gain = 0x11,
	.CTC_Ctrl = 0x00,
	.PsBaseLine = 0,
	.PsThresholdLow = 0,
	.PsThresholdHigh = 65535,
	.SensorCtrl = 0x00,
	.InterruptCtrl = 0x81,
	.SoftwareReset = 0x00,
	.FrameTime = 30,
	.CTC_Step = 0
};

enum {
	PBITS_10Bits,		//Default
	PBITS_12Bits,
	PBITS_14Bits,
	PBITS_16Bits
}PBITS_Value;

enum {
	PMA_MA1,		//Default
	PMA_MA2,
	PMA_MA4,
	PMA_MA8
}PMA_Value;

enum {
	LEDDRV_OFF,		//Default
	LEDDRV_5mA,
	LEDDRV_6mA,
	LEDDRV_7mA,
	LEDDRV_8mA,
	LEDDRV_9mA,		
	LEDDRV_10mA,
	LEDDRV_11mA,
	LEDDRV_12mA,	//Recommanded
	LEDDRV_13mA,
	LEDDRV_14mA,
	LEDDRV_15mA,
	LEDDRV_16mA,
	LEDDRV_17mA,
	LEDDRV_18mA,
	LEDDRV_19mA
}LEDDRV_Value;

enum {
	PS_TRIG_MODE_Dynamic,	//Default
	PS_TRIG_MODE_Static
}PS_TRIG_MODE_Value;

typedef struct {
    uint8_t cal_flag;
    uint8_t ctc_step;
} ltk1181_cal_info_t;


unsigned char Ltk1181_I2C_WriteByte(unsigned char reg, unsigned char data)
{
	int  result;
	unsigned char buff[2];

	buff[0] = reg;
	buff[1] = data;
	result = twi_write(SENSOR_LTK1181_TWI_ID, (unsigned char)Ltk1181_I2C_SlaveAddr, buff, 2);
	logi("Ltk1181_I2C_WriteByte reg:0x%x, data:0x%x,ret=%d",reg,data,result);
	return result;
}

unsigned char Ltk1181_I2C_WriteWord(unsigned char reg, unsigned short data)
{
	unsigned char result;
	unsigned char buff[3];

	buff[0] = (unsigned char)reg;
	buff[1] = (unsigned char)data;
	buff[2] = (unsigned char)(data >> 8);

	result = twi_write(SENSOR_LTK1181_TWI_ID, (unsigned char)Ltk1181_I2C_SlaveAddr, buff, 3);

	return result;
}

unsigned char Ltk1181_I2C_Read(unsigned char reg, unsigned char* data, unsigned char num)
{
	unsigned char result;

	#if TWI_IO_EN
    	result = twi_io_write((unsigned char)Ltk1181_I2C_SlaveAddr, &reg, num);
    	result = twi_io_read((unsigned char)Ltk1181_I2C_SlaveAddr, data, num);
	#else
    	result = twi_write(SENSOR_LTK1181_TWI_ID, (unsigned char)Ltk1181_I2C_SlaveAddr, &reg, num);
    	result = twi_read(SENSOR_LTK1181_TWI_ID, (unsigned char)Ltk1181_I2C_SlaveAddr, data, num);
	#endif

	return result;
}

unsigned char Ltk1181_I2C_ReadByte(unsigned char reg, unsigned char* data)
{
	unsigned char result;

	#if TWI_IO_EN
    	result = twi_io_write((unsigned char)Ltk1181_I2C_SlaveAddr, &reg, 1);
    	result = twi_io_read((unsigned char)Ltk1181_I2C_SlaveAddr, data, 1);
	#else
    	result = twi_write_stop_ctl(SENSOR_LTK1181_TWI_ID, (unsigned char)Ltk1181_I2C_SlaveAddr, &reg, 1,0);
    	result = twi_read(SENSOR_LTK1181_TWI_ID, (unsigned char)Ltk1181_I2C_SlaveAddr, data, 1);
	#endif

	return result;
}

unsigned char Ltk1181_I2C_ReadWord(unsigned char reg, unsigned short* data)
{
	unsigned char result;
	unsigned char buff[3];

	result = twi_read((unsigned char)Ltk1181_I2C_SlaveAddr, reg, buff, 3);
	data[0] = buff[1];
	data[0] <<= 8;
	data[0] |= buff[0];

	return result;
}

AT_SRAM(.sensor)
void * sensor_proximity_task(void *para)
{
    logi("!!sensor_proximity_task start");
	while (1) {
         if (sensor_proximity_task_exit) {
            logi("!!sensor_proximity_task start exit 1");
            break;
        }
        OAL_sem_wait(&sensor_proximity_sem);

        if (sensor_proximity_task_exit) {
            logi("!!sensor_proximity_task start exit 2");
            break;
        }
        logi("!!get_data");
		//unsigned char LTK1181_INT=2; //Need to modify
		unsigned char LTK1181_INTFlag=0;
		
		Ltk1181_I2C_ReadByte(Reg_InterruptFlag, &LTK1181_INTFlag);
		logi("** Interrupt Flag = 0x%x", LTK1181_INTFlag);
		
		LTK1181_INTFlag = LTK1181_INTFlag & 0x60; //读取阈值溢出中断标志寄存器

		logi("\n***LTK1181 Near(0x02)/Far(0x04) Flag = 0x%x", LTK1181_INTFlag);

        unsigned char reg;
		unsigned short PDAT = 0;
		ear_in_flag = 0;
        Ltk1181_I2C_ReadByte(Reg_PDAT_H, &reg);
        logi("**High byte Ps Data=0x%x",reg);
		PDAT = reg;
		PDAT <<= 8;
        
        Ltk1181_I2C_ReadByte(Reg_PDAT_L, &reg);
        logi("**Low Byte Ps Data=0x%x",reg);
		PDAT |= reg;
		
		logi("*** Ps data = %d", PDAT);	//PXY values

		Ltk1181_Clear_Interrupt();		
		
		if(LTK1181_INTFlag==0x20)//STATUS PHIGH 寄存器标志位，Pdata溢出高阈值
        {
            logi(" >>>IN  EAR<<< ");
            //event_put(SENSOR_EVENT_P_HT, NULL, 0);
            ear_in_flag = 1;
           
        }
        if(LTK1181_INTFlag==0x40)//STATUS PLOW 寄存器标志位，Pdata低于低阈值
        {
            logi(" <<<OUT EAR>>>");
            //event_put(SENSOR_EVENT_P_LT, NULL, 0);
            ear_in_flag = 2;
        }
        if (ear_in_flag != 0) {
        if (earStateTimer != NULL) {
            OAL_timer_activate(&earStateTimer, OAL_TIMER_NO_ACTIVATE);
            OAL_timer_activate(&earStateTimer, OAL_TIMER_AUTO_ACTIVATE);
        }
        }
	    if(OAL_SUCCESS == OAL_thread_wait_join(sensor_proximity_task_handle, 0)) {
           break;
        }	
	}
    sensor_proximity_task_deleted = true;

    OAL_thread_delete_self(sensor_proximity_task_handle);
     if (NULL != sensor_proximity_sem) {
        OAL_sem_destroy(&sensor_proximity_sem);
        sensor_proximity_sem = NULL;
    }
    return NULL;
	
}

unsigned char Ltk1181_Set_PsCtrl(unsigned char PBITS, unsigned char PMA)
{
	unsigned char result;

	Ltk1181_Reg.PBITS = PBITS;
	Ltk1181_Reg.PMA = PMA;

	result = Ltk1181_I2C_WriteByte(Reg_PsCtrl, Ltk1181_Reg.PsCtrl);

	return result;
}

unsigned char Ltk1181_Get_PsCtrl(void)
{
	return Ltk1181_Reg.PsCtrl;
}

unsigned char Ltk1181_Set_PsPulseWidth(unsigned char PPW)
{
	unsigned char result;

	Ltk1181_Reg.PPW = PPW;

	result = Ltk1181_I2C_WriteByte(Reg_PsPulseWidth, Ltk1181_Reg.PsPulseWidth);

	return result;
}

unsigned char Ltk1181_Get_PsPulseWidth(void)
{
	return Ltk1181_Reg.PsPulseWidth;
}

unsigned char Ltk1181_Set_PsBurstCount(unsigned char PBC)
{
	unsigned char result;

	Ltk1181_Reg.PBC = PBC;

	result = Ltk1181_I2C_WriteByte(Reg_PsBurstCount, Ltk1181_Reg.PsBurstCount);

	return result;
}

unsigned char Ltk1181_Get_PsBurstCount(void)
{
	return Ltk1181_Reg.PsBurstCount;
}

unsigned char Ltk1181_Set_DriverCtrl(unsigned char LEDDRV)
{
	unsigned char result;

	Ltk1181_Reg.LEDDRV = LEDDRV;

	result = Ltk1181_I2C_WriteByte(Reg_DriverCtrl, Ltk1181_Reg.DriverCtrl);

	return result;
}

unsigned char Ltk1181_Get_DriverCtrl(void)
{
	return Ltk1181_Reg.DriverCtrl;
}

unsigned char Ltk1181_Set_PsIntCtrl(unsigned char PS_BOOST_INT, unsigned char PS_TRIG_MODE, unsigned char PPERS)
{
	unsigned char result;

	Ltk1181_Reg.PS_BOOST_INT = PS_BOOST_INT;
	Ltk1181_Reg.PS_TRIG_MODE = PS_TRIG_MODE;
	Ltk1181_Reg.PPERS = PPERS;

	result = Ltk1181_I2C_WriteByte(Reg_PsIntCtrl, Ltk1181_Reg.PsIntCtrl);

	return result;
}

unsigned char Ltk1181_Get_PsIntCtrl(void)
{
	return Ltk1181_Reg.PsIntCtrl;
}

unsigned char Ltk1181_Set_CTC_Gain(unsigned char CTC_GAIN)
{
	unsigned char result;

	Ltk1181_Reg.CTC_GAIN = CTC_GAIN;

	result = Ltk1181_I2C_WriteByte(Reg_CTC_Gain, Ltk1181_Reg.CTC_Gain);

	return result;
}

unsigned char Ltk1181_Get_CTC_Gain(void)
{
	return Ltk1181_Reg.CTC_Gain;
}

unsigned char Ltk1181_Set_CTC_Ctrl(unsigned char CTC_AUTO, unsigned char CTC_EN)
{
	unsigned char result;

    Ltk1181_Reg.CTC_AUTO = CTC_AUTO;
    Ltk1181_Reg.CTC_EN = CTC_EN;

    result = Ltk1181_I2C_WriteByte(0xC8, 0x10); // Enable internel Auto_CTC switch  

    result = Ltk1181_I2C_WriteByte(Reg_CTC_Ctrl, Ltk1181_Reg.CTC_Ctrl);
    result = Ltk1181_I2C_WriteByte(Reg_CTC_Ctrl, 0xD0);

	return result;
}

unsigned char Ltk1181_Get_CTC_Ctrl(void)
{
	return Ltk1181_Reg.CTC_Ctrl;
}

unsigned char Ltk1181_Set_CTC_Step(unsigned char CTC_STEP)
{
	unsigned char result;

	Ltk1181_Reg.CTC_STEP = CTC_STEP;

	result = Ltk1181_I2C_WriteByte(Reg_CTC_Step, Ltk1181_Reg.CTC_Step);

	return result;
}

unsigned char Ltk1181_Get_CTC_Step(void)
{
	return Ltk1181_Reg.CTC_Step;
}

unsigned char Ltk1181_Set_PsBaseLine(unsigned short PsBaseLine)
{
	unsigned char result;

	Ltk1181_Reg.PsBaseLine = PsBaseLine;

	result = Ltk1181_I2C_WriteWord(Reg_PsBaseLine_L, Ltk1181_Reg.PsBaseLine);

	return result;
}

unsigned short Ltk1181_Get_PsBaseLine(void)
{
	return Ltk1181_Reg.PsBaseLine;
}

unsigned char Ltk1181_Set_PsThresholdLow(unsigned short PsThresholdLow)
{
	unsigned char result;

	Ltk1181_Reg.PsThresholdLow = PsThresholdLow;

	result = Ltk1181_I2C_WriteWord(Reg_PsThresholdLow_L, Ltk1181_Reg.PsThresholdLow);

	return result;
}

unsigned short Ltk1181_Get_PsThresholdLow(void)
{
	return Ltk1181_Reg.PsThresholdLow;
}

unsigned char Ltk1181_Set_PsThresholdHigh(unsigned short PsThresholdHigh)
{
	unsigned char result;

	Ltk1181_Reg.PsThresholdHigh = PsThresholdHigh;

	result = Ltk1181_I2C_WriteWord(Reg_PsThresholdHigh_L, Ltk1181_Reg.PsThresholdHigh);

	return result;
}

unsigned short Ltk1181_Get_PsThresholdHigh(void)
{
	return Ltk1181_Reg.PsThresholdHigh;
}

unsigned char Ltk1181_Set_SensorCtrl(unsigned char ONESHOT, unsigned char IRDCLR, unsigned char PEN)
{
	unsigned char result;

	Ltk1181_Reg.ONESHOT = ONESHOT;
	Ltk1181_Reg.IRDCLR = IRDCLR;
	Ltk1181_Reg.PEN = PEN;

	result = Ltk1181_I2C_WriteByte(Reg_SensorCtrl, Ltk1181_Reg.SensorCtrl);

	return result;
}

unsigned char Ltk1181_Get_SensorCtrl(void)
{
	return Ltk1181_Reg.SensorCtrl;
}

unsigned char Ltk1181_Set_InterruptCtrl(unsigned char OBJIEN, unsigned char PDATIEN, unsigned char PIEN)
{
	unsigned char result;

	Ltk1181_Reg.OBJIEN = OBJIEN;
	Ltk1181_Reg.PDATIEN = PDATIEN;
	Ltk1181_Reg.PEN = PIEN;

	result = Ltk1181_I2C_WriteByte(Reg_InterruptCtrl, Ltk1181_Reg.InterruptCtrl);

	return result;
}

unsigned char Ltk1181_Get_InterruptCtrl(void)
{
	return Ltk1181_Reg.InterruptCtrl;
}

unsigned char Ltk1181_SoftReset(void)
{
	unsigned char result;

	result = Ltk1181_I2C_WriteByte(Reg_SoftwareReset, Ltk1181_Do_SoftReset);
	
	HAL_mdelay(5);

	return result;
}

unsigned char Ltk1181_Set_FrameTime(unsigned short FT)
{
	unsigned char result;

	Ltk1181_Reg.FT = FT;

	result = Ltk1181_I2C_WriteWord(Reg_FrameTime_L, Ltk1181_Reg.FrameTime);

	return result;
}

unsigned short Ltk1181_Get_FrameTime(void)
{
	return Ltk1181_Reg.FrameTime;
}

AT_SRAM(.sensor)
void Ltk1181_get_datat_cb(void)
{
  if (NULL != sensor_proximity_sem) {
        logi("ltk1181_get_datat_cb");
      OAL_sem_post(&sensor_proximity_sem);
  }
}

AT_SRAM(.sensor)
void ear_stat_timer_cb(void *xTimer)
{
    logd("ear_stat_timer_cb ear_in_flag:%d,ear_in_state=%d",ear_in_flag,ear_in_state);
    if (ear_in_flag == 1) {
        if (ear_in_state == 0) {
            event_put(SENSOR_EVENT_P_HT, NULL, 0);
            ear_in_state = 1;
        }
    } else if (ear_in_flag == 2) {
        if (ear_in_state ==1) {
            event_put(SENSOR_EVENT_P_LT, NULL, 0);
            ear_in_state = 0;
       }
    }
}

unsigned char Ltk1181_Initial(unsigned char withReset)
{
	unsigned char result;
	unsigned char reg;
    uint8_t cal_flag = 0;
    uint8_t cal_step = 0;
		
	logi("!!Ltk1181_Initial start");
    
    Ltk1181_calibration_get_flash(&cal_flag,&cal_step);
    logi("ltk cal_flag:%d,cal_step:%d",cal_flag,cal_step);
	if(withReset)
		result = Ltk1181_SoftReset();
	
	//result = Ltk1181_Who_am_I();
	result = Ltk1181_Clear_Interrupt();
	result = Ltk1181_Set_PsCtrl(Ltk1181_Default_PBITS, Ltk1181_Default_PMA);
	result = Ltk1181_Set_PsPulseWidth(Ltk1181_Default_PsPulseWidth);
	result = Ltk1181_Set_PsBurstCount(Ltk1181_Default_PsBurstCount);
	result = Ltk1181_Set_DriverCtrl(Ltk1181_Default_LEDDRV);
	result = Ltk1181_Set_PsIntCtrl(Ltk1181_Default_PS_BOOST_INT, Ltk1181_Default_PS_TRIG_MODE, Ltk1181_Default_PPERS);
	result = Ltk1181_Set_InterruptCtrl(Ltk1181_Default_OBJIEN, Ltk1181_Default_PDATIEN, Ltk1181_Default_PIEN);
	result = Ltk1181_Set_PsThresholdHigh(Ltk1181_Default_Threshold_High);
	result = Ltk1181_Set_PsThresholdLow(Ltk1181_Default_Threshold_Low);
	

	/*********** CTC setting *****************/
	result = Ltk1181_Set_CTC_Gain(Ltk1181_Default_CTC_Gain);
    if (cal_flag){
	    result = Ltk1181_Set_CTC_Step(cal_step);
    }else{
	    result = Ltk1181_Set_CTC_Step(Ltk1181_Default_CTC_Step);
    }
	result = Ltk1181_Set_CTC_Ctrl(Ltk1181_Default_Auto_CTC_en, Ltk1181_Default_CTC_en);
	//result = Ltk1181_Enable_Auto_CT(1);
		
	Ltk1181_Enable_PS(1);
	//Ltk1181_calibration();

	Ltk1181_I2C_ReadByte(Reg_InterruptFlag,&reg);
	logi("Interrupt Flag = 0x%x",reg);

	//unsigned char reg;
	unsigned short PDAT = 0;
		
        Ltk1181_I2C_ReadByte(Reg_PDAT_H, &reg);
        logi("**High byte Ps Data=0x%x",reg);
		PDAT = reg;
		PDAT <<= 8;
        
        Ltk1181_I2C_ReadByte(Reg_PDAT_L, &reg);
        logi("**Low Byte Ps Data=0x%x",reg);
		PDAT |= reg;

		logi("*** Ps data = %d", PDAT);	//PXY values	

	sensor_proximity_task_deleted = false;
    sensor_proximity_task_exit = false;

	if (sensor_proximity_task_handle == NULL) {
        sensor_proximity_task_handle = OAL_thread_create(sensor_proximity_task, NULL, "task proximty", 1, 256);
     }

    if (sensor_proximity_sem == NULL) {
        sensor_proximity_sem = OAL_sem_create_binary();
      }

     if (earStateTimer == 0) {
        OAL_timer_create(&earStateTimer, "earStatTimer", OAL_ms_to_ticks(200),
            ear_stat_timer_cb, (void *)1, 0, OAL_TIMER_NO_ACTIVATE);
    }
     ear_in_state = 0;
	return result;
}

unsigned char Ltk1181_Clear_Interrupt(void)
{
	unsigned char result;

	result = Ltk1181_I2C_WriteByte(Reg_InterruptFlag, 0x00);
	result = Ltk1181_I2C_ReadByte(Reg_InterruptFlag, &Ltk1181_Reg.InterruptFlag);

	return result;
}

unsigned char Ltk1181_Reload_RegiterMap(void)
{
	unsigned char result;

	result = Ltk1181_I2C_ReadByte(Reg_InterruptFlag, &Ltk1181_Reg.InterruptFlag);
	result = Ltk1181_I2C_ReadByte(Reg_ErrorFlag, &Ltk1181_Reg.ErrorFlag);
	result = Ltk1181_I2C_ReadWord(Reg_PDAT_L, &Ltk1181_Reg.PDAT);
	result = Ltk1181_I2C_ReadByte(Reg_PID, &Ltk1181_Reg.PID);
	result = Ltk1181_I2C_ReadByte(Reg_VID, &Ltk1181_Reg.VID);
	result = Ltk1181_I2C_ReadByte(Reg_PsCtrl, &Ltk1181_Reg.PsCtrl);
	result = Ltk1181_I2C_ReadByte(Reg_PsPulseWidth, &Ltk1181_Reg.PsPulseWidth);
	result = Ltk1181_I2C_ReadByte(Reg_PsBurstCount, &Ltk1181_Reg.PsBurstCount);
	result = Ltk1181_I2C_ReadByte(Reg_DriverCtrl, &Ltk1181_Reg.DriverCtrl);
	result = Ltk1181_I2C_ReadByte(Reg_PsIntCtrl, &Ltk1181_Reg.PsIntCtrl);
	result = Ltk1181_I2C_ReadByte(Reg_CTC_Gain, &Ltk1181_Reg.CTC_Gain);
	result = Ltk1181_I2C_ReadByte(Reg_CTC_Ctrl, &Ltk1181_Reg.CTC_Ctrl);
	result = Ltk1181_I2C_ReadWord(Reg_PsBaseLine_L, &Ltk1181_Reg.PsBaseLine);
	result = Ltk1181_I2C_ReadWord(Reg_PsThresholdLow_L, &Ltk1181_Reg.PsThresholdLow);
	result = Ltk1181_I2C_ReadWord(Reg_PsThresholdHigh_L, &Ltk1181_Reg.PsThresholdHigh);
	result = Ltk1181_I2C_ReadByte(Reg_SensorCtrl, &Ltk1181_Reg.SensorCtrl);
	result = Ltk1181_I2C_ReadByte(Reg_InterruptCtrl, &Ltk1181_Reg.InterruptCtrl);
	result = Ltk1181_I2C_ReadByte(Reg_SoftwareReset, &Ltk1181_Reg.SoftwareReset);
	result = Ltk1181_I2C_ReadByte(Reg_CTC_Step, &Ltk1181_Reg.CTC_Step);
	result = Ltk1181_I2C_ReadWord(Reg_FrameTime_L, &Ltk1181_Reg.FrameTime);

	return result;
}

void Ltk1181_Get_RegisterMap(Ltk1181_Reg_* data)
{
	data->InterruptFlag = Ltk1181_Reg.InterruptFlag;
	data->ErrorFlag = Ltk1181_Reg.ErrorFlag;
	data->PDAT = Ltk1181_Reg.PDAT;
	data->PID = Ltk1181_Reg.PID;
	data->VID = Ltk1181_Reg.VID;
	data->PsCtrl = Ltk1181_Reg.PsCtrl;
	data->PsPulseWidth = Ltk1181_Reg.PsPulseWidth;
	data->PsBurstCount = Ltk1181_Reg.PsBurstCount;
	data->DriverCtrl = Ltk1181_Reg.DriverCtrl;
	data->PsIntCtrl = Ltk1181_Reg.PsIntCtrl;
	data->CTC_Gain = Ltk1181_Reg.CTC_Gain;
	data->CTC_Ctrl = Ltk1181_Reg.CTC_Ctrl;
	data->PsBaseLine = Ltk1181_Reg.PsBaseLine;
	data->PsThresholdLow = Ltk1181_Reg.PsThresholdLow;
	data->PsThresholdHigh = Ltk1181_Reg.PsThresholdHigh;
	data->SensorCtrl = Ltk1181_Reg.SensorCtrl;
	data->InterruptCtrl = Ltk1181_Reg.InterruptCtrl;
	data->SoftwareReset = Ltk1181_Reg.SoftwareReset;
	data->CTC_Step = Ltk1181_Reg.CTC_Step;
	data->FrameTime = Ltk1181_Reg.FrameTime;
}

unsigned char Ltk1181_Enable_PS(unsigned char enable)
{
	unsigned char result;

	Ltk1181_Reg.PEN = enable;
	result = Ltk1181_I2C_WriteByte(Reg_SensorCtrl, Ltk1181_Reg.SensorCtrl);

	return result;
}

unsigned short Ltk1181_Read_PDAT(void)
{
	unsigned char reg;
	//Ltk1181_I2C_ReadWord(Reg_PDAT_L, &Ltk1181_Reg.PDAT);
	
	Ltk1181_I2C_ReadByte(Reg_PDAT_H, &reg);
	Ltk1181_Reg.PDAT = reg;
	Ltk1181_Reg.PDAT <<= 8;
		   
	Ltk1181_I2C_ReadByte(Reg_PDAT_L, &reg);
	Ltk1181_Reg.PDAT |= reg;
		
	logi("*** PDAT = %d", Ltk1181_Reg.PDAT);
	return Ltk1181_Reg.PDAT;
}

unsigned char Ltk1181_Read_PDAT_with_Check_DATIF(unsigned short* PDAT)
{
	unsigned char buff[5];

	Ltk1181_I2C_Read(Reg_InterruptFlag, buff, 4);
	Ltk1181_Reg.InterruptFlag = buff[0];
	Ltk1181_Reg.PDAT = buff[3];
	Ltk1181_Reg.PDAT <<= 8;
	Ltk1181_Reg.PDAT |= buff[2];

	if (Ltk1181_Reg.DATIF)
		PDAT[0] = Ltk1181_Reg.PDAT;
	else
		return 0;

	Ltk1181_Clear_Interrupt();

	return 1;
}

unsigned char Ltk1181_Enable_Auto_CT(unsigned char enable)
{
	unsigned char result;
	unsigned char PEN_buff = Ltk1181_Reg.PEN;

	Ltk1181_Reg.CTC_AUTO = enable;
	if (Ltk1181_Reg.CTC_AUTO)
		Ltk1181_Reg.CTC_EN = 1;

	logi("PEN_buff = %d", PEN_buff);
	
	if(PEN_buff)
		Ltk1181_Enable_PS(0);

	result = Ltk1181_I2C_ReadByte(Reg_CTC_Ctrl, &Ltk1181_Reg.CTC_Ctrl);

	if(PEN_buff)
		Ltk1181_Enable_PS(1);

	return result;
}


unsigned char Ltk1181_Who_am_I(void)
{
	unsigned char result,device_PID,device_VID;
    result = 1;
    
    for (int i = 0; i < 3; i++) {
	result=Ltk1181_I2C_ReadByte(Reg_PID, &device_PID);

	result=Ltk1181_I2C_ReadByte(Reg_VID, &device_VID);

    	if (device_PID != Ltk1181_Reg.PID || device_VID != Ltk1181_Reg.VID)
    	{	
    		logi("**The device is offline!");
    		
    	}else{
            logi("**The device is online");
            break;
        }
    }
	
	return result;
}


AT_SRAM(.sensor)//写校准信息到flash
void Ltk1181_calibration_store_flash(uint8_t flag, uint8_t step)
{
	ltk1181_cal_info_t cal_info_t;
    logi("Ltk1181_calibration_store_flash");
    cal_info_t.cal_flag = flag;
    cal_info_t.ctc_step = step;
    sys_info_write(SENSOR_LTK_CAL_INFO, &cal_info_t, sizeof(cal_info_t));

}

//从flash读校准信息
void Ltk1181_calibration_get_flash(uint8_t *flag, uint8_t *step)
{
    ltk1181_cal_info_t cal_info_t;
    sys_info_read(SENSOR_LTK_CAL_INFO, &cal_info_t, sizeof(cal_info_t));
    if(cal_info_t.cal_flag == 1 ) {
        *flag = 1;
         *step = cal_info_t.ctc_step;
    }else{
        *flag = 0;
        *step = 0;
    }
   
}


unsigned char Ltk1181_calibration(uint8_t *step)
{
	unsigned char result, div, icount=0;
__maybe_unused	unsigned char CTC_Gain=7, CTC_Step=0;
	unsigned short PDAT,PDAT_avg=0;

	//result=Ltk1181_Set_CTC_Gain(Ltk1181_Default_CTC_Gain);
	//result=Ltk1181_Set_CTC_Step(Ltk1181_Default_CTC_Step);
	//result=Ltk1181_Set_FrameTime(30);
	result = Ltk1181_Set_CTC_Step(0);
    HAL_mdelay(250);
	for(int i=0; i<Ltk1181_CTC_Avg_Loop; i++){

		PDAT = Ltk1181_Read_PDAT();
		PDAT_avg = PDAT_avg + PDAT;
		logi("**PDAT = %d, PDAT_avg = %d", PDAT, PDAT_avg);

	}

	PDAT = PDAT_avg/Ltk1181_CTC_Avg_Loop;
	logi("**PDAT Averge = %d, Ltk1181_Default_Full_scale = %d", PDAT, Ltk1181_Default_Full_scale);

	if (PDAT < Ltk1181_Default_Full_scale)
		{
				div = PDAT/Ltk1181_CTC_G7_Step_Values;
				logi("!!! Div = %d", div);
				CTC_Step=div;
		}
	else
		{
			logi("----------While loop------------");
			result=Ltk1181_Set_CTC_Gain(Ltk1181_CTC_G7_Values);
			while(PDAT >= Ltk1181_Default_Full_scale && CTC_Step <= Ltk1181_CTC_Step_limit)
			{
				div = PDAT/Ltk1181_CTC_G7_Step_Values;
				logi("!!! loop = %d, Div = %d", icount, div);
				CTC_Step = CTC_Step + div;
				logi("!!! CTC_Step = %d", CTC_Step);
				//result=Ltk1181_Set_CTC_Gain(CTC_Gain);
				result=Ltk1181_Set_CTC_Step(CTC_Step);
			
				PDAT_avg = 0;
			
				for(int i=0; i<Ltk1181_CTC_Avg_Loop; i++)
				{
					PDAT=Ltk1181_Read_PDAT();
					PDAT_avg = PDAT_avg + PDAT;
					logi("**PDAT = %d, PDAT_avg = %d", PDAT, PDAT_avg);
				}
				
				PDAT = PDAT_avg/Ltk1181_CTC_Avg_Loop;
				icount++;
			}
		
	}

	/*
	else
	{
		result=Ltk1181_Set_CTC_Gain(CTC_Gain);
		result=Ltk1181_Set_CTC_Step(Ltk1181_Default_Full_scale/Ltk1181_CTC_G7_Step_Values);
		PDAT = Ltk1181_Read_PDAT();

		if (PDAT<Ltk1181_Default_Full_scale)
			CTC_Step = (PDAT/Ltk1181_CTC_G7_Values)+(Ltk1181_Default_Full_scale/Ltk1181_CTC_G7_Step_Values);
			else
				{
					logi("CTC_Step bigger than 30 count");
					CTC_Step = 30;
				}
	}*/

	result=Ltk1181_Set_CTC_Gain(Ltk1181_CTC_G7_Values);
	result=Ltk1181_Set_CTC_Step(CTC_Step);
	logi("CTC_Gain = %d, CTC_Step = %d", CTC_Gain, CTC_Step);
//	result=Ltk1181_Set_FrameTime(Ltk1181_Default_FrameTime);
    if (result == 0){
        Ltk1181_calibration_store_flash(1,CTC_Step);
        *step = CTC_Step;
    }
	return result;
}


void Ltk1181_suspend(void)
{
 __maybe_unused unsigned char result;
  result = Ltk1181_Enable_PS(0);

}


void Ltk1181_sensor_deinit(void)
{
    logi("LTK1181_sensor_deinit");
     if (earStateTimer != NULL) {
           OAL_timer_activate(&earStateTimer, OAL_TIMER_NO_ACTIVATE);
     }
    Ltk1181_suspend();

    if (NULL != sensor_proximity_task_handle) {
        sensor_proximity_task_exit = true;

        if (NULL != sensor_proximity_sem) {
            OAL_sem_post(&sensor_proximity_sem);
        }

        while (!sensor_proximity_task_deleted) {
            OAL_sleep(5);
        }
    }
    /* wait idle task free TCB */
    wait_idle_running_a_period(10000);
    sensor_proximity_task_handle = NULL;
}
#endif


