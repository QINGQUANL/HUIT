#ifndef _WSOLA_PLC
#define _WSOLA_PLC

#include "sys_inc.h"
#include "sltk_mem.h"

typedef struct
{
    /* frame size */
    int Fsize;
    /* sampling rate */
    int fs;
    /* channels */
    int channels;
    /* delay flag */
    int need_dly;
}plc_prms_t;

void  sltkplc_conceal(void* handle, int16_t** output);
void sltkplc_conceal2(void *handle,short **out);
void  sltkplc_addhist(void* hndle, int16_t** input);
void* sltkplc_open(plc_prms_t* prm);
void  sltkplc_close(void* handle);

#endif
