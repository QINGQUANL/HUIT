
#include "sys_inc.h"
#include "usbd_desc.h"
#include "usbd_def.h"
#include "user_config.h"
#include "usb_app_config.h"


#define COMP_AUDIO_SAMPLE_FREQ(frq)      (uint8_t)(frq), (uint8_t)((frq >> 8)), (uint8_t)((frq >> 16))

#define COMP_AUDIO_PACKET_SZE(frq)          (uint8_t)(((frq * 2 * 2)/1000) & 0xFF), \
                                       (uint8_t)((((frq * 2 * 2)/1000) >> 8) & 0xFF)

uint8_t USB_Audio_CfgDesc_header[] = {
    0x09,
    0x02,
    LOBYTE(241),      //这个长度是个变量，描述符总长
    HIBYTE(241),
#if USB_AUDIO_MFI_EN || USB_EQ_EN || USB_AENC_TOOLS_EN || USB_ADDITIONAL_HID
    0x05,    
#else
    0x04,   //配置描述符  interface数4
#endif
    0x01,
    0x00,
    0x80,   //主机供电
    0x32,   //100mA
};

const uint8_t USB_Audio_Standard_AC_descripor[] = {
    //standard interface AC descriptor
    0x09,
    0x04,
    0x00,       /* bInterfaceNumber: Number of Interface */
    0x00,       /* bAlternateSetting: Alternate setting */
    0x00,       /*  number of ep */
    0x01,       /* interface class  USB_DEVICE_CLASS_COMP_AUDIO*/
    0x01,       //bInterfaceSubClass  COMP_AUDIO_SUBCLASS_AUDIOCONTROL
    0x00,       //COMP_AUDIO_PROTOCOL_UNDEFINED
    0x00,      //INTERFACE， CLASS AUDIO， SUBCLASS：audio control
};

const uint8_t USB_Audio_Class_AC_desc[] = {
//class-specific AC interface descriptor
    0x0A,
    0x24,      //INTERFACE DESCRYPT
    0x01,       //header descriptor subtype
    0x00,       //bcd ADC   1.0
    0x01,
    0x48,       //total number of bytes returned for audio control descriptor 
    0x00,
    0x02,       //bIncollection ,2 audio stream interface in collection
    0x01,       // interface number:1
    0x02,       // interface number:2
};

uint8_t USB_Audio_Class_Headphone_InputTerminate_Desc[] = 
{
    0x0C,
    0x24,       //interface descriptor
    0x02,       //input terminate subtype
    ID_HEADPHONE_IT,       //id of input terminate
    0x01,       //terminiate type: 0101    usb stream
    0x01,
    0x00,       //no association
#if 1
    0x02,       //2 channel
    0x03,       //stereo
    0x00,       //unused
#else
    0x08
    LEFT_FRONT | RIGHT_FRONT | CENTER_FRONT | LOW_FREQUENCY_ENHANCEMENT | LEFT_SURROUND | RIGHT_SURROUND | LEFT_OF_CENTER | RIGHT_OF_CENTER
    0x00,       //reserve for dolby
#endif
    
    0x00,       //iterminate,index of string descriptor
    0x00,   
};

const uint8_t USB_Audio_Class_Headphone_FeatureUnit_Desc[] = {
    //audio interface descriptor, audio feature unit, link to usb stream id-01, 
    0x09,
    0x24,       //interface descriptor
    0x06,       //feature unit descriptor subtype
    ID_HEADPHONE_FEATURE,       //ID of feature unit
    ID_HEADPHONE_IT,       //id of entity to witch this feature connect
    0x02,       //control size, 2 bytes for each element of bmaControl array 
    USB_SPEAKER_MUTE_EN | (USB_SPEAKER_VOLUME_EN << 1),       //mute of volume control on master channel
    0x00,       //volume control on left font channel
    0x00,       //volume control on right font channel
    //0x00,       //ifeature, index of string descriptor
};

const uint8_t USB_Audio_Class_Headphone_OutputTerminate_Desc[] = {
    //audio interface descriptor,  output terminate, id-3, headphone link to feature 2 
    0x09,
    0x24,       //interface descriptor
    0x03,       //output terminat subtype(speaker/headphone)
    ID_HEADPHONE_OT,       //id of this terminate
    LOBYTE(USB_OUTPUT_TYPE),       //0302  headphone  ,(0301 speaker)
    HIBYTE(USB_OUTPUT_TYPE),
    0x00,       //
    ID_HEADPHONE_FEATURE,       //follow feature ID 2
    0x00,       //id of string descriptor
};

