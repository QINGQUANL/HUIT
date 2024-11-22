#define LOG_LEV 4
#include "sys_inc.h" // libsys.a
#include "oal.h" // liboal.a
#include "hal.h" //libhal.a
#include "sltk_mem.h"

#include "system.h" //libmuddleware.a
#include "info_mem.h"
#include "mode_manager.h"
#include "console.h"
#include "mode.h"
#include "app_timer.h"
#include "app_rtc.h"
#include "app_key.h"
#include "app_pwm.h"
#include "app_spi.h"
#include "user_config.h"
#include "app_gpio_irq.h"
#include "console.h"
#include "disp.h"
#include "app_pmu.h"
#include "app_exception.h"
#include "psm_manager.h"
#include "iodm_test.h"
#include "usbd_app.h"
#include "g24_api.h"
#include "app_lcdc.h"
#include "app_psm.h"
#include "usb_app_config.h"
#include "app_psm.h"
#include "authority.h"
#include "app_device.h"
#include "sys_exception.h"
#include "ble.h"
#include "lcd_display.h"

void pc_update_process();
void usbd_gpio_sofware_detect_init();

OAL_THREAD main_task_handler;

AT(.os_heap)
uint8_t SysHeap[ SYS_HEAP_SIZE ] __attribute__((aligned(8)));
extern uint32_t TickRateHz;

AT(.startup_seg)
void system_init(void)
{
    clk_init_param_t clk_param;
    norflash_param_t norflash_param;

    //sys层初始化
    SYS_init();
    //硬件抽象层初始化

    clk_param.use_dcdc = PMU_CORE_USE_DCDC;
    clk_param.cpu_src = CORE_PLL_CLK;
#if OVER_FREQ_EN
    clk_param.cpu_src = CPU_PLL_CLK;
#endif
    clk_param.bus_rate = 32000000;

    if (clk_param.cpu_src == CPU_PLL_CLK) {
#if PMU_CORELDO_EN
        clk_param.dcdc_volt = 1240;                   //按ShaoZhen建议修改为1240更可靠
#else
        clk_param.dcdc_volt = 1200;
#endif
        clk_param.pll_rate = 192000000;
    } else {
        clk_param.dcdc_volt = 1200;
        clk_param.pll_rate = 384000000;
    }

    norflash_param.clk_sel = NORFLASH_CLK_32M;
    norflash_param.line_mode = LINE_MODE_FOUR;        //当使用单线模式LINE_MODE_SINGLE时, clk不能选择超过32M
    norflash_param.size_mode = 3; // 1MB
    HAL_init(&clk_param, &norflash_param);
    
#if (G24_2T4_SHORT_PACKET_3TIMES || G24_2T4_WITH_MONO_MONITOR) && G24_FUNC_ROLE
    HAL_ahb_src_set(CLK_24M_HOSC);                   //时钟来源于：晶振--AHB--BT，避免pll倍频后芯片差异
    G24_2T4_POWER_ON();                              //给主副芯片保持上电状态
#if !G24_2T4_DONGLE_MCHIP
    exception_fault_restart_set(false);              //出现fault死机时，副芯片不复位，由主芯片统一控制
#endif
#endif

    //uart初始化
    uart_init();

#if USB_PC_NO_PD0_UPDATE_EN
        if(is_usb_update_flag_set()){
            clear_usb_update_flag();
            pc_update_process();
        }
#endif

    
    //swd初始化
#if SWD_DEBUG_EN
    swd_init();
#endif
    TickRateHz = 10000;
    // 内核抽象层初始化
    OAL_init(SysHeap, SYS_HEAP_SIZE, TIME_BEFORE_WFI);
    //初始化pmu
    pmu_early_init();
    //framework层初始化
    framework_init(1024);

#if 0//BT_EWS_IODM_TEST_MODE_EN
    iodm_test_init();
#endif
}

void* task_main_loop(void *params)
{
    //console初始化
    sys_info_init();
    pmu_late_init();
#if 0//DEBUG_LOG_EN
    console_init();
#endif
    link_info_init();
    app_rtc_init();
    app_framework_init();

    app_key_init();
    pmu_isr_bh_init();
    app_gpio_irq_en();
    pmu_task_init();
    app_spi_init();
    app_timer_init();
    app_exception_init();
    app_disp_init();
    lcd_controller_init();
    lcd_screen_init(0);
    lcd_display_init();
    uart2_ble_init();
#if BT_DUT_EN && BT_IODM_TEST_MODE_EN
    if(!nor_mp_dut_flag_get()) {
        iodm_test_init();
    } else {
        logi("didn't enter iodm");
        HAL_gpio_func_set(UART2_RX_PIN_NUM, PIN_FUNCDISABLE);
    }
#endif

    mode_init();

#if USB_DEVICE_EN || USB_HOST_EN
    if(!nor_mp_dut_flag_get()) {
        logi("mode get ---- %d",mode_sta_get());
        if((mode_sta_get() == MODE_G24) || (mode_sta_get() == MODE_USBDEV)) {
            usb_initialize();
        } else if(mode_sta_get() == MODE_BT){
#if USB_DETECT_USE_SOFWARE        
           usbd_gpio_sofware_detect_init();
#endif
        }
    }
#endif

    app_device_init();

    mode_manager(params);
    return NULL;
}


AT(.startup_seg)
int main(void)
{
    __maybe_unused uint32_t cpu0_cur_freq;
    __maybe_unused uint32_t cpu1_cur_freq;
    __maybe_unused uint32_t cpu_src;
    float freq;

    // 系统库初始化
    system_init();
    // 打印一句欢迎的调试信息。
    logi("karaoke Hello! welcome to spark sdk.\r\n");

    cpu_src = HAL_clk_get_src(CLK_CPU_SYS);
	freq = HAL_clk_get_rate(cpu_src) / (HAL_clk_get_div(CLK_CPU_SYS) + HAL_clk_get_frac_div(CLK_CPU_SYS) / 64.f) / (HAL_clk_get_div(CLK_CPU0) + HAL_clk_get_frac_div(CLK_CPU0) / 32.f);
	cpu0_cur_freq = (uint32_t)freq;
	freq = HAL_clk_get_rate(cpu_src) / (HAL_clk_get_div(CLK_CPU_SYS) + HAL_clk_get_frac_div(CLK_CPU_SYS) / 64.f) / (HAL_clk_get_div(CLK_CPU1) + HAL_clk_get_frac_div(CLK_CPU1) / 32.f);
	cpu1_cur_freq = (uint32_t)freq;

	logi("core pll:%u, cpu pll:%u, cpu0:%u, cpu1:%u, ahb:%u, norf:%u",
	    HAL_clk_get_rate(CORE_PLL_CLK),
	    HAL_clk_get_rate(CPU_PLL_CLK),
	    cpu0_cur_freq,
	    cpu1_cur_freq,
	    HAL_clk_get_rate(CLK_AHB),
	    HAL_clk_get_rate(CLK_NORF));

    main_task_handler = OAL_thread_create(task_main_loop, 0, "task main", MAIN_TASK_PRIO, MAIN_TASK_STACK_SIZE);

    OAL_start_scheduler();

	return 0;
}
