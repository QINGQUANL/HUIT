

#ifndef _HAL_LOG_H_
#define _HAL_LOG_H_

#include "sys_define.h"
#include "sys_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* current printf level */
#if DEBUG_LOG_EN    /* global log switch */
#ifdef LOG_LEV      /* file log switch */
#define CURRENT_PRINTF_LEVEL LOG_LEV
#else
#define CURRENT_PRINTF_LEVEL 0
#endif
#else
#define CURRENT_PRINTF_LEVEL 0
#endif

#define ERROR_LEVEL     1
#define WARN_LEVEL      2
#define INFO_LEVEL      3
#define DEBUG_LEVEL     4
#define VERBOSE_LEVEL   5

#include "sys_types.h"
#include "serial_debug_io.h"

/*
 * Dummy printf for disabled debugging statements to use whilst maintaining
 * gcc's format checking.
 */
#define no_printf(format, ...) \
({                             \
    do {                       \
    } while (0);               \
})

#ifdef SLTK_LOG_TAG
#define ERROR_PREFIX        "(e):"
#define WARN_PREFIX         "(w):"
#define INFO_PREFIX         "(i):"
#define DEBUG_PREFIX        "(d):"
#define VERBOSE_PREFIX      "(v):"
#else
#define SLTK_LOG_TAG ""
#define ERROR_PREFIX        ""
#define WARN_PREFIX         ""
#define INFO_PREFIX         ""
#define DEBUG_PREFIX        ""
#define VERBOSE_PREFIX      ""
#endif

#if (CURRENT_PRINTF_LEVEL >= ERROR_LEVEL)
#define logx(format, ...) printf_debug(format, ##__VA_ARGS__)
#else
#define logx(format, ...) no_printf(format, ...)
#endif

/* Error Log */
#if (CURRENT_PRINTF_LEVEL >= ERROR_LEVEL)
#define loge(format, ...) \
    logx(SLTK_LOG_TAG ERROR_PREFIX format "\r\n", ##__VA_ARGS__)
#else
#define loge(format, ...) no_printf(format, ...)
#endif

/* Warn Log */
#if (CURRENT_PRINTF_LEVEL >= WARN_LEVEL)
#define logw(format, ...) \
    logx(SLTK_LOG_TAG WARN_PREFIX format "\r\n", ##__VA_ARGS__)
#else
#define logw(format, ...) no_printf(format, ...)
#endif


/* Info Log */
#if (CURRENT_PRINTF_LEVEL >= INFO_LEVEL)
#define logi(format, ...) \
    logx(SLTK_LOG_TAG INFO_PREFIX format "\r\n", ##__VA_ARGS__)
#else
#define logi(format, ...) no_printf(format, ...)
#endif

/* Debug Log */
#if (CURRENT_PRINTF_LEVEL >= DEBUG_LEVEL)
#define logd(format, ...) \
    logx(SLTK_LOG_TAG DEBUG_PREFIX format "\r\n", ##__VA_ARGS__)
#else
#define logd(format, ...) no_printf(format, ...)
#endif

/* Verbose Log */
#if (CURRENT_PRINTF_LEVEL >= VERBOSE_LEVEL)
#define logv(format, ...) \
    logx(SLTK_LOG_TAG VERBOSE_PREFIX format "\r\n", ##__VA_ARGS__)
#else
#define logv(format, ...) no_printf(format, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _HAL_LOG_H_ */
