#ifndef _USB_H_
#define _USB_H_
#include "hal_gpio.h"
#include "hal_sdmmc_io.h"

enum usb_detect_status_e {
    USB_DETECT_INIT = 0,
    USB_DETECT,
    USB_WORKING,
    USB_DISABLE,
};

typedef struct {
    enum usb_detect_status_e sta;
    uint8_t host_out_cnt;
    uint8_t host_in_cnt;
    uint8_t pc_in_cnt;
    uint8_t pc_out_cnt;
    uint32_t sof_frame_num;
    uint8_t dm_status_check_cnt;
    uint8_t edge_cnt;
    bool sof_check_en;
    bool dm_status_check_en;
    bool edge_check_en;
} usb_det_t;
extern usb_det_t usb_det;

enum usb_detect_mode_e {
    USB_HOST = 0,
    USB_DEVICE,
};

typedef struct _USB_DETECT_CALLBACK_
{
    void (*on_usb_cable_plus_in)();
    void (*on_udisk_plus_in)();
}UsbDetectCb;

void usb_detect_init(UsbDetectCb* pUsbDetectCb);
void usb_detect_deinit();


#define USB_HOST_DET_CNT        10  //20ms * 10
#define USB_DEVICE_DET_CNT      20  //20ms * 20     /* when some udisks are inserted, the DM will be low for 160ms */

#define DEVICE_USER_HANDLE_CFG                      1
#define DEVICE_USER_MSP_INIT                        2
#define DEVICE_USER_MSP_DEINIT                      3
#define DEVICE_USER_COMP_SOF                        4
#define DEVICE_USER_COMP_IsoINIncomplete            5
#define DEVICE_USER_COMP_IsoOutIncomplete           6
#define DEVICE_USER_COMP_DEBUG                      7
#define DEVICE_USER_COMP_EP0TXRDY                   8
#define DEVICE_USER_COMP_AudioDataIn                9
#define DEVICE_USER_COMP_AudioEP0TxReady            10
#define DEVICE_USER_COMP_AudioSOF                   11
#define DEVICE_USER_COMP_AudioIsoINIncomplete       12
#define DEVICE_USER_COMP_AudioIsoOUTIncomplete      13
#define DEVICE_USER_COMP_AudioRecCallback           14
#define DEVICE_USER_COMP_HIDGetReportData           15
#define DEVICE_USER_COMP_AudioRecordCallback        16

#define USBD_COMP_AUDIO_FREQ                        48000

#define USB_HS_MAX_PACKET_SIZE                      512
#define USB_FS_MAX_PACKET_SIZE                      64
#define USB_MAX_EP0_SIZE                            64

#define COMP_AUDIO_OUT_PACKET                       (uint32_t)(((USBD_COMP_AUDIO_FREQ * 2 * 2) /1000))
#define COMP_AUDIO_OUT_PACKET_NUM                   8
#define COMP_AUDIO_TOTAL_BUF_SIZE                   ((uint32_t)(COMP_AUDIO_OUT_PACKET * COMP_AUDIO_OUT_PACKET_NUM))

typedef enum
{
  COMP_AUDIO_OFFSET_NONE = 0,
  COMP_AUDIO_OFFSET_HALF,
  COMP_AUDIO_OFFSET_FULL,
  COMP_AUDIO_OFFSET_UNKNOWN,
}
COMP_AUDIO_OffsetTypeDef;

#define HOST_USER_SELECT_CONFIGURATION      1
#define HOST_USER_CLASS_ACTIVE              2
#define HOST_USER_CLASS_SELECTED            3
#define HOST_USER_CONNECTION                4
#define HOST_USER_DISCONNECTION             5
#define HOST_USER_UNRECOVERED_ERROR         6
#define HOST_USER_MSP_INIT                  7
#define HOST_USER_MSP_DEINIT                8
#define HOST_USER_HANDLE_CFG                9
#define HOST_USER_SOFPROCESS                10


#if (USB_DM_PIN_SEL == 0)
#define USB_DM_PIN_CFG          (PIN_B3 | PIN_FUNC15 | PIN_DRI1)
#define USB_DM_PIN_NUM          (PIN_B3)
#define USB_DM_FUNC_WORKING     (PIN_FUNC15)
#define USB_DM_FUNC_DETECT      (PIN_FUNCINPUT)
#define USB_DM_FUNC_WKUP        (PIN_FUNC14)
#define USB_DM_PULL_DISABLE     (PIN_PULL_DISABLE)
#define USB_DM_PULL_DETECT      (PIN_PULL_UP100K)
#define USB_DM_PULL_WKUP        (PIN_PULL_UP100K)
#define USB_DM_DRI              (PIN_DRI1)
#elif (USB_DM_PIN_SEL == 1)

#endif

#if (USB_DP_PIN_SEL == 0)
#define USB_DP_PIN_CFG          (PIN_B4 | PIN_FUNC15 | PIN_DRI1)
#define USB_DP_PIN_NUM          (PIN_B4)
#define USB_DP_FUNC_WORKING     (PIN_FUNC15)
#define USB_DP_FUNC_DETECT      (PIN_FUNCINPUT)
#define USB_DP_FUNC_WKUP        (PIN_FUNC14)
#define USB_DP_PULL_DISABLE     (PIN_PULL_DISABLE)
#define USB_DP_PULL_DETECT      (PIN_PULL_DOWN100K)
#define USB_DP_PULL_WKUP        (PIN_PULL_DOWN100K)
#define USB_DP_DRI              (PIN_DRI1)

#elif (USB_DP_PIN_SEL == 1)

#endif

#endif
