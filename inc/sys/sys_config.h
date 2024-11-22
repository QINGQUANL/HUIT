#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_


/***************************************************************************
                system config
 ***************************************************************************/
#define SYS_CHIP_ID                 CHIP_SL6801
#define SYS_PLATFORM                PLATFORM_CHIP
#define SYS_STACK_SIZE              512

/***************************************************************************
                dual cpu
***************************************************************************/
#define DUAL_CPU_EN                 1

/***************************************************************************
                pmu config
***************************************************************************/
#define PMU_EXIST                   1
#define PMU_VERSION                 1

/***************************************************************************
                 cache config
 ***************************************************************************/
#define CACHE_EN                    1
#define CACHE_MONITOR_EN            0
#define BANK_EN                     0

/***************************************************************************
                Interrupt Priority
***************************************************************************/
/* priority under 5'isr must not use the FreeRTOS interfaces
 * the value bigger, the priority lower
 */
#define IRQn_BROM_WR_PRIO           0
#define IRQn_AUSSTX_DMA_PRIO        2
#define IRQn_AUSSRX_DMA_PRIO        2
#define IRQn_I2STX_DMA_PRIO         2
#define IRQn_I2SRX_DMA_PRIO         2
#define IRQn_ASRC_DMA_PRIO          6

#define IRQn_RTC_PRCM_PRIO          6
#define IRQn_USB_PRIO               6
#define IRQn_BT_PRIO                7
#define IRQn_BLE_PRIO               7
#define IRQn_SDMMC_PRIO             8
#define IRQn_GPIOB_PRIO             8
#define IRQn_GPIOC_PRIO             7
#define IRQn_GPIOD_PRIO             8

#define IRQn_NORF_PRIO              9
#define IRQn_MSGC0_PRIO             15

/***************************************************************************
                Task Priority
***************************************************************************/
/* the value bigger, the priority higher
 *
 */
#define UPDATE_TASK_HIGHEST_PRIO    19              //更新任务必须使用最高优先级
#define BT_ISR_BH_TASK_PRIO         19
#define BLE_ISR_BH_TASK_PRIO        18
#define PMU_ISR_BH_TASK_PRIO        3
#define BT_CTRL_TASK_PRIO           9
#define BT_SNIFFER_TASK_PRIO        10
#define BT_AUDIO_TASK_PRIO          6
#define BT_HOST_TASK_PRIO           7
#define BLE_HOST_TASK_PRIO          7
#define DEC_TASK_PRIO               1
#define LINEIN_TASK_PRIO            9
#define MAIN_TASK_PRIO              1
#define RECORDE_TASK_PRIO           1
#define TRACK_TASK_PRIO             9
#define SPDIF_TASK_PRIO             9
#define TIMER_TASK_PRIO             1
#define G24_LMP_TASK_PRIO           6
#define G24_MAIN_TASK_PRIO          5
#define G24_IDUPDATA_TASK_PRIO      6
#define KEY_AUDIO_TASK_PRIO         7

/***************************************************************************
                Task stack size in byte
***************************************************************************/
#define BT_ISR_BH_TASK_STACK_SIZE   (256)
#define BLE_ISR_BH_TASK_STACK_SIZE  (128)
#define PMU_ISR_BH_TASK_STACK_SIZE  (128)
#define BT_CTRL_TASK_STACK_SIZE     (256+64)
#define BT_SNIFFER_TASK_STACK_SIZE  (256)
#define BT_AUDIO_TASK_STACK_SIZE    (512)
#define BT_HOST_TASK_STACK_SIZE     (256+64)
#define BLE_HOST_TASK_STACK_SIZE    (256+64)
#define DEC_TASK_STACK_SIZE         (512)
#define LINEIN_TASK_STACK_SIZE      (200)
#define MAIN_TASK_STACK_SIZE        (512)
#define RECORDE_TASK_STACK_SIZE     (256)
#define TRACK_TASK_STACK_SIZE       (256)
#define SPDIF_TASK_STACK_SIZE       (256)
#define G24_IDUPDATA_TASK_SIZE      (256)
#define KEY_AUDIO_TASK_SIZE         (512)

/***************************************************************************
 *               bt config
 ***************************************************************************/
#define BT_ISR_BH_IN_TASK           1               //BT ISR使用任务作为下半部
#define BLE_ISR_BH_IN_TASK          1               //BLE ISR使用任务作为下半部
#define BT_SNIFFER_IN_TASK          1

/***************************************************************************
 *               power/sleep config
 ***************************************************************************/
#define BT_SNIFF_EN                 1

#define BT_CON_AUTO_SLEEP_EN        0               //蓝牙连接时自动睡眠，防止某些手机不会自动发送SNIFF请求
#define BT_CON_AUTO_SLEEP_WAIT      40000           //单位:毫秒, iphone < 30秒

#define BT_DIS_AUTO_SLEEP_EN        0               //蓝牙断开时自动睡眠，在没有使能蓝牙断开时自动关机功能时，可以节省系统功耗
#define BT_DIS_AUTO_SLEEP_MAX       800             //unit: slot

/***************************************************************************
 *               audio sys config
 ***************************************************************************/
#define AUDIO_CALC_CPU_LOAD_EN      0
#define AUDIO_KARAOKE_PROFILE_EN    0
#define AUDIO_LOCAL_MUSIC_EN        1

/***************************************************************************
 *               debug config
 ***************************************************************************/
#define DEBUG_WR_IRQ_EN             1               //使能写保护功能，保护代码段，防止系统堆栈溢出
#define DEBUG_FAULT_DIAGNOSIS       1               //使能异常信息解析功能，更加方便阅读，需要占用更多代码空间
#define DEBUG_EXCEPTION_INIT        1               //使能异常报警功能
#define DEBUG_LOG_EN                1               //UART打印总开关
#define DEBUG_LOG_SPP_EN            0
#define DEBUG_DUMP_TASK_EN          0
#define DEBUG_SCO_DUMP_EN           0
#define DEBUG_DEEP_SLEEP_EN         0               //使能休眠led指示灯
#define DEBUG_RUNTIME_PRINT         0
#define DEBUG_EVB_NOBAT             0
#define DEBUG_ADV_EN                0
#define DEBUG_CHECK_OS_MEMORY       0
#define DEBUG_AUTOTEST_LOG_EN       1
#define DEBUG_LOCKUP                0
#define DEBUG_DEDICATE_LOG_EN       0

/***************************************************************************
 *               其它
 ***************************************************************************/
#define STARTUP_TUNING              1  //开机速度优化

/**************************************************************************
*               频偏校准使用的内存方式
***************************************************************************/
#define TRIM_MEMORY_USE_RAM          0
#endif