const uint8_t USB_Audio_Class_Microphone_InputTerminate_Desc[] = {
    //audio interface descriptor, input terminate , microphone ,id-4 stereo
    0x0C,
    0x24,       //interface descriptor
    0x02,       //input terminat subtype
    ID_MICROPHONE_IT,       //id of this terminate
    LOBYTE(USB_INPUT_TYPE),       // input type: microphone (0201)
    HIBYTE(USB_INPUT_TYPE),
    0x00,       //
    MIC_INPUT_CHANNALS,
#if USB_MIC_CHANNEL == USB_MIC_CHANNEL_MONO  
    0x00,
#elif USB_MIC_CHANNEL == USB_MIC_CHANNEL_STERO
    0x03,       // stereo
#elif USB_MIC_CHANNEL == USB_MIC_CHANNEL_4CHANNEL
    0x0F,
#elif USB_MIC_CHANNEL == USB_MIC_CHANNEL_6CHANNEL
    0x3F,   
#elif USB_MIC_CHANNEL == USB_MIC_CHANNEL_6CHANNEL
    0xFF,      
#endif    
    0x00,
    0x00,
    0x00, 
};

const uint8_t USB_Audio_Class_Microphone_FeatureUnit_Desc[] = {
    //audio interface descriptor, audio feature unit ,  one or more feature must work, otherwise the host would ignore the feature and not enum mircophone
    0x0A,                                 /* bLength */
    0x24,      /* bDescriptorType */
    0x06,           /* bDescriptorSubtype */
    ID_MICROPHONE_FEATURE,             /* bUnitID */
    ID_MICROPHONE_IT,     //id of entity to witch this feature connect
    0x02,                                 /* bControlSize */
    USB_MICROPHONE_MUTE_EN | (USB_MICROPHONE_VOLUME_EN << 1),   //COMP_AUDIO_CONTROL_MUTE |AUDIO_CONTROL_VOLUME,/* bmaControls(0) */
    0x00,     //mute of volume control on master channel,0 means not control
    0x00,     //volume control on right font channel , this is depend on the control size
    0x00,     //id string of this terminate 
};

const uint8_t USB_Audio_Class_Microphone_Selector[] = {
    0x07,
    0x24,
    0x05,   //selector
    ID_MICROPHONE_SELECTOR,
    0x01,
    ID_MICROPHONE_FEATURE,
    0x00
};


const uint8_t USB_Audio_Class_Microphone_OutputTerminate_Desc[] = {
    //audio interface descriptor, output terminate ,id-5 , link to microphone 
    0x09,
    0x24,       //interface descriptor
    0x03,       //output terminat subtype
    ID_MICROPHONE_OT,       //id 
    0x01,       //0101  usb stream 
    0x01,       
    0x00,
    ID_MICROPHONE_FEATURE,       //link to terminite 4
    0x00,      //
};

const uint8_t USB_Audio_standard_interface_headphone_default_Dest[] = {
    0x09,
    0x04,       //interface desc
    0x01,       //interface number
    0x00,       /* bAlternateSetting: Alternate setting */
    0x00,       //number of ep
    0x01,       /* interface class  USB_DEVICE_CLASS_COMP_AUDIO*/
    0x02,       // sub class audio stream
    0x00,
    0x00,     
};

const uint8_t USB_Audio_standard_interface_headphone_Dest[] = {
    0x09,       
    0x04,       //interface desc
    0x01,       //interface number
    0x01,       /* bAlternateSetting: Alternate setting */
    0x01,      //number of ep
    0x01,      /* interface class  USB_DEVICE_CLASS_COMP_AUDIO*/
    0x02,      // sub class audio stream
    0x00,
    0x00,  
};

const uint8_t USB_Audio_interface_headphone_general_desc[] = {
    0x07,   
    0x24,       //interface descriptor
    0x01,       //stream general
    ID_HEADPHONE_IT,       //terminate link, link to terminate id-1, usb stream which link to output terminate 
    0x01,       //bdelay
    0x01,       //0001  ,audio PCM
    0x00,       //
};

