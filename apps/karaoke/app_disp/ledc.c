#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "hal_gpio.h"
#include "hal_ledc.h"
#include "ledc.h"
#include "led_digital.h"


#if LED_DIGITAL_EN

uint32_t led_lt[2]; //for light led enable

//extern uint8_t led_buf[LED_COM_NUM];
//extern const uint8_t led_truth_tbl[LED_MAX_NUM];

uint8_t *led_buf_ptr;

AT_SRAM(.ledc_rodata)
const uint8_t led_truth_tbl[LED_MAX_NUM] =
{
    //0---------1---------2---------3---------4---------5---------6---------
    LED_1A,   LED_1B,   LED_1E,   LED_AUX,  LED_TF,   LED_NULL, LED_NULL,

    //7---------8---------9--------10--------11--------12--------13---------
    LED_1F,   LED_2A,   LED_2B,   LED_2E,   LED_2D,   LED_NULL, LED_NULL,

    //14--------15--------16--------17-------18--------19--------20---------
    LED_1G,   LED_2F,   LED_COL,  LED_3B,   LED_FM,   LED_BAT0, LED_NULL,

    //21--------22--------23--------24--------25--------26--------27--------
    LED_1C,   LED_2G,   LED_3F,   LED_3C,   LED_4E,   LED_NULL, LED_NULL,

    //28--------29--------30--------31--------32--------33--------34--------
    LED_1D,   LED_2C,   LED_3G,   LED_3A,   LED_4C,   LED_4G,   LED_NULL,

    //35--------36--------37--------38--------39--------40--------41--------
    LED_3D,   LED_USB,  LED_3E,   LED_4D,   LED_4F,   LED_4B,   LED_NULL,

    //42--------43--------44--------45--------46--------47--------48--------
    LED_NULL, LED_NULL, LED_BAT1, LED_NULL, LED_NULL, LED_4A,   LED_NULL,
    
    //49--------50--------51--------52--------53--------54--------55--------
    LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_NULL, LED_NULL,
};

void ledc_init(void)
{
    ledc_param_t ledc_param;

    LEDC_X0_INIT();
    LEDC_X1_INIT();
    LEDC_X2_INIT();
    LEDC_X3_INIT();
    LEDC_X4_INIT();
    LEDC_X5_INIT();
    LEDC_X6_INIT();
    LEDC_X7_INIT();
    LEDC_X8_INIT();
    LEDC_X9_INIT();
    LEDC_X10_INIT();
    LEDC_X11_INIT();
    LEDC_X12_INIT();
    LEDC_X13_INIT();
    LEDC_X14_INIT();
    LEDC_X15_INIT();
    ledc_param.io_ctrl.io_mapping[0] = LEDC_X0_MAP;
    ledc_param.io_ctrl.io_mapping[1] = LEDC_X1_MAP;
    ledc_param.io_ctrl.io_mapping[2] = LEDC_X2_MAP;
    ledc_param.io_ctrl.io_mapping[3] = LEDC_X3_MAP;
    ledc_param.io_ctrl.io_mapping[4] = LEDC_X4_MAP;
    ledc_param.io_ctrl.io_mapping[5] = LEDC_X5_MAP;
    ledc_param.io_ctrl.io_mapping[6] = LEDC_X6_MAP;
    ledc_param.io_ctrl.io_mapping[7] = LEDC_X7_MAP;
    ledc_param.io_ctrl.io_mapping[8] = LEDC_X8_MAP;
    ledc_param.io_ctrl.io_mapping[9] = LEDC_X9_MAP;
    ledc_param.io_ctrl.io_mapping[10] = LEDC_X10_MAP;
    ledc_param.io_ctrl.io_mapping[11] = LEDC_X11_MAP;
    ledc_param.io_ctrl.io_mapping[12] = LEDC_X12_MAP;
    ledc_param.io_ctrl.io_mapping[13] = LEDC_X13_MAP;
    ledc_param.io_ctrl.io_mapping[14] = LEDC_X14_MAP;
    ledc_param.io_ctrl.io_mapping[15] = LEDC_X15_MAP;
    ledc_param.io_ctrl.io_en[0] = !!LEDC_X0_SEL;
    ledc_param.io_ctrl.io_en[1] = !!LEDC_X1_SEL;
    ledc_param.io_ctrl.io_en[2] = !!LEDC_X2_SEL;
    ledc_param.io_ctrl.io_en[3] = !!LEDC_X3_SEL;
    ledc_param.io_ctrl.io_en[4] = !!LEDC_X4_SEL;
    ledc_param.io_ctrl.io_en[5] = !!LEDC_X5_SEL;
    ledc_param.io_ctrl.io_en[6] = !!LEDC_X6_SEL;
    ledc_param.io_ctrl.io_en[7] = !!LEDC_X7_SEL;
    ledc_param.io_ctrl.io_en[8] = !!LEDC_X8_SEL;
    ledc_param.io_ctrl.io_en[9] = !!LEDC_X9_SEL;
    ledc_param.io_ctrl.io_en[10] = !!LEDC_X10_SEL;
    ledc_param.io_ctrl.io_en[11] = !!LEDC_X11_SEL;
    ledc_param.io_ctrl.io_en[12] = !!LEDC_X12_SEL;
    ledc_param.io_ctrl.io_en[13] = !!LEDC_X13_SEL;
    ledc_param.io_ctrl.io_en[14] = !!LEDC_X14_SEL;
    ledc_param.io_ctrl.io_en[15] = !!LEDC_X15_SEL;

    ledc_param.clk_src = LEDC_CLK_LOSC;
    if(ledc_param.clk_src == LEDC_CLK_100K) {
        ledc_param.clk_div = 9;         //(9+1)/100k = 0.1ms
    } else {
        ledc_param.clk_div = 2;         //(2+1)/32k = 0.094ms
    }
    ledc_param.keep_time = 4;           //(4+1) *0.1 = 0.5ms

    ledc_param.det_en = LEDC_DET_NONE;
    ledc_param.int_en = LEDC_INT_NONE;
    ledc_param.det_dbc = 0xFF;

    ledc_param.ledc_interface.irq_callback = ledc_irq_cb;
    ledc_param.ledc_interface.null_get     = ledc_null_get;
    HAL_ledc_module_init(&ledc_param);
}

