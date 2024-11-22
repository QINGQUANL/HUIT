
#ifndef _SETUP_
#define _SETUP_

#include "sys_types.h"
#include "usbd_def.h"
#include "hal_ll_usb.h"

typedef struct _INTERFACE_INDEX_
{
    uint8_t audio_ctrl_index;
    uint8_t audio_speaker_index;
    uint8_t audio_mic_index;
    uint8_t hid_index;
    uint8_t msc_index;
}InterfaceIndex;

typedef struct _SETUP_EVENT_CALLBACK_
{
    void (*OnSetAddr)();
    void (*OnSetInterface)();
    void (*OnClearFeature)();
    void (*OnSetFeature)();
    void (*OnSetConfiguation)();

    BOOL (*OnPreStandardSetupProcess)();
    BOOL (*OnEp0_pre_setup)();
    void (*OnEp0_setup_processed)(uint8_t wParam);
}SetupEventCallback;

enum {
    REQUEST_TO_DEVICE,
    REQUEST_TO_INTERFACE,
    REQUEST_TO_ENDPOINT
};

//Feature Selector
enum {
    ENDPOINT_STALL,
    REMOTE_WAKEUP,
    TEST_MODE
};

void ep0_setup();
void setup_comp_init();
void setup_msc_init();
BOOL standard_setup();
BOOL class_setup();
bool usb_ep0_get_descriptor();
bool usb_ep0_set_address();
bool usb_ep0_set_configuration(void);
bool usb_ep0_set_interface(void);
bool usb_ep0_set_address();
BOOL standard_req_clear_feature();
BOOL standard_req_set_feature();
BOOL standard_req_get_status();
bool usb_ep0_get_configuration(void);
bool usb_ep0_get_interface(void);
void usb_check_faddr(void);
void usb_ep0_receive(void);
void set_config_desc_length(int length);
void set_config_desc(uint8_t* pDesc);

#endif
