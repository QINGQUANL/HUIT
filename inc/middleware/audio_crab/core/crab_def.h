/*****************************************************************************
* Project:
* All project
* Copyright (C) 2008-2018, shenju Ltd.
* All Rights Reserved.
* Description:
* All code is genrated by shenju-yangyang
* Author:
* yangyang
****************************************************************************/
#ifndef __CRAB_DEF_H__
#define __CRAB_DEF_H__

#include "oal.h"

//////////////////////////////////////////////////////////////////////////
// Basic types definitions
//////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#define HAVE_STDINT_H 1
#define HAVE_COMMON_H 0
#else
#define HAVE_STDINT_H 0
#define HAVE_COMMON_H 0
#define HAVE_STRING_H 1
#endif
#define HAVE_SELF_TYPE_H 1

#if HAVE_STDINT_H
#include <stdint.h>
#endif

#if HAVE_SELF_TYPE_H
#else
typedef char                    int8_t;
typedef short                    int16_t;
typedef int                        int32_t;
typedef signed long long        int64_t;
typedef unsigned char            uint8_t;
typedef unsigned short            uint16_t;
typedef unsigned int            uint32_t;
typedef unsigned long long        uint64_t;
#endif

typedef int                        bool_t;
typedef    void* handle_t;

#if HAVE_COMMON_H
#include "yp3_common.h"
#endif

#if HAVE_STRING_H
#include <string.h>
#endif

//////////////////////////////////////////////////////////////////////////
// Error values
//////////////////////////////////////////////////////////////////////////
typedef enum {
    CRAB_SUCCESS = 0,
    CRAB_FAILED = -1,
    CRAB_END = -2,
    CRAB_ENOTSUPPORT = -3
}CRAB_RESULT;

//////////////////////////////////////////////////////////////////////////
// Booleans
//////////////////////////////////////////////////////////////////////////
#define CRAB_FALSE            (0)
#define CRAB_TRUE            (!CRAB_FALSE)

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define USE_CRAB_API_LOCK 1
#define CRAB_LRC_MAX_BYTES 128

#define CRAB_VOLUME_MASK_HEADPHONE  0x00000000
#define CRAB_VOLUME_MASK_SPEAKER    0x00010000
#define CRAB_VOLUME_MASK_BLUETOOTH  0x00020000

#define CRAB_EVENT_READY_SUCC    1
#define CRAB_EVENT_READY_FAIL    2
#define CRAB_EVENT_SOURCE_END    4
#define CRAB_EVENT_CODEC_END     8
#define CRAB_EVENT_RENDER_END    16
#define CRAB_EVENT_FADE_OUT      32

#define CRAB_FOURCC_BE( a, b, c, d ) \
( ((uint32_t)d) | ( ((uint32_t)c) << 8 ) \
| ( ((uint32_t)b) << 16 ) | ( ((uint32_t)a) << 24 ) )
#define CRAB_TWOCC_BE( a, b ) \
( (uint16_t)(b) | ( (uint16_t)(a) << 8 ) )

#define CRAB_FOURCC( a, b, c, d ) \
( ((uint32_t)a) | ( ((uint32_t)b) << 8 ) \
| ( ((uint32_t)c) << 16 ) | ( ((uint32_t)d) << 24 ) )
#define CRAB_TWOCC( a, b ) \
( (uint16_t)(a) | ( (uint16_t)(b) << 8 ) )

#define MAXIMUM(a,b)            ((a) > (b) ? (a) : (b))
#define MINIMUM(a,b)            ((a) < (b) ? (a) : (b))

typedef enum {
    CMD_CRAB_DEFAULT_VOLUME = 1,
    CMD_CRAB_DEFAULT_SPEED,
    CMD_CRAB_DEFAULT_EQ,
}CRAB_COMMAND_DEFAULT_OPT;

