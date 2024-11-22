#ifndef _AENC_BWE_QMF_H_
#define _AENC_BWE_QMF_H_

#include "sltk_preproc_common.h"


void * qmfas_open(int L);
void qmfas_analysis(void* qmf, int16_t* input, int16_t* lout, int16_t* hout);
void qmfas_synthesis(void* qmf, int16_t* lin, int16_t* hin, int16_t* out);
void qmfas_close(void* qmf);

#endif