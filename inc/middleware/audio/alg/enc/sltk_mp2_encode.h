/*
 * copyright (c) 2001 Fabrice Bellard
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * mpeg audio declarations for both encoder and decoder.
 */

#ifndef SLTK_MP2ENC_H_
#define SLTK_MP2ENC_H_

#include "sltk_enc_common.h"

#define MP2_PCM_FRAME_SZ    (1152)

//mp2编码参数
typedef struct {
    /* 采样率:支持16000, 22050, 24000, 32000, 44100, 48000 */
    int sample_rate;

    /* 比特率(bps)
     * 如果采样率为: 44100, 48000, 32000, 则可选的比特率有：
     *       32000, 48000, 56000, 64000, 80000, 96000, 112000, 128000, 160000, 192000, 224000, 256000, 320000, 384000
     * 如果采样率为: 22050, 24000, 16000, 则可选的比特率有：
     *       8000, 16000, 24000, 32000, 40000, 48000, 56000, 64000, 80000, 96000, 112000, 128000, 144000, 160000
     */
    int bit_rate;

    /* 声道数, 1 or 2 */
    int channels;
} mp2enc_prm_t;

//callback方式
void *mp2enc_open(encoder_init_t * init);
int mp2enc_proc(void *handle);
int mp2enc_cmd(void *handle, uint32_t cmd, uint32_t arg);
void mp2enc_close(void *handle);


/*------------------------------------------------------------------------------
* 函数功能: mp2编码器初始化
* 函数名称: void *sltk_mp2enc_open(mp2enc_prm_t *prm);
* 参    数:
*           prm: 初始化参数
* 返 回 值: 返回编码器句柄
* 说明 : mp2编码器支持16bit pcm输入，因此初始化参数中无采样位深参数
*-------------------------------------------------------------------------------*/
void *sltk_mp2enc_open(mp2enc_prm_t *prm);

/*------------------------------------------------------------------------------
* 函数功能: mp2编码处理函数
* 函数名称: int32_t sltk_mp2enc_proc(void *handle, void **p_in_pcmbuf, uint32_t *p_in_samples, uint8_t **p_out_bsbuf, uint32_t *p_out_bytes);
* 参    数:
*           handle: 编码器句柄
*           p_in_pcmbuf: 输入buffer的指针， *p_in_pcmbuf在处理后会发生变化（表示数据被处理）
*           p_in_samples：输入采样点数的指针, *p_in_samples在处理后会变小，
*                         表示剩余待处理的PCM数据点数, 当*p_in_samples减小到0则表明所有输入数据都被处理了
*           p_out_bsbuf: *p_out_bsbuf表示编码输出码流（bitstream)buffer
*           p_out_bytes: *p_out_bytes表示输出码流数据量（bytes)
* 返 回 值: 成功返回0，失败返回负数
* 说明 : 调用者应循环调用此函数，直到输入数据完全被处理才算一帧完成，编码器内部凑够11523个点才会输出数据。
         特别的，当输入一帧点数为1152时，调用一次即产生一帧输出。
*-------------------------------------------------------------------------------*/
int32_t sltk_mp2enc_proc(void *handle, void **p_in_pcmbuf, uint32_t *p_in_samples, uint8_t **p_out_bsbuf, uint32_t *p_out_bytes);

/*------------------------------------------------------------------------------
* 函数功能: mp2编码器退出
* 函数名称: void sltk_mp2enc_close(void *handle);
* 参    数:
*           handle: 编码器句柄
* 返 回 值: 无
* 说明 :
*-------------------------------------------------------------------------------*/
void sltk_mp2enc_close(void *handle);


////////////////////// Example code //////////////////////////////////
#if 0 //Example code start

//mp2编码示例代码(伪代码)
void mp2enc_test_loop(void)
{
    void *mp2enc_handle;
    mp2enc_prm_t prm;
    prm.sample_rate = 48000;
    prm.bit_rate = 192000;
    prm.channels = 2;
    mp2enc_handle = sltk_mp2enc_open(&prm);

    while (1) {
        //samples = MP2_PCM_FRAME_SZ; //一次给一个mp2帧
        samples = 128; //一次给128个点

        //读取pcm数据
        read_pcm_data(inbuf, samples);

        void *in_pcmbuf = inbuf;
        uint32_t in_samples = samples;
        uint8_t *out_bsbuf;
        uint32_t out_bytes;
        //循环处理直到输入被处理完
        while (in_samples) {
            //编码处理
            sltk_mp2enc_proc(mp2enc_handle, &in_pcmbuf, &in_samples, &out_bsbuf, &out_bytes);
            if (out_bytes) {
                //保存编码后的数据
                store_bsbuffer(out_bsbuf, out_bytes);
            }
        }
    }

    sltk_mp2enc_close(mp2enc_handle);
}
#endif //Example code end

#endif /* AVCODEC_MPEGAUDIO_H */
