#ifndef _SLTK_ADMOMINI_H_
#define _SLTK_ADMOMINI_H_

#include "sltk_math.h"
#include "sltk_filter.h"
#include "sltk_preproc_common.h"
#include "sltk_aenc.h"
#include "sltk_drc.h"

/* adaptive dual mic noise reduction library interface definition */

/* calibration of mic response (magnitude) */
#define ADMOMINI_MASK_CAB      (1<<0)
/* single mic noise suppression */
#define ADMOMINI_MASK_ANS      (1<<1)
/* wind noise reduction (only effective when ans on)  */
#define ADMOMINI_MASK_WNS      (1<<2)
/* comfort noise generation mask */
#define ADMOMINI_MASK_CNG      (1<<3)
/* gain smoothing mask */
#define ADMOMINI_MASK_GSM      (1<<4)
/* automatic gain control mask */
#define ADMOMINI_MASK_AGC      (1<<5)
/* adaptive steering mask */
#define ADMOMINI_MASK_ADP      (1<<6)
/* tx equalizer */
#define ADMOMINI_MASK_EQ       (1<<7)
/* ceps snr smoothing */
#define ADMOMINI_MASK_SMSNR       (1<<8)

typedef struct
{
    /* sampling frequency */
    uint16_t fs;
    /* frame length in samplels, must be power of 2*/
    uint16_t N;
    /* overlap factor, 2: 50%, 4:75%, other valuse are unallowable */
    uint16_t R;
    /* mic distance in mm */
    uint16_t d;
}admomini_initprm_t;

typedef struct
{
    /* function mask */
    uint16_t func_mask;
    /* initial ambient noise recursive estimation duration in frames */
    uint8_t finit;
    /* prms for calibration */
    /* step size for calibration filter */
    float uw;
    /* step size for adaptive beamforming filter */
    float u;
    /* vad thd in dB */
    float vad_thd;
    /* wind onset thd */
    float wind_thd;
    /* smoothing prm for reference psd calculation */
    float alpha_xpsd;
    /* calibration low freq */
    uint16_t cflow;
    /* calibration high freq */
    uint16_t cfhigh;
   /* wind noise detection freq region low */
    uint16_t wflow;
    /* wind noise detection freq region high */
    uint16_t wfhigh;
    /* slow varied calibrated weight low pass cutoff freq */
    uint16_t lpfws;
    /* fast varied calibrated weight low pass cutoff freq */
    uint16_t lpfwf;
    /* prms for ANS */
    /* Ambient noise psd estimatin bias */
    float Npsd_bias;
    /* optimal SNR in dB */
    float pf_Xi_opt;
    /* maximum of allowable SPP */
    float pf_ph1_thd;
    /* ambient noise psd smoothing prm*/
    float pf_alpha_npsd;
    /* SPP smoothing prm */
    float pf_alpha_PH1;
    /* SNR smoothing prm */
    float pf_alpha_Xis;
    /* minimum gain in dB */
    float pf_GHmin;
    /* minimum SNR for signal to ambient noise */
    float SNR_s_Min;
    /* allowable mimum psd */
    float psd_Min;

    /* Wind Noise prms */
    /* recursive wiener filter prm a */
    float Rwa;
    /* recursive wiener filter prm c */
    float Rwc;
 
    /* diff thd */
    //float diff_thd;
     /* decay time after onset false */
    int8_t N_decay;
    /* hold time after onset true */
    int8_t N_hold;

    /* CNG module */
    /* CNG smoothing prm for frequency domain */
    float pf_alpha_Nmin_f;
    /* CNG increment step */
    float pf_inc_Nmin_t;
    /* CNG level */
    float pf_Gcn;
 
    /*gain smoothing module */
    /* up limit for snr to use gain smoothing */
    float gain_smooth_thd;
    /* gain smoothing width in frequency bin num */
    float gain_smooth_width;
    /* calibration thd */
    float cab_thd;
    /* beam prms */
    /* concerned freq region low bound */
    uint16_t bflow;
    /* concerned freq region upper bound */
    uint16_t bfhigh;
    /* beam power smoothing prm */
    float alpha_pb;
    /* beam power decay factor */
    float alpha_decayb;
}admomini_rtprm_t;


/*------------------------------------------------------------------------------
* 函数功能: admomini降噪算法退出
* 函数名称: void admomini_close(void* handle);
* 参    数:
*           handle: 算法句柄（即admomini_open的返回值)
* 返 回 值: 无
* 说明 :此函数释放所有admomini_open时分配的资源
*-------------------------------------------------------------------------------*/
void admomini_close(void* handle);

