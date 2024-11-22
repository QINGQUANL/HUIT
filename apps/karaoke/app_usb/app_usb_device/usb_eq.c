#define LOG_LEV 4

#include "hal_ll_usb.h"
#include "user_config.h"
#include "device.h"
#include "event.h"
#include "usb.h"
#include "hal.h"
#include "usbd_msc.h"
#include "usb_bulk_msc.h"
#include "nor_fw_upg.h"
#include "eq_uploader_common.h"
#include "usbd_def.h"
#include "usb_app_config.h"

#if USB_EQ_EN

enum {
    ACMD_S_ERASE_SECTOR = 3,                // 0x20,  erase sector
    ACMD_S_ERASE_HBLK_32KB,          // 0x52,  erase 32KB, half block
    ACMD_S_ERASE_FBLK_64KB,          // 0xD8,  erase 64KB, full block
    ACMD_S_ERASE_CHIP,                    // 0xC7,  erase chip,
};

extern void msc_data_prepare_receive(uint8_t *pbuf, uint16_t size);
extern void comp_msc_bot_send_csw(uint8_t CSW_Status);
extern int32_t nor_write_new(uint32_t ram_addr, uint32_t flash_addr, uint32_t len, uint32_t wait_busy);
extern int32_t nor_erase_new(uint32_t cmd, uint32_t flash_addr, uint8_t wait_busy);

void usb_sys_module_init();
extern USBD_COMP_MSC_BOT_HandleTypeDef scsi_ctrl;

uint32_t blockDataLength;
__ALIGN_BEGIN uint8_t g_usbEqBuffer[512] __ALIGN_END;
uint32_t g_usb_eq_addr;
uint32_t g_usb_eq_Length;

void usb_eq_init()
{
    g_usb_eq_addr= sys_flash_layout_address_get_by_tag("EQ2P", &g_usb_eq_Length);
    logi("g_usb_eq_addr %x",g_usb_eq_addr);
}

AT_SRAM(.usbd_eq)
uint8_t usb_set_eq(uint8_t *params)
{
    if (scsi_ctrl.bot_state == USBD_COMP_BOT_IDLE) /* Idle */
    {
        blockDataLength = 0;
        scsi_ctrl.scsi_blk_len =*((uint32_t*)&params[2]);
        scsi_ctrl.scsi_blk_addr = *((uint32_t*)&params[6]);

        logi("rc eq cmd %x %x",scsi_ctrl.scsi_blk_len, scsi_ctrl.cbw.dDataLength);
        if (scsi_ctrl.cbw.dDataLength != scsi_ctrl.scsi_blk_len)
        {
          return -1;
        }

        /* Prepare EP to receive first data packet */
        scsi_ctrl.bot_state = USBD_COMP_BOT_DATA_OUT;
        msc_data_prepare_receive (g_usbEqBuffer/*scsi_ctrl.bot_data*/,MIN (scsi_ctrl.scsi_blk_len, COMP_MSC_MEDIA_PACKET));
        return 0;
    }
    else /* Write Process ongoing */
    {
        uint32_t len;
        len = MIN(scsi_ctrl.scsi_blk_len, COMP_MSC_MEDIA_PACKET);

        memcpy(g_usbEqBuffer + blockDataLength, scsi_ctrl.bot_data, len);

        scsi_ctrl.scsi_blk_addr += len;
        scsi_ctrl.scsi_blk_len -= len;
        blockDataLength += len;
        
        /* case 12 : Ho = Do */
        scsi_ctrl.csw.dDataResidue -= len;
        if (scsi_ctrl.scsi_blk_len == 0)
        {
            int i;
            for(i=0;i< 64;i++)
            {
                if(/*scsi_ctrl.bot_data[8*i]*/g_usbEqBuffer[8*i] == 0xFF)
                {
                    break;
                }
            }

            uint16_t length = 8*i;
            logi("usb_set_eq %x", length);
            common_set_eq_data_isr(/*scsi_ctrl.bot_data*/g_usbEqBuffer, length);
            blockDataLength = 0;
            comp_msc_bot_send_csw(USBD_COMP_CSW_CMD_PASSED);
        }
        else
        {
            /* Prepare EP to Receive next packet */
            msc_data_prepare_receive(scsi_ctrl.bot_data , MIN(scsi_ctrl.scsi_blk_len, COMP_MSC_MEDIA_PACKET));
        }
    }
    
    return 1;
}

