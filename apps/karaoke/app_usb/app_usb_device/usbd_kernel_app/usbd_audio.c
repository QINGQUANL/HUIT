#define LOG_LEV 4
#include "sys_inc.h"
#include "hal.h"
#include "usbd_def.h"
#include "usbd_desc.h"
#include "usbd_audio.h"
#include "usb_hid.h"
#include "usb_device.h"
#include "oal.h"
#include "event.h"
#include "user_config.h"
#include "info_mem.h"
#include "psm_manager.h"
#include "hal_ll_usb.h"
#include "usb_app_config.h"
#include "usbd_app_common.h"
#include "usbd_phone_app.h"

#if (USBD_MODE == USB_MODE_SPEAKER_MIC) || (USBD_MODE == USB_MODE_MIC || (USBD_MODE == USB_MODE_TWO_AUDIO_CONFIG))

extern uint8_t usbd_mfi_msc_interface[];
extern uint8_t usbd_mfi_msc_ep_in[];
extern uint8_t usbd_mfi_msc_ep_out[];
extern uint8_t msc_interface_desc[];
extern uint8_t msc_epout_desc[];
extern uint8_t msc_epin_desc[];
extern uint8_t hid_desc_interface2[];
extern uint8_t hid_class_report_desc2[];
extern uint8_t hid_report2[];
extern uint8_t hid_report3[];
extern uint8_t USBD_DeviceDesc[];
extern const uint8_t USB_Audio_Class_Microphone_Selector[];
extern const uint8_t hid_common_interface_desc[];
extern const uint8_t hid_common_epin_desc[];
extern const uint8_t hid_common_epout_desc[];
extern uint8_t COMP_HID_Custom_Report_Desc[];
extern uint8_t USBMIC_Audio_CfgDesc_header[];
extern const uint8_t USBMIC_Audio_Standard_AC_descripor[];
extern const uint8_t USBMIC_Audio_Class_AC_desc[];
extern const uint8_t USBMIC_Audio_Class_Microphone_InputTerminate_Desc[];
extern const uint8_t USBMIC_Audio_Class_Microphone_FeatureUnit_Desc[];
extern const uint8_t USBMIC_Audio_Class_Microphone_Selector[];
extern const uint8_t USBMIC_Audio_Class_Microphone_OutputTerminate_Desc[];
extern const uint8_t USBMIC_Audio_Class_Microphone_OutputTerminate_Desc[];
extern const uint8_t USBMIC_Audio_standard_interface_Mic_default_Dest[];
extern const uint8_t USBMIC_Audio_standard_interface_mic_Dest[];
extern const uint8_t USBMIC_Audio_interface_class_micro_general_desc[];
extern const uint8_t USBMIC_Audio_interface_class_micro_format_desc[];
extern const uint8_t USBMIC_Audio_EpIn_Desc[];
extern const uint8_t USBMIC_Audio_EpIn_class_general_Desc[];
extern const uint8_t USBMIC_hid_desc_interface[];
extern const uint8_t USBMIC_hid_class_report_desc[];
extern const uint8_t USBMIC_hid_ep_desc[];
extern const uint8_t USBMIC_hid_desc_interface2[];
extern const uint8_t USBMIC_hid_class_report_desc2[];
extern const uint8_t USBMIC_hid_report2[0x41];
extern const uint8_t USBMIC_hid_report3[0x24];
extern const uint8_t usb_hid_custom_desc_interface[];
extern const uint8_t usb_hid_custom_class_report_desc[];
extern const uint8_t usb_hid_custom_in_ep_desc[];
extern const uint8_t usb_hid_custom_out_ep_desc[];

extern USBD_COMP_MSC_BOT_HandleTypeDef scsi_ctrl;
extern bool g_enableEqCtrl;

void usbd_pc_remove_detect_timer_cb();
void setup_set_interface_index(InterfaceIndex* pInterfaceIndex);
void usbd_ps4_device_test_unit();
void on_hid_in_data_tx_complete(type_usb_dev* pUsbdev);
void usbd_hid_app_init();
uint8_t usbd_get_resource_index();
void usbd_restore_resource_index(uint8_t index);
void iso_out_proc(type_usb_dev* pUsbdev);
bool usb_ep0_descriptor(const void *buf, uint16_t len);
void hal_set_index(uint8_t index);
void usb_ep3_hid_out_proc(type_usb_dev* pUsbdev);

void OnEp0_setup_processed(uint8_t wParam);

SetupEventCallback g_usbAudioSetupEventCb = {
    NULL,   //OnSetAddr
    NULL,  //OnSetInterface
    NULL,   //OnClearFeature
    NULL,   //OnSetFeature
    NULL,   //OnSetConfiguation
    NULL,   //OnPreStandardSetupProcess
    NULL,   //OnEp0_pre_setup
#if USB_PC_NO_PD0_UPDATE_EN
    OnEp0_setup_processed,
#else
    NULL,
#endif
} ;


uint8_t* audio_desc;
__ALIGN_BEGIN uint8_t hidOutBuff[32] __ALIGN_END;
eUsbAudioType g_usbAudioType;

extern PCB_USB_STATE_CB pcd_usb_state_cb;
extern PCD_EP_PROC pcd_ep_proc;
extern uint8_t* pUsb_CfgDesc;
extern USBD_DescriptorsTypeDef COMP_Desc;

extern void usb_epaudio_init(void);
extern void boot_usb_init(void);
extern void hal_pcd_init(PCD_EP_PROC* pcd_ep_proc, PCB_USB_STATE_CB* pcd_usb_state_cb);

