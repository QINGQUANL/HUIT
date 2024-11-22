
#ifndef _AUDIO_PLAYER_H_
#define _AUDIO_PLAYER_H_

#include "sys_inc.h"
#include "event.h"
#include "sltk_audio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PLAYER_DEV_UDISK = 0,
    PLAYER_DEV_SDCARD,
    PLAYER_DEV_NUM
} PLAYER_DEV_TYPE;

typedef enum {
    PLAYER_CMD_NONE             = 0,

    /** nonsynchronous cmd **/

    /* resume play;
     * The command argument is maximum time to wait for the command to be processed
     * Example:
     *    player_cmd(PLAYER_CMD_RESUME, 0);
     */
    PLAYER_CMD_RESUME               = 1,

    /* pause play;
     * The command argument is maximum time to wait for the command to be processed
     * Example:
     *    player_cmd(PLAYER_CMD_PAUSE, 0);
     */
    PLAYER_CMD_PAUSE                = 2,


    /* stop play;
     * The command argument is maximum time to wait for the command to be processed
     * NOTE: This command is an internal command. APP should not use it
     */
    PLAYER_CMD_STOP                 = 3,


    /* seek to play;
     * The command argument is the play time(millisecond) to seek
     * Example:
     *    //seek to 20s
     *    player_cmd(PLAYER_CMD_SEEK, 20000);
     */
    PLAYER_CMD_SEEK                 = 4,

    PLAYER_CMD_BP_SEEK_HELPER       = 5,
    PLAYER_CMD_DUMP_SEEK_HELPER     = 6,

    /* open command
     * NOTE: This command is an internal command. APP should not use it
     */
    PLAYER_CMD_OPEN                 = 7,

    /* open command
     * NOTE: This command is an internal command. APP should not use it
     */
    PLAYER_CMD_CLOSE                = 8,

    /** synchronous cmd **/

    /* set loop play;
     * The command argument: 0: do not loop; 1: loop
     * Example:
     *    //set loop play
     *    player_cmd(PLAYER_CMD_LOOP, 1);
     */
    PLAYER_CMD_LOOP                 = 9,

    /* set play duration(millisecond);
     * Example:
     *    //Play one minute
     *    player_cmd(PLAYER_CMD_SET_PLAY_DURATION, 60000);
     */
    PLAYER_CMD_SET_PLAY_DURATION    = 10,

    PLAYER_CMD_GET_STREAM_SOURCE    = 11,

    /* get audio_player status. audio_player status type see @ap_status_t
     * The command argument is a @uint32_t pointer
     * Example:
     *    uint32_t sta;
     *    player_cmd(PLAYER_CMD_GET_STATUS, (uint32_t)&sta);
     */
    PLAYER_CMD_GET_STATUS           = 12,

    /* get audio_player error type. audio_player error type see @ap_err_t
     * The command argument is a @uint32_t pointer
     * Example:
     *    uint32_t err_type;
     *    player_cmd(PLAYER_CMD_GET_ERROR_TYPE, (uint32_t)&err_type);
     */
    PLAYER_CMD_GET_ERROR_TYPE       = 13,    /* get error type */

    /* get current play time(millisecond)
     * The command argument is a @uint32_t pointer
     * Example:
     *    uint32_t t;
     *    player_cmd(PLAYER_CMD_GET_TIME, (uint32_t)&t);
     */
    PLAYER_CMD_GET_TIME             = 14,

    /* get this file media info
     * The command argument is a @media_info_t pointer
     * Example:
     *    media_info_t info;
     *    player_cmd(PLAYER_CMD_GET_MEDIAINFO, (uint32_t)&info);
     */
    PLAYER_CMD_GET_MEDIAINFO        = 15,

    /* wait all command done;
     * The command argument is ignored
     * Example:
     *    player_cmd(PLAYER_CMD_WAIT_DONE, 0);
     */
    PLAYER_CMD_WAIT_DONE            = 16,    /* wait cmd done */

    PLAYER_CMD_SET_NEXT_FILE_FUNC = 17,
    PLAYER_CMD_SET_UST_PRIV_DATA_TO_FLASH = 18,
    PLAYER_CMD_GET_UST_PRIV_DATA_FROM_FLASH   = 19,
    PLAYER_CMD_GET_SOURCE_INFO = 20,
} player_cmd_t;

