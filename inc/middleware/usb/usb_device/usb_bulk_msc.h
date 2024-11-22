#ifndef _BULK_MSC_H_
#define _BULK_MSC_H_

#define COMP_SENSE_LIST_DEEPTH                           4


/* SCSI Commands */
#define COMP_SCSI_FORMAT_UNIT                            0x04
#define COMP_SCSI_INQUIRY                                0x12
#define COMP_SCSI_MODE_SELECT6                           0x15
#define COMP_SCSI_MODE_SELECT10                          0x55
#define COMP_SCSI_MODE_SENSE6                            0x1A
#define COMP_SCSI_MODE_SENSE10                           0x5A
#define COMP_SCSI_ALLOW_MEDIUM_REMOVAL                   0x1E
#define COMP_SCSI_READ6                                  0x08
#define COMP_SCSI_READ10                                 0x28
#define COMP_SCSI_READ12                                 0xA8
#define COMP_SCSI_READ16                                 0x88

#define COMP_SCSI_READ_CAPACITY10                        0x25
#define COMP_SCSI_READ_CAPACITY16                        0x9E

#define COMP_SCSI_REQUEST_SENSE                          0x03
#define COMP_SCSI_START_STOP_UNIT                        0x1B
#define COMP_SCSI_TEST_UNIT_READY                        0x00
#define COMP_SCSI_WRITE6                                 0x0A
#define COMP_SCSI_WRITE10                                0x2A
#define COMP_SCSI_WRITE12                                0xAA
#define COMP_SCSI_WRITE16                                0x8A

#define COMP_SCSI_VERIFY10                               0x2F
#define COMP_SCSI_VERIFY12                               0xAF
#define COMP_SCSI_VERIFY16                               0x8F

#define SCSI_FW_UPGRADE                         0xC0


#define COMP_SCSI_SEND_DIAGNOSTIC                        0x1D
#define COMP_SCSI_READ_FORMAT_CAPACITIES                 0x23

#define COMP_NO_SENSE                                    0
#define COMP_RECOVERED_ERROR                             1
#define COMP_NOT_READY                                   2
#define COMP_MEDIUM_ERROR                                3
#define COMP_HARDWARE_ERROR                              4
#define COMP_ILLEGAL_REQUEST                             5
#define COMP_UNIT_ATTENTION                              6
#define COMP_DATA_PROTECT                                7
#define COMP_BLANK_CHECK                                 8
#define COMP_VENDOR_SPECIFIC                             9
#define COMP_COPY_ABORTED                                10
#define COMP_ABORTED_COMMAND                             11
#define COMP_VOLUME_OVERFLOW                             13
#define COMP_MISCOMPARE                                  14


#define COMP_INVALID_CDB                                 0x20
#define COMP_INVALID_FIELED_IN_COMMAND                   0x24
#define COMP_PARAMETER_LIST_LENGTH_ERROR                 0x1A
#define COMP_INVALID_FIELD_IN_PARAMETER_LIST             0x26
#define COMP_ADDRESS_OUT_OF_RANGE                        0x21
#define COMP_MEDIUM_NOT_PRESENT                          0x3A
#define COMP_MEDIUM_HAVE_CHANGED                         0x28
#define COMP_WRITE_PROTECTED                             0x27
#define COMP_UNRECOVERED_READ_ERROR                0x11
#define COMP_WRITE_FAULT                    0x03

#define COMP_READ_FORMAT_CAPACITY_DATA_LEN               0x0C
#define COMP_READ_CAPACITY10_DATA_LEN                    0x08
#define COMP_MODE_SENSE10_DATA_LEN                       0x08
#define COMP_MODE_SENSE6_DATA_LEN                        0x04
#define COMP_REQUEST_SENSE_DATA_LEN                      0x12
#define COMP_STANDARD_INQUIRY_DATA_LEN                   0x24
#define COMP_BLKVFY                                      0x04

#define USBD_COMP_BOT_IDLE                      0       /* Idle state */
#define USBD_COMP_BOT_DATA_OUT                  1       /* Data Out state */
#define USBD_COMP_BOT_DATA_IN                   2       /* Data In state */
#define USBD_COMP_BOT_LAST_DATA_IN              3       /* Last Data In Last */
#define USBD_COMP_BOT_SEND_DATA                 4       /* Send Immediate data */
#define USBD_COMP_BOT_NO_DATA                   5       /* No data Stage */
#define USBD_COMP_BOT_CSW                       6
/* CSW Status Definitions */
#define USBD_COMP_CSW_CMD_PASSED                0x00
#define USBD_COMP_CSW_CMD_FAILED                0x01
#define USBD_COMP_CSW_PHASE_ERROR               0x02

#define USBD_COMP_BOT_STATUS_NORMAL             0
#define USBD_COMP_BOT_STATUS_RECOVERY           1
#define USBD_COMP_BOT_STATUS_ERROR              2


#define COMP_MSC_MEDIA_PACKET       64

typedef struct
{
    uint32_t dSignature;
    uint32_t dTag;
    uint32_t dDataLength;
    uint8_t  bmFlags;
    uint8_t  bLUN;
    uint8_t  bCBLength;
    uint8_t  CB[16];
    uint8_t  ReservedForAlign;
}
USBD_COMP_MSC_BOT_CBWTypeDef;


typedef struct
{
    uint32_t dSignature;
    uint32_t dTag;
    uint32_t dDataResidue;
    uint8_t  bStatus;
    uint8_t  ReservedForAlign[3];
}
USBD_COMP_MSC_BOT_CSWTypeDef;

typedef struct _COMP_SENSE_ITEM {
    char Skey;
    union {
        struct _ASCs {
            char ASC;
            char ASCQ;
        }b;
        unsigned int    ASC;
        char *pData;
    } w;
} USBD_COMP_SCSI_SenseTypeDef;

typedef struct
{
    uint32_t                 interface;
    uint8_t                  bot_state;
    uint8_t                  bot_status;
    uint16_t                 bot_data_length;
    __ALIGN_BEGIN uint8_t bot_data[512] __ALIGN_END;
    USBD_COMP_MSC_BOT_CBWTypeDef  cbw;
    USBD_COMP_MSC_BOT_CSWTypeDef  csw;

    USBD_COMP_SCSI_SenseTypeDef   scsi_sense[COMP_SENSE_LIST_DEEPTH];
    uint8_t                  scsi_sense_head;
    uint8_t                  scsi_sense_tail;

    uint32_t                 scsi_blk_size;
    uint32_t                 scsi_blk_nbr;

    uint64_t                 scsi_blk_addr;
    uint64_t                 scsi_blk_len;
}
USBD_COMP_MSC_BOT_HandleTypeDef;

typedef struct _USBD_COMP_STORAGE
{
    int8_t(*Init) ();
    int8_t(*GetCapacity) (uint32_t *block_num, uint16_t *block_size);
    BOOL(*IsReady) ();
    BOOL(*IsWriteProtected) ();
    int8_t(*Read) (uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
    int8_t(*Write)(uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
    int8_t(*UsbExtendCommand)(uint8_t *params);
    int8_t *pInquiry;
    BOOL   (*pre_transmit_cmd)(uint8_t sisc_cmd);

}USBD_COMP_StorageTypeDef;

void register_callback(USBD_COMP_StorageTypeDef* storageCb);
#endif


