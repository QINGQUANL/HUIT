#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "hal_clk.h"
#include "hal_gpio.h"
#include "hal_timestamp.h"
#include "hal_norflash.h"
#include "hal_spi.h"
#include "spi.h"
#include "app_spi.h"
#include "flash_stream_io.h"

#if (SPI0_EN || SPI1_EN)


volatile uint8_t spi_lock;

void app_spi_init(void)
{
    __maybe_unused spi_param_t spi_param;
    spi_flash_stream_api_t spi_flash_stream_api;
#if SPI0_EN
    HAL_gpio_config(SPI0_CS_PIN_CFG);
    HAL_gpio_config(SPI0_CLK_PIN_CFG);
    HAL_gpio_config(SPI0_MOSI_PIN_CFG);
    HAL_gpio_config(SPI0_MISO_PIN_CFG);
    HAL_norflash_chip1_mapping(0x1);             //MISO select PB2
#endif

#if SPI1_EN
#if SPI1_MODE_SEL
    SPI1_CS_HIGH();
    SPI1_CS_INIT();
    HAL_gpio_config(SPI1_CLK_PIN_CFG);
    HAL_gpio_config(SPI1_MOSI_PIN_CFG);
#if (SPI1_DIRECT_SEL == 0)
    HAL_gpio_config(SPI1_MISO_PIN_CFG);
#endif

    spi_param.datasize = 1;                 //N byte
    spi_param.casize = SPI_CASIZE_0BYTE;
    spi_param.dual_rw = DISABLE;
    spi_param.rxphase = SPI_RX_PHASE_0;
    spi_param.cs = SPI_CS_MODE_LOW;
    spi_param.ms = SPI_MS_MODE_M;
#if SPI1_DIRECT_SEL
    spi_param.clkdiv = 8;
    spi_param.direct = SPI_1LINE_BIDIRECTIONAL;
#else
    spi_param.clkdiv = 4;
    spi_param.direct = SPI_2LINE_UNIDIRECTIONAL;
#endif
    spi_param.first = SPI_FIRST_MSB;
    spi_param.cpha = SPI_PHASE_0;
    spi_param.cpol = SPI_POLARITY_LOW;

    spi_param.dma_param.rx_en = DISABLE;
    spi_param.dma_param.tx_en = DISABLE;
    spi_param.dma_param.rx_callback = spi_dma_rx_callback;
    spi_param.dma_param.tx_callback = spi_dma_tx_callback;

    HAL_spi_module_init(&spi_param);
    spi_dma_mode_init(&spi_param.dma_param);
#else
    SPI1_CS_HIGH();
    SPI1_CLK_LOW();
    SPI1_MOSI_LOW();

    SPI1_CS_INIT();
    SPI1_CLK_INIT();
    SPI1_MOSI_INIT();
#if SPI1_DIRECT_SEL
    SPI1_DAT_RD();              //3线默认配置为读(输入)，在写之前需要配置为输出，写完后还原为输入
#else
    SPI1_MISO_INIT();
#endif
#endif
    spi_flash_stream_api.write     = spi_flash_write;
    spi_flash_stream_api.read      = spi_flash_read;
    spi_flash_stream_api.dma_write = spi_flash_dma_write;
    spi_flash_stream_api.dma_read  = spi_flash_dma_read;
    spi_flash_stream_api_init(&spi_flash_stream_api);
#endif
}

AT_SRAM(.spi)
void spi_delay(void)
{
    //udelay(1);
}

AT_SRAM(.spi)
void spi_write(uint32_t data)
{
#if SPI1_EN
#if SPI1_MODE_SEL
    HAL_spi_module_write(data);

#else
#if SPI1_DIRECT_SEL
    SPI1_DAT_WR();
#endif
    SPI1_CLK_LOW();
    for (uint8_t i=0;i<8;i++) {
        if (data & 0x80) {
            SPI1_MOSI_HIGH();
        } else {
            SPI1_MOSI_LOW();
        }
        spi_delay();
        SPI1_CLK_HIGH();
        spi_delay();
        SPI1_CLK_LOW();
        data <<= 1;
    }
#if SPI1_DIRECT_SEL
    SPI1_DAT_RD();              //默认配置为读(输入)
#endif
#endif
#endif
}

