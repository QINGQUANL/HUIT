#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "sleep.h"
#include "hal.h"
#include "oal.h"
#include "app_pmu.h"
#include "audio_service.h"
#include "tws_profile.h"

#if CPU_LOAD_EN
static uint64_t idle_begin, idle_end, idle_total;
uint32_t cpu0_load_per = 0;

static uint64_t cpu1_time_mark = 0, cpu1_time_total = 0;
uint32_t cpu1_load_per = 0;
uint8_t cpu1_idle_sta = 0;
uint8_t cpu1_idle_cnt_need_clear = 0;

AT_SRAM(.sys)
void cpu1_load_calc_start()
{
    cpu1_time_mark = HAL_get_timestamp();
    cpu1_idle_sta = 1;
}

AT_SRAM(.sys)
void cpu1_load_calc_end()
{
    cpu1_time_total += (HAL_get_timestamp() - cpu1_time_mark);
    cpu1_idle_sta = 0;
    cpu1_idle_cnt_need_clear = 0;
}

AT_SRAM(.sys)
uint32_t sys_get_cpu0_load_per()
{
    return cpu0_load_per;
}

AT_SRAM(.sys)
uint32_t sys_get_cpu1_load_per()
{
    return cpu1_load_per;
}

AT_SRAM(.sys)
void cpu_load_timer_cb(void* timer)
{      
	cpu0_load_per = 100 - (uint32_t)(idle_total/10/CPU_LOAD_PERIOD);

	if(cpu1_idle_cnt_need_clear) {
	   cpu1_idle_cnt_need_clear = 0;
	   cpu1_load_per = 0;
	} else {
	   cpu1_load_per = 100 - (uint32_t)(cpu1_time_total/10/CPU_LOAD_PERIOD);
	}

	if(cpu1_idle_sta) {
		cpu1_idle_cnt_need_clear = 1;
	}

	#if CPU_LOAD_EN
	logi("----->>>>>cpu load:%u-%u", cpu0_load_per, cpu1_load_per);
	#endif
	idle_total = 0;
	cpu1_time_total = 0;
}

uint32_t get_cpu0_loadper()
{
    return cpu0_load_per;
}

uint32_t get_cpu1_loadper()
{
    return cpu1_load_per;
}

#else
AT_SRAM(.sys)
void cpu1_load_calc_start()
{
}
AT_SRAM(.sys)
void cpu1_load_calc_end()
{
}

uint32_t get_cpu0_loadper()
{
    return 0;
}

uint32_t get_cpu1_loadper()
{
    return 0;
}


#endif

#define portNVIC_SYSTICK_CTRL_REG			( * ( ( volatile uint32_t * ) 0xe000e010 ) )
#define portNVIC_SYSTICK_LOAD_REG			( * ( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG	( * ( ( volatile uint32_t * ) 0xe000e018 ) )
#define portNVIC_SYSPRI2_REG				( * ( ( volatile uint32_t * ) 0xe000ed20 ) )


AT_SRAM(.sys)
void app_exception_cb(uint32_t cmd)
{
    switch (cmd) {
        case OS_USER_APPLICATION_IDLE:
#if SLEEP_EN
            sleep();
#endif
#if (TIME_BEFORE_WFI != 1)
            __asm volatile( "cpsid i" );
#if CPU_LOAD_EN
            idle_begin = HAL_get_timestamp();
#endif
		    __asm volatile( "dsb" );
            __asm volatile( "wfi" );
            __asm volatile( "isb" );
#if CPU_LOAD_EN
            idle_end = HAL_get_timestamp();
            idle_total +=  (idle_end - idle_begin);
#endif
            __asm volatile( "cpsie i" );
#endif
            break;

        case OS_USER_PRE_SLEEP:
            while(portNVIC_SYSTICK_LOAD_REG && (portNVIC_SYSTICK_CURRENT_VALUE_REG == 0));
#if CPU_LOAD_EN
            idle_begin = HAL_get_timestamp();
#endif
            break;

        case  OS_USER_POST_SLEEP:
#if CPU_LOAD_EN
            idle_end = HAL_get_timestamp();
            idle_total +=  (idle_end - idle_begin);
#endif
            break;

        case OS_USER_STACK_OVERFLOW:
            loge("err OS_USER_STACK_OVERFLOW");
            break;
        
        case OS_USER_MALLOC_FAILED:
            loge("err OS_USER_MALLOC_FAILED");
            break;
            
        default:
            break;
    }
}

#if SLEEP_EN
AT_SRAM(.sleep1)
void sleep_timer_cb(void *xTimer)
{
    pm_prevent_sleep_delay_clear();
}
#endif

void tws_status_timer_cb(void *xTimer)
{
    update_cpu_low_freq_delay_time();

    logd("INFO:a2dp(%X,%X,%X, %X), sco(%X,%X,%X,%X,%X), dac(%X,%d)",
        debug_avdtp_receive_data_cnt & 0xFF,
        debug_avdtp_receive_data_sniffer_cnt & 0xFF,
        l2_recv & 0xFF,
        debug_a2dp_dec_cnt & 0xFF,

        debug_ld_sco_manage_ctrl_entity_normal_cnt & 0xFF,
        debug_ld_sco_manage_ctrl_entity_loss_cnt & 0xFF,
        debug_s_sniff_data_rx_report_normal_cnt & 0xFF,
        debug_s_sniff_data_rx_report_loss_cnt & 0xFF,
        debug_sco_process_cnt & 0xFF,

        debug_dac_data,
        audio_service_get_mute_mode()
        );

#if AUDIO_MUTE_EN
    if (pmu_vbus_is_in()) {
        //在充电盒
        if (!audio_service_get_mute_mode()) {
            audio_service_set_mute_mode(true);
        }
    } else {
        //不在充电盒
        if (audio_service_get_mute_mode()) {
            audio_service_set_mute_mode(false);
        }
    }
#endif
}

OAL_TIMER SleepTimerHandle;
OAL_TIMER CpuLoadTimerHandle;
OAL_TIMER ews_status_timer_handle;

void timer_create(void)
{
#if 0
    if (ews_status_timer_handle == NULL) {
        OAL_timer_create(&ews_status_timer_handle, "ews_sta", OAL_ms_to_ticks(2000),
            tws_status_timer_cb, (void *)1, 1, OAL_TIMER_AUTO_ACTIVATE);
    }
#endif

#if SLEEP_EN
    sleep_delay_set();
    OAL_timer_create(&SleepTimerHandle, "SleepTimer", OAL_ms_to_ticks(1000),
        sleep_timer_cb, (void *)1, 1, OAL_TIMER_AUTO_ACTIVATE);

#endif

#if CPU_LOAD_EN
    if (CpuLoadTimerHandle == 0) {
       // CpuLoadTimerHandle = xTimerCreate("CpuLoadTimer", OAL_ms_to_ticks(CPU_LOAD_PERIOD), 1, (void *)1, cpu_load_timer_cb);
       OAL_timer_create(&CpuLoadTimerHandle, "CpuLoadTimer", OAL_ms_to_ticks(CPU_LOAD_PERIOD),
        cpu_load_timer_cb, (void *)1, 1, OAL_TIMER_AUTO_ACTIVATE);

    }
    //xTimerStart(CpuLoadTimerHandle, 10);
#endif

}


void app_exception_init()
{
    sys_execption_callback_register(app_exception_cb);
    timer_create();

}
