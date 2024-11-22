#define LOG_LEV 4

#include "sys_inc.h" // libsys.a
#include "oal.h" // liboal.a
#include "hal.h" //libhal.a
#include "system.h" //libmuddleware.a
#include "audition_usb_app.h"
#include "setup.h"
#include "pcm.h"
#include "mixer.h"
#include "user_config.h"
#include "audio_dump.h"
//#include "audio.h"

#if USBD_MODE == USB_MODE_DATA_COLLECTOR

#define IN
#define OUT

uint8_t ep0_state;
SetupEventCallback g_setupEventCallback;
void usbd_register_setup_event(SetupEventCallback* setupEventCb);
extern void msc_data_prepare_receive(uint8_t *pbuf, uint16_t size);
extern void msc_data_transmit_dma(uint8_t *pbuf, uint16_t size);
extern void msc_data_transmit(uint8_t *pbuf, uint16_t size);
extern void USB_FlushRxFifo(uint32_t num);
extern void USB_FlushTxFifo(uint32_t num);

void bulk_dma_complete();


extern PCB_USB_STATE_CB pcd_usb_state_cb;
extern PCD_EP_PROC pcd_ep_proc;
extern USBD_SetupReqTypedef setupReq;
extern USBD_DescriptorsTypeDef COMP_Desc;
extern type_usb_dev usb_dev;

uint32_t g_rxBuff;
uint32_t g_usbRxCount;

uint32_t g_txBuff;
uint32_t g_txCount;

#define USBD_EP0_IDLE       0
#define USBD_EP0_DATA_OUT   1
#define USBD_EP0_DATA_IN    2

#define USB_REQ_VENDOR_AUDIO_PACKET_PARAM  0xB0        //return UsbAuditionPacketParam
#define USB_SET_VENDOR_AUDIO_PARAM  0xC0
#define CMD_SET_AUDITION_CONTROL	0xD0

uint8_t pcmBuff[1024] __attribute__((aligned(4)));
uint8_t recBuff[1024] __attribute__((aligned(4)));

UsbAuditionCb g_usbAuditionActionCb;
UsbAuditionPacketParam g_usbAudionPacketParam;
UsbAudidtionParam g_UsbAuditionParam;
UsbAuditionControl g_usbAuditionControl;

AT_SRAM(.usbd_seg)
uint32_t SetupPcmBuff()
{
    return (uint32_t)pcmBuff;
}

AT_SRAM(.usbd_seg)
uint32_t onPrepareMicData(uint32_t IN length)
{
#if 0 //验证USB中断的实时性
        {
        static uint32_t last_t;
        uint32_t cur_t = HAL_timestamp_to_us(HAL_get_timestamp());
        uint32_t delta_t = cur_t - last_t;
        last_t = cur_t;
        logx("R(%d, %d)", delta_t, length);
        }
        int16_t *p16 = (int16_t*)recBuff;
        static int16_t kkval;
        for (int i = 0; i < 192; i++) {
            p16[2*i+1] = p16[2*i] = kkval;
            kkval++;
        }
#endif
        audio_pcm_dump_data_read(recBuff, length);
        
        return (uint32_t)recBuff;

}

AT_SRAM(.usbd_seg)
uint32_t onPcmReceive(uint32_t IN length)
{
    static uint32_t aa = 0;
    uint32_t time = HAL_timestamp_to_us(HAL_get_timestamp());
    logi("time %d",time - aa);
    aa = time;
    //printf_array(pcmBuff,length);
    return (uint32_t)pcmBuff;
}


AT_SRAM(.usbd_seg)
void onPlayParamChange(uint32_t sampleRate,uint8_t sampleBits,uint8_t channels)
{
    logi("%s, %d,%d,%d",__func__,sampleRate, sampleBits, channels);
}

AT_SRAM(.usbd_seg)
void onRecordParamChange(uint32_t sampleRate,uint8_t sampleBits,uint8_t channels)
{
    logi("%s, %d,%d,%d",__func__,sampleRate, sampleBits, channels);

    //info.rec_fs_set = sampleRate;
    //info.rec_bits_set = sampleBits;
    //info.rec_ch_set = channels;
    //info.rec_param_update = 1;
}

AT_SRAM(.usbd_seg)
void onPlayControlChange(BOOL bPause1)
{
    logi("%s, %d",__func__, bPause1);
}

AT_SRAM(.usbd_seg)
void onRecordControlChange(BOOL bPause1)
{
    logi("%s,%d",__func__,bPause1);
}

uint16_t usbd_get_audition_pid()
{
    return 0x4901;
}

