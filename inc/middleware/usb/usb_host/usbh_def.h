
#ifndef _USBH_DEF_
#define _USBH_DEF_

#define ValBit(VAR,POS)                               (VAR & (1 << POS))
#define SetBit(VAR,POS)                               (VAR |= (1 << POS))
#define ClrBit(VAR,POS)                               (VAR &= ((1 << POS)^255))

#define  LE16(addr)             (((uint16_t)(*((uint8_t *)(addr))))\
                                + (((uint16_t)(*(((uint8_t *)(addr)) + 1))) << 8))

#define  LE16S(addr)              (uint16_t)(LE16((addr)))

#define  LE32(addr)              ((((uint32_t)(*(((uint8_t *)(addr)) + 0))) + \
                                              (((uint32_t)(*(((uint8_t *)(addr)) + 1))) << 8) + \
                                              (((uint32_t)(*(((uint8_t *)(addr)) + 2))) << 16) + \
                                              (((uint32_t)(*(((uint8_t *)(addr)) + 3))) << 24)))

#define  LE64(addr)              ((((uint64_t)(*(((uint8_t *)(addr)) + 0))) + \
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 1))) <<  8) +\
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 2))) << 16) +\
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 3))) << 24) +\
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 4))) << 32) +\
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 5))) << 40) +\
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 6))) << 48) +\
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 7))) << 56)))


#define  LE24(addr)              ((((uint32_t)(*(((uint8_t *)(addr)) + 0))) + \
                                              (((uint32_t)(*(((uint8_t *)(addr)) + 1))) << 8) + \
                                              (((uint32_t)(*(((uint8_t *)(addr)) + 2))) << 16)))


#define  LE32S(addr)              (int32_t)(LE32((addr)))

#define  USB_DESC_TYPE_DEVICE                              1
#define  USB_DESC_TYPE_CONFIGURATION                       2
#define  USB_DESC_TYPE_STRING                              3
#define  USB_DESC_TYPE_INTERFACE                           4
#define  USB_DESC_TYPE_ENDPOINT                            5
#define  USB_DESC_TYPE_DEVICE_QUALIFIER                    6
#define  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION           7
#define  USB_DESC_TYPE_INTERFACE_POWER                     8
#define  USB_DESC_TYPE_HID                                 0x21
#define  USB_DESC_TYPE_HID_REPORT                          0x22

#define USB_DEVICE_DESC_SIZE                               18
#define USB_CONFIGURATION_DESC_SIZE                        9
#define USB_HID_DESC_SIZE                                  9
#define USB_INTERFACE_DESC_SIZE                            9
#define USB_ENDPOINT_DESC_SIZE                             7

#define  USB_DESC_DEVICE                    ((USB_DESC_TYPE_DEVICE << 8) & 0xFF00)
#define  USB_DESC_CONFIGURATION             ((USB_DESC_TYPE_CONFIGURATION << 8) & 0xFF00)
#define  USB_DESC_STRING                    ((USB_DESC_TYPE_STRING << 8) & 0xFF00)
#define  USB_DESC_INTERFACE                 ((USB_DESC_TYPE_INTERFACE << 8) & 0xFF00)
#define  USB_DESC_ENDPOINT                  ((USB_DESC_TYPE_INTERFACE << 8) & 0xFF00)
#define  USB_DESC_DEVICE_QUALIFIER          ((USB_DESC_TYPE_DEVICE_QUALIFIER << 8) & 0xFF00)
#define  USB_DESC_OTHER_SPEED_CONFIGURATION ((USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION << 8) & 0xFF00)
#define  USB_DESC_INTERFACE_POWER           ((USB_DESC_TYPE_INTERFACE_POWER << 8) & 0xFF00)
#define  USB_DESC_HID_REPORT                ((USB_DESC_TYPE_HID_REPORT << 8) & 0xFF00)
#define  USB_DESC_HID                       ((USB_DESC_TYPE_HID << 8) & 0xFF00)

#define  USB_REQ_DIR_MASK                               0x80
#define  USB_D2H                                        0x80
#define  USB_H2D                                        0x00

#define  USB_REQ_GET_MAX_LUN 0xFE

#define USBH_MAX_NUM_INTERFACES 2

typedef enum {
    USBH_URB_IDLE = 0,
    USBH_URB_DONE,
    USBH_URB_NOTREADY,
    USBH_URB_NYET,
    USBH_URB_ERROR,
    USBH_URB_STALL
}USBH_URBStateTypeDef;

typedef union
{
    uint16_t w;
    struct BW
    {
        uint8_t msb;
        uint8_t lsb;
    }
    bw;
}
uint16_t_uint8_t;


