#define LOG_LEV 4

#include "hal_ll_usb.h"
#include "user_config.h"
#include "device.h"
#include "event.h"
#include "usb.h"
#include "hal.h"
#include "event.h"
#include "usb_sys.h"

void register_pc_detect_cb(PcDetectCb* detectCb);

static BOOL _is_usb_initial = FALSE;


PcDetectCb pcDetectCb = {
    on_pc_out,
    on_pc_arrival
};

AT_SRAM(.usb_seg)
void on_pc_cable_plus_in()
{
    if(!_is_usb_initial) {
        logi("on_cable_in");
        _is_usb_initial = TRUE;
        event_put(USB_EVENT_CABLE_IN,0 ,0);
    }
}

UsbDetectCb g_usbDetectCb = {
#if USB_DEVICE_EN
    on_pc_cable_plus_in,
#else
    0,
#endif

#if USB_HOST_EN        
    on_hw_udisk_plus_in,
#else
    0
#endif
};


AT_SRAM(.usbd_audio)
void on_pc_arrival()
{
    logi("%s",__func__);
}

void on_pc_out()
{
    _is_usb_initial = FALSE;
    logi("%s",__func__);
}

void on_hw_udisk_plus_in()
{
    logi("%s",__func__);
    event_put(USB_EVENT_UDISK_SIGNAL_IN,0 ,0);
}

void usb_sys_clk_unit_init()
{
    module_ccu_enable(CCU_USB);
    HAL_cdelay(2000);
}

void usb_sys_module_init()
{
    logi("usbd init ++++");
#if USB_MODULE_ALWAYS_ON == 0    
    usb_detect_deinit();    //设备确认后不再使用USB检测模块
#endif    

    HAL_clk_set_src(CLK_USB, CLK_48M);
    HAL_clk_enable(CLK_USB);
    HAL_clk_bypass_usb_suspend_clk(true);

    hal_usb_bus_pull_down();

    HAL_gpio_config(PIN_B23 | PIN_FUNC15| PIN_DRI1);    //dm
    HAL_gpio_config(PIN_B22 | PIN_FUNC15| PIN_DRI1);    //dp
}

void usb_sys_module_deinit()
{
    logi("usbd module deinit---");
    HAL_clk_bypass_usb_suspend_clk(false);
    HAL_clk_disable(CLK_USB);
    module_ccu_disable(CCU_USB);
}

void usb_initialize()
{
    logi("usbd_detect_init-------------");
    usb_sys_clk_unit_init();
#if USB_MODULE_ALWAYS_ON && USB_DEVICE_EN
    event_put(USB_EVENT_CABLE_IN,0 ,0);
#else
#if USB_DEVICE_EN
    register_pc_detect_cb(&pcDetectCb);
#endif
    usb_detect_init(&g_usbDetectCb);
#endif
    //usbd_cardreader_init();
}

void usb_deinitialize()
{
    usb_sys_module_deinit();       //模块关闭后重置以消除一些残留
    HAL_mdelay(20);
    usb_sys_clk_unit_init();
    usb_detect_init(&g_usbDetectCb);
}

