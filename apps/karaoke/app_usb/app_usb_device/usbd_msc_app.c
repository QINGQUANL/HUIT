#define LOG_LEV 4

#include "hal_ll_usb.h"
#include "user_config.h"
#include "device.h"
#include "event.h"
#include "usb.h"
#include "hal.h"
#include "event.h"
#include "usbd_msc.h"
#include "usbd_app_common.h"

#define LUN0    0

#define STATUS_SD_OUT   0
#define STATUS_SD_IN    1

BOOL usb_try_sd_call_back(uint32_t dev_num);
void HAL_sd_switch_card(uint8_t cardNum);
void HAL_set_work_sd_num(uint8_t curNum);
DRESULT usb_sd_disk_read(BYTE *buff, DWORD sector, UINT count,uint8_t lun);
DRESULT usb_sd_disk_read_start(BYTE *buff, DWORD sector,uint8_t lun);
DRESULT usb_sd_disk_write(BYTE *buff, DWORD sector, UINT count,uint8_t lun);
void usb_sd_test_unit(uint8_t lun);
uint8_t usd_sd_wait_ready(uint32_t timeout, uint8_t lun);
void sd_switch_io(uint8_t lun);

uint8_t yp3_get_sd_status(uint8_t sd_id);

static BOOL s_usb_cardreader_init;
static uint8_t s_sd_remove_detect_count = 0 ;


int8_t cardReadString[36] = {
    0x00,
    0x80,
    0x02,
    0x02,
    (0x25 - 5),   //bug here 0x24
    0x00,
    0x00,
    0x10,
    'S', 'M', 'T', 'L', 'I', 'N', 'K', ' ', /* Manufacturer: 8 bytes  */
    'U', 'D', 'I', 'S', 'K', ' ', ' ', ' ', /* Product     : 16 Bytes */
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    '1', '.', '0', '0',                      /* Version     : 4 Bytes  */
};


UsbdCardreaderSdFunction g_SdOpFunction = {
    HAL_sd_switch_card, //切换卡
    HAL_sd_disk_ioctl,

    usb_sd_disk_read,  //同步读，发起读以后要等待读完成
    usb_sd_disk_read_start, //异步读，发起后退出
    usd_sd_wait_ready,  //异步读状态获取，异步读发起后会在某个会调用到这里

    usb_sd_disk_write, //同步写，等到完成
    0,  //异步写等待，暂时保留在这里
    0,  //异步写，暂时没有用到
    HAL_sd_get_status,
    usb_sd_test_unit    //检测SD的是否移走
};

void usbd_cardreader_register_sd_wr_cb()
{
    usbd_register_sd_wr_cb(&g_SdOpFunction);
}

AT_SRAM(.usb_cardreader)
DRESULT usb_sd_disk_read(BYTE *buff, DWORD sector, UINT count, uint8_t lun)
{
    DRESULT res = 0;
    s_sd_remove_detect_count = 0;
    if(usbd_is_cardreader_enable(lun)) {
        res = HAL_sd_disk_read2(buff,sector,count);
        if(res) {
            logi("%s - %d, res =%x",__func__,lun, res);
            usbd_msc_enable_cardreader(lun, FALSE);
        }
    }
    return res;
}

AT_SRAM(.usb_cardreader)
DRESULT usb_sd_disk_read_start(BYTE *buff, DWORD sector, uint8_t lun)
{
    s_sd_remove_detect_count = 0;
    if(usbd_is_cardreader_enable(lun)) {
        HAL_sd_disk_read_start(buff,sector);
    }
    return 0;
}

AT_SRAM(.usb_cardreader)
DRESULT usb_sd_disk_write(BYTE *buff, DWORD sector, UINT count ,uint8_t lun)
{
    uint8_t res = 0;
    s_sd_remove_detect_count = 0;
    if(usbd_is_cardreader_enable(lun)) {    //有可能还会连续发过来
        res = HAL_sd_disk_write(buff,sector,count);
        if(res != 0) {
            usbd_msc_enable_cardreader(lun, FALSE);
        }
    }
    return res;
}

AT_SRAM(.usb_cardreader)
uint8_t usd_sd_wait_ready(uint32_t timeout, uint8_t lun)
{
    DRESULT res = 1;
    if(usbd_is_cardreader_enable(lun)) {
        res = HAL_wait_sd_ready(timeout);
        if(res != 0) {
            logi("%s %d ,lun %d",__func__,res,lun);
            usbd_msc_enable_cardreader(lun, FALSE);
        }
    }
    return res;
}

