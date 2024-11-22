#ifndef _LOCAL_PLAY_MANAGER_H_
#define _LOCAL_PLAY_MANAGER_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "oal.h"
#include "device.h"
#include "sltk_audio.h"
#include "sys_config.h"

/* FAST_STEP_MS use for fast backward and fast forward */
#define FAST_STEP_MS 5000
#define FAST_EVENT_PERIOD 500

enum play_direction_e {
    PLAY_NEXT,
    PLAY_PREV,
    PLAY_SELECT,
};

enum play_repeat_e {
    REPEAT_ALL,
    REPEAT_SINGLE,
    REPEAT_RANDOM,
    REPEAT_FOLDER,
    REPEAT_MAX = REPEAT_FOLDER,
};

struct play_time {
    uint32_t min;
    uint32_t sec;
    uint32_t ms;
};

enum file_manager_ret {
    FMG_OK              = 0,
    FMG_ERR             = -1,
    FMG_OPENDIR_ERR     = -2,
    FMG_READDIR_ERR     = -3,
    FMG_END_OF_DIR      = -4,
    FMG_NO_DIR          = -5,
    FMG_NO_FILE         = -6,
    FMG_NO_NEXT_FILE    = -7,
    FMG_NO_LAST_FILE    = -8,
};

enum player_ret {
    PLAYER_RET_SUCCESS      = 0,
    PLAYER_RET_DEV_OFFLINE  = -1,
    PLAYER_RET_DEV_NOFILE   = -2,
};


struct file_manager {
    int32_t total_file;               /* 当前设备音频文件数量 */
    int32_t limit_min_num;            /* 限制播放的文件最小编号（文件夹循环播放使用到此变量) */
    int32_t limit_max_num;            /* 限制播放的文件最大编号（文件夹循环播放使用到此变量) */
    uint32_t sdcard_total_file;       /* SD卡音频文件数量 */
    uint32_t udisk_total_file;        /* U盘音频文件数量 */
    uint32_t sdcard_file_checksum;    /* SD卡文件检索的校验和（此参数用于标识是否是同一个SD卡） */
    uint32_t udisk_file_checksum;     /* U盘文件检索的校验和（此参数用于标识是否是同一个U盘） */
    uint32_t sdcard_file_fail_count;  /* 表示SD卡连续出现无法播放的文件个数 */
    uint32_t udisk_file_fail_count;   /* 表示U盘连续出现无法播放的文件个数 */
    bool sdcard_file_invalid : 1;     /* 当SD卡中所有文件都不可播放时，此变量被置为1 */
    bool udisk_file_invalid : 1;      /* 当U盘中所有文件都不可播放时，此变量被置为1 */
};

struct native_player {
#if AUDIO_LOCAL_MUSIC_EN
    struct file_manager manager;            /* 文件管理 */
    struct play_time ptime;                 /* 播放时间 */

    uint32_t file_num;                      /* 当前的文件编号 */
    uint32_t total_dir_num;                 /* 总文件夹个数 */
    uint32_t dir_num;                       /* 当前文件夹编号 */

    /* fast_ms use for fast backward and fast forward */
    int32_t last_time;                     /* 快进快退前的播放时间点 */
    int32_t fast_ms;                       /* 总共快进快退的时间 */
    uint32_t fast_event_timestamp;         /* 快进快退事件时间戳 */
#endif
    uint8_t localmusic_volume;                  /* 音乐音量（范围：0~100） */
    uint8_t volume_step;

#if AUDIO_LOCAL_MUSIC_EN
    uint8_t repeat_mode;                   /* 重复播放模式 */

    uint8_t breakpoint_wr;                 /* 0:null; 1: save break info; 2: clear; */
    uint8_t breakpoint_rd;                 /* 0:null; 1: read break info; */
    uint8_t play_direction;                /* 播放方向，决定播上一首还是播下一首或者选择指定文件号播放 */

    bool mute_for_fast : 1;                /* 快进快退时是否mute */
    bool resume_for_fast : 1;              /* 此变量标识快进快退完成后是否需要调用resume cmd */
    bool file_num_dirty : 1;               /* 标识文件编号是否已经过时（文件夹切换时用到此变量） */
    bool paused : 1;                       /* 标识是否是暂停状态 */
    bool fast : 1;                         /* 标识是否是快进快退状态 */
    bool wait_get_folder_range : 1;        /* 标识是否需要获取文件夹文件编号范围 */
    bool is_music_mode : 1;                /* 标识是否是music mode */
    bool n10 : 1;                          /* 标识上下首时文件号是加减10还是加减1 */
#endif
    notify_clbk  app_listener;
};

typedef struct {
    char* root;
    notify_clbk listener;
}local_play_manager_init_para;