/*------------------------------------------------------------------------------
* 函数功能: admomini降噪算法初始化
* 函数名称: void* admomini_open(admomini_initprm_t* prm);
* 参    数:
*           admomini_initprm_t: 初始化参数
* 返 回 值: 返回算法句柄
* 说明 :admomoni降噪算法还需要配合admomini_cmd做参数配置. 详细参考Example code
*-------------------------------------------------------------------------------*/
void* admomini_open(admomini_initprm_t* prm);

/*------------------------------------------------------------------------------
* 函数功能: admomini降噪算法处理一帧数据
* 函数名称: int admomini_proc(void* handle, int16_t* xin, int16_t* xout);
* 参    数:
*           handle:  算法句柄（即admomini_open的返回值)
*           xin: 输入数据，要求为2ch,16bit交织存放的pcm数据， 帧长为N个采样点（admomini_open时配置的N参数)
*           xout: 输出数据， 输出一帧格式为1ch,16bit的pcm数据, 帧长为N个采样点（admomini_open时配置的N参数)
* 返 回 值: 成功返回0， 失败返回负数值
* 说明 :
*-------------------------------------------------------------------------------*/
int admomini_proc(void* handle, int16_t* xin, int16_t* xout);

/* admfd real time prm setting function */
/*------------------------------------------------------------------------------
* 函数功能: admomini降噪算法实时参数设置
* 函数名称: int admomini_cmd(void* handle, uint32_t cmd, uint32_t arg);
* 参    数:
*           handle:  算法句柄（即admomini_open的返回值)
*           cmd: cmd类型
*           arg: cmd的参数
* 返 回 值: 成功返回0， 失败返回负数值
* 说明 :
    cmd类型, 以及arg的取值如下
    SLTK_CMD_PP_COMMON_RESET            //reset算法模块， arg未使用
    SLTK_CMD_PP_ADMOMINI_SETRTPRM       //实时参数调节，arg为指向admomini_rtprm_t结构的指针
    SLTK_CMD_PP_ADMOMINI_SETAGCRTPRM    //算法内部的AGC模块的参数调节， arg为指向aenc_drcrtprm_t结构的指针
    SLTK_CMD_PP_ADMOMINI_SETEQRTPRM     //算法内部的EQ模块的参数调节， arg为指向aenc_eqrtprm_t结构的指针
*-------------------------------------------------------------------------------*/
int admomini_cmd(void* handle, uint32_t cmd, uint32_t arg);

////////////////////// Example code //////////////////////////////////
#if 0 //Example code start

#define ADMOMINI_EQ_EN   1       //EQ使能
#define TEST_ADMOMINI_FS (16000) //采样率
#define TEST_ADMOMINI_N  (128)   //帧长

