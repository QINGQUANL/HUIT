#ifndef _HEARING_EXCEL_H_
#define _HEARING_EXCEL_H_

#include "sltk_aenc_bwe.h"
#include "sltk_afc.h"

#include "sltk_msdrc.h"

#include "sltk_hpemafc.h"
#include "sltk_howling_notch.h"

#define HE_MAX_WDRC_CHNUM   (16)

/* ambient noise reduction */
#define HE_MASK_ANS     (1<<0)
/* snr smoothing */
#define HE_MASK_SMSNR   (1<<1)
/* gain smooth */
#define HE_MASK_GSM     (1<<2)
/* multichannel drc */
#define HE_MASK_WDRC    (1<<3)
/* comfort noise */
#define HE_MASK_CNG     (1<<4)
/* equalizer */
#define HE_MASK_EQ      (1<<5)
/* afc */
#define HE_MASK_AFC     (1<<6)
/* nhs */
#define HE_MASK_NHS  (1<<7)



#define HE_MAX_NOTCH_NUM    (12)


/* add prms for app */
typedef struct
{
    float a_tune_audio[8];          //调音   
    float a_gain;                   //增音、通透平衡
    float a_tone;                   //音调
    float a_conversation_boost;     //对话增强
    float a_ambient_noise_reduction;//降低环境噪声
}he_appprm_t;

typedef struct 
{
    /* drc prm for the desired channel */
    msdrc_rtprm_t drcprm;
    /* channel index */
    int32_t chidx;
}he_drcprm_t;

typedef struct
{
    /* sampling rate */
    uint32_t fs;
    /* frame length in samples */
    uint16_t N;
    /* decimate factor */
    uint16_t R;
    /* sub-bands number(dft length), must be times of 4 */
    uint16_t B;
    /* filter order, must be times of B */
    uint16_t K;
    /* low complexity flag */
    uint8_t lc;
    /* wdrc channel number */
    uint8_t ch;
    /* drc section number for each channel */
    const uint8_t* sec;
}he_initprm_t;


/* hearing excel de-noise prms */
typedef struct 
{
    /* module mask */
    uint32_t func_mask;
    /* smoothing factor for psd estimation */
    float alpha_psd;
    /* threshold of ph1 */
    float ph1_thd;
    /* init stage time in frame count */
    uint16_t finit;
    /* opt snr */
    float Xi_opt;
    /* smooth prm for final npsd estimation */
    float alpha_npsd;
    /* Npsd estimation bias */
    float Npsd_bias;
    /* smooth prm for PH1 estimation */
    float alpha_PH1;
    /* smooth prm of priori stationary snr */
    float alpha_Xis;
    /* minimum suppress gain */
    float GHmin;
    /* minimum npsd of subband signals */
    float psd_Min;
    /* SNR threshold for ambient noise */
    float SNR_s;
    /* ==== real-time prms for ceps-snr smoothing ==== */
    float smcnst_low;
    float smcnst_mid;
    float smcnst_high;
    float smcnst_pith;
    float smcnst_alpha;
    float cpsnr_bias;
    float cpes_vthd;
    /* ==== floatime prms for gain smoothing ==== */
    float gain_smooth_thd;
    uint16_t gain_smooth_width;
    /* ==== real-time prms for comfort noise generation ==== */
    float alpha_Nmin_f;
    float inc_Nmin_t;
    /* comfort noise gain */
    float Gcn;
}he_nrprm_t;

void* hearingexcel_open(he_initprm_t* prm);

int hearingexcel_proc(void* handle, int16_t* rxin, int16_t* txin, int16_t *txoutlc, int16_t* txout);

int hearingexcel_cmd(void* handle, uint32_t cmd, uint32_t arg);

void hearingexcel_close(void* handle);



#endif