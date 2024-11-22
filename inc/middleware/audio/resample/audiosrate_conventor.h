#ifndef AUDIOSRATE_CONVENTOR_H
#define AUDIOSRATE_CONVENTOR_H

#include "sys_inc.h"

typedef int (*user_xet_data_clbk)(void* user, void* buf, int32_t samples);

enum asrc_quality_e {
    ASRC_QUALITY_LOW,
    ASRC_QUALITY_HIGH,
};

typedef struct {
    //数据回调函数：
    //如果设置为输入回调，则输入数据由clbk获取， 输出数据调用audiosrate_conventor_get_period获取
    //如果设置为输出回调，则输入数据由audiosrate_conventor_set_period写入， 输出数据由clbk抛出
    user_xet_data_clbk clbk;
    void*    user;      //user_xet_data_clbk clbk的第一个参数
    uint32_t srate_in;  //输入采样率
    uint32_t srate_out; //输出采样率
    uint16_t io_period; //一次处理多少个采样点
    uint8_t  ch; //声道数
    uint8_t  sample_size; //采样精度（16bit则sample_size为2）
    uint8_t quality; //重采样质量，参考@enum asrc_quality_e
    bool fast_mode;  //快速模式，线程占用率会更高一些
}asrc_init_param;

int audiosrate_conventor_get_period(void* handle, void* dst);
int audiosrate_conventor_set_period(void* handle, void* src);
void  destroy_audiosrate_conventor(void* handle);
void* create_audiosrate_conventor(asrc_init_param * init);

#endif