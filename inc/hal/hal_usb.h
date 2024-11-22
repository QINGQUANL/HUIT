#ifndef _HAL_USB_H_
#define _HAL_USB_H_
void hal_usb_bus_pull_down();   //总线加个500K的下拉
void hal_usbh_reset_bus();          //复位总线
void hal_usbh_restore_reset_bus();  //回复总线的复位状态
uint32_t get_sof_frame();

void hal_usbh_iso_init();

#endif

