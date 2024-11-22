#ifndef _SLTK_FADE_H_
#define _SLTK_FADE_H_

#ifdef _MSC_VER
#include "al_types.h"
#else
#include "sys_types.h"
#endif
#include "sltk_codecs_common.h"

#ifdef __cplusplus
extern "C" {
#endif

//淡入淡出类型
enum fade_type_e {
    FADE_IN,         /* 类型：淡入 */
    FADE_OUT,        /* 类型：淡出 */
};

enum fade_status_e {
    FADE_STA_RESET,        //当前状态：重置(复位)
    FADE_STA_IN_ENDED,     //当前状态：淡入结束
    FADE_STA_IN,           //当前状态：正在淡入
    FADE_STA_OUT_ENDED,    //当前状态：淡出结束
    FADE_STA_OUT,          //当前状态：正在淡出
};

typedef struct {
    float gain;               //gain表示当前的淡入淡出增益值
    float alpha;              //gain平滑系数
    uint32_t rate;            //采样率(如：8000， 16000， 44100， 48000)

    uint8_t sample_size;      //采样精度：16bit为2，32bit为4
    uint8_t ch;               //声道数
    uint8_t type;             //淡入淡出类型, 取值参考@enum fade_type_e
    uint8_t status;           //当前淡入淡出的状态， 取值参考@enum fade_status_e
} audio_fade_t;

/*------------------------------------------------------------------------------
* 函数功能: 淡入淡出重置
* 函数名称: void fade_reset(audio_fade_t *fade)
* 参    数:
*           audio_fade_t: 淡入淡出控制器指针
* 返 回 值: 无
* 说明 :将淡入淡出控制器内容清空
*-------------------------------------------------------------------------------*/
void fade_reset(audio_fade_t *fade);

/*------------------------------------------------------------------------------
* 函数功能: 淡入淡出配置
* 函数名称: fade_config(audio_fade_t *fade, uint32_t rate, uint32_t sample_size, uint32_t ch)
* 参    数:
*           audio_fade_t: 淡入淡出指针
*            rate: 采样率
*            sample_size: 采样精度：16bit则为2,32bit为4
*            ch: 声道数（单声道配置1，双声道配置2）
* 返 回 值: 无
* 说明 :实际上是对audio_fade_t进行初始化
*-------------------------------------------------------------------------------*/
void fade_config(audio_fade_t *fade, uint32_t rate, uint32_t sample_size, uint32_t ch);

/*------------------------------------------------------------------------------
* 函数功能: 开始淡入淡出
* 函数名称: void fade_start(audio_fade_t *fade, uint32_t type, uint32_t fade_ms)
* 参    数:
*           fade: 淡入淡出控制指针
*           type: 淡入淡出类型
*        fade_ms: 淡入淡出的时间（一般为30ms等）
* 返 回 值: 无
* 说明 :对audio_fade_t进行设置（即设置类型（淡入、淡出）、淡入淡出的时间）
*-------------------------------------------------------------------------------*/
void fade_start(audio_fade_t *fade, uint32_t type, uint32_t fade_ms);

/*------------------------------------------------------------------------------
* 函数功能: 淡入淡出处理函数
* 函数名称: void fade_proc(audio_fade_t *fade, void *buf, uint32_t samples)
* 参    数:
*           fade: 淡入淡出控制器指针
*            buf: pcm数据
*          sample: 一帧的采样点数
* 返 回 值: 无
* 说明 :对交织方式的PCM数据进行淡入淡出处理
*-------------------------------------------------------------------------------*/
void fade_proc(audio_fade_t *fade, void *buf, uint32_t samples);

/*------------------------------------------------------------------------------
* 函数功能: 分离式双声道淡入淡出处理函数
* 函数名称: void fade_proc_deinterleave(audio_fade_t *fade, void *buf[], uint32_t samples)
* 参    数:
*           fade: 淡入淡出控制器指针
*          buf[]: pcm buffer数组
*          sample: 一帧的采样点数
* 返 回 值: 无
* 说明 :对分离式双声道的PCM数据进行淡入淡出处理
*-------------------------------------------------------------------------------*/
void fade_proc_deinterleave(audio_fade_t *fade, void *buf[], uint32_t samples);

/*------------------------------------------------------------------------------
* 函数功能: 获取当前淡入淡出状态
* 函数名称: uint32_t fade_get_status(audio_fade_t *fade)
* 参    数:
*           fade: 淡入淡出控制器指针
* 返 回 值: 当前淡入淡出状态, 取值参考@enum fade_status_e
* 说明 :获取当前音频淡入淡出状态
*-------------------------------------------------------------------------------*/
uint32_t fade_get_status(audio_fade_t *fade);

/*------------------------------------------------------------------------------
* 函数功能: 设置当前淡入淡出状态
* 函数名称: void fade_set_status(audio_fade_t *fade, uint8_t sta)
* 参    数:
*           fade: 淡入淡出控制器指针
*            sta: 需要设置的状态, 取值参考@enum fade_status_e
* 返 回 值: 无
* 说明 :修改当前音频淡入淡出状态
*-------------------------------------------------------------------------------*/
void fade_set_status(audio_fade_t *fade, uint8_t sta);

#ifdef __cplusplus
}
#endif

#endif
