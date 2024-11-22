#define LOG_LEV 4
#include "sys_inc.h" // libsys.a
#include "oal.h" // liboal.a
#include "hal.h" //libhal.a
#include "system.h" //libmuddleware.a
#include "usbd_def.h"
#include "usb_hid.h"
#include "usb_hid_app.h"
#include "event.h"

void usbd_ps4_device_test_unit();


OAL_TIMER HidKeyDownTimerHandle;
BOOL g_hid_send_done ;
uint8_t ps4_test_unit_set = 0xFF;

void usbd_hid_reset()
{
    ps4_test_unit_set = 0xFF;
}

void usbd_hid_app_init()
{
    if(ps4_test_unit_set == 0xFF) {
        event_put(USB_EVENT_TEST_HOST_IS_PS4, 0, 0);
        ps4_test_unit_set = 0;
        g_hid_send_done = TRUE;
        usbd_ps4_device_test_unit();
    }
}
//HID数据发送完毕后会进入这里
AT_SRAM(.usbd_audio)
void on_hid_in_data_tx_complete(type_usb_dev* pUsbdev)
{
    logi("on_hid_in_data_tx_complete %d", ps4_test_unit_set);
    //通过判断这个变量来确认是都是PS4
    //当g_hid_send_done = TRUE，说明不是PS4
    //当g_hid_send_done = FALSE， 说明没有枚举或者就是PS4设备
    
    g_hid_send_done = TRUE; //没有进入这个回调，不能再次发送HID数据
    if(ps4_test_unit_set == 0) {
        g_hid_send_done = FALSE;
        usbd_ps4_device_test_unit();
        ps4_test_unit_set++;
    }
}

void hid_send_data_strict(uint8_t* data, uint16_t length)
{
    logi("hid_send_data_strict");
    if(g_hid_send_done) {
        g_hid_send_done = FALSE;
        logi("hid send data...%x", data[0]);
        usbd_hid_send_data (data, length);
    }
}

void hid_key_up()
{
    uint8_t keyUp = 0;
    hid_send_data_strict(&keyUp, 1);
}

void usbd_hid_key_push(E_HID_KEY keyState)
{   
    //if(dev.pc_sta == DEV_OFFLINE)
    //    return;
    
    //if(usbd_get_max_packet() == 0)
    //    return;

    hid_send_data_strict (&keyState, 1);
    if (HidKeyDownTimerHandle == 0) {
        OAL_timer_create(&HidKeyDownTimerHandle, "hid_key_up", OAL_ms_to_ticks(50),
            hid_key_up, (void *)1, FALSE, OAL_TIMER_AUTO_ACTIVATE);
    } else {
        OAL_timer_activate(&HidKeyDownTimerHandle, OAL_TIMER_AUTO_ACTIVATE);
    }
}

void hal_set_index(uint8_t index);
void *USB_ReadPacket_New(uint8_t *dest, uint16_t len, uint8_t epnum);
///   usbd_hid_send_data  用来发送HID数据，最大长度32
AT_SRAM(.usb_ep3_hid)
void usb_ep3_hid_out_proc(type_usb_dev* pUsbdev)
{
    uint8_t dest[32];
    hal_set_index(EP_HID);   //切换USB FIFO
    uint16_t legnth = USB_GetRxLength(EP_HID);
    uint16_t readlegnth = legnth;
    if(readlegnth & 0x03) {
        readlegnth &= ~0x03;
        readlegnth += 4;
    }
    
    logi("reallegnth %d, %d", legnth, readlegnth);
    USB_ReadPacket(dest, readlegnth, EP_HID);
    
    printf_array(dest, legnth);

    //数据处理完毕后准备接收下一包数据
    USBD_EPx_prepare_receive(EP_HID);
}


void usbd_ps4_device_test_unit()
{
    uint8_t a = 0;
    hid_send_data_strict(&a, 1);
}

BOOL usbd_is_target_maybe_ps4()
{
    logi("g_hid_send_done %d %d", g_hid_send_done, ps4_test_unit_set);
    return !g_hid_send_done;  //只有能进入on_hid_in_data_tx_complete回调就不是PS4
}


