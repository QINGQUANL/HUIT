#ifndef _HAL_GPIO_H_
#define _HAL_GPIO_H_

#ifdef __cplusplus
 extern "C" {
#endif

#define PIN_PULL_VAL_SHIFT          (0)
#define PIN_PULL_VAL_MASK           (0x3 << PIN_PULL_VAL_SHIFT)
#define PIN_PULL_SHIFT              (2)
#define PIN_PULL_MASK               (0x3 << PIN_PULL_SHIFT)
#define PIN_DRIVING_SHIFT           (4)
#define PIN_DRIVING_MASK            (0x3 << PIN_DRIVING_SHIFT)
#define PIN_VALUE_SHIFT             (6)
#define PIN_VALUE_MASK              (0x1 << PIN_VALUE_SHIFT)
#define PIN_FUNC_SHIFT              (7)
#define PIN_FUNC_MASK               (0xf << PIN_FUNC_SHIFT)
#define PIN_ID_SHIFT                (11)
#define PIN_ID_MASK                 (0x1f << PIN_ID_SHIFT)
#define PIN_GROUP_SHIFT             (16)
#define PIN_GROUP_MASK              (0xf << PIN_GROUP_SHIFT)

#define _PIN_NAME(group, index) \
    ((group << PIN_GROUP_SHIFT) | (index << PIN_ID_SHIFT))

#define _PIN_FUNC(func) (func << PIN_FUNC_SHIFT)

#define _PIN_VAL(val) (val << PIN_VALUE_SHIFT)

#define _PIN_DRI(driving) (driving << PIN_DRIVING_SHIFT)

#define _PIN_PULL(pull, val) \
    ((pull << PIN_PULL_SHIFT) | (val << PIN_PULL_VAL_SHIFT))

enum pin_name_e {
    /* PAx 引脚定义 */
    PIN_A0  = _PIN_NAME(0,0),
    PIN_A1  = _PIN_NAME(0,1),
    PIN_A2  = _PIN_NAME(0,2),
    PIN_A3  = _PIN_NAME(0,3),
    PIN_A4  = _PIN_NAME(0,4),
    PIN_A5  = _PIN_NAME(0,5),
    PIN_A6  = _PIN_NAME(0,6),
    PIN_A7  = _PIN_NAME(0,7),
    PIN_A8  = _PIN_NAME(0,8),
    PIN_A9  = _PIN_NAME(0,9),
    PIN_A10 = _PIN_NAME(0,10),
    PIN_A11 = _PIN_NAME(0,11),
    PIN_A12 = _PIN_NAME(0,12),
    PIN_A13 = _PIN_NAME(0,13),
    PIN_A14 = _PIN_NAME(0,14),
    PIN_A15 = _PIN_NAME(0,15),
    PIN_A16 = _PIN_NAME(0,16),
    PIN_A17 = _PIN_NAME(0,17),
    PIN_A18 = _PIN_NAME(0,18),
    PIN_A19 = _PIN_NAME(0,19),
    PIN_A20 = _PIN_NAME(0,20),
    PIN_A21 = _PIN_NAME(0,21),
    PIN_A22 = _PIN_NAME(0,22),
    PIN_A23 = _PIN_NAME(0,23),
    PIN_A24 = _PIN_NAME(0,24),
    PIN_A25 = _PIN_NAME(0,25),
    PIN_A26 = _PIN_NAME(0,26),
    PIN_A27 = _PIN_NAME(0,27),
    PIN_A28 = _PIN_NAME(0,28),
    PIN_A29 = _PIN_NAME(0,29),
    PIN_A30 = _PIN_NAME(0,30),
    PIN_A31 = _PIN_NAME(0,31),

    /* PBx 引脚定义 */
    PIN_B0  = _PIN_NAME(1,0),
    PIN_B1  = _PIN_NAME(1,1),
    PIN_B2  = _PIN_NAME(1,2),
    PIN_B3  = _PIN_NAME(1,3),
    PIN_B4  = _PIN_NAME(1,4),
    PIN_B5  = _PIN_NAME(1,5),
    PIN_B6  = _PIN_NAME(1,6),
    PIN_B7  = _PIN_NAME(1,7),
    PIN_B8  = _PIN_NAME(1,8),
    PIN_B9  = _PIN_NAME(1,9),
    PIN_B10 = _PIN_NAME(1,10),
    PIN_B11 = _PIN_NAME(1,11),
    PIN_B12 = _PIN_NAME(1,12),
    PIN_B13 = _PIN_NAME(1,13),
    PIN_B14 = _PIN_NAME(1,14),
    PIN_B15 = _PIN_NAME(1,15),
    PIN_B16 = _PIN_NAME(1,16),
    PIN_B17 = _PIN_NAME(1,17),
    PIN_B18 = _PIN_NAME(1,18),
    PIN_B19 = _PIN_NAME(1,19),
    PIN_B20 = _PIN_NAME(1,20),
    PIN_B21 = _PIN_NAME(1,21),
    PIN_B22 = _PIN_NAME(1,22),
    PIN_B23 = _PIN_NAME(1,23),
    PIN_B24 = _PIN_NAME(1,24),
    PIN_B25 = _PIN_NAME(1,25),
    PIN_B26 = _PIN_NAME(1,26),
    PIN_B27 = _PIN_NAME(1,27),
    PIN_B28 = _PIN_NAME(1,28),
    PIN_B29 = _PIN_NAME(1,29),
    PIN_B30 = _PIN_NAME(1,30),
    PIN_B31 = _PIN_NAME(1,31),

    /* PCx 引脚定义 */
    PIN_C0  = _PIN_NAME(2,0),
    PIN_C1  = _PIN_NAME(2,1),
    PIN_C2  = _PIN_NAME(2,2),
    PIN_C3  = _PIN_NAME(2,3),
    PIN_C4  = _PIN_NAME(2,4),
    PIN_C5  = _PIN_NAME(2,5),
    PIN_C6  = _PIN_NAME(2,6),
    PIN_C7  = _PIN_NAME(2,7),
    PIN_C8  = _PIN_NAME(2,8),
    PIN_C9  = _PIN_NAME(2,9),
    PIN_C10 = _PIN_NAME(2,10),
    PIN_C11 = _PIN_NAME(2,11),
    PIN_C12 = _PIN_NAME(2,12),
    PIN_C13 = _PIN_NAME(2,13),
    PIN_C14 = _PIN_NAME(2,14),
    PIN_C15 = _PIN_NAME(2,15),
    PIN_C16 = _PIN_NAME(2,16),
    PIN_C17 = _PIN_NAME(2,17),
    PIN_C18 = _PIN_NAME(2,18),
    PIN_C19 = _PIN_NAME(2,19),
    PIN_C20 = _PIN_NAME(2,20),
    PIN_C21 = _PIN_NAME(2,21),
    PIN_C22 = _PIN_NAME(2,22),
    PIN_C23 = _PIN_NAME(2,23),
    PIN_C24 = _PIN_NAME(2,24),
    PIN_C25 = _PIN_NAME(2,25),
    PIN_C26 = _PIN_NAME(2,26),
    PIN_C27 = _PIN_NAME(2,27),
    PIN_C28 = _PIN_NAME(2,28),
    PIN_C29 = _PIN_NAME(2,29),
    PIN_C30 = _PIN_NAME(2,30),
    PIN_C31 = _PIN_NAME(2,31),

    /* PDx 引脚定义 */
    PIN_D0  = _PIN_NAME(3,0),
    PIN_D1  = _PIN_NAME(3,1),
    PIN_D2  = _PIN_NAME(3,2),
    PIN_D3  = _PIN_NAME(3,3),
    PIN_D4  = _PIN_NAME(3,4),
    PIN_D5  = _PIN_NAME(3,5),
    PIN_D6  = _PIN_NAME(3,6),
    PIN_D7  = _PIN_NAME(3,7),
    PIN_D8  = _PIN_NAME(3,8),
    PIN_D9  = _PIN_NAME(3,9),
    PIN_D10 = _PIN_NAME(3,10),
    PIN_D11 = _PIN_NAME(3,11),
    PIN_D12 = _PIN_NAME(3,12),
    PIN_D13 = _PIN_NAME(3,13),
    PIN_D14 = _PIN_NAME(3,14),
    PIN_D15 = _PIN_NAME(3,15),
    PIN_D16 = _PIN_NAME(3,16),
    PIN_D17 = _PIN_NAME(3,17),
    PIN_D18 = _PIN_NAME(3,18),
    PIN_D19 = _PIN_NAME(3,19),
    PIN_D20 = _PIN_NAME(3,20),
    PIN_D21 = _PIN_NAME(3,21),
    PIN_D22 = _PIN_NAME(3,22),
    PIN_D23 = _PIN_NAME(3,23),
    PIN_D24 = _PIN_NAME(3,24),
    PIN_D25 = _PIN_NAME(3,25),
    PIN_D26 = _PIN_NAME(3,26),
    PIN_D27 = _PIN_NAME(3,27),
    PIN_D28 = _PIN_NAME(3,28),
    PIN_D29 = _PIN_NAME(3,29),
    PIN_D30 = _PIN_NAME(3,30),
    PIN_D31 = _PIN_NAME(3,31),
};

/* 引脚功能定义 */
enum pin_func_e {
    PIN_FUNC0  = _PIN_FUNC(0),
    PIN_FUNC1  = _PIN_FUNC(1),
    PIN_FUNC2  = _PIN_FUNC(2),
    PIN_FUNC3  = _PIN_FUNC(3),
    PIN_FUNC4  = _PIN_FUNC(4),
    PIN_FUNC5  = _PIN_FUNC(5),
    PIN_FUNC6  = _PIN_FUNC(6),
    PIN_FUNC7  = _PIN_FUNC(7),
    PIN_FUNC8  = _PIN_FUNC(8),
    PIN_FUNC9  = _PIN_FUNC(9),
    PIN_FUNC10 = _PIN_FUNC(10),
    PIN_FUNC11 = _PIN_FUNC(11),
    PIN_FUNC12 = _PIN_FUNC(12),
    PIN_FUNC13 = _PIN_FUNC(13),
    PIN_FUNC14 = _PIN_FUNC(14),
    PIN_FUNC15 = _PIN_FUNC(15),

    PIN_FUNCINPUT   = PIN_FUNC0,    /* func0为输入功能 */
    PIN_FUNCOUTPUT  = PIN_FUNC1,    /* func1为输出功能 */
    PIN_FUNCEINT    = PIN_FUNC14,   /* func14为中断功能 */
    PIN_FUNCDISABLE = PIN_FUNC15,   /* func15为disable */
};

/* 引脚电平定义 */
enum pin_value_e {
    PIN_LOW     = _PIN_VAL(0), /* 低电平 */
    PIN_HIGH    = _PIN_VAL(1), /* 高电平 */
};

/* 引脚驱动能力定义（档位越高驱动能力越强） */
enum pin_driving_e {
    PIN_DRI0  =_PIN_DRI(0),   /* 驱动能力档位0 */
    PIN_DRI1  =_PIN_DRI(1),   /* 驱动能力档位1 */
    PIN_DRI2  =_PIN_DRI(2),   /* 驱动能力档位2 */
    PIN_DRI3  =_PIN_DRI(3),   /* 驱动能力档位3 */
};

/* 引脚上下拉功能定义 */
enum pin_pull_e {
    PIN_PULL_DISABLE            = _PIN_PULL(0,0), /* 无上下拉 */
    PIN_PULL_UP2K               = _PIN_PULL(2,0), /* 上拉2K */
    PIN_PULL_UP100K             = _PIN_PULL(2,3), /* 上拉100K */
    PIN_PULL_DOWN2K             = _PIN_PULL(1,0), /* 下拉2K */
    PIN_PULL_DOWN100K           = _PIN_PULL(1,1), /* 下拉100K */
    PIN_PULL_UP2K_DOWN2K        = _PIN_PULL(3,0), /* 上拉2K以及下拉100K */
    PIN_PULL_UP2K_DOWN100K      = _PIN_PULL(3,1), /* 上拉100K以及下拉2K */
    PIN_PULL_UP100K_DOWN2K      = _PIN_PULL(3,2), /* 上拉100K以及下拉2K */
    PIN_PULL_UP100K_DOWN100K    = _PIN_PULL(3,3), /* 上拉100K以及下拉100K */
};

/* 引脚中断使能定义 */
enum pin_irq_en_type {
    PIN_IRQ_DIS  = 0x0<<31,
    PIN_IRQ_EN   = 0x1<<31,
};

/* 引脚中断类型定义 */
enum pin_irq_type {
    PIN_IRQ_R_EDGE  = 0x0, /* 上升沿中断 */
    PIN_IRQ_F_EDGE  = 0x1, /* 下降沿中断 */
    PIN_IRQ_H_LEV   = 0x2, /* 高电平中断 */
    PIN_IRQ_L_LEV   = 0x3, /* 低电平中断 */
    PIN_IRQ_RF_EDGE = 0x4, /* 上升以及下降沿中断 */
};

enum pin_disp_io_func {
    PIN_DISPLAY_LEDC = 0x0,
    PIN_DISPLAY_LCDC = 0x1,
};

/* 获取引脚的电平状态
 * @param pin:引脚定义， 参考@enum pin_name_e
 * @return: 低电平返回0； 高电平返回1
 * Example:
 *      //获取PB8引脚电平状态
 *      sta = HAL_gpio_value_get(PIN_B8);
 */
uint32_t HAL_gpio_value_get(uint32_t pin);


/* 设置引脚的电平
 * @param pin:引脚定义， 参考@enum pin_name_e
 * @param value:设置的电平状态； 0-低电平； 1-高电平
 * Example:
 *      //设置PB8引脚为高电平
 *      HAL_gpio_value_set(PIN_B8， 1);
 */
void HAL_gpio_value_set(uint32_t pin, uint32_t value);


//pin=(pin_name), irqclk_cfg=([15:8]-debounce clock cycles | [7]-interrupt clock select | [6:4]-debounce clock pre-scale n)
void HAL_gpio_irq_clk_config(uint32_t pin, uint32_t irqclk_cfg);


/* 引脚中断配置
 * @param pin:引脚定义， 参考@enum pin_name_e
 * @param irq_cfg: 中断配置 irq_cfg=(pin_irq_en_type|pin_irq_type)
 * 在本函数中，会将对应gpio的function配置为FUNC14中断功能
 * Example:
 *      //使能PB8上升下降沿中断
 *      HAL_gpio_irq_config(PIN_B8，PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
 *      //关闭PB8中断
 *      HAL_gpio_irq_config(PIN_B8，PIN_IRQ_DIS);
 */
void HAL_gpio_irq_config(uint32_t pin, uint32_t irq_cfg);


/* 获取引脚中断pending状态
 * @param pin:引脚定义， 参考@enum pin_name_e
 * @return: 返回中断pending
 */
bool HAL_gpio_irq_pending_get(uint32_t pin);


/* 清除引脚中断pending状态
 * @param pin:引脚定义， 参考@enum pin_name_e
 */
void HAL_gpio_irq_pending_clear(uint32_t pin);


/* 配置引脚； cfg = (pin_name | pin_func | pin_val | pin_driving | pin_pull)
 * @param cfg:引脚配置
 * Example:
 *      //设置PB8 func为中断功能
 *      HAL_gpio_pin_config(PIN_B8 | PIN_FUNCEINT);
 *
 *      //设置PB8 为func4, 驱动能力3， 上拉100K
 *      HAL_gpio_pin_config(PIN_B8 | PIN_FUNC4 | PIN_DRI3 | PIN_PULL_UP100K)；
 *
 *      //设置PB8 为输入功能， 下拉2K
 *      HAL_gpio_pin_config(PIN_B8 | PIN_FUNCINPUT | PIN_PULL_UP100K)；
 *
 *      //设置PB8 为输出功能， 且输出高电平
 *      HAL_gpio_pin_config(PIN_B8 | PIN_FUNCOUTPUT | PIN_HIGH)；
 */
void HAL_gpio_config(uint32_t cfg);


/* 获取引脚wakeup pending状态
 * @param pin:引脚定义， 参考@enum pin_name_e
 * @return: 返回wakeup pending
 */
bool HAL_gpio_wakeup_pending_get(uint32_t pin);


/* 清除引脚wakeup pending状态
 * @param pin:引脚定义， 参考@enum pin_name_e
 */
void HAL_gpio_wakeup_pending_clear(uint32_t pin);


/* 设置引脚wakeup
 * @param pin:引脚定义， 参考@enum pin_name_e
 * @param enable: true:使能wakeup; false:不使能wakeup
 */
void HAL_gpio_wakeup_set(uint32_t pin, bool enable);


/* 单独设置引脚功能
 * @param pin:引脚定义， 参考@enum pin_name_e
 * @param func:引脚功能， 参考@enum pin_func_e
 */
void HAL_gpio_func_set(uint32_t pin, uint32_t func);

/* 获取引脚功能
 * @param pin:引脚定义， 参考@enum pin_name_e
 * @return:返回当前引脚功能， 参考@enum pin_func_e
 */
uint32_t HAL_gpio_func_get(uint32_t pin);

/* 单独设置引脚驱动能力
 * @param pin:引脚定义， 参考@enum pin_name_e
 * @param func:引脚驱动能力， 参考@enum pin_driving_e
 */
void HAL_gpio_driving_set(uint32_t pin, uint32_t driving);


/* 单独设置引脚上下拉
 * @param pin:引脚定义， 参考@enum pin_name_e
 * @param func:引脚上下拉， 参考@enum pin_pull_e
 */
void HAL_gpio_pull_set(uint32_t pin, uint32_t pull);


#ifdef __cplusplus
}
#endif

#endif /* _HAL_GPIO_H_ */
