#ifndef _APP_SPI_H_
#define _APP_SPI_H_

//SPI0
#if (SPI0_CS_PIN_SEL == 0)
#define SPI0_CS_PIN_CFG     (PIN_B7 | PIN_FUNC3 | PIN_DRI1 | PIN_PULL_UP100K)
#endif

#if (SPI0_CLK_PIN_SEL == 0)
#define SPI0_CLK_PIN_CFG    (PIN_B0 | PIN_FUNC5 | PIN_DRI1)
#endif

#if (SPI0_MOSI_PIN_SEL == 0)
#define SPI0_MOSI_PIN_CFG   (PIN_B1 | PIN_FUNC5 | PIN_DRI1)
#endif

#if (SPI0_MISO_PIN_SEL == 0)
#define SPI0_MISO_PIN_CFG   (PIN_B2 | PIN_FUNC5 | PIN_DRI1)
#endif


//SPI1
#if SPI1_MODE_SEL
//使用SPI1硬件模块
#if (SPI1_CS_PIN_SEL == 0)
#define SPI1_CS_PIN_CFG     (PIN_A15 | PIN_FUNC2)      //fpga
#define SPI1_CS_PIN         (PIN_A15)
#define SPI1_CS_FUNC        (PIN_FUNC2)
#elif (SPI1_CS_PIN_SEL == 1)
#define SPI1_CS_PIN_CFG     (PIN_B2 | PIN_FUNC6)
#define SPI1_CS_PIN         (PIN_B2)
#define SPI1_CS_FUNC        (PIN_FUNC6)
#elif (SPI1_CS_PIN_SEL == 2)
#define SPI1_CS_PIN_CFG     (PIN_B6 | PIN_FUNC6)
#define SPI1_CS_PIN         (PIN_B6)
#define SPI1_CS_FUNC        (PIN_FUNC6)
#elif (SPI1_CS_PIN_SEL == 3)
#define SPI1_CS_PIN_CFG     (PIN_B13 | PIN_FUNC6)
#define SPI1_CS_PIN         (PIN_B13)
#define SPI1_CS_FUNC        (PIN_FUNC6)
#elif (SPI1_CS_PIN_SEL == 4)
#define SPI1_CS_PIN_CFG     (PIN_B14 | PIN_FUNC6)
#define SPI1_CS_PIN         (PIN_B14)
#define SPI1_CS_FUNC        (PIN_FUNC6)
#elif (SPI1_CS_PIN_SEL == 5)
#define SPI1_CS_PIN_CFG     (PIN_B16 | PIN_FUNC5)
#define SPI1_CS_PIN         (PIN_B16)
#define SPI1_CS_FUNC        (PIN_FUNC5)
#elif (SPI1_CS_PIN_SEL == 6)
#define SPI1_CS_PIN_CFG     (PIN_B19 | PIN_FUNC10)
#define SPI1_CS_PIN         (PIN_B19)
#define SPI1_CS_FUNC        (PIN_FUNC10)
#elif (SPI1_CS_PIN_SEL == 7)
#define SPI1_CS_PIN_CFG     (PIN_B25 | PIN_FUNC5)
#define SPI1_CS_PIN         (PIN_B25)
#define SPI1_CS_FUNC        (PIN_FUNC5)
#elif (SPI1_CS_PIN_SEL == 8)
#define SPI1_CS_PIN_CFG     (PIN_D1 | PIN_FUNC2)
#define SPI1_CS_PIN         (PIN_D1)
#define SPI1_CS_FUNC        (PIN_FUNC2)
#elif (SPI1_CS_PIN_SEL == 9)
#define SPI1_CS_PIN_CFG     (PIN_D2 | PIN_FUNC2)
#define SPI1_CS_PIN         (PIN_D2)
#define SPI1_CS_FUNC        (PIN_FUNC2)
#elif (SPI1_CS_PIN_SEL == 10)
#define SPI1_CS_PIN_CFG     (PIN_D7 | PIN_FUNC2)
#define SPI1_CS_PIN         (PIN_D7)
#define SPI1_CS_FUNC        (PIN_FUNC2)
#elif (SPI1_CS_PIN_SEL == 11)
#define SPI1_CS_PIN_CFG     (PIN_D9 | PIN_FUNC8)
#define SPI1_CS_PIN         (PIN_D9)
#define SPI1_CS_FUNC        (PIN_FUNC8)
#endif

