#ifndef _HAL_SPI_LIB_H_
#define _HAL_SPI_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "compiler.h"

typedef struct {
    bool rx_en;
    bool tx_en;
    void (*rx_callback)();
    void (*tx_callback)();
} spi_dma_t;

typedef struct {
    uint16_t datasize;      //data size
    uint8_t casize;         //cmd addr size
    uint8_t rxphase;        //RX phase select
    uint8_t cs;             //CS Mode low/high
    uint8_t ms;             //master/slave mode
    uint8_t clkdiv;         //clock divider
    uint8_t direct;         //direct mode
    uint8_t first;          //MSB/LSB first
    uint8_t cpha;           //phase 0/1
    uint8_t cpol;           //polarity high/low
    bool dual_rw;           //Dual Data Line
    spi_dma_t dma_param;
} spi_param_t;


enum spi_polarity_e {
    SPI_POLARITY_LOW = 0x0,
    SPI_POLARITY_HIGH,
};

enum spi_phase_e {
    SPI_PHASE_0 = 0x0,
    SPI_PHASE_1,
};

enum spi_first_e {
    SPI_FIRST_MSB = 0x0,
    SPI_FIRST_LSB,
};

enum spi_direct_e {
    SPI_2LINE_UNIDIRECTIONAL = 0x0,
    SPI_1LINE_BIDIRECTIONAL,
};

enum spi_casize_e {
    SPI_CASIZE_0BYTE        = 0,
    SPI_CASIZE_1BYTE        = 8,
    SPI_CASIZE_2BYTE        = 16,
    SPI_CASIZE_3BYTE        = 24,
    SPI_CASIZE_4BYTE        = 32,
    SPI_CASIZE_5BYTE        = 40,
    SPI_CASIZE_6BYTE        = 48,
    SPI_CASIZE_7BYTE        = 56,
    SPI_CASIZE_8BYTE        = 64,
};

enum spi_ms_mode_e {
    SPI_MS_MODE_M = 0x0,
    SPI_MS_MODE_S,
};

enum spi_cs_mode_e {
    SPI_CS_MODE_LOW = 0x0,
    SPI_CS_MODE_HIGH,
};

enum spi_rx_phase_e {
    SPI_RX_PHASE_0 = 0x0,
    SPI_RX_PHASE_1,
};

void HAL_spi_module_init(spi_param_t *param);
void HAL_spi_module_transfer_config(uint8_t ca_size, uint16_t data_size, bool dual_rw);
void HAL_spi_module_write(uint32_t data);
uint32_t HAL_spi_module_read(void);
void HAL_spi_module_write_with_ca(uint32_t ca_r0, uint32_t ca_r1, uint32_t data);
uint32_t HAL_spi_module_read_with_ca(uint32_t ca_r0, uint32_t ca_r1);
void HAL_spi_module_write_start_with_ca(uint32_t ca_r0, uint32_t ca_r1, bool wait_complete);
void HAL_spi_module_read_start_with_ca(uint32_t ca_r0, uint32_t ca_r1, bool wait_complete);
void HAL_spi_module_write_data(uint32_t data);
uint32_t HAL_spi_module_read_data(void);
void HAL_spi_module_transfer_complete_wait(void);
//spi dma interface
bool HAL_is_spi_tx_dma_transing(void);
bool HAL_is_spi_rx_dma_transing(void);
void HAL_spi_module_dma_tx_init(void (*full_callback)(void));
void HAL_spi_module_dma_rx_init(void (*full_callback)(void));
void HAL_spi_dma_read(uint32_t ca_r0, uint32_t ca_r1, uint8_t cmd_len, uint8_t *data_buf, uint16_t data_len);
void HAL_spi_dma_write(uint32_t ca_r0, uint32_t ca_r1, uint8_t cmd_len, uint8_t *data_buf, uint16_t data_len);



#ifdef __cplusplus
}
#endif

#endif
