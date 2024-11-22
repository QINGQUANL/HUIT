#ifndef _SPI_H_
#define _SPI_H_
#include "hal.h"

void spi_dma_mode_init(spi_dma_t *dma_param);
uint32_t spi_flash_cmd_addr_combine(uint32_t addr, uint32_t cmd);

#endif

