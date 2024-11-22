/*
* Copyright (c) 2016 Smartlink  Technology Co. Ltd.
* All rights reserved.
**
@File : libc_common.h
@ History :
    Author              Date                Comment
    BRAD_YAN        2018/2/2        Create libc common header file
    wuxiaopeng       2018/6/21      add delay and sleep interfaces

@ Description :
*/

#ifndef __LIBC_COMMON_H
#define __LIBC_COMMON_H


#ifdef __cplusplus
 extern "C" {
#endif

void *memcpy(void *dst, const void *src, size_t bytes);
void *memset(void *dst, int val, size_t bytes);
void *memmove(void *dst, const void *src, size_t count);
int memcmp(const void *buf1, const void *buf2, size_t count);

#define libc_memcpy(dst, src, bytes) memcpy(dst, src, bytes)
#define libc_memset(dst, val, bytes) memset(dst, val, bytes)
#define libc_memmove(dst, src, count) memmove(dst, src, count)
#define libc_memcmp(buf1, buf2, count) memcmp(buf1, buf2, count)

#define sltk_memcpy(dst, src, bytes) memcpy(dst, src, bytes)
#define sltk_memset(dst, val, bytes) memset(dst, val, bytes)
#define sltk_memmove(dst, src, count) memmove(dst, src, count)
#define sltk_memcmp(buf1, buf2, count) memcmp(buf1, buf2, count)

void *libc_malloc(uint32_t size);
void libc_free(void* ptr);
void *libc_calloc(uint32_t size);
int libc_printf(const char *fmt, ...);

void qsort(void  *base, size_t nel, size_t width, int (*comp)(const void *, const void *));
int atoi(const char *s);
#ifdef __cplusplus
}
#endif

#endif /* __LIBC_COMMON_H */