//播放器事件定义
typedef enum {
    PLAYER_EVENT_NONE                 = 0,
    //seek 命令处理完成事件
    PLAYER_EVENT_SEEK_COMPLETED       = PLAYER_EVENT | 0x1,
    //播放异常事件
    PLAYER_EVENT_ERR                  = PLAYER_EVENT | 0x2,
    //文件播放结束事件
    PLAYER_EVENT_END_OF_FILE          = PLAYER_EVENT | 0x3,
    //停止播放命令处理完成事件
    PLAYER_EVENT_STOP_COMPLETED       = PLAYER_EVENT | 0x4,
    //暂停命令处理完成事件
    PLAYER_EVENT_PAUSE_COMPLETED      = PLAYER_EVENT | 0x5,
    //继续播放命令处理完成事件
    PLAYER_EVENT_RESUME_COMPLETED     = PLAYER_EVENT | 0x6,
    //open命令处理完成事件
    PLAYER_EVENT_OPEN_COMPLETED       = PLAYER_EVENT | 0x7,
} player_event_t;

typedef enum {
    APERR_NONE                      = 0, //没有错误
    APERR_COMMON                    = 1, //播放器通用错误类型
    APERR_OPEN_STREAM               = 2, //播放器打开数据流出错
    APERR_FORMAT_PROBE              = 3, //音频格式错误
    APERR_CREATE_PARSER             = 4, //创建parser失败
    APERR_OPEN_PARSER               = 5, //parser open失败
    APERR_OPEN_INVALID_PARAMS       = 6, //音频流参数不支持
    APERR_CREATE_DEC                = 7, //创建decoder失败
    APERR_OPEN_DEC                  = 8, //decoder open失败
    APERR_RESET_DEC                 = 9, //reset decoder失败
    APERR_START_OUTPUT              = 10, //开始播放失败
    APERR_CREATE_TRACK              = 11, //创建audio track失败
    APERR_OPEN_TRACK                = 12, //audio track open失败
    APERR_SEEK                      = 13, //seek命令执行出错
    APERR_FRAME_DEC                 = 14, //解码数据帧出错
} ap_err_t;

//播放器状态定义
typedef enum {
    AP_STATUS_CLOSED                = 0, //表示播放器已经关闭
    AP_STATUS_OPENED                = 1, //表示播放器已经打开
    AP_STATUS_STOPPED               = 2, //表示已经停止播放
    AP_STATUS_PLAYING               = 3, //表示正在播放
    AP_STATUS_PAUSED                = 4, //表示已经暂停
    AP_STATUS_ERROR                 = 5, //表示播放器处于播放错误状态
    AP_STATUS_DESTROY               = 6, //表示播放器已经被销毁
} ap_status_t;

typedef struct ap_init_params_s {
    uint8_t src_type; //数据流源， 参考@streamtype_t

    /* Maximum sample rate */
    uint32_t max_rate;
    /* Maximum channels */
    uint8_t max_ch;
    bool fast_seek_table_enable;
} ap_init_params_t;

typedef struct {
    uint32_t num;       /* 表示文件编号或者文件夹编号(dir变量为1时表明num是文件夹编号) */
    uint32_t seek_time; /* 从文件的哪个时间点（毫秒）开始播放 */
#if AUDIO_LOCAL_MUSIC_EN
    char name[50];      /* 文件名，如果为空字符串，则使用文件编号 */
#else
    char name[10];      /* 文件名，如果为空字符串，则使用文件编号 */
#endif
    uint8_t type;       /* SLTK_STYPE_FLASH_TONE or SLTK_STYPE_FATFS */
    uint8_t dev_type;   /* PLAYER_DEV_UDISK or PLAYER_DEV_SDCARD */
    uint8_t tone_vol;
    bool pause : 1;     /* 打开后是否设置成暂停状态 */
    bool dir : 1;       /* num变量表示的是文件编号还是文件夹编号 */
    notify_clbk client_listener;
} source_info_t;

/* send a command to the player
 * @param cmd: player command, see @player_cmd_t
 * @param arg: player command argument
 * @return: return zero on success, a negative errno value on failure
 */
int32_t player_cmd(player_cmd_t cmd, uint32_t arg);

/***************************************************************************
 *               audio player API
 ***************************************************************************/

/* 打开文件和解码器
 * @param info: 数据来源信息
 * @return: 成功返回0；失败返回负值错误码
 */
int32_t player_open(source_info_t *info);

/* 关闭文件和解码器
 * @param max_wait_complete_time: 最大等待播放完成的时间；如果设为0则表示立即关闭播放器
 */
void player_close(uint32_t max_wait_complete_time);

/* 继续播放
 * @param maximum_block_time: 最大等待处理的时间
 */
