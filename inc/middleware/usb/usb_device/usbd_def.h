#ifndef _USBD_DEF_H_
#define _USBD_DEF_H_

#include "sys_types.h"

typedef  struct  _usb_setup_req_
{
    uint8_t   bmRequest;
    uint8_t   bRequest;
    uint16_t  wValue;
    uint16_t  wIndex;
    uint16_t  wLength;
}USBD_SetupReqTypedef;

typedef struct
{
    uint8_t* (*GetDeviceDescriptor)(uint16_t *length);
    uint8_t* (*GetLangIDStrDescriptor)(uint16_t *length);
    uint8_t* (*GetManufacturerStrDescriptor)(uint16_t *length);
    uint8_t* (*GetProductStrDescriptor)(uint16_t *length);
    uint8_t* (*GetSerialStrDescriptor)(uint16_t *length);
    uint8_t* (*GetConfigurationStrDescriptor)(uint16_t *length);
    uint8_t* (*GetInterfaceStrDescriptor)(uint16_t *length);
    uint8_t* (*GetHidReportDescriptor)(uint16_t *length);

    uint8_t* (*GetOtherStrDescriptor)(uint16_t *length);
    uint8_t* (*GetUnProcessDescriptor)(uint8_t type, uint16_t *length);
    uint16_t(*GetUsbPID)(void);
} USBD_DescriptorsTypeDef;


enum {
    USB_DEFAULT_STATE,
    USB_ADDRESS_STATE,
    USB_CONFIG_STATE,
    USB_SUSPEND_STATE
};

#define LOBYTE(x)  ((uint8_t)(x & 0x00FF))
#define HIBYTE(x)  ((uint8_t)((x & 0xFF00) >>8))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#define  USB_LEN_DEV_QUALIFIER_DESC                     0x0A
#define  USB_LEN_DEV_DESC                               0x12
#define  USB_LEN_CFG_DESC                               0x09
#define  USB_LEN_IF_DESC                                0x09
#define  USB_LEN_EP_DESC                                0x07
#define  USB_LEN_OTG_DESC                               0x03
#define  USB_LEN_LANGID_STR_DESC                        0x04
#define  USB_LEN_OTHER_SPEED_DESC_SIZ                   0x09

#define  USBD_IDX_LANGID_STR                            0x00 
#define  USBD_IDX_MFC_STR                               0x01 
#define  USBD_IDX_PRODUCT_STR                           0x02
#define  USBD_IDX_SERIAL_STR                            0x03 
#define  USBD_IDX_CONFIG_STR                            0x04 
#define  USBD_IDX_INTERFACE_STR                         0x05 

#define  USB_REQ_TYPE_STANDARD                          0x00
#define  USB_REQ_TYPE_CLASS                             0x20
#define  USB_REQ_TYPE_VENDOR                            0x40
#define  USB_REQ_TYPE_MASK                              0x60

#define  USB_REQ_RECIPIENT_DEVICE                       0x00
#define  USB_REQ_RECIPIENT_INTERFACE                    0x01
#define  USB_REQ_RECIPIENT_ENDPOINT                     0x02
#define  USB_REQ_RECIPIENT_MASK                         0x03

#define  USB_REQ_GET_STATUS                             0x00
#define  USB_REQ_CLEAR_FEATURE                          0x01
#define  USB_REQ_SET_FEATURE                            0x03
#define  USB_REQ_SET_ADDRESS                            0x05
#define  USB_REQ_GET_DESCRIPTOR                         0x06
#define  USB_REQ_SET_DESCRIPTOR                         0x07
#define  USB_REQ_GET_CONFIGURATION                      0x08
#define  USB_REQ_SET_CONFIGURATION                      0x09
#define  USB_REQ_GET_INTERFACE                          0x0A
#define  USB_REQ_SET_INTERFACE                          0x0B
#define  USB_REQ_SYNCH_FRAME                            0x0C

#define SMARTLINK_SET_DATA                              0xC0
#define SMARTLINK_GET_DATA                              0xC1

#define SMARTLINK_SET_LOCAL_BTADDR                      0xB0
#define SMARTLINK_SET_LINK_ADDR                         0xB1

#define  USB_DESC_TYPE_DEVICE                              1
#define  USB_DESC_TYPE_CONFIGURATION                       2
#define  USB_DESC_TYPE_STRING                              3
#define  USB_DESC_TYPE_INTERFACE                           4
#define  USB_DESC_TYPE_ENDPOINT                            5
#define  USB_DESC_TYPE_DEVICE_QUALIFIER                    6
#define  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION           7
#define  USB_DESC_TYPE_HID                                 0x21
#define  USB_DESC_TYPE_HID_REPORT                          0x22

#define USBD_EP_TYPE_CTRL                                 0
#define USBD_EP_TYPE_ISOC                                 1
#define USBD_EP_TYPE_BULK                                 2
#define USBD_EP_TYPE_INTR                                 3

#define USBD_COMP_BOT_CBW_SIGNATURE             0x43425355
#define USBD_COMP_BOT_CSW_SIGNATURE             0x53425355
#define USBD_COMP_BOT_CBW_LENGTH                31
#define USBD_COMP_BOT_CSW_LENGTH                13

#define USB_MAX_EP0_SIZE                                  64

#define USBD_MAX_NUM_INTERFACES               5

#define DMA_OUT    0
#define DMA_IN     1


#define COMP_BOT_GET_MAX_LUN              0xFE
#define COMP_BOT_RESET                    0xFF
#define USB_COMP_MSC_CONFIG_DESC_SIZ      32

#define COMP_MSC_EPIN_ADDR                0x81
#define COMP_MSC_EPOUT_ADDR               0x01


#endif
