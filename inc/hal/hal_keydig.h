#ifndef _HAL_KEYDIG_LIB_H_
#define _HAL_KEYDIG_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif

#define KDIG_CH_MAX     3

enum kdig_ch_e {
    KDIG_CH0 = 0,
    KDIG_CH1,
    KDIG_CH2,
};

enum kdig_clk_div_e {
    KDIG_CLOCKDIV_1 = 0,
    KDIG_CLOCKDIV_2,
    KDIG_CLOCKDIV_4,
    KDIG_CLOCKDIV_8,
    KDIG_CLOCKDIV_16,
    KDIG_CLOCKDIV_32,
    KDIG_CLOCKDIV_64,
    KDIG_CLOCKDIV_128,
};

enum kdig_down_lvl_e {
    KDIG_DOWN_LVL_LOW = 0,
    KDIG_DOWN_LVL_HIGH,
};

enum kdig_mc_mode_e {
    KDIG_MC_MODE_NORMAL = 0,     //to shoot multi-click,key status must be in release status
    KDIG_MC_MODE_ABNORMAL,       //to shoot multi-click,key status is not care
};

enum kdig_irq_sta_e {
    KDIG_DOWN_IRQ_STA      = (0x1<<0),
    KDIG_UP_IRQ_STA        = (0x1<<4),
    KDIG_HOLD_IRQ_STA      = (0x1<<8),
    KDIG_LONG_IRQ_STA      = (0x1<<12),
    KDIG_MC_IRQ_STA        = (0x1<<16),
};

enum kdig_irq_en_e {
    KDIG_DOWN_IRQ_EN      = (0x1<<0),
    KDIG_UP_IRQ_EN        = (0x1<<4),
    KDIG_HOLD_IRQ_EN      = (0x1<<8),
    KDIG_LONG_IRQ_EN      = (0x1<<12),
    KDIG_MC_IRQ_EN        = (0x1<<16),
};

enum kdig_wakeup_sta_e {
    KDIG_DOWN_WKUP_STA      = (0x1<<0),
    KDIG_UP_WKUP_STA        = (0x1<<4),
    KDIG_HOLD_WKUP_STA      = (0x1<<8),
    KDIG_LONG_WKUP_STA      = (0x1<<12),
    KDIG_MC_WKUP_STA        = (0x1<<16),
};

enum kdig_wakeup_en_e {
    KDIG_DOWN_WKUP_EN      = (0x1<<0),
    KDIG_UP_WKUP_EN        = (0x1<<4),
    KDIG_HOLD_WKUP_EN      = (0x1<<8),
    KDIG_LONG_WKUP_EN      = (0x1<<12),
    KDIG_MC_WKUP_EN        = (0x1<<16),
};

typedef struct {
    bool press_en;
    bool multi_click_en;
    bool down_level;
    bool multi_click_mode;
    uint32_t irq;
} kdig_ch_param_t;

typedef struct key_dig_sta {
    bool dat;
    uint8_t mc_num;
} key_dig_sta_t;

typedef struct {
    uint8_t clk_div;          //32k div
    uint16_t hold_cycle;      //max 0x1000(4096)
    uint8_t down_debounce;
    uint8_t up_debounce;

    uint16_t long_cycle;      //max 0x1000(4096)

    uint16_t release_cycle;   //max 0x1000(4096)
    uint32_t monitor_cycle;   //max 0x10000(65536)
} kdig_param_t;

typedef void (*kdig_irq_callback)(uint32_t pending, key_dig_sta_t *kdata);

void HAL_kdig_module_init(kdig_param_t *dkc_param);
void HAL_kdig_module_ch_config(uint8_t channel, kdig_ch_param_t *ch_param);
void HAL_kdig_irq_en(kdig_irq_callback kdig_cb);
void HAL_kdig_wakeup_set(uint8_t channel, uint32_t wakeup_tpye, bool enable);


#ifdef __cplusplus
}
#endif

#endif