AT_SRAM(.spi)
uint8_t spi_read(void)
{
    uint8_t value;
    value = 0;

#if SPI1_EN
#if SPI1_MODE_SEL
    value = HAL_spi_module_read();

#else
    SPI1_CLK_LOW();
    spi_delay();
    for (uint8_t i=0;i<8;i++) {
        SPI1_CLK_HIGH();
        spi_delay();
        value <<= 1;
        if (SPI1_DAT_VAL()) {
            value |= 0x01;
        }
        SPI1_CLK_LOW();
        spi_delay();
    }
#endif
#endif

    return value;
}

/*
AT_SRAM(.spi)
int32_t spi_flash_is_busy(void)
{
    int32_t ret;

#if SPI0_EN
    norflash_chip_sel(FLASH_CHIP1);
    ret = HAL_norflash_waitbusy();
    norflash_chip_sel(FLASH_CHIP0);

#elif SPI1_EN
    //(0x05) read status register
    SPI1_CS_LOW();
    spi_write(0x05);
    ret = spi_read() & 0x01;
    SPI1_CS_HIGH();

#endif

    return ret;
}
*/

AT_SRAM(.spi)
void spi_flash_waitbusy(void)
{
#if SPI0_EN
    uint32_t ret;
    while(1) {
        HAL_mdelay(10);
        HAL_norflash_chip_sel(FLASH_CHIP1);
        ret = HAL_norflash_waitbusy();
        HAL_norflash_chip_sel(FLASH_CHIP0);
        if (ret == 0) {
            break;
        }
    }

#elif SPI1_EN
    uint8_t val;
    //(0x05) read status register
    SPI1_CS_LOW();
    spi_write(0x05);
    do {
        val = spi_read();
    } while(val & 0x01);
    SPI1_CS_HIGH();

#endif
}


AT_SRAM(.spi)
void spi_flash_erase(uint32_t flash_addr, uint32_t chip_erase, uint32_t wait_busy)
{
    int count = 500;
    while (spi_lock && count) {
        --count;
        HAL_msleep(1);
    }

    if (spi_lock) {
        loge("spi_lock err!!!");
    }

    spi_lock = 1;

#if SPI0_EN
    HAL_norflash_chip_sel(FLASH_CHIP1);
    HAL_norflash_erase(flash_addr, wait_busy);
    HAL_norflash_chip_sel(FLASH_CHIP0);

#elif SPI1_EN
    uint8_t val;

    //(0x06) write enable
    SPI1_CS_LOW();
    spi_write(0x06);
    SPI1_CS_HIGH();

    if (chip_erase) {
        //(0xC7) chip erase
        SPI1_CS_LOW();
        spi_write(0xC7);
        SPI1_CS_HIGH();
    } else {
        //(0x20) sector erase
        SPI1_CS_LOW();
        spi_write(0x20);
        spi_write((uint8_t)(flash_addr>>16));
        spi_write((uint8_t)(flash_addr>>8));
        spi_write((uint8_t)(flash_addr>>0));
        SPI1_CS_HIGH();
    }

    if (wait_busy) {
        //(0x05) read status register
        SPI1_CS_LOW();
        spi_write(0x05);
        do {
            val = spi_read();
        } while(val & 0x01);
        SPI1_CS_HIGH();
    }
#endif

    spi_lock = 0;
}

AT_SRAM(.spi)
void spi_flash_write(uint8_t *ram_addr, uint32_t flash_addr, uint32_t bytes, uint32_t wait_busy)
{
    int count = 500;
    while (spi_lock && count) {
        --count;
        HAL_msleep(1);
    }

    if (spi_lock) {
        loge("spi_lock err!!!");
    }

    spi_lock = 1;

#if SPI0_EN
    HAL_norflash_chip_sel(FLASH_CHIP1);
    HAL_norflash_write((uint32_t)ram_addr, flash_addr, bytes, 0, wait_busy);
    HAL_norflash_chip_sel(FLASH_CHIP0);

#elif SPI1_EN
    uint8_t val;

    //(0x06) write enable
    SPI1_CS_LOW();
    spi_write(0x06);
    SPI1_CS_HIGH();

    //(0x02) page program
    SPI1_CS_LOW();
    spi_write(0x02);
    spi_write((uint8_t)(flash_addr>>16));
    spi_write((uint8_t)(flash_addr>>8));
    spi_write((uint8_t)(flash_addr>>0));
    for (uint32_t i=0;i<bytes;i++) {
        spi_write((uint32_t)(*(ram_addr+i)));
    }
    SPI1_CS_HIGH();

    if (wait_busy) {
        //(0x05) read status register
        SPI1_CS_LOW();
        spi_write(0x05);
        do {
            val = spi_read();
        } while(val & 0x01);
        SPI1_CS_HIGH();
    }
#endif

    spi_lock = 0;
}