uint8_t USB_Audio_Audio_Format_Interface_headphone_Dest[] = {     //最多支持4个采样率 ,9 BYTE的位置空出来，需要的时候填充
    0x0E,
    0x24,       //interface descriptor
    0x02,       //type: format type
    0x01,       //fomat type-1
    0x02,       //2 channels
    0x02,       //subframe: 2bytes
    0x10,       //16bits
    0x02,       //1 sample rate support
    0x44,0xAC,0x00, //必须支持这个采样率，不然有些手机会外放，比如P40，三星S9等
    0x80,0xBB,0x00,
};

const uint8_t USB_Audio_EpOut_Desc[] = {
    0x09,
    0x05,       //endpoint
    COMP_AUDIO_OUT_EP,  //endpoint number
    0x0D,       //mode sychornize
    0xC0,       //MPS: 0000C        192BYTE
    0x00,
    0x01,       //bInterval:: 1ms
    0x00,
    0x00,      //ep descript, out: 03   type:iso,  mps:192  bInterval:: 1ms   
};

const uint8_t USB_Audio_ep_isoout_class_desc[] = {
    0x07,
    0x25,     //audio endpoint descriptor
    0x01,     //  end point general
    0x01,     //  bmAttribute, sample frequecy control
    0x00,
    0x00,
    0x00,    //endpoint descript, stream general, bTerminalLink:1, bDelay: 0x01
};


uint8_t USB_Audio_standard_interface_headphone_Dest_24[] = {
    0x09,       
    0x04,       //interface desc
    0x01,       //interface number
    0x02,       /* bAlternateSetting: Alternate setting */
    0x01,      //number of ep
    0x01,      /* interface class  USB_DEVICE_CLASS_COMP_AUDIO*/
    0x02,      // sub class audio stream
    0x00,
    0x00,  
};

uint8_t USB_Audio_interface_headphone_general_desc_24[] = {
    0x07,   
    0x24,       //interface descriptor
    0x01,       //stream general
    ID_HEADPHONE_IT,       //terminate link, link to terminate id-1, usb stream which link to output terminate 
    0x01,       //bdelay
    0x01,       //0001  ,audio PCM
    0x00,       //
};

uint8_t USB_Audio_Audio_Format_Interface_headphone_Dest_24[] = {     //最多支持4个采样率 ,9 BYTE的位置空出来，需要的时候填充
    0x0E,
    0x24,       //interface descriptor
    0x02,       //type: format type
    0x01,       //fomat type-1
    0x02,       //2 channels
    0x03,       //subframe: 2bytes
    0x18,       //16bits
    0x02,       //sample rate support
    0x44,0xAC,0x00, //必须支持这个采样率，不然有些手机会外放，比如P40，三星S9等
    0x80,0xBB,0x00,
};

uint8_t USB_Audio_EpOut_Desc_24[] = {
    0x09,
    0x05,       //endpoint
    COMP_AUDIO_OUT_EP,  //endpoint number
    0x0D,       //mode sychornize
    0x20,       //MPS: 0000C        768
    0x01,
    0x01,       //bInterval:: 1ms
    0x00,
    0x00,      //ep descript, out: 03   type:iso,  mps:192  bInterval:: 1ms   
};

uint8_t USB_Audio_ep_isoout_class_desc_24[] = {
    0x07,
    0x25,     //audio endpoint descriptor
    0x01,     //  end point general
    0x01,     //  bmAttribute, sample frequecy control
    0x00,
    0x00,
    0x00,    //endpoint descript, stream general, bTerminalLink:1, bDelay: 0x01
};


const uint8_t USB_Audio_standard_interface_Mic_default_Dest[] = {
    0x09,
    0x04,       //interface
    0x02,       /* bInterfaceNumber: Number of Interface */
    0x00,       /* bAlternateSetting: Alternate setting */
    0x00,       //number of ep
    0x01,       /* interface class  USB_DEVICE_CLASS_COMP_AUDIO*/
    0x02,       // sub class audio stream
    0x00,
    0x00,
};

const uint8_t USB_Audio_standard_interface_mic_Dest[] = {
    0x09,
    0x04,        //interface
    0x02,        /* bInterfaceNumber: Number of Interface */
    0x01,       /* bAlternateSetting: Alternate setting */
    0x01,       //number of ep
    0x01,       /* interface class  USB_DEVICE_CLASS_COMP_AUDIO*/
    0x02,       //sub class audio stream
    0x00,
    0x00,      // id of string descriptor
};

const uint8_t USB_Audio_interface_class_micro_general_desc[] = {
    0x07,
    0x24,       //interface descriptor
    0x01,       //stream general
    ID_MICROPHONE_OT,       //terminate link to id 5, link to microphone
    0x01,       //interface delay
    0x01,       //0001 pcm format
    0x00,
};

