#ifndef _SLTK_ADMOMNI_AENC_H_
#define _SLTK_ADMOMNI_AENC_H_

#include "sys_types.h"
#include "sltk_preproc_common.h"
#include "sltk_aenc.h"
#include "sltk_admomini.h"

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
    /* partition number for AEC */
    uint8_t P;
    /* partition number for AES */
    uint8_t L;
    /* partition number for DR */
    uint8_t M;
    /* bulk delay block count */
    uint8_t O;
}admomni_aenc_initprm_t;


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

    /* ### fields for AENC prm ### */
    uint16_t aenc_mask;
    float alpha_slow;
    float alpha_fast;
    /* prms for res */
    float res_overest;
    float alpha_fast_resG;
    float alpha_slow_resG;
    /* prms for double talk detection */
    float dt_thd_norm;
    float dt_thd_limit;
    float dt_init_thd;
    float dt_steprise_decvar;
    float dt_stepfall_decvar;
    /* non-stationary SNR low bound */
    float SNR_ns_Min;
    float alpha_Xins;
    /* rse */
    float rse_maxG;
    uint16_t rse_thd;
    uint16_t ref_thd;
    float dt_overest;
    float init_overest;
}admomni_aenc_rtprm_t;


/* close admomni_aenc handle */
void admomni_aenc_close(void* handle);
/* open admomni_aenc handle */
void* admomni_aenc_open(admomni_aenc_initprm_t* prm);
/* admomni_aenc proc function */
int admomni_aenc_proc(void* handle, int16_t* rin, int16_t* xin, int16_t* rout, int16_t* xout);
/* admomni_aenc real time prm setting function */
int admomni_aenc_cmd(void* handle, uint32_t cmd, uint32_t arg);


#endif