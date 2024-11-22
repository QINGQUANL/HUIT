#define LOG_LEV 5
#include "sys_inc.h"
#include "user_config.h"
#include "sensor.h"
#include "ui.h"
#include "hal.h"
#include "psm_manager.h"
#include "tws_profile.h"
#include "tws_api.h"
#include "air_api.h"
#include "psm_manager.h"
#include "info_mem.h"

#if (SENSOR_PROXIMITY_EN && (SENSOR_PROXIMITY_DEVICE_SEL == 2 ||SENSOR_PROXIMITY_DEVICE_SEL == 4))
#include "sensor_proximity_tmd2635.h"
#endif
#if (SENSOR_PROXIMITY_EN && (SENSOR_PROXIMITY_DEVICE_SEL == 3 ||SENSOR_PROXIMITY_DEVICE_SEL == 4))
#include "LTK1181.h"
#endif

#if SENSOR_PROXIMITY_EN

bool sensor_proximity_online_sta;               //在线状态（根据读ID返回状态）
bool sensor_proximity_ht_first;                 //初始化时清0，首次ht（入耳）后置1
bool sensor_proximity_lt_sta;                   //在出耳暂停音乐时置1，2秒后清0，用于辅助出/入耳控制播放状态
uint32_t sensor_proximity_lt_cnt;               //用于2秒计时
uint8_t sensor_proximity_ps_sta;                //当前耳机的出/入耳状态，主/副耳上都记录
uint8_t sensor_proximity_ps_sta_ews;            //对方耳机的出/入耳状态，主/副耳上都记录


#if SENSOR_PROXIMITY_NO_PLAY_WHEN_LT_TO
bool sensor_proximity_lt_timeout_sta;           //lt超时状态，初始为0，持续30秒后置1
OAL_TIMER sensor_proximity_lt_timer_handle;     //lt状态后计时
#endif
bool sensor_proximity_ht_need_play;             //0-入耳不需播放，1-入耳需播放

volatile bool proximity_timer_start;
volatile uint32_t proximity_timer_cnt;
volatile bool proximity_sta;
volatile uint32_t proximity_in_cnt;
volatile uint32_t proximity_out_cnt;
uint8_t sensor_reset_flag;
uint8_t sensor_reset_cnt;



__maybe_unused static uint8_t proximity_sensor_select;

const uint8_t sensor_p_pa22409001_init_tbl[] = {
    //reg,value
    /*0x00
     * [1]-PSEN:PS operation mode:0:Disable
     */
     0x00,0x00,

    /*0x01
    * [6:4]-LESC: Light Emitting Source current setting, 110 : 10mA,
    * [3:2]-PSPRST: PS interrupt persistence,01: 2,
    */
    0x01,0x64,

    /*0x02
    * [6]-PSMODESEL:PS offset mode selection:0: Offset measurement
    * [4]-CMR:Reset command:0: Clear command reset
    * [3:2]-INTSEL:Select source of interrupt flag:01: PS
    * [1]-PS INTF:PS interrupt flag:0: Inactive
    */
    0x02,0x04,

    /*0x03
    * [6]-PITYPE:Interrupt type of PS:1: Hysteresis type
    * [5:3]-PSSLP:PS sleep time:110: 400ms
    */
    0x03,0x70,

    /*0x08
    * [7:0]-PSLT[7:0]:PS Low_Threshold byte:PS Low_Threshold level setting
    */
    0x08,0x3c,

    /*0x0a
    * [7:0]-PSHT[7:0]:PS High_Threshold byte
    */
    0x0a,0x6e,

    /*0x11
    * [7:0]-PSADMOD[7:0]:PS AD mode
    */
    0x11,0x82,

    /*0x12
    * [6:4]-FLTFC:Digital filter coefficient:000: None
    * [3:2]-PSPDS:PS PD Setting:11
    * [1]-IRLP:IR Light Pulse:0: 32 pulse
    */
    0x12,0x0c,

    /*0x00
    * [1]-PSEN:PS operation mode:1:Enable
    */
    0x00,0x02,
};

