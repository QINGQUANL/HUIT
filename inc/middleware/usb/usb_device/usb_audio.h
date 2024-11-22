#ifndef _USB_AUDIO_
#define _USB_AUDIO_

#include "sys_types.h"
#include "usbd_def.h"
#include "setup.h"

#define _OUT
#define _IN
#define _INOUT

typedef struct
{
    uint8_t cmd;
    uint8_t data[USB_MAX_EP0_SIZE];
    USBD_SetupReqTypedef setup_bak;
}
USBD_COMP_AUDIO_ControlTypeDef;

typedef struct _USBD_AUDIO_CALLBACK_
{
    void (*AudioSystemParamInit)(uint16_t* pcmPacketSize, uint16_t* micPacketSize);
    void (*InitAudio)(uint32_t _OUT *ptrPcmBuf, uint16_t* _OUT pcmBufLength);
    BOOL (*OnPrepareMicData)(uint32_t _INOUT *ptrMicBuf, uint16_t* _INOUT length);
    void (*OnNewPcmReceiveBuff)(uint8_t _INOUT *newBuf, uint16_t _IN length);
    void (*GetCur)(USBD_COMP_AUDIO_ControlTypeDef* pControl, uint16_t* _OUT curValue);
    void (*SetCur)(USBD_COMP_AUDIO_ControlTypeDef* pControl);
    
    void (*GetMin)(uint16_t* _OUT min);
    void (*GetMax)(uint16_t* _OUT max);
    
    void (*GetRes)(uint16_t* _OUT step);
    void (*SetRes)(USBD_COMP_AUDIO_ControlTypeDef* _IN pControl);
    
    void (*OnAlternateSettingChange)();
    
    BOOL (*ExternAudioCmd)(USBD_SetupReqTypedef* _IN setupReq);
    BOOL (*ExternAudioData)(USBD_COMP_AUDIO_ControlTypeDef* _IN pControl);
}USBD_Audio_CallbackTypedef;

void registrer_usb_audio_callback(USBD_Audio_CallbackTypedef* pUsbdAudioCb);

#endif

