
#ifndef __HAL_HCD_H
#define __HAL_HCD_H

#include "hal_ll_usb.h"

typedef enum
{
  HAL_HCD_STATE_RESET    = 0x00,
  HAL_HCD_STATE_READY    = 0x01,
  HAL_HCD_STATE_ERROR    = 0x02,
  HAL_HCD_STATE_BUSY     = 0x03,
  HAL_HCD_STATE_TIMEOUT  = 0x04
} HCD_StateTypeDef;


#ifndef IN
#define IN  /*parameter direction flag*/
#endif

#ifndef OUT
#define OUT
#endif

typedef struct _USBH_ISR_DMA_ROUTINE_IMPL_
{
    /*
        return true indicate you do not want to use default isr process, this routine is called before defautl ISR process
        */
    BOOL (*pUsbHostIsrPreCallback)();
    /*
        this routine is called after ISR default process done,you can not filter any ISR; parameter 'isr' indicate which interrupt have occured
       */
    void (*pUsbHostIsrProcessedCallback)(uint8_t IN isrCommon, uint16_t isrOut,uint16_t isrIn);

    //------ you must implement all of the below functions in dma mode -------------
    void (*pDmaOutImpl)(uint8_t ep_type ,uint8_t IN chnum);
    void (*pDmaInImpl)(uint8_t ep_type,uint8_t IN chnum);
    void (*pNotReadyProcess)(uint8_t state, uint8_t IN chnum);
    void (*pErrorProcess)(uint8_t IN chnum);
    void (*pDmaRequestSenseProcess)();
    void (*pStallProcess)(uint8_t IN chnum);
}UsbhIsrDmaRoutineImpl;

typedef struct _hcd_endpoint_proc
{
    void (*ep0_req)();
    void (*bulk_in_proc)();
    void (*bulk_out_proc)();
    void (*bulk_dma_complete)();

    void (*connect)();
    void (*disconnect)();
    void (*babble_detect)();

    BOOL (*hdc_isr_pre_proc)(uint8_t usbIsr,uint16_t txIsr,uint16_t rxIsr);
    void (*hdc_isr_processed_proc)(uint8_t usbIsr,uint16_t txIsr,uint16_t rxIsr);
    
    BOOL (*hdc_dma_isr_pre_proc)(void);
    void (*hdc_dma_isr_processed_proc)(void);
}HCD_EP_PROC;

#define HCD_PHY_ULPI                 1
#define HCD_PHY_EMBEDDED             2

void   HAL_HCD_Init(HCD_EP_PROC* hcd_ep_proc);
BOOL is_device_insert();
BOOL hal_is_pc_in();
/* Non-Blocking mode: Interrupt */
void   HAL_HCD_IRQHandler();
void HAL_HCD_DMA_IRQHandler();
void usbh_ep0_req_proc();
USBH_StatusTypeDef wait_req_complete(int32_t kick);
#endif
