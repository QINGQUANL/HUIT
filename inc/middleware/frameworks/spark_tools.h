/*
*********************************************************************************************************
*
* Filename      : tools.h
* Description   : Header file---tools driver
* Revision      : V1.0
* Author        : Tong Yan
* Created       : 2019/04/09
* Last Updated  : 2019/04/09
*
* Revision History:
* 2019/04/09   Revision 1.0
*
*
*********************************************************************************************************
*/

#ifndef __SPARK_TOOLS_H__
#define __SPARK_TOOLS_H__
#include "sys_inc.h"
/********************************************************************************************************
 *   DEFINE
 ********************************************************************************************************/
#define ENDIAN_COV16(x)                         (((((uint16_t)x) & 0xff00) >> 8) | ((((uint16_t)x) & 0xff) << 8))
#define ENDIAN_COV32(x)                         (((((uint32_t)x) & 0xff000000) >> 24) | ((((uint32_t)x) & 0xff0000) >> 8) | \
                                                ((((uint32_t)x) & 0x000000ff) << 24) | ((((uint32_t)x) & 0x00ff00) << 8))
#define SET_NUM32_BIT_N(num, n)                 ((num) |= (1 << ((n) % 32)))
#define RESET_NUM32_BIT_N(num, n)               ((num) &= (~(1 << ((n) % 32))))
#define SET_NUM16_BIT_N(num, n)                 ((num) |= (1 << ((n) % 16)))
#define RESET_NUM16_BIT_N(num, n)               ((num) &= (~(1 << ((n) % 16))))
#define SET_NUM8_BIT_N(num, n)                  ((num) |= (1 << ((n) % 8)))
#define RESET_NUM8_BIT_N(num, n)                ((num) &= (~(1 << ((n) % 8))))
/********************************************************************************************************
 *   CONSTANTS
 ********************************************************************************************************/

/********************************************************************************************************
 *   CUSTOMIZATION
 ********************************************************************************************************/

/********************************************************************************************************
 *   DATA TYPES
 ********************************************************************************************************/

/********************************************************************************************************
 *   GLOBAL VARIABLES
 ********************************************************************************************************/

/********************************************************************************************************
 *   FUNCTIONS
 ********************************************************************************************************/
int string_length(char *str);
int string_to_dec(unsigned int *dest, const char *str);
int32_t str_to_dec(uint32_t *dest, const char *str, uint32_t len);
uint32_t string_to_intdec(const uint8_t *str);
char *string_copy(char *dest, const char *src);
char *str_copy(char *dest, const char *src, unsigned int len);
int string_to_hex(unsigned int *dest, const char *str);
uint32_t str_to_hex(const uint8_t *str);

#endif /* __SPARK_TOOLS_H__ */
