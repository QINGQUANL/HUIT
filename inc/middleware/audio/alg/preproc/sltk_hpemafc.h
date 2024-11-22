#ifndef _HPEMAFC_H_
#define _HPEMAFC_H_

#include "sltk_preproc_common.h"

/* soft clipper */
#define HPEMAFC_MASK_SC     (1<<0)
/* hybrid adaptive mode */
#define HPEMAFC_MASK_HBD    (1<<1)
/* using prediction err to update filter */
#define HPEMAFC_MASK_PEM    (1<<2)
/* improved variable step size control */
#define HPEMAFC_MASK_IPVSS  (1<<3)
/* residual-echo based step size control */
#define HPEMAFC_MASK_RPVSS  (1<<4)
/* use ltp control */
#define HPEMAFC_MASK_LTP  	(1<<5)
/* use low pass filter control */
#define HPEMAFC_MASK_LPF  	(1<<6)

typedef struct
{
    /* 采样率，单位：Hz */
    uint32_t fs;
    /* 帧长，每次处理的样本点数*/
    uint16_t N;
    /* 声学反馈路径的建模长度（帧数），即声学反馈路径建模总体对应的时间为 N*P/fs 秒 */
    uint16_t P;
    /* FFT长度，必须满足B >= 2*N, B越大，频域分辨率越好，但是计算量也会越大*/
    uint16_t B;
    /* 大块纯延时的长度（帧数），即算法内部会自行补偿这部分大段的纯延时，而不会用滤波器来建模，对应的时间为 N*O/fs 秒*/
    uint16_t O;
    /* 用于内部音频预测的滤波器阶数，建议保持默认值 */
    uint16_t Q;
    /* 更新内部音频滤波器的时间周期，单位：秒，建议保持默认值*/
    uint16_t L;
    /* 建模的声学反馈路径的零响应部分长度（帧数），一般来自于内部 FIFO、DMA BUffer等带来的延时，建议保持默认值*/
    uint16_t PO;

    /* 以下参数暂不支持修改 */
    /* ltp 标志位;是否使用ltp */
    uint8_t ltp;
    /* ltp参数更新周期（ms） */
    uint8_t PI;
    /* 计算校正周期（ms） */
    uint32_t Lt;
    /* 最大基本频率（HZ） */
    uint16_t fmax;
    /* 最小基本频率（HZ） */
    uint16_t fmin;
}hpemafc_initprm_t;

typedef struct
{
    /* 使用的函数功能 */
    uint32_t func_mask;
    /* SC功能参数(soft clipper prm) */
    float beta;
    /* PEM常量步长（不使用ipvss有效） */
    float u1;
    /* nlms常量步长（使用ipvss有效） */
    float u2;
    /* ipvss步长的上限（func_mask开启了RPVSS功能则此参数无效） */
    float uH;
    /* ipvss步长的下限（func_mask开启了RPVSS功能则此参数无效） */
    float uL;
    /* rpvss步长的上限（func_mask需开启RPVSS功能） */
    float uoH;
    /* rpvss步长的下限（func_mask需开启RPVSS功能） */
    float uoL;
    /* threshold for hybrid determination(使用默认值) */
    float gamma;
    /* 最小的信号功率值（dBFS）*/
    float minPsd;
    /* 最小反馈增益(db) */
    float minH;
    /* 低通滤波器频点，小于fc的信号成分不参与自适应AFC算法运算，所以不会被影响 */
    uint32_t fc;
}hpemafc_rtprm_t;

/* IR prm struct */
typedef struct {
    /* buffer for impulse reponse store */
    float *hr;
    /* length pointer */
    int32_t *pL;
} hpemafc_irprm_t;

typedef struct
{
    /* feedback impulse response */
    const float* h;
    /* length of filter */
    int32_t Lh;
    /* loopback gain in dB */
    float G;
}hpemafc_fbprm_t;

void* hpemafc_open(hpemafc_initprm_t* prm);

int hpemafc_proc(void* handle, int16_t * rxin, int16_t * txin, int16_t * txout);

int hpemafc_cmd(void* handle, uint32_t cmd, uint32_t arg);

void hpemafc_close(void* handle);


#endif