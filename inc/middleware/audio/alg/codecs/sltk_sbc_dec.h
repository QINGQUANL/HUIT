/*
 *
 *  Bluetooth low-complexity, subband codec (SBC) library
 *
 *  Copyright (C) 2008-2010  Nokia Corporation
 *  Copyright (C) 2012-2014  Intel Corporation
 *  Copyright (C) 2004-2010  Marcel Holtmann <marcel@holtmann.org>
 *  Copyright (C) 2004-2005  Henryk Ploetz <henryk@ploetzli.ch>
 *  Copyright (C) 2005-2006  Brad Midgley <bmidgley@xmission.com>
 *
 *
 *  This library is sltk_free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef SLTK_SBC_DEC_H
#define SLTK_SBC_DEC_H

#ifdef __cplusplus
extern "C" {
#endif
#include "sltk_codecs_common.h"

/*
*    描述 ：SBC解码open函数
*    参数 ：
*       @ sltk_codec_init_t* init_prms
*           初始化参数
*
*    返回 ：返回句柄
*/
void*   sbc_decoder_open(sltk_codec_init_t * init_prms);
/*
*    描述 ：SBC解码close函数
*    参数 ：
*       @ void* handle
*           句柄
*
*    返回 ：无
*/
void    sbc_decoder_close(void *handle);
/*
*    描述 ：SBC解码一帧
*    参数 ：
*       @ void* handle
*           句柄
*       @ int16_t *bsbuf
*           码流地址
*       @ uint32_t bs_bytes
*           码流长度
*       @ int16_t **ch0_pcm
*           以指针形式传递给User左声道解码的数据地址，非空就有，空就无
*       @ int16_t **ch1_pcm
*           以指针形式传递给User右声道解码的数据地址，非空就有，空就无
*       @ uint32_t *pcm_samples
*           以指针形式传递给User解码的pcm sample数
*
*    返回 ：解码状态
*/
int32_t sbc_frame_decode(void *handle, int16_t *bsbuf, uint32_t bs_bytes, int16_t **ch0_pcm, int16_t **ch1_pcm, uint32_t *pcm_samples);
/*
*    描述 ：SBC头信息获取函数
*    参数 ：
*       @ void *bsbuf
*           參考數據buffer
*       @ sltk_stream_info_t *info
*           獲取的數據信息放置在此
*
*    返回 ：capture狀態
*/
int32_t sbc_get_stream_info(void *bsbuf, sltk_stream_info_t *info);
/*
*    描述 ：probe SBC一幀有多少個samples
*    参数 ：
*       @ void *bsbuf
*           參考數據buffer
*       @ uint32_t *samples
*           以指針形式傳遞給User一幀有多少個samples
*
*    返回 ：capture狀態
*/
int32_t sbc_get_samples(void *bsbuf, uint32_t *samples);

/*
    DEC Dumy code :
#include "sltk_sbc_dec.h"
#define MAX_SBC_FRAME_LEN (119)

        sltk_stream_info_t info = {0};
        int32_t samples = 128; //Max case
        int32_t dec_init = 0;
        int32_t ret = 0;
        void* decoder = 0;
        int16_t* pcmbuf[2] = {0};
        uint8_t* bs_buf = libc_calloc(MAX_SBC_FRAME_LEN);
        uint8_t* out_pcm = libc_calloc(128*2*sizeof(int16_t));

        while(1) {
            if(!dec_init) {
                fread(bs_buf, 1, MAX_SBC_FRAME_LEN, infile);
                sbc_get_stream_info(bs_buf, &info);
                if (0 == info.chunk_size) {
                    exit(1);
                }
                //Check a little about samples...
                sbc_get_samples(bs_buf, &samples);

                decoder = sbc_decoder_open((sltk_codec_init_t*)&info);
                dec_init = 1;

                fseek(infile, 0, SEEK_SET);
                continue;
            }
            else {
                ret = fread(bs_buf, 1, info.chunk_size, infile);
                if(ret < info.chunk_size) {
                    break;
                }
                ret = sbc_frame_decode(decoder, (int16_t*)bs_buf, info.chunk_size, &pcmbuf[0], &pcmbuf[1], &samples);
                if(ret != 0 && samples == 0) {
                    break;
                }

                noninterleave2interleave(pcmbuf[0], pcmbuf[1], out_pcm, samples);

                fwrite(out_pcm, 2*sizeof(int16_t), samples, outfile);
            }
        }

        //Deinit all ....
*/
#ifdef __cplusplus
}
#endif
#endif                          /* __SBC_H */
