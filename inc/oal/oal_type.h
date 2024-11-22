/*****************************************************************************
* Project:
* All project
* Copyright (C) 2018-2021,  shenju Ltd.
* All Rights Reserved.
* Description:

* Author:
* yangyang
****************************************************************************/
#ifndef _OAL_TYPE_H_
#define _OAL_TYPE_H_

#ifdef _WIN32
#include <stdint.h>
#include <ctype.h>
#else
#include "sys_types.h"
#endif
//////////////////////////////////////////////////////////////////////////
// Basic types definitions
//////////////////////////////////////////////////////////////////////////
typedef	void*					OAL_HANDLE;
typedef	void*					OAL_PTR;
typedef void*					OAL_THREAD;
typedef void*					OAL_MUTEX;
typedef void*					OAL_SEM;
typedef void*					OAL_TIMER;

//////////////////////////////////////////////////////////////////////////
// Error values
//////////////////////////////////////////////////////////////////////////
typedef enum {
	OAL_SUCCESS	= 0,
	OAL_FAILED = -1,
	OAL_END = -2,
	OAL_ENOTSUPPORT	= -3
}OAL_RESULT;

#define OAL_MAX_PATH			260
#define OAL_INVALID_SOCKET  	-1
#define OAL_INVALID_SIZE		0xFFFFFFFF

//////////////////////////////////////////////////////////////////////////
// Booleans
//////////////////////////////////////////////////////////////////////////
#define OAL_FALSE			(0)
#define OAL_TRUE			(!OAL_FALSE)

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#define OAL_FOURCC_BE( a, b, c, d ) \
( ((uint32_t)d) | ( ((uint32_t)c) << 8 ) \
| ( ((uint32_t)b) << 16 ) | ( ((uint32_t)a) << 24 ) )
#define OAL_TWOCC_BE( a, b ) \
( (uint16_t)(b) | ( (uint16_t)(a) << 8 ) )

#define OAL_FOURCC( a, b, c, d ) \
( ((uint32_t)a) | ( ((uint32_t)b) << 8 ) \
| ( ((uint32_t)c) << 16 ) | ( ((uint32_t)d) << 24 ) )
#define OAL_TWOCC( a, b ) \
( (uint16_t)(a) | ( (uint16_t)(b) << 8 ) )

#define MAXIMUM(a,b)            ((a) > (b) ? (a) : (b))
#define MINIMUM(a,b)            ((a) < (b) ? (a) : (b))


#endif
