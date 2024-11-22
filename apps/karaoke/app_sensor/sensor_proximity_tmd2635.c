#define LOG_LEV 5
#include "sys_inc.h"
#include "user_config.h"
#include "hal.h"
#include "sensor.h"
#include "sensor_proximity_tmd2635.h"
#include "sys_exception.h"


#if (SENSOR_PROXIMITY_EN && (SENSOR_PROXIMITY_DEVICE_SEL == 2 || SENSOR_PROXIMITY_DEVICE_SEL == 4))

static tmd2635_instance_state tmd2635_state;
tmd2635_instance_state *p_tmd2635_state = &tmd2635_state;
OAL_THREAD tmd2635_proximity_task_handle;
bool volatile tmd2635_proximity_task_exit;
OAL_SEM tmd2635_proximity_sem;
bool tmd2635_proximity_task_deleted;
void* tmd2635_proximity_task(void *para);
void tmd2635_calcual(void);


AT_SRAM(.sensor)
uint32_t iic_write(uint8_t address,uint8_t addr, uint8_t pdata)
{
    int rv = 0;
    uint8_t data[2];
    data[0] = addr;
    data[1] = pdata;
#if TWI_IO_EN
    twi_io_write(address, data, 2);
#else
    rv = twi_write(SENSOR_TMD2635_TWI_ID, address, data, 2);
#endif
    return rv;

}

AT_SRAM(.sensor)
uint32_t iic_read(uint8_t address,uint8_t addr, uint8_t * pdata, uint8_t size)
{
    int rv =0 ;
#if TWI_IO_EN
    twi_io_write(address, &addr, 1);
    twi_io_read(address, pdata, 1);
#else
    rv = twi_write_stop_ctl(SENSOR_TMD2635_TWI_ID, address, &addr, size,0);
    rv = twi_read(SENSOR_TMD2635_TWI_ID, address, pdata, size);
#endif
    return rv;

}

AT_SRAM(.sensor)
uint32_t tmd2635_inst_i2c_read_byte(tmd2635_instance_state *state,
                                                   uint8_t reg,
                                                   uint8_t *buffer)
{
    uint32_t rv = NRF_SUCCESS;
    rv = iic_read(state->address, reg, buffer, 1);
 
	return rv;
}

//static uint32_t tmd2635_inst_i2c_read_block(tmd2635_instance_state *state,
//                                                     uint8_t reg,
//                                                     uint8_t *buffer,
//                                                     uint32_t len)
//{
//    uint32_t rv = NRF_SUCCESS;
//    rv = iic_read(state->address, reg, buffer, (uint8_t)len);
//	
//	return rv;
//}

AT_SRAM(.sensor)
static uint32_t tmd2635_inst_i2c_write_byte(tmd2635_instance_state *state,
                                                     uint8_t reg,
                                                     uint8_t val)
{
    uint32_t rv = NRF_SUCCESS;

    rv = iic_write(state->address, reg, val);
	return rv;
}



uint32_t tmd2635_get_who_am_i(tmd2635_instance_state *state, uint8_t *buffer)
{
    uint32_t rv = 0;
    int ret = 0;
        for (uint32_t i=0;i < 5;i++){
             ret = HAL_twi_ack_check(SENSOR_TMD2635_TWI_ID, state->address);
             if (ret == 0) {
                 rv = iic_read(state->address, TMD2635_ID_REG, buffer, 1);
                      if (*buffer == 0x44 ) {
                        logi("proximity-sensor is online id:0x%x rv:%d",  *buffer,rv);
                        return true;
                }
            } else {
                logi("tmd2635 no ack");
            }
            HAL_mdelay(100);
         }

	return rv;
 
}