typedef union _USB_Setup
{
    uint32_t d8[2];

    struct _SetupPkt_Struc
    {
        uint8_t           bmRequestType;
        uint8_t           bRequest;
        uint16_t_uint8_t  wValue;
        uint16_t_uint8_t  wIndex;
        uint16_t_uint8_t  wLength;
    } b;
}
USB_Setup_TypeDef;

typedef  struct  _DescHeader
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
}
USBH_DescHeader_t;

typedef struct _DeviceDescriptor
{
    uint8_t   bLength;
    uint8_t   bDescriptorType;
    uint16_t  bcdUSB;        /* USB Specification Number which device complies too */
    uint8_t   bDeviceClass;
    uint8_t   bDeviceSubClass;
    uint8_t   bDeviceProtocol;
    /* If equal to Zero, each interface specifies its own class
    code if equal to 0xFF, the class code is vendor specified.
    Otherwise field is valid Class Code.*/
    uint8_t   bMaxPacketSize;
    uint16_t  idVendor;      /* Vendor ID (Assigned by USB Org) */
    uint16_t  idProduct;     /* Product ID (Assigned by Manufacturer) */
    uint16_t  bcdDevice;     /* Device Release Number */
    uint8_t   iManufacturer;  /* Index of Manufacturer String Descriptor */
    uint8_t   iProduct;       /* Index of Product String Descriptor */
    uint8_t   iSerialNumber;  /* Index of Serial Number String Descriptor */
    uint8_t   bNumConfigurations; /* Number of Possible Configurations */
}
USBH_DevDescTypeDef;

typedef struct _EndpointDescriptor
{
    uint8_t   bLength;
    uint8_t   bDescriptorType;
    uint8_t   bEndpointAddress;   /* indicates what endpoint this descriptor is describing */
    uint8_t   bmAttributes;       /* specifies the transfer type. */
    uint16_t  wMaxPacketSize;    /* Maximum Packet Size this endpoint is capable of sending or receiving */
    uint8_t   bInterval;          /* is used to specify the polling interval of certain transfers. */
}
USBH_EpDescTypeDef;

typedef struct _InterfaceDescriptor
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;    /* Value used to select alternative setting */
    uint8_t bNumEndpoints;        /* Number of Endpoints used for this interface */
    uint8_t bInterfaceClass;      /* Class Code (Assigned by USB Org) */
    uint8_t bInterfaceSubClass;   /* Subclass Code (Assigned by USB Org) */
    uint8_t bInterfaceProtocol;   /* Protocol Code */
    uint8_t iInterface;           /* Index of String Descriptor Describing this interface */
    USBH_EpDescTypeDef               Ep_Desc[USBH_MAX_NUM_INTERFACES];
}
USBH_InterfaceDescTypeDef;


typedef struct _ConfigurationDescriptor
{
    uint8_t   bLength;
    uint8_t   bDescriptorType;
    uint16_t  wTotalLength;        /* Total Length of Data Returned */
    uint8_t   bNumInterfaces;       /* Number of Interfaces */
    uint8_t   bConfigurationValue;  /* Value to use as an argument to select this configuration*/
    uint8_t   iConfiguration;       /*Index of String Descriptor Describing this configuration */
    uint8_t   bmAttributes;         /* D7 Bus Powered , D6 Self Powered, D5 Remote Wakeup , D4..0 Reserved (0)*/
    uint8_t   bMaxPower;            /*Maximum Power Consumption */
    USBH_InterfaceDescTypeDef        Itf_Desc[USBH_MAX_NUM_INTERFACES];
}
USBH_CfgDescTypeDef;

typedef struct
{
    uint8_t                           Data[512];
    uint8_t                           address;
    uint8_t                           speed;
    uint8_t                           current_interface;
    USBH_DevDescTypeDef               DevDesc;
    USBH_CfgDescTypeDef               CfgDesc;

}USBH_DeviceTypeDef;

typedef enum
{
    CTRL_IDLE = 0,
    CTRL_SETUP,
    CTRL_SETUP_WAIT,
    CTRL_DATA_IN,
    CTRL_DATA_IN_WAIT,
    CTRL_DATA_OUT,
    CTRL_DATA_OUT_WAIT,
    CTRL_STATUS_IN,
    CTRL_STATUS_IN_WAIT,
    CTRL_STATUS_OUT,
    CTRL_STATUS_OUT_WAIT,
    CTRL_ERROR,
    CTRL_STALLED,
    CTRL_COMPLETE
}CTRL_StateTypeDef;


#endif