AT_SRAM(.spi)
void spi_flash_read(uint8_t *ram_addr, uint32_t flash_addr, uint32_t bytes)
{
    int count = 500;
    while (spi_lock && count) {
        --count;
        HAL_msleep(1);
    }


    if (spi_lock) {
        loge("spi_lock err!!!");
    }

    spi_lock = 1;

#if SPI0_EN
    HAL_norflash_read((uint32_t)ram_addr, FLASH_CHIP0_SIZE+flash_addr, bytes);

#elif SPI1_EN
    //(0x03) read data
    SPI1_CS_LOW();
    spi_write(0x03);
    spi_write((uint8_t)(flash_addr>>16));
    spi_write((uint8_t)(flash_addr>>8));
    spi_write((uint8_t)(flash_addr>>0));
    for (uint32_t i=0;i<bytes;i++) {
        *(ram_addr+i) = (uint8_t)spi_read();
    }
    SPI1_CS_HIGH();
#endif

    spi_lock= 0;
}

uint32_t get_spi_flash_size(void)
{
    return SPI_FLASH_SIZE;
}

AT_SRAM(.spi)
void spi_flash_dma_read(uint8_t *rd_buf, uint32_t rd_addr, uint16_t rd_size)
{
    uint32_t addr;
    addr = spi_flash_cmd_addr_combine(rd_addr,0x03);
    HAL_spi_dma_read(addr,0x00,SPI_CASIZE_4BYTE,rd_buf,rd_size);
    while(HAL_is_spi_rx_dma_transing());
}

AT_SRAM(.spi)
void spi_flash_dma_write(uint8_t *wr_buf, uint32_t wr_addr, uint16_t wr_size)
{
    uint32_t addr;
    addr = spi_flash_cmd_addr_combine(wr_addr,0x02);
    HAL_spi_dma_write(addr,0x00,SPI_CASIZE_4BYTE,wr_buf,wr_size);
    while(HAL_is_spi_tx_dma_transing());
}

AT_SRAM(.spi)
void spi_dma_rx_callback(void)
{
    //logi("spi dma rx cb");
}

AT_SRAM(.spi)
void spi_dma_tx_callback(void)
{
    //logi("spi dma tx cb");
}

#define SPI_TEST_EN     0

#if SPI_TEST_EN

__attribute__((aligned(4)))
uint8_t ttbuf[0x100];

AT_SRAM(.spi)
void spi_flash_write_en(void)
{
    SPI1_CS_LOW();
    HAL_spi_module_transfer_config(SPI_CASIZE_1BYTE,0,DISABLE);
    HAL_spi_module_write_start_with_ca(0x06,0x00,1);
    SPI1_CS_HIGH();
}

AT_SRAM(.spi)
void spi_flash_wait_busy(void)
{
    uint32_t flash_sta = 0x01;
    SPI1_CS_LOW();
    do {
        HAL_spi_module_transfer_config(SPI_CASIZE_1BYTE,1,DISABLE);
        HAL_spi_module_read_start_with_ca(0x05,0x00,1);
        flash_sta = HAL_spi_module_read_data();
    } while(flash_sta & 0x01);
    SPI1_CS_HIGH();
}

AT_SRAM(.spi)
void spi_flash_wait_WEL(void)
{
    uint32_t flash_sta = 0;
    SPI1_CS_LOW();
    do {
        HAL_spi_module_transfer_config(SPI_CASIZE_1BYTE,1,DISABLE);
        HAL_spi_module_read_start_with_ca(0x05,0x00,1);
        flash_sta = HAL_spi_module_read_data();
    } while((flash_sta & 0x02)==0);
    SPI1_CS_HIGH();
}

AT_SRAM(.spi)
void spi_flash_read_within_32bytes(uint8_t *rd_buf, uint32_t rd_addr, uint8_t byte_size)
{
    uint32_t addr;
    uint8_t size =  (byte_size+3)/4;
    uint8_t size_single = byte_size%4;
    uint32_t data_val = 0;

    if(byte_size>32 || byte_size==0) {
        loge("out of range");
        return;
    }
    addr = spi_flash_cmd_addr_combine(rd_addr,0x03);
    SPI1_CS_LOW();
    HAL_spi_module_transfer_config(SPI_CASIZE_4BYTE,byte_size,DISABLE);
    HAL_spi_module_read_start_with_ca(addr,0x00,1);
    for (uint8_t n=0;n<size;n++) {
        data_val = HAL_spi_module_read_data();
        if(byte_size>=(n+1)*4) {
            memcpy(&rd_buf[n*4],&data_val,4);
        } else {
            memcpy(&rd_buf[n*4],&data_val,size_single);
        }
        //logi("val = %x", data_val);
    }
    SPI1_CS_HIGH();
}