void ledc_deinit(void)
{
    HAL_ledc_module_deinit();
}

void ledc_null_get(uint32_t *led_null)
{
    //the bit of null LED, config to 1
    for(uint32_t m=0; m<LED_MAX_NUM; m++) {
        if(LED_NULL == led_truth_tbl[m]) {
            if(m < 32) {
                led_null[0] |= BIT(m);
            } else if(m < 64) {
                led_null[1] |= BIT(m-32);
            } 
            
            led_null[2]++;     //null cnt
        }
    }
}

AT_SRAM(.ledc)
void ledc_lt_get(uint32_t led_num)
{
    uint32_t n;
    for(n=0; n<LED_MAX_NUM; n++) {
        if(led_num == led_truth_tbl[n]) {
            break;
        }
    }

    if(n < 32) {
        led_lt[0] |= BIT(n);
    } else if (n < 64) {
        led_lt[1] |= BIT(n-32);
    } 
}

AT_SRAM(.ledc)
void ledc_update(void)
{
    uint32_t n;

    led_lt[0] = 0;
    led_lt[1] = 0;
    led_buf_ptr = (uint8_t *) get_led_buf();
    for(uint32_t group=0; group<LED_COM_NUM; group++) {
        for(uint32_t seg=0; seg<=LED_SEG_NUM; seg++) {
            if(led_buf_ptr[group] & BIT(seg)) {
                n = group<<4;
                ledc_lt_get(n+seg+1);
            }
        }
    }

    HAL_ledc_lt_update(led_lt);
}

AT_SRAM(.ledc)
void ledc_irq_cb(uint32_t reg)
{
    if (reg & LEDC_INT_D0_FALL) {
    }
    if (reg & LEDC_INT_D1_FALL) {
    }
    if (reg & LEDC_INT_D2_FALL) {
    }
    if (reg & LEDC_INT_D3_FALL) {
    }
    if (reg & LEDC_INT_D4_FALL) {
    }
    if (reg & LEDC_INT_D5_FALL) {
    }
    if (reg & LEDC_INT_D6_FALL) {
    }
    if (reg & LEDC_INT_D7_FALL) {
    }
    if (reg & LEDC_INT_D8_FALL) {
    }


    if (reg & LEDC_INT_D0_RISE) {
    }
    if (reg & LEDC_INT_D1_RISE) {
    }
    if (reg & LEDC_INT_D2_RISE) {
    }
    if (reg & LEDC_INT_D3_RISE) {
    }
    if (reg & LEDC_INT_D4_RISE) {
    }
    if (reg & LEDC_INT_D5_RISE) {
    }
    if (reg & LEDC_INT_D6_RISE) {
    }
    if (reg & LEDC_INT_D7_RISE) {
    }
    if (reg & LEDC_INT_D8_RISE) {
    }

}
#endif
