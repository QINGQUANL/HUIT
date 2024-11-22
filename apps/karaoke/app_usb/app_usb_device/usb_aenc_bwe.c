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
#include "sltk_aenc.h"
#include "sltk_drc.h"
#include "usb_aenc_bwe.h"

#if USB_AENC_TOOLS_EN
extern void msc_data_prepare_receive(uint8_t *pbuf, uint16_t size);
extern void comp_msc_bot_send_csw(uint8_t CSW_Status);
extern int32_t nor_write_new(uint32_t ram_addr, uint32_t flash_addr, uint32_t len, uint32_t wait_busy);
extern int32_t nor_erase_new(uint32_t cmd, uint32_t flash_addr, uint8_t wait_busy);
void aenc_set_prm_block(uint8_t* aenc_buff, int16_t length);
aenc_rtprm_t* aenc_get_aenc_prm_tmp();
void update_drc_tx_prm();
void update_aenc_prm();
void update_aenc_prm_eq();
aenc_eqrtprm_t* aenc_get_prm_eq();

void usb_sys_module_init();
extern USBD_COMP_MSC_BOT_HandleTypeDef scsi_ctrl;

uint32_t blockDataLength;
__ALIGN_BEGIN uint8_t g_usbEqBuffer[512] __ALIGN_END;
uint32_t g_usb_eq_addr;
uint32_t g_usb_eq_Length;

void usb_aenc_bwe_init()
{
    //g_usb_eq_addr= sys_flash_layout_address_get_by_tag("EQ2P", &g_usb_eq_Length);
    //logi("g_usb_eq_addr %x",g_usb_eq_addr);
}

AT_SRAM(.usbd_aenc)
uint8_t usb_set_aenc(uint8_t *params)
{
    if (scsi_ctrl.bot_state == USBD_COMP_BOT_IDLE) /* Idle */
    {
        blockDataLength = 0;
        scsi_ctrl.scsi_blk_len =*((uint32_t*)&params[2]);
        scsi_ctrl.scsi_blk_addr = *((uint32_t*)&params[6]);

       // logi("rc eq cmd %x %x",scsi_ctrl.scsi_blk_len, scsi_ctrl.cbw.dDataLength);
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

        //logi("blockDataLength %d", blockDataLength);
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

            __maybe_unused uint16_t length = 8*i;
            //logi("usb_set_aenc %x", length);
#if AUDIO_MIC_AENC            
            aenc_set_prm_block(g_usbEqBuffer, length);
#elif AUDIO_MIC_NSBWE_AENC
            nsbwe_set_prm_block(g_usbEqBuffer, length);
#endif  
                
            blockDataLength = 0;
            comp_msc_bot_send_csw(USBD_COMP_CSW_CMD_PASSED);
        }
        else
        {
            /* Prepare EP to Receive next packet */
            msc_data_prepare_receive(scsi_ctrl.bot_data , MIN(scsi_ctrl.scsi_blk_len, COMP_MSC_MEDIA_PACKET));
        }
    }
    //logi("aenc end");
    return 1;
}

AT_SRAM(.usbd_aenc)
int8_t comp_aenc_scsi_common_process_write()
{
    uint32_t len;
    len = MIN(scsi_ctrl.scsi_blk_len, COMP_MSC_MEDIA_PACKET);

    memcpy(g_usbEqBuffer + blockDataLength, scsi_ctrl.bot_data, len);
    //memcpy(buff + blockDataLength , scsi_ctrl.bot_data, 64);

    scsi_ctrl.scsi_blk_addr += len;
    scsi_ctrl.scsi_blk_len -= len;
    blockDataLength += len;

    if((blockDataLength == 256) || (scsi_ctrl.scsi_blk_len == 0)) {
        //logi("scsi_ctrl.scsi_blk_addr %x %x",g_usb_eq_addr,  g_usb_eq_addr + scsi_ctrl.scsi_blk_addr - blockDataLength,blockDataLength);
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


AT_SRAM(.usbd_aenc)
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
        return comp_aenc_scsi_common_process_write();
    }
    return 0;
}

