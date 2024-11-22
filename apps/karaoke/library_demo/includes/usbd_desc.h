#ifndef _USBD_DESC_H_
#define _USBD_DESC_H_


#define ID_HEADPHONE_IT        0x01
#define ID_HEADPHONE_FEATURE   0x02
#define ID_HEADPHONE_OT        0x03
                                       
#define ID_MICROPHONE_IT        0x04
#define ID_MICROPHONE_FEATURE   0x05
#define ID_MICROPHONE_OT        0x06
#define ID_MICROPHONE_MIXER     0x07

#define COMP_INTERFACE_DESC_SIZE    9

#define COMP_HID_EPIN_ADDR      0x83
#define COMP_HID_EPOUT_ADDR     0x03
#define COMP_AUDIO_OUT_EP       0x02
#define COMP_AUDIO_IN_EP        0x82

#define COMP_HID_DESCRIPTOR_TYPE    0x21
#define COMP_HID_REPORT_DESC        0x22

extern uint8_t USB_Audio_CfgDesc_header[];
extern uint8_t USB_Audio_Standard_AC_descripor[];
extern uint8_t USB_Audio_Class_AC_desc[];
extern uint8_t USB_Audio_Class_Headphone_InputTerminate_Desc[];
extern uint8_t USB_Audio_Class_Headphone_FeatureUnit_Desc[];
extern uint8_t USB_Audio_Class_Headphone_OutputTerminate_Desc[];
extern uint8_t USB_Audio_Class_Microphone_InputTerminate_Desc[];
extern uint8_t USB_Audio_Class_Microphone_FeatureUnit_Desc[];
extern uint8_t USB_Audio_Class_Microphone_OutputTerminate_Desc[];
extern uint8_t USB_Audio_standard_interface_headphone_default_Dest[];
extern uint8_t USB_Audio_standard_interface_headphone_Dest[] ;
extern uint8_t USB_Audio_interface_headphone_general_desc[];
extern uint8_t USB_Audio_Audio_Format_Interface_headphone_Dest[];
extern uint8_t USB_Audio_EpOut_Desc[];
extern uint8_t USB_Audio_ep_isoout_class_desc[];
extern uint8_t USB_Audio_standard_interface_Mic_default_Dest[];
extern uint8_t USB_Audio_standard_interface_mic_Dest[];
extern uint8_t USB_Audio_interface_class_micro_general_desc[];
extern uint8_t USB_Audio_interface_class_micro_format_desc[];
extern uint8_t USB_Audio_EpIn_Desc[];
extern uint8_t USB_Audio_EpIn_class_general_Desc[];
extern uint8_t hid_desc_interface[];
extern uint8_t hid_class_report_desc[];
extern uint8_t hid_ep_desc[];

#endif