typedef enum {
    CMD_CRAB_ = 0x100,
    CMD_CRAB_PAUSE,        /* audio_crab_control( &handle, CMD_CRAB_PAUSE, 0, 0, 0 ); to pause the current playback */
    CMD_CRAB_RESUME,    /* audio_crab_control( &handle, CMD_CRAB_RESUME, 0, 0, 0 ); to resume the current playback from pause */
    CMD_CRAB_SEEK,         /* audio_crab_control( &handle, CMD_CRAB_SEEK, pos, 0, 0 ); To let audio crab seek to the given position
                        by : pos, be notes that the pos is form 0 ~ duration by command CMD_CRAB_GET_DURATION */
    CMD_CRAB_START, /* Do not call this command from user on this version */
    CMD_CRAB_STOP,    /* Do not call this command from user on this version */
    CMD_CRAB_CHECK_PLAY_STATUS,    /* Control the music engine to get the current status of the playback:
                            for example: audio_crab_control( &handle, CMD_CRAB_CHECK_PLAY_STATUS, 0, 0, &status)
                            the parameter status is for retrieve the value of play status, with the enum type CRAB_PLAY_STATUS,
                            0: stoped. 1: playing, 2: paused. */
    CMD_CRAB_GET_MUSIC_INFO,
    CMD_CRAB_DEFAULT,        /* Control the crab_core use the EQ mode in parameters*/
}CRAB_COMMAND;

////////////////////////////////////////////////////////////////////////////////////////////////////////
// for pf_notify call back function pointer message type
////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum {
    CRAB_SUPPORTED_MEDIA_FORMAT_NULL = 0,
    CRAB_SUPPORTED_MEDIA_FORMAT_HTTP = 0x70747468,
    CRAB_SUPPORTED_MEDIA_FORMAT_FILE = 0x656C6966,
    CRAB_SUPPORTED_MEDIA_FORMAT_RECO = 0x6F636572,
    CRAB_SUPPORTED_MEDIA_FORMAT_MP3 = 0x33706D,
    CRAB_SUPPORTED_MEDIA_FORMAT_FLAC = 0x63616C66,
    CRAB_SUPPORTED_MEDIA_FORMAT_AMR = 0x726D61,
    CRAB_SUPPORTED_MEDIA_FORMAT_APE = 0x657061,
    CRAB_SUPPORTED_MEDIA_FORMAT_WAV = 0x766177,
    CRAB_SUPPORTED_MEDIA_FORMAT_SOCK = 0x6B636F73,
    CRAB_SUPPORTED_MEDIA_FORMAT_MEM = 0x6D656D,
    CRAB_SUPPORTED_MEDIA_FORMAT_PCM = 0x6D6370,
    CRAB_SUPPORTED_MEDIA_FORMAT_SPKR = 0x726B7073,
    CRAB_SUPPORTED_MEDIA_FORMAT_HLS = 0x736C68,
    CRAB_SUPPORTED_MEDIA_FORMAT_M4A = 0x61346D,
}CRAB_SUPPORTED_MEDIA_FORMAT;

typedef enum {
    CRAB_PLAYBACK_OPTION_NORMAL = 0,
    CRAB_PLAYBACK_OPTION_TIMERREPORT = 4,
    CRAB_PLAYBACK_OPTION_RECO_VAD = 8,
    CRAB_PLAYBACK_OPTION_RECO_LISTEN = 16,
}CRAB_PLAYBACK_OPTION;


typedef enum {
    ENGINE_RUN_STATUS_STOPPED = 0,
    ENGINE_RUN_STATUS_RUNNING,
    ENGINE_RUN_STATUS_SUSPENDED
}ENGINE_RUN_STATUS;

typedef enum {
    ENGINE_SEEK_SET = 0,
    ENGINE_SEEK_CUR = 1,
    ENGINE_SEEK_END = 2
}ENGINERW_SEEK_OPTION;

typedef enum {
    MSG_CRAB_ = 0x200,
    MSG_CRAB_PLAYOVER,            /* from pf_notify call back function to notify the user the playback
                                   or record ( record buffer is full) is finished.*/
    MSG_CRAB_TIMER_REPORT,        /* report to user the player processing in second by parameter: para1 */
    MSG_CRAB_OPEN_PA,             /* call user to open pa*/
    MSG_CRAB_CLOSE_PA,             /* call user to close pa*/
}CRAB_MESSAGE;

typedef enum {
    STATUS_CRAB_STOPED = 0,
    STATUS_CRAB_PLAYING = 1,
    STATUS_CRAB_PAUSED = 2,
}CRAB_PLAY_STATUS;

typedef enum {
    COM_TYPE_NULL = 0,
    COM_TYPE_READER = 1,
    COM_TYPE_WRITER = 2,
    COM_TYPE_CODEC = 3,
}ENGINE_COM_TYPE;

typedef enum {
    COM_THREAD_METHOD_FOLLOW = 0,
    COM_THREAD_METHOD_SINGLE = 1
}COM_THREAD_METHOD;

#endif
