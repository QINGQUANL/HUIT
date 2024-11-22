#ifndef _MIC_I2S_AFC_H_
#define _MIC_I2S_AFC_H_

#include "sys_inc.h"

//dongle端相关函数，负责发送算法处理前的数据以及接收算法处理后的数据
void *dongle_mic_i2s_afc_open(void);
void dongle_mic_i2s_afc_send(void *handle, int32_t *micbuf, int32_t *refbuf, uint32_t samples);
void dongle_mic_i2s_afc_received(void *handle, int32_t *micbuf, uint32_t samples);
void dongle_mic_i2s_afc_close(void *handle);

//dsp端相关函数，负责接收dongle发来的数据做算法处理后回发给dongle
void *dsp_mic_i2s_afc_open(void);
void dsp_mic_i2s_afc_close(void *handle);

#endif //_MIC_I2S_AFC_H_