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
 *  This library is free software; you can redistribute it and/or
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

#ifndef SLTK_SBC_ENC_H_
#define SLTK_SBC_ENC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sys_types.h"

#define SBC_FREQ_16000		0x00
#define SBC_FREQ_32000		0x01
#define SBC_FREQ_44100		0x02
#define SBC_FREQ_48000		0x03

/* blocks */
#define SBC_BLK_4		0x00
#define SBC_BLK_8		0x01
#define SBC_BLK_12		0x02
#define SBC_BLK_16		0x03

/* channel mode */
#define SBC_MODE_MONO		0x00
#define SBC_MODE_DUAL_CHANNEL	0x01
#define SBC_MODE_STEREO		0x02
#define SBC_MODE_JOINT_STEREO	0x03

/* allocation method */
#define SBC_AM_LOUDNESS		0x00
#define SBC_AM_SNR		0x01

/* subbands */
#define SBC_SB_4		0x00
#define SBC_SB_8		0x01

/* data endianess */
#define SBC_LE			0x00
#define SBC_BE			0x01

typedef struct
{
    /*
        采样率 (MSBC时，固定为16000)
    */
	uint8_t frequency;
    /*
        每帧Block数(one block = 8 samples)，4、8、12、16，8缺省。越大，编码一帧的利用率越高 (MSBC时，固定为15)
    */
	uint8_t blocks;
    /*
        子带编码滤波器组数，4、8 (MSBC时，固定为8)
    */
	uint8_t subbands;
    /*
        编码模式，channel mode (MSBC时，固定为mono)
    */
	uint8_t mode;
    /*
        比特池分配方式 (MSBC时，固定为loudness)
    */
	uint8_t allocation;
    /*
        每block bit池容量 (MSBC时，固定为26)
    */
	uint8_t bitpool;
    /*
        是否编码成msbc
    */
	uint8_t msbc;
}sbcenc_prms_t;

/*
*    描述 ：SBC编码open函数
*    参数 ：
*       @ sbcenc_prms_t* prm
*           初始化参数
*
*    返回 ：返回句柄
*/
void* sbc_encode_open(sbcenc_prms_t* prm);
/*
*    描述 ：SBC编码close函数
*    参数 ：
*       @ void* handle
*           句柄
*
*    返回 ：无
*/
void  sbc_encode_close(void* handle);
/*
*    描述 ：SBC编码proc函数
*    参数 ：
*       @ void* handle
*           句柄
*       @ short** input
*           source pcm，16bit，格式为noninterleave，不同声道用不同的地址，所以格式为short**
*       @ int* samples
*           以指针形式传递给encoder source pcm sample数，传递给外部encoder工作一轮之后剩余未编码sample数
*       @ uint8_t** output
*           以指针形式传递给user编码码流的实际地址
*       @ int* outbytes
*           以指针形式传递给user编码码流的长度
*
*    返回 ：编码状态
*/
int   sbc_encode_proc(void* handle, short** input, int* samples, uint8_t** output, int* outbytes);

/*
    ENC Dumy code :
#include "sltk_sbc_enc.h"

        int ret = 0;
        int16_t *pcmbuf[2];
        void* enc = 0;
        uint8_t* enc_out_buf = 0;
        sbcenc_prms_t prm;

        switch (srate) {
        case 44100:
            prm.frequency = SBC_FREQ_44100;
            break;
        case 48000:
            prm.frequency = SBC_FREQ_48000;
            break;
        default:
            prm.frequency = SBC_FREQ_44100;
            break;
        }

        prm.blocks = SBC_BLK_16;
        prm.subbands = SBC_SB_8;
        prm.mode = SBC_MODE_MONO;
        prm.allocation = SBC_AM_LOUDNESS;
        prm.bitpool = 32;
        prm.msbc = 0;

        enc = sbc_encode_open(&prm);
        if (NULL == enc) {
            exit(1);
        }

        int block_true = 4 + prm.blocks * 4;
        pcmbuf[0] = general_calloc(block_true * ch * sizeof(int16_t));
        if (2 == ch) {
            pcmbuf[1] = general_calloc(block_true * ch * sizeof(int16_t));
        }

        while(1) {
            readitems = block_true;
            ret = fread(enc_in_buf, 2*ch, readitems, infile);
            if(ret < readitems) {
                break;
            }

            interleave2noninterleave(enc_in_buf, pcmbuf[0], pcmbuf[1], readitems);

            if (enc) {
                while (readitems) {
                    ret = sbc_encode_proc(enc, pcmbuf, &readitems, &enc_out_buf, &outbytes);
                    if (ret < 0) {
                        break;
                    }
                    fwrite(enc_out_buf, 1, outbytes, outfile);
                }
            }
        }

        //Deinit all .....
*/

#endif /* __SBC_H */
