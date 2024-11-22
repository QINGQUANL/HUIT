#ifndef _HAL_TWI_LIB_H_
#define _HAL_TWI_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "compiler.h"

#define TWI1_ADDR_MODE          TWI_ADDR_MODE_7BIT
#define TWI2_ADDR_MODE          TWI_ADDR_MODE_7BIT

enum twi_id_e {
    TWI_ID_01 = 0,
    TWI_ID_02
};


void HAL_twi_module_init(uint32_t id, uint32_t fre);
void HAL_twi_module_deinit(uint32_t id);
void HAL_twi_module_clk_set(uint32_t id, uint32_t fre);
int HAL_twi_module_write(uint32_t id, uint32_t addr, const uint8_t *data, uint32_t bytes,uint8_t need_stop);
int HAL_twi_module_read(uint32_t id, uint32_t addr, uint8_t *data, uint32_t bytes);
int HAL_twi_ack_check(uint32_t id, uint32_t addr);
void HAL_ll_twi_module_init(uint32_t id, uint32_t fre);

#ifdef __cplusplus
}
#endif

#endif
