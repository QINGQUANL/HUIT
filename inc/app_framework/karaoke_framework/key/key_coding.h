#ifndef _KEY_CODING_H_
#define _KEY_CODING_H_


enum kcd_direction_e {
    DIRECTION_INIT          = 0,
    DIRECTION_CLOCKWISE,
    DIRECTION_ANTICLOCKWISE,
};

enum kcd_status_e {
    STATUS_NONE             = 0,
    STATUS_CLOCKWISE,
    STATUS_ANTICLOCKWISE,
};


typedef struct {
    uint8_t volatile cur_direction;
    uint8_t volatile last_direction;
    uint8_t volatile status;
    uint8_t volatile step_delay;
    uint8_t volatile switch_delay;
    uint32_t key_a_pin;
    uint32_t key_b_pin;
    uint32_t key_a_cfg;
    uint32_t key_b_cfg;
} kcd_t;




void kcd_init(uint32_t a_pin, uint32_t a_pin_cfg, uint32_t b_pin, uint32_t b_pin_cfg);
void kcd_get_sta(void);
void kcd_delay(void);
uint32_t kcd_get_key(void);

#endif