InterfaceIndex g_audioIndexInfo;
int8_t g_usb_private_data_len;
uint8_t g_hid_set_report_count = 0;
PCD_ISR_CB usbd_pcd_isr_cb;
uint32_t usb_request_frame;
bool  usbd_addition_hid_ready;

#define NUM_AUDIO_DESC  (28 + 3 + 9)    //尽可能保留多点

const uint8_t* pUsbAudioDesc[NUM_AUDIO_DESC] = {     //保留两个以额外对24bit的支持,一个为HID的支持
    USB_Audio_CfgDesc_header,
    USB_Audio_Standard_AC_descripor,
    USB_Audio_Class_AC_desc,
    USB_Audio_Class_Headphone_InputTerminate_Desc,
    USB_Audio_Class_Headphone_FeatureUnit_Desc,
    USB_Audio_Class_Headphone_OutputTerminate_Desc,
    USB_Audio_Class_Microphone_InputTerminate_Desc,
    USB_Audio_Class_Microphone_FeatureUnit_Desc,
    USB_Audio_Class_Microphone_OutputTerminate_Desc,
    USB_Audio_standard_interface_headphone_default_Dest,
    
    USB_Audio_standard_interface_headphone_Dest,
    USB_Audio_interface_headphone_general_desc,
    USB_Audio_Audio_Format_Interface_headphone_Dest,
    USB_Audio_EpOut_Desc,
    USB_Audio_ep_isoout_class_desc,
#if USBD_HEADPHONE_24BITS_SUPPORT_EN
    USB_Audio_standard_interface_headphone_Dest_24,
    USB_Audio_interface_headphone_general_desc_24,
    USB_Audio_Audio_Format_Interface_headphone_Dest_24,
    USB_Audio_EpOut_Desc_24,
    USB_Audio_ep_isoout_class_desc_24,
#endif
    USB_Audio_standard_interface_Mic_default_Dest,
    USB_Audio_standard_interface_mic_Dest,
    USB_Audio_interface_class_micro_general_desc,
    USB_Audio_interface_class_micro_format_desc,
    USB_Audio_EpIn_Desc,
    USB_Audio_EpIn_class_general_Desc,

    0,  //USB_Audio_standard_interface_mic_Dest_2    //为支持16bit 和 24采样率空出这个位置
    0,  //USB_Audio_interface_class_micro_general_desc_2
    0,  //USB_Audio_interface_class_micro_format_desc_2
    0,  //USB_Audio_EpIn_Desc_2
    0,  //USB_Audio_EpIn_class_general_desc_2
    //
#if USB_EP3_HID_TYPE == USB_HID_AUDIO
    hid_desc_interface,
    hid_class_report_desc,
    hid_ep_desc,
#else //USB_EP3_HID_TYPE == USB_HID_AUDIO
    usb_hid_custom_desc_interface,
    usb_hid_custom_class_report_desc,
    usb_hid_custom_in_ep_desc,
    usb_hid_custom_out_ep_desc
#endif

#if USB_AUDIO_MFI_EN
    usbd_mfi_msc_interface,
    usbd_mfi_msc_ep_in,
    usbd_mfi_msc_ep_out
#endif

#if USB_EQ_EN || USB_AENC_TOOLS_EN
    msc_interface_desc,
    msc_epout_desc,
    msc_epin_desc
#elif USB_ADDITIONAL_HID
    hid_common_interface_desc,
    hid_class_report_desc,
    hid_common_epout_desc,
    hid_common_epin_desc
#endif
};

const uint8_t* pUsbMicDesc[NUM_AUDIO_DESC] = {
    USBMIC_Audio_CfgDesc_header,
    USBMIC_Audio_Standard_AC_descripor,
    USBMIC_Audio_Class_AC_desc,
    USBMIC_Audio_Class_Microphone_InputTerminate_Desc,
    USBMIC_Audio_Class_Microphone_FeatureUnit_Desc,
#if USB_SPECIAL_SUPPORT
    USBMIC_Audio_Class_Microphone_Selector,
#endif
    USBMIC_Audio_Class_Microphone_OutputTerminate_Desc,
    USBMIC_Audio_standard_interface_Mic_default_Dest,
    USBMIC_Audio_standard_interface_mic_Dest,
    USBMIC_Audio_interface_class_micro_general_desc,
    USBMIC_Audio_interface_class_micro_format_desc,
    USBMIC_Audio_EpIn_Desc,
    USBMIC_Audio_EpIn_class_general_Desc,

#if USB_EP3_HID_TYPE == USB_HID_AUDIO_CTRL
    USBMIC_hid_desc_interface,
    USBMIC_hid_class_report_desc,
    USBMIC_hid_ep_desc,
#if  USB_SPECIAL_SUPPORT
    USBMIC_hid_desc_interface2,
    USBMIC_hid_class_report_desc2,
#endif

#if USB_AUDIO_MFI_EN
    usbd_mfi_msc_interface,
    usbd_mfi_msc_ep_in,
    usbd_mfi_msc_ep_out
#endif

#else   //USB_EP3_HID_TYPE == USB_HID_AUDIO
    usb_hid_custom_desc_interface,
    usb_hid_custom_class_report_desc,
    usb_hid_custom_in_ep_desc,
    usb_hid_custom_out_ep_desc
#endif

};

