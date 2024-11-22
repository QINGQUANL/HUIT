
#include "sys_types.h"



/* single microphone ambient noise and wind noise reduction module header file */


/* ambient noise reduction mask */
#define NSLITE_MASK_ANS   (1<<0)
/* comfort noise generation mask */
#define NSLITE_MASK_CNG   (1<<1)
/* gain smoothing mask */
#define NSLITE_MASK_GSM   (1<<2)


/* init prms for smmw module */
typedef struct
{
    /* sampling frequency */
    uint16_t fs;
    /* frame length in samplels, must be power of 2*/
    uint16_t N;
    /* overlap factor, 2: 50%, 4:75%, other valuse are unallowable */
    uint16_t R;
}nslite_initprm_t;

/* real time prms for smmw module */
typedef struct
{
    /* function mask */
    uint16_t func_mask;
    /* initial ambient noise recursive estimation duration in frames */
    uint8_t finit;
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

}nslite_rtprm_t;

/* close smmw handle */
void nslite_close(void* handle);
/* open smmw handle */
void* nslite_open(nslite_initprm_t* prm);
int nslite_proc_step(void* handle, int16_t* txin, int16_t* txout, uint32_t step);
/* smmw proc function */
int nslite_proc(void* handle, int16_t* txin, int16_t* txout);
/* smmw real time prm setting function */
int nslite_cmd(void* handle, uint32_t cmd, uint32_t arg);