const uint8_t USB_Audio_interface_class_micro_format_desc[] = { //最多支持4个采样率， 9 BYTE的位置空出来，需要的时候填充
    0x08 + USB_MIC_SAMPLE_NUMBER*3 ,
    0x24,       //interface descriptor
    0x02,       //stream format type
    0x01,       //format type-1
    MIC_INPUT_CHANNALS,
    USBD_MIC_SAMPLEBIT,       // frame size:in BYTES
    USBD_MIC_SAMPLEBIT * 8,       // bit resolution:  n*8
    USB_MIC_SAMPLE_NUMBER,       //sample rate num support
    
#if USB_MIC_SR_44100_EN
    COMP_AUDIO_SAMPLE_FREQ(44100),
#endif
#if USB_MIC_SR_48000_EN
	COMP_AUDIO_SAMPLE_FREQ(48000),
#endif
#if USB_MIC_SR_96000_EN
    COMP_AUDIO_SAMPLE_FREQ(96000),
#endif
#if USB_MIC_SR_192000_EN
    COMP_AUDIO_SAMPLE_FREQ(192000),
#endif
	//COMP_AUDIO_SAMPLE_FREQ(44100),
};

const uint8_t USB_Audio_EpIn_Desc[] = {
    0x09,       //
    0x05,       //end pointer desc
    COMP_AUDIO_IN_EP,   //in endpint
    0x0D,       //synchronous   , ISO
    LOBYTE(AUDIO_MIC_MPS), 
    HIBYTE(AUDIO_MIC_MPS),    //mps
    0x01,       //internal : 1ms
    0x00,       //refresh
    0x00,
//187
    
};

const uint8_t USB_Audio_EpIn_class_general_Desc[] = {
    0x07,
    0x25,       //audio endpoint descriptor
    0x01,       //end point general
    0x00,       //bmAttribute, sample frequecy control
    0x00,       //no used
    0x00,       //no used
    0x00,       // //no used
};


const uint8_t hid_desc_interface[] = {
    //---------------------- HID --------------------------
    COMP_INTERFACE_DESC_SIZE,            /* bLength */
    USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
    0x03,                                 /* bInterfaceNumber */
    0x00,                                 /* bAlternateSetting */
    0x01,                                 /* bNumEndpoints */
    0x03,               /* bInterfaceClass */
    0x00,          /* bInterfaceSubClass */
    0x00,             /* bInterfaceProtocol */
    0x00,                                 /* iInterface */
    /* 09 ,18 byte*/
};

const uint8_t hid_class_report_desc[]={
    0x09,         /*bLength: HID Descriptor size*/
    COMP_HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
    0x01,         /*bcdHID: HID Class Spec release number*/
    0x02,
    0x00,         /*bCountryCode: Hardware target country*/
    0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
    0x22,         /*bDescriptorType*/
    0x21,/*wItemLength: Total length of Report descriptor*/
    0x00,
    /* 09 ,27 byte */
};

const uint8_t hid_ep_desc[] = {
    /* Descriptor of Key Endpoint */
    0x07,          /*bLength: Endpoint Descriptor size*/
    USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/
    COMP_HID_EPIN_ADDR,     /*bEndpointAddress: Endpoint Address (IN)*/
    0x03,          /*bmAttributes: Interrupt endpoint*/
    0x08,           /*wMaxPacketSize: 128 Byte max */
    0x00,
    0x01,          /*bInterval: Polling Interval (10 ms)*/
    /* 07 ,34 byte */
};


const uint8_t usb_hid_custom_desc_interface[] = {
    //---------------------- HID --------------------------
    COMP_INTERFACE_DESC_SIZE,            /* bLength */
    USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
    0x03,                                 /* bInterfaceNumber */
    0x00,                                 /* bAlternateSetting */
    0x02,                                 /* bNumEndpoints */
    0x03,               /* bInterfaceClass */
    0x00,          /* bInterfaceSubClass */
    0x00,             /* bInterfaceProtocol */
    0x00,                                 /* iInterface */
    /* 09 ,18 byte*/
};

const uint8_t usb_hid_custom_class_report_desc[]={
    0x09,         /*bLength: HID Descriptor size*/
    COMP_HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
    0x01,         /*bcdHID: HID Class Spec release number*/
    0x02,
    0x00,         /*bCountryCode: Hardware target country*/
    0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
    0x22,         /*bDescriptorType*/
    0x21,/*wItemLength: Total length of Report descriptor*/
    0x00,
    /* 09 ,27 byte */
};