//卡移除了返回true
AT_SRAM(.usb_cardreader)
bool is_sdcard_detach()
{
    sd_detect_set(1);
    HAL_mdelay(2);

    bool isDetach = sd_get_detect_gpio_status();
    logi("isDetach %d", isDetach);
    sd_detect_set(0);
    
    return isDetach;
}


AT_SRAM(.usb_cardreader)
void usb_sd_test_unit(uint8_t lun)
{
#if SD_DETECT_EN
    //读写的时候频繁进入这里会拖慢读写速度，所以做下限制
    if(s_sd_remove_detect_count > 2) {
        if(usbd_is_cardreader_enable(LUN0))
        {
            if(is_sdcard_detach())
            {
                logi("sd0 out");
                usbd_msc_enable_cardreader(LUN0,FALSE);
                return;
            }
        } else {
            if(is_sdcard_detach() == 0)
            {
                logi("sd0 in");
                HAL_sd_switch_card(DEV_SDCARD);
                if(usb_try_sd_call_back(DEV_SDCARD))
                {
                    return ;
                }
            }
        }

        s_sd_remove_detect_count = 0;
    }
    s_sd_remove_detect_count++;
#else
    usbd_msc_enable_cardreader(LUN0,TRUE);
#endif
}

//不要删除这个函数，保持空
AT_SRAM(.usb_cardreader)
void HAL_sd_switch_card(uint8_t cardNum)
{
}

DSTATUS HAL_sd_deinit();
uint8_t usbd_device_sd_initialize(uint8_t lun)
{
    sd_clk_init();

    sd_disk_init_params_t params;
    params.buswide = SD_4WIRE_EN ? 4 : 1;
    params.workClkDiv = SD_CLKDIV;
    params.sd_num = lun;
    params.busMode = SD_MULTI_WR_PERFORMANCE_MODE;
    sd_pin_init();

    return HAL_sd_disk_init(&params) ;
}

AT_SRAM(.usb_cardreader)
BOOL usb_try_sd_call_back(uint32_t dev_num)
{
    //调用usbd_msc_enable_cardreader以通知USB，媒体已经初始化完成，可以访问
    if(usbd_device_sd_initialize(DEV_SDCARD) == 0) {
        //先调用usbd_mscs_cardreader_init，然后再调用usbd_msc_enable_cardreader
        logi("sd disk read pas ---->>>");
        usbd_msc_enable_cardreader(LUN0,TRUE); //SD初始化成功后调用这个函数以切换状态
    } else {
        return FALSE;
    }

    return TRUE;
}

void usbds_cardreader_init()
{
    if (s_usb_cardreader_init)
    {
        return;
    }

    usb_detect_deinit(); //关闭USB内置的检测模块

    s_usb_cardreader_init = TRUE;
    logi("usbds_cardreader_init");
    //线获取之前注册的函数，USB退出的时候重新改回去
    HAL_app_register_sd_callback(0); //自己判断状态，不用回调

    // dis bt
    INTC_DisableIRQ(IRQn_BT);

    usbd_msc_cardreader_init();
    usbd_cardreader_register_sd_wr_cb();

    INTC_SetPriority(IRQn_SDMMC, IRQn_SDMMC_PRIO);
    INTC_SetPriority(IRQn_USB, 15);

    ////直接初始化成MULTI_READ模式，因为其它流程可能是使用SINGLE MODE方式
    usbd_msc_enable_cardreader(0, FALSE);

    if (usbd_device_sd_initialize(LUN0) == 0)
    {
        usbd_msc_enable_cardreader(LUN0, TRUE);
    }

    //usbd_set_inquiry_string(cardReadString);  //设置自定义的字符串

    usb_sof_isr_en(FALSE); //不需要监控SOF
    usbd_start();
    OAL_sleep(2000); //等待两秒钟后再启动PC拔出的定时器
    //调整定时器的时间会影响USB的检测，如果想快点就调小点
    usb_pc_out_detect_init();
}

bool is_usb_cardreader_device_attach()
{
    return s_usb_cardreader_init;
}

void sd_detect_force();
void sd_hal_cb(uint8_t cmd, uint32_t *data);

void usbd_cardread_deinit()
{
    logi("usbd_cardread_deinit");
    s_usb_cardreader_init = FALSE;
    usb_pc_out_detect_deinit();
    USB_Disconnect();
    usbd_msc_cardreader_deinit();
    sd_detect_force();
    HAL_app_register_sd_callback(sd_hal_cb);    //回复原先的SD机制
    device_detect_timer_start();    //恢复SD检测
}

void usbd_cardreader_initialize()
{
    device_detect_timer_stop(); //停止原先的SD检测，使用USB的testUnitReady来检测
    usbds_cardreader_init();
}

