#ifndef SLTK_PLATFORM_H_
#define SLTK_PLATFORM_H_

#include "sys_types.h"

#define BIQUAD_LOWSHELVING  0
#define BIQUAD_PEAKNOTCH    1
#define BIQUAD_HIGHSHELVING 2
#define BIQUAD_LOWPASS      3
#define BIQUAD_BANDPASS     4
#define BIQUAD_HIGHPASS     5
#define BIQUAD_ALLPASS      6

/* parametrical biquad filter prms */
typedef struct {
    /* filter type shelf low, shelf high, peak/notch */
    int16_t filter_type;
    /* center frequency(peak/notch) or crossover frequency(shelving filter ) */
    uint16_t fc;
    /* Gain -12~12 dB, Q3 */
    float G;
    /* Qfactor, -64 ~ 64, Q10 */
    float Q_factor;
} biquad_setting_t;

typedef struct {
    /* Numerator */
    float b0;
    float b1;
    float b2;
    /*  denominator, a0 is normalized to 1.0  */
    float a1;
    float a2;
} biquad_coeff_t;

typedef struct {
    int8_t enable;
    float state[2];
    biquad_coeff_t coeff;
} biquad_filter_t;

typedef struct {
    int8_t enable;
    float b1;
    float state;
} ap1st_filter_t;

typedef struct {
    int8_t enable;
    float b1;
    float b2;
    float state[2];
} ap2nd_filter_t;
/*
    二階iir濾波器設計函數
    滤波器的結果保存于biquad_coeff_t* coeff中
*/
void filter_design(void * h_coeff, void * h_prm, int32_t fs);
/*
    同上，滤波器的結果保存于filter中的coeff结构体，
    区别仅仅是biquad_init是filter_design的封装
*/
void biquad_init(biquad_filter_t * filter, biquad_setting_t * prm, int32_t fs);
void biquad_proc(biquad_filter_t * filter, float * input, float * output, int32_t samples);
/*
    直接设置参数接口
        coeff->b0 = b_in[0];
        coeff->b1 = b_in[1];
        coeff->b2 = b_in[2];
        coeff->a1 = a_in[0];
        coeff->a2 = a_in[1];
*/
void biquad_setcoeffs(biquad_filter_t * filter, float * b_in, float * a_in);
void biquad_clear(biquad_filter_t * filter);
/*
    一阶全通滤波器api
*/
void ap1st_init(ap1st_filter_t * filter);
void ap1st_setcoeff(ap1st_filter_t * filter, float b1);
void ap1st_proc(ap1st_filter_t * filter, float * input, float * output, int32_t samples);
void ap1st_clear(ap1st_filter_t * filter);
/*
    二阶全通滤波器api
*/
void ap2nd_init(ap2nd_filter_t * filter);
void ap2nd_setcoeff(ap2nd_filter_t * filter, float b1, float b2);
void ap2nd_proc(ap2nd_filter_t * filter, float * input, float * output, int32_t samples);
void ap2nd_clear(ap2nd_filter_t * filter);

typedef struct
{
    /* block length, must be times of 2 */
    int16_t L;
    /* channels */
    int8_t ch;
}iir_interpdec2_prm_t;

void* sp_iir_interp2_open(iir_interpdec2_prm_t* prm);
void  sp_iir_interp2_close(void* handle);
/* interpolation function,
input and output buffer should be caller-allocated
number of input samples(16bit) should be the same as the open prm L
number of output samples(16bit) will be 2*inputsamples
*/
int   sp_iir_interp2_proc(void* handle, int16_t* input, int16_t* output);
void* sp_iir_dec2_open(iir_interpdec2_prm_t* prm);
void  sp_iir_dec2_close(void* handle);
/* decimation function,
input and output buffer should be caller-allocated
number of input samples(16bit) should be the same as the open prm L
number of output samples(16bit) will be inputsamples
*/
int sp_iir_dec2_proc(void* handle, int16_t* input, int16_t* output);


typedef struct
{
    /* block length, must be times of 3 */
    int16_t L;
    /* channels */
    int8_t ch;
}iir_interpdec3_prm_t;

void* sp_iir_interp3_open(iir_interpdec3_prm_t* prm);
void sp_iir_interp3_close(void* handle);
/* interpolation function,
input and output buffer should be caller-allocated
number of input samples(16bit) should be the same as the open prm L
number of output samples(16bit) will be 2*inputsamples
*/
int sp_iir_interp3_proc(void* handle, int16_t* input, int16_t* output);
void* sp_iir_dec3_open(iir_interpdec3_prm_t* prm);
void sp_iir_dec3_close(void* handle);
/* decimation function,
input and output buffer should be caller-allocated
number of input samples(16bit) should be the same as the open prm L
number of output samples(16bit) will be inputsamples
*/
int sp_iir_dec3_proc(void* handle, int16_t* input, int16_t* output);

#endif