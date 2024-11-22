/*****************************************************************************
* Project:
* All project
* Copyright (C) 2018-2021,  shenju Ltd.
* All Rights Reserved.
* Description:

* Author:
* yangyang
****************************************************************************/
#ifndef _OAL_MEM_H_
#define _OAL_MEM_H_

#include "oal_type.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Memory
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// #define ENABLE_OAL_MEMORY_STATISTICAL
#ifdef ENABLE_OAL_MEMORY_STATISTICAL
#define OAL_malloc(x) OAL_malloc2(x, __FILE__, __LINE__)
#define OAL_calloc(x,y) OAL_calloc2(x, y, __FILE__, __LINE__)
//#define OAL_realloc(x,y) OAL_realloc2(x, y, __FILE__, __LINE__)
#define OAL_free(x) OAL_free2(x, __FILE__, __LINE__)//do{if(x)OAL_free2(x);x=0;}while(0)
#else
#define OAL_malloc(x) OAL_malloc1(x)
#define OAL_calloc(x,y) OAL_calloc1(x,y)
// #define OAL_realloc(x,y) OAL_realloc1(x,y)
#define OAL_free(x) OAL_free1(x)//do{if(x)OAL_free1(x);x=0;}while(0)
#endif


void* OAL_malloc1(uint32_t size);
void* OAL_malloc2(uint32_t size, char *file, int line);
void* OAL_calloc1(uint32_t count, uint32_t size);
void* OAL_calloc2(uint32_t count, uint32_t size, char *file, int line);
//void* OAL_realloc1(void *memory, uint32_t size);
//void* OAL_realloc2(void *memory, uint32_t size, char *file, int line);
void OAL_free1(void *memory);
void OAL_free2(void *memory, char *file, int line);

void *OAL_memcpy(void *dst, const void *src, uint32_t size);
void* OAL_memset(void *dst, int c, uint32_t size);

uint32_t OAL_heap_freespace_get( void );
void OAL_print_all_freeblock(void);

#endif


