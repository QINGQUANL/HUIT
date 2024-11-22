#ifndef SLTK_AGC_H_
#define SLTK_AGC_H_

#include "sltk_aenc.h"

void *agc_open(void);
void  agc_close(void *handle);
int   agc_proc(void *handle, short *data, int Len);
void  agc_reset(void *handle);
int   agc_setrtprms(void* handle, agc_rtprm_t* prm);

#endif
