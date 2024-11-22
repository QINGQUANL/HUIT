#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "hal_clk.h"
#include "hal_gpio.h"
#include "ff.h"
#include "hal.h"
#include "middleware.h"
#include "karaoke_framework.h"

void HAL_fsca_register_sd_callback(void (*sdmmc_fsca_callback)(uint8_t cmd, BYTE **data));
void sdmmc_fsca_cb(uint8_t cmd, BYTE **data);

//uint8_t* sd_read_buff;
uint16_t read_point;
bool     buff_empty[2];
FIL file_read;

//记得把开发版的电源线插上
void app_sd_wr_init()
{
    //sd_read_buff = libc_calloc(1024);
}

FATFS otafs;

FATFS* fatfs;
uint8_t sd_read_buff[512];
bool app_sd_read_init()
{   
    logi("app_sd_read_init");
    if(device_initialize(0) != 0) {
        logi("sd init fail ====== ");
        return false;
    }

    HAL_fsca_register_sd_callback(sdmmc_fsca_cb);
    f_unmount("0:");
    xfs_fscache_sd_init();
    xfs_cmd(XFS_CMD_FSCACHE_ENABLE, 0);
    xfs_cmd(XFS_CMD_TIMEOUT_SET, 5000);     //uint: ms
    memset(&otafs, 0, sizeof(FATFS));
    
    f_mount(&otafs, "0:", 1);
    read_point = 0;
    FRESULT res = f_open(&file_read, "0:\\ota.up", FA_READ | FA_OPEN_EXISTING /*FA_WRITE | FA_CREATE_NEW*/);
    
    if (FR_NO_FILE == res || FR_NO_PATH == res) {
        logi("open file fail");
        return false;
    }
    f_lseek(&file_read, 0);
    
    UINT nRead;
    f_read(&file_read,sd_read_buff, 512, &nRead);   //读取数据
    buff_empty[0] = false;
    buff_empty[1] = true;
    printf_array(sd_read_buff, 512);
    f_close(&file_read);

    DWORD ncl;
    f_getfree("0:", &ncl, &fatfs);
    __maybe_unused uint32_t total = fatfs->n_fatent * fatfs->csize;    //总容量
    __maybe_unused uint32_t free = ncl * fatfs->csize;     //剩余容量
    logi("ncl %dM, %d M", total/2 , free/2);    //获取容量
    f_mkfs("0:", FM_ANY, 0, sd_read_buff,  sizeof(sd_read_buff));   //格式化
    
    return true; 
}

void app_sd_read_data(uint8_t* des, uint16_t readLength)
{
    UINT nRead;
    uint16_t point = read_point + readLength;
    if(point >= 1024) {
        memcpy(des, sd_read_buff + read_point, 1024 - read_point);
        
    } else {
        memcpy(des, sd_read_buff + read_point, readLength );
        if(read_point <= 512 && point > 512) {
            buff_empty[0] = true;
        }
        
        read_point += readLength;
        
        if(read_point > 512 && buff_empty[0]) {
            buff_empty[0] = false;
            f_read(&file_read,sd_read_buff, 512, &nRead);
        } else if(buff_empty[1]) {
            f_read(&file_read,sd_read_buff + 512, 512, &nRead);
            buff_empty[1] = false;
        }
    }
}

void app_sd_wr_deinit()
{
    f_close(&file_read);
    f_unmount("0:");
    
    //if(sd_read_buff) {
    //    libc_free(sd_read_buff);
    //    sd_read_buff = NULL;
    //}
}

#define FF_YEAR 2023
#define FF_MONTH 6
#define FF_DAY   1

DWORD get_fattime(void )
{
    //获取时间，这是一个时间戳
    return ((DWORD)(FF_YEAR - 1980) << 25 | (DWORD)FF_MONTH << 21 | (DWORD)FF_DAY << 16);
}


