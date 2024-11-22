#ifndef _USBD_APP_COMMON_H_
#define _USBD_APP_COMMON_H_

#include "usb_sys.h"

void register_pc_detect_cb(PcDetectCb* detectCb);

void on_usb_sof_receive();
void usb_pc_out_detect_init();
void usb_pc_out_detect_deinit();
void on_pc_detect_timer_close();

#endif

