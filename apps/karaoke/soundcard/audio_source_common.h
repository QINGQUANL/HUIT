#ifndef _AUDIO_SOURCE_COMMON_H_
#define _AUDIO_SOURCE_COMMON_H_

#include "sys_inc.h"
#include "ring_buffer.h"
#include "audio_service.h"

#define AUDIO_SOURCE_PROC_PERIOD  G_AUDIO_PROC_PERIOD

#ifdef __cplusplus
extern "C" {
#endif

enum audio_source_sta_e {
    AUDIO_SOURCE_STA_IDLE,
    AUDIO_SOURCE_STA_ACTIVE,
};

enum audio_source_type_e {
    AUDIO_SOURCE_TYPE_NORMAL,   //正常类型
    AUDIO_SOURCE_TYPE_INTERP,   //插补类型
};

typedef struct {
    struct ring_buffer bufctrl;

    int64_t total_interp_samples;       //总插补采样点
    volatile int64_t source_samples;    //source采样点
    int64_t base_delta_us;              //基础delta值（us数据量）

    volatile uint32_t source_update_t;  //source更新时间
    int32_t interp_deltaN;              //插补处理delta值
    uint32_t fs;
    uint32_t last_interp_timesamp;      //上次插补时间
    void *buf;

    uint16_t active_th;                 //活动数据量阈值
    uint16_t trigger_th;                //触发读取的数据量阈值
    uint16_t period_bytes;              //每组的数据量（单位为byte）

    uint8_t sample_size;
    uint8_t ch;
    uint8_t status;
    uint8_t type;

    uint8_t overrun_flag;   //over run标志

    int16_t hist[2];        //增加数据时记录处理历史数据
    int32_t hist_res;       //上次数据处理结果：1：增加数据，0：不增加数据
    int32_t n_cnt;          //增加数据时处理数据数量

    int16_t sub_hist[2];    //减少数据时记录处理历史数据
    int32_t sub_n_cnt;      //减少数据时处理数据数量

    void *asrc;
    void *asrc_inbuf;
    void *asrc_outbuf;
    int32_t asrc_out_res;

    int8_t use_new_interp_flag;     //使用新插值标志
    int32_t interp_th;              //插值阈值，使用新插值阈值为24；旧插值阈值为12
} audio_source_common_t;

typedef struct {
    void *buf;
    uint32_t fs;

    uint16_t buf_bytes;
    uint16_t trigger_th;    //读取数据量阈值
    uint16_t period_bytes;  //每组数据量（单位byte）

    uint8_t sample_size;    //采样精度
    uint8_t ch;             //声道数
    uint8_t type;           //source类型：AUDIO_SOURCE_TYPE_NORMAL和AUDIO_SOURCE_TYPE_INTERP

    int8_t use_new_interp_flag;    //使用新插值标志
} audio_source_init_params_t;

/*------------------------------------------------------------------------------
* 函数功能: source初始化
* 函数名称: void audio_source_init(audio_source_common_t *src, const audio_source_init_params_t *params)
* 参    数:
*           src： source共用数据
*         params：source初始化参数
* 返 回 值: 无
* 说明 :将params中的参数赋值给src
*-------------------------------------------------------------------------------*/
void audio_source_init(audio_source_common_t *src,
    const audio_source_init_params_t *params);

/*------------------------------------------------------------------------------
* 函数功能: source写数据
* 函数名称: void audio_sink_init(audio_sink_common_t *sink, const audio_sink_init_params_t *params
* 参    数:
*           src: source共用数据
*           buf：需要写的数据
*       samples：采样点
* 返 回 值: 无
* 说明 :无
*-------------------------------------------------------------------------------*/
void audio_source_write(audio_source_common_t *src, void *buf, uint32_t samples);

/*------------------------------------------------------------------------------
* 函数功能: source读数据
* 函数名称: int audio_source_read(audio_source_common_t *src, void *buf, uint32_t samples)
* 参    数:
*           src: source共用数据
*           buf：read出数据的存储buffer
*       samples：采样点
* 返 回 值: 无
* 说明 :无
*-------------------------------------------------------------------------------*/
int audio_source_read(audio_source_common_t *src, void *buf, uint32_t samples);

#ifdef __cplusplus
}
#endif

#endif

