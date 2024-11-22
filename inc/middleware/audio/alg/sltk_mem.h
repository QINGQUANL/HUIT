#ifndef SLTK_MEM_H
#define SLTK_MEM_H

#ifdef _MSC_VER
#include "platform.h"
#else
#include "sys_inc.h"
#include "oal.h"

void* sltk_malloc(uint32_t size);
void  sltk_free(void* ptr);
void* sltk_calloc(uint32_t size);
/* mem allocated */
void general_memconfig(void *base_addr, uint32_t mem_size);
void *general_malloc(uint32_t size);
void *general_calloc(uint32_t size);
void* general_calloc_max(uint32_t* psize);
void *general_sltk_malloc_max();
void general_free(void *ptr);
void *general_malloc_stack(uint32_t size);
void *general_calloc_stack(uint32_t size);
void general_free_stack();
uint32_t general_get_avaheapsize();
void general_inc_mem(uint32_t inc_size);
uint32_t general_get_maxblocksize(void);
uint32_t general_get_heapmallocsize(void);
#endif

#endif