//算法初始化，参数配置
void *admomini_init(void)
{
    void *handle;
    admomini_initprm_t init_prm;
    
    admomini_rtprm_t prms;
    aenc_drcrtprm_t drc_prm;

    init_prm.fs = TEST_ADMOMINI_FS;
    init_prm.N = TEST_ADMOMINI_N;
    init_prm.R = 4;
    init_prm.d = 20;
    handle = admomini_open(&init_prm);
    if (handle == NULL) {
        loge("open enc controller error!");
        return NULL;
    }

    drc_prm.func_mask = MASK_LIMITER /*| MASK_NOISEGATE*/;
    drc_prm.lev_mode = 0;
    drc_prm.look_ahead = 0;
    drc_prm.lt_thd = -15;
    drc_prm.lt_ratio = 2;
    drc_prm.lt_kw = 20;
    drc_prm.lt_att = 3;
    drc_prm.lt_rel = 200;
    drc_prm.ng_thd = -24;
    drc_prm.ng_ratio = 5;
    drc_prm.ng_kw = 20;
    drc_prm.ng_hold = 25;
    drc_prm.ng_att = 1;
    drc_prm.ng_rel = 50;
    drc_prm.peak_att = 1;
    drc_prm.peak_rel = 30;
    drc_prm.makeup_gain = 6;

#if ADMOMINI_EQ_EN
        prms.func_mask = (ADMOMINI_MASK_CAB | ADMOMINI_MASK_ANS /*| ADMOMINI_MASK_WNS */| ADMOMINI_MASK_EQ
                    | ADMOMINI_MASK_GSM /*| ADMOMINI_MASK_CNG */| ADMOMINI_MASK_AGC | ADMOMINI_MASK_ADP);
#else
        prms.func_mask = (ADMOMINI_MASK_CAB | ADMOMINI_MASK_ANS /*| ADMOMINI_MASK_WNS | ADMOMINI_MASK_EQ*/
                    | ADMOMINI_MASK_GSM /*| ADMOMINI_MASK_CNG */| ADMOMINI_MASK_AGC | ADMOMINI_MASK_ADP);
#endif
    //prms.func_mask &= ~(ADMOMINI_MASK_CNG | ADMOMINI_MASK_AGC);
    //prms.func_mask &= ~(ADMOMINI_MASK_ANS|ADMOMINI_MASK_AGC|ADMOMINI_MASK_CNG);
    prms.finit = 50;
    /* step size for adaptive beamforming filter */
    prms.u  = 0.003;
    /* step size for calibration filter */
    prms.uw = prms.u*init_prm.N;
    /* vad thd in dB */
    prms.vad_thd = 0.5;
    /* wind onset thd */
    prms.wind_thd = -.5;
    /* smoothing prm for reference psd calculation */
    prms.alpha_xpsd = 0.04;
    /* calibration low freq */
    prms.cflow=50;
    /* calibration high freq */
    prms.cfhigh=4000;
   /* wind noise detection freq region low */
    prms.wflow=200;
    /* wind noise detection freq region high */
    prms.wfhigh=1000;
    /* slow varied calibrated weight low pass cutoff freq */
    prms.lpfws=2;
    /* fast varied calibrated weight low pass cutoff freq */
    prms.lpfwf=20;

    prms.pf_Xi_opt = 9;//(real_t) 3.1622776601683793;   //pow(10,15./10);
    prms.pf_ph1_thd = (float) .99;
    prms.pf_alpha_npsd = 1 - (float) 0.8;
    prms.pf_alpha_PH1 = 1 - (float) 0.9;
    prms.pf_alpha_Xis = 1 - (float) 0.99;

    /* recursive wiener filter prm a */
    prms.Rwa = (float)1.;
    /* recursive wiener filter prm c */
    prms.Rwc = (float).75;
    /* decay time after onset false */
    prms.N_decay = 10;
    /* hold time after onset true */
    prms.N_hold = 2;

    prms.pf_alpha_Nmin_f = (float)0.7;
    prms.pf_inc_Nmin_t = (float)0.001;
    prms.pf_GHmin = -15;//(real_t) 0.1;   //pow(10,-10./10);
    prms.pf_Gcn = 0;
    prms.Npsd_bias           =   (float).9;    
    prms.gain_smooth_thd     =   (float)0.4;  
    prms.gain_smooth_width   =   (float)5;
    prms.cab_thd = (float)3;
    prms.SNR_s_Min           =   (float)-6;
    prms.psd_Min = -60;

    /* beam prms */
    prms.bflow = 350;
    prms.bfhigh = 1000;
    prms.alpha_pb = (float).25;
    prms.alpha_decayb = (float)0.996;

    if(admomini_cmd(handle, SLTK_CMD_PP_ADMOMINI_SETRTPRM, (uint32_t)&prms)) {
        loge("set aenc real time prms error!");
    }
    if(admomini_cmd(handle, SLTK_CMD_PP_ADMOMINI_SETAGCRTPRM, (uint32_t)&drc_prm)){
        loge("set tx agc real time prms error!");
    }
#if ADMOMINI_EQ_EN
    /* TX EQ 参数 */
    const aenc_eqrtprm_t eqprm =
    {
        0,
        {
            {BIQUAD_PEAKNOTCH, 250, -2, 1.414},
            {BIQUAD_PEAKNOTCH, 500, -4, 1.414},
            {BIQUAD_PEAKNOTCH, 1000, -4, 1.414},
            {BIQUAD_PEAKNOTCH, 2000, -2, 1.414},
            {0,},
        }
    };
    if(admomini_cmd(handle, SLTK_CMD_PP_ADMOMINI_SETEQRTPRM, (uint32_t)&eqprm)) {
        loge("set tx eq real time prms error!");
    }
#endif

    return handle;
}

//降噪处理示例代码(伪代码)
void admomini_test_loop(void)
{
    //算法句柄
    void *admomini_handle;

    admomini_handle = admomini_init();
    inbuf_bytes = TEST_ADMOMINI_N * 2 * sizeof(int16_t);
    outbuf_bytes = TEST_ADMOMINI_N * 1 * sizeof(int16_t);

    while (1) {
        //读双MIC数据
        read_2ch_mic_data(inbuf, inbuf_bytes);
        //降噪处理
        admomini_proc(admomini_handle, inbuf, outbuf);
        //保存算法输出数据
        store_1ch_mic_data(outbuf, outbuf_bytes);
    }

    admomini_close(admomini_handle);
}
#endif //Example code end

#endif //_SLTK_ADMOMINI_H_