#if (SPI1_CLK_PIN_SEL == 0)
#define SPI1_CLK_PIN_CFG    (PIN_A14 | PIN_FUNC2)      //fpga
#elif (SPI1_CLK_PIN_SEL == 1)
#define SPI1_CLK_PIN_CFG    (PIN_A4 | PIN_FUNC9)
#elif (SPI1_CLK_PIN_SEL == 2)
#define SPI1_CLK_PIN_CFG    (PIN_B1 | PIN_FUNC6)
#elif (SPI1_CLK_PIN_SEL == 3)
#define SPI1_CLK_PIN_CFG    (PIN_B5 | PIN_FUNC6)
#elif (SPI1_CLK_PIN_SEL == 4)
#define SPI1_CLK_PIN_CFG    (PIN_B10 | PIN_FUNC6)
#elif (SPI1_CLK_PIN_SEL == 5)
#define SPI1_CLK_PIN_CFG    (PIN_B18 | PIN_FUNC10)
#elif (SPI1_CLK_PIN_SEL == 6)
#define SPI1_CLK_PIN_CFG    (PIN_B27 | PIN_FUNC5)
#elif (SPI1_CLK_PIN_SEL == 7)
#define SPI1_CLK_PIN_CFG    (PIN_D5 | PIN_FUNC2)
#elif (SPI1_CLK_PIN_SEL == 8)
#define SPI1_CLK_PIN_CFG    (PIN_D8 | PIN_FUNC8)
#elif (SPI1_CLK_PIN_SEL == 9)
#define SPI1_CLK_PIN_CFG    (PIN_D12 | PIN_FUNC8)
#endif

#if (SPI1_MOSI_PIN_SEL == 0)
#define SPI1_MOSI_PIN_CFG   (PIN_A16 | PIN_FUNC2 | PIN_PULL_UP100K)       //fpgas
#elif (SPI1_MOSI_PIN_SEL == 1)
#define SPI1_MOSI_PIN_CFG   (PIN_A5 | PIN_FUNC9 | PIN_PULL_UP100K)
#elif (SPI1_MOSI_PIN_SEL == 2)
#define SPI1_MOSI_PIN_CFG   (PIN_B0 | PIN_FUNC6 | PIN_PULL_UP100K)
#elif (SPI1_MOSI_PIN_SEL == 3)
#define SPI1_MOSI_PIN_CFG   (PIN_B3 | PIN_FUNC8 | PIN_PULL_UP100K)
#elif (SPI1_MOSI_PIN_SEL == 4)
#define SPI1_MOSI_PIN_CFG   (PIN_B4 | PIN_FUNC6 | PIN_PULL_UP100K)
#elif (SPI1_MOSI_PIN_SEL == 5)
#define SPI1_MOSI_PIN_CFG   (PIN_B11 | PIN_FUNC6 | PIN_PULL_UP100K)
#elif (SPI1_MOSI_PIN_SEL == 6)
#define SPI1_MOSI_PIN_CFG   (PIN_B12 | PIN_FUNC9 | PIN_PULL_UP100K)
#elif (SPI1_MOSI_PIN_SEL == 7)
#define SPI1_MOSI_PIN_CFG   (PIN_B17 | PIN_FUNC10 | PIN_PULL_UP100K)
#elif (SPI1_MOSI_PIN_SEL == 8)
#define SPI1_MOSI_PIN_CFG   (PIN_B26 | PIN_FUNC5 | PIN_PULL_UP100K)
#elif (SPI1_MOSI_PIN_SEL == 9)
#define SPI1_MOSI_PIN_CFG   (PIN_D4 | PIN_FUNC2 | PIN_PULL_UP100K)
#elif (SPI1_MOSI_PIN_SEL == 10)
#define SPI1_MOSI_PIN_CFG   (PIN_D10 | PIN_FUNC8 | PIN_PULL_UP100K)
#endif

