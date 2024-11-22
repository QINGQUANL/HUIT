#ifndef _SD_H_
#define _SD_H_

#include "diskio.h"

typedef struct {
    uint8_t sta;
    uint8_t out_cnt;
    uint8_t in_cnt;
} sd_det_t;
extern sd_det_t sd_det;

#define SD_IN_DET_CNT       10  //20ms * 10 = 200ms
#define SD_OUT_DET_CNT      3   //20ms *3 = 60ms

#define SD_CLK_PULL         (PIN_PULL_DISABLE)
#define SD_WKUP_FUNC        (PIN_FUNC14)
#define SD_DETECT_FUNC      (PIN_FUNCINPUT)
#define SD_DETECT_PULL      (PIN_PULL_UP100K)

typedef struct {
    bool sd_en;
    bool sd_detect_en;
    bool sd_pin_share;
    uint32_t clk_pin;
    uint32_t clk_func;
    uint8_t  sd_clkdiv;
    uint32_t sd_detect_pin;
    bool   sd_4wire_en;
    uint32_t sd_d0_pin_cfg;
    uint32_t sd_d1_pin_cfg;
    uint32_t sd_d2_pin_cfg;
    uint32_t sd_d3_pin_cfg;
    uint32_t sd_cmd_cfg;
    uint32_t sd_clk_cfg;
} sd_info_ctl;


enum sd_detach_sta_e {
    SD_DETACH_DISABLE = 0x0,
    SD_DETACH_ENABLE,
    SD_DETACH_WAKEUP,
};


void sd_init(sd_info_ctl *sd_info_ctl);
void sd_pin_init(void);
void sd_clk_init(void);
void sd_clk_deinit(void);
void sd_detect_set(uint8_t sta);
void sd_detect(void);
DRESULT sd_get_status(void);
DRESULT sd_failcb(void);
void sd_set_wakeup(bool enable);
void sd_irq_process(void);
uint32_t sd_clkdiv_set(void);
void sd_hal_cb(uint8_t cmd, uint32_t *data);
void* get_sd_info_ctl(void);
BOOL sd_get_detect_gpio_status();

#endif

