#define LOG_LEV 4
#include "sys_inc.h" // libsys.a
#include "oal.h" // liboal.a
#include "hal.h" //libhal.a
#include "system.h" //libmuddleware.a
#include "usbd_def.h"
#include "usbd_desc.h"
#include "usbd_audio.h"
#include "usbd_audio_app.h"
#include "usb_hid.h"
#include "setup.h"
#include "event.h"

int library_main_usb()
{
    logi("hello usb");
    return 0;
}