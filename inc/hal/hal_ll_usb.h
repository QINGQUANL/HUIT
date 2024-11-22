
#ifndef __HAL_LL_USB_H
#define __HAL_LL_USB_H

#include "sys_types.h"

typedef enum
{
   USB_OTG_DEVICE_MODE  = 0U,
   USB_OTG_HOST_MODE    = 1U,
}USB_OTG_ModeTypeDef;


typedef enum {
  URB_IDLE = 0U,
  URB_DONE,
  URB_NOTREADY,
  URB_ERROR,
  URB_STALL

}USB_OTG_URBStateTypeDef;



typedef enum {
  HC_IDLE = 0U,
  HC_XFRC,
  HC_HALTED,
  HC_NAK,
  HC_NYET,
  HC_STALL,
  HC_XACTERR,
  HC_BBLERR,
  HC_DATATGLERR

}USB_OTG_HCStateTypeDef;

//EP0״̬��
enum {
    IDLE_STATE,
    TRANSER_OVER_STATE,
    TRANSFERING_STATE,
    RECEIVING_STATE,
    STATUS_SETUP,
    STATUS_STATE
};

typedef enum
{
    USBH_OK   = 0,
    USBH_BUSY,          //数据正在发送
    USBH_FAIL,          //发送失败
    USBH_NOT_SUPPORTED, //不支持
    USBH_BABBLE_DETECT, //数据长度错误
    USBH_UNRECOVERED_ERROR, //不可恢复的错误
    USBH_TIMEOUT,       //超时
    USBH_CDB_LENGTH_EEROR,//错误的CDB格式
    USBH_STALL,         //STALL了
    USBH_RX_ERROR,      //严重的总线错误
    USBH_STALL_RECOVER, //之前STALL过，已经执行了clear_feature命令，需要恢复
}USBH_StatusTypeDef;

#define EP0_MAX_SIZE    64
#define EP1_MAX_SZIE    64
#define EP3_MAX_SIZE    32

#define EP2_IN_MAX_SIZE 96
#define EP2_OUT_MAX_SIZE 768

#define BULK_EP_MAX_SIZE    EP1_MAX_SZIE
#define IOSO_EP_MAX_SIZE    EP2_OUT_MAX_SIZE
#define IOSI_EP_MAX_SIZE    EP2_IN_MAX_SIZE

#define OPCODE_TEST_UNIT_READY            0x00
#define OPCODE_READ_CAPACITY10            0x25
#define OPCODE_READ_FORMAT_CAPACITY       0x23
#define OPCODE_READ10                     0x28
#define OPCODE_WRITE10                    0x2A
#define OPCODE_REQUEST_SENSE              0x03
#define OPCODE_INQUIRY                    0x12

#define OPCODE_CLEAR_FEATURE              0xEF

#define OPCODE_PRIVATE_WRITE              0xF0
#define OPCODE_PRIVATE_READ               0xF1
#define OPCODE_PRIVATE_WRITE_NO_DATA      0xF2

enum{
    BULK_CMD_OUT,
    BULK_DATA_OUT,
    BULK_DATA_IN,
    BULK_CSW_END
};

#define DMA_CHANNEL_BULK    0
#define DMA_CHANNEL_ISO_TX  1
#define DMA_CHANNEL_ISO_RX  2

