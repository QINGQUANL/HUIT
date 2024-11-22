#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "sd.h"
#include "hal.h"
#include "event.h"
#include "device.h"
#include "app_device.h"

#if SD_EN

sd_info_ctl sd_info_ctl_t;

void sd_info_ctl_init(void)
{
    sd_info_ctl_t.sd_en         = SD_EN;
    sd_info_ctl_t.sd_detect_en  = SD_DETECT_EN;
    sd_info_ctl_t.clk_pin       = SD_CLK_PIN_NUM;
    sd_info_ctl_t.clk_func      = SD_CLK_FUNC;
    sd_info_ctl_t.sd_detect_pin = SD_DETECT_PIN_NUM;
    sd_info_ctl_t.sd_clkdiv     = SD_CLKDIV;
    sd_info_ctl_t.sd_pin_share  = 0;
    sd_info_ctl_t.sd_4wire_en   = SD_4WIRE_EN;
    sd_info_ctl_t.sd_d0_pin_cfg = SD_D0_PIN_CFG;
    sd_info_ctl_t.sd_d1_pin_cfg = SD_D1_PIN_CFG;
    sd_info_ctl_t.sd_d2_pin_cfg = SD_D2_PIN_CFG;
    sd_info_ctl_t.sd_d3_pin_cfg = SD_D3_PIN_CFG;
    sd_info_ctl_t.sd_clk_cfg    = SD_CMD_PIN_CFG;
    sd_info_ctl_t.sd_cmd_cfg    = SD_CLK_PIN_CFG;
}

void app_sd_init(void)
{
    logi("app_sd_init");
   sd_info_ctl_init();
   sd_init((void*)&sd_info_ctl_t);
}
#endif


AT_SRAM(.device)
void app_device_detect_cb(void)
{

#if  SD_EN
#if SD_DETECT_EN
    sd_detect();
#else
    event_put(DEVICE_EVENT_SD_IN, NULL, 0);
#endif
#endif

#if LINEIN_DETECT_EN
    linein_detect();
#endif

}



AT_SRAM(.device)
int device_initialize(uint32_t dev_num)
{
    logi("device_initialize: %d", dev_num);

#if SD_EN
    sd_detect_set(SD_DETACH_DISABLE);
    logi("%s,%d",__func__,__LINE__);
    for(uint8_t i=0;i<10;i++) {
        sd_disk_init_params_t params;
        params.buswide = SD_4WIRE_EN ? 4 : 1;
        if (HAL_sd_disk_init(&params) == 0) {
            device_sdcard_sta_set(DEV_OK);
            logi("sd init ok.");
            sd_detect_set(SD_DETACH_ENABLE);
            return 0;
        } else {
            HAL_msleep(30);
        }
    }
    device_sdcard_sta_set(DEV_FAIL);
    logi("sd init error.");
    sd_detect_set(SD_DETACH_ENABLE);
#endif
    logi("%s,%d",__func__,__LINE__);
    return -1;
}


AT_SRAM(.device)
int device_try_cb(uint32_t dev_num)
{
    if (device_is_online(dev_num)) {
        if (!device_is_initialized(dev_num)) {
            if (device_initialize(dev_num) < 0) {
                logd("init dev(%d) fail", dev_num);
                device_set_sta(dev_num, DEV_FAIL);
                return -1;
            }
        }
#if 0//TURNING_SPARK_TEMP

        else if ((DEV_UDISK == dev_num) && USB_DP_FUNC_DETECT == HAL_gpio_func_get(USB_DP_PIN_NUM)) {
            logi("force dp to working");
            HAL_gpio_pull_set(USB_DP_PIN_NUM, USB_DM_FUNC_WORKING);
        }
#endif
        device_set_num(dev_num);
        return 0;
    } else {
        return -2;
    }
}

void app_device_init(void)
{
    logi("app_device_init ++++++++++++++++");
#if (SD_EN)
    device_t dev;
    bool detect_enable;
    dev.cur_dev = DEV_NONE;

    dev.sdcard_sta = DEV_OFFLINE;
#if (SD_DETECT_EN == 0)
    dev.sdcard_sta = DEV_UNINITIALIZED;
    event_put(DEVICE_EVENT_SD_IN, NULL, 0);
    detect_enable = false;
#else
    detect_enable = true;
#endif
    app_sd_init();

    device_manager_init(&dev, detect_enable, app_device_detect_cb);
    device_register_try_callback(device_try_cb);
#endif
}
