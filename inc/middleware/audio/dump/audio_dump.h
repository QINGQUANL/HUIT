#ifndef _AUDIO_DUMP_H_
#define _AUDIO_DUMP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sys_inc.h"
#include "sltk_audio.h"

//dump设备类型
enum audio_dump_device_e {
    AUDIO_DUMP_DEVICE_SPDIF,    //使用spdif抓数据
    AUDIO_DUMP_DEVICE_USB,      //使用USB抓数据
};

//dump类型，表明采样率，声道数和采样位深(特别注意24bit数据实际上是32bit的高24bit)
enum audio_dump_type_e {
    AUDIO_DUMP_TYPE_192K_2CH_24BIT,
    AUDIO_DUMP_TYPE_192K_2CH_16BIT,
    AUDIO_DUMP_TYPE_48K_2CH_24BIT,
    AUDIO_DUMP_TYPE_48K_2CH_16BIT,
    AUDIO_DUMP_TYPE_44K100_2CH_24BIT,
    AUDIO_DUMP_TYPE_44K100_2CH_16BIT,
    AUDIO_DUMP_TYPE_32K_2CH_24BIT,
    AUDIO_DUMP_TYPE_32K_2CH_16BIT,
    AUDIO_DUMP_TYPE_16K_6CH_16BIT,
    AUDIO_DUMP_TYPE_8K_6CH_16BIT,
    AUDIO_DUMP_TYPE_48K_6CH_16BIT, //SPDIF不支持
};

/*------------------------------------------------------------------------------
* 函数功能: dump pcm数据初始化
* 函数名称: void audio_pcm_dump_device_open(uint8_t dump_device, uint8_t dump_type, uint32_t period);
* 参    数:
*           dump_device: 使用什么抓数据, 取值参考@enum audio_dump_device_e
*           dump_type: dump类型，表明采样率，声道数和采样位深，具体值参考@enum audio_dump_type_e
*           period: 表示一次dump多少个采样点的数据.period决定了
                audio_pcm_dump_deinterleave(或者audio_pcm_dump_interleave)函数应传入的数据量
* 返 回 值: 无
* 说明 : 此函数完成硬件模块（不包含IO口的配置）的初始化，数据buffer初始化
*-------------------------------------------------------------------------------*/
void audio_pcm_dump_device_open(uint8_t dump_device, uint8_t dump_type, uint32_t period);
#define audio_pcm_dump_open(dump_type, period) \
    audio_pcm_dump_device_open(AUDIO_DUMP_DEVICE_SPDIF, dump_type, period)

/*------------------------------------------------------------------------------
* 函数功能: dump pcm数据退出
* 函数名称: void audio_pcm_dump_close(void);
* 参    数: 无
* 返 回 值: 无
* 说明 : 此函数完成硬件模块（不包含IO口的配置）的退出，数据buffer释放
*-------------------------------------------------------------------------------*/
void audio_pcm_dump_close(void);

/*------------------------------------------------------------------------------
* 函数功能: 抓数据功能（传入数据为分离模式（各个声道数据独立存放）的数据）
* 函数名称: void audio_pcm_dump_deinterleave(void *ch0, void *ch1, void *ch2, void *ch3, void *ch4, void *ch5);
* 参    数:
*           ch0: 通道0的数据buffer, 如传入NULL, 则表示此通道无数据，函数内部自动填0数据
*           ....
*           ch5: 通道5的数据buffer, 如传入NULL, 则表示此通道无数据，函数内部自动填0数据
* 返 回 值: 无
* 说明 :
*-------------------------------------------------------------------------------*/
void audio_pcm_dump_deinterleave(void *ch0, void *ch1, void *ch2, void *ch3, void *ch4, void *ch5);

/*------------------------------------------------------------------------------
* 函数功能: 抓数据功能（传入数据为交织模式（各个声道数据交织存放于一个buffer中）的数据）
* 函数名称: void audio_pcm_dump_interleave(void *data, uint32_t ch_count);
* 参    数:
*           data: pcm数据buffer
*           ch_count: 实际数据通道数，ch_count必须小于或者等于audio_pcm_dump_open时指定的声道数
* 返 回 值: 无
* 说明 : 交织模式数据存放方式为：ch0,ch1,...,chn, ch0,ch1,...,chn,  ch0,ch1,...,chn
*-------------------------------------------------------------------------------*/
void audio_pcm_dump_interleave(void *data, uint32_t ch_count);

/*------------------------------------------------------------------------------
* 函数功能: 读取dump buffer中的数据（由USB等dump设备调用)
* 函数名称: void audio_pcm_dump_data_read(void *data, uint32_t bytes);
* 参    数:
*           data: 放pcm数据的buffer
*           bytes: 要读走多少bytes数据
* 返 回 值: 无
* 说明 :
*-------------------------------------------------------------------------------*/
void audio_pcm_dump_data_read(void *data, uint32_t bytes);

#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_DUMP_H_ */
