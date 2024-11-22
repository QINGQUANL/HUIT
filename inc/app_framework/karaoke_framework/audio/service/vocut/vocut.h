#ifndef VOCUT_H
#define VOCUT_H

#include "sys_types.h"

typedef struct
{
    /* unroll the allpass filter structure of the crossover filter */
    float alpha1;
    float alpha21;
    float alpha22;
    float alpha31;
    float alpha32;
    /* delay structure (L/R independently )*/
    float dl1[2];
    float dl2[2][2];
    float dl3[2][2];
}crossflit;

typedef struct _karaoke_crossfilter_t
{
    crossflit f;
    float zn_1[2];
    float zn_2[2];
}karaoke_crossfilter_t;

typedef struct
{
    karaoke_crossfilter_t cfL;
    karaoke_crossfilter_t cfH;
    uint8_t sample_size;
    uint32_t period;
    int8_t chnum;
} vocut_prm_t;

typedef void (*vocut_mix_proc)(void *hd, void *input, void* output, uint32_t samples);

typedef struct
{
    vocut_prm_t prm;
    void *vocut_buf;
    vocut_mix_proc vocut_procesor;
} vocut_t;

void vocal_cut_close(void *handle);
void vocal_cut_proc(void *handle, void *inbuf, void *outbuf, int32_t samples);
void *vocal_cut_open(vocut_prm_t* prm);

#endif