#define player_resume(maximum_block_time)   player_cmd(PLAYER_CMD_RESUME, maximum_block_time)

/* 暂停播放
 * @param maximum_block_time: 最大等待处理的时间
 */
#define player_pause(maximum_block_time)    player_cmd(PLAYER_CMD_PAUSE, maximum_block_time);


/* 设置断点播放
 * @param seek_to_ms: 表示要跳转到哪个时间点播放
 */
#define player_seek(seek_to_ms)             player_cmd(PLAYER_CMD_SEEK, seek_to_ms);


#define player_bp_seek_helper()             player_cmd(PLAYER_CMD_BP_SEEK_HELPER, 0);
#define player_dump_seek_helper()           player_cmd(PLAYER_CMD_DUMP_SEEK_HELPER, 0);

/* 设置循环播放
 * @param loop_play: 0:取消循环播放； 1：循环播放
 */
#define player_loop_play(loop_play)         player_cmd(PLAYER_CMD_LOOP, loop_play);

/* 设置播放时长
 * @param duration_ms: 表示要播放的时长（单位：毫秒）
 */
#define player_set_play_duration(duration_ms) player_cmd(PLAYER_CMD_SET_PLAY_DURATION, duration_ms);

/* 获取当前的数据流源
 * @return：返回当前的数据流源，参考@streamtype_t
 */
#define player_get_stream_source() \
    ({ \
        uint32_t src; \
        player_cmd(PLAYER_CMD_GET_STREAM_SOURCE, (uint32_t)&src); \
        src; \
    })

/* 获取播放器状态
 * @return：返回当前的播放器状态，参考@ap_status_t
 */
#define player_get_status() \
    ({ \
        uint32_t sta; \
        player_cmd(PLAYER_CMD_GET_STATUS, (uint32_t)&sta); \
        sta; \
    })


/* 获取播放器错误类型
 * @return：返回当前的播放器错误类型，参考@ap_err_t
 */
#define player_get_error_type() \
    ({ \
        uint32_t err_type; \
        player_cmd(PLAYER_CMD_GET_ERROR_TYPE, (uint32_t)&err_type); \
        err_type; \
    })


/* 获取当前播放的时间
 * @return：返回当前播放的时间（单位:毫秒）
 */
#define player_get_current_play_time()  \
    ({ \
        uint32_t t; \
        player_cmd(PLAYER_CMD_GET_TIME, (uint32_t)&t); \
        t; \
    })

#define player_get_source_info(p_info)  \
    ({ \
        player_cmd(PLAYER_CMD_GET_SOURCE_INFO, (uint32_t)p_info); \
    })

#define player_tone_is_playing()  \
    ({ \
        source_info_t info; \
        bool ret; \
        player_cmd(PLAYER_CMD_GET_SOURCE_INFO, (uint32_t)&info); \
        if ((SLTK_STYPE_FLASH_TONE == info.type) && \
            (AP_STATUS_CLOSED != player_get_status())) { \
            ret = true; \
        } else { \
            ret = false; \
        } \
        ret; \
    })

#define player_tone_num_is_playing(tone_num)  \
    ({ \
        source_info_t info; \
        bool ret; \
        player_cmd(PLAYER_CMD_GET_SOURCE_INFO, (uint32_t)&info); \
        if ((SLTK_STYPE_FLASH_TONE == info.type) && (tone_num == info.num) && \
            (AP_STATUS_CLOSED != player_get_status())) { \
            ret = true; \
        } else { \
            ret = false; \
        } \
        ret; \
    })

//get media info
/* 获取media 信息
 * @return p_info：返回当前数据流的media info; 参考@media_info_t
 * @return: 成功返回0；失败返回负值错误码
 */
#define player_get_media_info(p_info)  player_cmd(PLAYER_CMD_GET_MEDIAINFO, (uint32_t)p_info)

//等待命令被处理完毕
#define player_wait_command_processed() player_cmd(PLAYER_CMD_WAIT_DONE, OAL_WAIT_FOREVER)

//设置获取下一个提示音文件的函数
#define player_set_next_file_func(func) player_cmd(PLAYER_CMD_SET_NEXT_FILE_FUNC, (uint32_t)func)

//保存播放器私有数据到flash中
#define player_store_priv_data_to_flash() player_cmd(PLAYER_CMD_SET_UST_PRIV_DATA_TO_FLASH, 0)

//从flash中加载播放器私有数据
#define player_load_priv_data_from_flash() player_cmd(PLAYER_CMD_GET_UST_PRIV_DATA_FROM_FLASH, 0)


#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_PLAYER_H_ */