AT_SRAM(.spi)
void spi_flash_write_within_32bytes(uint8_t *wt_buf, uint32_t wt_addr, uint8_t byte_size)
{
    uint32_t addr;
    uint8_t size =  (byte_size+3)/4;
    uint32_t *data_ptr = (uint32_t *)wt_buf;

    if(byte_size>32 || byte_size==0) {
        loge("out of range");
        return;
    }

    addr = spi_flash_cmd_addr_combine(wt_addr,0x02);
    spi_flash_wait_busy();
    spi_flash_write_en();
    spi_flash_wait_WEL();
    //write to TX FIFO
    for(uint8_t n=0;n<size;n++) {
        HAL_spi_module_write_data(*data_ptr);
        data_ptr++;
    }
    //(0x02) page program
    SPI1_CS_LOW();
    HAL_spi_module_transfer_config(SPI_CASIZE_4BYTE,byte_size,DISABLE);
    HAL_spi_module_write_start_with_ca(addr,0x00,1);
    SPI1_CS_HIGH();

    spi_flash_wait_busy();
}

void spi_test(void)
{
    /*
        spi flash:
        All instructions,addresses and data are shifted in and out of the device, beginning with the most
        significant bit on the the first rising edge of SCLK after /CS is driven low.
      */
    logi("%s", __func__);

#if SPI0_EN
    logi("spi erase");
    spi_flash_erase(0x00, 0, 0);
    spi_flash_waitbusy();
    logi("%s,%d", __func__,__LINE__);

    for (uint32_t i=0;i<0x100;i++) {
        ttbuf[i] = i;
    }

    logi("spi write");
    spi_flash_write((uint8_t *)ttbuf, 0x00, 0x100, 1);
    spi_flash_waitbusy();
    memset(ttbuf, 0x55, 0x100);

    logi("spi read");
    spi_flash_read((uint8_t *)ttbuf, 0x00, 0x100);
    printf_array(ttbuf, 0x100);

#elif SPI1_EN
#if SPI1_MODE_SEL
    volatile uint32_t val;

    //(0x9F) read identification
    SPI1_CS_LOW();
    HAL_spi_module_transfer_config(SPI_CASIZE_1BYTE,3,DISABLE);
    val = HAL_spi_module_read_with_ca(0x9F,0x00);
    SPI1_CS_HIGH();
    logi("id = %x", val);

    //(0x90) read manufacturer/device id
    SPI1_CS_LOW();
    HAL_spi_module_transfer_config(SPI_CASIZE_4BYTE,2,DISABLE);
    val = HAL_spi_module_read_with_ca(0x90 | 0x000000,0x00);
    SPI1_CS_HIGH();
    logi("id = %x", val);

    //(0x06) write enable
    SPI1_CS_LOW();
    HAL_spi_module_transfer_config(SPI_CASIZE_1BYTE,0,DISABLE);
    HAL_spi_module_write_with_ca(0x06, 0x00, 0);
    SPI1_CS_HIGH();

    //(0x20) sector erase
    SPI1_CS_LOW();
    HAL_spi_module_transfer_config(SPI_CASIZE_4BYTE,0,DISABLE);
    HAL_spi_module_write_with_ca(0x20 | 0x000000, 0x00,0);
    SPI1_CS_HIGH();

    //(0x05) read status register
    SPI1_CS_LOW();
    do {
        HAL_mdelay(10);
        HAL_spi_module_transfer_config(SPI_CASIZE_1BYTE,1,DISABLE);
        val = (uint8_t)HAL_spi_module_read_with_ca(0x05,0x00);
    } while(val & 0x01);
    SPI1_CS_HIGH();

    for (uint8_t n=0;n<0x10;n++) {
        //(0x06) write enable
        SPI1_CS_LOW();
        HAL_spi_module_transfer_config(SPI_CASIZE_0BYTE,1,DISABLE);
        HAL_spi_module_write(0x06);
        SPI1_CS_HIGH();

        //(0x02) page program
        SPI1_CS_LOW();
        HAL_spi_module_transfer_config(SPI_CASIZE_4BYTE,1,DISABLE);
        HAL_spi_module_write_with_ca(0x02|(n<<24), 0x00, 0xa0+n);
        SPI1_CS_HIGH();

        //(0x05) read status register
        SPI1_CS_LOW();
        do {
            HAL_mdelay(10);
            HAL_spi_module_transfer_config(SPI_CASIZE_1BYTE,1,DISABLE);
            val = (uint8_t)HAL_spi_module_read_with_ca(0x05, 0x00);
        } while(val & 0x01);
        SPI1_CS_HIGH();

        //(0x03) read data
        SPI1_CS_LOW();
        HAL_spi_module_transfer_config(SPI_CASIZE_4BYTE,1,DISABLE);
        val = (uint8_t)HAL_spi_module_read_with_ca(0x03|(n<<24),0x00);
        SPI1_CS_HIGH();
        if(val != 0xa0+n) {
            loge("read 1byte ERR!!");
        }
        logi("val = %x", val);
    }

    logi("start_with_ca test---------");
    //use spi_module_read_start_with_ca/spi_module_read_start_with_ca test
    for (uint32_t i=0;i<0x80;i++) {
        ttbuf[i] = i;
    }
    spi_flash_write_within_32bytes(ttbuf,0x100,17);
    spi_flash_read_within_32bytes(&ttbuf[0x80],0x100,17);
    if(memcmp(&ttbuf[0],&ttbuf[0x80],17) != 0) {
        loge("write and read 17 byte test ERR!!");
    }
    printf_array(&ttbuf[0x80],32);

    logi("dma test---------");
    spi_dma_t dma_param;
    dma_param.rx_en = ENABLE;
    dma_param.tx_en = ENABLE;
    dma_param.rx_callback = NULL;
    dma_param.tx_callback = NULL;
    spi_dma_mode_init(&dma_param);

    for (uint32_t i=0;i<0x80;i++) {
        ttbuf[i] = i;
    }

    //dma write
    logi("dma write");
    spi_flash_write_en();
    spi_flash_wait_WEL();
    SPI1_CS_LOW();
    spi_flash_dma_write(ttbuf,0x200,0x80);
    SPI1_CS_HIGH();
    spi_flash_wait_busy();

    //dma read
    logi("dma read");
    memset(&ttbuf[0x80],0,0x80);
    SPI1_CS_LOW();
    spi_flash_dma_read(&ttbuf[0x80],0x200,0x80);
    SPI1_CS_HIGH();
    if(memcmp(&ttbuf[0],&ttbuf[0x80],0x80) != 0) {
        loge("dma test ERR!!");
    }
    printf_array(&ttbuf[0x80],0x80);

    logi("spi_module_write/spi_module_read test---------");
    SPI1_CS_INIT();
    //spi_module_write/spi_module_read only use in this case: ca_size 0byte ,data_size 1byte
    HAL_spi_module_transfer_config(SPI_CASIZE_0BYTE,1,DISABLE);

    SPI1_CS_LOW();
    HAL_spi_module_write(0x03);
    HAL_spi_module_write(0x00);
    HAL_spi_module_write(0x00);
    HAL_spi_module_write(0x00);
    for (uint8_t n=0;n<0x10;n++) {
        val = (uint8_t)HAL_spi_module_read();
        if(val != 0xa0+n) {
            loge("read 16byte ERR!!");
        }
        logi("val = %x", val);
    }

    SPI1_CS_HIGH();

    logi("---------");

#endif
    logi("spi erase");
    spi_flash_erase(0x7D000, 0, 1);
    for (uint32_t i=0;i<0x100;i++) {
        ttbuf[i] = i;
    }

    logi("spi write");
    spi_flash_write((uint8_t *)ttbuf, 0x7D000, 0x100, 1);
    memset(ttbuf, 0x55, 0x100);

    logi("spi read");
    spi_flash_read((uint8_t *)ttbuf, 0x7D000, 0x100);
    printf_array(ttbuf, 0x100);
#endif

    while(1);
}


#endif


#else
void app_spi_init(void)
{
}

void spi_flash_read(uint8_t *ram_addr, uint32_t flash_addr, uint32_t bytes)
{
}

void spi_flash_write(uint8_t *ram_addr, uint32_t flash_addr,
    uint32_t bytes, uint32_t wait_busy)
{
}

void spi_flash_erase(uint32_t flash_addr, uint32_t chip_erase, uint32_t wait_busy)
{
}

#endif

