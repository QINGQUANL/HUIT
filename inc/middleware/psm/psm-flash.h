#ifndef __PSM_FLASH_H__
#define __PSM_FLASH_H__

#ifdef __linux__
#include <stdio.h>
#endif
#include <psm-types.h>

typedef struct flash_desc {
	/** The flash device */
	uint8_t   fl_dev;
	/** The start address on flash */
	uint32_t  fl_start;
	/** The size on flash */
	uint32_t  fl_size;
	/** The flash erase block size */
	uint32_t  fl_block_size;
	/** The actual size used in each flash block */
	uint32_t  fl_actul_used_block_size;
} flash_desc_t;


typedef struct mdev {
#ifdef __linux__
	FILE* file;
#endif 
	flash_desc_t flash;
}mdev_t;


typedef enum {
	WIN_FILE = 1,
	INTERNAL_FLASH,
	EXTERNAL_SPI_NOR_FLASH,
}store_type;

void flash_drv_init(void);
mdev_t* flash_drv_open(flash_desc_t* fdesc);
int flash_drv_close(mdev_t* dev);
int flash_drv_write(mdev_t* dev, const uint8_t* rom_buf,
	uint32_t len, uint32_t addr);
int flash_drv_read(mdev_t* dev, uint8_t* buf,
	uint32_t len, uint32_t addr);
int flash_drv_erase(mdev_t* dev, unsigned long start,
	unsigned long size);
int flash_drv_eraseall(mdev_t* dev);

#endif