#if (SPI1_MISO_PIN_SEL == 0)
#define SPI1_MISO_PIN_CFG   (PIN_A17 | PIN_FUNC2 | PIN_PULL_UP100K)
#elif (SPI1_MISO_PIN_SEL == 1)
#define SPI1_MISO_PIN_CFG   (PIN_B3 | PIN_FUNC6 | PIN_PULL_UP100K)
#elif (SPI1_MISO_PIN_SEL == 2)
#define SPI1_MISO_PIN_CFG   (PIN_B10 | PIN_FUNC12 | PIN_PULL_UP100K)
#elif (SPI1_MISO_PIN_SEL == 3)
#define SPI1_MISO_PIN_CFG   (PIN_B12 | PIN_FUNC6 | PIN_PULL_UP100K)
#elif (SPI1_MISO_PIN_SEL == 4)
#define SPI1_MISO_PIN_CFG   (PIN_B16 | PIN_FUNC10 | PIN_PULL_UP100K)
#elif (SPI1_MISO_PIN_SEL == 5)
#define SPI1_MISO_PIN_CFG   (PIN_B19 | PIN_FUNC5 | PIN_PULL_UP100K)
#elif (SPI1_MISO_PIN_SEL == 6)
#define SPI1_MISO_PIN_CFG   (PIN_B24 | PIN_FUNC6 | PIN_PULL_UP100K)
#elif (SPI1_MISO_PIN_SEL == 7)
#define SPI1_MISO_PIN_CFG   (PIN_B28 | PIN_FUNC5 | PIN_PULL_UP100K)
#elif (SPI1_MISO_PIN_SEL == 8)
#define SPI1_MISO_PIN_CFG   (PIN_D3 | PIN_FUNC2 | PIN_PULL_UP100K)
#elif (SPI1_MISO_PIN_SEL == 9)
#define SPI1_MISO_PIN_CFG   (PIN_D6 | PIN_FUNC2 | PIN_PULL_UP100K)
#elif (SPI1_MISO_PIN_SEL == 10)
#define SPI1_MISO_PIN_CFG   (PIN_D11 | PIN_FUNC8 | PIN_PULL_UP100K)
#endif

#define SPI1_CS_INIT()      HAL_gpio_func_set(SPI1_CS_PIN, PIN_FUNCOUTPUT)
#define SPI1_CS_DEINIT()    HAL_gpio_func_set(SPI1_CS_PIN, SPI1_CS_FUNC)
#define SPI1_CS_LOW()       HAL_gpio_value_set(SPI1_CS_PIN, 0)
#define SPI1_CS_HIGH()      HAL_gpio_value_set(SPI1_CS_PIN, 1)


#else
//使用GPIO软件模拟
#define SPI1_CS_PIN         (PIN_A15)
#define SPI1_CLK_PIN        (PIN_A14)
#define SPI1_MOSI_PIN       (PIN_A16)
#define SPI1_MISO_PIN       (PIN_A17)

#define SPI1_CS_INIT()      HAL_gpio_config(SPI1_CS_PIN | PIN_FUNCOUTPUT | PIN_DRI1)
#define SPI1_CLK_INIT()     HAL_gpio_config(SPI1_CLK_PIN | PIN_FUNCOUTPUT | PIN_DRI1)
#define SPI1_MOSI_INIT()    HAL_gpio_config(SPI1_MOSI_PIN | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_PULL_UP2K)
#define SPI1_MISO_INIT()    HAL_gpio_config(SPI1_MISO_PIN | PIN_FUNCINPUT | PIN_DRI1 | PIN_PULL_UP2K)

#define SPI1_CS_LOW()       HAL_gpio_value_set(SPI1_CS_PIN, 0)
#define SPI1_CS_HIGH()      HAL_gpio_value_set(SPI1_CS_PIN, 1)

#define SPI1_CLK_LOW()      HAL_gpio_value_set(SPI1_CLK_PIN, 0)
#define SPI1_CLK_HIGH()     HAL_gpio_value_set(SPI1_CLK_PIN, 1)

#define SPI1_MOSI_LOW()     HAL_gpio_value_set(SPI1_MOSI_PIN, 0)
#define SPI1_MOSI_HIGH()    HAL_gpio_value_set(SPI1_MOSI_PIN, 1)

#if SPI1_DIRECT_SEL
//3线
#define SPI1_DAT_WR()       HAL_gpio_func_set(SPI1_MOSI_PIN, PIN_FUNCOUTPUT)
#define SPI1_DAT_RD()       HAL_gpio_func_set(SPI1_MOSI_PIN, PIN_FUNCINPUT)
#define SPI1_DAT_VAL()      HAL_gpio_value_get(SPI1_MOSI_PIN)
#else
//4线
#define SPI1_DAT_VAL()      HAL_gpio_value_get(SPI1_MISO_PIN)
#endif

#endif

void app_spi_init(void);
void spi_flash_erase(uint32_t flash_addr, uint32_t chip_erase, uint32_t wait_busy);
void spi_flash_write(uint8_t *ram_addr, uint32_t flash_addr, uint32_t bytes, uint32_t wait_busy);
void spi_flash_read(uint8_t *ram_addr, uint32_t flash_addr, uint32_t bytes);
void spi_flash_waitbusy(void);
void spi_flash_dma_write(uint8_t *wr_buf, uint32_t wr_addr, uint16_t wr_size);      //wr_size max 4096
void spi_flash_dma_read(uint8_t *rd_buf, uint32_t rd_addr, uint16_t rd_size);       //rd_size max 4096
void spi_dma_rx_callback(void);
void spi_dma_tx_callback(void);
void spi_test(void);



#endif

