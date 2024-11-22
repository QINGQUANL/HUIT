#ifndef _KEY_IR_H_
#define _KEY_IR_H_


/*
* if IR_DATA_SHIFT is defined as 0, the format of usercode/datacode is "b0:b1:b2:...:b13:b14:b15",
* if IR_DATA_SHIFT is defined as 1, the format of usercode/datacode is "b15:b14:b13:...:b2:b1:b0".
*/



#define KEY_IR_USER_NONE    0x0000



typedef struct {
    uint8_t volatile cnt;
    uint8_t volatile data_code;
    uint16_t volatile user_code;
    bool  is_data_shift;
    uint8_t key_num_max;
    const uint8_t (*key_table)[2];
} kir_t;

void kir_init(bool ir_data_shift, uint8_t key_max,const uint8_t (*key_table)[2]);
uint8_t kir_get_key(void);
void kir_get_data(void);
uint8_t kir_shift_8bit(uint8_t data);
uint16_t kir_shift_16bit(uint16_t data);

#endif