void usbd_audition_init()
{
    g_usbAuditionActionCb.SetupPcmBuff = SetupPcmBuff;
    g_usbAuditionActionCb.onPrepareMicData = onPrepareMicData;
    g_usbAuditionActionCb.onPcmReceive = onPcmReceive;
    g_usbAuditionActionCb.onPlayParamChange = onPlayParamChange;
    g_usbAuditionActionCb.onRecordParamChange = onRecordParamChange;
    g_usbAuditionActionCb.onPlayControlChange = onPlayControlChange;
    g_usbAuditionActionCb.onRecordControlChange = onRecordControlChange;

    g_setupEventCallback.OnPreStandardSetupProcess = PreStandardSetupProcess;
    g_setupEventCallback.OnEp0_pre_setup = OnEp0_pre_setup;
    usbd_register_setup_event(&g_setupEventCallback);
    ep0_state = USBD_EP0_IDLE;



    COMP_Desc.GetUsbPID = usbd_get_audition_pid;

    pcd_usb_state_cb.initialize_cb = usbd_audition_device_init_cb;//注册相关回调
    //pcd_usb_state_cb.sof_cb;
    pcd_ep_proc.bulk_out_proc = bulk_out_proc;
    pcd_ep_proc.bulk_in_proc = bulk_in_proc;
    pcd_ep_proc.bulk_dma_complete = bulk_dma_complete;
    hal_pcd_init(&pcd_ep_proc, &pcd_usb_state_cb);
    //winusb_module_init(&usbAuditionChangeCb);
}

AT_SRAM(.usbd_seg)
uint16_t cal_pack_size(uint32_t sampleRate,uint8_t sampleBits,uint8_t channels)
{
    int bytes ;
    if(g_UsbAuditionParam.sampleBites == 24)
        bytes = 4;
    else
        bytes = g_UsbAuditionParam.sampleBites/8;

    return sampleRate/1000 * bytes * channels;
}

uint32_t g_txDataCount = 0;
AT_SRAM(.usbd_seg)
void bulk_in_proc(type_usb_dev* pUsbdev)
{
    if(g_txDataCount != 0) {
        g_txBuff += g_usbAudionPacketParam.recPacketLength / 64 * 64;
        //USB_EPnStartTX((uint8_t*)g_txBuff, sendCount, EP_BULK);
        msc_data_transmit((uint8_t*)g_txBuff, g_txDataCount);
        g_txDataCount = 0;

    } else {
        g_txBuff = g_usbAuditionActionCb.onPrepareMicData(g_usbAudionPacketParam.recPacketLength);
        //msc_data_transmit ((uint8_t*)g_txBuff,g_usbAudionPacketParam.recPacketLength);
        msc_data_transmit_dma_new ((uint8_t*)g_txBuff,g_usbAudionPacketParam.recPacketLength/64 * 64);
        g_txDataCount = g_usbAudionPacketParam.recPacketLength % 64;
    }
}

AT_SRAM(.usbd_seg)
void bulk_out_proc(type_usb_dev* pUsbdev)
{
    uint32_t rxCount = 64;
    if(g_usbRxCount > 64) {
        USB_ReadPacket((uint8_t*)g_rxBuff, 64, EP_BULK);
        g_usbRxCount -= 64;
        g_rxBuff += 64;

    } else {
        USB_ReadPacket((uint8_t*)g_rxBuff, g_usbRxCount, EP_BULK);
        rxCount = g_usbRxCount;
        g_usbRxCount = 0;
    }

    if(g_usbRxCount == 0) {
        g_rxBuff = g_usbAuditionActionCb.onPcmReceive(g_usbAudionPacketParam.playPacketLength);
        msc_data_prepare_receive((uint8_t*)g_rxBuff,g_usbAudionPacketParam.playPacketLength);
        g_usbRxCount = g_usbAudionPacketParam.playPacketLength;
    } else {
        USB_EPnStartRX((uint8_t*)g_rxBuff, rxCount, EP_BULK);
    }

}

AT_SRAM(.usbd_seg)
void audition_ep_in_proc_new()
{
    bulk_in_proc(0);
}

AT_SRAM(.usbd_seg)
void bulk_dma_complete()
{
    //logi("bulk dma cpl");
    if (usb_dev.is_bulk_dma_tx){
        audition_ep_in_proc_new();
    }
    else{
        //msc_ep_out_proc();
    }
}

