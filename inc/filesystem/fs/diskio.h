/*-----------------------------------------------------------------------/
/  Low level disk interface modlue include file   (C)ChaN, 2014          /
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_sdmmc_io.h"


/* Status of Disk Functions */


typedef struct __STRUCT_DISK_IO_CALLBACK{
	DSTATUS (*pMMCDiskStatus)(void);
	DSTATUS (*pMMCDiskInit)(void);
	DSTATUS (*pMMCDiskRead)(BYTE *buff,DWORD sector,UINT count);
	DRESULT (*pMMCDiskWrite)(BYTE *buff,DWORD sector,UINT count);
	DRESULT (*pMMCDiskIoctl)(BYTE cmd,void *buff);
	DRESULT	(*pMMCReadFailCallback)(void);
}DISK_IO_CALLFOPS;

DRESULT usb_get_status(void);

/* -------------- fix sl28xx rom bug -------- */
DSTATUS SL28xx_USB_disk_status(void);
DSTATUS SL28xx_USB_disk_initialize(void);
DRESULT SL28xx_USB_disk_read(BYTE *buff, DWORD sector, UINT count);
DRESULT SL28xx_USB_disk_write(BYTE *buff, DWORD sector, UINT count);
DRESULT SL28xx_USB_disk_ioctl(
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
);

/*---------------------------------------*/
/* Prototypes for disk control functions */


DSTATUS disk_initialize (BYTE pdrv);
DSTATUS disk_status (BYTE pdrv);
DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count);
DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count);
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);
void disk_config(DISK_IO_CALLFOPS fops);


/* Definitions of physical drive number for each drive */
#define DEV_MMC		0	/* Example: Map MMC/SD card to physical drive 0 */
#define DEV_USB		1	/* Example: Map USB MSD to physical drive 1 */


/* Generic command (Not used by FatFs) */
#define CTRL_POWER			5	/* Get/Set power status */
#define CTRL_LOCK			6	/* Lock/Unlock media removal */
#define CTRL_EJECT			7	/* Eject media */
#define CTRL_FORMAT			8	/* Create physical format on the media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE		10	/* Get card type */
#define MMC_GET_CSD			11	/* Get CSD */
#define MMC_GET_CID			12	/* Get CID */
#define MMC_GET_OCR			13	/* Get OCR */
#define MMC_GET_SDSTAT		14	/* Get SD status */
#define ISDIO_READ			55	/* Read data form SD iSDIO register */
#define ISDIO_WRITE			56	/* Write data to SD iSDIO register */
#define ISDIO_MRITE			57	/* Masked write data to SD iSDIO register */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV			20	/* Get F/W revision */
#define ATA_GET_MODEL		21	/* Get model name */
#define ATA_GET_SN			22	/* Get serial number */

#define SECTOR_SZ       512
#define FS_CACHE_COUNT  4


typedef DSTATUS (*xfs_disk_status_func)(void);
typedef DSTATUS (*xfs_disk_initialize_func)(void);
typedef DRESULT (*xfs_disk_read_start_func)(BYTE *buff, DWORD sector);
typedef DSTATUS (*xfs_disk_get_status_func)(void);
typedef DRESULT (*xfs_disk_read_func)(BYTE *buff, DWORD sector, UINT count);
typedef DRESULT (*xfs_disk_write_func)(BYTE* buff, DWORD sector, UINT count);
typedef DRESULT (*xfs_disk_ioctl_func)(BYTE cmd, void* buff);
typedef DRESULT (*xfs_disk_failcb_func)(void);


typedef struct {
    xfs_disk_status_func status;
    xfs_disk_initialize_func init;
    xfs_disk_read_start_func read_start;
    xfs_disk_get_status_func get_status;
    xfs_disk_read_func read;
    xfs_disk_write_func write;
    xfs_disk_ioctl_func ioctl;
    xfs_disk_failcb_func failcb;
} xfs_disk_interface_t;

typedef struct fs_cache_s {
    BYTE tmp[SECTOR_SZ];
    BYTE *cache[FS_CACHE_COUNT];

    DWORD r_sec;
    DWORD w_sec;
    DWORD tmp_sec;
    DWORD total_sec;

    volatile uint32_t wptr;
    volatile uint32_t rptr;
    volatile uint32_t disk_status;
    volatile bool wait_flag;
    volatile bool lock;
    volatile bool single_mode;
    volatile bool tmp_init;
    bool cache_enable;
    uint32_t timeout;
} fs_cache_t;

extern fs_cache_t *fsca;

void fsca_init(bool sdcard);
void fsca_deinit(void);
void fsca_enable(bool enable);
void fsca_timeout_set(uint32_t time);
int fsca_disk_read_continue(void);

void xfs_config_sdmmc_interface(void);
void xfs_config_usb_interface(void);
void xfsdisk_delay_for_bt_enable(BOOL bEnable);
uint8_t xfs_is_usb_unit_ready();

int disk_is_lock(void);
int fs_cache_wait_complete(void);
int fsca_read_disk(BYTE *buff, DWORD sector);
int fsca_disk_read_start(void);
void USB_disk_read_dma(uint8_t *buff, uint32_t sector, uint16_t count);
void USB_disk_write_dma (uint8_t *buff, uint32_t sector,uint16_t count);

void sdmmc_fsca_cb(uint8_t cmd, BYTE **data);

DRESULT disk_check_sd(void);
#ifdef __cplusplus
}
#endif

#endif
