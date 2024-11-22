#ifndef _USBD_HID_APP_
#define _USBD_HID_APP_

typedef enum _E_HID_KEY
{
    HID_KEY_UP = 0,
    HID_KEY_VOL_UP = 0x01,
    HID_KEY_VOL_DOWN = 0x02,
    HID_KEY_PLAY_PAUSE = 0x08,
    HID_KEY_NEXT = 0x10,
    HID_KEY_PRE = 0x20
}E_HID_KEY;

void usbd_hid_key_push(E_HID_KEY keyState);
BOOL usbd_is_target_maybe_ps4();

#endif

