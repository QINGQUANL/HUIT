#define LOG_LEV 4
#include "hal_ll_usb.h"
#include "user_config.h"
#include "device.h"
#include "event.h"
#include "usb.h"
#include "hal.h"
#include "event.h"
#include "usbd_audio_app.h"
#include "usb_bulk_msc.h"
#include "usbd_app_common.h"
#include "usbd_msc_app.h"
#include "usb_sys.h"

void on_pc_arrival();
void on_pc_out();

void usbd_initialize()
{
    
}

void usbd_deinitialize()
{

#if USB_DEVICE_TYPE == USB_DEVICE_TYPE_CARDREADER
    usbd_cardread_deinit();
    usb_deinitialize();
#else
#if (USBD_MODE == USB_MODE_SPEAKER_MIC) || (USBD_MODE == USB_MODE_MIC || (USBD_MODE == USB_MODE_TWO_AUDIO_CONFIG))
    logi("usbd_deinit ---- ");
    usbd_audio_app_deinit();
    usb_deinitialize();    
#endif

#endif
}

//--------------------- 升级相关 -----------------------------
int8_t s_updateString[16] = {
    'U', 'P', 'D', 'A', 'T', 'E', ' ', 'F', 
    'R', 'O', 'M', ' ', 'A', 'U', 'D', 'I', /* Product     : 16 Bytes */
};

extern uint8_t COMP_STORAGE_Inquirydata[];

void boot_usb_init(void);
void usbd_msc_init(void);
uint32_t boot_flash_init(void);
BOOL hal_is_pc_in();
uint32_t try_update_pc(void);

AT_SRAM(.usb_update)
void pc_update_process()
{
    memcpy(COMP_STORAGE_Inquirydata + 16 , s_updateString, 16);
    
    HAL_norflash_workmode_change(0);
    boot_usb_init();
    usbd_msc_init();
    register_callback(0);
    usbd_start();

    while(1){}
}