const uint8_t usb_hid_custom_in_ep_desc[] = {
    /* Descriptor of Key Endpoint */
    0x07,          /*bLength: Endpoint Descriptor size*/
    USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/
    COMP_HID_EPIN_ADDR,     /*bEndpointAddress: Endpoint Address (IN)*/
    0x03,          /*bmAttributes: Interrupt endpoint*/
    0x20,           /*wMaxPacketSize: 32 Byte max */
    0x00,
    0x01,          /*bInterval: Polling Interval (10 ms)*/
    /* 07 ,34 byte */
};

const uint8_t usb_hid_custom_out_ep_desc[] = {
    /* Descriptor of Key Endpoint */
    0x07,          /*bLength: Endpoint Descriptor size*/
    USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/
    COMP_HID_EPOUT_ADDR,     /*bEndpointAddress: Endpoint Address (OUT)*/
    0x03,          /*bmAttributes: Interrupt endpoint*/
    0x20,           /*wMaxPacketSize: 20 Byte max */
    0x00,
    0x01,          /*bInterval: Polling Interval (10 ms)*/
    /* 07 ,34 byte */
};


#if USB_EQ_EN || USB_AENC_TOOLS_EN
const uint8_t msc_interface_desc[] = {
  /********************  Mass Storage interface ********************/
  0x09,   /* bLength: Interface Descriptor size */
  0x04,   /* bDescriptorType: */
  0x04,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints*/
  0x08,   /* bInterfaceClass: MSC Class */
  0x06,   /* bInterfaceSubClass : SCSI transparent*/
  0x50,   /* nInterfaceProtocol */
  0x05,          /* iInterface: */
};

const uint8_t msc_epout_desc[] = {
    0x07,   /*Endpoint descriptor length = 7*/
	0x05,   /*Endpoint descriptor type */
	COMP_MSC_EPIN_ADDR,   /*Endpoint address (IN, address 1) */
	0x02,   /*Bulk endpoint type */
	LOBYTE(64),
	HIBYTE(64),
	0x00,   /*Polling interval in milliseconds */
};

const uint8_t msc_epin_desc[] = {
    0x07,   /*Endpoint descriptor length = 7 */
	0x05,   /*Endpoint descriptor type */
	COMP_MSC_EPOUT_ADDR,   /*Endpoint address (OUT, address 1) */
	0x02,   /*Bulk endpoint type */
	LOBYTE(64),
	HIBYTE(64),
	0x00     /*Polling interval in milliseconds*/
};
#elif USB_ADDITIONAL_HID
const uint8_t hid_common_interface_desc[] = {
    COMP_INTERFACE_DESC_SIZE,            /* bLength */
    USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
    IT_MSC,                                 /* bInterfaceNumber */
    0x00,                                 /* bAlternateSetting */
    0x02,                                 /* bNumEndpoints */
    0x03,               /* bInterfaceClass */
    0x00,          /* bInterfaceSubClass */
    0x00,             /* bInterfaceProtocol */
    0x00,                                 /* iInterface */
};

const uint8_t hid_common_epout_desc[] = {
    0x07,   /*Endpoint descriptor length = 7*/
	0x05,   /*Endpoint descriptor type */
	COMP_MSC_EPIN_ADDR,   /*Endpoint address (IN, address 1) */
	0x03,   /*hid endpoint type */
	LOBYTE(64),
	HIBYTE(64),
	0x09,   /*Polling interval in milliseconds 10ms*/
};

const uint8_t hid_common_epin_desc[] = {
    0x07,   /*Endpoint descriptor length = 7 */
	0x05,   /*Endpoint descriptor type */
	COMP_MSC_EPOUT_ADDR,   /*Endpoint address (OUT, address 1) */
	0x03,   /*hid endpoint type */
	LOBYTE(64),
	HIBYTE(64),
	0x00     /*Polling interval in milliseconds*/
};
#endif

uint8_t USBMIC_Audio_CfgDesc_header[] = {
    0x09,
    0x02,
    LOBYTE(241),      //这个长度是个变量，描述符总长
    HIBYTE(241),
#if USB_SPECIAL_SUPPORT || USB_AUDIO_MFI_EN
    0x04, //2 audio  + 2 hid
#else
    0x03,   //2 audio  + 1 hid
#endif   
    0x01,
    0x00,
    0x80,   //主机供电
    0x32,   //100mA
};

