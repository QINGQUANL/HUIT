
#ifndef _AUDIO_RECORDER_H_
#define _AUDIO_RECORDER_H_

#include "oal.h"
#include "sys_inc.h"
#include "sltk_audio.h"

#ifdef __cplusplus
extern "C" {
#endif

//录音错误类型定义
enum ar_err_e {
    AR_ERR_NONE,
    AR_ERR_COMMON,          //通用错误类型
    AR_ERR_OPEN_STREAM,     //打开文件出错
    AR_ERR_WRITE_STREAM,    //写数据出错
    AR_ERR_NO_STORE_FILE,   //文件保存出错
};

//录音状态定义
enum ar_status_e {
    AR_STATUS_CLOSED,
    AR_STATUS_OPENED,
    AR_STATUS_STOPPED,
    AR_STATUS_RECORDING,
    AR_STATUS_PAUSED,
    AR_STATUS_ERROR,
    AR_STATUS_DESTROY,
};

#define AR_EVENT    0xE0000000

//录音事件定义
enum ar_event_e {
    AR_EVENT_NONE           = 0,
    AR_EVENT_OPEN           = AR_EVENT | 0x1,
    AR_EVENT_CLOSE          = AR_EVENT | 0x2,
    AR_EVENT_START          = AR_EVENT | 0x3,
    AR_EVENT_STOP           = AR_EVENT | 0x4,
    AR_EVENT_ERROR          = AR_EVENT | 0x5,
};

enum ar_cmd_e {
    AR_CMD_NONE = 0,

    /* start record command;
     * The command argument is ignored
     * Example:
     *    recorder_cmd(AR_CMD_START, 0);
     */
    AR_CMD_START,


    /* resume record command;
     * NOTE: this command is ignored
     */
    AR_CMD_RESUME,

    /* pause record command;
     * NOTE: this command is ignored
     */
    AR_CMD_PAUSE,

    /* stop record command;
     * The command argument is ignored
     * Example:
     *    recorder_cmd(AR_CMD_STOP, 0);
     */
    AR_CMD_STOP,

    /* open record command;
     * NOTE: This command is an internal command. APP should not use it
     */
    AR_CMD_OPEN,

    /* close record command;
     * NOTE: This command is an internal command. APP should not use it
     */
    AR_CMD_CLOSE,

    /* config record params;
     * The command argument is a @recorder_stream_params_t pointer
     * Example:
     *    recorder_stream_params_t params;
     *    params.fs = 48000;
     *    params.ch = 2;
     *    params.fmt = AR_STREAM_FMT_ADPCM;
     *    recorder_cmd(AR_CMD_START, (uint32_t)&params);
     */
    AR_CMD_SET_STREAM_PARAMS,


    /* get record error type;
     * The command argument is a uint32_t pointer
     * Example:
     *    uint32_t err_type;
     *    recorder_cmd(AR_CMD_GET_ERROR_TYPE, (uint32_t)&err_type);
     */
    AR_CMD_GET_ERROR_TYPE,


    /* clear record error type;
     * The command argument is ignored
     * Example:
     *    recorder_cmd(AR_CMD_CLEAR_ERROR_TYPE, 0);
     */
    AR_CMD_CLEAR_ERROR_TYPE,
};

enum ar_stream_fmt_e {
    AR_STREAM_FMT_PCM,
    AR_STREAM_FMT_ADPCM,
    AR_STREAM_FMT_MP3,
    AR_STREAM_FMT_MP2,
    AR_STREAM_FMT_SBC,
};

typedef struct recorder_init_params_s {
    uint8_t mode_num;
    uint8_t streamtype;
    void (*record_data_get)(int16_t* buf, uint32_t len);
} recorder_init_params_t;

typedef struct {
    uint32_t fs; /* record data sample rate */
    uint8_t ch;  /* record data channels */
    uint8_t fmt; /* encoder type, see @enum ar_stream_fmt_e */
    uint16_t truncate_time;
} recorder_stream_params_t;

typedef void (*recorder_event_listener_t)(void *recorder, uint32_t event);

/* send a command to the recorder
 * @param cmd: recorder command, see @enum ar_cmd_e
 * @param arg: recorder command argument
 * @return: return zero on success, a negative errno value on failure
 */
int32_t recorder_cmd(uint32_t cmd, uint32_t arg);



/***************************************************************************
 *               recorder API
 ***************************************************************************/
/* 创建一个recorder实例
 * @param params:
 */
void create_recorder(recorder_init_params_t *params);

//销毁一个recorder实例
void destroy_recorder(void);

/* 打开文件和编码器
 * @param src_url: 录音文件路径
 * @return: 成功返回0；错误返回负值错误码
 */
int32_t recorder_open(void *src_url, void *dst_url);

//关闭文件和编码器
void recorder_close(void);

/* 配置录音事件监听器
 * @param listener: 接收录音事件的函数
 * @return: 成功返回0；错误返回负值错误码
 */
int32_t recorder_set_event_listener(recorder_event_listener_t listener);

//获取录音事件（单位：毫秒）
int32_t recorder_get_time(void);

//设置录音数据流参数, p_params为@recorder_stream_params_t 的指针
#define recorder_set_stream_params(p_params) \
    recorder_cmd(AR_CMD_SET_STREAM_PARAMS, (uint32_t)(p_params))

//开始录音
#define recorder_start() recorder_cmd(AR_CMD_START, 1)
//停止录音
#define recorder_stop() recorder_cmd(AR_CMD_STOP, 1)

//获取错误类型；返回值参考@enum ar_err_e
#define recorder_get_error_type() \
    ({ \
        uint32_t err_type; \
        recorder_cmd(AR_CMD_GET_ERROR_TYPE, (uint32_t)&err_type); \
        err_type; \
    })

//清除错误类型
#define recorder_clear_error_type() recorder_cmd(AR_CMD_CLEAR_ERROR_TYPE, 0)

#ifdef __cplusplus
}
#endif


#endif
