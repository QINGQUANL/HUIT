#ifndef _SLTK_ADPTVAD_H_
#define _SLTK_ADPTVAD_H_

#include "sltk_math.h"
#include "sltk_filter.h"
#include "sltk_preproc_common.h"

typedef enum
{
	/* no active speaker */
	VAD_RET_NOACTIVE = 0,
	/* active speaker presend */
	VAD_RET_ACTIVE = 1,
}vad_ret_t;

/* prms for vad */
typedef struct
{
    /* sampling frequency */
    uint32_t fs;
    /* freq high */
    uint32_t fhigh;
    /* freq low */
    uint32_t flow;
    /* frame length */
    uint32_t N;
    /* mode, 0: freq domain, 1: time domain */
    int32_t mode;

}adptvad_initprm_t;

typedef struct
{
    /* minimum psd of noise in dBfs */
    int32_t LambdaMin;
    /* minimum psd of voice in concerned freq region in dBfs */
    int32_t VoiceMin;
}adptvad_rtprm_t;




void* adptvad_open(adptvad_initprm_t* prms);

vad_ret_t  adptvad_procfd(void* obj, float* pf64Px);

vad_ret_t  adptvad_proctd(void* obj, int16_t* x);

int adptvad_cmd(void *handle, uint32_t cmd, uint32_t cmd_prm);

void  adptvad_close(void* obj);

#endif