AT_SRAM(.usbd_eq)
int8_t comp_scsi_common_process_write()
{
    uint32_t len;
    len = MIN(scsi_ctrl.scsi_blk_len, COMP_MSC_MEDIA_PACKET);

    memcpy(g_usbEqBuffer + blockDataLength, scsi_ctrl.bot_data, len);
    //memcpy(buff + blockDataLength , scsi_ctrl.bot_data, 64);

    scsi_ctrl.scsi_blk_addr += len;
    scsi_ctrl.scsi_blk_len -= len;
    blockDataLength += len;

    if((blockDataLength == 256) || (scsi_ctrl.scsi_blk_len == 0)) {
        logi("scsi_ctrl.scsi_blk_addr %x %x",g_usb_eq_addr,  g_usb_eq_addr + scsi_ctrl.scsi_blk_addr - blockDataLength,blockDataLength);
        if(g_usb_eq_addr > 1024 * 128) {  
            nor_write_new((uint32_t)g_usbEqBuffer, g_usb_eq_addr + scsi_ctrl.scsi_blk_addr - blockDataLength, blockDataLength, 1);
        }
        blockDataLength = 0;
    }
    
    /* case 12 : Ho = Do */
    scsi_ctrl.csw.dDataResidue -= len;
    if (scsi_ctrl.scsi_blk_len == 0)
    {
        blockDataLength = 0;
        comp_msc_bot_send_csw(USBD_COMP_CSW_CMD_PASSED);
    }
    else
    {
        /* Prepare EP to Receive next packet */
        msc_data_prepare_receive(scsi_ctrl.bot_data , MIN(scsi_ctrl.scsi_blk_len, COMP_MSC_MEDIA_PACKET));
    }

    return 1;
}


AT_SRAM(.usbd_eq)
int8_t comp_scsi_common_write()
{
    if (scsi_ctrl.bot_state == USBD_COMP_BOT_IDLE) /* Idle */
    {
        scsi_ctrl.scsi_blk_len = *(uint32_t*)(scsi_ctrl.cbw.CB + 2);
        scsi_ctrl.scsi_blk_addr = *((uint32_t*)(scsi_ctrl.cbw.CB + 6));

        blockDataLength = 0;
        /* cases 3,11,13 : Hn,Ho <> D0 */
        if (scsi_ctrl.cbw.dDataLength != scsi_ctrl.scsi_blk_len)
        {
            return -1;
        }

        /* Prepare EP to receive first data packet */
        scsi_ctrl.bot_state = USBD_COMP_BOT_DATA_OUT;
        msc_data_prepare_receive(scsi_ctrl.bot_data, MIN(scsi_ctrl.scsi_blk_len, COMP_MSC_MEDIA_PACKET));
    }
    else /* Write Process ongoing */
    {
        return comp_scsi_common_process_write();
    }
    return 0;
}

