#ifndef _I2S_SLAVER_REC_H_
#define _I2S_SLAVER_REC_H_

#include "sys_inc.h"

void i2s_slaver_rec_store_data(void *handle, void *buf, uint32_t samples);
uint8_t i2s_slaver_rec_sample_size_get(void *handle);
void *i2s_slaver_rec_create(void);

#endif