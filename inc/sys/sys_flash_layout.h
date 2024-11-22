#ifndef __SYS_FLASH_LAYOUT_H__
#define __SYS_FLASH_LAYOUT_H__

#include "sys_types.h"

#define SYS_FLASH_LAYOUT_BOOTLOADER_START                   0
#define SYS_FLASH_LAYOUT_BOOTLOADER_HEADER_LENGTH           0x60
#define SYS_FLASH_LAYOUT_PARTITION_TABLE_LENGTH             0x100
#define SYS_FLASH_LAYOUT_FIRMWARE_HEADER_LENGTH             0x30

// the size shell be 0x60
typedef struct _boot_loader_header_s{
    char tag[4];
    uint32_t ram_addr_write;
    uint32_t ram_addr_run;
    uint32_t boot_addr;
    uint32_t boot_len;
    uint32_t boot_crc;
    uint32_t norf_baud;
    uint32_t norf_line;
    uint32_t boot_parition_length_with_4KB;
    uint32_t reserved1[2];
    uint32_t flag;
    uint32_t reserved2[10];
    uint32_t head_crc;
}boot_loader_header_t;

// 16 bytes
typedef struct {
    uint8_t tag[4];
    uint32_t addr;
    uint32_t len;
    uint32_t crc;
}partition_table_item_t;

// 256 bytes
typedef struct _partition_table_s{
    uint32_t partition_table_num;
    uint32_t version;
    uint8_t  reserved[8];
    partition_table_item_t item_firmware;
    partition_table_item_t others[14];
}partition_table_t;

// 0x30 bytes
typedef struct _firmware_header_s{
    uint32_t firmware_header_len;
    uint32_t timestamp;
    uint8_t  reserved[8];
    uint32_t loadToRam;
    uint32_t runFromRam;
    uint32_t loadLength;
    uint32_t loadCrc;
    uint8_t   reserved0[16];
}firmware_header_t;

void sys_flash_layout_init();
partition_table_t *sys_flash_layout_partition_table_get();
firmware_header_t *sys_flash_layout_firmware_header_get();
uint32_t sys_flash_layout_firmware_address_app_bin_get();
uint32_t sys_flash_layout_address_get_by_tag(const char *tag, uint32_t *partition_length);



#endif