const uint8_t* pDualUsbDeviceDesc[2][NUM_AUDIO_DESC] = {
{
    USB_Audio_CfgDesc_header,
    USB_Audio_Standard_AC_descripor,
    USB_Audio_Class_AC_desc,
    USB_Audio_Class_Headphone_InputTerminate_Desc,
    USB_Audio_Class_Headphone_FeatureUnit_Desc,
    USB_Audio_Class_Headphone_OutputTerminate_Desc,
    USB_Audio_Class_Microphone_InputTerminate_Desc,
    USB_Audio_Class_Microphone_FeatureUnit_Desc,
    USB_Audio_Class_Microphone_OutputTerminate_Desc,
    USB_Audio_standard_interface_headphone_default_Dest,

    USB_Audio_standard_interface_headphone_Dest,
    USB_Audio_interface_headphone_general_desc,
    USB_Audio_Audio_Format_Interface_headphone_Dest,
    USB_Audio_EpOut_Desc,
    USB_Audio_ep_isoout_class_desc,
#if USBD_HEADPHONE_24BITS_SUPPORT_EN
    USB_Audio_standard_interface_headphone_Dest_24,
    USB_Audio_interface_headphone_general_desc_24,
    USB_Audio_Audio_Format_Interface_headphone_Dest_24,
    USB_Audio_EpOut_Desc_24,
    USB_Audio_ep_isoout_class_desc_24,
#endif
    USB_Audio_standard_interface_Mic_default_Dest,
    USB_Audio_standard_interface_mic_Dest,
    USB_Audio_interface_class_micro_general_desc,
    USB_Audio_interface_class_micro_format_desc,
    USB_Audio_EpIn_Desc,
    USB_Audio_EpIn_class_general_Desc,

    0,  //USB_Audio_standard_interface_mic_Dest_2    //为支持16bit 和 24采样率空出这个位置
    0,  //USB_Audio_interface_class_micro_general_desc_2
    0,  //USB_Audio_interface_class_micro_format_desc_2
    0,  //USB_Audio_EpIn_Desc_2
    0,  //USB_Audio_EpIn_class_general_desc_2
    //
    hid_desc_interface,
    hid_class_report_desc,
    hid_ep_desc,
#if USB_AUDIO_MFI_EN
    usbd_mfi_msc_interface,
    usbd_mfi_msc_ep_in,
    usbd_mfi_msc_ep_out
#endif

#if USB_EQ_EN || USB_AENC_TOOLS_EN
    msc_interface_desc,
    msc_epout_desc,
    msc_epin_desc
#elif USB_ADDITIONAL_HID
    hid_common_interface_desc,
    hid_class_report_desc,
    hid_common_epout_desc,
    hid_common_epin_desc
#endif
},

{
//MIC ONLY的描述符
    USBMIC_Audio_CfgDesc_header,
    USBMIC_Audio_Standard_AC_descripor,
    USBMIC_Audio_Class_AC_desc,
    USBMIC_Audio_Class_Microphone_InputTerminate_Desc,
    USBMIC_Audio_Class_Microphone_FeatureUnit_Desc,
#if USB_SPECIAL_SUPPORT
    USBMIC_Audio_Class_Microphone_Selector,
#endif    
    USBMIC_Audio_Class_Microphone_OutputTerminate_Desc,
    USBMIC_Audio_standard_interface_Mic_default_Dest,
    USBMIC_Audio_standard_interface_mic_Dest,
    USBMIC_Audio_interface_class_micro_general_desc,
    USBMIC_Audio_interface_class_micro_format_desc,
    USBMIC_Audio_EpIn_Desc,
    USBMIC_Audio_EpIn_class_general_Desc,

    USBMIC_hid_desc_interface,
    USBMIC_hid_class_report_desc,
    USBMIC_hid_ep_desc,
#if  USB_SPECIAL_SUPPORT
    USBMIC_hid_desc_interface2,
    USBMIC_hid_class_report_desc2,
#endif

#if USB_AUDIO_MFI_EN
    usbd_mfi_msc_interface,
    usbd_mfi_msc_ep_in,
    usbd_mfi_msc_ep_out
#endif
},
};

static uint8_t s_usb_mode = USB_MODE_SPEAKER_MIC;
void usbd_change_mode(uint8_t mode)
{
    s_usb_mode = mode;
}



uint32_t usb_get_cfgdesc_length()
{
    uint32_t cfgLength = 0;
#if (USBD_MODE == USB_MODE_SPEAKER_MIC) || (USBD_MODE == USB_MODE_MIC)
#if USBD_MODE == USB_MODE_SPEAKER_MIC
    #define pUsbDesc pUsbAudioDesc
#elif USBD_MODE == USB_MODE_MIC
    #define pUsbDesc pUsbMicDesc
#endif

    for(uint8_t i=0; i < NUM_AUDIO_DESC ;i++ )
    {
        if(pUsbDesc[i] != NULL) {
            cfgLength += ((uint8_t*)pUsbDesc[i])[0];
        }
    }
#elif USBD_MODE == USB_MODE_TWO_AUDIO_CONFIG
    for(uint8_t i=0; i < NUM_AUDIO_DESC ;i++ )
    {
        if(pDualUsbDeviceDesc[s_usb_mode][i] != NULL) {
            cfgLength += ((uint8_t*)pDualUsbDeviceDesc[s_usb_mode][i])[0];
        }
    }
#endif

    return cfgLength;
}

