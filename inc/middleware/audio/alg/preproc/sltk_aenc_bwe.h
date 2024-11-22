#ifndef _SLTK_AENC_BWE_H_
#define _SLTK_AENC_BWE_H_

#include "sys_types.h"
#include "sltk_preproc_common.h"
#include "sltk_admfd.h"
#include "sltk_admomini.h"
#include "sltk_admomni_aenc.h"
#include "sltk_admaenc.h"
#include "sltk_aenc.h"
#include "sltk_drc.h"

typedef struct
 {
    /* sampling frequency */
    int fs;
    /* block length */
    int16_t N;
    /* 1: single-mic AENC; 2: dual mic ADMAENC */
    int16_t chnum;
    //0 ， 1:消回声 2：双MIC，固定主MIC，主离嘴近， 3，有一个要嘴近
    /* 2ch mode: 0: admaenc, 1: admomni_aenc, 2: admfd, 3: admomni*/
    int8_t mode_2ch;
    /* when chnum=1, use aenc_initprm_t type
       when chnum=2, use initprm type according to mode_2ch
    */
    void* initprm;
} aenc_bwe_initprm_t;


typedef struct
{
    void* rtprm;
    /*high freq snr status*/
    int8_t hfsnr_sta;
}aenc_bwe_rtprm_t;




/* close aenc_bwe handle */
void aenc_bwe_close(void* handle);
/* open aenc_bwe handle */
void* aenc_bwe_open(aenc_bwe_initprm_t* prm);

int aenc_bwe_1ch_proc(void* handle, int16_t* rin, int16_t* xin, int16_t* rout, int16_t* xout);
int aenc_bwe_2ch_proc(void* handle, int16_t* rin, int16_t* xin, int16_t* rout, int16_t* xout);
/* aenc_bwe proc function */
int aenc_bwe_proc(void* handle, int16_t* rin, int16_t* xin, int16_t* rout, int16_t* xout);
/* aenc_bwe real time prm setting function */
int aenc_bwe_cmd(void* handle, uint32_t cmd, uint32_t arg);



#endif