
#ifndef _AUDITION_USB_H_
#define _AUDITION_USB_H_

#include "hal_ll_usb.h"

#define _RET_NEW_BUF_
#define _IN

typedef struct _USB_AUDION_PACKET_PARAM_
{
    uint16_t playPolling;
    uint16_t recordPolling;
    uint16_t playPacketLength;
    uint16_t recPacketLength;
}UsbAuditionPacketParam;

typedef struct _USB_AUDION_PARAM_
{
    uint8_t type;	//0: play 1,record
	uint8_t reserved;	//淇濈暀
	uint8_t sampleBites;	//閲囨牱浣嶆暟
	uint8_t sampleChannels	; //閲囨牱閫氶亾鏁?
	uint32_t sampleRate;	//閲囨牱鐜?
}UsbAudidtionParam;

typedef struct _USB_AUDION_CONTROL
{
    uint8_t type;	//0: play 1,record
    BOOL bPause;
}UsbAuditionControl;


typedef struct _USB_AUDITION_CALLBACK_
{
    _RET_NEW_BUF_ uint32_t (*SetupPcmBuff)();
    _RET_NEW_BUF_ uint32_t (*onPrepareMicData)(uint32_t _IN length);
    _RET_NEW_BUF_ uint32_t (*onPcmReceive)(uint32_t _IN length);

    void (*onPlayParamChange)(uint32_t sampleRate,uint8_t sampleBits,uint8_t channels);
    void (*onRecordParamChange)(uint32_t sampleRate,uint8_t sampleBits,uint8_t channels);
    void (*onPlayControlChange)(BOOL bPause1);
    void (*onRecordControlChange)(BOOL bPause1);
}UsbAuditionCb;

void bulk_out_proc(type_usb_dev* pUsbdev);
void bulk_in_proc(type_usb_dev* pUsbdev);


void usbd_audition_init();
BOOL PreStandardSetupProcess();
void usbd_audition_device_init_cb();
BOOL OnEp0_pre_setup();

void msc_data_transmit_dma_new(uint8_t* src,uint16_t dataLen);
uint8_t usbd_read_csr0();

#endif

