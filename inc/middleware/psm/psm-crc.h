#ifndef __PSM_CRC_H__
#define __PSM_CRC_H__

#ifdef __linux__
#include <stdint.h>
#else
#include "sys_inc.h"
#endif

uint32_t soft_crc32(const void* data__, int data_size, uint32_t crc);

#endif

