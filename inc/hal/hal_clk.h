#ifndef _HAL_CLK_LIB_H_
#define _HAL_CLK_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys_types.h"

#define SRC_EXT_LOSC   32768
#define SRC_32K_HOSC   32000
#define SRC_12M_HOSC   12000000
#define SRC_24M_HOSC   24000000
#define SRC_50K_RCOSC  50000

typedef struct {
    uint8_t use_dcdc;
    uint32_t dcdc_volt;
    uint32_t cpu_src;
    uint32_t pll_rate;
    uint32_t bus_rate;
} clk_init_param_t;

typedef struct vf_table {
    uint32_t freq;
    uint32_t volt;
} vf_table_t;

enum clk_sram_speed_e {
    SRAM_NORMAL,
    SRAM_HIGH,
};

enum ccu_id_e {
    CCU_CPU1 = 0,   //0
    CCU_NORF,       //1
    CCU_ANCR,       //2
    CCU_VAD,        //3

    CCU_MBIST = 32, //32
    CCU_DMAC,       //33
    CCU_BTSS,       //34
    CCU_USB,        //35
    CCU_SDMMC,      //36
    CCU_AUSS,       //37
    CCU_SIC,        //38
    CCU_ASRC,       //39
    CCU_MBOX,       //40
    CCU_EQ,         //41
    CCU_DRC,        //42
                    //
    CCU_DKC = 64,   //64
    CCU_IR,         //65
    CCU_LEDC,       //66
                    //67
    CCU_PWM = 68,   //68
    CCU_TIMER,      //69
    CCU_GPIO,       //70
                    //71
    CCU_UART0 = 72, //72
    CCU_UART1,      //73
    CCU_UART2,      //74
                    //75
    CCU_TRNG = 76,  //76
    CCU_SPI,        //77
                    //78
                    //79
    CCU_TWI0 = 80,  //80
    CCU_TWI1,       //81
    CCU_STWI,       //82
                    //83
    CCU_KADC = 84,  //84
    CCU_I2S,        //85
                    //86
    CCU_SPDIF = 87, //87
    CCU_EID,        //88
    CCU_TKC,        //89
    CCU_SPINLOCK,   //90
    CCU_HCEC,       //91
    CCU_LCDC,       //92
};


/*
  *  CLK_50K_RCOSC-->| 0
  *                                  |LOSC_SEL-->CLK_LOSC-->|0
  *  CLK_EXT_LOSC---->| 1                                       |
  *                                                                              |-->CLK32K_SRC_SEL-->CLK_32K
  *                                                                              |
  *  CLK_32K_HOSC----------------------------->|1
  *
*/
enum clk_id_e {
    //rom audio use the number 0, so we can not change
    //we just can follow
    AUDIO_PLL_CLK,    /* 0 */
    AUDIO_PLLX8_CLK,  /* 1 */
    CORE_PLL_CLK,     /* 2 */
    CLK_CPU_SYS,      /* 3 */
    CLK_CPU0,         /* 4 */
    CLK_CPU1,         /* 5 */
    CLK_AHB,          /* 6 */
    CLK_APB,          /* 7 */
    CLK_96M,          /* 8 */
    CLK_48M,          /* 9 */
    CLK_24M_HOSC,     /* 10 CLK_30M_RCOSC or CLK_HOSC IC:CLK_HOSC */
    CLK_12M_HOSC,     /* 11 HOSC / 2 */
    CLK_32K_HOSC,     /* 12 HOSC / 750 */
    CLK_EXT_LOSC,     /* 13 external LOSC 32768Hz */
    CLK_50K_RCOSC,    /* 14 50K +/- 20K 等同于LOSC */
    CLK_32K,          /* 15 from CLK_32K_HOSC or CLK_LOSC IC:CLK_LF */
    CLK_LOSC,         /* 16 from CLK_50K_RC or CLK_EXT_LOSC 等同于RC50K */
    CLK_SDMMC,        /* 17 */
    CLK_USB,          /* 18 */
    CLK_AUSS,         /* 19 */
    CLK_I2S,          /* 20 */
    CLK_UART0,        /* 21 */
    CLK_UART1,        /* 22 */
    CLK_TWI1,         /* 23 */
    CLK_TWI2,         /* 24 */
    CLK_KADC,         /* 25 */
    CLK_TRNG,         /* 26 */
    CLK_EID,          /* 27 */
    CLK_FM,           /* 28 */
    CLK_BTSS,         /* 29 */
    CLK_SIC,          /* 30 */
    CLK_SPDIF,        /* 31 */
    CLK_TKC,          /* 32 */
    CLK_GPIO,         /* 33 */
    CLK_IR,           /* 34 FIXME:sl2801 not support, sl6800 support */
    CLK_LEDC,         /* 35 FIXME:sl2801 not support, sl6800 support */
    CLK_UART2,        /* 36 FIXME:sl2801 not support, sl6800 support */
    CLK_ASRC,         /* 37 FIXME:sl2801 not support, sl6800 support */
    CLK_SYSTICK,      /* 38 FIXME:sl2801 not support, sl6800 support */
    CLK_PWM,          /* 39 */
    CLK_DKC,          /* 40 */
    CLK_192M,         /* 41 */
    CPU_PLL_CLK,      /* 42 */
    CLK_NORF,         /* 43 */
    CLK_ANC,          /* 44 */
    CLK_MBIST,        /* 45 */
    CLK_MBOX,         /* 46 */
    CLK_SPINLOCK,     /* 47 */
    CLK_EQ,           /* 48 */
    CLK_DRC,          /* 49 */
    CLK_STWI,         /* 50 */
    CLK_HCEC,         /* 51 */
    CLK_TIMESTAMP,    /* 52 */
    CLK_PLLDIV_AUDX1, /* 53 */
    CLK_SPI,          /* 54 */
    CLK_AUDIOMCLK,    /* 55 */
    CLK_SPDIFTX,      /* 56 */
    CLK_SPDIFRX,      /* 57 */
    CLK_TWI0,         /* 58 */
    CLK_HOSC,         /* 59 external 24M osc */
    CLK_30M_RCOSC,    /* 60 */
    CLK_VAD,          /* 61 */
    CLK_VAD_RC30M,    /* 62 */
    CLK_LCDC,         /* 63 */
    CLK_DCLK,         /* 64 */

