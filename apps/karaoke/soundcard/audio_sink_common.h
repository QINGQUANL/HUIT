#ifndef _AUDIO_SINK_COMMON_H_
#define _AUDIO_SINK_COMMON_H_

#include "sys_inc.h"
#include "ring_buffer.h"
#include "audio_service.h"

#define AUDIO_SINK_PROC_PERIOD  G_AUDIO_PROC_PERIOD

#ifdef __cplusplus
extern "C" {
#endif

enum audio_sink_sta_e {
    AUDIO_SINK_STA_IDLE,
    AUDIO_SINK_STA_ACTIVE,
};

typedef struct {
    struct ring_buffer bufctrl;

    volatile int64_t sink_samples;      //sink采样点
    int64_t base_delta_us;              //基础delta值（us数据量）
    int64_t total_interp_samples;       //总插补采样率

    volatile uint32_t sink_update_t;    //sink更新时间
    uint32_t sink_update_t_th;          //sink更新时间阈值
    int32_t interp_deltaN;              //插补delta值
    uint32_t last_interp_timesamp;      //上次插补时间
    uint32_t fs;
    void *buf;
    reformat_func_t reformat;           //重设格式（立体声变为单声道）

    uint16_t active_th;                 //活动数据量阈值
    uint16_t trigger_th;                //触发读取数据量阈值
    uint16_t period_bytes;              //每组数据量（单位byte）

    uint8_t sample_size;
    uint8_t ch;
    uint8_t input_sample_size;
    uint8_t input_ch;
    uint8_t status;
    uint8_t type;

    int16_t hist[2];        //增加数据时记录最后一个点数据
    int32_t n_cnt;          //增加数据时处理数据数量

    int16_t sub_hist[2];    //减少数据时记录最后一个点数据
    int32_t sub_n_cnt;      //减少数据时处理数据数量
    void *asrc;

    int8_t use_new_interp_flag; //使用新插值标志：0:不使用；1:使用
    int32_t new_interp_th;     //插值阈值：根据是否使用新插值，使用：24；不使用：12
} audio_sink_common_t;

typedef struct {
    void *buf;
    uint32_t fs;

    uint16_t buf_bytes;
    uint16_t trigger_th;        //读取数据阈值
    uint16_t period_bytes;      //每组数据量（单位byte）

    uint8_t sample_size;        //采样精度
    uint8_t ch;                 //声道数
    uint8_t input_sample_size;  //输入采样精度
    uint8_t input_ch;           //输入声道数
    uint32_t sink_update_t_th;  //sink更新时间阈值

    int8_t use_new_interp_flag; //使用新插值标志：0:不使用；1:使用
} audio_sink_init_params_t;

/*------------------------------------------------------------------------------
* 函数功能: sink初始化
* 函数名称: void audio_sink_init(audio_sink_common_t *sink, const audio_sink_init_params_t *params
* 参    数:
*           sink: sink共用数据
*         params：sink初始化参数
* 返 回 值: 无
* 说明 :将params中的参数赋值给sink
*-------------------------------------------------------------------------------*/
void audio_sink_init(audio_sink_common_t *sink,
    const audio_sink_init_params_t *params);

/*------------------------------------------------------------------------------
* 函数功能: sink写数据
* 函数名称: void audio_sink_write(audio_sink_common_t *sink, void *interpbuf, void *buf, uint32_t input_samples)
* 参    数:
*           sink: sink共用数据
*      interpbuf：插补buffer
*           vbuf：需要write的buffer
*  input_samples：输入的采样点
* 返 回 值: 无
* 说明 :无
*-------------------------------------------------------------------------------*/
void audio_sink_write(audio_sink_common_t *sink,
    void *interpbuf, void *buf, uint32_t input_samples);

/*------------------------------------------------------------------------------
* 函数功能: sink读数据
* 函数名称: int audio_sink_read(audio_sink_common_t *sink, void *buf, uint32_t samples)
* 参    数:
*          sink： sink共用数据
*           buf：read出数据的存储buffer
*       samples：输入的采样点
* 返 回 值: 无
* 说明 :无
*-------------------------------------------------------------------------------*/
int audio_sink_read(audio_sink_common_t *sink, void *buf, uint32_t samples);

#ifdef __cplusplus
}
#endif

#endif

