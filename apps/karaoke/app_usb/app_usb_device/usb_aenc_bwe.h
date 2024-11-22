#ifndef _USB_AENC_BWE_H_
#define _USB_AENC_BWE_H_

enum EQ_EQUIPMENT
{
	AMPLITUDE,
	COMPRESS,
	EXPANSION,
	NOISE
};


enum E_Item_Type
{
	e_type_reserved = 0,
	e_drc_switch,	//1 DRC开关
	e_drc_thd,		//2 启动电平
	e_drc_att,		//3	时间
	e_drc_ratio,	//4 压缩比例
	e_drc_rel,		//5 释放时间
	e_drc_hold,		//6 保持时间
	e_drc_kw,		//7 过渡宽度

	e_lookahead,	//8 前视
	e_makeupgain,	//9 补偿增益

	//tx prm mask 
	e_aec		= 0x20,	
	e_aes,
	e_ns,
	e_dr,
	e_avc,
	e_txagc,
	e_rxagc,
	e_cng,
	e_emp,
	e_txnot,
	e_gssmooth,	
	e_tx_eq,
	e_vad,
	e_smsnr,
	
	//eq
	e_eq_fre    = 0x30,
	e_eq_q,
	e_eq_gain,
	e_eq_type,
	e_eq_enable,
	e_eq_predecay,

	//aenc_rt_prm  base
	e_finit     = 0x40,
	e_pf_xi_opt,
	e_pf_ph1_thd,
	e_pf_GHmin,
	e_SNR_s_Min,
	e_SNR_ns_Min,
	e_pf_Gcn,       //70
	e_rse_maxG,
	e_ref_thd,
	e_dt_overest,
	e_init_overest,
	//aenc_rt_prm  advance
	e_alpha_slow,
	e_alpha_fast,
	e_res_overest,
	e_alpha_fast_resG,
	e_alpha_slow_resG,
	e_dt_thd_norm,  //80
	e_dt_thd_limit,
	e_dt_init_thd,
	e_dt_steprise_decvar,
	e_dt_stepfall_decvar,
	e_pf_alpha_npsd,
	e_pf_alpha_PH1,
	e_pf_alpha_Xis,
	e_pf_alpha_Xins,
	e_pf_alpha_z,
	e_pf_alpha_Nmin_f,
	e_pf_inc_Nmin_t,
	e_pf_decay,
	e_avq_reftx,
	e_avq_refrx,
	e_avq_vadthd,
	e_avq_offset,
	e_avq_dec_step,
	e_avq_sf_norm,
	e_avq_alpha_fref,
	e_avq_alpha_sref,
	e_avq_alpha_fn,
	e_avq_alpha_sn,
	e_avq_Gmax,
	e_Npsd_bias,
	e_decay_lthd,
	e_alpha_decay,
	e_decay_km,
	e_rise_km,
	e_thd_km,
	e_gain_smooth_thd,
	e_gain_smooth_width,
	e_rse_thd,
	e_vadinc_time,
	e_vaddec_time,
	e_vadNmin,
	e_vadVmin,

    e_bwe_finit = 0x80,
	e_bwe_alpha_npsd,
	e_bwe_alpha_xis ,
	e_bwe_GHmin,
	e_bwe_Psdmin,
	e_bwe_Npsd_bias,
	e_bwe_SNR_s_min,
	e_bwe_vthd,

	e_reset_all = 0xFE,		//      复位全部
};

typedef union _aencbwe_data_type {
    float                  fdata;//float
}aencbwe_data_type;

typedef struct _aencbwe_data {
    uint8_t      page_id;   //e.g. PAGE_MUSIC
    uint8_t      item_type; //参考@TunningTypeClass, 表示调的是什么参数，例如EQ中的某段的Q值
    uint8_t      item_index; //例如:EQ中表示调的是哪一段的参数
    uint8_t      reserve;
    aencbwe_data_type fill;      //参数值
}aencbwe_data;


enum {
    ACMD_S_ERASE_SECTOR = 3,                // 0x20,  erase sector
    ACMD_S_ERASE_HBLK_32KB,          // 0x52,  erase 32KB, half block
    ACMD_S_ERASE_FBLK_64KB,          // 0xD8,  erase 64KB, full block
    ACMD_S_ERASE_CHIP,                    // 0xC7,  erase chip,
};

enum {
    SUB_CMD_DRC_TX_PRM = 0,
    SUB_CMD_PRM = 1,    
};

void nsbwe_set_prm_block(uint8_t* aenc_buff, int16_t length);
void reset_drc_tx_prm();
void reset_aenc_prm_eq();


#endif

