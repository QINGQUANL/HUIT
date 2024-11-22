#ifndef _SLTK_EQ_H_
#define _SLTK_EQ_H_

#include "sys_types.h"
#include "sltk_filter.h"
#include "sltk_postproc_common.h"

/*
 * software interface
 */
typedef int32_t eq_sample_t;

#define MUSIC_TYPE_POP          0
#define MUSIC_TYPE_ROCK         1
#define MUSIC_TYPE_Jazz         2
#define MUSIC_TYPE_CLASSICAL    3
#define MUSIC_TYPE_DANCE        4
#define MUSIC_TYPE_COUNTRY      5

typedef struct {
    /*
     * saturation flag, if the eq is followed by limiter(like the one
     * embedded in DRC), then should be set to 0 to maintain the dynamic
     * range as much as possible
     */
    int8_t need_saturation;
    /*
     * frac bits for input and output sample, Qbits == 15 for short input,
     * 15 < Qbits < 32 for 32bit(int) input the eq process will keep the
     * same Qbits in the output, if the eq will boost the input, it is
     * strongly suggested that use a headroom for the internal dynamic
     * range, that is use Qbits <= 24 for
     */
    int8_t Qbits;
    /*
     * channel count
     */
    int8_t channels;
    /*
     * sampling frequency
     */
    int32_t fs;
} eq_initprm_t;

typedef struct {
    eq_initprm_t eq_initprm;
    uint32_t period;
    uint8_t support_coeff_update;
} hw_eq_initprm_t;

typedef struct {
    /*
     * 0 for left channel, 1 for right channel
     */
    int8_t ch;
    /*
     * band idx, 0 to MAX_EQ_BANDS -1
     */
    int8_t band;
    /*
     * filter settings
     */
    biquad_setting_t setting;
    /* disable */
    uint8_t disable;
} eq_sectprm_t;

typedef struct {
    /*
     * 0 for left channel, 1 for right channel
     */
    int8_t ch;
    /*
     */
    float PreAmp;

} eq_pampprm_t;

/*
 * realtime prms all in one
 */
typedef struct {
    float PreAmp[2];
    biquad_setting_t setting[2][14];
} eq_rtprm_t;

typedef struct
{
    float* coeffs[2];
    int32_t coeff_items[2];
}eq_batchcoeff_t;