    CLK_INVALID = 100,/* 100 */
};

void HAL_clk_enable(enum clk_id_e clk_id);
void HAL_clk_disable(enum clk_id_e clk_id);

void HAL_clk_set_div(enum clk_id_e clk_id, uint32_t div);
uint32_t HAL_clk_get_div(enum clk_id_e clk_id);
uint32_t HAL_clk_get_frac_div(enum clk_id_e clk_id);
void HAL_clk_set_frac_div(enum clk_id_e clk_id, uint32_t div);

void HAL_clk_set_rate(enum clk_id_e clk_id, uint32_t rate);
uint32_t HAL_clk_get_rate(enum clk_id_e clk_id);
void HAL_clk_set_src(enum clk_id_e clk_id, enum clk_id_e src);
enum clk_id_e HAL_clk_get_src(enum clk_id_e clk_id);

void module_reset();
bool module_is_enable(enum ccu_id_e ccu_id);
void module_ccu_enable(enum ccu_id_e ccu_id);
void module_ccu_disable(enum ccu_id_e ccu_id);

void HAL_set_corepll_clk(uint32_t rate);
uint32_t HAL_get_corepll_clk(void);


void auto_set_cpux_freq(void);
void charge_cpu_freq_enable(uint8_t en);
uint32_t get_cpu_low_freq_delay_time(void);
void update_cpu_low_freq_delay_time(void);
void set_cpu_low_freq_delay_time(uint32_t delay_ms);

void HAL_clk_bypass_usb_suspend_clk(bool bypass);
void HAL_clk_dcxo_set(uint16_t dcxo_current, bool dcxo_lp_en, bool xo32k_en);
void HAL_clk_init(uint32_t cpu_src, uint32_t pll_rate, uint32_t bus_rate);
void HAL_clk_reinit(uint32_t volt, uint32_t pll, uint32_t sys_div, uint32_t cpu0_div, uint32_t cpu1_div, uint32_t ahb_div, uint32_t nor_div);
void HAL_clk_reset_cpu1(void);

uint32_t HAL_clk_log2(uint32_t m);
/* clk_rc30m_cal(0, 4, 0, 0), cal 1024cycles */

uint32_t HAL_sys_get_ver(void);
void HAL_clk_set_vf_table(vf_table_t *table);
void HAL_clk_set_sram_speed(enum clk_sram_speed_e sram_speed);
void HAL_audiopll_sleep(void);
void HAL_set_audiopll_rate_ex(uint32_t rate);
void HAL_adjust_audiopll(uint32_t rate);
uint32_t HAL_get_audiopll_rate(void);
void HAL_clk_set_cpux_freq(uint32_t cpu0_freq, uint32_t cpu1_freq);
void set_cpu_volt_value(uint32_t value);
void HAL_ahb_src_set(uint32_t ahb_src);
bool HAL_ahb_src_is_use_24m_hosc(void);
bool HAL_vf_table_vail_get(void);

#ifdef __cplusplus
}
#endif

#endif /* _HAL_CLK_LIB_H_ */