void usb_audio_set_cfgdesc(uint8_t* pDesc )
{
    uint16_t curLength = 0;
#if USBD_MODE == USB_MODE_SPEAKER_MIC
    #define pUsbAudioDesc pUsbAudioDesc
#elif USBD_MODE == USB_MODE_MIC
    #define pUsbAudioDesc pUsbMicDesc
#endif

#if (USBD_MODE == USB_MODE_SPEAKER_MIC) || (USBD_MODE == USB_MODE_MIC) 
    for(uint8_t i=0; i < NUM_AUDIO_DESC ;i++ )
    {
        //logi("pUsbAudioDesc[i] %x",pUsbAudioDesc[i]);
        if(pUsbAudioDesc[i] != NULL) {
            memcpy(pDesc + curLength, pUsbAudioDesc[i], ((uint8_t*)pUsbAudioDesc[i])[0]);
            curLength += ((uint8_t*)pUsbAudioDesc[i])[0];
        }
    }
#elif USBD_MODE == USB_MODE_TWO_AUDIO_CONFIG
    for(uint8_t i=0; i < NUM_AUDIO_DESC ;i++ )
    {
        //logi("pUsbAudioDesc[i] %x",pUsbAudioDesc[i]);
        if(pDualUsbDeviceDesc[s_usb_mode][i] != NULL) {
            memcpy(pDesc + curLength, pDualUsbDeviceDesc[s_usb_mode][i], ((uint8_t*)pDualUsbDeviceDesc[s_usb_mode][i])[0]);
            curLength += ((uint8_t*)pDualUsbDeviceDesc[s_usb_mode][i])[0];
        }
    }
#endif
}

#define USBD_StringSerial "iAP Interface"
#define USB_Default "USB2.0"
void USBD_GetString(uint8_t *desc, uint8_t *unicode, uint16_t *len);
extern USBD_SetupReqTypedef setupReq;
extern uint8_t  USBD_StrDesc[0x100];    //ROM的变量
uint8_t* GetOtherStrDescriptor_new(uint16_t *length)
{
    uint8_t* setupBuff = (uint8_t*)&setupReq;
    if(setupBuff[2] == 0x07) {    //根据MFI字符串索引来，目前是7 {
        USBD_GetString((uint8_t *)USBD_StringSerial, USBD_StrDesc, length);
    }
    else {
        USBD_GetString((uint8_t*)USB_Default, USBD_StrDesc, length);
    }

    return USBD_StrDesc;
}

#define USB_STRING_SPECIAL    "SpeicalMic"
uint8_t* GetProductStringDescriptor_new(uint16_t *length)
{
    USBD_GetString((uint8_t*)USB_STRING_SPECIAL, USBD_StrDesc, length);

    return USBD_StrDesc;
}

/*
原厂PID规则：  
bit 0-3  :  无特殊定义
bit 4-7  :  采样位数
bit12-15 :  设备类型
*/
uint16_t usbd_get_pid()
{
    if(g_usbAudioType == AUDIO_DOLBY) {
        return 0x0004;
    }else {
#if USBD_MODE == USB_MODE_TWO_AUDIO_CONFIG
    if(s_usb_mode == USB_MODE_SPEAKER_MIC) {
#if USB_AUDIO_MFI_EN    
        return 0x000F | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 );  //MFi
#elif USB_EQ_EN
        return 0x000E | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8);
#elif USB_AENC_TOOLS_EN
        return 0x000A | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8);
#elif USB_ADDITIONAL_HID
        return 0x000B | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8);
#else
    #if USB_MIC_CHANNEL == USB_MIC_CHANNEL_STERO
        return 0x0003 | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8);
    #elif USB_MIC_CHANNEL == USB_MIC_CHANNEL_MONO
        return 0x1000 | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8);
    #else
        return 0x2000 | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8);
    #endif
#endif
    } else {
#if USB_SPECIAL_SUPPORT
        return 0x0002 ;     //special pid
#elif USB_AENC_TOOLS_EN
        return 0x000A | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 );
#elif USB_ADDITIONAL_HID
        return 0x000B | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8);
#else
     #if USB_MIC_CHANNEL == USB_MIC_CHANNEL_STERO
        return 0x2000 | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8);
     #elif USB_MIC_CHANNEL == USB_MIC_CHANNEL_MONO
        return 0x2001 | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8);
     #else
        return 0x3000 | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8);
     #endif
#endif
    }
#elif USBD_MODE == USB_MODE_SPEAKER_MIC    
  #if USB_AUDIO_MFI_EN    
        return 0x000F | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) ;  //MFi
  #elif USB_EQ_EN
        return 0x000E | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_MIC_SR_192000_EN);
  #elif USB_AENC_TOOLS_EN
        return 0x000A | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8) | (USB_MIC_SR_192000_EN);
  #elif USB_ADDITIONAL_HID
        return 0x000B | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8) | (USB_MIC_SR_192000_EN);
  #else
    #if USB_MIC_CHANNEL == USB_MIC_CHANNEL_STERO
        return 0x0003 | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8) | (USB_MIC_SR_192000_EN);
    #elif USB_MIC_CHANNEL == USB_MIC_CHANNEL_MONO
        return 0x1000 | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8) | (USB_MIC_SR_192000_EN);
    #else
        return 0x2000 | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8) | (USB_MIC_SR_192000_EN);
    #endif
  #endif
#elif USBD_MODE == USB_MODE_MIC
    #if USB_SPECIAL_SUPPORT
        return 0x0002 ;     //special pid
    #elif USB_AENC_TOOLS_EN
        return 0x000A | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8) | (USB_MIC_SR_192000_EN);
    #elif USB_ADDITIONAL_HID
        return 0x000B | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8) | (USB_MIC_SR_192000_EN);
    #else
     #if USB_MIC_CHANNEL == USB_MIC_CHANNEL_STERO
        return 0x2000 | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8) | (USB_MIC_SR_192000_EN);
     #elif USB_MIC_CHANNEL == USB_MIC_CHANNEL_MONO
        return 0x2001 | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8) | (USB_MIC_SR_192000_EN);
     #else
        return 0x3000 | ((USBD_MIC_SAMPLEBIT & 0xFF) << 4 ) | (USB_EP3_HID_TYPE << 8) | (USB_MIC_SR_192000_EN);
     #endif
    #endif
#endif
    }
}

