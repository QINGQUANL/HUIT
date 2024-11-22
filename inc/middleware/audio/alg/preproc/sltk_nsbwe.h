#ifndef _NSBWE_H_
#define _NSBWE_H_
//#include "sltk_types.h"
//#include "preproc_common.h"
#include "sltk_bwe_qmf.h"


/* noise suppression mask */
#define NSBWE_MASK_NS       (1<<0)

typedef struct 
{
    /* sampling rate */
    uint32_t fs;
    /* frame length */
    uint16_t N;
    /* overlap factor, 2: 50%, 4:75%*/
    uint16_t R;
    /* split lpf/hpf for independent processing ? */
    uint16_t split;

}nsbwe_initprm_t;


typedef struct 
{
    /* func mask */
    uint32_t func_mask;
    /* init frame for npsd estimation */
    uint16_t finit;
    /* psd estimation smoothing prm */
    float alpha_npsd;
    /* snr smoothing prm */
    float alpha_xis;
    /* minimum gain */
    float GHmin;
    /* minimum psd in dBFs */
    float Psdmin;       /*-100-0*/
    /* npsd bias */
    float Npsd_bias;
    /* SNR minimum */
    float SNR_s_min;
    /* vad deterimin threshold in dBFs */
    float vthd;     /*-100-0*/
}nsbwe_rtprm_t;

void* nsbwe_open(nsbwe_initprm_t* prms);
int nsbwe_proc(void* handle, int16_t* input, int16_t* output);
int nsbwe_cmd(void* handle, uint32_t cmd, uint32_t arg);
void nsbwe_close(void* handle);

typedef void *(*nsbwe_open_t)(nsbwe_initprm_t * prms);
typedef int (*nsbwe_proc_t)(void* handle, int16_t* input, int16_t* output);
typedef int (*nsbwe_cmd_t)(void *handle, uint32_t cmd, uint32_t arg);
typedef void (*nsbwe_close_t)(void *handle);

typedef struct {
    nsbwe_open_t open;
    nsbwe_proc_t proc;
    nsbwe_cmd_t cmd;
    nsbwe_close_t close;
} nsbwe_lib_t;

extern const nsbwe_lib_t sltk_nsbwe_lib;
#endif