const uint8_t USBMIC_Audio_Standard_AC_descripor[] = {
    //standard interface AC descriptor
    0x09,
    0x04,
    0x00,       /* bInterfaceNumber: Number of Interface */
    0x00,       /* bAlternateSetting: Alternate setting */
    0x00,       /*  number of ep */
    0x01,       /* interface class  USB_DEVICE_CLASS_COMP_AUDIO*/
    0x01,       //bInterfaceSubClass  COMP_AUDIO_SUBCLASS_AUDIOCONTROL
    0x00,       //COMP_AUDIO_PROTOCOL_UNDEFINED
    0x00,      //INTERFACE， CLASS AUDIO， SUBCLASS：audio control
};

const uint8_t USBMIC_Audio_Class_AC_desc[] = {
//class-specific AC interface descriptor
    0x09,
    0x24,      //INTERFACE DESCRYPT
    0x01,       //header descriptor subtype
    0x00,       //bcd ADC   1.0
    0x01,
#if USB_SPECIAL_SUPPORT
    0x2E,
#else    
    0x27,       //total number of bytes returned for audio control descriptor 
#endif
    0x00,
    0x01,       //bIncollection ,1 audio stream interface in collection
    0x01,       // interface number:1
};


const uint8_t USBMIC_Audio_Class_Microphone_InputTerminate_Desc[] = {
    //audio interface descriptor, input terminate , microphone ,id-4 stereo
    0x0C,
    0x24,       //interface descriptor
    0x02,       //input terminat subtype
    ID_MICROPHONE_IT,       //id of this terminate
    LOBYTE(USB_INPUT_TYPE),       // input type: microphone (0201)
    HIBYTE(USB_INPUT_TYPE),
    0x00,       //
#if USB_MIC_CHANNEL == USB_MIC_CHANNEL_MONO  
    MIC_INPUT_CHANNALS,     //1
    0x00,
#elif USB_MIC_CHANNEL == USB_MIC_CHANNEL_STERO
    MIC_INPUT_CHANNALS,       //2 channel
    0x03,       // stereo
#endif    
    0x00,
    0x00,
    0x00, 
};

const uint8_t USBMIC_Audio_Class_Microphone_FeatureUnit_Desc[] = {
    //audio interface descriptor, audio feature unit ,  one or more feature must work, otherwise the host would ignore the feature and not enum mircophone
    0x09,                                 /* bLength */
    0x24,      /* bDescriptorType */
    0x06,           /* bDescriptorSubtype */
    ID_MICROPHONE_FEATURE,             /* bUnitID */
    ID_MICROPHONE_IT,     //id of entity to witch this feature connect
    0x02,                                 /* bControlSize */
    USB_MICROPHONE_MUTE_EN | (USB_MICROPHONE_VOLUME_EN << 1) ,   //COMP_AUDIO_CONTROL_MUTE |AUDIO_CONTROL_VOLUME,/* bmaControls(0) */
    0x00,     //mute of volume control on master channel,0 means not control
    0x00,     //volume control on right font channel , this is depend on the control size
    //0x00,     //id string of this terminate 
};

#if USB_SPECIAL_SUPPORT
const uint8_t USBMIC_Audio_Class_Microphone_Selector[] = {
    0x07,
    0x24,
    0x05,
    0x06,
    0x01,
    ID_MICROPHONE_FEATURE,
    0x00
};

const uint8_t USBMIC_Audio_Class_Microphone_OutputTerminate_Desc[] = {
    //audio interface descriptor, output terminate ,id-5 , link to microphone 
    0x09,
    0x24,       //interface descriptor
    0x03,       //output terminat subtype
    ID_MICROPHONE_OT,       //id 
    0x01,       //0101  usb stream 
    0x01,       
    0x00,
    0x05,       //link to terminite 4
    0x00,      //
};

#else
const uint8_t USBMIC_Audio_Class_Microphone_OutputTerminate_Desc[] = {
    //audio interface descriptor, output terminate ,id-5 , link to microphone 
    0x09,
    0x24,       //interface descriptor
    0x03,       //output terminat subtype
    ID_MICROPHONE_OT,       //id 
    0x01,       //0101  usb stream 
    0x01,       
    0x00,
    ID_MICROPHONE_FEATURE,       //link to terminite 4
    0x00,      //
};
#endif