void usbd_init_audio_config_desc(eUsbAudioType usbAudioType)
{
#if (USBD_MODE == USB_MODE_SPEAKER_MIC) || (USBD_MODE == USB_MODE_TWO_AUDIO_CONFIG)
    if(usbAudioType == AUDIO_STEREO) {
        USB_Audio_Audio_Format_Interface_headphone_Dest[4] = 2;
        USB_Audio_Class_Headphone_InputTerminate_Desc[7] = 2;
        USB_Audio_Class_Headphone_InputTerminate_Desc[8] = 3;
    } else if(AUDIO_DOLBY == usbAudioType) {
#if USB_AUDIO_DOLBY_SUPPORT_EN        
        USB_Audio_Audio_Format_Interface_headphone_Dest[4] = 8;
        USB_Audio_Class_Headphone_InputTerminate_Desc[7] = 8;
        USB_Audio_Class_Headphone_InputTerminate_Desc[8] = LEFT_FRONT | RIGHT_FRONT | CENTER_FRONT | LOW_FREQUENCY_ENHANCEMENT | LEFT_SURROUND | RIGHT_SURROUND | LEFT_OF_CENTER | RIGHT_OF_CENTER;
#endif
    }
#endif

#if (USBD_MODE == USB_MODE_SPEAKER_MIC)
    USB_Audio_CfgDesc_header[2] = LOBYTE(usb_get_cfgdesc_length());
    USB_Audio_CfgDesc_header[3] = HIBYTE(usb_get_cfgdesc_length());
#elif (USBD_MODE == USB_MODE_MIC)
    USBMIC_Audio_CfgDesc_header[2] = LOBYTE(usb_get_cfgdesc_length());
    USBMIC_Audio_CfgDesc_header[3] = HIBYTE(usb_get_cfgdesc_length());
#else
    if(s_usb_mode == USB_MODE_SPEAKER_MIC) {
        USB_Audio_CfgDesc_header[2] = LOBYTE(usb_get_cfgdesc_length());
        USB_Audio_CfgDesc_header[3] = HIBYTE(usb_get_cfgdesc_length());
    } else {
        USBMIC_Audio_CfgDesc_header[2] = LOBYTE(usb_get_cfgdesc_length());
        USBMIC_Audio_CfgDesc_header[3] = HIBYTE(usb_get_cfgdesc_length());
    }
#endif
    
    usb_audio_set_cfgdesc(&audio_desc[0]);    //先调整完各个参数，最后再拷贝到USB描述符的BUFF中   
}

void comp_device_init()
{
    logi("%s",__func__);
    usb_epaudio_init_new();
    usbd_hid_init(hidOutBuff, 32);
}

void ep0_setup_new();

AT_SRAM(.usb_setup_device)
void usbd_setup()
{
    //event_put(USB_ENUM_SETUP_TRANSACTION, 0, 0);
    ep0_setup_new();
}

AT_SRAM(.usb_device)
void iso_out_proc_new(type_usb_dev* pUsbdev)
{
    uint8_t bak = usbd_get_resource_index();
    iso_out_proc(pUsbdev);

    usbd_restore_resource_index(bak);
}

void usbd_on_addition_hid_data_receieve(const uint8_t* hid_data, uint8_t length);
AT_SRAM(.usb_addition_hid)
void usbd_addition_hid_out_proc(type_usb_dev* pUsbdev)
{
    hal_set_index(EP_ADDI_HID);
    uint16_t bulk_length = USB_GetRxLength(EP_ADDI_HID);    //获取长度
    uint8_t reallength = bulk_length;
    if(bulk_length % 4 != 0) {
        bulk_length = bulk_length/ 4;
        bulk_length = bulk_length* 4 + 4;
    }
    logi("hid length %d", bulk_length);
    USB_ReadPacket(scsi_ctrl.bot_data , bulk_length, EP_ADDI_HID);
    usbd_on_addition_hid_data_receieve(scsi_ctrl.bot_data, reallength);
    USBD_EPx_prepare_receive(EP_ADDI_HID);  //调用这个函数以接收下一包数据
}

AT_SRAM(.usb_addition_hid)
void usbd_addition_hid_in_proc(type_usb_dev* pUsbdev)
{
    usbd_addition_hid_ready = true;
    
}

//这个函数是异步的，调用此函数后，触发usbd_addition_hid_in_proc只能算发送完成
//只有确认USB在线的时候调用此函数才能被正确执行
AT_SRAM(.usb_addition_hid)
bool usbd_addition_hid_send_data(uint8_t* send_data, uint16_t length)
{
    //length不允许超过端点最大包长64BYTE
    bool res = false;
    if(usbd_addition_hid_ready) {
        usbd_addition_hid_ready = false;
        hal_set_index(EP_ADDI_HID);
        USB_WritePacket(send_data,length ,EP_ADDI_HID);
        usb_epn_tx_ready(EP_ADDI_HID);
        res = true;
        //
     }
    return !res;
}

AT_SRAM(.usb_mfi_device)
void mfi_bulk_in_proc(type_usb_dev* pUsbdev)
{
    hal_set_index(EP_BULK);     //切换USB FIFO

/*
    USB_WritePacket     将数据写入到FIFO中
    usb_epn_tx_ready    据写入FIFO后，需要调用这个函数以表示数据准备好
    数
    如果数据多于64BYTE，要分多次发送
*/
    //USB_WritePacket(src,len,EP_BULK)
    //usb_epn_tx_ready();   
}

