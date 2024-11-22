

#ifndef __HAL_PCD_H
#define __HAL_PCD_H

#include "hal_ll_usb.h"

typedef enum
{
    HAL_PCD_STATE_RESET   = 0x00,
    HAL_PCD_STATE_RESUME,
    HAL_PCD_STATE_SUSPEND,
    HAL_PCD_STATE_READY,
    HAL_PCD_STATE_ERROR,
    HAL_PCD_STATE_BUSY,
    HAL_PCD_STATE_TIMEOUT 
} PCD_StateTypeDef;

typedef struct _pcd_isr_process_
{
    void (*pre_process_isr)(uint8_t* comIsr,uint16_t* outIst,uint16_t* inIsr);
    void (*handled_process_isr)(uint8_t comIsr,uint16_t outIst,uint16_t inIsr);
}PCD_ISR_CB;

typedef struct _pcd_endpoint_proc
{
    void (*setup)(type_usb_dev* pUsbdev);
    void (*bulk_in_proc)(type_usb_dev* pUsbdev);
    void (*bulk_out_proc)(type_usb_dev* pUsbdev);
    void (*iso_in_proc)(type_usb_dev* pUsbdev);
    void (*iso_out_proc)(type_usb_dev* pUsbdev);
    void (*hid_in_proc)(type_usb_dev* pUsbdev);
    void (*hid_out_proc)(type_usb_dev* pUsbdev);
    //------------------------------------
    void (*iso_dma_complete)();
    void (*bulk_dma_complete)();
}PCD_EP_PROC;

typedef struct _pcd_usb_state_cb
{
    void (*initialize_cb)();
    void (*deinitialize_cb)();
    void (*suspend_cb)();
    void (*resume_cb)();
    void (*reset_cb)();
    void (*sof_cb)();
}PCB_USB_STATE_CB;

typedef struct _pcd_state_call_back
{
    void (*pUser)(PCD_StateTypeDef state);
}PCD_STATE_CB;

void hal_pcd_init(PCD_EP_PROC* pcd_ep_proc,PCB_USB_STATE_CB* pcd_usb_state_cb);
void hal_pcd_deInit();
void register_pcd_cb(PCD_ISR_CB* p_pcd_cb);
void hal_pcd_start();
void hal_pcd_stop();
void HAL_PCD_IRQHandler();
void HAL_PCD_DMA_IRQHandler();
void HAL_PCD_SOFCallback();
void HAL_PCD_ResetCallback();
void HAL_PCD_SuspendCallback();
void HAL_PCD_ResumeCallback();
void usb_bot_reset();
BOOL is_sof_received();

#endif
