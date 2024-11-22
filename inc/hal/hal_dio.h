#ifndef _DIO_H_
#define _DIO_H_

#ifdef __cplusplus
 extern "C" {
#endif


#define DIO_CTRL_DAT_REG_E    0x40000108

enum dio_id_e {
    DIO_ID0                     = (0),      //dio0
    DIO_ID1                     = (16),     //dio1
};

enum dio_func_e {
    DIO_FUNC_DISABLE            = (0x0),    //disable
    DIO_FUNC_OUTPUT             = (0x1<<0), //output
    DIO_FUNC_INPUT              = (0x1<<1), //input
    DIO_FUNC_LED                = (0x1<<10),//LED0/LED1 mapping
    DIO_FUNC_MASK               = (DIO_FUNC_OUTPUT | DIO_FUNC_INPUT | DIO_FUNC_LED),
};

enum dio_value_e {
    DIO_VALUE_L                 = (0x0<<8), //output "0"
    DIO_VALUE_H                 = (0x1<<8),
    DIO_VALUE_OUTPUT_MASK       = (0x1<<8),
    DIO_VALUE_INPUT_MASK        = (0x1<<9),
};

enum dio_driving_e {
    DIO_DRI0                    = (0x0<<2), //450ohm
    DIO_DRI1                    = (0x1<<2), //140ohm
    DIO_DRI2                    = (0x2<<2), //80ohm
    DIO_DRI3                    = (0x3<<2), //60ohm
    DIO_DRI_MASK                = (0x3<<2),
};

enum dio_pull_e {
    DIO_PULL_RES_DISABLE        = (0x0<<4), //disable
    DIO_PULL_RES_DOWN           = (0x1<<4), //down
    DIO_PULL_RES_UP             = (0x2<<4), //up
    DIO_PULL_RES_UP_DOWN        = (0x3<<4), //up and down
    DIO_PULL_RES_MASK           = (0x3<<4),

    DIO_PULL_VAL_UP2K_DOWN2K    = (0x0<<6),
    DIO_PULL_VAL_UP2K_DOWN100K  = (0x1<<6),
    DIO_PULL_VAL_UP100K_DOWN2K  = (0x2<<6),
    DIO_PULL_VAL_UP100K_DOWN100K = (0x3<<6),
    DIO_PULL_VAL_MASK           = (0x3<<6),

    DIO_PULL_DISABLE            = (DIO_PULL_RES_DISABLE),
    DIO_PULL_UP_2K              = (DIO_PULL_RES_UP | DIO_PULL_VAL_UP2K_DOWN2K),
    DIO_PULL_UP_100K            = (DIO_PULL_RES_UP | DIO_PULL_VAL_UP100K_DOWN2K),
    DIO_PULL_DOWN_2K            = (DIO_PULL_RES_DOWN | DIO_PULL_VAL_UP2K_DOWN2K),
    DIO_PULL_DOWN_100K          = (DIO_PULL_RES_DOWN | DIO_PULL_VAL_UP2K_DOWN100K),
    DIO_PULL_UP_2K_DOWN_2K      = (DIO_PULL_RES_UP_DOWN | DIO_PULL_VAL_UP2K_DOWN2K),
    DIO_PULL_UP_2K_DOWN_100K    = (DIO_PULL_RES_UP_DOWN | DIO_PULL_VAL_UP2K_DOWN100K),
    DIO_PULL_UP_100K_DOWN_2K    = (DIO_PULL_RES_UP_DOWN | DIO_PULL_VAL_UP100K_DOWN2K),
    DIO_PULL_UP_100K_DOWN_100K  = (DIO_PULL_RES_UP_DOWN | DIO_PULL_VAL_UP100K_DOWN100K),
    DIO_PULL_MASK               = (DIO_PULL_RES_MASK | DIO_PULL_VAL_MASK),
};

void HAL_dio_func_set(uint32_t id, uint32_t func);
void HAL_dio_driving_set(uint32_t id, uint32_t driving);
void HAL_dio_pull_set(uint32_t id, uint32_t pull);
void HAL_dio_value_set(uint32_t id, uint32_t value);
uint32_t HAL_dio_value_get(uint32_t id);

#ifdef __cplusplus
}
#endif


#endif