//这仅仅是一个入口，当
AT_SRAM(.usb_mfi_device)
void mfi_bulk_out_proc(type_usb_dev* pUsbdev)
{
    hal_set_index(EP_BULK);   //切换USB FIFO

//如果数据多于64BYTE，要分多次接收
    //uint16_t bulk_length = USB_GetRxLength(EP_BULK);    //获取长度
    //USB_ReadPacket(dest, len, EP_BULK);     //调用这个函数将bulk数据读出来  
    USBD_EPx_prepare_receive(EP_BULK);  //调用这个函数以接收下一包数据
}

extern SetupEventCallback* g_setupEventCallback;
void usbd_hid_reset();

void on_usb_address_set()
{
    g_usb_private_data_len = 0;
    usbd_hid_reset();
}

void on_usb_set_config()
{
    logi("usb set config..");
    usbd_hid_app_init();
}

AT_SRAM(.StandardSetupProcess)
BOOL OnEp0PreStandardSetupProcess()
{
#if 0
    //下面两个命令为私有命令，用作配对使用，不要修改
    if(setupReq.bRequest == SMARTLINK_SET_DATA) {
        //00 C0 00 00 00 00 xx 00
        if(setupReq.wIndex == 0) {
            g_usb_private_data_len = setupReq.wLength;
            logi("sl set bt addr, len +++++++++ %d",g_usb_private_data_len);
        }

        if(g_usb_private_data_len > 0) {
            usb_ep0_rx_ready(); //cmd received, prepare to receive data
        } else {
            usb_ep0_setup_end();
        }
        return TRUE;
    } else if(setupReq.bRequest == SMARTLINK_GET_DATA) {
        //80 C1 00 00 00 00 06 00        
        logi("smart link get data ++++++ %d", setupReq.wLength);
        if(setupReq.wIndex == 0) {  //get bt address in psm
            sys_info_read(BT_ADDR_INFO, hidOutBuff, 6);
            logi("bt addr");
            printf_array(hidOutBuff, 6);
            USB_EP0StartTX(hidOutBuff, setupReq.wLength);
        }
        //usb_ep0_setup_end();
        return TRUE;
    } 
    //<<<<<<<---------------------------------------------
#endif
#if USB_PHONE_APP_EN
    if(setupReq.bRequest == SL_PHONE_CMD) {
        hal_set_index(EP_CTRL);
        uint8_t subCmd = setupReq.wValue & 0xFF;
        if(subCmd >= SMARTLINK_GET_BAT_VOL &&  subCmd <= SMARTLINK_GET_VERSION) {
            g_usb_private_data_len = 0;
            onep0_preprocess_get_data(&hidOutBuff[0]);
            if(setupReq.wLength == 0) {
                usb_ep0_setup_end();
            } else {
                logi("usb tx %d", setupReq.wLength);
                printf_array(hidOutBuff, 5);
                usb_ep0_descriptor(hidOutBuff, setupReq.wLength);   //setup end
            }

            return TRUE;    //已处理
        } else if(subCmd >= SMARTLINK_SET_SOUND_MODE && subCmd <= SMARTLINK_SET_RESET_FACTORY) {

            g_usb_private_data_len = setupReq.wLength;
            if(g_usb_private_data_len > 0) {
                usb_ep0_rx_ready(); //等数据接收完成后再OnEp0_presetup中处理
            } else {
                onep0_preprocess_set_data(&hidOutBuff[0]);  //处理那些没有数据的命令
                usb_ep0_setup_end();
            }
            return TRUE;
        }
    }
#endif
#if USB_ADDITIONAL_HID
    if(setupReq.bRequest == USB_REQ_GET_DESCRIPTOR) {
        if(HIBYTE(setupReq.wValue) == USB_DESC_TYPE_HID_REPORT){
            if(LOBYTE(setupReq.wIndex) == 0x04) { //new hid interface number
                usb_ep0_descriptor(COMP_HID_Custom_Report_Desc, 33);
                return TRUE;
            }
        }
    }
#endif
    return FALSE;
}

AT_SRAM(.usbd_presetup)
BOOL OnEp0_presetup()
{
#if 0
    //下面两个命令为私有命令，用作配对使用，不要修改 >>>>>>>>>>>>>>>>>>>>>
    if(g_usb_private_data_len > 0) {
        logi("receive ep0 data");
        hal_set_index(EP_CTRL);
        if(setupReq.bRequest == SMARTLINK_SET_DATA) {
            
            if(g_usb_private_data_len <= 0 ) {
                g_usb_private_data_len = 0;

                usb_ep0_setup_end();
             } else {
                USB_ReadPacket(hidOutBuff, 32, EP_CTRL);
                //printf_array(hidOutBuff, 32);

                g_usb_private_data_len -= 32;

                uint8_t addrLen = hidOutBuff[1];
                if(addrLen > 12) {
                    addrLen = 12;
                }

                /*
                byte0: command
                byte1: info len
                byte2: control
                byte3: reserved;
                byte4-byten : info data
                */

                if(hidOutBuff[0] == SMARTLINK_SET_LOCAL_BTADDR) {
                    sys_info_write(BT_ADDR_INFO, hidOutBuff + 4, addrLen);
                    logi("set local addr");
                    printf_array(hidOutBuff + 4, 6);
                } else if(hidOutBuff[0] == SMARTLINK_SET_LINK_ADDR) {
                    sys_info_write(G24_LINK_INFO, hidOutBuff + 4, addrLen);
                    logi("set link addr");
                    printf_array(hidOutBuff + 4, 12);
                }

                usb_ep0_rx_ready();     //prepare to receive null packet
             }
        }
    }
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#endif
#if USB_PHONE_APP_EN
    if(setupReq.bRequest == SL_PHONE_CMD) {
        //logi("receive ep0 data %d", g_usb_private_data_len);
        if(g_usb_private_data_len > 0) {
            uint8_t subCmd = setupReq.wValue & 0xFF;
            hal_set_index(EP_CTRL);
            if((subCmd >= SMARTLINK_SET_SOUND_MODE) && (subCmd <= SMARTLINK_SET_RESET_FACTORY)) {
                if(g_usb_private_data_len <= 0 ) {
                    g_usb_private_data_len = 0;

                    usb_ep0_setup_end();
                } else {
                    USB_ReadPacket(hidOutBuff, setupReq.wLength, EP_CTRL);
                    onep0_preprocess_set_data(&hidOutBuff[0]);

                    g_usb_private_data_len -= setupReq.wLength;
                    usb_ep0_rx_ready();
                }
            }
        }
    }
#endif
    return FALSE;   //continue pass the event to rom   
}

