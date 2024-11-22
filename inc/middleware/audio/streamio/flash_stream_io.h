

#ifndef _FLASH_STREAM_IO_H_
#define _FLASH_STREAM_IO_H_

#include "sys_inc.h"
#include "sltk_sys.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void (*write)(uint8_t *ram_addr, uint32_t flash_addr, uint32_t bytes, uint32_t wait_busy);
    void (*read)(uint8_t *ram_addr, uint32_t flash_addr, uint32_t bytes);
    void (*dma_write)(uint8_t *wr_buf, uint32_t wr_addr, uint16_t wr_size);
    void (*dma_read)(uint8_t *rd_buf, uint32_t rd_addr, uint16_t rd_size);
} spi_flash_stream_api_t;

void spi_flash_stream_api_init(spi_flash_stream_api_t *api);

void* create_tone_stream(void);

streamio_t* create_stream(streamtype_t type);
void destroy_stream(streamio_t *io);


#ifdef __cplusplus
}
#endif

#endif