/**
 * Loads default values in config registers.
 *
 * @param[i] state       state of sensor instance
 *
 * @return uint32_t
 * SNS_RC_FAILED - COM port failure
 * NRF_SUCCESS
 */
 uint32_t tmd2635_device_set_default(tmd2635_instance_state *state)
{
    uint32_t rv = NRF_SUCCESS;
    uint8_t i;
    tmd2635_reg_setting default_setting[] = {
        {TMD2635_REG_ENABLE,     PWDIS|PON}, // PON-pwr on
          {TMD2635_PRATE_REG,    PTIME_US(3000)},
         // {TMD2635_REG_PRATE_PROX_TIME,   0x1F}, // default value
          {TMD2635_PILTL_REG, 0xB8},// 0x0F}, //
                {TMD2635_PILTH_REG, 0x0B},// 0xA0}, //3000
                {TMD2635_PIHTL_REG, 0x70}, // 
                {TMD2635_PIHTH_REG, 0x17}, //6000
                {TMD2635_PWTIME_REG,   PWTIME_MS(50)}, 
                {TMD2635_CFG0_REG,     CFG0_RESERVED|NO_PWLONG},
                {TMD2635_PCFG0_REG,    (PGAIN_2X|PPULSES(15))},
                {TMD2635_PCFG1_REG,    (PPLEN_8US|PLDRIVE_MA(4))},
                {TMD2635_CFG8_REG,    PD_NEAR},
                {TMD2635_REG_PERS_INT_FILTERS,   0x03}, // 3 consecutive proximtiy values out of range
                {TMD2635_REG_CFG3_CONFIG3,    INT_READ_CLEAR|CFG3_RESERVED1},
                {TMD2635_CFG6_REG,    (APC_ENABLE|CFG6_RESERVED)},
                {TMD2635_CALIBCFG_REG, BINSRCH_TARGET_15|CALIBCFG_RESERVED|AUTO_OFFSET_ADJ|PROX_DATA_AVG_4},
                {TMD2635_INTENAB_REG,  PIM_STATE_BASED|PIEN_TMD|PSIDIS|CIEN|ZIDIS},
                {TMD2635_REG_POFFSETL_PROX_OFFSET_LOW,    220},//  offset  最大值255
                {TMD2635_REG_POFFSETH_PROX_OFFSET_HIGH,   0},//符号位，1为负，0为正。
    };

    for (i = 0; i < sizeof(default_setting)/sizeof(default_setting[0]); i++)
    {
        for(int j = 0; j < 5; j++) {
            rv = tmd2635_inst_i2c_write_byte(state,
                        default_setting[i].reg,
                        default_setting[i].value);
            if (rv == 0) {
                break;
            } else {
                logi ("write default_setting[i].reg:0x%x,val:0x%x,ret:%d",default_setting[i].reg,default_setting[i].value,rv);
            }

        }
    }
    logi("tmd2635 tmd2635_device_set_default!!");
    return rv;
}



bool tmd2635_sensor_online(void)
{
      uint8_t id;
      p_tmd2635_state->address = 0x39;
      p_tmd2635_state->publish_sensors = TMD2635_PROX;
      p_tmd2635_state->is_dri = true;
     tmd2635_get_who_am_i(p_tmd2635_state,&id);
     logi("!!tmd2635 current id:0x%x",id);
     if (id==0x44){
        return true;
     } else {
        return false;
     }

}


uint8_t calib_flag,TMD2635_ID,OFFSET;
OAL_TIMER tmd2635_reset_timer;
#define TMD2635_RESET_RECORD_PERIOD         500
uint8_t tmd2635_low_count = 0;
bool cal_tmd2635_flag = 0;

/*
AT_SRAM(.sensor)
void tmd2635_reset_timer_cb(void* xTimer)
{
   if (!HAL_gpio_value_get(SENSOR_PROXIMITY_INT_PIN_NUM)){
    tmd2635_low_count++;
  }else{
    tmd2635_low_count = 0;
  }

  if (tmd2635_low_count >= 3) {
    logi("tmd2635 software reset");
    tmd2635_inst_i2c_write_byte(p_tmd2635_state,TMD2635_REG_SOFTRST_SOFT_RESET,0x01);
    HAL_gpio_irq_config(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_IRQ_DIS);
    tmd2635_device_set_default(p_tmd2635_state);
    tmd2635_resume();
    HAL_gpio_irq_config(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_F_EDGE);
  }

}*/