AT_SRAM(.usbd_eq)
uint8_t usb_data_read(uint8_t* pBuff, uint8_t *params)
{
    if (scsi_ctrl.bot_state == USBD_COMP_BOT_IDLE) {
        scsi_ctrl.scsi_blk_len =*((uint32_t*)&params[2]);
        scsi_ctrl.scsi_blk_addr = *((uint32_t*)&params[6]);

        uUPDrvCmdType cmdType;
        cmdType.uCmd = 0x07;
        memset(g_usbEqBuffer,0x00,512);
        logi("read addr %x",g_usb_eq_addr + scsi_ctrl.scsi_blk_addr);
        CMP_UPG_Norf_Read(g_usbEqBuffer,g_usb_eq_addr + scsi_ctrl.scsi_blk_addr,512, (uint8_t *)&cmdType);
        scsi_ctrl.bot_state = USBD_COMP_BOT_DATA_IN;
        blockDataLength = 0;
    }
       
    if(scsi_ctrl.scsi_blk_len < 64) {
        scsi_ctrl.scsi_blk_len = 0;
    } else {
        scsi_ctrl.scsi_blk_len -= 64;
    }

    USB_EPnStartTX(g_usbEqBuffer + blockDataLength, 64, EP_BULK);
    blockDataLength += 64;
    scsi_ctrl.bot_data_length = MIN(scsi_ctrl.scsi_blk_len, 64);
    if (scsi_ctrl.scsi_blk_len == 0)
    {
        scsi_ctrl.bot_state = USBD_COMP_BOT_LAST_DATA_IN;
    }
        
    return 0;
 }

AT_SRAM(.usbd_eq)
int8_t usb_eq_write (uint8_t *params)
{
    if (scsi_ctrl.bot_state == USBD_COMP_BOT_IDLE) /* Idle */
    {
        scsi_ctrl.scsi_blk_len =*((uint32_t*)&params[2]);
        scsi_ctrl.scsi_blk_addr = *((uint32_t*)&params[6]);

        /* cases 3,11,13 : Hn,Ho <> D0 */
        if (scsi_ctrl.cbw.dDataLength != scsi_ctrl.scsi_blk_len)
        {
          return -1;
        }

        /* Prepare EP to receive first data packet */
        scsi_ctrl.bot_state = USBD_COMP_BOT_DATA_OUT;
        msc_data_prepare_receive (scsi_ctrl.bot_data,MIN (scsi_ctrl.scsi_blk_len, COMP_MSC_MEDIA_PACKET));
        //logi("prepare receive data");
        return 0;//has data
    }
    else /* Write Process ongoing */
    {
        logi("prepare write data");
        return comp_scsi_common_process_write(params);
    }
    return 0;
}

uint32_t get_cpu0_loadper();
uint32_t get_cpu1_loadper();
uint32_t audio_engine_get_avaheapsize(void);
uint32_t OAL_heap_freespace_get( void );


