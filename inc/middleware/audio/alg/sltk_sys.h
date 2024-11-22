#ifndef SLTK_SYS_H
#define SLTK_SYS_H

#include "oal.h"
#include "sys_inc.h"
#include "stream_io.h"

#define AFX_INTERNAL_LOG_EN 0

#if AFX_INTERNAL_LOG_EN
#include "HAL_log.h"
#define al_log(format, ...) printf_debug(format, ##__VA_ARGS__)
#else
#define al_log(format, ...)
#endif

#define sltk_printf(...)

#ifndef NULL
#define NULL (void*)(0)
#endif
#ifndef SEEK_CUR
#define SEEK_CUR    1
#endif
#ifndef SEEK_END
#define SEEK_END    2
#endif
#ifndef SEEK_SET
#define SEEK_SET    0
#endif

#endif