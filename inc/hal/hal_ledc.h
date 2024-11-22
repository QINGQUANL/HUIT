#ifndef _HAL_LEDC_LIB_H_
#define _HAL_LEDC_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif


#define LED_MAX_NUM     56
#define LED_PIN_NUM     8

enum ledc_clk_e {
    LEDC_CLK_100K = (0x00<<0),
    LEDC_CLK_LOSC = (0x01<<0),
};

enum ledc_det_e {
    LEDC_DET_NONE = (0x0),

    LEDC_DET_D0 = (0x1<<0),
    LEDC_DET_D1 = (0x1<<1),
    LEDC_DET_D2 = (0x1<<2),
    LEDC_DET_D3 = (0x1<<3),
    LEDC_DET_D4 = (0x1<<4),
    LEDC_DET_D5 = (0x1<<5),
    LEDC_DET_D6 = (0x1<<6),
    LEDC_DET_D7 = (0x1<<7),
    LEDC_DET_D8 = (0x1<<8),

    LEDC_DET_MASK = (0x1FF),
};

enum ledc_int_e {
    LEDC_INT_NONE = (0x0),

    LEDC_INT_D0_FALL = (0x1<<0),
    LEDC_INT_D1_FALL = (0x1<<1),
    LEDC_INT_D2_FALL = (0x1<<2),
    LEDC_INT_D3_FALL = (0x1<<3),
    LEDC_INT_D4_FALL = (0x1<<4),
    LEDC_INT_D5_FALL = (0x1<<5),
    LEDC_INT_D6_FALL = (0x1<<6),
    LEDC_INT_D7_FALL = (0x1<<7),
    LEDC_INT_D8_FALL = (0x1<<8),

    LEDC_INT_D0_RISE = (0x1<<16),
    LEDC_INT_D1_RISE = (0x1<<17),
    LEDC_INT_D2_RISE = (0x1<<18),
    LEDC_INT_D3_RISE = (0x1<<19),
    LEDC_INT_D4_RISE = (0x1<<20),
    LEDC_INT_D5_RISE = (0x1<<21),
    LEDC_INT_D6_RISE = (0x1<<22),
    LEDC_INT_D7_RISE = (0x1<<23),
    LEDC_INT_D8_RISE = (0x1<<24),

    LEDC_INT_MASK    = (0x01FF01FF),
};

typedef struct
{
    void (*null_get)(uint32_t *led_null);
    void (*irq_callback)(uint32_t reg);
} ledc_interface_t;

typedef struct {
    uint8_t io_mapping[16];
    bool io_en[16];
} ledc_io_ctrl_type;


typedef struct
{
    uint32_t clk_src;       //clock source
    uint32_t clk_div;       //clock deivider
    uint32_t keep_time;     //LT_KP_TIME, (scan period of every led)
    uint32_t det_en;        //input state detect enable
    uint32_t int_en;        //input rise/fall interrupt enable
    uint8_t det_dbc;        //pin detect debounce
    ledc_interface_t ledc_interface;
    ledc_io_ctrl_type io_ctrl;
} ledc_param_t;


void HAL_ledc_module_init(ledc_param_t *ledc_param);
void HAL_ledc_module_deinit(void);
void HAL_ledc_module_io_setting(ledc_io_ctrl_type *io_ctrl);
void HAL_ledc_lt_update(uint32_t *lt);
void ledc_null_get(uint32_t *led_null);
void ledc_irq_cb(uint32_t reg);
void ledc_irq_handler(void);
uint32_t HAL_ledc_input_sta_get(void);


#ifdef __cplusplus
}
#endif

#endif