/*
    dummy code:

----> Eq调音区域，初始化，控制阶段都需要Load该区域
    用户通过改变此区域的值并且调用eq_cmd来实现对EQ算法的控制
const biquad_setting_t eq_rt_bands[MAX_EQ_BANDS]={
    { BIQUAD_PEAKNOTCH, 25,    0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 50,    0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 75,    0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 150,   0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 300,   0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 600,   0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 1000,  0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 2000,  0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 3000,  0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 6000,  0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 8000,  0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 10000, 0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 12000, 0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 16000, 0.0, 0.75 }
};

const bool eq_rt_bands_disable[MAX_EQ_BANDS] = {
    false, false, false, false, false, false, false, false, false, false
};

const float eq_rt_channel_preamp = 0.0f;

Hw eq sram只能使用sram7

----> Open阶段
#include "sltk_eq.h"
    eq_initprm_t init;
    //通道数
    init.channels        = ch;
    //采样率
    init.fs              = fs;
    //是否内部做Qbit饱和
    init.need_saturation = 1;
    //sample_size是输入输出数据比特位精度(byte)，一般默认32bit(4 byte)，但是16bit算法是支持的
    //但是头文件写死了32bit，还是按规矩来吧，最好IO都是32bit
    init.Qbits           = 8 * samples_size - 1;

#if USE_HWEQ
    hw_eq_initprm_t hw_eq_initprm;
    hw_eq_initprm.eq_initprm = init;
    //一次处理吞吐量，单位sample
    hw_eq_initprm.period = period;
    void *eqh = hw_eq_open(&hw_eq_initprm);
#else
    void *eqh = eq_open(&init);
#endif
    if(eqh) {

----> 初始化阶段
    eq_sectprm_t sec;
    eq_pampprm_t preamp;
    for(int ch = 0; ch < 2; ch++) {
        sec.ch = ch;
        for(int b = 0; b < 14; b++) {
            sec.band = b;
            sec.disable = eq_rt_bands_disable[b];
            libc_memcpy(&sec.setting, &eq_rt_bands[b], sizeof(biquad_setting_t));
#if USE_HWEQ
            hw_eq_cmd(eqh, SLTK_CMD_AFX_EQ_SETSECT, (uint32_t)(&sec));
#else
            eq_cmd(eqh, SLTK_CMD_AFX_EQ_SETSECT, (uint32_t)(&sec));
#endif
        }
        preamp.ch = ch;
        preamp.PreAmp = eq_rt_channel_preamp;
#if USE_HWEQ
        hw_eq_cmd(eq->eqh, SLTK_CMD_AFX_EQ_SETPREAMP, (uint32_t)(&preamp));
#else
        eq_cmd(eq->eqh, SLTK_CMD_AFX_EQ_SETPREAMP, (uint32_t)(&preamp));
#endif
    }

----> 处理阶段
        while(1) {
            int ret = fread(buffer, 4*ch, samples, instream);
            if(ret < samples) break;

            //EQ的控制cmd一定要和eq_process串行，或者通过信号量来控制以保护eq算法内部逻辑不出混乱
            recive_cmd(eqh);
#if USE_HWEQ
            hw_eq_process(eqh, buffer, buffer, ret);
#else
            eq_process(eqh, buffer, buffer, ret);
#endif
            fwrite(buffer, 4*ch, ret, outstream);
        }
#if USE_HWEQ
        hw_eq_close(eqh);
#else
        eq_close(eqh);
#endif
    }
    exit(0);

----> eq_cmd的例子
void recive_cmd(void* handle, ...)
{
    用户自己修改 eq_rt_bands
    用户自己修改 eq_rt_bands_disable
    用户自己修改 eq_rt_channel_preamp

    //以下把已经修改的eq_rt_bands或者eq_rt_bands_disable或者eq_rt_channel_preamp同步到eq实时算法中
    //如果明确知道修改了那个ch，那个band，可以不必全遍历
    eq_sectprm_t sec;
    eq_pampprm_t preamp;
    for(int ch = 0; ch < 2; ch++) {
        sec.ch = ch;
        for(int b = 0; b < 14; b++) {
            sec.band = b;
            sec.disable = eq_rt_bands_disable[b];
            libc_memcpy(&sec.setting, &eq_rt_bands[b], sizeof(biquad_setting_t));
#if USE_HWEQ
            hw_eq_cmd(eqh, SLTK_CMD_AFX_EQ_SETSECT, (uint32_t)(&sec));
#else
            eq_cmd(eqh, SLTK_CMD_AFX_EQ_SETSECT, (uint32_t)(&sec));
#endif
        }
        preamp.ch = ch;
        preamp.PreAmp = eq_rt_channel_preamp;
#if USE_HWEQ
        hw_eq_cmd(eq->eqh, SLTK_CMD_AFX_EQ_SETPREAMP, (uint32_t)(&preamp));
#else
        eq_cmd(eq->eqh, SLTK_CMD_AFX_EQ_SETPREAMP, (uint32_t)(&preamp));
#endif
    }
}

*/
void *eq_open(eq_initprm_t * prm);
int   eq_process(void *handle, eq_sample_t * x, eq_sample_t * y, int32_t samples);
int   eq_cmd(void *handle, uint32_t cmd, uint32_t arg);
void  eq_close(void *handle);

void *hw_eq_open(hw_eq_initprm_t * prm);
int   hw_eq_process(void *handle, void * x, void * y, int32_t samples);
int   hw_eq_cmd(void *handle, uint32_t cmd, uint32_t arg);
void  hw_eq_close(void *handle);

#endif