AT_SRAM(.usbd_aenc)
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

AT_SRAM(.usbd_aenc)
int8_t usb_aenc_write (uint8_t *params)
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
        return comp_aenc_scsi_common_process_write(params);
    }
    return 0;
}

AT_SRAM(.usbd_aenc)
int8_t usbAnecbweExtendCommand(uint8_t *params)
{
    uint8_t res = 0;
    if(params[1] == 0xAE){
        res = usb_set_aenc(params);
    }else if(params[1] == 0xAF){    //get function
        //获取底层支持的功能
        *(uint32_t*)(scsi_ctrl.bot_data) = (AUDIO_MIC_AENC | (AUDIO_MIC_NSBWE_AENC << 1));
        //MIC设置
        *(uint32_t*)(scsi_ctrl.bot_data + 4) =  AUDIO_REC_INPUT_SEL;
        
        scsi_ctrl.bot_data_length = 8;
    }

    return res;   //unkown cmd or have no data
}

#if AUDIO_MIC_AENC

aenc_drcrtprm_t* aenc_get_drc_tx_prm_tmp();
void reset_aenc_prm();

AT_SRAM(.usbd_aenc)
void aenc_set_drc_tx_prm_data(aencbwe_data* eqData)
{
    aenc_drcrtprm_t* p = aenc_get_drc_tx_prm_tmp();
    logi("aenc_drcrtprm_t %x, %x, eq %x", p, p->lt_thd, eqData);
    switch(eqData->item_type) {
        case e_drc_switch:
            {
                if(eqData->item_index == AMPLITUDE) {
                    if(eqData->fill.fdata) {
                        p->func_mask |= MASK_LIMITER;
                    } else {
                        p->func_mask &= ~MASK_LIMITER;
                    }
                }else if(eqData->item_index == NOISE) {
                    if(eqData->fill.fdata) {
                        p->func_mask |= MASK_NOISEGATE;
                    } else {
                        p->func_mask &= ~MASK_NOISEGATE;
                    }
                }
            }
            break;
        case e_drc_thd:
            if(eqData->item_index == AMPLITUDE ) {
                p->lt_thd = (int8_t)eqData->fill.fdata;
            } else if(eqData->item_index == NOISE) {
                p->ng_thd = (int8_t)eqData->fill.fdata;
            }
            break;
        case e_drc_att:
            if(eqData->item_index == AMPLITUDE ) {
                p->lt_att= (int16_t)eqData->fill.fdata;
            } else if(eqData->item_index == NOISE) {
                p->ng_att = (int16_t)eqData->fill.fdata;
            }
            break;
        case e_drc_ratio:
            if(eqData->item_index == AMPLITUDE ) {
                p->lt_ratio = (int8_t)eqData->fill.fdata;
            } else if(eqData->item_index == NOISE) {
                p->ng_ratio = (int8_t)eqData->fill.fdata;
            }
            break;
        case e_drc_rel:
            if(eqData->item_index == AMPLITUDE ) {
                p->lt_rel = (int16_t)eqData->fill.fdata;
            } else if(eqData->item_index == NOISE) {
                p->ng_rel = (int16_t)eqData->fill.fdata;
            }
            break;
        case e_drc_hold:
            if(eqData->item_index == AMPLITUDE ) {
                p->lt_rel = (int16_t)eqData->fill.fdata;
            } else if(eqData->item_index == NOISE) {
                p->ng_hold = (int16_t)eqData->fill.fdata;
            }
            break;
        case e_drc_kw:
            if(eqData->item_index == AMPLITUDE ) {
                p->lt_kw = (int8_t)eqData->fill.fdata;
            } else if(eqData->item_index == NOISE) {
                p->ng_kw = (int8_t)eqData->fill.fdata;
            }
            break;
        case e_lookahead:
            p->look_ahead = (int8_t)eqData->fill.fdata;
            break;
        case e_makeupgain:
            p->makeup_gain = (int8_t)eqData->fill.fdata;
            break;
    }
    update_drc_tx_prm();
}

