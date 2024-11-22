

#ifndef _VOLUME_CONTROL_H_
#define _VOLUME_CONTROL_H_

#include "sys_inc.h"

#ifdef __cplusplus
extern "C" {
#endif

//音量控制器
typedef struct {
    float amp;          //amp表示当前的线性幅度增益值 amplification = exp(decibels * 0.115129),
                        //例如：对数域0db,对应的线性域增益为amplification = exp(0 * 0.115129) = 1.0

    float alpha;        //amp平滑更新系数，volume_control_init的时候会自行计算好
    float target_volume;//目前音量，volume_control_set_volume写入的值， amp会慢慢更新接近target_volume直到相等
    uint8_t ch;         //声道数
    uint8_t sample_size;//采样精度， 16bit则为2， 32bit则为4
} volume_control_t;

/*------------------------------------------------------------------------------
* 函数功能: 音量控制器初始化
* 函数名称: void volume_control_init(volume_control_t *ctrl, uint32_t fs, uint8_t channels, uint8_t sample_size, float volume)
* 参    数:
*           ctrl: 音量控制器指针
*             fs: 采样率(例如:8000, 16000, 441000, 48000等)
*       channels: 声道数（单声道则配置1， 双声道配置2)
*    sample_size: 采样精度， 16bit则为2， 32bit则为4
*         volume: 初始化音量值(线性域幅度增益); 例如：1.0表示输出原始信号， 0.0表示完全静音
* 返 回 值: 无
* 说明 :本质上是对volume_control_t做初始化
*-------------------------------------------------------------------------------*/
void volume_control_init(volume_control_t *ctrl, uint32_t fs,
    uint8_t channels, uint8_t sample_size, float volume);


/*------------------------------------------------------------------------------
* 函数功能: 音量设置
* 函数名称: void volume_control_set_volume(volume_control_t *ctrl, float volume)
* 参    数:
*           ctrl: 音量控制器指针
*           volume: 需要设置的线性域幅度增益, 例如：1.0表示输出原始信号， 0.0表示完全静音
* 返 回 值: 无
* 说明 : 此函数一般由APP调用
*-------------------------------------------------------------------------------*/
void volume_control_set_volume(volume_control_t *ctrl, float volume);

/*------------------------------------------------------------------------------
* 函数功能: 音量处理函数(输入输出buffer相同)
* 函数名称: void volume_control_proc(volume_control_t *ctrl, void *buf, int32_t samples)
* 参    数:
*           ctrl: 音量控制器指针
*           buf: pcm数据
*           samples: 一帧的采样点数
* 返 回 值: 无
* 说明 : 此函数对PCM数据做处理
*-------------------------------------------------------------------------------*/
void volume_control_proc(volume_control_t *ctrl, void *buf, int32_t samples);

/*------------------------------------------------------------------------------
* 函数功能: 音量处理函数(输入输出buffer不同)
* 函数名称: void volume_control_proc2(volume_control_t *ctrl, void *outbuf, void *inbuf, int32_t samples)
* 参    数:
*           ctrl: 音量控制器指针
*           outbuf: 输出pcm数据buffer
*           inbuf: 输出pcm数据buffer
*           samples: 一帧的采样点数
* 返 回 值: 无
* 说明 : 此函数对PCM数据做处理
*-------------------------------------------------------------------------------*/
void volume_control_proc2(volume_control_t *ctrl, void *outbuf, void *inbuf, int32_t samples);

/*------------------------------------------------------------------------------
* 函数功能: 音量处理函数(混音方式， outbuf数据 = outbuf数据 + inbuf数据 * inbuf音量)
* 函数名称: void volume_control_mix_proc(volume_control_t *ctrl, void *outbuf, void *inbuf, int32_t samples)
* 参    数:
*           ctrl: 音量控制器指针
*           outbuf: 输出pcm数据buffer
*           inbuf: 输出pcm数据buffer
*           samples: 一帧的采样点数
* 返 回 值: 无
* 说明 : 此函数对PCM数据做处理
*-------------------------------------------------------------------------------*/
void volume_control_mix_proc(volume_control_t *ctrl, void *outbuf, void *inbuf, int32_t samples);

#ifdef __cplusplus
}
#endif

#endif /* _VOLUME_CONTROL_H_ */