void tmd2635_suspend(void)
{
  int ret;
  for (int i = 0; i<3; i++) {
      ret=tmd2635_inst_i2c_write_byte(p_tmd2635_state,TMD2635_REG_ENABLE,0x10);
      if (ret == 0) {
          logi("**tmd2635_suspend,ret=%d",ret);
            break;
      }
  }
}


void tmd2635_resume(void)
{
  int ret = 0;
  for (int i = 0; i<3; i++) {
      ret = tmd2635_inst_i2c_write_byte(p_tmd2635_state,TMD2635_REG_ENABLE,0x15);
      if (ret == 0) {
        logi("**tmd2635_resume ret=%d",ret);
        break;
      }
  }
}



AT_SRAM(.sensor)
void tmd2635_calcual(void)
{

#if 1
        logi("tmd2635 calib start");

        uint8_t  status = 0,CALIBSTAT = 0;
        uint8_t  offset_l = 0, offset_h = 0,ENABLE=0x15;
                        //      0    1    2    3    4    5    6    7    8   9  10   11   12
        uint8_t CLIB_data[16]={0x01,0xFF,0x5A,0x98,0x01,0x3F,0x15,0X04,0X62,0,0x84,0x1f,0x02};
        calib_flag = 0;
    
        for (int i = 0; i < 5; i++) {
           //执行无遮挡校准测试方式
            iic_read(p_tmd2635_state->address,TMD2635_STATUS_REG,&status,1);//读取状态位,清除状态寄存器。
            iic_write(p_tmd2635_state->address,TMD2635_REG_ENABLE,CLIB_data[0]);//配置只有PON为1，使能晶振
            iic_write(p_tmd2635_state->address,TMD2635_CALIBCFG_REG,CLIB_data[2]);//配置校准寄存器
            iic_write(p_tmd2635_state->address,TMD2635_CALIB_REG,CLIB_data[4]);//开始校准
            HAL_mdelay(30);////校准延时30ms
            iic_read(p_tmd2635_state->address,TMD2635_CALIBSTAT_REG,&CALIBSTAT,1);//读取校准状态
            iic_read(p_tmd2635_state->address,TMD2635_STATUS_REG,&status,1);//读取状态位
            if (0x08 == (status&0x08))//如果校准完成，读取POFFSET值
            {
            calib_flag=1;//校准成功
            //OLED_init();
            //读取OFFSET后，将数据存起来，下次启动写入到OFFSET寄存器，同时sensor会自己将启用新offset值。
            iic_read(p_tmd2635_state->address,TMD2635_STATUS_REG,&status,1);
            iic_read(p_tmd2635_state->address,TMD2635_POFFSETL_REG,&offset_l,1);
            iic_read(p_tmd2635_state->address,TMD2635_POFFSETH_REG,&offset_h,1);
                //M_I2C_ReadByte(TMD2635,TMD2635_STATUS_REG,&CLIB_data[7],1);
            iic_write(p_tmd2635_state->address,TMD2635_REG_ENABLE,ENABLE);
            OFFSET=(offset_h<<8|offset_l);
                logi("tmd2635 calib ok OFFSET:%d",OFFSET);
                break;
            }
            else
            {
            calib_flag=0;//校准失败
            logi("tmd2635 calib fail");
            }
       }
 #endif


}


void tmd2635_start(void)
{
    int ret = 0;

    logi("tmd2635 start work!!");
    
    tmd2635_proximity_task_deleted = false;
    tmd2635_proximity_task_exit = false;
    
    if (tmd2635_proximity_task_handle == NULL) {
        logi("tmd2635 start tmd2635_proximity_task!!");
        tmd2635_proximity_task_handle = OAL_thread_create(tmd2635_proximity_task, NULL, "task proximty", 1, 256);
    }
  
    if (tmd2635_proximity_sem == NULL)
        tmd2635_proximity_sem = OAL_sem_create_binary();

    for (int i = 0; i < 3; i++) {
        ret = tmd2635_inst_i2c_write_byte(p_tmd2635_state,TMD2635_REG_ENABLE,0x15);
        if (ret == 0) {
            break;
        }
    }
    tmd2635_low_count = 0;
    cal_tmd2635_flag = 0;
    /*if (tmd2635_reset_timer == 0) {
        OAL_timer_create(&tmd2635_reset_timer, "tmd2635_reset_timer", OAL_ms_to_ticks(TMD2635_RESET_RECORD_PERIOD),
        tmd2635_reset_timer_cb, (void *)1, 1, OAL_TIMER_AUTO_ACTIVATE);
    } else {
        OAL_timer_activate(&tmd2635_reset_timer, OAL_TIMER_AUTO_ACTIVATE);
    }*/

}
AT_SRAM(.sensor)
void tmd2635_get_datat_cb(void)
{
  if (NULL != tmd2635_proximity_sem) {
        logi("tmd2635_get_datat_cb");
      OAL_sem_post(&tmd2635_proximity_sem);
  }
}



