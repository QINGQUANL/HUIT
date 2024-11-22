#ifndef _USBD_AUDDIO_
#define _USBD_AUDDIO_

#include "setup.h"
#include "hal_ll_usb.h"
#include "usb_audio.h"

typedef enum _USB_AUDIO_TYPE_
{
    AUDIO_STEREO,
    AUDIO_DOLBY
}eUsbAudioType;

void usbd_start();
void usbd_stop();
BOOL is_sof_received();
BOOL usbd_is_target_maybe_ps4();

void usbd_init_kernel_audio(USBD_Audio_CallbackTypedef* pUsbdAudioCb);
void usbd_deinit_kernel_audio();
void usbd_set_audio_mode(uint8_t stereo_or_dolby);
uint8_t usbd_get_audio_mode();
void hal_pcd_init_new(PCD_EP_PROC* pcd_ep_proc, PCB_USB_STATE_CB* pcd_usb_state_cb);
void usb_epaudio_init_new(void);
void usb_ephid_init(void);

enum PLAYER_SPATIAL_LOCALTION{
    LEFT_FRONT  = 1 << 0,       //L
    RIGHT_FRONT = 1 << 1,       //R
    CENTER_FRONT = 1 << 2,      //C
    LOW_FREQUENCY_ENHANCEMENT = 1 << 3,  //LFE
    LEFT_SURROUND = 1 << 4,     //Ls
    RIGHT_SURROUND = 1 << 5,    //Rs
    LEFT_OF_CENTER  = 1 << 6,   //Lc
    RIGHT_OF_CENTER = 1 << 7,   //Rc

    SURROUND = 1 << 8,          //S
    SIDE_LEFT = 1 << 9,         //Sl
    SIDE_RIGHT = 1 << 10,       //Sr
    TOP     = 1 << 11,          //T
};

#define CHANNEL     2
#define MAX_SAMPLERATE  192000
#define SAMPLEBITS  16

#endif
