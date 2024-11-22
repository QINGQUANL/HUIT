/*****************************************************************************
* Project:
* All project
* Copyright (C) 2018-2021,  shenju Ltd.
* All Rights Reserved.
* Description:

* Author:
* yangyang
****************************************************************************/
#ifndef _OAL_EVENT_H_
#define _OAL_EVENT_H_

#include "oal_type.h"

OAL_HANDLE OAL_event_create();

uint8_t     OAL_event_wait(OAL_HANDLE h, uint8_t bits, uint32_t wait_milliseconds);
uint8_t     OAL_event_wait_clear(OAL_HANDLE h, uint8_t bits, uint32_t wait_milliseconds);
uint8_t     OAL_event_wait_any(OAL_HANDLE h, uint8_t bits, uint32_t wait_milliseconds);
uint8_t     OAL_event_wait_any_clear(OAL_HANDLE h, uint8_t bits, uint32_t wait_milliseconds);

void       OAL_event_set(OAL_HANDLE h, uint8_t bits);
void       OAL_event_clear(OAL_HANDLE h, uint8_t bits);
void       OAL_event_destroy(OAL_HANDLE h);

#endif


