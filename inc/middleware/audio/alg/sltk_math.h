#ifndef SLTK_MATH_H
#define SLTK_MATH_H


#ifdef _MSC_VER
#include "platform.h"
#else
#include "sys_types.h"
void  sltk_imdct(float * input, int nbits);
/*
    浮点实数快速傅里叶变换，不失普遍性，设N = 数据点处理长度
    输入 ：N个实数数据点
    输出 ：N / 2 + 1个复数数据点（共轭决定如此）
    32<= N <= 8192

    @ ：nbit
        N = 2^nbit
    @ : input
        inplace算法模式，input为数据的输入输出buffer
        Buffer长度要求为 N + 2
*/
void  sltk_realfft(float * input, int nbits);
/*
    浮点快速傅里叶反实数变换，不失普遍性，设N = 数据点处理长度
    输入 ：N / 2 + 1个复数数据点（共轭决定如此）
    输出 ：N个实数数据点
    32<= N <= 8192

    @ ：nbit
        log2(N) , N = 数据点长度, N = 2^nbit
    @ : input
        inplace算法模式，input为数据的输入输出buffer
        Buffer长度要求为 N + 2
*/
void  sltk_realifft(float * input, int nbits);
/*
    浮点底2对数函数
*/
float sltk_log2(float x);
/*
    浮点底2指数函数
*/
float sltk_pow2(float x);
/*
    浮点自然指数函数
*/
float sltk_exp(float x);
/*
    浮点自然对数函数
*/
float sltk_log(float x);
/*
    浮点正弦函数
*/
float sltk_sin(float x);
/*
    浮点余弦函数
*/
float sltk_cos(float x);
/*
    浮点正切函数
*/
float sltk_tan(float x);
/*
    前导0位数计数
*/
uint32_t __lzcnt(uint32_t x);
uint16_t av_bswap16(uint16_t x);
uint32_t av_bswap32(uint32_t x);
/* 
    位反转函数
*/
unsigned int bitrev(unsigned int x);
/*
    浮点实数开方函数
*/
float __sqrtf32(float x);
/*
    浮点实数绝对值函数 
*/
float __absf32(float x);

void sweep_gen(int32_t *io, int ch, int32_t samples);
void gen_1Khz(int32_t *io, int ch, int32_t samples);

typedef struct {
    /* sampling frequency */
    int fs;
    /* sweep start frequency */
    int fstart;
    /* sweep end frequency */
    int fend;
    /* sweep length in ms */
    float sweep_time;
    /* fade in time in ms */
    float fadein_time;
    /* fade out time in ms */
    float fadeout_time;
}sltk_sweep_initprm_t;

void* sltk_sweep_open(sltk_sweep_initprm_t* prm);
int sltk_sweep_proc(void* handle, int* outbuffer, int* samples);
void sltk_sweep_close(void* handle);
extern const int16_t primes[646];
#endif

/* saturation 16bit, return int32_t */
#define SLTK_SAT_S32_H(x) ((int32_t)(x) > 32767 ? 32767 :( (int32_t)(x) <-32768 ? -32768 : (int32_t)(x)))
/* ABS */
#define SLTK_ABS_S32(x) (((int)x) > 0 ? ((int)x) : (-((int)x)))
#define SLTK_MIN_S32(a,b) (((int32_t)(a)) > ((int32_t)(b)) ? ((int32_t)(b)):((int32_t)(a)))
#define SLTK_MAX_S32(a,b) (((int32_t)(a)) > ((int32_t)(b)) ? ((int32_t)(a)):((int32_t)(b)))
#define SLTK_MIN_S64(a,b) (((int64_t)(a)) > ((int64_t)(b)) ? ((int64_t)(b)):((int64_t)(a)))

#define SLTK_CONST_LN_2     0.693147180559945f
#define SLTK_CONST_LOG2_E   1.442695040888963f
#define SLTK_CONST_PI       3.141592653589793f
#define SLTK_CONSTANT_SQRT2 1.414213562373095f

#define SLTK_MULT_REAL_REAL(x,y,q)  ((float)((float)(x))*((float)(y)))
#define SLTK_MULT_REAL_MSHR(x,y) ((float)(x))*((float)(y))
#define SLTK_ASHIFTL_REAL(x,q) ((float)(x))
#define SLTK_ASHIFTR_REAL(x,q) ((float)(x))
#define SLTK_FLOAT_TO_REAL(x,q) ((float)(x))
#define SLTK_SAT_REAL_H(x) (((float)(x)) > 32767 ? 32767 : (((float)(x)) < -32768 ?  -32768 : ((float)(x))))

#define SLTK_FAST_MATH_TABLE_SIZE  512
#define SLTK_FAST_MATH_TABLE_SIZE_1_4  128
#define SLTK_FAST_MATH_TABLE_SIZE_1_2  256
#define SLTK_FAST_MATH_TABLE_SIZE_3_4  384

// #define M_PI ((float)3.1415926535897932384626433832795)
#define LOG2_10    3.321928094887362f
#define LOG10_2    0.301029995663981f
#define _20_M_LOG10_2   6.020599913279620f
#define LOG2_10_DIV_20  0.166096404744368f

#endif