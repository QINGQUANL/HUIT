
#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include "sys_types.h"

#ifdef __cplusplus
 extern "C" {
#endif

struct ring_buffer {
    uint8_t *base;
    uint32_t size;              /* max number elements, must be 2^n */
    volatile uint32_t rpos;     /* read position */
    volatile uint32_t wpos;     /* write position */
    uint32_t boundary;          /* Virtual buffers size, must be (size * 2^n) */
};

/* init the struct ring_buffer before use it */
void ring_buffer_init(struct ring_buffer *rbuf,
    uint8_t *buffer_base, uint32_t size);

/* get read pointer */
uint8_t *ring_buffer_get_rptr(struct ring_buffer *rbuf);

/* get write pointer */
uint8_t *ring_buffer_get_wptr(struct ring_buffer *rbuf);

/* update write position */
void ring_buffer_update_wpos(
    struct ring_buffer *rbuf, uint32_t n);

/* update read position */
void ring_buffer_update_rpos(
    struct ring_buffer *rbuf, uint32_t n);

/* get read avail */
uint32_t ring_buffer_get_r_avail(struct ring_buffer *rbuf);

uint32_t ring_buffer_get_pick_avail(struct ring_buffer *rbuf, uint32_t pick_pos);

/* get write avail */
uint32_t ring_buffer_get_w_avail(struct ring_buffer *rbuf);

/* get avail contiguous read block */
void ring_buffer_get_r_block(
    struct ring_buffer *rbuf, uint32_t *avail, uint8_t **rptr);

/* get avail contiguous write block */
void ring_buffer_get_w_block(
    struct ring_buffer *rbuf, uint32_t *avail, uint8_t **wptr);

void ring_buffer_write(struct ring_buffer *rbuf, uint8_t *buf, uint32_t bytes);
void ring_buffer_read(struct ring_buffer *rbuf, uint8_t *buf, uint32_t bytes);
void ring_buffer_pick(struct ring_buffer *rbuf, uint32_t *p_pick_pos,
    uint8_t *buf, uint32_t bytes);
void ring_buffer_rewrite(struct ring_buffer *rbuf, uint32_t *p_rewrite_pos,
    uint8_t *buf, uint32_t bytes);


#ifdef __cplusplus
}
#endif

#endif /* _RING_BUFFER_H_ */
