#ifndef _USBH_MSC_H_
#define _USBH_MSC_H_

#include "hal_ll_usb.h"

#define BOT_CBW_SIGNATURE            0x43425355
#define BOT_CBW_TAG                  0x026AA9A0             
#define BOT_CSW_SIGNATURE            0x53425355           
#define BOT_CBW_LENGTH               31
#define BOT_CSW_LENGTH               13     

#define BOT_SEND_CSW_DISABLE         0
#define BOT_SEND_CSW_ENABLE          1

#define BOT_DIR_IN                   0
#define BOT_DIR_OUT                  1
#define BOT_DIR_BOTH                 2

#define BOT_PAGE_LENGTH              512


#define BOT_CBW_CB_LENGTH            16

#define DATA_LEN_MODE_TEST_UNIT_READY        0
#define DATA_LEN_READ_CAPACITY10             8
#define DATA_LEN_READ_FORMAT_CAPACITY        0xFC
#define DATA_LEN_INQUIRY                    36
#define DATA_LEN_REQUEST_SENSE              14

#define CBW_CB_LENGTH                       16
#define CBW_LENGTH                          10

#define  USB_EP_DIR_OUT                                 0x00
#define  USB_EP_DIR_IN                                  0x80
#define  USB_EP_DIR_MSK                                 0x80

#define SCSI_SENSE_KEY_NO_SENSE                          0x00
#define SCSI_SENSE_KEY_RECOVERED_ERROR                   0x01
#define SCSI_SENSE_KEY_NOT_READY                         0x02
#define SCSI_SENSE_KEY_MEDIUM_ERROR                      0x03
#define SCSI_SENSE_KEY_HARDWARE_ERROR                    0x04
#define SCSI_SENSE_KEY_ILLEGAL_REQUEST                   0x05
#define SCSI_SENSE_KEY_UNIT_ATTENTION                    0x06
#define SCSI_SENSE_KEY_DATA_PROTECT                      0x07
#define SCSI_SENSE_KEY_BLANK_CHECK                       0x08
#define SCSI_SENSE_KEY_VENDOR_SPECIFIC                   0x09
#define SCSI_SENSE_KEY_COPY_ABORTED                      0x0A
#define SCSI_SENSE_KEY_ABORTED_COMMAND                   0x0B
#define SCSI_SENSE_KEY_VOLUME_OVERFLOW                   0x0D
#define SCSI_SENSE_KEY_MISCOMPARE                        0x0E

#define SCSI_ASC_NO_ADDITIONAL_SENSE_INFORMATION         0x00
#define SCSI_ASC_LOGICAL_UNIT_NOT_READY                  0x04
#define SCSI_ASC_INVALID_FIELD_IN_CDB                    0x24
#define SCSI_ASC_WRITE_PROTECTED                         0x27
#define SCSI_ASC_FORMAT_ERROR                            0x31
#define SCSI_ASC_INVALID_COMMAND_OPERATION_CODE          0x20
#define SCSI_ASC_NOT_READY_TO_READY_CHANGE               0x28
#define SCSI_ASC_MEDIUM_NOT_PRESENT                      0x3A

#define SCSI_ASCQ_FORMAT_COMMAND_FAILED                  0x01
#define SCSI_ASCQ_INITIALIZING_COMMAND_REQUIRED          0x02
#define SCSI_ASCQ_OPERATION_IN_PROGRESS                  0x07

typedef struct
{
    uint32_t block_nbr;
    uint32_t block_size;
} Storage_CapacityTypeDef;

typedef struct 
{
    uint8_t data[8];
}SCSI_CapacityTypeDef;

typedef struct 
{
    uint32_t capacityListLength;
    SCSI_CapacityTypeDef capacity[4]; //理论上最多可以到达31个
}SCSI_FormatCapacityTypedef;


typedef struct
{
    uint8_t errorCode;
    uint8_t reserved0;
    uint8_t key;
    uint8_t infomation[4];
    uint8_t addtionLength;  //10
    uint8_t reserved1[4];
    uint8_t asc;
    uint8_t ascq;
    uint8_t reserved2;
} SCSI_SenseTypeDef;



typedef struct
{
    uint8_t PeripheralQualifier;
    uint8_t DeviceType;
    uint8_t RemovableMedia;
    uint8_t vendor_id[9];
    uint8_t product_id[17];
    uint8_t revision_id[5];
}SCSI_StdInquiryDataTypeDef;


typedef union
{
    struct __CBW
    {
        uint32_t Signature;
        uint32_t Tag;
        uint32_t DataTransferLength;
        uint8_t  Flags;
        uint8_t  LUN;
        uint8_t  CBLength;
        uint8_t  CB[16];
    }field;
    uint8_t data[31];
}
BOT_CBWTypeDef;

typedef union
{
    struct __CSW
    {
        uint32_t Signature;
        uint32_t Tag;
        uint32_t DataResidue;
        uint8_t  Status;
    }field;
    uint8_t data[13];
}
BOT_CSWTypeDef;

enum{
    DO_INQUIRY,
    DO_READ_CAPACITY,
    DO_TESTUNIT
};

typedef struct _USBH_CLASS_PROCESS_CB_
{
    void (*pOnGetInquiryCpl)(SCSI_StdInquiryDataTypeDef* inquiry);
    void (*pOnGetCapacityCpl)(Storage_CapacityTypeDef* storageCapacity);
    BOOL (*pTestUnitReadyCpl)(USBH_StatusTypeDef status);    //返回0表明不想继续了
    uint16_t (*pGetTestUnitReadyMaxTimes)(); //设置testUnitReady最大的次数
    void (*testUnitReadyDelayCb)(); 
}UsbClassProcessCb;

void usbh_msc_init();
USBH_StatusTypeDef bulk_send_cbw();
USBH_StatusTypeDef usbh_msc_scsi_request_sense(SCSI_SenseTypeDef *sense_data);
USBH_StatusTypeDef usbh_msc_scsi_inquiry(SCSI_StdInquiryDataTypeDef *inquiry);
USBH_StatusTypeDef usbh_msc_scsi_read_capacity(SCSI_CapacityTypeDef *capacity);
USBH_StatusTypeDef usbh_msc_scsi_read_format_capacity(SCSI_FormatCapacityTypedef *formatCapacity);
USBH_StatusTypeDef usbh_msc_scsi_test_unit_ready();
USBH_StatusTypeDef usbh_msc_process();
USBH_StatusTypeDef usbh_msc_read(uint32_t startSector, uint8_t* buffer, uint16_t sectorCount,BOOL block);
USBH_StatusTypeDef usbh_msc_write(uint32_t startSector, uint8_t* buffer, uint32_t sectorNums,BOOL block);
USBH_StatusTypeDef usbh_mcs_clear_feature();
USBH_StatusTypeDef usbh_mcs_recerve_csw();
USBH_StatusTypeDef  usbh_request_sense_loop();

USBH_StatusTypeDef usbh_do_get_inquiry(SCSI_StdInquiryDataTypeDef *inquiry);
USBH_StatusTypeDef usbh_do_read_capacity(SCSI_CapacityTypeDef *capacity);
USBH_StatusTypeDef usbh_do_read_format_capacity(SCSI_FormatCapacityTypedef *formatCapacity);


#endif