BOOL OnEp0_pre_setup()
{
    if((setupReq.bRequest != USB_SET_VENDOR_AUDIO_PARAM) && (setupReq.bRequest != CMD_SET_AUDITION_CONTROL))
        return 0;

    uint16_t csr0 = usbd_read_csr0();
    if (csr0 & 0x01) {
        logi("g_UsbAuditionParam.type %d",g_UsbAuditionParam.type);
        if(setupReq.bRequest == USB_SET_VENDOR_AUDIO_PARAM){
            USB_ReadPacket((uint8_t*)&g_UsbAuditionParam, 8, EP_CTRL);

            logi("g_UsbAuditionParam.type %d",g_UsbAuditionParam.type);
            if(g_UsbAuditionParam.type == 0) {
                g_usbAuditionActionCb.onPlayParamChange(g_UsbAuditionParam.sampleRate,g_UsbAuditionParam.sampleBites,g_UsbAuditionParam.sampleChannels);
                g_usbAudionPacketParam.playPacketLength = cal_pack_size(g_UsbAuditionParam.sampleRate,g_UsbAuditionParam.sampleBites,g_UsbAuditionParam.sampleChannels);
                logi("play length %d",g_usbAudionPacketParam.playPacketLength);
                g_rxBuff = g_usbAuditionActionCb.onPcmReceive(g_usbAudionPacketParam.playPacketLength);
                g_usbRxCount = g_usbAudionPacketParam.playPacketLength;

                USB_FlushRxFifo(EP_BULK);
                msc_data_prepare_receive ((uint8_t*)g_rxBuff,g_usbAudionPacketParam.playPacketLength);
            } else if(g_UsbAuditionParam.type == 1){
                g_usbAudionPacketParam.recPacketLength = cal_pack_size(g_UsbAuditionParam.sampleRate,g_UsbAuditionParam.sampleBites,g_UsbAuditionParam.sampleChannels);
                logi("rec length %d",g_usbAudionPacketParam.recPacketLength);
                g_usbAuditionActionCb.onRecordParamChange(g_UsbAuditionParam.sampleRate,g_UsbAuditionParam.sampleBites,g_UsbAuditionParam.sampleChannels);
                //USB_FlushTxFifo(EP_BULK);
                //HAL_mdelay(2);

                g_txBuff = g_usbAuditionActionCb.SetupPcmBuff();
                msc_data_transmit_dma_new ((uint8_t*)g_txBuff,g_usbAudionPacketParam.recPacketLength / 64 * 64);
                g_txDataCount = g_usbAudionPacketParam.recPacketLength % 64;
            }
        } else if(setupReq.bRequest == CMD_SET_AUDITION_CONTROL){
            USB_ReadPacket((uint8_t*)&g_usbAuditionControl, 8, EP_CTRL);

            if(g_usbAuditionControl.type == 0) {
                g_usbAuditionActionCb.onPlayControlChange(g_usbAuditionControl.bPause);
            } else if(g_usbAuditionControl.type == 1){
                g_usbAuditionActionCb.onRecordControlChange(g_usbAuditionControl.bPause);
            }
            //USBD_CtlPrepareRx ((uint8_t*)&g_pUsbdCoreDev->request, 8);
        }
        usb_ep0_setup_end();
        ep0_state = USBD_EP0_IDLE;

        setupReq.bRequest = 0;

        return 1;
    }

    return 0;
}

AT_SRAM(.usbd_seg)
BOOL PreStandardSetupProcess()
{
    if((setupReq.bRequest != USB_SET_VENDOR_AUDIO_PARAM) && (setupReq.bRequest != CMD_SET_AUDITION_CONTROL))
        return 0;

    if(ep0_state == USBD_EP0_IDLE) {
        logi("rx data %d",setupReq.wLength);
        if((setupReq.bRequest == USB_SET_VENDOR_AUDIO_PARAM) ){
            USB_EP0StartRX((uint8_t*)&g_UsbAuditionParam, setupReq.wLength);
        } else if(setupReq.bRequest == CMD_SET_AUDITION_CONTROL) {
            USB_EP0StartRX((uint8_t*)&g_usbAuditionControl, setupReq.wLength);
        }
        ep0_state = USBD_EP0_DATA_OUT;
    }

    return 1;
}

AT_SRAM(.usbd_seg)
void usbd_audition_device_init_cb()
{
    g_rxBuff = g_usbAuditionActionCb.SetupPcmBuff();
    msc_data_prepare_receive ((uint8_t*)g_rxBuff,g_usbAudionPacketParam.playPacketLength);
    g_usbRxCount = g_usbAudionPacketParam.playPacketLength;

    //msc_data_transmit ((uint8_t*)g_txBuff,g_usbAudionPacketParam.recPacketLength);
    //g_txDataCount = g_usbAudionPacketParam.recPacketLength;
}

#endif