AT_SRAM(.sensor)
void* tmd2635_proximity_task(void *para)
{
   logi("!!tmd2635_proximity_task");
    while (1) {
         if (tmd2635_proximity_task_exit) {
            
            logi("!!tmd2635_proximity_task exit 1");
            break;
        }
        OAL_sem_wait(&tmd2635_proximity_sem);

        if (tmd2635_proximity_task_exit) {
            
            logi("!!tmd2635_proximity_task exit 2");
            break;
        }
        uint8_t TMD2635_INT;
#if 0 
        uint8_t PDATAL_REG;
        iic_read(p_tmd2635_state->address,TMD2635_PDATAL_REG,&PDATAL_REG,1);

        uint8_t PDATAH_REG;
        iic_read(p_tmd2635_state->address,TMD2635_PDATAH_REG,&PDATAH_REG,1);
        uint16_t pdata;
        pdata = (PDATAH_REG<<8)|PDATAL_REG;
#endif
        iic_read(p_tmd2635_state->address,TMD2635_STATUS_REG,&TMD2635_INT,1);//读取阈值溢出中断标志寄存器
        /*uint8_t offset=0;

        
         iic_read(p_tmd2635_state->address,TMD2635_POFFSETL_REG,&offset,1);*/

     //    logi(" PDATA:%d,  TMD2635_STATUS_REG:0x%x",pdata, TMD2635_INT);
        
        if((TMD2635_INT>>6)==2)//STATUS PHIGH 寄存器标志位，Pdata溢出高阈值
        {
            logi(" >>>IN  EAR<<< ");
            event_put(SENSOR_EVENT_P_HT, NULL, 0);
           // tmd2635_low_count++;
        }
        if((TMD2635_INT>>6)==1)//STATUS PLOW 寄存器标志位，Pdata低于低阈值
        {
            logi(" <<<OUT EAR>>>");
            event_put(SENSOR_EVENT_P_LT, NULL, 0);
        }

        if(OAL_SUCCESS == OAL_thread_wait_join(tmd2635_proximity_task_handle, 0)) {
            
           logi("!!tmd2635_proximity_task exit 3");
           break;
        }
    }
    tmd2635_proximity_task_deleted = true;

    OAL_thread_delete_self(tmd2635_proximity_task_handle);
     if (NULL != tmd2635_proximity_sem) {
        OAL_sem_destroy(&tmd2635_proximity_sem);
        tmd2635_proximity_sem = NULL;
    }
    return NULL;
}


void tmd2635_sensor_init(void)
{
    tmd2635_device_set_default(p_tmd2635_state);
    //TMD2635_CLIB(p_tmd2635_state);
}


void tmd2635_sensor_deinit(void)
{
    logi("tmd2635_sensor_deinit");
  /*  if (tmd2635_reset_timer) {
        OAL_timer_activate(&tmd2635_reset_timer, OAL_TIMER_NO_ACTIVATE);
    }*/
    tmd2635_suspend();

    if (NULL != tmd2635_proximity_task_handle) {
        tmd2635_proximity_task_exit = true;

        if (NULL != tmd2635_proximity_sem) {
            OAL_sem_post(&tmd2635_proximity_sem);
        }

        while (!tmd2635_proximity_task_deleted) {
            OAL_sleep(5);
        }
    }
    /* wait idle task free TCB */
    wait_idle_running_a_period(10000);
    tmd2635_proximity_task_handle = NULL;
}
#endif

