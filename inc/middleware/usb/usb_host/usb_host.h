#ifndef USB_HOST_H
#define USB_HOST_H
#include "hal_ll_usb.h"

typedef struct
{
    void(*pBulkReadTransferCompleteCallback)(USBH_StatusTypeDef status);
    void(*pBulkWriteTransferCompleteCallback)(USBH_StatusTypeDef status);
    void (*pBulkRxError)(USBH_StatusTypeDef status);
    void(*pBulkUnkownCompleteCallback)(USBH_StatusTypeDef status);
    //void*  pData;
} USBH_DMA_CALLBACK;

#define USBH_DEVICE_ADDRESS 33

void register_wr_cpl_cb(USBH_DMA_CALLBACK* usbh_rw_cb);

void usbh_ep0_req();
void usbh_reset(int16_t resetTime);
USBH_StatusTypeDef usbh_handle_enum();
void usbh_resume(void);
void usb_host_init(void);
void usbh_connected();
void usbh_start();
void usbh_stop();
void usbh_bulk_dma_cpl();
void usbh_bulk_in();
void usbh_bulk_out();
void usbh_enable_pull_down();
void usbh_reset_device(uint32_t msgKick);
USBH_StatusTypeDef  usbh_process();
void usb_bus_pull_down();
int32_t udisk_initialize(void);
#endif
