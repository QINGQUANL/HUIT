#ifndef EQ_UPLOADER_COMMON_H
#define EQ_UPLOADER_COMMON_H

#include "atunning_common.h"
#include "aeffect_plugin.h"

extern void COMP_HID_Send_Data(uint8_t* buff,uint8_t length );

#define CHECK_AND_PREPARE_BUFFER(buf, size, exception) \
    if(size != 64) exception;\
    else libc_memset(buf, 0xff, size)

#define SEND_DATA_TO_PC(buf) \
    COMP_HID_Send_Data(buf, 64)

void on_upload_for_page_id(PageClass page_id);
void on_usb_eq_prepare_ready();
void on_get_available_space(uint32_t* total_space_in_byte, uint32_t* available_space_in_byte);
void common_set_eq_data_isr(uint8_t* buff,uint32_t length);
void on_get_aprx_cpuload(uint32_t* cpu0_load_per, uint32_t* cpu1_load_per);
void on_get_module_space();
#endif
