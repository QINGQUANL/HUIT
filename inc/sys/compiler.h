/*
* Copyright (c) 2016 Smartlink  Technology Co. Ltd.
* All rights reserved.
**
@File : compiler.h
@ History :
    Author          Date                Comment
    weijincai

@ Description :

*/

#ifndef _COMPILER_H_
#define _COMPILER_H_

#include "sys_types.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

#define ffs(x)          __builtin_ffs(x)
#define popcount(x)     __builtin_popcount(x)
#define ctz(x)          __builtin_ctz(x)
#define clz(x)          __builtin_clz(x)


#ifdef USE_BUILTIN_STRING_FUNC
#define memcpy(dst, src, bytes)     __builtin_memcpy(dst, src, bytes)
#define memset(dst, val, bytes)     __builtin_memset(dst, val, bytes)
#define memmove(dst, src, size)     __builtin_memmove(dst, src, size)
#define memcmp(buf1, buf2, count)   __builtin_memcmp(buf1, buf2, count)
#else
void *sltk_memcpy(void *dst, const void *src, uint32_t bytes);
void *sltk_memset(void *dst, int val, uint32_t bytes);
void *sltk_memmove(void *dst, const void *src, uint32_t count);
int sltk_memcmp(const void *buf1, const void *buf2, uint32_t count);
#endif

#define __maybe_unused __attribute__((unused))

#define MODULE_FUNC __attribute__((weak))


/* Exported types ------------------------------------------------------------*/

/**
  * @brief  HAL Status structures definition
  */
typedef enum
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U,
  HAL_UNINIT   = 0x04U,
} HAL_StatusTypeDef;

/**
  * @brief  HAL Lock structures definition
  */
typedef enum
{
  HAL_UNLOCKED = 0x00U,
  HAL_LOCKED   = 0x01U
} HAL_LockTypeDef;

/* Exported macro ------------------------------------------------------------*/
#define HAL_MAX_DELAY      0xFFFFFFFFU

#define HAL_IS_BIT_SET(REG, BIT)         (((REG) & (BIT)) != RESET)
#define HAL_IS_BIT_CLR(REG, BIT)         (((REG) & (BIT)) == RESET)

#define __HAL_LINKDMA(__HANDLE__, __PPP_DMA_FIELD__, __DMA_HANDLE__)               \
                        do{                                                      \
                              (__HANDLE__)->__PPP_DMA_FIELD__ = &(__DMA_HANDLE__); \
                              (__DMA_HANDLE__).Parent = (__HANDLE__);             \
                          } while(0)

#define UNUSED(x) ((void)(x))

/** @brief Reset the Handle's State field.
  * @param __HANDLE__: specifies the Peripheral Handle.
  * @note  This macro can be used for the following purpose:
  *          - When the Handle is declared as local variable; before passing it as parameter
  *            to HAL_PPP_Init() for the first time, it is mandatory to use this macro
  *            to set to 0 the Handle's "State" field.
  *            Otherwise, "State" field may have any random value and the first time the function
  *            HAL_PPP_Init() is called, the low level hardware initialization will be missed
  *            (i.e. HAL_PPP_MspInit() will not be executed).
  *          - When there is a need to reconfigure the low level hardware: instead of calling
  *            HAL_PPP_DeInit() then HAL_PPP_Init(), user can make a call to this macro then HAL_PPP_Init().
  *            In this later function, when the Handle's "State" field is set to 0, it will execute the function
  *            HAL_PPP_MspInit() which will reconfigure the low level hardware.
  * @retval None
  */
#define __HAL_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = 0U)

#if (SYS_OS_MODULE != OS_NONE)
  /* Reserved for future use */
  //#error "USE_RTOS should be 0 in the current HAL release"
  #define __HAL_LOCK(__HANDLE__)
	#define __HAL_UNLOCK(__HANDLE__)
#else
  #define __HAL_LOCK(__HANDLE__)                                           \
                                do{                                        \
                                    if((__HANDLE__)->Lock == HAL_LOCKED)   \
                                    {                                      \
                                       return HAL_BUSY;                    \
                                    }                                      \
                                    else                                   \
                                    {                                      \
                                       (__HANDLE__)->Lock = HAL_LOCKED;    \
                                    }                                      \
                                  }while (0)

  #define __HAL_UNLOCK(__HANDLE__)                                          \
                                  do{                                       \
                                      (__HANDLE__)->Lock = HAL_UNLOCKED;    \
                                    }while (0)
#endif /* USE_RTOS */

#if  defined ( __GNUC__ )
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
  #ifndef __packed
    #define __packed __attribute__((__packed__))
  #endif /* __packed */
#endif /* __GNUC__ */


/* Macro to get variable aligned on 4-bytes, for __ICCARM__ the directive "#pragma data_alignment=4" must be used instead */
#if defined   (__GNUC__)        /* GNU Compiler */
  #ifndef __ALIGN_END
    #define __ALIGN_END    __attribute__ ((aligned (4)))
  #endif /* __ALIGN_END */
  #ifndef __ALIGN_BEGIN
    #define __ALIGN_BEGIN
  #endif /* __ALIGN_BEGIN */
#else
  #ifndef __ALIGN_END
    #define __ALIGN_END
  #endif /* __ALIGN_END */
  #ifndef __ALIGN_BEGIN
    #if defined   (__CC_ARM)      /* ARM Compiler */
      #define __ALIGN_BEGIN    __align(4)
    #elif defined (__ICCARM__)    /* IAR Compiler */
      #define __ALIGN_BEGIN
    #endif /* __CC_ARM */
  #endif /* __ALIGN_BEGIN */
#endif /* __GNUC__ */


/**
  * @brief  __RAM_FUNC definition
  */
#if defined ( __CC_ARM   )
/* ARM Compiler
   ------------
   RAM functions are defined using the toolchain options.
   Functions that are executed in RAM should reside in a separate source module.
   Using the 'Options for File' dialog you can simply change the 'Code / Const'
   area of a module to a memory space in physical RAM.
   Available memory areas are declared in the 'Target' tab of the 'Options for Target'
   dialog.
*/
#define __RAM_FUNC HAL_StatusTypeDef

#elif defined ( __ICCARM__ )
/* ICCARM Compiler
   ---------------
   RAM functions are defined using a specific toolchain keyword "__ramfunc".
*/
#define __RAM_FUNC __ramfunc HAL_StatusTypeDef

#elif defined   (  __GNUC__  )
/* GNU Compiler
   ------------
  RAM functions are defined using a specific toolchain attribute
   "__attribute__((section(".RamFunc")))".
*/
#define __RAM_FUNC HAL_StatusTypeDef  __attribute__((section(".RamFunc")))

#endif

/**
  * @brief  __NOINLINE definition
  */
#if defined ( __CC_ARM   ) || defined   (  __GNUC__  )
/* ARM & GNUCompiler
   ----------------
*/
#define __NOINLINE __attribute__ ( (noinline) )

#elif defined ( __ICCARM__ )
/* ICCARM Compiler
   ---------------
*/
#define __NOINLINE _Pragma("optimize = no_inline")

#endif

#if defined   (  __GNUC__  )
#define AT(x)           __attribute__((section(#x)))
#define AT_SRAM(x)      __attribute__((section(#x"_sram_seg")))
#define AT_G24_SRAM(x)      __attribute__((section(#x"_sram_g24_seg")))
#define AT_BT_SRAM(x)      __attribute__((section(#x"_sram_bt_seg")))

#endif


#ifdef __cplusplus
}
#endif

#endif // _COMPILER_H_
