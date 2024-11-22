/*****************************************************************************
* Project:
* All project
* Copyright (C) 2008-2018, shenju Ltd.
* All Rights Reserved.
* Description:
* All code is genrated by shenju-yangyang
* Author:
* yangyang
****************************************************************************/
#ifndef __CRAB_UTIL_H__
#define __CRAB_UTIL_H__

#include "crab_def.h"
#ifndef _WIN32
#include "cli.h"
#define crab_printf Debug_printf
#else
#define crab_printf(x, format, ...) \
do { \
    printf("-" format "", ##__VA_ARGS__ ); \
}while(0)
#endif

#define DISP_KEY_CRAB 0xc9

#ifndef OAL_assert
#define OAL_assert(expr)                   \
    if(!(expr))                        \
    {                                  \
        crab_printf(100, "Assert: %s, %s: %d\n", (char *)__FILE__, (char *)__LINE__, expr);  \
    }          
#endif

extern void* memcpy(void* dst, const void* src, size_t bytes);
extern void* memset(void* dst, int val, size_t bytes);
extern void* memmove(void* dst, const void* src, size_t count);
extern int memcmp(const void* buf1, const void* buf2, size_t count);

//////////////////////////////////////////////////////////////////////////
unsigned int Time2Bytes(unsigned short    ChannelNum,
    unsigned short    BitPerSample,
    unsigned int    SampleRate,
    unsigned int time_ms);
unsigned int Bytes2Time(unsigned short    ChannelNum,
    unsigned short    BitPerSample,
    unsigned int    SampleRate,
    unsigned int bytes);

void Interleave2NoneInterleave(int16_t* in, int in_samples, int16_t* out0, int16_t* out1);
void Noneinterleave2Interleave(int16_t* in0, int16_t* in1, int16_t* out, int out_samples);

uint64_t Resample_s16(const int16_t* input, int16_t* output, int inSampleRate, int outSampleRate, uint64_t inputSize, uint32_t channels);
uint64_t Resample_x(const int16_t* input, int16_t* output, int32_t rate, uint64_t inputSize, uint32_t channels);
///////////////////////////////////////////////////////////////////////////
char* crab_strdup(const char* str);
char *crab_str_parten_from(char *src, char *tag_start, char *tag_end, int *len);
char *crab_path_ext_name(char *path, int *extnamelen);
char *crab_new_path_name_with_ext(char *path, const char *ext);
void crab_printfBuffer(unsigned char* pData, unsigned int len);

#endif
