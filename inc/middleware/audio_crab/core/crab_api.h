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

#ifndef __CRAB_API_H__
#define __CRAB_API_H__

#include "crab_core.h"

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////
#define AUDIO_CRAB_API
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
// for audio_crab_control api command
/////////////////////////////////////////////////////////////

typedef void (*CRAB_CALLBACK_FUNC)(void* h, CRAB_MESSAGE m, unsigned int p1, unsigned int p2, void* ptr);

AUDIO_CRAB_API void audio_crab_init(void);

/*************************************************************************************************************************
// Init audio crab after the machine is power on.
// BE NOTES: this API MUST be called only once when the machine is power on.
// return value: 0: failed, otherwise the handle of the instance.
*************************************************************************************************************************/
AUDIO_CRAB_API handle_t audio_crab_create(void);

/*************************************************************************************************************************
// Register crab compontents.
// BE NOTES: 
//    crab_left: maybe source com or reader com.
//    crab_abdomen: must be codec com.
//    crab_right: maybe render com or writer com.
// return value: 0 : success. -1 : failed.
*************************************************************************************************************************/
AUDIO_CRAB_API int audio_crab_register(handle_t p, const XCOM_CAPBILITY* crab_com);

/*************************************************************************************************************************
// Unnit audio crab after the machine will power off or the instance will be destoried.
// BE NOTES: this API MUST be called only once after and be pair of audio_crab_create.
// return value: 0 : success. -1 : failed.
*************************************************************************************************************************/
AUDIO_CRAB_API int audio_crab_destroy(handle_t p);

/*************************************************************************************************************************
// return value: 0 : success. -1 : failed.
// pf_notify : Message callback from audio crab, such as: play over, reference: CRAB_MESSAGE
               "user" will be send out in the first parameter
               message will be send out such as: MSG_CRAB_PLAYOVER in the second parameter
*************************************************************************************************************************/
AUDIO_CRAB_API int audio_crab_start(handle_t p, MUSIC_CONFIG config, CRAB_CALLBACK_FUNC pf_notify);

/*************************************************************************************************************************
// Destroy audio crab and set the handle to 0
// return value: 0 : success. -1 : failed.
*************************************************************************************************************************/
AUDIO_CRAB_API int audio_crab_stop(handle_t p);

/*************************************************************************************************************************
// Control audio crab with the command
// return value: 0 : success. -1 : failed.
// command :
            CMD_CRAB_PAUSE, 0,0,0
            CMD_CRAB_SEEK, seek_pos,0,0
            ......
            ...
            reference: CRAB_COMMAND
*************************************************************************************************************************/
AUDIO_CRAB_API int audio_crab_control(handle_t p, CRAB_COMMAND command, unsigned int param1, unsigned int param2, void* pCtrlStructure);


/*************************************************************************************************************************
// Control audio crab push and pull APIs
// return value: 0 : success. -1 : failed.
// p : handle of crab
// bf: XBUFFER want to push or pull, notes, XBUFFER Length is want size, when the function return, bf Lenght will be fixed
       as real used.
// timeout: will wait timeout ms for push or pull, if it can push or pull, the function will return right now.
*************************************************************************************************************************/
AUDIO_CRAB_API int audio_crab_push(handle_t p, XBUFFER* bf, uint32_t timeout);
AUDIO_CRAB_API int audio_crab_pull(handle_t p, XBUFFER* bf, uint32_t timeout);


#ifdef __cplusplus
}
#endif


#endif

