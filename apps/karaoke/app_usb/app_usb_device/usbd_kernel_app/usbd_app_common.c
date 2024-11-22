
#define LOG_LEV 4
#include "hal_ll_usb.h"
#include "user_config.h"
#include "device.h"
#include "event.h"
#include "usb.h"
#include "hal.h"
#include "event.h"
#include "usbd_app_common.h"

uint8_t sof_count = 0;
OAL_TIMER _pc_out_detect_timer = NULL;
PcDetectCb* pc_detect_cb;

void register_pc_detect_cb(PcDetectCb* detectCb)
{
    pc_detect_cb = detectCb;
}


void usb_pc_out_detect_deinit()
{
    if (_pc_out_detect_timer != NULL)
    {
        OAL_timer_activate(&_pc_out_detect_timer, OAL_TIMER_NO_ACTIVATE);
        OAL_timer_delete(&_pc_out_detect_timer);
        _pc_out_detect_timer = NULL;
    }
}

void on_pc_detect_timer_close()
{
    if(_pc_out_detect_timer != NULL) {
        OAL_timer_activate(&_pc_out_detect_timer, OAL_TIMER_NO_ACTIVATE);
        OAL_timer_delete(&_pc_out_detect_timer);
        _pc_out_detect_timer = NULL;
    }

    if(pc_detect_cb) {
        if(pc_detect_cb->on_pc_out) {
            pc_detect_cb->on_pc_out();
        }
    }
}


uint32_t usbd_get_request_frame();
//检测sof
void usbd_pc_remove_detect_timer_cb()
{
    static uint8_t removeCount = 0;
    static uint32_t frame = 0;
    static uint32_t requeset_frame = 0;

    uint32_t curFrame = get_sof_frame();
    uint32_t currequestFrame = usbd_get_request_frame();
    if((frame == curFrame) && (currequestFrame == requeset_frame) ) {
        removeCount++;
        if(removeCount >= 3) {
            logi("PC out");
            removeCount = 0;
            event_put(USB_EVENT_CABLE_OUT,0 ,0);
            
            frame = 0;
            return;
        }
    }
    requeset_frame = currequestFrame;
    frame = curFrame;
}

AT_SRAM(.usbd_common)
void on_usb_sof_receive()
{
#if USB_MODULE_ALWAYS_ON == 0
    sof_count++;
    if(sof_count > 3) {
        sof_count = 0;
        usb_sof_isr_en(FALSE);  //已经接收到SOF，关掉其中断
        if(pc_detect_cb) {
            if(pc_detect_cb->on_pc_arrival) {
                pc_detect_cb->on_pc_arrival();
            }
        }
    }
#endif    
}

void usb_pc_out_detect_init()
{
    if(_pc_out_detect_timer == NULL) {
        OAL_timer_create(&_pc_out_detect_timer, "pcdetect", OAL_ms_to_ticks(700), usbd_pc_remove_detect_timer_cb,
            (void *)0, TRUE,OAL_TIMER_AUTO_ACTIVATE);
    }
}