typedef enum {
    LOCAL_PLAY_MANAGER_CMD_REQ_APP_HANDLE_EVENT = 0,
    LOCAL_PLAY_MANAGER_CMD_REQ_APP_HANDLE_DEV_OFFLINE,
    LOCAL_PLAY_MANAGER_CMD_REQ_APP_DISP_STU,
    LOCAL_PLAY_MANAGER_CMD_REQ_APP_DISP_MEMU,
    LOCAL_PLAY_MANAGER_CMD_REQ_APP_DISP_MEMU_SET,
    LOCAL_PLAY_MANAGER_CMD_REQ_APP_CHECKING_NEXT_FILE,
    LOCAL_PLAY_MANAGER_CMD_REQ_APP_CHECKING_PC_CONNECT_STU,
    LOCAL_PLAY_MANAGER_CMD_REQ_APP_STOP_SD,
    LOCAL_PLAY_MANAGER_CMD_REQ_APP_SET_SD_STU,
    LOCAL_PLAY_MANAGER_CMD_REQ_APP_RESET_AND_SET_USB_STU,
    LOCAL_PLAY_MANAGER_CMD_REQ_APP_PLAY_NUM_READ,
    LOCAL_PLAY_MANAGER_CMD_REQ_APP_PLAY_NUM_WRITE,
    LOCAL_PLAY_MANAGER_CMD_REQ_APP_PLAY_TIME_READ,
    LOCAL_PLAY_MANAGER_CMD_REQ_APP_PLAY_TIME_WRITE,
    LOCAL_PLAY_MANAGER_CMD_REQ_APP_PLAY_INFO_WRITE_WHEN_PWROFF,
    LOCAL_PLAY_MANAGER_CMD_REQ_APP_FILE_SCAN,
}local_play_manager_cmd;

typedef struct {
    uint8_t  menu_type;
    uint32_t val0;
    uint32_t va11;
}local_play_manager_disp_menu_ack;

void local_play_manager_submitting_event(uint32_t who, uint32_t event);

void local_play_manager_init_err_handling();
void local_play_manager_dec_err_handling();
void local_play_manager_musicmode_open_finish_handling();
void local_play_manager_recordmode_open_finish_handling();
void local_play_manager_stop_finish_handling();

void local_play_manager_set_play_direction(enum play_direction_e play_direction);
void local_play_manager_player_exit_music_mode(void);

/*
    各模式切換處理函數
 */
void local_play_manager_musicmode_swicth_proc(bool enter);
void local_play_manager_btmode_swicth_proc(bool enter);
void local_play_manager_usbamode_swicth_proc(bool enter);
void local_play_manager_lineinmode_switch_proc(bool enter);
void local_play_manager_spdifmode_switch_proc(bool enter);
void local_play_manager_lineinmode_swicth_proc(bool enter);

void local_play_manager_sdcard_check_valid(uint32_t checksum);
void local_play_manager_udisk_check_valid(uint32_t checksum);
/*
    場景保存和復原
*/
void local_play_manager_musicmode_scene_snapshot();
void local_play_manager_musicmode_scene_recovery();


void local_play_manager_mute_for_fastskip();
int locaL_play_manager_get_playfile_num();
void local_play_manager_set_autoplay_filenum_step10();

int local_play_manager_checkout_next_file();
int local_play_manager_play_file(int index, bool reset_pause_sta);
int local_play_manager_auto_play_file(int32_t dir_inc);

//切换文件播放（next_file: false-上一首； true-下一首）
void local_play_manager_play_switch_file(bool next_file);
//切换文件夹播放（next_folder: false-上一文件夹； true-下一文件夹）
void local_play_manager_play_switch_folder(bool next_folder);

int local_play_manager_play_new_device(uint32_t dev_num, uint32_t direction, bool sd, bool scan);
int local_play_manager_switch_device(uint32_t direction, bool back);
int local_play_manager_fast_step(int32_t step_ms);
int local_play_manager_play_or_pause(bool sync_info_and_disp);

//快进快退(forward: 0-快退；1-快进)
void local_play_manager_fast_skip(bool forward);
int local_play_manager_fast_end(void);
int local_play_manager_is_fast_step_mode();
int local_play_manager_is_paused();
//如果info非空，時間戳信息將會保存至info中
int local_play_manager_get_play_time(struct play_time* info);
int local_play_manager_set_repeat(void);
int local_play_manager_volume_set(uint8_t vol);
int local_play_manager_volume_get();
int local_play_manager_set_eq(void);
int local_play_manager_volume_down(void);
int local_play_manager_volume_up(void);
void local_play_manager_breakpoint_info_invalid(void);
void local_play_manager_breakpoint_info_standby();
uint32_t local_play_manager_get_breakpoint_time(void);
void local_play_manager_play_num_read(void);
void local_play_manager_play_num_write(void);
void local_play_manager_play_time_read(void);
void local_play_manager_play_time_write(void);
void local_play_manager_play_info_write_when_poweroff(void);
uint8_t local_play_manager_spec_get_info(uint8_t *buf);
void local_play_manager_dump_seek_table();

int local_play_manager_file_manager_init(char *root);
/* metadata（ID3）保存函数
 * 此函数由audio库调用； 播放器调用此函数以key-value键值对方式给出 metadata信息；
 * @param ctype: 解码类型
 * @param key: metadata key值
 * @param val: metadata value值
 */
void local_play_manager_metadata_save(uint32_t ctype,
    const char *key, const char *val);
int local_play_manager_file_manager_deinit(void);
/* 初始/銷毀 */
int32_t local_play_manager_init(local_play_manager_init_para* init);
void local_play_manager_deinit();

#ifdef __cplusplus
}
#endif

#endif