typedef struct {
    uint8_t set_addr;                    //���ַ,device
    uint8_t cfg_value;                   //Config Value, ����Set Config��Get Config,device
    uint8_t cur_state;                   //USB��ǰ״̬��,device

    uint8_t ep0_cur_state;                   //EP0��״̬��
    uint8_t ep0_next_state;             //host would use this value, and device not
    uint8_t* ep_ex_data_ptr;           //���ͻ��߽��յ����ݵ�ָ��
    uint16_t ep_ex_data_len;           //
    uint8_t ep0_first_rx_packet;        //mark whether the first in packet is comming, host only
    uint8_t ep0_host_req_type;           //ep0 setup request type,host only 
    uint8_t bulk_cmd_type;              //mark the commnad of bulk,host   
    uint8_t bulk_trans_state;              //direction of bulk transmit
    uint8_t* bulk_data_ptr;              //host only
    uint16_t bulk_data_len;              //host only
    uint8_t* csw;                       //buff for receive csw

    uint8_t* ep_data_ptr[4];                    //��ǰ����ָ��
    uint16_t ep_data_len[4];                    //��ǰ����ʣ��
    uint16_t ep_tx_packet_size[4];            //������
    uint16_t ep_rx_packet_size[4];      //
    uint16_t msc_rx_pkt_size;               //���յ����ݳ���
    uint8_t  is_bulk_dma_tx;

    uint8_t hid_interface;
    uint8_t audio_interface;
    uint8_t msc_interface;
    
    uint8_t isoc_tx_interface;           //ISOC��MIC���͵Ľӿ�
    uint8_t isoc_tx_enable;              //ISOCʹ��
    uint8_t isoc_tx_enable_int;          //���ж�������ISOCʹ��

    volatile uint8_t ep1_tx_stall;       //ep1 tx stall��־
    volatile uint8_t ep1_rx_stall;       //ep1 rx stall��־
    volatile uint8_t ep1_pipe_stall;     //ep1 Stall�ı�־

    uint32_t  lparam;   //reserved for externd
    uint32_t* wParam;   //reserved for externd
} type_usb_dev;

typedef struct _USBH_WAITING_CB_
{
    void (*usbh_req_waiting)(); //
}UsbWaitingCb;

#define EP_CTRL                           0U
#define EP_BULK                           1U
#define EP_ADDI_HID                       1U
#define EP_ISOC                           2U
#define EP_HID                           3U
#define EP_TYPE_MSK                            3U

#define MAX_EP  3

#define HC_PID_DATA0                           0U
#define HC_PID_DATA2                           1U
#define HC_PID_DATA1                           2U
#define HC_PID_SETUP                           3U

/* Exported functions --------------------------------------------------------*/
void USB_DevInit(type_usb_dev* usb_dev);
void USB_SetCurrentMode( USB_OTG_ModeTypeDef mode);
void USB_FlushRxFifo(uint32_t num);
void USB_FlushTxFifo ( uint32_t num );
void USB_FlushAllFifo();
void usb_epn_tx_ready(uint8_t epnum);
void USBD_EPx_prepare_receive(uint8_t epnum);
void USBH_EPx_prepare_receive(uint8_t epnum);
BOOL usb_ep0_setup_end();
void USBD_ReadWritePacket_dma(uint8_t *src, uint16_t len,uint8_t epnum, BOOL isUsbTX,BOOL isAutoTransfer);
void USB_Disable_dma(uint8_t epnum,BOOL isUsbTX);
void USB_WritePacket(uint8_t *src,uint16_t len, uint8_t epnum);
void *USB_ReadPacket(uint8_t *dest, uint16_t len,uint8_t epnum);
void USB_EPSetStall(uint8_t epnum);
void USB_EPClearStall(uint8_t epnum);
void USB_SetDevAddress (uint8_t address);
void USB_Connect ();
void USB_Disconnect ();
void USB_StopDevice();
uint8_t  USB_ReadCommonInterrupts ();
uint16_t USB_ReadRxEpInterrupt ();
uint16_t USB_ReadTxEpInterrupt ();
void usb_ep0_rx_ready();
void USB_ResetPort(uint16_t mDelayTime);

void USB_EP0StartTX(uint8_t* src,uint16_t dataLen);
void USB_EP0StartRX(uint8_t* src,uint16_t dataLen);
void USB_EPnStartRX(uint8_t* src,uint16_t dataLen, uint8_t epnum);
void USB_EPnStartTX(uint8_t* src,uint16_t dataLen, uint8_t epnum);
void usbd_epn_start_tx_dma(uint8_t* src,uint16_t dataLen, uint8_t epnum);
void usbd_epn_start_rx_dma(uint8_t* src,uint16_t dataLen, uint8_t epnum);
void usbh_bulk_dma(uint8_t* src,uint16_t dataLen,BOOL isTx);
void USB_StopHost();
void USBH_CtlSendSetup (uint8_t *buff);
uint16_t USB_GetRxLength(uint8_t epnum);
void usbh_suspend(void);
void usbh_resume(void);
void usb_epmass_init(void);
void usbd_start();
void usbd_stop();
BOOL is_sof_received();
void usbd_sys_module_init();
void usb_bus_pull_down();
void usb_sof_isr_en(BOOL bEnable);

#endif
