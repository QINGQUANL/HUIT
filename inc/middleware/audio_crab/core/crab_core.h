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
#ifndef __CRAB_CORE_H__
#define __CRAB_CORE_H__

#include "crab_def.h"
#include "crab_pin.h"

// for statistical
#define CRAB_STATISTICAL
#define pINFO                (&(_this->com.Base.pCD)->musicInfo)
#define pCONFIG              (&(_this->com.Base.pCD)->musicConfig)
#define PROC_CLOSE(x) \
do { \
    if(x != 0) {\
        x->Close(x); \
        OAL_free(x); \
        x = 0; \
    } \
}while(0)
#define PROC_FREE(x) \
do { \
    if(x != 0) {\
        OAL_free(x); \
        x = 0; \
    } \
}while(0)
typedef struct _xengine XENGINE;
typedef struct _xengine_com XENGINE_COM;
typedef struct _data_proc_com PROC_COM;
typedef struct _data_proc2_com PROC2_COM;
typedef struct _readwrite_com READWRITE_COM;
typedef struct _pushpull_com PUSHPULL_COM;
typedef struct _base_com BASE_COM;
typedef struct _xcom_capbility XCOM_CAPBILITY;

typedef void* (*CALL_CREATE)();
typedef CRAB_RESULT(*PFUNC_THIS)(void* _this);
typedef uint32_t(*U32_PFUNC_THIS)(void* _this);
typedef CRAB_RESULT(*PFUNC_THIS_BF)(void* _this, XBUFFER* bf);
typedef CRAB_RESULT(*PFUNC_THIS_BF_BF)(void* _this, XBUFFER* in, XBUFFER* out);
typedef CRAB_RESULT(*PFUNC_THIS_SEEK)(void* _this, int32_t offset, ENGINERW_SEEK_OPTION set);
typedef CRAB_RESULT(*PFUNC_THIS_CONTROL)(void* _this, uint32_t command, uint32_t param1, uint32_t param2, void* pCtrlStructure);

#define CRAB_MAX_COMS    5
///////////////////////////////////////////////////////
// gloabel def
///////////////////////////////////////////////////////
typedef struct {
    unsigned short record_sample_rate;
    unsigned char  record_channel_num;
    unsigned char  record_sample_bytes; // 2, 4, 16bits or 32 bits
    unsigned char  record_noise_mode; // 录音场景（降噪场景（默认关--会议，采访，教学，车站）
}RECO_CONFIG;

typedef struct {
    int speed_x;
    unsigned char eq_mode; //0: default, 1: pop, 2: rock, 3: jazz, 4: classical, 5: dance, 6: country
    unsigned short fade_ms;
}PLAY_CONFIG;

typedef struct
{
    char* pUser; /* the pointer of user handle, when the engine call "pf_notify", the user will be send out in the first parameter */    
    unsigned int    PlaybackOption; /* =0 or CRAB_PLAYBACK_OPTION_BENCHMARK | CRAB_PLAYBACK_OPTION_SHOWLOG etc...*/
    char* SrcUrl;        /* the full path and filename with protocol ahead, like: file://\dir\aaa.mp3, ... http://.......mp3
                                if user do not know the media format of the URL, the crab_core will auto detect the format
                                when user appointed codec4CC parameter is not 0, the crab_core will force use the codec4CC,
                                for some especial usage, see helper, for mem or raop ...etc*/
    char* DstUrl;        /* dst url, usage is same as SrcUrl*/
    unsigned int startPos; /* start position to seek from source com */
    unsigned int volume;
    union
    {
        RECO_CONFIG reco_cfg;
        PLAY_CONFIG play_cfg;
    } conf;
}MUSIC_CONFIG;

typedef struct
{
    unsigned int    MediaFormat;
    unsigned char   ChannelNum; /* retrive the media information of channel number*/
    unsigned char   BitPerSample; /* retrive the media information of bits of one sample*/
    unsigned short  SampleRate; /* retrive the media information of sample rate*/
    unsigned int    AvgBytesPerSec; /* retrive the media information of average bytes transfer of this media stream.*/
    unsigned int    Duration; /* retrive the total media duration in second*/
    unsigned int    TotalSize; /* retrive the total media size in bytes of generated media if possable.*/
    unsigned int    CurrentTimeOffset;
    unsigned int    CurrentPcmOffset;
    unsigned char   CanNotSpeedx;
    unsigned char   CanFadeOut;
    void            *TimerReportCom;
    char   CurrentLrc[CRAB_LRC_MAX_BYTES];
} MUSIC_INFO;

