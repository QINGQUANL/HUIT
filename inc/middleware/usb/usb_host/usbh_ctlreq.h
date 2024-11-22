
#ifndef __USBH_CTLREQ_H
#define __USBH_CTLREQ_H

#include "hal_ll_usb.h"
#include "usbh_def.h"

/*Standard Feature Selector for clear feature command*/
#define FEATURE_SELECTOR_ENDPOINT         0X00
#define FEATURE_SELECTOR_DEVICE           0X01


#define INTERFACE_DESC_TYPE               0x04
#define ENDPOINT_DESC_TYPE                0x05
#define INTERFACE_DESC_SIZE               0x09

extern uint8_t USBH_CfgDesc[512];

USBH_StatusTypeDef usbh_ctl_req(uint8_t *buff, uint16_t length);

USBH_StatusTypeDef usbh_get_descriptor(uint8_t  req_type,
    uint16_t value_idx,
    uint8_t* buff,
    uint16_t length);

USBH_StatusTypeDef usbh_get_dev_desc(uint8_t length);

USBH_StatusTypeDef usbh_get_string_desc(uint8_t string_index, uint16_t strEncode,
    uint8_t *buff,
    uint16_t length);

USBH_StatusTypeDef usbh_set_cfg(uint16_t configuration_value);

USBH_StatusTypeDef usbh_get_cfg_desc(uint16_t length);

USBH_StatusTypeDef usbh_set_address(uint8_t DeviceAddress);

USBH_StatusTypeDef usbh_set_interface(uint8_t ep_num, uint8_t altSetting);

USBH_StatusTypeDef usbh_clr_feature(uint8_t ep_num);
USBH_StatusTypeDef usbh_get_maxlun(uint8_t *Maxlun);
USBH_DescHeader_t      *usbh_get_next_desc(uint8_t   *pbuf, uint16_t  *ptr);

void usbh_parse_dev_desc(USBH_DevDescTypeDef* dev_desc, uint8_t *buf, uint16_t length);
void usbh_parse_cfg_desc(USBH_CfgDescTypeDef* cfg_desc, uint8_t *buf, uint16_t length);
void usbh_parse_interface_desc(USBH_InterfaceDescTypeDef *if_descriptor, uint8_t *buf);
void usbh_parse_string_desc(uint8_t* psrc, uint8_t* pdest, uint16_t length);
void usbh_parse_ep_desc(USBH_EpDescTypeDef  *ep_descriptor, uint8_t *buf);
#endif /* __USBH_CTLREQ_H */

/**
  * @}
  */


