#ifndef _SDMMC_IO_H_
#define _SDMMC_IO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sys_integer.h"
//#include "diskio.h"

#define SD_SINGLE_WR_MODE               0
#define SD_MULTI_WR_PERFORMANCE_MODE    1
#define SD_MULTI_WR_LOWPOWER_MODE       2

typedef struct {
    uint8_t buswide;
    uint8_t workClkDiv;
    uint8_t sd_num;
    uint8_t busMode;    //
} sd_disk_init_params_t;

typedef BYTE	DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;

enum sd_fsca_cmd {
	SD_FSCA_INIT,
	SD_FSCA_READ_CONTINUE,
	SD_GET_FSCA_TMP_BUFFER
};


enum sd_hal_cmd {
    SD_HAL_GET_STATUS,
    SD_HAL_SET_STATUS,
    SD_HAL_FAIL_CB,
    SD_HAL_GET_CLK_DIV
};

typedef struct _CARD_PRIVATE_INFO_
{
    DWORD total_sector;
    uint32_t g_cardtype;
    uint32_t g_cardLastStatus;
    uint32_t cardRca;
}CardPrivareInfo;

/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */


/* Command code for disk_ioctrl fucntion */

/* Generic command (Used by FatFs) */
#define CTRL_SYNC			0	/* Complete pending write process (needed at _FS_READONLY == 0) */
#define GET_SECTOR_COUNT	1	/* Get media size (needed at _USE_MKFS == 1) */
#define GET_SECTOR_SIZE		2	/* Get sector size (needed at _MAX_SS != _MIN_SS) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (needed at _USE_MKFS == 1) */
#define CTRL_TRIM			4	/* Inform device that the data on the block of sectors is no longer used (needed at _USE_TRIM == 1) */

DSTATUS HAL_sd_disk_status(void);
DSTATUS HAL_sd_disk_init(sd_disk_init_params_t *params);

DSTATUS HAL_disk_init_bypass(void);
DRESULT HAL_sd_disk_read_start(BYTE *buff, DWORD sector);
DRESULT HAL_sd_disk_read2(BYTE *buff, DWORD sector, UINT count);
DRESULT HAL_sd_disk_write(BYTE *buff, DWORD sector, UINT count);
DRESULT sd_disk_write2(BYTE *buff, DWORD sector);
DRESULT HAL_sd_disk_ioctl(BYTE cmd, void *buff);
uint8_t HAL_wait_sd_ready(uint32_t timeout);
void HAL_sd_disk_write_wait_ready();
DRESULT HAL_sd_disk_write_start(BYTE *buff, DWORD sector);
DRESULT HAL_sd_fail_cb(void);
DRESULT HAL_sd_get_status(void);
void HAL_switch_card_privateinfo(uint8_t sd_num);
void HAL_set_cartprivateinfo(uint8_t sd_num, CardPrivareInfo* privateInfo );
void HAL_get_cartprivateinfo(uint8_t sd_num, CardPrivareInfo* privateInfo );

typedef void (*sdmmc_fsca_callback)(uint8_t cmd, BYTE **data);
typedef void (*sdmmc_app_callback)(uint8_t cmd, uint32_t *data);


void HAL_fsca_register_sd_callback(void (*sdmmc_fsca_callback)(uint8_t cmd, BYTE **data));
void HAL_app_register_sd_callback(void (*sdmmc_app_callback)(uint8_t cmd, uint32_t *data));
void HAL_sd_stop(void);
void HAL_sd_set_work_clk_div(uint8_t ClockDiv);
void HAL_set_work_sd_num(uint8_t curNum);

//DRESULT sd_disk_read_retry(void);

#ifdef __cplusplus
}
#endif

#endif
