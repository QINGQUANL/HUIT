#ifndef _USBD_MSC_H_
#define _USBD_MSC_H_

#include "sys_types.h"
#include "sys_integer.h"

typedef struct MSC_SETUP_PROC
{
    BOOL (*msc_standard_setup)();
    BOOL (*msc_class_setup)();
}MscSetupProc;

typedef struct _USBD_CARDREADER_SD_FUNC_
{
    void (*sd_switch_card)(uint8_t cardNum);

    uint8_t (*sd_disk_ioctl)(BYTE cmd, void *buff);

    uint8_t (*sd_disk_read2)(uint8_t *buff, DWORD sector, UINT count,uint8_t lun);  //syn
    uint8_t (*sd_disk_read_start)(BYTE *buff, DWORD sector,uint8_t lun);    //asy
    uint8_t (*wait_sd_ready)(uint32_t timeout,uint8_t lun);                 //ays

    uint8_t (*sd_disk_write)(BYTE *buff, DWORD sector, UINT count,uint8_t lun);     //syn
    void (*sd_disk_write_wait_ready)();     //asy
    uint8_t (*sd_disk_write_start)(BYTE *buff, DWORD sector,uint8_t lun);   //asy
    uint8_t (*sd_get_status)(void);         //asy

    void (*sd_test_unit_ready)(uint8_t cardNum);
}UsbdCardreaderSdFunction;

typedef struct _USB_STATE_EVENT_CB_
{
    void (*suspend_cb)();
    void (*resume_cb)();
    void (*sof_cb)();
    
}UsbStateEventCb;

typedef struct
{
    void (*pc_out)(void);
}usb_pc_event_t;


void usbd_register_sd_wr_cb(UsbdCardreaderSdFunction* SdWrCb);

void usbd_msc_cardreader_init();
void usbd_msc_enable_cardreader(uint8_t lun,BOOL bEnable);
void usbd_msc_enable_protect(uint8_t lun, BOOL bEnable);
uint16_t usbd_msc_get_pid();
int8_t msc_get_capacity(uint32_t *block_num, uint16_t *block_size);
BOOL msc_is_ready ();
BOOL msc_is_write_protected ();
int8_t msc_read (uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t msc_write(uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
void msc_read_asy(uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
uint8_t wait_msc_read_asy_complete();
int8_t msc_write_asy(uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
void wait_msc_write_asy_complete();
void usbd_dual_sd_support_en();
void usbd_msc_cardreader_deinit();
BOOL usbd_is_cardreader_enable(uint8_t lun);
void register_usb_event_cb(UsbStateEventCb* usbStateCb);
void usbd_mw_pc_arrival_detect_init();
void usbd_register_pc_event(void *pc_out);
void usbd_set_inquiry_string(int8_t* inquiryString);

#endif