AT_SRAM(.usbd_aenc)
void aenc_set_prm_data(aencbwe_data* eqData)
{
    logi("prm data, type %d, index %d, data %d ",eqData->item_type, eqData->item_index,(int)(eqData->fill.fdata*1000) );

     if(eqData->item_type >= e_eq_fre && eqData->item_type <= e_eq_predecay){
        aenc_eqrtprm_t* p = aenc_get_prm_eq();
        if(eqData->item_index >= 5) {
            return ;
        }
        
        biquad_setting_t* eq = &p->setting[eqData->item_index];
        switch(eqData->item_type) {
        case e_eq_fre:
            eq->fc = (uint16_t)eqData->fill.fdata;
            break;
        case e_eq_q:
            eq->Q_factor = eqData->fill.fdata;
            break;
        case e_eq_gain:
            eq->G = eqData->fill.fdata;
            break;
        case e_eq_type:
            eq->filter_type = (int16_t)eqData->fill.fdata;
            break;
        case e_eq_enable:
            if(!eqData->fill.fdata) { //1
                eq->G = 0;//eqData->fill.u16data;
                eq->filter_type = 1 ;// 平衡
            }
            break;
        case e_eq_predecay:
            p->PreAmp = eqData->fill.fdata;
            break;
        }
        update_aenc_prm_eq();
    } else {
        aenc_rtprm_t* p = aenc_get_aenc_prm_tmp();
        switch(eqData->item_type) {
        case e_aec:
            if(eqData->fill.fdata) {
                p->func_mask |= MASK_AEC;
            } else {
                p->func_mask &= ~MASK_AEC;
            }
            break;
        case e_aes:
            if(eqData->fill.fdata) {
                p->func_mask |= MASK_AES;
            } else {
                p->func_mask &= ~MASK_AES;
            }
            break;
        case e_ns:
            if(eqData->fill.fdata) {
                p->func_mask |= MASK_NS;
            } else {
                p->func_mask &= ~MASK_NS;
            }
            break;
        case e_dr:
            if(eqData->fill.fdata) {
                p->func_mask |= MASK_DR;
            } else {
                p->func_mask &= ~MASK_DR;
            }
            break;
        case e_avc:
            if(eqData->fill.fdata) {
                p->func_mask |= MASK_AVC;
            } else {
                p->func_mask &= ~MASK_AVC;
            }
            break;
        case e_txagc:
            if(eqData->fill.fdata) {
                p->func_mask |= MASK_TXAGC;
            } else {
                p->func_mask &= ~MASK_TXAGC;
            }
            break;
        case e_rxagc:
            if(eqData->fill.fdata) {
                p->func_mask |= MASK_RXAGC;
            } else {
                p->func_mask &= ~MASK_RXAGC;
            }
            break;
        case e_cng:
            if(eqData->fill.fdata) {
                p->func_mask |= MASK_CNG;
            } else {
                p->func_mask &= ~MASK_CNG;
            }
            break;
        case e_emp:
            if(eqData->fill.fdata) {
                p->func_mask |= MASK_EMP;
            } else {
                p->func_mask &= ~MASK_EMP;
            }
            break;
        case e_txnot:
            if(eqData->fill.fdata) {
                p->func_mask |= MASK_TXNOT;
            } else {
                p->func_mask &= ~MASK_TXNOT;
            }
            break;
        case e_gssmooth:
            if(eqData->fill.fdata) {
                p->func_mask |= MASK_GSMOOTH;
            } else {
                p->func_mask &= ~MASK_GSMOOTH;
            }
            break;
        case e_tx_eq:
            if(eqData->fill.fdata) {
                p->func_mask |= MASK_TXEQ;
            } else {
                p->func_mask &= ~MASK_TXEQ;
            }
            break;
        case e_vad:
            if(eqData->fill.fdata) {
                p->func_mask |= MASK_INTVAD;
            } else {
                p->func_mask &= ~MASK_INTVAD;
            }
            break;
        case e_smsnr:
            break;
            
        case e_finit:
            p->finit = eqData->fill.fdata;
            break;
        case e_pf_xi_opt:
            p->pf_Xi_opt = eqData->fill.fdata;
            break;
        case e_pf_ph1_thd:
            p->pf_ph1_thd = eqData->fill.fdata;
            break;
        case e_pf_GHmin:
            p->pf_GHmin = eqData->fill.fdata;
            break;
        case e_SNR_s_Min:
            p->SNR_s_Min = eqData->fill.fdata;
            break;
        case e_SNR_ns_Min:
            p->SNR_ns_Min = eqData->fill.fdata;
            break;
        case e_pf_Gcn:
            p->pf_Gcn = eqData->fill.fdata;
            break;
        case e_rse_maxG:
            p->rse_maxG = eqData->fill.fdata;
            break;
        case e_ref_thd:
            p->ref_thd = eqData->fill.fdata;
            break;
        case e_dt_overest:
            p->dt_overest = eqData->fill.fdata;
            break; 
        case e_alpha_slow:
            p->alpha_slow = eqData->fill.fdata;
            break;
        case e_alpha_fast:
            p->alpha_fast = eqData->fill.fdata;
            break;
        case e_res_overest:
            p->res_overest = eqData->fill.fdata;
            break;
        case e_alpha_fast_resG:
            p->alpha_fast_resG = eqData->fill.fdata;
            break;
        case e_alpha_slow_resG:
            p->alpha_slow_resG = eqData->fill.fdata;
            break;
        case e_dt_thd_norm:
            p->dt_thd_norm = eqData->fill.fdata;
            break;
        case e_dt_thd_limit:
            p->dt_thd_limit = eqData->fill.fdata;
            break;
        case e_dt_init_thd:
            p->dt_init_thd = eqData->fill.fdata;
            break;
        case e_dt_steprise_decvar:
            p->dt_steprise_decvar = eqData->fill.fdata;
            break;
        case e_dt_stepfall_decvar:
            p->dt_stepfall_decvar = eqData->fill.fdata;
            break;
        case e_pf_alpha_npsd:
            p->pf_alpha_npsd = eqData->fill.fdata;
            break;
        case e_pf_alpha_PH1:
            p->pf_alpha_PH1 = eqData->fill.fdata;
            break;
        case e_pf_alpha_Xis:
            p->pf_alpha_Xis = eqData->fill.fdata;
            break;
        case e_pf_alpha_Xins:
            p->pf_alpha_Xins = eqData->fill.fdata;
            break;
        case e_pf_alpha_z:
            p->pf_alpha_z = eqData->fill.fdata;
            break;
        case e_pf_alpha_Nmin_f:
            p->pf_alpha_Nmin_f = eqData->fill.fdata;
            break;
        case e_pf_inc_Nmin_t:
            p->pf_inc_Nmin_t = eqData->fill.fdata;
            break;
        case e_pf_decay:
            p->pf_decay = eqData->fill.fdata;
            break;
        case e_avq_reftx:
            p->avq_reftx = eqData->fill.fdata;
            break;
        case e_avq_refrx:
            p->avq_refrx = eqData->fill.fdata;
            break;
        case e_avq_vadthd:
            p->avq_vadthd = eqData->fill.fdata;
            break;
        case e_avq_offset:
            p->avq_offset = eqData->fill.fdata;
            break;
        case e_avq_dec_step:
            p->avq_dec_step = eqData->fill.fdata;
            break;
        case e_avq_sf_norm:
            p->avq_sf_norm = eqData->fill.fdata;
            break;
        case e_avq_alpha_fref:
            p->avq_alpha_fref = eqData->fill.fdata;
            break;
        case e_avq_alpha_sref:
            p->avq_alpha_sref = eqData->fill.fdata;
            break;
        case e_avq_alpha_fn:
            p->avq_alpha_fn = eqData->fill.fdata;
            break;
        case e_avq_alpha_sn:
            p->avq_alpha_sn = eqData->fill.fdata;
            break;
        case e_avq_Gmax:
            p->avq_Gmax = eqData->fill.fdata;
            break;
        case e_Npsd_bias:
            p->Npsd_bias = eqData->fill.fdata;
            break;
        case e_decay_lthd:
            p->decay_lthd = eqData->fill.fdata;
            break;
        case e_alpha_decay:
            p->alpha_decay = eqData->fill.fdata;
            break;
        case e_decay_km:
            p->decay_km = eqData->fill.fdata;
            break;
        case e_rise_km:
            p->rise_km = eqData->fill.fdata;
            break;
        case e_thd_km:
            p->thd_km = eqData->fill.fdata;
            break;
        case e_gain_smooth_thd:
            p->gain_smooth_thd = eqData->fill.fdata;
            break;
        case e_gain_smooth_width:
            p->gain_smooth_width = eqData->fill.fdata;
            break;
        case e_rse_thd:
            p->rse_thd = eqData->fill.fdata;
            break;
        case e_vadinc_time:
            p->vadinc_time = eqData->fill.fdata;
            break;
        case e_vaddec_time:
            p->vaddec_time = eqData->fill.fdata;
            break;
        case e_vadNmin:
            p->vadNmin = eqData->fill.fdata;
            break;
        case e_vadVmin:
            p->vadVmin = eqData->fill.fdata;
            break;
        default:
            logi("cmd error, %d", eqData->item_type);
            break;
        }
        update_aenc_prm();
    }
}