const uint8_t USBMIC_Audio_standard_interface_Mic_default_Dest[] = {
    0x09,
    0x04,       //interface
    0x01,       /* bInterfaceNumber: Number of Interface */
    0x00,       /* bAlternateSetting: Alternate setting */
    0x00,       //number of ep
    0x01,       /* interface class  USB_DEVICE_CLASS_COMP_AUDIO*/
    0x02,       // sub class audio stream
    0x00,
    0x00,
};

const uint8_t USBMIC_Audio_standard_interface_mic_Dest[] = {
    0x09,
    0x04,        //interface
    0x01,        /* bInterfaceNumber: Number of Interface */
    0x01,       /* bAlternateSetting: Alternate setting */
    0x01,       //number of ep
    0x01,       /* interface class  USB_DEVICE_CLASS_COMP_AUDIO*/
    0x02,       //sub class audio stream
    0x00,
    0x00,      // id of string descriptor
};

const uint8_t USBMIC_Audio_interface_class_micro_general_desc[] = {
    0x07,
    0x24,       //interface descriptor
    0x01,       //stream general
    ID_MICROPHONE_OT,       //terminate link to id 5, link to microphone
    0x01,       //interface delay
    0x01,       //0001 pcm format
    0x00,
};

const uint8_t USBMIC_Audio_interface_class_micro_format_desc[] = { //最多支持4个采样率， 9 BYTE的位置空出来，需要的时候填充
    0x08 + USB_MIC_SAMPLE_NUMBER*3 ,
    0x24,       //interface descriptor
    0x02,       //stream format type
    0x01,       //format type-1
    MIC_INPUT_CHANNALS, 
    USBD_MIC_SAMPLEBIT,       // frame size:2BYTES
    USBD_MIC_SAMPLEBIT*8,       // bit resolution:  16bits
    USB_MIC_SAMPLE_NUMBER,       // one sample rate support
    
#if USB_MIC_SR_44100_EN
    COMP_AUDIO_SAMPLE_FREQ(44100),
#endif
#if USB_MIC_SR_48000_EN
	COMP_AUDIO_SAMPLE_FREQ(48000),
#endif
#if USB_MIC_SR_96000_EN
    COMP_AUDIO_SAMPLE_FREQ(96000),
#endif
#if USB_MIC_SR_192000_EN
    COMP_AUDIO_SAMPLE_FREQ(192000),
#endif
};

const uint8_t USBMIC_Audio_EpIn_Desc[] = {
    0x09,       //
    0x05,       //end pointer desc
    COMP_AUDIO_IN_EP,   //in endpint
    0x0D,       //synchronous   , ISO
    LOBYTE(AUDIO_MIC_MPS), 
    HIBYTE(AUDIO_MIC_MPS),
    0x01,       //internal : 1ms
    0x00,       //refresh
    0x00,
//187
    
};

const uint8_t USBMIC_Audio_EpIn_class_general_Desc[] = {
    0x07,
    0x25,       //audio endpoint descriptor
    0x01,       //end point general
    0x00,       //bmAttribute, sample frequecy control
    0x00,       //no used
    0x00,       //no used
    0x00,       // //no used
};


const uint8_t USBMIC_hid_desc_interface[] = {
    //---------------------- HID --------------------------
    COMP_INTERFACE_DESC_SIZE,            /* bLength */
    USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
    0x02,                                 /* bInterfaceNumber */
    0x00,                                 /* bAlternateSetting */
    0x01,                                 /* bNumEndpoints */
    0x03,               /* bInterfaceClass */
    0x00,          /* bInterfaceSubClass */
    0x00,             /* bInterfaceProtocol */
    0x00,                                 /* iInterface */
    /* 09 ,18 byte*/
};

const uint8_t USBMIC_hid_class_report_desc[]={
    0x09,         /*bLength: HID Descriptor size*/
    COMP_HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
    0x01,         /*bcdHID: HID Class Spec release number*/
    0x02,
    0x00,         /*bCountryCode: Hardware target country*/
    0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
    0x22,         /*bDescriptorType*/
#if USB_SPECIAL_SUPPORT
    0x41,
#else
    0x21,/*wItemLength: Total length of Report descriptor*/
#endif    
    0x00,
    /* 09 ,27 byte */
};

