#ifndef _USB_HID_
#define _USB_HID_

typedef struct _HID_CALLBACK_
{
    void(*HID_Data_Send_Done)();
}HID_CALLBACK;

void usbd_hid_init(uint8_t*  hidBuff,int length);
void usbd_hid_send_data(uint8_t*  hidBuff,int length);

#endif

