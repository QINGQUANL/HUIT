#ifndef _LEDC_H_
#define _LEDC_H_

#ifdef __cplusplus
 extern "C" {
#endif


#if (LEDC_X0_SEL==0)
#elif (LEDC_X0_SEL==1)
#define LEDC_X0_PIN_CFG             (PIN_D0 | PIN_FUNC5)        //for fpga
#elif (LEDC_X0_SEL==2)
#define LEDC_X0_PIN_CFG             (PIN_A4 | PIN_FUNC3)
#elif (LEDC_X0_SEL==3)
#define LEDC_X0_PIN_CFG             (PIN_B14 | PIN_FUNC12)
#elif (LEDC_X0_SEL==4)
#define LEDC_X0_PIN_CFG             (PIN_D11 | PIN_FUNC12)
#endif

#if (LEDC_X1_SEL==0)
#elif (LEDC_X1_SEL==1)
#define LEDC_X1_PIN_CFG             (PIN_D1 | PIN_FUNC5)        //for fpga
#elif (LEDC_X1_SEL==2)
#define LEDC_X1_PIN_CFG             (PIN_A5 | PIN_FUNC3)
#elif (LEDC_X1_SEL==3)
#define LEDC_X1_PIN_CFG             (PIN_B15 | PIN_FUNC12)
#endif

#if (LEDC_X2_SEL==0)
#elif (LEDC_X2_SEL==1)
#define LEDC_X2_PIN_CFG             (PIN_D2 | PIN_FUNC5)        //for fpga
#elif (LEDC_X2_SEL==2)
#define LEDC_X2_PIN_CFG             (PIN_B16 | PIN_FUNC12)
#endif

#if (LEDC_X3_SEL==0)
#elif (LEDC_X3_SEL==1)
#define LEDC_X3_PIN_CFG             (PIN_D3 | PIN_FUNC5)        //for fpga
#elif (LEDC_X3_SEL==2)
#define LEDC_X3_PIN_CFG             (PIN_B17 | PIN_FUNC12)
#endif

#if (LEDC_X4_SEL==0)
#elif (LEDC_X4_SEL==1)
#define LEDC_X4_PIN_CFG             (PIN_D4 | PIN_FUNC5)        //for fpga
#elif (LEDC_X4_SEL==2)
#define LEDC_X4_PIN_CFG             (PIN_B18 | PIN_FUNC12)
#endif

#if (LEDC_X5_SEL==0)
#elif (LEDC_X5_SEL==1)
#define LEDC_X5_PIN_CFG             (PIN_D5 | PIN_FUNC5)        //for fpga
#elif (LEDC_X5_SEL==2)
#define LEDC_X5_PIN_CFG             (PIN_B19 | PIN_FUNC12)
#endif

#if (LEDC_X6_SEL==0)
#elif (LEDC_X6_SEL==1)
#define LEDC_X6_PIN_CFG             (PIN_D6 | PIN_FUNC5)        //for fpga
#elif (LEDC_X6_SEL==2)
#define LEDC_X6_PIN_CFG             (PIN_B20 | PIN_FUNC12)
#elif (LEDC_X6_SEL==3)
#define LEDC_X6_PIN_CFG             (PIN_D1 | PIN_FUNC12)
#endif

#if (LEDC_X7_SEL==0)
#elif (LEDC_X7_SEL==1)
#define LEDC_X7_PIN_CFG             (PIN_D7 | PIN_FUNC5)        //for fpga
#elif (LEDC_X7_SEL==2)
#define LEDC_X7_PIN_CFG             (PIN_B21 | PIN_FUNC12)
#elif (LEDC_X7_SEL==3)
#define LEDC_X7_PIN_CFG             (PIN_D2 | PIN_FUNC12)
#endif

#if (LEDC_X8_SEL==0)
#elif (LEDC_X8_SEL==1)
#define LEDC_X8_PIN_CFG             (PIN_D8 | PIN_FUNC5)        //for fpga
#elif (LEDC_X8_SEL==2)
#define LEDC_X8_PIN_CFG             (PIN_B6 | PIN_FUNC12)
#elif (LEDC_X8_SEL==3)
#define LEDC_X8_PIN_CFG             (PIN_D3 | PIN_FUNC12)
#endif

#if (LEDC_X9_SEL==0)
#elif (LEDC_X9_SEL==1)
#define LEDC_X9_PIN_CFG             (PIN_D9 | PIN_FUNC5)        //for fpga
#elif (LEDC_X9_SEL==2)
#define LEDC_X9_PIN_CFG             (PIN_B5 | PIN_FUNC12)
#elif (LEDC_X9_SEL==3)
#define LEDC_X9_PIN_CFG             (PIN_D4 | PIN_FUNC12)
#endif

#if (LEDC_X10_SEL==0)
#elif (LEDC_X10_SEL==1)
#define LEDC_X10_PIN_CFG             (PIN_D10 | PIN_FUNC5)        //for fpga
#elif (LEDC_X10_SEL==2)
#define LEDC_X10_PIN_CFG             (PIN_B4 | PIN_FUNC12)
#elif (LEDC_X10_SEL==3)
#define LEDC_X10_PIN_CFG             (PIN_D5 | PIN_FUNC12)
#endif

#if (LEDC_X11_SEL==0)
#elif (LEDC_X11_SEL==1)
#define LEDC_X11_PIN_CFG             (PIN_D11 | PIN_FUNC5)        //for fpga
#elif (LEDC_X11_SEL==2)
#define LEDC_X11_PIN_CFG             (PIN_B3 | PIN_FUNC12)
#elif (LEDC_X11_SEL==3)
#define LEDC_X11_PIN_CFG             (PIN_D6 | PIN_FUNC12)
#endif

#if (LEDC_X12_SEL==0)
#elif (LEDC_X12_SEL==1)
#define LEDC_X12_PIN_CFG             (PIN_D12 | PIN_FUNC5)        //for fpga
#elif (LEDC_X12_SEL==2)
#define LEDC_X12_PIN_CFG             (PIN_B2 | PIN_FUNC12)
#elif (LEDC_X12_SEL==3)
#define LEDC_X12_PIN_CFG             (PIN_D7 | PIN_FUNC12)
#endif

#if (LEDC_X13_SEL==0)
#elif (LEDC_X13_SEL==1)
#define LEDC_X13_PIN_CFG             (PIN_D13 | PIN_FUNC5)        //for fpga
#elif (LEDC_X13_SEL==2)
#define LEDC_X13_PIN_CFG             (PIN_B1 | PIN_FUNC12)
#elif (LEDC_X13_SEL==3)
#define LEDC_X13_PIN_CFG             (PIN_D8 | PIN_FUNC12)
#endif

#if (LEDC_X14_SEL==0)
#elif (LEDC_X14_SEL==1)
#define LEDC_X14_PIN_CFG             (PIN_D14 | PIN_FUNC5)        //for fpga
#elif (LEDC_X14_SEL==2)
#define LEDC_X14_PIN_CFG             (PIN_B0 | PIN_FUNC12)
#elif (LEDC_X14_SEL==3)
#define LEDC_X14_PIN_CFG             (PIN_D9 | PIN_FUNC12)
#endif

#if (LEDC_X15_SEL==0)
#elif (LEDC_X15_SEL==1)
#define LEDC_X15_PIN_CFG             (PIN_D15 | PIN_FUNC5)        //for fpga
#elif (LEDC_X15_SEL==2)
#define LEDC_X15_PIN_CFG             (PIN_D10 | PIN_FUNC12)
#endif


#if (LEDC_X0_SEL==0)
#define LEDC_X0_INIT()
#else
#define LEDC_X0_INIT()              HAL_gpio_config(LEDC_X0_PIN_CFG);
#endif

#if (LEDC_X1_SEL==0)
#define LEDC_X1_INIT()
#else
#define LEDC_X1_INIT()              HAL_gpio_config(LEDC_X1_PIN_CFG);
#endif

#if (LEDC_X2_SEL==0)
#define LEDC_X2_INIT()
#else
#define LEDC_X2_INIT()              HAL_gpio_config(LEDC_X2_PIN_CFG);
#endif

#if (LEDC_X3_SEL==0)
#define LEDC_X3_INIT()
#else
#define LEDC_X3_INIT()              HAL_gpio_config(LEDC_X3_PIN_CFG);
#endif

#if (LEDC_X4_SEL==0)
#define LEDC_X4_INIT()
#else
#define LEDC_X4_INIT()              HAL_gpio_config(LEDC_X4_PIN_CFG);
#endif

#if (LEDC_X5_SEL==0)
#define LEDC_X5_INIT()
#else
#define LEDC_X5_INIT()              HAL_gpio_config(LEDC_X5_PIN_CFG);
#endif

#if (LEDC_X6_SEL==0)
#define LEDC_X6_INIT()
#else
#define LEDC_X6_INIT()              HAL_gpio_config(LEDC_X6_PIN_CFG);
#endif

#if (LEDC_X7_SEL==0)
#define LEDC_X7_INIT()
#else
#define LEDC_X7_INIT()              HAL_gpio_config(LEDC_X7_PIN_CFG);
#endif

#if (LEDC_X8_SEL==0)
#define LEDC_X8_INIT()
#else
#define LEDC_X8_INIT()              HAL_gpio_config(LEDC_X8_PIN_CFG);
#endif

#if (LEDC_X9_SEL==0)
#define LEDC_X9_INIT()
#else
#define LEDC_X9_INIT()              HAL_gpio_config(LEDC_X9_PIN_CFG);
#endif

#if (LEDC_X10_SEL==0)
#define LEDC_X10_INIT()
#else
#define LEDC_X10_INIT()             HAL_gpio_config(LEDC_X10_PIN_CFG);
#endif

#if (LEDC_X11_SEL==0)
#define LEDC_X11_INIT()
#else
#define LEDC_X11_INIT()             HAL_gpio_config(LEDC_X11_PIN_CFG);
#endif

#if (LEDC_X12_SEL==0)
#define LEDC_X12_INIT()
#else
#define LEDC_X12_INIT()             HAL_gpio_config(LEDC_X12_PIN_CFG);
#endif

#if (LEDC_X13_SEL==0)
#define LEDC_X13_INIT()
#else
#define LEDC_X13_INIT()             HAL_gpio_config(LEDC_X13_PIN_CFG);
#endif

#if (LEDC_X14_SEL==0)
#define LEDC_X14_INIT()
#else
#define LEDC_X14_INIT()             HAL_gpio_config(LEDC_X14_PIN_CFG);
#endif

#if (LEDC_X15_SEL==0)
#define LEDC_X15_INIT()
#else
#define LEDC_X15_INIT()             HAL_gpio_config(LEDC_X15_PIN_CFG);
#endif


#define LED_COM_NUM     5
#define LED_SEG_NUM     7


#define LED_NULL        0x00

#define LED_COL         0x01
#define LED_TF          0x02
#define LED_USB         0x03
#define LED_BT          0x04
#define LED_FM          0x04
#define LED_AUX         0x05
#define LED_BAT0        0x06
#define LED_BAT1        0x07

#define LED_1A          0x11
#define LED_1B          0x12
#define LED_1C          0x13
#define LED_1D          0x14
#define LED_1E          0x15
#define LED_1F          0x16
#define LED_1G          0x17

#define LED_2A          0x21
#define LED_2B          0x22
#define LED_2C          0x23
#define LED_2D          0x24
#define LED_2E          0x25
#define LED_2F          0x26
#define LED_2G          0x27

#define LED_3A          0x31
#define LED_3B          0x32
#define LED_3C          0x33
#define LED_3D          0x34
#define LED_3E          0x35
#define LED_3F          0x36
#define LED_3G          0x37

#define LED_4A          0x41
#define LED_4B          0x42
#define LED_4C          0x43
#define LED_4D          0x44
#define LED_4E          0x45
#define LED_4F          0x46
#define LED_4G          0x47


//define of every segment
#define LED_SEG_A       (1<<0)
#define LED_SEG_B       (1<<1)
#define LED_SEG_C       (1<<2)
#define LED_SEG_D       (1<<3)
#define LED_SEG_E       (1<<4)
#define LED_SEG_F       (1<<5)
#define LED_SEG_G       (1<<6)

#define LED_SEG_COL     (1<<0)
#define LED_SEG_TF      (1<<1)
#define LED_SEG_USB     (1<<2)
#define LED_SEG_BT      (1<<3)
#define LED_SEG_FM      (1<<3)
#define LED_SEG_AUX     (1<<4)
#define LED_SEG_BAT0    (1<<5)
#define LED_SEG_BAT1    (1<<6)


//the segment of every number and letter:
#define LED_0       (LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_E | LED_SEG_F)
#define LED_1       (LED_SEG_B | LED_SEG_C)
#define LED_2       (LED_SEG_A | LED_SEG_B | LED_SEG_G | LED_SEG_E | LED_SEG_D)
#define LED_3       (LED_SEG_A | LED_SEG_B | LED_SEG_G | LED_SEG_C | LED_SEG_D)
#define LED_4       (LED_SEG_B | LED_SEG_C | LED_SEG_F | LED_SEG_G)
#define LED_5       (LED_SEG_A | LED_SEG_C | LED_SEG_D | LED_SEG_F | LED_SEG_G)
#define LED_6       (LED_SEG_A | LED_SEG_C | LED_SEG_D | LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_7       (LED_SEG_A | LED_SEG_B | LED_SEG_C)
#define LED_8       (LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_9       (LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_F | LED_SEG_G)
#define LED_A       (LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_B       (LED_SEG_C | LED_SEG_D | LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_C       (LED_SEG_A | LED_SEG_D | LED_SEG_E | LED_SEG_F)
#define LED_D       (LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_E | LED_SEG_G)
#define LED_E       (LED_SEG_A | LED_SEG_D | LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_F       (LED_SEG_A | LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_G       (LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_F | LED_SEG_G)
#define LED_H       (LED_SEG_B | LED_SEG_C | LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_I       (LED_SEG_B | LED_SEG_C)
#define LED_J       (LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_E)
#define LED_K       (LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_L       (LED_SEG_D | LED_SEG_E | LED_SEG_F)
#define LED_M       (LED_SEG_C | LED_SEG_E | LED_SEG_G)
#define LED_N       (LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_E | LED_SEG_F)
#define LED_O       (LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_E | LED_SEG_F)
#define LED_P       (LED_SEG_A | LED_SEG_B | LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_Q       (LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_F | LED_SEG_G)
#define LED_R       (LED_SEG_E | LED_SEG_G)
#define LED_S       (LED_SEG_A | LED_SEG_C | LED_SEG_D | LED_SEG_F | LED_SEG_G)
#define LED_T       (LED_SEG_D | LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_U       (LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_E | LED_SEG_F)
#define LED_V       (LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_F | LED_SEG_E)
#define LED_W       (LED_SEG_C | LED_SEG_D | LED_SEG_E)
#define LED_X       (LED_SEG_B | LED_SEG_C | LED_SEG_E | LED_SEG_F)
#define LED_Y       (LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_F | LED_SEG_G)
#define LED_Z       (LED_SEG_A | LED_SEG_B | LED_SEG_D | LED_SEG_E | LED_SEG_G)

#define LED__       (LED_SEG_G)
#define LED_ON      (LED_8)
#define LED_OFF     (LED_NULL)


void ledc_init(void);
void ledc_deinit(void);
void ledc_null_get(uint32_t *led_null);
void ledc_irq_cb(uint32_t reg);
void ledc_lt_get(uint32_t led_num);
void ledc_update(void);


#ifdef __cplusplus
}
#endif

#endif

