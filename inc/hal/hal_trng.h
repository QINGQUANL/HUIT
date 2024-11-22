#ifndef _TRNG_H_
#define _TRNG_H_


void HAL_trng_module_init(void);
void HAL_trng_module_deinit(void);
uint32_t HAL_trng_get_data(void);


#endif

