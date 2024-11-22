#ifndef _SD_DISK_UPDATE_H_
#define _SD_DISK_UPDATE_H_

#include "ff.h"
#include "xfs.h"

#define ONCE_READ_SIZE            512
#define HALF_ONCE_READ            256

//正数的错误号使用文件系统的错误码
enum {
    UPDATE_OK = 0,
    UPDATE_LACK_OF_MEMORY = -1, 
    UPDATE_RET_FILE_HEADER_ERROR = -2,
    UPDATE_FILE_HEAD_ERR = -3,
    UPDATE_RET_FILE_STAMP_IS_SAME = -4,     //时间戳是一样的
    UPDATE_RET_FILE_DATA_CRC_ERROR = -5,
    UPDATE_FILE_NOT_SUPPORT = -6,
};

typedef enum _UPDATE_STAGE_{
    STAGE_BEGIN_CHECK_DATA = 0,
    STAGE_PRE_UPDATE_FLASH = 1,
    STAGE_UPDATE_FLASH_FINISH = 2,
}e_update_stage;

typedef struct _PARTITION_INFO_
{
    uint32_t startAddr;
    uint32_t length;
    uint32_t crc;
    uint32_t reserved;
}PartionInfo;

typedef struct _UPDATE_CALLBACK_
{
    void (*process_cb)(float process) ;
    void (*process_stage)(e_update_stage e_stage);
}UpdateCallback;

typedef struct _UPDATE_OPERATION_
{
    void* (*xopen)(void);
    uint8_t (*xclose)(void* fp);
    uint8_t (*xread)(void* fp, void* buff, UINT btr, UINT* br);
    uint8_t (*xseek)(void* fp, uint64_t ofs);
    uint64_t (*xsize)(void* fp);
    void (*xclean)(void);
    void (*process_cb)(float process) ;
    void (*process_stage)(e_update_stage e_stage);
}UPDATE_OPERATION;

void register_update_progress_cb(UpdateCallback* process_cb );
void register_update_operation(UPDATE_OPERATION* update_opt );
uint8_t update_process();

void pc_update_process();
void update_from_pc_flag_set();
void update_from_pc_flag_clear();
void update_from_sd_flag_set();
void update_from_sd_flag_clear();
BOOL is_update_from_sd_flag_set();
BOOL is_update_from_pc_flag_set();


#endif
