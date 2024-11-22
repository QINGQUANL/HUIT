#ifndef __HAL_CPU1_HEAP_H__
#define __HAL_CPU1_HEAP_H__

#include "sys_inc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *dec_buf;
    uint32_t dec_buf_size;
    void *engine_buf;
    uint32_t engine_buf_size;
} cpu1_heap_init_t;

/* for GUI heap */
void cpu1_heap_init(cpu1_heap_init_t *init);
void* cpu1_heap_malloc(uint32_t size);
void* cpu1_heap_calloc(uint32_t size);
void cpu1_heap_free(void *ptr);
void *cpu1_heap_realloc(void *ptr, size_t size)  ;
uint32_t cpu1_heap_get_avaheapsize();
uint32_t cpu1_heap_get_maxblocksize(void);
uint32_t cpu1_heap_get_heapmallocsize(void);

/* for other rom needs */
void *general_malloc(uint32_t size);
void *general_calloc(uint32_t size);
void general_free(void *ptr);
uint32_t general_get_avaheapsize(void);
uint32_t general_get_maxblocksize(void);
uint32_t general_get_heapmallocsize(void);


#ifdef __cplusplus
}
#endif

#endif /* __HAL_CPU1_HEAP_H__ */
