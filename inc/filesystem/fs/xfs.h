/* The Alps Open Source Project
 * Copyright (c) 2016 - 2017 Smartlink Technology Co. Ltd. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * xfs is the filesystem api for app
 *
 */

#ifndef _XFS_H_
#define _XFS_H_

#include "ff.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef FATFS XFS;
typedef FIL XFILE;
typedef DIR XDIR;
typedef FILINFO XFILINFO;

enum {
    DIR_TYPE_NONE,
    DIR_TYPE_BLACKLIST,
    DIR_TYPE_WHITELIST,
    DIR_TYPE_UNICODE_BLACKLIST,
    DIR_TYPE_UNICODE_WHITELIST,
};

typedef struct {
    uint32_t type;
    union {
        const char *ascii;
        const uint16_t *unicode;
    } name;
} dir_list_item_t;

enum xfs_disk_e {
    XFS_DISK_SDMMC,
    XFS_DISK_USB,
};

enum xfs_cmd_e {
    XFS_CMD_NONE,
    XFS_CMD_FSCACHE_INIT,
    XFS_CMD_FSCACHE_DEINIT,
    XFS_CMD_FSCACHE_ENABLE,
    XFS_CMD_GET_FILES_CHECKSUM,
    XFS_CMD_CONFIG_FILE_SUFFIX,
    XFS_CMD_CONFIG_DIR_LIST,
    XFS_CMD_TIMEOUT_SET,
};

FRESULT xfs_sd_init(FATFS *fs);
FRESULT xfs_cmd(UINT cmd, UINT val);
void xfs_fscache_sd_init();
void xfs_fscache_usb_init();

uint32_t xfscan(void); /* scan music files */
FRESULT xfopen_num(XFILE *fp, char *name, uint32_t num); /* Open a music file */
FRESULT xfs_dir_seek(uint32_t dir_num);
FRESULT xfs_get_current_num(uint32_t *p_dir_num, uint32_t *p_num);
FRESULT xfs_get_total_num(uint32_t *p_total_dir_num, uint32_t *p_total_num);
FRESULT xfs_get_folder_file_num_range(uint32_t cur_num,
    uint32_t *p_folder_min_file_num,
    uint32_t *p_folder_file_count);

/* Open or create a file */
FRESULT xfopen(XFILE* fp, const TCHAR* path, BYTE mode);
/* Close an open file object */
FRESULT xfclose(XFILE* fp);
/* Read data from the file */
FRESULT xfread(XFILE* fp, void* buff, UINT btr, UINT* br);
/* Write data to the file */
FRESULT xfwrite(XFILE* fp, const void* buff, UINT btw, UINT* bw);
/* Move file pointer of the file object */
FRESULT xflseek(XFILE* fp, FSIZE_t ofs);
/* Flush cached data of the writing file */
FRESULT xfsync(XFILE* fp);
/* Open a directory */
FRESULT xfopendir(XDIR* dp, const TCHAR* path);
/* Close an open directory */
FRESULT xfclosedir(XDIR* dp);
/* Read a directory item */
FRESULT xfreaddir(XDIR* dp, XFILINFO* fno);
/* Create a sub directory */
FRESULT xfmkdir(const TCHAR* path);
/* Delete an existing file or directory */
FRESULT xfunlink(const TCHAR* path);
/* Mount/Unmount a logical drive */
FRESULT xfmount(XFS* fs, const TCHAR* path, BYTE opt);
bool xfeof(XFILE* fp);
FSIZE_t xftell(XFILE* fp);
FSIZE_t xfsize(XFILE* fp);
FRESULT xfunmount(const TCHAR* path);
/*check sdcard state*/
FRESULT xfunchecksd(void);

#ifdef __cplusplus
}
#endif

#endif /* _XFS_H_ */
