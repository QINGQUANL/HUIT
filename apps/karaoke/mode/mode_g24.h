#ifndef _MODE_G24_H_
#define _MODE_G24_H_
#include "user_config.h"

#define ALL_PAIR    0xA0
#define ALL_PAIR_2  0x9F

enum 
{
    e_G24_MIC_PROPERTY_UNKOWN = 0,
    e_G24_MASTER_MIC_CONNECTED = 1, 
    e_G24_MASTER_DETACH = 2,
};

enum {
    e_G24_MIC_UNKOWN = 0, //未知
    e_G24_MIC_MASTER = 1, //主MIC
    e_G24_MIC_VICE = 2,   //副MIC
};

typedef struct {
    volatile uint8_t status;
    uint8_t scene;
    bool connect[2];
    bool del_linkkey_req[2];
} g24_mode_t;

bool is_in_mode_g24(void);
bool is_mode_g24_exit(void);
void g24_addr_default_get(uint8_t *addr);
g24_mode_t *g24_mode_param_get(void);
void g24_link_info_save(uint8_t link);
//void g24_2T4_master_slaver_sync_interrupt(void);
void g24_2T4_1TN_slaver_sync_interrupt(void);

void app_g24_monitor_init(void);
void g24_monitor_param_init();
void g24_rf_shutdown();
void g24_rf_turnon();
void g24_set_not_match_msg_processed(bool bset);
bool __mode__common();

void g24_handset_delete_link_process(void);

extern void mic_fdnrev_refresh(void);
extern bool mic_fdnrev_enable_set(uint8_t enable);
void g24_Channel_select_code(uint8_t channel);

uint32_t g24_allpair_receive(void);
void g24_allpair_send(uint32_t broadcastcode);
uint32_t g24_allpair_receive(void);
void g24_allpair_send(uint32_t broadcastcode);
void g24_Channel_select(uint8_t channel);
uint32_t g24_get_channel_ID(void);

#endif
