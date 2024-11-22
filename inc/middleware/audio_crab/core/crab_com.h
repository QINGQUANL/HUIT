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
#ifndef __CRAB_COM_H__
#define __CRAB_COM_H__

#include "crab_core.h"

CRAB_RESULT LoadPlugins(CRAB_INSTANCE* cd);
CRAB_RESULT UnLoadPlugins(CRAB_INSTANCE* cd);
CRAB_RESULT ControlPlugins(CRAB_INSTANCE* cd, uint32_t command,
    uint32_t param1,
    uint32_t param2,
    void* pCtrlStructure);
CRAB_RESULT SetStatusPlugins(CRAB_INSTANCE* cd, ENGINE_RUN_STATUS status);
void PostTimerReport(void *com, uint32_t pcm_step_bytes);

#endif
