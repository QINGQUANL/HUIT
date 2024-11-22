#ifndef _KEY_ADC_H_
#define _KEY_ADC_H_

#define KNOB_VOL_MAX       12


typedef struct {
    uint32_t val;
    uint32_t cnt;
    uint32_t long_sta;
} kadc_t;

typedef struct {
    uint32_t knob_val;
    uint8_t knob_step;
    uint8_t knob_vol_max;
    uint32_t knob_diff;
    uint16_t knob_tbl[KNOB_VOL_MAX];
} knob_ctl_t;

typedef struct {
    void (*knob_adc_cb)(uint8_t ch, uint32_t kval, uint32_t kstep);
} knob_cb_t;

typedef struct {
    bool adc_en;
    bool irq_mode_en;
    bool knob_en;
} kadc_ctl_t;


typedef struct {
    uint32_t knob_val;
    uint8_t knob_step;
} kadc_knob_tab;


typedef struct {
    uint32_t adc_val;
    uint32_t key_event;
} kadc_event_tab;


typedef struct {
    uint32_t table_max;
    kadc_event_tab *event_table;
    uint32_t *kadc_val;
}key_event_tab_ctl;


#define SHARE1_VOL_MAX      5


#define SHARE1_DIFF         1000
#define KADC_CH_SHARE1      11




void kadc_ccu_init(void);
void kadc_gpio_init(void);
void kadc_wakeup_init(void);
void kadc_init(const uint32_t (*kadc_init_config)[3],kadc_ctl_t *ad_ctl_tab, bool kadc_irq_en,uint32_t *knob_threshod);
uint32_t kadc_get_key(uint32_t ch, uint32_t data);
uint32_t kadc_scan_get_key(void);
void kadc_share1_init(void);
void kadc_share1(uint32_t data);
void knob_vol_init(knob_ctl_t *tab_t, knob_cb_t cb);
void key_event_table_init(uint8_t ch, kadc_event_tab *table, uint32_t table_max, uint32_t *kadc_val);//当前kadc值会记录在kadc_val中


#endif