void sensor_proximity_init(void)
{

    sensor_proximity_ht_first = false;

#if SENSOR_TEST_EN
    sensor_proximity_online_sta = true;
#else


#if (SENSOR_PROXIMITY_DEVICE_SEL == 0)
    if (sensor_proximity_online()) {
        sensor_proximity_online_sta = true;
    } else {
        sensor_proximity_online_sta = false;
        return;
    }

    /* int gpio初始化，function设置为EINT，开内部上拉，开上升沿/下降沿中断 */
    HAL_gpio_pull_set(SENSOR_PROXIMITY_INT_PIN_NUM, SENSOR_PROXIMITY_INT_PIN_PULL);
    HAL_gpio_func_set(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_FUNCEINT);
    HAL_gpio_irq_clk_config(SENSOR_PROXIMITY_INT_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
    HAL_gpio_irq_config(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);

    /* 寄存器初始化 */
    /* PA22409001 */
    for (uint8_t i=0;i<sizeof(sensor_p_pa22409001_init_tbl);i=i+2) {
        sensor_proximity_write(sensor_p_pa22409001_init_tbl[i], sensor_p_pa22409001_init_tbl[i+1]);
    }

#elif (SENSOR_PROXIMITY_DEVICE_SEL == 1)
    if (sensor_proximity_online()) {
        sensor_proximity_online_sta = true;
    } else {
        sensor_proximity_online_sta = false;
        return;
    }

#if SENSOR_PROXIMITY_DETECT_MODE
    /* Vout gpio初始化，function设置为EINT，开内部下拉，开上升沿 */
    HAL_gpio_pull_set(SENSOR_PROXIMITY_INT_PIN_NUM, SENSOR_PROXIMITY_INT_PIN_PULL);
    HAL_gpio_func_set(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_FUNCEINT);
    HAL_gpio_irq_clk_config(SENSOR_PROXIMITY_INT_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
    HAL_gpio_irq_config(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_R_EDGE);
#else
    /* Vout gpio初始化，function设置为INPUT,        默认是低电平，高电平有效 */
    HAL_gpio_config(SENSOR_PROXIMITY_INT_PIN_NUM|PIN_FUNCINPUT|PIN_DRI3|PIN_LOW|PIN_PULL_DISABLE);
#endif

#if SENSOR_PROXIMITY_CTL_PIN_ACTIVE_STATE
    HAL_gpio_config(SENSOR_PROXIMITY_CTL_PIN_NUM|PIN_FUNCOUTPUT|PIN_DRI3|PIN_LOW|PIN_PULL_DISABLE);
 #else
    /* 控制脚，低电平导通有效 */
    HAL_gpio_config(SENSOR_PROXIMITY_CTL_PIN_NUM|PIN_FUNCOUTPUT|PIN_DRI3|PIN_HIGH|PIN_PULL_DISABLE);
#endif
    proximity_timer_cnt = 0;
    proximity_sta = 0;
    proximity_in_cnt = 0;
    proximity_out_cnt = 0;
    proximity_timer_start = 1;

    sensor_proximity_ps_sta_set(false);
    sensor_proximity_ps_sta_ews_set(false);
#elif (SENSOR_PROXIMITY_DEVICE_SEL == 2)
        if (sensor_proximity_online_sta == 1) {
        HAL_gpio_irq_config(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_F_EDGE);
        tmd2635_start();
        sensor_proximity_ps_sta_set(false);
        sensor_proximity_ps_sta_ews_set(false);
        proximity_timer_start = 1;
       }

#elif (SENSOR_PROXIMITY_DEVICE_SEL == 3)
     /* int gpio初始化，function设置为EINT，开内部上拉，开下降沿中断 */
    if (sensor_proximity_online()) {
           sensor_proximity_online_sta = true;
    } else {
          sensor_proximity_online_sta = false;
       return;
    }
    Ltk1181_Initial(1);// LTK1181 start!
    HAL_gpio_pull_set(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_PULL_UP100K);
    HAL_gpio_func_set(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_FUNCEINT);
    HAL_gpio_irq_clk_config(SENSOR_PROXIMITY_INT_PIN_NUM, (0xff<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
    HAL_gpio_irq_config(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_F_EDGE);
    sensor_proximity_ps_sta_set(false);
    sensor_proximity_ps_sta_ews_set(false);
    proximity_timer_start = 1;
#elif (SENSOR_PROXIMITY_DEVICE_SEL == 4)
    if (sensor_proximity_online_sta == false) {
        return;
    }
    if (proximity_sensor_select == TMD2635) {
        HAL_gpio_irq_config(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_F_EDGE);
        tmd2635_start();
    } else if (proximity_sensor_select == LTK1181) {
        Ltk1181_Initial(1);// LTK1181 start!
        HAL_gpio_pull_set(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_PULL_UP100K);
        HAL_gpio_func_set(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_FUNCEINT);
        HAL_gpio_irq_clk_config(SENSOR_PROXIMITY_INT_PIN_NUM, (0xff<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
        HAL_gpio_irq_config(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_F_EDGE);
    }
    sensor_proximity_ps_sta_set(false);
    sensor_proximity_ps_sta_ews_set(false);
    proximity_timer_start = 1;

#endif

    sensor_proximity_ht_need_play_set(false);
#if SENSOR_PROXIMITY_NO_PLAY_WHEN_LT_TO
    sensor_proximity_lt_timer_create();
#endif
#endif

}

#if (SENSOR_PROXIMITY_DEVICE_SEL == 2 || SENSOR_PROXIMITY_DEVICE_SEL == 4)

void sensor_proximit_id_check(void)
{
 logi("sensor_proximit_id_check");

#if SENSOR_PROXIMITY_DEVICE_SEL == 4
    if(Ltk1181_Who_am_I() == 0){
        logi("ltk1181 online");
        proximity_sensor_select = LTK1181;
        sensor_proximity_online_sta = true;
    } else 
#endif
    {

        if (tmd2635_sensor_online()) {
            logi("tmd2635 online");
            proximity_sensor_select = TMD2635;
            sensor_proximity_online_sta = true;
        }
    }
#if 1
    if (!sensor_proximity_online_sta) {
        sensor_proximity_info_get();
        sensor_reset_flag = 1;
        sensor_reset_cnt++;
        sensor_proximity_info_set(sensor_reset_flag,sensor_reset_cnt);
        if(sensor_reset_cnt < 3) {
            logi("HAL_restart_pmu");
#if (SENSOR_PROXIMITY_EN && (SENSOR_PROXIMITY_DEVICE_SEL == 2 || SENSOR_PROXIMITY_DEVICE_SEL == 4))
            pmu_reset_time_set(1000);
#endif
            HAL_restart_pmu();
        }
    } else {
        if (sensor_reset_flag == 1) {
            sensor_reset_flag = 0;
            sensor_reset_cnt = 0;
            sensor_proximity_info_set(sensor_reset_flag,sensor_reset_cnt);
         }
    }
#endif
}
#endif


void sensor_proximity_mode_init(void)
{
    logi("sensor_proximity_mode_init");
#if (SENSOR_PROXIMITY_DEVICE_SEL == 2 || SENSOR_PROXIMITY_DEVICE_SEL == 4)
       sensor_proximit_id_check();
        /* int gpio初始化，function设置为EINT，开内部上拉，开下降沿中断 */
       if (proximity_sensor_select == TMD2635 && sensor_proximity_online_sta) {
            tmd2635_sensor_init();
        }
#endif
}

uint8_t sensor_proximity_get_select(void)
{
    return proximity_sensor_select;
}


#if 1
uint8_t sensor_proximity_reset_flag_get(void)
{

    return sensor_reset_flag;
}

uint8_t sensor_proximity_reset_cnt_get(void)
{

    return sensor_reset_cnt;
}


void sensor_proximity_info_get(void)
{
    uint8_t param[2];

    sys_info_read(SENSOR_RESET_INFO, param, 2);
    if(param[0]==0xff){
        param[0] = 0;
    }
    if(param[1] == 0xff){
        param[1] = 0;
    }

    sensor_reset_flag = param[0];
    sensor_reset_cnt = param[1];
    logi("**info_get,sensor_reset_flag:%d,sensor_reset_cnt:%d",sensor_reset_flag,sensor_reset_cnt);

}

void sensor_proximity_info_set(uint8_t reset_flag, uint8_t reset_cnt)
{

    uint8_t param[2];

    param[0] = reset_flag;
    param[1] = reset_cnt;
    sys_info_write(SENSOR_RESET_INFO, param, 2);

    logi("**info_set,sensor_reset_flag:%d,sensor_reset_cnt:%d",reset_flag,reset_cnt);
}

#endif

void sensor_proximity_pre_init(void)
{
     logi("sensor_proximity_pre_init");
#if (SENSOR_PROXIMITY_DEVICE_SEL == 2 || SENSOR_PROXIMITY_DEVICE_SEL == 4)
     sensor_proximit_id_check();
    /* int gpio初始化，function设置为EINT，开内部上拉，开下降沿中断 */
   if (proximity_sensor_select == TMD2635 && sensor_proximity_online_sta) {
        HAL_gpio_pull_set(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_PULL_UP100K);
        HAL_gpio_func_set(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_FUNCEINT);
        HAL_gpio_irq_clk_config(SENSOR_PROXIMITY_INT_PIN_NUM, (0xff<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
      //  HAL_gpio_irq_config(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_F_EDGE);
        HAL_gpio_irq_config(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_IRQ_DIS);
    }


#endif

}


void sensor_proximity_deinit(void)
{
    sensor_proximity_online_sta = false;
#if SENSOR_PROXIMITY_NO_PLAY_WHEN_LT_TO
    sensor_proximity_lt_timer_stop();
#endif
    HAL_gpio_irq_config(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_IRQ_DIS);

#if (SENSOR_PROXIMITY_DEVICE_SEL == 0)
    /* PA22409001 */
    sensor_proximity_write(SENSOR_PROXIMITY_REG_PS_EN, 0x00);
#elif (SENSOR_PROXIMITY_DEVICE_SEL == 1)
    proximity_timer_start = 0;
    HAL_gpio_config(SENSOR_PROXIMITY_INT_PIN_NUM|PIN_FUNCDISABLE|PIN_PULL_DISABLE);
    HAL_gpio_config(SENSOR_PROXIMITY_CTL_PIN_NUM|PIN_FUNCDISABLE|PIN_PULL_DISABLE);
#elif (SENSOR_PROXIMITY_DEVICE_SEL == 2)
    proximity_timer_start = 0;
    tmd2635_sensor_deinit();
    HAL_gpio_irq_config(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_IRQ_DIS);
    HAL_gpio_config(SENSOR_PROXIMITY_INT_PIN_NUM|PIN_FUNCDISABLE|PIN_PULL_UP100K);
#elif (SENSOR_PROXIMITY_DEVICE_SEL == 3)
    proximity_timer_start = 0;
    Ltk1181_sensor_deinit();
    HAL_gpio_irq_config(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_IRQ_DIS);
    HAL_gpio_config(SENSOR_PROXIMITY_INT_PIN_NUM|PIN_FUNCDISABLE|PIN_PULL_UP100K);
    
#elif (SENSOR_PROXIMITY_DEVICE_SEL == 4)
        proximity_timer_start = 0;
        if (proximity_sensor_select == TMD2635) {
            tmd2635_sensor_deinit();
        } else if (proximity_sensor_select == LTK1181) {
            Ltk1181_sensor_deinit();
         }
        HAL_gpio_irq_config(SENSOR_PROXIMITY_INT_PIN_NUM, PIN_IRQ_DIS);
        HAL_gpio_config(SENSOR_PROXIMITY_INT_PIN_NUM|PIN_FUNCDISABLE|PIN_PULL_UP100K);
#endif
}

bool sensor_proximity_online(void)
{
#if (SENSOR_PROXIMITY_DEVICE_SEL == 0)
    /* PA22409001 */

    uint8_t chip_id;

    /* 校验CHIP ID */
    for (uint32_t i=0;i<3;i++) {
        chip_id = sensor_proximity_read(SENSOR_PROXIMITY_REG_CHIP_ID);
        if (SENSOR_PROXIMITY_CHIP_ID == chip_id) {
            logi("proximity-sensor is online");
            return true;
        }
        HAL_mdelay(100);
    }

    loge("proximity-sensor error,chip_id should be 0x11,but is %x", chip_id);
    return false;

#elif (SENSOR_PROXIMITY_DEVICE_SEL == 1)
    /* HY2751 */
    return true;
#elif (SENSOR_PROXIMITY_DEVICE_SEL == 2 || SENSOR_PROXIMITY_DEVICE_SEL == 4)
    if (tmd2635_sensor_online()){
        return true;
    } else {
        return false;
    }
#elif (SENSOR_PROXIMITY_DEVICE_SEL == 3)
    if(Ltk1181_Who_am_I() == 0){
        return true;
    }else{
        return false;
    }
    
#endif
}

bool sensor_proximity_online_sta_get(void)
{
    return sensor_proximity_online_sta;
}


/*------------------------------------------------------------------------------
*函数功能:设置当前出入耳状态
*参数1：入耳-true
        出耳-false
*-------------------------------------------------------------------------------*/
AT(.sensor_sram_seg)
void sensor_proximity_ps_sta_set(uint8_t sta)
{
    //logi("@@ ps_sta:%d",sta);
    sensor_proximity_ps_sta = sta;
}

/*------------------------------------------------------------------------------
*函数功能:获取当前出入耳状态
*参数1：入耳-true
        出耳-false
*-------------------------------------------------------------------------------*/
AT_SRAM(.sensor)    /* 中断有用到 */
uint8_t sensor_proximity_ps_sta_get(void)
{
#if (SENSOR_PROXIMITY_DEVICE_SEL == 0)
    if (HAL_gpio_value_get(SENSOR_PROXIMITY_INT_PIN_NUM)) {
        return false;
    } else {
        return true;
    }

#else
    if (sensor_proximity_online_sta) {

       // logi("~~ get ps_sta:%d",sensor_proximity_ps_sta);
        return sensor_proximity_ps_sta;
    } else {
        return true;
    }
#endif
}


/*------------------------------------------------------------------------------
*函数功能:设置对方出入耳状态
*参数1：入耳-true
        出耳-false
*-------------------------------------------------------------------------------*/

void sensor_proximity_ps_sta_ews_set(uint8_t sta)
{
    //logi("&& ps_sta_ews:%d",sta);
    sensor_proximity_ps_sta_ews = sta;
}

/*------------------------------------------------------------------------------
*函数功能:获取对方出入耳状态
*参数1：入耳-true
        出耳-false
*-------------------------------------------------------------------------------*/
uint8_t sensor_proximity_ps_sta_ews_get(void)
{
    // logi("!! get ps_sta_ews:%d",sensor_proximity_ps_sta_ews);
    return sensor_proximity_ps_sta_ews;
}


AT_SRAM(.sensor)
void sensor_proximity_interrupt(void)
{
#if (SENSOR_PROXIMITY_DEVICE_SEL == 0)
    if (HAL_gpio_irq_pending_get(SENSOR_PROXIMITY_INT_PIN_NUM)) {
        HAL_gpio_irq_pending_clear(SENSOR_PROXIMITY_INT_PIN_NUM);

        if (sensor_proximity_online_sta) {
            if (SENSOR_PROXIMITY_PIN_ACTIVE_STATE == HAL_gpio_value_get(SENSOR_PROXIMITY_INT_PIN_NUM)) {
                /*近距离，
                * When the sensing data is more than the high threshold, interrupt becomes active.
                */
                if (HAL_pmu_vbus_state_get()) {
                    //在vbus in中，无效
                    logi("sensor ht,vbus in,return");
                    return;
                }
                logi("sensor p,ht");
                sensor_proximity_ht_first = true;
                event_put(SENSOR_EVENT_P_HT, NULL, 0);
            } else {
                /*远距离，
                * When the sensing data is less than the low threshold, interrupt is cleared.
                */

                if (HAL_pmu_vbus_state_get()) {
                    //在vbus in中，无效
                    logi("sensor lt,vbus in,return");
                    return;
                }
                if (!sensor_proximity_ht_first) {
                    //在未曾HT时，LT无效
                    logi("sensor lt,first,return");
                    return;
                }
                logi("sensor p,lt");
                event_put(SENSOR_EVENT_P_LT, NULL, 0);
            }
        }
    }
#elif (SENSOR_PROXIMITY_DEVICE_SEL == 1)
#if SENSOR_PROXIMITY_DETECT_MODE
        if (HAL_gpio_irq_pending_get(SENSOR_PROXIMITY_INT_PIN_NUM)) {
            HAL_gpio_irq_pending_clear(SENSOR_PROXIMITY_INT_PIN_NUM);

            if (sensor_proximity_online_sta) {
#if SENSOR_PROXIMITY_CTL_PIN_ACTIVE_STATE
                HAL_gpio_value_set(SENSOR_PROXIMITY_CTL_PIN_NUM, 0);
#else
                HAL_gpio_value_set(SENSOR_PROXIMITY_CTL_PIN_NUM, 1);
#endif
                proximity_in_cnt++;
                proximity_out_cnt = 0;
                if (!proximity_sta && (proximity_in_cnt == 10)) {
                    proximity_sta = 1;
                    if (pmu_ear_in_case_state_get()) {
                        return;
                    }
                    event_put(SENSOR_EVENT_P_HT, NULL, 0);
                    //logi("<<in>>");
                }
            }
        }
#endif

#elif (SENSOR_PROXIMITY_DEVICE_SEL == 2)
        if (sensor_proximity_online_sta && proximity_timer_start){
        if (HAL_gpio_irq_pending_get(SENSOR_PROXIMITY_INT_PIN_NUM)) {
            HAL_gpio_irq_pending_clear(SENSOR_PROXIMITY_INT_PIN_NUM);
            tmd2635_get_datat_cb();
        }
       }
#elif (SENSOR_PROXIMITY_DEVICE_SEL == 3)
        if (sensor_proximity_online_sta && proximity_timer_start){
        if (HAL_gpio_irq_pending_get(SENSOR_PROXIMITY_INT_PIN_NUM)) {
            HAL_gpio_irq_pending_clear(SENSOR_PROXIMITY_INT_PIN_NUM);
            Ltk1181_get_datat_cb();
        }
       }
#elif (SENSOR_PROXIMITY_DEVICE_SEL == 4)
        if (sensor_proximity_online_sta && proximity_timer_start) {
        if (HAL_gpio_irq_pending_get(SENSOR_PROXIMITY_INT_PIN_NUM)) {
            HAL_gpio_irq_pending_clear(SENSOR_PROXIMITY_INT_PIN_NUM);
           if(proximity_sensor_select == LTK1181) {
                Ltk1181_get_datat_cb();
           } else if (proximity_sensor_select == TMD2635) {
             tmd2635_get_datat_cb();
           }
        }
      }
#endif

}


#if SENSOR_PROXIMITY_EN
AT_SRAM(.sensor)
void sensor_proximity_lt_timer(void)
{
#if (SENSOR_PROXIMITY_DEVICE_SEL == 1)
#if SENSOR_PROXIMITY_DETECT_MODE
    if (proximity_timer_start) {
        proximity_timer_cnt++;
        if (proximity_timer_cnt % 10 == 0) { //5ms*10=50ms
#if SENSOR_PROXIMITY_CTL_PIN_ACTIVE_STATE
            HAL_gpio_value_set(SENSOR_PROXIMITY_CTL_PIN_NUM, 1);
#else

            HAL_gpio_value_set(SENSOR_PROXIMITY_CTL_PIN_NUM, 0);
#endif
        } else if (proximity_timer_cnt % 10 == 1) {
#if SENSOR_PROXIMITY_CTL_PIN_ACTIVE_STATE
            HAL_gpio_value_set(SENSOR_PROXIMITY_CTL_PIN_NUM, 0);
#else

            HAL_gpio_value_set(SENSOR_PROXIMITY_CTL_PIN_NUM, 1);
#endif
        }

        if (proximity_sta) {
            proximity_out_cnt++;
            if (proximity_out_cnt == 100) { //5ms*100=500ms
                proximity_sta = 0;
                proximity_in_cnt = 0;
                if (pmu_ear_in_case_state_get()) {
                    return;
                }
                event_put(SENSOR_EVENT_P_LT, NULL, 0);
            }
        }
    }
#else
    if (proximity_timer_start && sensor_proximity_online_sta) {
        if (!HAL_gpio_value_get(SENSOR_PROXIMITY_INT_PIN_NUM)) { //检测不到强光
#if SENSOR_PROXIMITY_CTL_PIN_ACTIVE_STATE
            HAL_gpio_value_set(SENSOR_PROXIMITY_CTL_PIN_NUM, 1); //打开激光
#else
            HAL_gpio_value_set(SENSOR_PROXIMITY_CTL_PIN_NUM, 0); //打开激光
#endif
            HAL_udelay(150);
            if (HAL_gpio_value_get(SENSOR_PROXIMITY_INT_PIN_NUM)) { //检测到入耳
                proximity_in_cnt++;
                proximity_out_cnt = 0;
                if (proximity_in_cnt > 2) {
                    if (proximity_sta == 0) {
                        logi("----in");
                        event_put(SENSOR_EVENT_P_HT, NULL, 0);
                    }
                    proximity_sta = 1;
                    proximity_in_cnt = 0;
                }
            } else {
                proximity_out_cnt++;
                proximity_in_cnt = 0;
                if (proximity_out_cnt > 2) {
                    if (proximity_sta == 1) {
                        logi("----out");
                        event_put(SENSOR_EVENT_P_LT, NULL, 0);
                    }
                    proximity_sta = 0;
                    proximity_out_cnt = 0;
                }
            }
        } else {
            //logi("unvalid");
        }
#if SENSOR_PROXIMITY_CTL_PIN_ACTIVE_STATE
        HAL_gpio_value_set(SENSOR_PROXIMITY_CTL_PIN_NUM, 0); //关闭激光

#else
        HAL_gpio_value_set(SENSOR_PROXIMITY_CTL_PIN_NUM, 1); //关闭激光
#endif
    }
#endif

#endif
    if (sensor_proximity_lt_sta) {
        sensor_proximity_lt_cnt++;
        if (sensor_proximity_lt_cnt >= 20) {   //100ms*20=2s,每100ms加1
            sensor_proximity_lt_cnt = 0;
            sensor_proximity_lt_sta = false;//出耳2s 后清0
        }
    } else {
        sensor_proximity_lt_cnt = 0;
    }
}
#endif

/*------------------------------------------------------------------------------
*函数功能:   在出耳暂停音乐时置1，2秒后清0，用于辅助出/入耳控制播放状态
*参数1：
*-------------------------------------------------------------------------------*/
void sensor_proximity_lt_sta_set(bool sta)
{
    sensor_proximity_lt_sta = sta;
    sensor_proximity_lt_cnt = 0;
}


/*------------------------------------------------------------------------------
*函数功能:获取出耳暂停音乐状态
*参数1：
*-------------------------------------------------------------------------------*/
bool sensor_proximity_lt_sta_get(void)
{
    return sensor_proximity_lt_sta;
}

#if SENSOR_PROXIMITY_NO_PLAY_WHEN_LT_TO

void sensor_proximity_lt_timer_create(void)
{
    if (NULL == sensor_proximity_lt_timer_handle) {
        OAL_timer_create(&sensor_proximity_lt_timer_handle, "sensor_lt_timer", OAL_ms_to_ticks(30000),
            sensor_proximity_lt_timeout, (void *)1, 0, OAL_TIMER_NO_ACTIVATE);
    }
}


/*------------------------------------------------------------------------------
*函数功能:   摘下，开始计时
*参数1：
*-------------------------------------------------------------------------------*/

void sensor_proximity_lt_timer_start(void)
{

    if (sensor_proximity_lt_timeout_sta) {
        return;
    }
    if (NULL != sensor_proximity_lt_timer_handle) {
        OAL_timer_activate(&sensor_proximity_lt_timer_handle, OAL_TIMER_AUTO_ACTIVATE);
    }
}


/*------------------------------------------------------------------------------
*函数功能: 远距离，停止计时 退出bt，停止计时
*参数1：
*-------------------------------------------------------------------------------*/

void sensor_proximity_lt_timer_stop(void)
{
    if (sensor_proximity_lt_timeout_sta) {
        return;
    }
    if (NULL != sensor_proximity_lt_timer_handle) {
         OAL_timer_activate(&sensor_proximity_lt_timer_handle, OAL_TIMER_NO_ACTIVATE);
    }
}

/*------------------------------------------------------------------------------
*函数功能: 远距离超时触发
*参数1：
*-------------------------------------------------------------------------------*/
void sensor_proximity_lt_timeout(void* xTimer)
{
    /* 摘下超时 */
    sensor_proximity_lt_timeout_sta = true;
}


/*------------------------------------------------------------------------------
*函数功能: 设置远距离超时状态
*参数1：使能标记 初始为0，持续30秒后置1
*-------------------------------------------------------------------------------*/
void sensor_proximity_lt_timeout_sta_set(bool sta)
{
    sensor_proximity_lt_timeout_sta = sta;
}


/*------------------------------------------------------------------------------
*函数功能: 获取远距离超时状态
*返回值：状态
*-------------------------------------------------------------------------------*/
bool sensor_proximity_lt_timeout_sta_get(void)
{
    /* 如状态为true，lt超时 */
    return sensor_proximity_lt_timeout_sta;
}
#endif


/*------------------------------------------------------------------------------
*函数功能: 设置入耳是否需要播放
*参数1：使能标记
*-------------------------------------------------------------------------------*/
void sensor_proximity_ht_need_play_set(bool sta)
{
    logi("&&sensor_proximity_ht_need_play_set:%d",sta);
    sensor_proximity_ht_need_play =  sta;
}


/*------------------------------------------------------------------------------
*函数功能: 获取入耳是否需要播放
*参数1：使能标记
*-------------------------------------------------------------------------------*/
bool sensor_proximity_ht_need_play_get(void)
{
    logi("!!sensor_proximity_ht_need_play_get:%d",sensor_proximity_ht_need_play);
    return sensor_proximity_ht_need_play;
}

#if (SENSOR_PROXIMITY_EN && (SENSOR_PROXIMITY_DEVICE_SEL == 1))

void sensor_proximity_write(uint8_t adr, uint8_t value)
{
#if (SENSOR_PROXIMITY_DEVICE_SEL == 0)
    /* PA22409001 */
    uint8_t data[2];

    data[0] = adr;
    data[1] = value;
    twi_write(SENSOR_PROXIMITY_TWI_ID, SENSOR_PROXIMITY_TWI_ADDR, data, 2);
#endif
}

AT_SRAM(.sensor)
void sensor_proximity_timer_start(bool start)
{
   proximity_timer_start = start;
}

AT_SRAM(.sensor)
bool sensor_proximity_timer_sta(void)
{
   return proximity_timer_start;
}
#endif


uint8_t sensor_proximity_read(uint8_t adr)
{
    uint8_t value = 0;

#if (SENSOR_PROXIMITY_DEVICE_SEL == 0)
        /* PA22409001 */
    twi_write(SENSOR_PROXIMITY_TWI_ID, SENSOR_PROXIMITY_TWI_ADDR, &adr, 1);
    twi_read(SENSOR_PROXIMITY_TWI_ID, SENSOR_PROXIMITY_TWI_ADDR, &value, 1);
#endif

    return value;
}
#endif

bool sensor_proximity_is_enable(void)
{
    bool enable;

#if SENSOR_PROXIMITY_EN
    enable = true;
#else
    enable = false;
#endif

    return enable;
}

#if (SENSOR_PROXIMITY_EN)
#if SENSOR_PROXIMITY_NO_PLAY_WHEN_LT_TO

/*------------------------------------------------------------------------------
*函数功能: 主副耳停止远距离计时timer
*参数1:
*-------------------------------------------------------------------------------*/
void sensor_proximity_lt_timer_stop_and_sync(void)
{
    uint8_t *param;
    uint32_t event;

    sensor_proximity_lt_timer_stop();

    if (tws_role_get() == TWS_MASTER) {
        event = SENSOR_EVENT_P_LT_TIMER_STOP;
        param = (uint8_t *)&event;
        tws_manager_app_cmd_send(TWS_APP_CMD_EVENT, param, 4);
    }
}


/*------------------------------------------------------------------------------
*函数功能: 主副耳启动远距离计时timer
*参数1:
*-------------------------------------------------------------------------------*/
void sensor_proximity_lt_timer_start_and_sync(void)
{
    uint8_t *param;
    uint32_t event;

    sensor_proximity_lt_timer_start();

    if (tws_role_get() == TWS_MASTER) {
        event = SENSOR_EVENT_P_LT_TIMER_START;
        param = (uint8_t *)&event;
        tws_manager_app_cmd_send(TWS_APP_CMD_EVENT, param, 4);
    }
}


/*------------------------------------------------------------------------------
*函数功能: 主副耳远距离标记设置
*参数1:
*-------------------------------------------------------------------------------*/
void sensor_proximity_lt_timeout_sta_set_and_sync(bool sta)
{
    uint8_t *param;
    uint32_t event;

    sensor_proximity_lt_timeout_sta_set(sta);

    if (tws_role_get() == TWS_MASTER) {
        event = SENSOR_EVENT_P_LT_TIMER_STA_CLEAR;
        param = (uint8_t *)&event;
        tws_manager_app_cmd_send(TWS_APP_CMD_EVENT, param, 4);
    }
}
#endif


/*------------------------------------------------------------------------------
*函数功能: 主副耳入耳播放标记设置
*参数1:
*-------------------------------------------------------------------------------*/
void sensor_proximity_ht_need_play_set_and_sync(bool sta)
{
    uint8_t *param;
    uint32_t event;

    sensor_proximity_ht_need_play_set(sta);

    if (tws_role_get() == TWS_MASTER) {
        if (sta) {
            event = SENSOR_EVENT_P_HT_NEED_PLAY_SET;
        } else {
            event = SENSOR_EVENT_P_HT_NEED_PLAY_CLEAR;
        }
        param = (uint8_t *)&event;
        tws_manager_app_cmd_send(TWS_APP_CMD_EVENT, param, 4);
    }
}
#endif


/*------------------------------------------------------------------------------
*函数功能: 获取主副耳入耳情况
*返回值:    SENSOR_P_SINGLE_OUT = 0x0,
         SENSOR_P_SINGLE_IN, 一边入耳

         SENSOR_P_DOUBLE_2OUT, 都不入耳
         SENSOR_P_DOUBLE_1IN_1OUT,一边入一边出
         SENSOR_P_DOUBLE_2IN,两个都入
*-------------------------------------------------------------------------------*/
uint8_t sensor_proximity_peer_sta_get(void)
{
    __maybe_unused uint8_t sta = 0;

#if BT_TWS_MP_MODE_EN
        if (!nor_tws_paired_get()) {
            return SENSOR_P_DOUBLE_2IN;
        }
#endif

#if SENSOR_PROXIMITY_EN
    if (tws_peer_con_status_get()) {//获取主副连接状态
        if (sensor_proximity_ps_sta_get() && !pmu_ear_in_case_state_get()) {//当前耳机入耳
#if (BT_AIR_SEL || BLE_AIR_SEL)
            if (sensor_proximity_ps_sta_ews_get()  && ((air_bat_partner_get()&0x80)==0)) {
                sta = SENSOR_P_DOUBLE_2IN;
            } else {
                sta = SENSOR_P_DOUBLE_1IN_1OUT;
            }
#else
            if (sensor_proximity_ps_sta_ews_get()) {//另一只是否入耳
                sta = SENSOR_P_DOUBLE_2IN;
            } else {
                sta = SENSOR_P_DOUBLE_1IN_1OUT;
            }

#endif
        } else {//当前耳机没有入耳
#if (BT_AIR_SEL || BLE_AIR_SEL)
            if (sensor_proximity_ps_sta_ews_get() && ((air_bat_partner_get()&0x80)==0)) {
                sta = SENSOR_P_DOUBLE_1IN_1OUT;
            } else {
                sta = SENSOR_P_DOUBLE_2OUT;
            }
#else
            if (sensor_proximity_ps_sta_ews_get()) {
                sta = SENSOR_P_DOUBLE_1IN_1OUT;
            } else {
                sta = SENSOR_P_DOUBLE_2OUT;
            }
#endif
        }
    } else {
        if (sensor_proximity_ps_sta_get() && !pmu_ear_in_case_state_get()) {
            sta = SENSOR_P_SINGLE_IN;
        } else {
            sta = SENSOR_P_SINGLE_OUT;
        }
    }
    logi("peer_sta_get, ps_sta:%d ,ps_sta_ews:%d, stat:%d",sensor_proximity_ps_sta, sensor_proximity_ps_sta_ews,sta);
    return sta;

#else
    return SENSOR_P_DOUBLE_2IN;
#endif
}


