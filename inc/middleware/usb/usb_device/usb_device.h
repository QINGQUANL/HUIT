#ifndef _USB_DEVICE_
#define _USB_DEVICE_

#include "setup.h"
#include "hal_ll_usb.h"
#include "hal_usb_pcd.h"
#include "usbd_def.h"
#include "usb_hid.h"
#include "usbd_msc.h"
#include "usb_bulk_msc.h"

extern PCB_USB_STATE_CB pcd_usb_state_cb;
extern PCD_EP_PROC pcd_ep_proc;
extern uint8_t* pUsb_CfgDesc;
extern USBD_DescriptorsTypeDef COMP_Desc;

void setup_set_interface_index(InterfaceIndex* pInterfaceIndex);
void usbd_start();
void usbd_stop();
BOOL is_sof_received();
void usb_epaudio_init(void);
void hal_pcd_init(PCD_EP_PROC* pcd_ep_proc, PCB_USB_STATE_CB* pcd_usb_state_cb);
void usbd_msc_init(void);
#endif