AT_SRAM(.usbd_eq)
int8_t usbExtendCommand(uint8_t *params)
{
    uint8_t res = 0;
    if(params[1] == 0xDA){
        res = usb_set_eq(params);
    } else if(params[1] == 0xDB) {
        //on_get_available_space((uint32_t*)scsi_ctrl.bot_data, (uint32_t*)(scsi_ctrl.bot_data+4));
        *(uint32_t*)(scsi_ctrl.bot_data) = get_cpu0_loadper();
        *(uint32_t*)(scsi_ctrl.bot_data + 4) = get_cpu1_loadper();
        *(uint32_t*)(scsi_ctrl.bot_data + 8) = SYS_HEAP_SIZE;
        *(uint32_t*)(scsi_ctrl.bot_data + 12) = AUDIO_HEAP_SIZE_TURNING;
        *(uint32_t*)(scsi_ctrl.bot_data + 16) = OAL_heap_freespace_get();
        *(uint32_t*)(scsi_ctrl.bot_data + 20) = audio_engine_get_avaheapsize();
        scsi_ctrl.bot_data_length = 64;
    }else if(params[1] == 0xDC){
        memset(scsi_ctrl.bot_data,0xFF,512);
        //on_get_module_space((uint8_t*)scsi_ctrl.bot_data);
        scsi_ctrl.bot_data_length = 512;
    }else if(params[1] == 0xDD){
        //on_usb_eq_prepare_ready();
        scsi_ctrl.bot_data_length = 0;
        res = 1;    //no data
    }else if(params[1] == 0xDE){
        //on_get_aprx_cpuload((uint32_t*)scsi_ctrl.bot_data, (uint32_t*)(scsi_ctrl.bot_data+4));
        scsi_ctrl.bot_data_length = 0x08;
    }else if(params[1] == 0xD9){
        logi("prepare for update");
        scsi_ctrl.bot_data_length = 0;
        res = 1;    //no data
        event_put(SYSTEM_EVENT_UPDATE_STANBY, 0 ,0);
    }else if(params[1] == 0xD8) {
        *(uint32_t*)scsi_ctrl.bot_data = EQ_TOOL_FUNC0;
        *(uint32_t*)(scsi_ctrl.bot_data + 4) = EQ_TOOL_FUNC1;
        *(uint32_t*)(scsi_ctrl.bot_data + 8) = EQ_TOOL_FUNC2;
        *(uint32_t*)(scsi_ctrl.bot_data + 12) = EQ_TOOL_FUNC3;
        *(uint32_t*)(scsi_ctrl.bot_data + 16) = EQ_TOOL_FUNC4;
        *(uint32_t*)(scsi_ctrl.bot_data + 20) = EQ_TOOL_FUNC5;
        *(uint32_t*)(scsi_ctrl.bot_data + 24) = EQ_TOOL_FUNC6;
        *(uint32_t*)(scsi_ctrl.bot_data + 28) = EQ_TOOL_FUNC7;
        
        scsi_ctrl.bot_data_length = 32;
    }else if((params[1] & 0x10) == 0x10 ){    //read flash id
        uint8_t cmd = params[1] & (~0x10);
        __maybe_unused uint32_t addr = *(uint32_t*)(params + 6);
        //logi("cmd: %x , %x  %x",cmd ,addr, *((uint32_t*)&params[2]));

        switch(cmd) {
            case NOR_CMDF_ID_READ:
                CMP_UPG_Norf_Comm(scsi_ctrl.bot_data, params);
                scsi_ctrl.bot_data_length = *((uint32_t*)&params[2]);
                break;
            case NOR_CMDF_WRITE_BYTE:
            case NOR_CMDF_WRITE_PAGE:
            case NOR_CMDS_WRITE_RAM:
                logi("write page");
                res = usb_eq_write(params);
                break;
            case NOR_CMDF_ERASE_SECTOR:
                logi("erase %x", g_usb_eq_addr + addr*0x1000);
                if(g_usb_eq_addr > 1024 * 128) {
                    nor_erase_new(ACMD_S_ERASE_SECTOR, g_usb_eq_addr + addr*0x1000, 1);
                }
                logi("erase end");
                res = 1;    //no data
            break;
            case NOR_CMDF_ERASE_CHIP:
                logi("erase chip");
                //nor_erase_new(ACMD_S_ERASE_CHIP, addr, 1);
                logi("erase chip end");
                break;
            case NOR_CMDF_READ:
            case NOR_CMDF_READ_FAST:
                logi("read");
                res = usb_data_read(scsi_ctrl.bot_data,params);
                logi("read end");
            break;
            case NOR_CMDD_GET_CRC:
                {
                uint32_t addr = *(uint32_t*)(params+6);
                *(uint32_t*)(params+6) = addr + g_usb_eq_addr;
                logi("get crc %x" ,addr + g_usb_eq_addr);
                
                CMP_UPG_Nor_GetCRC((uint32_t *)scsi_ctrl.bot_data,params);
                scsi_ctrl.bot_data_length =0x02;
                }
            break;

            default:
                logi("unkown cmd %x",params[1]);
            break;
        }

    }
    else {
        //scsi_ctrl.bot_data_length = on_get_afxmsk_clbk(params[1], (uint32_t *)scsi_ctrl.bot_data);
        //loge("++++ %d, %d", scsi_ctrl.bot_data_length, scsi_ctrl.bot_data[0]);
    }

    return res;   //unkown cmd or have no data
}

#endif  //USB_EQ_EN