AT_SRAM(.usbd_aenc)
void aenc_set_prm_block(uint8_t* aenc_buff, int16_t length)
{
    logi("aenc_set_prm_block %d", length);
    printf_array(aenc_buff, length);
    aencbwe_data* p;
    for(int i=0; i < length; i += 8){
        p = (aencbwe_data*)(aenc_buff + i);
        if(p->item_type <= e_makeupgain) {
            aenc_set_drc_tx_prm_data((aencbwe_data*)(aenc_buff + i));
        } else if(p->item_type <= e_vadVmin) {
            aenc_set_prm_data((aencbwe_data*)(aenc_buff + i));
        } else if(p->item_type == e_reset_all){
            reset_aenc_prm_eq();
            reset_drc_tx_prm();
            reset_aenc_prm();
        }
    }
}

#elif AUDIO_MIC_NSBWE_AENC
extern nsbwe_rtprm_t* nsbwe_get_rtprm_tmp();
void update_nsbwe_rtprm();

void nsbwe_rtprm_data(aencbwe_data* eqData)
{
    nsbwe_rtprm_t* nsbwe = nsbwe_get_rtprm_tmp();
    switch(eqData->item_type){
        case e_bwe_finit:
            nsbwe->finit = (uint16_t)eqData->fill.fdata;
            break;
        case e_bwe_alpha_npsd:
            nsbwe->alpha_npsd = eqData->fill.fdata;
            break;
        case e_bwe_alpha_xis:
            nsbwe->alpha_xis = eqData->fill.fdata;
            break;
        case e_bwe_GHmin:
            nsbwe->GHmin = eqData->fill.fdata;
            break;
        case e_bwe_Psdmin:
            nsbwe->Psdmin = eqData->fill.fdata;
            break;
        case e_bwe_Npsd_bias:
            nsbwe->Npsd_bias = eqData->fill.fdata;
            break;
        case e_bwe_SNR_s_min:
            nsbwe->SNR_s_min = eqData->fill.fdata;
            break;
        case e_bwe_vthd:
            nsbwe->vthd = eqData->fill.fdata;
            break;
    }
    update_nsbwe_rtprm();
}


AT_SRAM(.usbd_aenc)
void nsbwe_set_prm_block(uint8_t* aenc_buff, int16_t length)
{
    aencbwe_data* p;
    for(int i=0; i < length; i += 8){
        p = (aencbwe_data*)(aenc_buff + i);
        if(p->item_type <= e_bwe_vthd && p->item_type >= e_bwe_finit) {
            nsbwe_rtprm_data((aencbwe_data*)(aenc_buff + i));
        } else{
            logi("item error %d", p->item_type);
        }
    }
}

#endif

#endif