AT_SRAM(.usb_pcd)
void usbd_pcd_new()
{
    HAL_PCD_IRQHandler();
}

AT_SRAM(.usb_pcd)
void usb_epaudio_init1()
{
    event_put(USB_EVENT_BUS_RESET, 0 ,0);
}

AT_SRAM(.usb_pcd)
void usbd_audio_reset_cb()
{
    usb_epmass_init();
    usb_epaudio_init1();
    usb_ephid_init();
}


#if USB_PC_NO_PD0_UPDATE_EN
AT_SRAM(.usb_device_setup_process)
void OnEp0_setup_processed(uint8_t wParam)
{
    logi("OnEp0_setup_processed, %d",setupReq.wLength);
    //printf_array(&setupReq, 8);
    if ((setupReq.bmRequest & 0x60) == 0x20){
        if ((setupReq.bmRequest & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_INTERFACE) {
            uint8_t index = setupReq.wIndex & 0xFF;
            if(index == g_audioIndexInfo.hid_index) {
                if(setupReq.bRequest == 0x01) { //COMP_HID_REQ_GET_REPORT
                    uint8_t HID_Buffer[4];
                    uint16_t length = setupReq.wLength;
                    if(length > 4) {
                        length = 4;
                    }
                    memset(HID_Buffer, 0, 4);
                    
                    g_hid_set_report_count++;
                    
                    if(g_hid_set_report_count >= 5) {
                        g_hid_set_report_count = 0;
#if USB_PC_NO_PD0_UPDATE_EN
                        if(setupReq.wLength == 0x20) {
                            event_put(USB_EVENT_UPDATE_COMMAND_REC , 0, 0);
                        } 
#endif  
#if AUDIO_MIC_AFC
                        if(setupReq.wLength == 0x10) {
                            void afc_adaptive();
                            logi("usb lear afc");
                            afc_adaptive(); //学习
                        } else if(setupReq.wLength == 7) {
                            void get_afc_learn_res(uint8_t* is_learn_cpl, uint16_t* learnRes); 
                            get_afc_learn_res(&HID_Buffer[2], (uint16_t*)HID_Buffer);
                        }
#endif
                    }

                    USB_EP0StartTX ((uint8_t *)&HID_Buffer,length);
                    return;

                } else if(setupReq.bRequest == 0x09){   //hid set report
                   usb_ep0_setup_end();
                   return ;
                }
            }
        }

#if USB_PC_NO_PD0_UPDATE_EN
    g_hid_set_report_count = 0;
#endif
    } else if(setupReq.bmRequest == 0x80) {
#if USB_PC_NO_PD0_UPDATE_EN    
    g_hid_set_report_count = 0;
#endif
    }
}

#endif

uint8_t* onGetHidReportDescriptor(uint16_t *length) {
    uint8_t maxLen = setupReq.wLength;
    if(setupReq.wIndex == 2) {
        if(maxLen > 0x41) {
            maxLen = 0x41;
        }

        *length = maxLen;
        return hid_report2;
    } else if(setupReq.wIndex == 3){
        if(maxLen > 0x24) {
            maxLen = 0x24;
        }

        *length = maxLen;
        return hid_report3;
    }

    return hid_report3;
}

/**/
void usbd_set_audio_mode(uint8_t stereo_or_dolby)
{
    g_usbAudioType = stereo_or_dolby;
}

uint8_t usbd_get_audio_mode()
{
    return g_usbAudioType;
}

AT_SRAM(.usbd_common)
void usbd_handled_process_isr(uint8_t comIsr,uint16_t outIsr,uint16_t inIsr)
{
    //logi("usbd_han %d", usb_request_frame);
    uint16_t usbRequest = comIsr & (1 << 3);    //usb sof come
    usbRequest |= outIsr & 0x0E;    //usb out request come
    usbRequest |= inIsr & 0x0F;      //usb in request or control request 
    
    if(usbRequest) {
        usb_request_frame++;
    }
}

uint32_t usbd_get_request_frame()
{
    return usb_request_frame;
}

void usbd_init_kernel_audio(USBD_Audio_CallbackTypedef* pUsbdAudioCb)
{
    logi("%s",__func__);

    pcd_usb_state_cb.reset_cb = usbd_audio_reset_cb;
    pcd_usb_state_cb.initialize_cb = comp_device_init;//comp_device_init;  //注册相关回调
    pcd_usb_state_cb.sof_cb = on_usb_sof_receive;

    int descLength = usb_get_cfgdesc_length();
    audio_desc = OAL_malloc(descLength);

    //g_usbAudioType = AUDIO_DOLBY; //USB_AUDIO_TYPE;
#if USB_AUDIO_DOLBY_SUPPORT_EN == 0
    g_usbAudioType = AUDIO_STEREO;
#endif
    usbd_init_audio_config_desc(g_usbAudioType);
    
    set_config_desc(&audio_desc[0]);
    set_config_desc_length(descLength);

    memset(&g_audioIndexInfo, 0xFF, sizeof(InterfaceIndex));
#if (USBD_MODE == USB_MODE_SPEAKER_MIC)    
    g_audioIndexInfo.audio_ctrl_index = IT_AUDIO_DEFAULT;  //设置接口索引
    g_audioIndexInfo.audio_speaker_index = IT_USBAUDIO_SPEAKER; 
    g_audioIndexInfo.audio_mic_index = IT_USBAUDIO_MICROPHONE;
    g_audioIndexInfo.hid_index = IT_USBAUDIO_HID;
    g_audioIndexInfo.msc_index = IT_MSC;  //没有用要设置成0xFF
#elif (USBD_MODE == USB_MODE_MIC) 
    g_audioIndexInfo.audio_ctrl_index = IT_AUDIO_DEFAULT;  //设置接口索引
    //g_audioIndexInfo.audio_speaker_index = IT_USBMIC_MICROPHONE; 
    g_audioIndexInfo.audio_mic_index = IT_USBMIC_MICROPHONE;
    g_audioIndexInfo.hid_index = IT_USBMIC_HID;
    g_audioIndexInfo.msc_index = IT_MSC;  //没有用要设置成0xFF
#else   //两个模式
    if(s_usb_mode == USB_MODE_SPEAKER_MIC) {
        g_audioIndexInfo.audio_ctrl_index = IT_AUDIO_DEFAULT;  //设置接口索引
        g_audioIndexInfo.audio_speaker_index = IT_USBAUDIO_SPEAKER; 
        g_audioIndexInfo.audio_mic_index = IT_USBAUDIO_MICROPHONE;
        g_audioIndexInfo.hid_index = IT_USBAUDIO_HID;
        g_audioIndexInfo.msc_index = IT_MSC;  //没有用要设置成0xFF
    } else {
        g_audioIndexInfo.audio_ctrl_index = IT_AUDIO_DEFAULT;  //设置接口索引
        g_audioIndexInfo.audio_mic_index = IT_USBMIC_MICROPHONE;
        g_audioIndexInfo.hid_index = IT_USBMIC_HID;
        //g_audioIndexInfo.msc_index = IT_MSC;
    }
#endif

#if USB_EP3_HID_TYPE == USB_HID_CUSTOM
    g_enableEqCtrl = 1;
    pcd_ep_proc.hid_out_proc = usb_ep3_hid_out_proc;
#endif
    setup_set_interface_index(&g_audioIndexInfo);
    COMP_Desc.GetUsbPID = usbd_get_pid;
    //修复配置描述符相关操作
    registrer_usb_audio_callback(pUsbdAudioCb);
    pcd_ep_proc.hid_in_proc = on_hid_in_data_tx_complete;

#if USB_AUDIO_MFI_EN
    //处理MFI相关的字符串描述符
    COMP_Desc.GetOtherStrDescriptor = GetOtherStrDescriptor_new;
    pcd_ep_proc.bulk_in_proc = mfi_bulk_in_proc;
    pcd_ep_proc.bulk_out_proc = mfi_bulk_out_proc;  //重写底层的回调接口以支持MFI自定义处理
#endif

#if USB_ADDITIONAL_HID
    usbd_addition_hid_ready = true;
    pcd_ep_proc.bulk_in_proc = usbd_addition_hid_in_proc;
    pcd_ep_proc.bulk_out_proc = usbd_addition_hid_out_proc;
#endif

#if (USBD_MODE == USB_MODE_MIC) & USB_SPECIAL_SUPPORT
    COMP_Desc.GetHidReportDescriptor = onGetHidReportDescriptor;
    COMP_Desc.GetProductStrDescriptor = GetProductStringDescriptor_new;
#endif

#if USB_SPECIAL_SUPPORT
    USBD_DeviceDesc[0x08] = 0x35;
    USBD_DeviceDesc[0x09] = 0x12;
#endif
    usb_request_frame = 0;
    usbd_pcd_isr_cb.pre_process_isr = NULL;
    usbd_pcd_isr_cb.handled_process_isr = usbd_handled_process_isr;
    register_pcd_cb(&usbd_pcd_isr_cb);  //注册这个回调用于查看是否有USB响应
    
    //由于枚举过程USB中断会很多，如果没有蓝牙等对时间要求比较高的功能，
    //可以不用重写setup功能，这样让ROM代码自己处理，速度会很快
    //pcd_ep_proc.setup = usbd_setup; 
    pcd_ep_proc.iso_out_proc = iso_out_proc_new;

    g_setupEventCallback = &g_usbAudioSetupEventCb;
    g_usbAudioSetupEventCb.OnSetAddr = on_usb_address_set;
    g_usbAudioSetupEventCb.OnSetConfiguation = on_usb_set_config;
#if USB_PHONE_APP_EN
    g_usbAudioSetupEventCb.OnEp0_pre_setup = OnEp0_presetup;
#endif
#if USB_PHONE_APP_EN || USB_ADDITIONAL_HID
    g_usbAudioSetupEventCb.OnPreStandardSetupProcess = OnEp0PreStandardSetupProcess;
#endif

#if USB_PC_NO_PD0_UPDATE_EN
    g_usbAudioSetupEventCb.OnEp0_setup_processed = OnEp0_setup_processed;
#endif
    //<<<<<------------------------------
    
    hal_pcd_init_new(&pcd_ep_proc, &pcd_usb_state_cb);
    usb_sof_isr_en(TRUE);
#if USB_MODULE_ALWAYS_ON == 0    
    usb_pc_out_detect_init();
#endif
}

void usbd_deinit_kernel_audio()
{
    if(audio_desc != NULL) {
        OAL_free(audio_desc);
        audio_desc = NULL;
    }
}


#endif

