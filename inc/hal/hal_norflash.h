#ifndef _NORFLASH_H_
#define _NORFLASH_H_


#define FLASH_CHIP0_SIZE    0x80000     //512KB

/*
 * flash类型
 */
enum spiflash_type_e {
    SPIFLASH_TYPE_INT       = 0,   /* 内部flash */
    SPIFLASH_TYPE_EXT_SPI0  = 1,   /* 外部spi0 flash */
    SPIFLASH_TYPE_EXT_SPI1  = 2,   /* 外部spi1 flash */
};

enum flash_chip_e {
    FLASH_CHIP0 = 0x0,
    FLASH_CHIP1,
};

enum erase_cmd_e {
    ERASE_BLCOK_64K = 0,
    ERASE_BLCOK_32K,
    ERASE_ERASE_4K,
    ERASE_CHIP
};


enum
{
    NOR_WORK_MODE_INDIRECT=0,
    NOR_WORK_MODE_DIRECT,
};

/* Line mode Define */
enum
{
    LINE_MODE_SINGLE = 0x00,
    LINE_MODE_TWO,
    LINE_MODE_FOUR,
};

/* clk Define */
enum
{
    NORFLASH_CLK_32M = 0x00,
    NORFLASH_CLK_48M,
};

typedef struct {
    uint8_t clk_sel;
    uint8_t line_mode;
    uint8_t size_mode; // 0: 128KB, 1: 256KB, 2: 512KB, 3: 1MB, 4: 2MB, 5: 4MB
} norflash_param_t;



void HAL_norflash_init(uint32_t offset, norflash_param_t *para);
void HAL_norflash_read_id(uint32_t ram_addr, uint32_t len);
void HAL_norflash_read(uint32_t ram_addr, uint32_t flash_addr, uint32_t bytes);
void HAL_norflash_write(uint32_t ram_addr, uint32_t flash_addr, uint32_t bytes, bool erase, uint32_t wait_busy);
void HAL_norflash_erase(uint32_t flash_addr, uint32_t wait_busy);
void HAL_norflash_erase_plus(uint8_t cmd, uint32_t flash_addr);
int32_t HAL_norflash_waitbusy(void);
uint16_t HAL_norflash_get_crc(uint16_t val, uint8_t *buf, uint32_t len);
void HAL_decrypt_code(void);
void HAL_norflash_chip1_mapping(uint32_t map);
void HAL_norflash_workmode_change(uint32_t work_mode);
void HAL_norflash_clk_change(uint32_t clk_div);
void HAL_norflash_chip_sel(uint32_t chip_id);
void HAL_norflash_load_code(uint32_t addr_ram, uint32_t addr_flash, uint32_t len);
void HAL_norflash_write_irq_clear_region_pending(uint8_t region, uint8_t cpux);
void HAL_norflash_write_irq_set_region_addr(uint8_t region, uint32_t start_addr, uint32_t end_addr);




#endif

