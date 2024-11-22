/*
 * 移频啸叫抑制算法
 * 喇叭播放的声音被话筒拾音经过内部电路放大再通过喇叭播放，如此周而复始的反馈叠加加强形成啸叫声
 * 移频啸叫抑制算法通过改变频率来破坏成声反馈条件达到啸叫抑制的目的
 * 
 */
#ifndef _SLTK_FREQ_SHIFT_H_
#define _SLTK_FREQ_SHIFT_H_

#include "sys_types.h"
#include "sltk_preproc_common.h"


//移频初始化参数
typedef struct {
    /* channels(只支持1ch) */
    int8_t channels;
    /* Qbits for input, 16bit pcm则Qbits设为15， 32bit pcm则Qbits设为31 */
    int8_t Qbits;
    /* 采样率（如：8000, 16000, 32000, 48000等） */
    int32_t fs;
} fs_prm_t;


//实时参数调节
typedef struct {
    /* shift freq in Hz (0~10Hz) 
     * 移频量越大啸叫抑制效果越好但信号失真大，反之抑制效果弱但信号失真小
     */
    int8_t deltafreq;
} fs_rtprm_t;


/*------------------------------------------------------------------------------
* 函数功能: 移频算法初始化
* 函数名称: typedef void* (*fs_open_t)(fs_prm_t* prm);
* 参    数:
*           fs_prm_t: 初始化参数
* 返 回 值: 返回移频算法句柄
* 说明 :移频算法还需要配合cmd做参数配置. 详细参考Example code
*-------------------------------------------------------------------------------*/
typedef void* (*fs_open_t)(fs_prm_t* prm);

/*------------------------------------------------------------------------------
* 函数功能: 移频算法处理一帧数据
* 函数名称: typedef int (*fs_proc_t)(void* handle, void* input, void* output, int32_t samples);
* 参    数:
*           handle: 移频算法句柄
*           input: 输入pcm数据buffer
*           output: 输出pcm数据buffer
*           samples: 一帧的采样点数
* 返 回 值: 成功返回0, 失败返回负数
* 说明 :
*-------------------------------------------------------------------------------*/
typedef int (*fs_proc_t)(void* handle, void* input, void* output, int32_t samples);


/*------------------------------------------------------------------------------
* 函数功能: 移频算法配置
* 函数名称: typedef int (*fs_cmd_t)(void* handle, uint32_t cmd, uint32_t arg);
* 参    数:
*           handle: 移频算法句柄
*           cmd: 配置命令
*           arg: cmd的参数，不同cmd其arg有不同含义
* 返 回 值: 成功返回0, 失败返回负数
* 说明 :
*    cmd可取的值以及相应的arg如下:
*        SLTK_CMD_PP_COMMON_SETRTPRM //实时参数配置， 对应的arg为fs_rtprm_t*
*        SLTK_CMD_PP_COMMON_RESET   //算法内部reset, arg无意义
*-------------------------------------------------------------------------------*/
typedef int (*fs_cmd_t)(void* handle, uint32_t cmd, uint32_t arg);

/*------------------------------------------------------------------------------
* 函数功能: 移频算法退出
* 函数名称: typedef void (*fs_close_t)(void* handle);
* 参    数:
*           handle: 移频算法句柄
* 返 回 值: 无
* 说明 :
*-------------------------------------------------------------------------------*/
typedef void (*fs_close_t)(void* handle);


/* freq_shift_lib_t定义了移频算法的接口 */
typedef struct {
    fs_open_t open;
    fs_proc_t proc;
    fs_cmd_t cmd;
    fs_close_t close;
} freq_shift_lib_t;

//sltk_freq_shift_normal_lib为标准的移频算法，消耗的CPU资源比较少
extern const freq_shift_lib_t sltk_freq_shift_normal_lib;
//sltk_freq_shift_high_lib为高质量的移频算法，消耗的CPU资源较多
extern const freq_shift_lib_t sltk_freq_shift_high_lib;


////////////////////// Example code //////////////////////////////////
#if 0 //Example code start
void freq_shift_test_loop(void)
{
    const freq_shift_lib_t *fs_lib = &sltk_freq_shift_normal_lib;
    //const freq_shift_lib_t *fs_lib = &sltk_freq_shift_high_lib;
    void *fs_handle;

    //算法初始化
    fs_prm_t fs_prm;
    fs_prm.channels = 1;
    fs_prm.Qbits = 15;
    fs_prm.fs = 48000;
    fs_handle = fs_lib->open(&fs_prm);

    //移频参数配置
    fs_rtprm_t fs_rtprm;
    fs_rtprm.deltafreq = 10;
    fs_lib->cmd(fs_handle, SLTK_CMD_PP_COMMON_SETRTPRM, (uint32_t)&fs_rtprm);
    fs_lib->cmd(fs_handle, SLTK_CMD_PP_COMMON_RESET, 0);

    while (1) {
        samples = 128;
        //读一帧mic数据
        read_mic_data(micbuf, samples)；
        //对MIC pcm数据做移频处理
        fs_lib->proc(fs_handle, micbuf, micbuf, samples);
        //播放mic数据
        play_mic_data(micbuf, samples);
    }

    //移频算法关闭
    fs_lib->close(fs_handle);

}
#endif //Example code end

#endif