const uint8_t USBMIC_hid_ep_desc[] = {
    /* Descriptor of Key Endpoint */
    0x07,          /*bLength: Endpoint Descriptor size*/
    USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/
    COMP_HID_EPIN_ADDR,     /*bEndpointAddress: Endpoint Address (IN)*/
    0x03,          /*bmAttributes: Interrupt endpoint*/
    0x08,           /*wMaxPacketSize: 128 Byte max */
    0x00,
    0x01,          /*bInterval: Polling Interval (10 ms)*/
    /* 07 ,34 byte */
};

#if USB_SPECIAL_SUPPORT
const uint8_t USBMIC_hid_desc_interface2[] = {
    //---------------------- HID --------------------------
    COMP_INTERFACE_DESC_SIZE,            /* bLength */
    USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
    0x03,                                 /* bInterfaceNumber */
    0x00,                                 /* bAlternateSetting */
    0x00,                                 /* bNumEndpoints */
    0x03,               /* bInterfaceClass */
    0x00,          /* bInterfaceSubClass */
    0x00,             /* bInterfaceProtocol */
    0x00,                                 /* iInterface */
    /* 09 ,18 byte*/
};

const uint8_t USBMIC_hid_class_report_desc2[]={
    0x09,         /*bLength: HID Descriptor size*/
    COMP_HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
    0x01,         /*bcdHID: HID Class Spec release number*/
    0x02,
    0x00,         /*bCountryCode: Hardware target country*/
    0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
    0x22,         /*bDescriptorType*/
    0x24,       /*wItemLength: Total length of Report descriptor*/
    0x00,
    /* 09 ,27 byte */
};

const uint8_t USBMIC_hid_report2[0x41] = {
    0x05,0x01,0x09,0x06,0xa1,0x01,0x05,0x07,
    0x19,0xe0,0x29,0xe7,0x15,0x00,0x25,0x01,
    0x75,0x01,0x95,0x08,0x81,0x02,0x95,0x01,
    0x75,0x08,0x81,0x01,0x95,0x03,0x75,0x01,
    0x05,0x08,0x19,0x01,0x29,0x03,0x91,0x02,
    0x95,0x01,0x75,0x05,0x91,0x01,0x95,0x06,
    0x75,0x08,0x15,0x00,0x26,0xff,0x00,0x05,
    0x07,0x19,0x00,0x2a,0xff,0x00,0x81,0x00,
    0xc0
};

const uint8_t USBMIC_hid_report3[0x24] = {
    0x06,0x00,0xff,0x0a,0xaa,0x55,0xa1,0x01,
    0x15,0x00,0x26,0xff,0x00,0x75,0x08,0x96,
    0x00,0x01,0x09,0x01,0x81,0x02,0x96,0x00,
    0x01,0x09,0x01,0x91,0x02,0x95,0x08,0x09,
    0x01,0xb1,0x02,0xc0
};

#endif  //#if USB_SPECIAL_SUPPORT

#define USB_MSC_CONFIG_DESC_SIZ 
#define MSC_MAX_PACKET  64
#define MSC_IN_EP       0x81
#define MSC_OUT_EP      0x01

const uint8_t usbd_mfi_msc_interface[] = {
    0x09,   /* bLength: Interface Descriptor size */
    0x04,   /* bDescriptorType: */
    IT_MSC,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x02,   /* bNumEndpoints*/
    0xff,   /* bInterfaceClass: MSC Class */
    0xf0,   /* bInterfaceSubClass : SCSI transparent*/
    0x00,   /* nInterfaceProtocol */
    0x07,          /* iInterface: */
};

const uint8_t usbd_mfi_msc_ep_in[]={
    0x07,   /*Endpoint descriptor length = 7*/
    0x05,   /*Endpoint descriptor type */
    MSC_IN_EP,   /*Endpoint address (IN, address 1) */
    0x02,   /*Bulk endpoint type */
    LOBYTE(MSC_MAX_PACKET),
    HIBYTE(MSC_MAX_PACKET),
    0x01,   /*Polling interval in milliseconds */ 
};

const uint8_t usbd_mfi_msc_ep_out[]={
//25  
  0x07,   /*Endpoint descriptor length = 7 */
  0x05,   /*Endpoint descriptor type */
  MSC_OUT_EP,   /*Endpoint address (OUT, address 1) */
  0x02,   /*Bulk endpoint type */
  LOBYTE(MSC_MAX_PACKET),
  HIBYTE(MSC_MAX_PACKET),
  0x01,     /*Polling interval in milliseconds*/
};