typedef struct
{
    OAL_HANDLE ev;
    void(*pf_notify)(handle_t, CRAB_MESSAGE, uint32_t, uint32_t, void*);
    MUSIC_INFO musicInfo;
    MUSIC_CONFIG musicConfig;
    OAL_QUEUE* inst_queue;
    XENGINE* first_engine;
    const XCOM_CAPBILITY* readerCP;
    const XCOM_CAPBILITY* writerCP;
    const XCOM_CAPBILITY* writerCP_bypass_front;
    const XCOM_CAPBILITY* writerCP_bypass_end;
    const XCOM_CAPBILITY* coms[CRAB_MAX_COMS];
    PUSHPULL_COM* pusher;
    PUSHPULL_COM* puller;
} CRAB_INSTANCE;

typedef struct
{
    CRAB_INSTANCE* inst;
    CRAB_MESSAGE   msg;
}INSTANCE_MSG;



///////////////////////////////////////////////////////
// compontents def
///////////////////////////////////////////////////////
struct _xcom_capbility {
    ENGINE_COM_TYPE      type;
    uint32_t             inputFormat;
    uint32_t             outputFormat;
    COM_THREAD_METHOD    threadMethod;
    CALL_CREATE          callCreate;
    uint32_t             bufferingSizeIn;
    uint32_t             bufferingSizeOut;
    char*                pName;
};

struct _base_com {
    CRAB_INSTANCE* pCD;
    XCOM_CAPBILITY CB;

    // option
    OAL_THREAD_PRIORITY threadPriority;
    unsigned int        stackSize;
};

struct _data_proc_com {
    BASE_COM                Base;
    PFUNC_THIS_BF           Proc;
    PFUNC_THIS              Close;
};

struct _data_proc2_com {
    BASE_COM                Base;
    PFUNC_THIS_BF_BF        Proc2;
    PFUNC_THIS              Close;
};

struct _pushpull_com {
    OAL_SEM sem;
    XPIN *pin;
    uint32_t io_size;
    uint32_t buffering_size;
};

struct _readwrite_com {
    BASE_COM                Base;
    PFUNC_THIS_BF           Read;
    PFUNC_THIS_BF           Write;
    PFUNC_THIS              Close;
    PFUNC_THIS_SEEK         Seek;
    U32_PFUNC_THIS          Size;
    U32_PFUNC_THIS          Position;
};

struct _xengine_com {
    BASE_COM                Base;
    PFUNC_THIS                Run;
    PFUNC_THIS                Close;
    PFUNC_THIS_CONTROL        Control;
    PFUNC_THIS_BF            Sniffer;
    XPIN* pInputPin;
    XPIN* pOutputPin;
    uint32_t                    inputSize;
    uint32_t                    outputSize;
    READWRITE_COM* pReader;
    READWRITE_COM* pWriter;
    READWRITE_COM* pWriter_bypass;
    XENGINE_COM* pNextCom;
    XENGINE* pEngine;
#ifdef CRAB_STATISTICAL
    uint32_t            runTime;
#endif
};

struct _xengine {
    CRAB_INSTANCE* pCD;
    XENGINE_COM* com;
    OAL_SEM                sem;
    bool_t            End;
    OAL_SEM                semEnd;
    OAL_THREAD            thread;
    OAL_MUTEX            ctrlLock;
    ENGINE_RUN_STATUS    runStatus;
    OAL_SEM                semStatusChangeReport;
    XENGINE* next;
};


XENGINE* xengine_init(CRAB_INSTANCE* cd);
void xengine_uninit(XENGINE* _this);
CRAB_RESULT xengine_control(XENGINE* _this,
    uint32_t command,
    uint32_t param1,
    uint32_t param2,
    void* pCtrlStructure);
CRAB_RESULT xengine_cat(XENGINE* _this, XENGINE* next);
CRAB_RESULT xengine_insert(XENGINE* _this, XENGINE_COM* com);
CRAB_RESULT xengine_com_cat(XENGINE_COM* _this, XENGINE_COM* nextcom, bool_t isSameEngine);
CRAB_RESULT xengine_com_front_plugin(XENGINE_COM* _this);
CRAB_RESULT xengine_com_end_plugin(XENGINE_COM* _this);
CRAB_RESULT xengine_set_status(XENGINE* _this, ENGINE_RUN_STATUS status);

BASE_COM* CallCreate(XCOM_CAPBILITY cb, CRAB_INSTANCE* cd);

#endif
