#ifndef _SLTK_ADMFD_H_
#define _SLTK_ADMFD_H_

#include "sys_types.h"
/* adaptive dual mic noise reduction library interface definition */

/* calibration of mic response (magnitude) */
#define ADMFD_MASK_CAB      (1<<0)
/* single mic noise suppression */
#define ADMFD_MASK_ANS      (1<<1)
/* wind noise reduction (only effective when ans on)  */
#define ADMFD_MASK_WNS      (1<<2)
/* comfort noise generation mask */
#define ADMFD_MASK_CNG      (1<<3)
/* gain smoothing mask */
#define ADMFD_MASK_GSM      (1<<4)
/* automatic gain control mask */
#define ADMFD_MASK_AGC      (1<<5)
/* adaptive steering mask */
#define ADMFD_MASK_ADP      (1<<6)
/* tx equalizer */
#define ADMFD_MASK_EQ       (1<<7)
/* internal vad */
#define ADMFD_MASK_INTVAD   (1<<8)
/* snr smoothing */
#define ADMFD_MASK_SMSNR    (1<<9)
/* coh snr estimation and suppression */
#define ADMFD_MASK_COHSNR   (1<<10)
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
    /* mode, 0: front mic first, 1: rear mic first */
    uint8_t mode;
    /* angle of desired voice in degree 0 ~ 90 */
    uint16_t theta;
}admfd_initprm_t;

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
    /* vad prms */
    /* vad inc time in ms */
    int16_t     vadinc_time;
    /* vad dec time in ms */
    int16_t     vaddec_time;
    /* vad noise psd min */
    int16_t     vadNmin;
    /* vad voice psd min */
    int16_t     vadVmin;
    float SNRminCoh;
    int32_t fcoh;
    int32_t fcoh_start;
}admfd_rtprm_t;


/* close admfd handle */
void admfd_close(void* handle);
/* open admfd handle */
void* admfd_open(admfd_initprm_t* prm);
/* admfd proc function */
int admfd_proc(void* handle, int16_t* xin, int16_t* xout);
/* admfd real time prm setting function */
int admfd_cmd(void* handle, uint32_t cmd, uint32_t arg);

#endif