#ifndef _USBD_AUDIO_APP_H_
#define _USBD_AUDIO_APP_H_

#include "usbd_audio.h"
#include "usb_audio.h"

typedef struct {
    uint16_t g_logicSpeakerVolume;
    uint16_t g_logicMicVolume;
    uint8_t  g_isPcmMute;
    uint8_t  g_isMicMute;
} usbdev_context_t;

void usbd_audio_system_param_init(uint16_t* pcmPacketSize, uint16_t* micPacketSize);
void usbd_init_audio(uint32_t _OUT *ptrPcmBuf, uint16_t* _OUT pcmBufLength);
BOOL on_prepare_up_stream(uint32_t _INOUT *ptrMicBuf, uint16_t* _INOUT length);
void on_usbd_down_stream(uint8_t _INOUT *newBuf, uint16_t _INOUT length);
void on_get_cur(USBD_COMP_AUDIO_ControlTypeDef* pControl, uint16_t* _OUT curValue);
void on_set_cur(USBD_COMP_AUDIO_ControlTypeDef* pControl);
void on_get_min(uint16_t* _OUT min);
void on_get_max(uint16_t* _OUT max);
void on_get_res(uint16_t* _OUT step);
void on_set_res(USBD_COMP_AUDIO_ControlTypeDef* _IN pControl);
void on_alternate_setting_change();
BOOL ExternAudioCmd(USBD_SetupReqTypedef* _IN setupReq);
BOOL ExternAudioData(USBD_COMP_AUDIO_ControlTypeDef* _IN pControl);

void usbd_audio_app_init();
void usbd_audio_app_deinit();
void usbdev_volume_set_process(void);


#endif
