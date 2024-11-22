#ifndef _SLTK_AFC_H_
#define _SLTK_AFC_H_

#include "sltk_preproc_common.h"
#include "sltk_aenc.h"
/* init prms */
typedef struct {
    uint16_t fs;
    uint16_t Qbits;
    uint16_t N;
    uint8_t P;
    uint8_t O;
    uint8_t Q;

} afc_prm_t;


typedef struct {
    float *hr;
    int32_t *pL;
} afc_irprm_t;



void *afc_open(afc_prm_t * init_prms);

int afc_blockproc(void *enc, int32_t * rxin, int32_t * txin, int32_t * txout);

int afc_cmd(void *enc, uint32_t cmd, uint32_t cmd_prm);

void afc_close(void *enc);



#endif
