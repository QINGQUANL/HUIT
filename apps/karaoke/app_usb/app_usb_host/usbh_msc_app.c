#define LOG_LEV 4
#include "hal_ll_usb.h"
#include "user_config.h"
#include "device.h"
#include "event.h"
#include "usb.h"
#include "hal.h"
#include "event.h"
#include "usb_host.h"
#include "hal_usb_hcd.h"

void usbh_read_disk();

extern HCD_EP_PROC hcd_ep_proc;

uint8_t usbh_disk_process()
{
    logi("%s",__func__);
    usb_detect_deinit();    //关闭硬件检查
    
    usb_host_init();
    usbh_start();

    __maybe_unused int32_t res1 = udisk_initialize();
    
    logi("usbh res %d", res1);
    usbh_read_disk();

    return res1;
}


