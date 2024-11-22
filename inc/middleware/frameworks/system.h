#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#ifdef __cplusplus
extern "C"
{
#endif



#if (SYS_PLATFORM == PLATFORM_FPGA)
#define SW_DIO_PIN_SEL          3
#define SW_CLK_PIN_SEL          3

#else
#define SW_DIO_PIN_SEL          2
#define SW_CLK_PIN_SEL          2
#endif


#if (SW_DIO_PIN_SEL == 0)
#define SW_DIO_PIN_CFG          (PIN_B0 | PIN_FUNC4)
#elif (SW_DIO_PIN_SEL == 1)
#define SW_DIO_PIN_CFG          (PIN_B3 | PIN_FUNC3)
#elif (SW_DIO_PIN_SEL == 2)
#define SW_DIO_PIN_CFG          (PIN_D0 | PIN_FUNC3)
#elif (SW_DIO_PIN_SEL == 3)
#define SW_DIO_PIN_CFG          (PIN_A3 | PIN_FUNC2)
#endif

#if (SW_CLK_PIN_SEL == 0)
#define SW_CLK_PIN_CFG          (PIN_B2 | PIN_FUNC4)
#elif (SW_CLK_PIN_SEL == 1)
#define SW_CLK_PIN_CFG          (PIN_B4 | PIN_FUNC3)
#elif (SW_CLK_PIN_SEL == 2)
#define SW_CLK_PIN_CFG          (PIN_D1 | PIN_FUNC3)
#elif (SW_CLK_PIN_SEL == 3)
#define SW_CLK_PIN_CFG          (PIN_A2 | PIN_FUNC2)
#endif


// void swd_init(void);
// void swd_suspend(bool suspend);


void exception_init(void);
void hexdump(char* name, char* base, int len);
void wait_idle_running_a_period(uint32_t wait_ms);
uint32_t sys_get_ver(void);
void write_protect_addr_set(uint8_t index, uint32_t start_addr, uint32_t end_addr);
void write_protect_enable(uint8_t index, bool enable);

void framework_init(uint32_t flash_size_KB);

typedef struct{
    uint8_t flash_size_mode; // 0: 128KB, 1: 256KB, 2: 512KB, 3: 1MB, 4: 2MB, 5: 4MB
    uint8_t flash_clk_sel; // NORFLASH_CLK_48M
    uint8_t flash_line_mode; // LINE_MODE_FOUR
    uint8_t *os_heap;
    uint32_t os_heap_size;
    uint32_t cpu0_freq;
    uint32_t cpu1_freq;
    uint8_t time_before_wfi;
    uint8_t pmu_core_use_power;
    uint32_t dcdc_volt;
    uint32_t cpu_src;
    uint32_t pll_rate;
    uint32_t bus_rate;
}SPARK_SDK_PARA;

void spark_sdk_init(SPARK_SDK_PARA *para);

#ifdef __cplusplus
}
#endif

#endif

