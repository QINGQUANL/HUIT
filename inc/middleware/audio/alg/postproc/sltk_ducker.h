#ifndef __SLTK_DUCKER_H__
#define __SLTK_DUCKER_H__

#include "sys_types.h"
#include "sltk_filter.h"
#include "sltk_postproc_common.h"

typedef struct
{
    /* frac bits number */
    int8_t Qbits;
    /* ducking input channels */
    int8_t dking_chnum;
    /* ducked input channels */
    int8_t dked_chnum;
    /* maximum look_ahead in ms*/
    uint8_t max_dly;
    /* sampling frequency */
    int32_t fs;
}duck_initprm_t;

typedef struct
{
    /* attack time in ms */
    uint16_t att_time;
    /* release time in ms */
    uint16_t rel_time;
    /* rms avg time */
    uint16_t avg_time;
    /* look ahead time in ms */
    uint16_t look_ahead;
    /* hold time  in ms */
    uint16_t hold_time;
    /* threshold */
    int8_t  duck_thd;
    /* destination gain */
    int8_t  duck_gain;
}duck_rtprm_t;

/*------------------------------------------------------------------------------
* 函数功能: 创建一个闪避算法实例
* 函数名称: void* ducker_open(duck_initprm_t* prm)
* 参    数:
*           prm: 闪避算法初始化参数
* 返 回 值: 成功返回算法实例句柄，失败返回NULL
* 说明 :
*-------------------------------------------------------------------------------*/
void* ducker_open(duck_initprm_t* prm);

/*------------------------------------------------------------------------------
* 函数功能: 闪避算法处理函数
* 函数名称: int ducker_proc(void* handle, int32_t* dking, int32_t* dked, int32_t samples)
* 参    数:
*           handle: 算法句柄, 即ducker_open的返回值
*           dking: 请求dked避让的音频数据
*           dked: 需要避让的音频数据
*           samples: 一帧处理的点数
* 返 回 值: 成功返回0，失败返回负数
* 说明 :dked音频信号避让dking信号， 如果触发闪避则算法处理的结果是dked信号变小了
*-------------------------------------------------------------------------------*/
int ducker_proc(void* handle, int32_t* dking, int32_t* dked, int32_t samples);

/*------------------------------------------------------------------------------
* 函数功能: 闪避算法cmd函数
* 函数名称: int ducker_cmd(void* handle, uint32_t cmd, uint32_t arg)
* 参    数:
*           handle: 算法句柄, 即ducker_open的返回值
*           cmd: 配置命令
*           arg: 命令参数。具体含义和cmd有关，一般是参数结构体指针
* 返 回 值: 成功返回0，失败返回负数
* 说明 :用来配置实时参数，例如
*       duck_rtprm_t duck_rt = {
*           50,  //att_time
*           200, //rel_time
*           200,//avg_time
*           0,   //look_ahead
*           20, //hold_time
*           -55, //duck_thd
*           -25, //duck_gain
*       };
*       ducker_cmd(duck_handle, SLTK_CMD_AFX_COMMON_SETRTPRM, (uint32_t)&duck_rt);
*-------------------------------------------------------------------------------*/
int ducker_cmd(void* handle, uint32_t cmd, uint32_t arg);

/*------------------------------------------------------------------------------
* 函数功能: 闪避算法退出函数
* 函数名称: void ducker_close(void* handle)
* 参    数:
*           handle: 算法句柄, 即ducker_open的返回值
* 返 回 值: 无
* 说明 :释放此算法使用的资源
*-------------------------------------------------------------------------------*/
void ducker_close(void* handle);


typedef void* (*ducker_lib_open)(duck_initprm_t* prm);
typedef int   (*ducker_lib_proc)(void* handle, int32_t* dking, int32_t* dked, int32_t samples);
typedef int   (*ducker_lib_cmd)(void* handle, uint32_t cmd, uint32_t arg);
typedef void  (*ducker_lib_close)(void* handle);

typedef struct {
    ducker_lib_open  open;
    ducker_lib_proc  proc;
    ducker_lib_cmd   cmd;
    ducker_lib_close close;
} ducker_lib_t;

extern const ducker_lib_t sltk_ducker_lib;
extern const void* ducker_handle;
#define VOID2DUCKER(a) ((ducker_lib_t*)(a))


#endif