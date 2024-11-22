#ifndef _USB_SYS_H_
#define _USB_SYS_H_

typedef struct _PC_DETECT_CALLBACK_
{
    void (*on_pc_out)();
    void (*on_pc_arrival)();
}PcDetectCb;

void on_pc_arrival();
void on_pc_out();
void on_hw_udisk_plus_in();

void usb_sys_module_init();
void usb_sys_module_deinit();
void usb_initialize();
void usb_deinitialize();
void usbd_audio_app_init();
#endif
