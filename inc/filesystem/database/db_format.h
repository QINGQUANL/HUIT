#ifndef _DB_FORMAT_H_
#define _DB_FORMAT_H_

#ifdef _WIN32
#include "stdint.h"
#include "assert.h"
#else
#define assert(...)
#endif

#define ASCII_FORMAT			(0)
#define ANSI_FORMAT				(1<<0)
#define UTF_8_NOBOM_FORMAT		(1<<1)
#define UTF_8_BOM_FORMAT		(1<<2)
#define UTF_16_L_ENDIAN_FORMAT	(1<<3)
#define UTF_16_B_ENDIAN_FORMAT	(1<<4)
#define UTF_32_L_ENDIAN_FORMAT	(1<<5)
#define UTF_32_B_ENDIAN_FORMAT	(1<<6)
#define ERROR_FORMAT			(-1)

int ISO_8859_1_to_utf8(const uint8_t* source_buf, uint32_t source_buf_len, uint8_t* real_buf, uint32_t buf_space);
int UTF_16LE_to_utf8(const uint8_t* source_buf, uint32_t source_buf_len, uint8_t* real_buf, uint32_t buf_space);
int UTF_16BE_to_utf8(const uint8_t* source_buf, uint32_t source_buf_len, uint8_t* real_buf, uint32_t buf_space);
int db_gb2312_to_utf8(const uint8_t* source_buf, uint32_t source_buf_len, uint8_t* real_buf, uint32_t buf_space);
int db_gb2312_to_utf8Ex(const uint8_t* source_buf, uint32_t source_buf_len, uint8_t* real_buf, uint32_t buf_space, int * full);
int db_check_format(const uint8_t * buf, uint32_t len, int default_format);//default_format：ANSI_FORMAT==国标码优先；others==utf8优先

#endif
