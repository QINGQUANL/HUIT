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
#ifndef __CRAB_PIN_H__
#define __CRAB_PIN_H__

#include "crab_def.h"

/////////////////////////////////////////////////////////////////////
// for ring buffer apis
/////////////////////////////////////////////////////////////////////
typedef struct
{
    unsigned char *writePtr;
    unsigned char *readPtr;
    unsigned char *beginPtr;
    unsigned char *extaPtr;

    int            ringBufferSize;
    int            ringBufferExtraSize;
    short        full;
    short        allocFlag;

    OAL_MUTEX ctrlMutex;
}XRECYLEBUFFER;

XRECYLEBUFFER *xrecylebuffer_init(int nSize, int nExtraSize, unsigned char *pool);
void xrecylebuffer_uninit(XRECYLEBUFFER *buffer, unsigned char *pool);
unsigned char *xrecylebuffer_GetWritePtr(XRECYLEBUFFER *buf);
unsigned char *xrecylebuffer_GetReadPtr(XRECYLEBUFFER *buf);
int    xrecylebuffer_GetFreeCount(XRECYLEBUFFER *buf);
int    xrecylebuffer_GetStoredCount(XRECYLEBUFFER *buf);
int    xrecylebuffer_AdvanceWritePtr(XRECYLEBUFFER *buf, int nBytes);
int    xrecylebuffer_AdvanceReadPtr(XRECYLEBUFFER *buf, int nBytes);
int    xrecylebuffer_Reset(XRECYLEBUFFER *buf);
int    xrecylebuffer_GetSize(XRECYLEBUFFER *buf);
int    xrecylebuffer_GetExtraSize(XRECYLEBUFFER *buf);

/////////////////////////////////////////////////////////////////////
// for block buffer apis
/////////////////////////////////////////////////////////////////////
typedef struct
{
    unsigned char *data;
    unsigned char *readptr;
    unsigned char *writeptr;
    int            blockSize;
    int            blocks;
    int            totalLength;
    int            contentCount;
    OAL_MUTEX    ctrlMutex;
}XBLOCKBUFFER;
XBLOCKBUFFER *blockbuffer_init(int blockSize, int blocks);
void blockbuffer_uninit(XBLOCKBUFFER *buffer);
unsigned char *blockbuffer_GetWritePtr(XBLOCKBUFFER *buffer);
unsigned char *blockbuffer_GetReadPtr(XBLOCKBUFFER *buffer);
int    blockbuffer_GetFreeCount(XBLOCKBUFFER *buf);
int    blockbuffer_GetStoredCount(XBLOCKBUFFER *buf);
int    blockbuffer_AdvanceWritePtr(XBLOCKBUFFER *buf);
int    blockbuffer_AdvanceReadPtr(XBLOCKBUFFER *buf);

/////////////////////////////////////////////////////////////////////
// for pin apis
/////////////////////////////////////////////////////////////////////
typedef struct {
    uint8_t *pData;
    uint32_t Length;
}XBUFFER;

typedef enum {
    XPIN_WAKEUP_MODE_AUTO = 0,
    XPIN_WAKEUP_MODE_CATLIKE = 1,
    XPIN_WAKEUP_MODE_FORCE = 2,
}XPIN_WAKEUP_MODE;

typedef struct _pin
{
    XRECYLEBUFFER        *pRingBuffer;
    OAL_SEM                sem;
    uint32_t        wakeupSize;
    bool_t            isInputPin;
}XPIN;


void xpin_uninit(XPIN *pin);
CRAB_RESULT xpin_connect(XPIN **outputPin, XPIN **inputPin,
    OAL_SEM outputSem, OAL_SEM inputSem,
    uint32_t outputWakeupSize, uint32_t inputWakeupSize, uint32_t totalSize);

void xpin_ResetBuffer(XPIN *pin);

uint32_t xpin_Size(XPIN *pin);

CRAB_RESULT xpin_BeginRead(XPIN *pin, XBUFFER *pBuf);
void xpin_EndRead(XPIN *pin, uint32_t len, XPIN_WAKEUP_MODE mode);

CRAB_RESULT    xpin_BeginWrite(XPIN *pin, XBUFFER *pBuf);
void xpin_EndWrite(XPIN *pin, uint32_t len, XPIN_WAKEUP_MODE mode);

void xbuffer_copy(XBUFFER* dst, XBUFFER* src);

#endif
