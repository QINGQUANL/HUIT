#define LOG_LEV 4
#include "sys_inc.h"
#include "key.h"


volatile uint8_t key_onoff_interval;     //两次间隔时间
volatile uint8_t key_onoff_click_cnt;    //多击次数
volatile bool key_onoff_state;           //true-按下; false-松开
volatile uint32_t key_onoff_long_time;   //长按时间
volatile bool key_long_press;

static key_func_ctl *k_func_ctl;


void key_onoff_init(void)
{
    k_func_ctl = (key_func_ctl *)key_get_ctl_func();
}

void key_onoff_state_set(uint32_t state)
{
    key_onoff_state = state;
    uint32_t ktime;
    ktime = key_onoff_long_time*KEY_SCAN_CYCLE;
    if (!state) {
            if (k_func_ctl->key_func_lu_en || k_func_ctl->key_func_m_en) {
            uint8_t n;
            uint32_t kret;
            if (ktime > KEY_CNT_1000MS) {
                n = ktime / KEY_CNT_1000MS;
                if (n>0 && n < 10) {
                    kret = KEY_EVENT_LU | (n<<8) | KEY_M;
                    event_put(kret, NULL, 0);
                    //logi("onoff lu 0x%x",kret);
                    key_long_press = false;
                }
            }
        }
   }
}

uint32_t key_onoff_state_get(void)
{
    return key_onoff_state;
}

AT_SRAM(.key)
void key_onoff_process(uint32_t event)
{
    /* ultra short */
    if (event == PMU_EVENT_POWERKEY_US) {
        key_onoff_state = true;
        key_onoff_long_time = 0;
        key_long_press = false;
       if(k_func_ctl->key_func_s_en)
            event_put(KEY_EVENT_S | KEY_M, NULL, 0);
            //logi("onoff s");

    }

    /* short up */
    if (event == PMU_EVENT_POWERKEY_SU) {
        key_onoff_state = false;
        if (key_long_press) {
            logi("long_press break su");
            return;
        }

       if (k_func_ctl->key_func_lu_en || k_func_ctl->key_func_m_en) {
            if (key_onoff_long_time >= KEY_CNT_2000MS && key_onoff_long_time < KEY_CNT_4000MS) {
                /* 长按超过2S松开 */
                event_put(KEY_EVENT_LU | KEY_M | (0x2<<8), NULL, 0);
            } else {
                key_onoff_interval = KEY_M_CLICK_INTERVAL;
                key_onoff_click_cnt++;
            }
      } else {
            event_put(KEY_EVENT_SU | KEY_M, NULL, 0);
      }

    }

    /* long */
    if (event == PMU_EVENT_POWERKEY_L) {

       if( k_func_ctl->key_func_l_en) {
        //logi("onoff long");
        /* 硬件产生，由PMU_LONG_PRESS_TIME配置 */
        event_put(KEY_EVENT_L | KEY_M, NULL, 0);
        key_long_press = true;
      }

    }

}

AT_SRAM(.onoff_key)
void key_onoff_scan(void)
{

    if ( !k_func_ctl ||  (!k_func_ctl->key_func_m_en && !k_func_ctl->key_func_lu_en
        && !k_func_ctl->key_func_ll_en))  {
        return;
    }
    uint32_t kret;
    uint32_t ktime;

    kret = KEY_EVENT_NONE;


    if (k_func_ctl->key_func_m_en) {
        /* 单击/多击 */
        if (key_onoff_interval > 0) {
            key_onoff_interval--;
        }
        if (key_onoff_interval == 1) {
            if (key_onoff_click_cnt == 1) {
                //单击
                kret = KEY_EVENT_SU | KEY_M;
            } else {
                //多击
                kret = KEY_EVENT_M | KEY_M | (key_onoff_click_cnt<<8);
            }
            key_onoff_click_cnt = 0;
            event_put(kret, NULL, 0);
            //logi("konoff m:%x ", kret);
        }
    }


    if (k_func_ctl->key_func_l_en ||k_func_ctl->key_func_lu_en || k_func_ctl->key_func_ll_en || k_func_ctl->key_func_h_en) {
    /* 长按 */
        if (key_onoff_state) {
            key_onoff_long_time++;
            ktime = key_onoff_long_time * KEY_SCAN_CYCLE;
            
            if (k_func_ctl->key_func_ll_en) {
                uint8_t n;
                  if (ktime % KEY_CNT_1000MS == 0) {
                    n = ktime / KEY_CNT_1000MS;
                    if (n>0 && n < 10) {
                        kret = KEY_EVENT_L | (n<<8) | KEY_M;
                        event_put(kret, NULL, 0);
                        //logi("konoff ll:%x", kret);
                        if(n>1) {
                            key_long_press = true;
                        }
                    }
                 }
            }
            
           if (k_func_ctl->key_func_l_en) {
                if (ktime == k_func_ctl->key_long_time) {
                    kret = KEY_EVENT_L | KEY_M;
                    event_put(kret, NULL, 0);
                    //logi("konoff l:%x", kret);
                }
            }

           if (k_func_ctl->key_func_h_en) {
                if (ktime > k_func_ctl->key_long_time) {
                    if ((ktime - k_func_ctl->key_long_time) % k_func_ctl->key_hold_time == 0) {
                        kret = KEY_EVENT_H | KEY_M;
                        event_put(kret, NULL, 0);
                        //logi("konoff h:%x", kret);
                    }
                }
           }

        } else {
            key_onoff_long_time = 0;
        }
   }

}


