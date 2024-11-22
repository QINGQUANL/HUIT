
/*****************************************************************************
* MIC音效相关代码
****************************************************************************/
#define LOG_LEV 4
#include "amixer.h"
#include "audio_in.h"
#include "audio_out.h"
#include "waudio_track.h"
#include "middleware.h"
#include "amsg.h"
#include "aeffect_component.h"
#include "audio_service.h"
#include "audio_out.h"
#include "volume_control.h"
#include "g24_api.h"
#include "soundcard.h"
#include "user_config.h"
#include "local_play_manager.h"
#include "tone_play_manager.h"
#include "app_psm.h"
#include "psm_manager.h"
#include "sltk_ducker.h"
#include "sltk_aenc_bwe.h"
#include "sltk_nsbwe.h"
#include "audio_dump.h"
#include "eq_loader_common.h"
#include "usb_aenc_bwe.h"

#define MIN(a,b)    ((a) < (b) ? (a) : (b))

#define FRAMESIZE 256
bool load_aencbwe_setting(int8_t eqconfig_id);
int32_t scan_page(uint32_t ptr_start, uint16_t offset_start, uint16_t scan_range, uint32_t *pid);
uint8_t* common_get_eq_controller_buf();

aenc_drcrtprm_t drc_txprm; 
aenc_bwe_rtprm_t aenc_bwe_rtprm;
aenc_rtprm_t aenc_rtprm;

aenc_eqrtprm_t eqtxprm =
{
    0,      //预衰减
    {
        {BIQUAD_PEAKNOTCH, 25, 0, 0.75},
        {BIQUAD_PEAKNOTCH, 75, 0, 0.75},
        {BIQUAD_PEAKNOTCH, 300, 0, 0.75},
        {BIQUAD_PEAKNOTCH, 1000, 0, 0.75},
        {BIQUAD_PEAKNOTCH,3000, 0.0, 0.75},
    }
};

#if USB_AENC_TOOLS_EN
void* aenc_h_bak = NULL;
const aenc_eqrtprm_t eqtxprm_default =
{
    0,      //预衰减
    {
        {BIQUAD_PEAKNOTCH, 25, 0, 0.75},
        {BIQUAD_PEAKNOTCH, 75, 0, 0.75},
        {BIQUAD_PEAKNOTCH, 300, 0, 0.75},
        {BIQUAD_PEAKNOTCH, 1000, 0, 0.75},
        {BIQUAD_PEAKNOTCH,3000, 0.0, 0.75},
    }
};

#if INPUT_SEL_IS_1CH(AUDIO_REC_INPUT_SEL)
#if AUDIO_MIC_AENC
    //对参数进行备份
    aenc_drcrtprm_t drc_txprm_default;  //默认值
    aenc_bwe_rtprm_t aenc_bwe_rtprm_default;    //
    aenc_rtprm_t aenc_rtprm_default;
#elif AUDIO_MIC_NSBWE_AENC
    nsbwe_rtprm_t nsbwe_rtprm_bak;
#endif

#else   //AUDIO_MIC_AENC

#endif

#endif  //USB_AENC_TOOLS_EN

typedef struct {
    void *aenc;
    int16_t *buf;

    void *cpux_handle;
#if AUDIO_MIC_AENC
    int16_t *rx_in[2];
    int16_t *tx_in[2];
    int16_t *rx_out[2];
    int16_t *tx_out[2];
#elif AUDIO_MIC_NSBWE_AENC
    int16_t *inbuf[2];
    int16_t *outbuf[2];
#endif
    uint32_t pcm_total_cnt;

    uint32_t pcm_res_cnt[2];
    uint8_t aenc_toggle;

    uint32_t period;
    uint8_t ch;
} Lmic_aenc_bwe_t;

void Lmic_aenc_bwe_close(void *handle)
{
    Lmic_aenc_bwe_t *hd = handle;

 #if USBD_MODE == USB_MODE_DATA_COLLECTOR
    audio_pcm_dump_close();
#endif

    if (hd) {
#if AUDIO_MIC_AENC
        if (hd->aenc) {
            aenc_bwe_close(hd->aenc);
        }
        if (hd->rx_in[0]) {
            general_free(hd->rx_in[0]);
            hd->rx_in[0] = NULL;
        }
        if (hd->rx_in[1]) {
            general_free(hd->rx_in[1]);
            hd->rx_in[1] = NULL;
        }

        if (hd->tx_in[0]) {
            general_free(hd->tx_in[0]);
            hd->tx_in[0] = NULL;
        }
        if (hd->tx_in[1]) {
            general_free(hd->tx_in[1]);
            hd->tx_in[1] = NULL;
        }

        if (hd->rx_out[0]) {
            general_free(hd->rx_out[0]);
            hd->rx_out[0] = NULL;
        }
        if (hd->rx_out[1]) {
            general_free(hd->rx_out[1]);
            hd->rx_out[1] = NULL;
        }

        if (hd->tx_out[0]) {
            general_free(hd->tx_out[0]);
            hd->tx_out[0] = NULL;
        }
        if (hd->tx_out[1]) {
            general_free(hd->tx_out[1]);
            hd->tx_out[1] = NULL;
        }
#elif AUDIO_MIC_NSBWE_AENC
        if (hd->aenc) {
            nsbwe_close(hd->aenc);
        }
        if (hd->inbuf[0]) {
            general_free(hd->inbuf[0]);
            hd->inbuf[0] = NULL;
        }
        if (hd->inbuf[1]) {
            general_free(hd->inbuf[1]);
            hd->inbuf[1] = NULL;
        }

        if (hd->outbuf[0]) {
            general_free(hd->outbuf[0]);
            hd->outbuf[0] = NULL;
        }
        if (hd->outbuf[1]) {
            general_free(hd->outbuf[1]);
            hd->outbuf[1] = NULL;
        }
#endif
        general_free(hd);
    }
}

AT_SRAM(.Lmic_aenc_proc)
void Lmic_aenc_proc(void *params)
{
#if AUDIO_MIC_AENC
    Lmic_aenc_bwe_t *hd = params;

#if USBD_MODE == USB_MODE_DATA_COLLECTOR
    static int16_t mic0buf[FRAMESIZE];
    static int16_t mic1buf[FRAMESIZE];
    static int16_t rxin_tmp[FRAMESIZE];
    for (int i = 0; i < FRAMESIZE; i++) {
        mic0buf[i] =((int16_t*)hd->tx_in[hd->aenc_toggle])[2*i];
        mic1buf[i] = ((int16_t*)hd->tx_in[hd->aenc_toggle])[2*i+1];
        rxin_tmp[i] = ((int16_t*)hd->rx_in[hd->aenc_toggle])[i];
    }
#endif

#if INPUT_SEL_IS_1CH(AUDIO_REC_INPUT_SEL)
    aenc_bwe_1ch_proc(hd->aenc, (int16_t*)hd->rx_in[hd->aenc_toggle],
        (int16_t*)hd->tx_in[hd->aenc_toggle],
        (int16_t*)hd->rx_out[hd->aenc_toggle],
        (int16_t*)hd->tx_out[hd->aenc_toggle]);
#else
    aenc_bwe_2ch_proc(hd->aenc, (int16_t*)hd->rx_in[hd->aenc_toggle],
        (int16_t*)hd->tx_in[hd->aenc_toggle],
        (int16_t*)hd->rx_out[hd->aenc_toggle],
        (int16_t*)hd->tx_out[hd->aenc_toggle]);
#endif

#if USBD_MODE == USB_MODE_DATA_COLLECTOR
    logx("dump ");
    audio_pcm_dump_deinterleave(rxin_tmp, hd->rx_out[hd->aenc_toggle],
        mic0buf, mic1buf, hd->tx_out[hd->aenc_toggle], NULL);
#endif

#elif AUDIO_MIC_NSBWE_AENC
    Lmic_aenc_bwe_t *hd = params;

    nsbwe_proc(hd->aenc,(int16_t*)hd->inbuf[hd->aenc_toggle],(int16_t*)hd->outbuf[hd->aenc_toggle]);
#endif
}

void *Lmic_aenc_bwe_open(uint32_t fs, uint32_t period)
{
    Lmic_aenc_bwe_t *hd;

    hd = general_calloc(sizeof(*hd));
    if (NULL == hd) {
        return NULL;
    }
#if AUDIO_MIC_AENC
    aenc_bwe_initprm_t bwe_initprm;
    if (FRAMESIZE % period) {
        Lmic_aenc_bwe_close(hd);
        return NULL;
    }
#if INPUT_SEL_IS_1CH(AUDIO_REC_INPUT_SEL)
    hd->ch = 1;
#else
    hd->ch = 2;
#endif

#if USBD_MODE == USB_MODE_DATA_COLLECTOR
    audio_pcm_dump_device_open(AUDIO_DUMP_DEVICE_USB, AUDIO_DUMP_TYPE_48K_6CH_16BIT, FRAMESIZE);
#endif

    hd->pcm_total_cnt = FRAMESIZE / period;
    hd->pcm_res_cnt[0] = 0;
    hd->pcm_res_cnt[1] = hd->pcm_total_cnt;
    hd->aenc_toggle = 0;
    hd->rx_in[0] = general_calloc(FRAMESIZE * sizeof(int16_t));
    hd->rx_in[1] = general_calloc(FRAMESIZE * sizeof(int16_t));
    hd->tx_in[0] = general_calloc(FRAMESIZE * hd->ch * sizeof(int16_t));
    hd->tx_in[1] = general_calloc(FRAMESIZE * hd->ch * sizeof(int16_t));
    hd->rx_out[0] = general_calloc(FRAMESIZE * sizeof(int16_t));
    hd->rx_out[1] = general_calloc(FRAMESIZE * sizeof(int16_t));
    hd->tx_out[0] = general_calloc(FRAMESIZE * sizeof(int16_t));
    hd->tx_out[1] = general_calloc(FRAMESIZE * sizeof(int16_t));

    if (!(hd->rx_in[0] && hd->rx_in[1] && hd->tx_in[0] && hd->tx_in[1] &&
          hd->rx_out[0] && hd->rx_out[1] && hd->tx_out[0] && hd->tx_out[1])) {
        Lmic_aenc_bwe_close(hd);
        return NULL;
    }
    hd->period = period;

#if INPUT_SEL_IS_1CH(AUDIO_REC_INPUT_SEL)
    aenc_drcrtprm_t drc_rxprm;
    aenc_initprm_t init_prm;
    init_prm.fs = fs/2;
    init_prm.N = FRAMESIZE/2;
    init_prm.P = 1;
    init_prm.L = 1;
    init_prm.M = 1;
    init_prm.O = 0;
    bwe_initprm.chnum = 1;
    bwe_initprm.fs = fs;
    bwe_initprm.N = FRAMESIZE;
    bwe_initprm.mode_2ch = 0;
    bwe_initprm.initprm = (void*)&init_prm;
    hd->aenc = aenc_bwe_open(&bwe_initprm);
    if(hd->aenc == NULL){
        loge("open enc controller error!");
        Lmic_aenc_bwe_close(hd);
        return NULL;
    }

    //需使用MASK_TXAGC
    drc_txprm.func_mask = MASK_LIMITER /*| MASK_NOISEGATE*/;
    drc_txprm.lev_mode = 0;
    drc_txprm.look_ahead = 0;
    drc_txprm.lt_thd = -6;
    drc_txprm.lt_ratio = 3;
    drc_txprm.lt_kw = 20;
    drc_txprm.lt_att = 1;
    drc_txprm.lt_rel = 800;
    drc_txprm.ng_thd = -42;
    drc_txprm.ng_ratio = 5;
    drc_txprm.ng_kw = 20;
    drc_txprm.ng_hold = 25;
    drc_txprm.ng_att = 50;
    drc_txprm.ng_rel = 150;
    drc_txprm.peak_att = 1;
    drc_txprm.peak_rel = 30;
    drc_txprm.makeup_gain = 0;

    drc_rxprm.func_mask = MASK_LIMITER | MASK_NOISEGATE;
    drc_rxprm.lev_mode = 0;
    drc_rxprm.look_ahead = 0;
    drc_rxprm.lt_thd = -9;
    drc_rxprm.lt_ratio = 2;
    drc_rxprm.lt_kw = 15;
    drc_rxprm.lt_att = 1;
    drc_rxprm.lt_rel = 800;
    drc_rxprm.ng_thd = -50;
    drc_rxprm.ng_ratio = 5;
    drc_rxprm.ng_kw = 20;
    drc_rxprm.ng_hold = 25;
    drc_rxprm.ng_att = 50;
    drc_rxprm.ng_rel = 150;
    drc_rxprm.peak_att = 1;
    drc_rxprm.peak_rel = 30;
    drc_rxprm.makeup_gain = 0;

//    prms.func_mask = 0x6ff;//0x414;//0x4af;
//    prms.func_mask &= ~(MASK_TXNOT | MASK_AEC | MASK_AES | MASK_TXEQ | MASK_DR | /*MASK_RXAGC |*/ MASK_AVC | MASK_TXAGC /*| MASK_CNG*/);
//    prms.func_mask |= 0;//(/*MASK_TXEQ | MASK_RXEQ*/);
//    //prms.func_mask |= MASK_INTVAD;

    aenc_rtprm.func_mask = MASK_NS | MASK_CNG | MASK_EMP | MASK_GSMOOTH /*| MASK_TXAGC*/;

    //降噪深度可调节pf_GHmin和SNR_s_Min，一般同步调整，值越大降噪深度越小
    aenc_rtprm.finit = 40;
    aenc_rtprm.alpha_slow = (float) 0.3;
    aenc_rtprm.alpha_fast = (float) .5;
    aenc_rtprm.res_overest = (float) 0;
    aenc_rtprm.alpha_fast_resG = (float) 0.5;
    aenc_rtprm.alpha_slow_resG = (float) 0;
    aenc_rtprm.dt_thd_norm = (float) 0.92;//0.92;
    aenc_rtprm.dt_thd_limit = (float) 0.15;
    aenc_rtprm.dt_init_thd = (float) 0.5;
    aenc_rtprm.dt_steprise_decvar = (float) 0.0145;
    aenc_rtprm.dt_stepfall_decvar = (float) 8. / 30000;
    aenc_rtprm.pf_Xi_opt = 9;//(float) 3.1622776601683793;   //pow(10,15./10);
    aenc_rtprm.pf_ph1_thd = (float) .995;
    aenc_rtprm.pf_alpha_npsd = 1 - (float) 0.8;
    aenc_rtprm.pf_alpha_PH1 = 1 - (float) 0.9;
    aenc_rtprm.pf_alpha_Xis = 1 - (float) 0.99;
    aenc_rtprm.pf_alpha_Xins = 1 - (float) 0.95;
    aenc_rtprm.pf_alpha_z = 1 - (float) 0.95;
    aenc_rtprm.pf_alpha_Nmin_f = (float)0.1;
    aenc_rtprm.pf_inc_Nmin_t = (float)0.001;
    aenc_rtprm.pf_GHmin = -20;//(float) 0.1;   //pow(10,-10./10);
    aenc_rtprm.pf_Gcn = -15;
    aenc_rtprm.pf_decay = -8;//(float)1.5848931925e-001;

    aenc_rtprm.avq_reftx = -27;//0.002;//0.0032;//(float)5.0223678130e+000;
    aenc_rtprm.avq_refrx = -18;//0.5;//0.3162;//(float)5.0223678130e+000;
    aenc_rtprm.avq_vadthd = -40;//(float)6.5000000000e-003;
    aenc_rtprm.avq_offset = 9;//(float)2.989735285398626;//(float) 0.996578428466209;   //3;;
    aenc_rtprm.avq_dec_step = 5;//(float)1.3287712380e-002;
    aenc_rtprm.avq_sf_norm = -30;//(float)-1.0034333189e-001;
    aenc_rtprm.avq_alpha_fref      =   (float)0.03;
    aenc_rtprm.avq_alpha_sref      =   (float)0.01;
    aenc_rtprm.avq_alpha_fn        =   (float)0.15;
    aenc_rtprm.avq_alpha_sn        =   (float)0.05;
    aenc_rtprm.avq_Gmax            =   (float) 10;//30dB
    aenc_rtprm.Npsd_bias           =   (float).8;
    aenc_rtprm.decay_lthd          =   (float)-3.2;//0.4786;
    aenc_rtprm.alpha_decay         =   (float)0.4;
    aenc_rtprm.decay_km            =   (float)0.001;
    aenc_rtprm.rise_km             =   (float)0.4;
    aenc_rtprm.thd_km              =   (float)-40;
    aenc_rtprm.gain_smooth_thd     =   (float)0.4;
    aenc_rtprm.gain_smooth_width   =   (float)5;
    aenc_rtprm.SNR_ns_Min          =   (float)-60;
    aenc_rtprm.SNR_s_Min           =   (float)-20;
    aenc_rtprm.psd_Min = -100;
    aenc_rtprm.rse_maxG            = -6;//(float).25;
    aenc_rtprm.rse_thd             = 0;
    aenc_rtprm.ref_thd             = 250;
    aenc_rtprm.dt_overest          = 0;
    aenc_rtprm.init_overest        = 0;
    /* vad prm */
    aenc_rtprm.vadinc_time = 10;
    aenc_rtprm.vaddec_time = 100;
    aenc_rtprm.vadNmin = -45;
    aenc_rtprm.vadVmin = -36;

    load_aencbwe_setting(0);
    
    aenc_bwe_rtprm.rtprm = &aenc_rtprm;
    aenc_bwe_rtprm.hfsnr_sta = 1;
    logi("aenc param set ++++++++");
    if(aenc_bwe_cmd(hd->aenc, SLTK_CMD_PP_AENCBWE_SETRTPRM, (uint32_t)&aenc_bwe_rtprm)){
        loge("set aenc real time prms error!");
        Lmic_aenc_bwe_close(hd);
        return NULL;
    }
    if(aenc_bwe_cmd(hd->aenc, SLTK_CMD_PP_AENCBWE_SETRXAGCRTPRM, (uint32_t)&drc_rxprm)){
        loge("set rx agc real time prms error!");
        Lmic_aenc_bwe_close(hd);
        return NULL;
    }
    if(aenc_bwe_cmd(hd->aenc, SLTK_CMD_PP_AENCBWE_SETTXAGCRTPRM, (uint32_t)&drc_txprm)){
        loge("set tx agc real time prms error!");
        Lmic_aenc_bwe_close(hd);
        return NULL;
    }

    if(aenc_rtprm.func_mask & MASK_TXEQ) {
        aenc_bwe_cmd(hd->aenc, SLTK_CMD_PP_AENCBWE_SETTXEQ, (uint32_t)&eqtxprm);
    }

#if USB_AENC_TOOLS_EN
    memcpy(&drc_txprm_default, &drc_txprm, sizeof(aenc_drcrtprm_t));

    memcpy(&aenc_rtprm_default, &aenc_rtprm, sizeof(aenc_rtprm));     //backup
    memcpy(&aenc_bwe_rtprm_default, &aenc_bwe_rtprm, sizeof(aenc_bwe_rtprm));
    aenc_bwe_rtprm_default.rtprm = &aenc_rtprm_default;
    aenc_h_bak = hd->aenc;
#endif

#else
    aenc_drcrtprm_t drc_txprm;
    admomini_initprm_t init_prm;
    admomini_rtprm_t prms;
    aenc_bwe_rtprm_t bwe_rtprm;
    init_prm.fs = fs/2;
    init_prm.N = FRAMESIZE/2;
    init_prm.R = 4;
    init_prm.d = 16;
    bwe_initprm.fs = fs;
    bwe_initprm.chnum = 2;
    bwe_initprm.N = FRAMESIZE;
    bwe_initprm.mode_2ch = 3;
    bwe_initprm.initprm = &init_prm;
    bwe_initprm.initprm = (void*)&init_prm;
    hd->aenc = aenc_bwe_open(&bwe_initprm);
    if(hd->aenc == NULL){
        loge("open enc controller error!");
        Lmic_aenc_bwe_close(hd);
        return NULL;
    }

    drc_txprm.func_mask = MASK_LIMITER /*| MASK_NOISEGATE*/;
    drc_txprm.lev_mode = 0;
    drc_txprm.look_ahead = 0;
    drc_txprm.lt_thd = -6;
    drc_txprm.lt_ratio = 3;
    drc_txprm.lt_kw = 20;
    drc_txprm.lt_att = 1;
    drc_txprm.lt_rel = 800;
    drc_txprm.ng_thd = -42;
    drc_txprm.ng_ratio = 5;
    drc_txprm.ng_kw = 20;
    drc_txprm.ng_hold = 25;
    drc_txprm.ng_att = 50;
    drc_txprm.ng_rel = 150;
    drc_txprm.peak_att = 1;
    drc_txprm.peak_rel = 30;
    drc_txprm.makeup_gain = 0;

    prms.func_mask = (ADMOMINI_MASK_CAB | ADMOMINI_MASK_ANS /*| ADMOMINI_MASK_WNS */ | ADMOMINI_MASK_EQ
                    | ADMOMINI_MASK_GSM /*| ADMOMINI_MASK_CNG */| ADMOMINI_MASK_AGC | ADMOMINI_MASK_ADP);
    //prms.func_mask &= ~(ADMOMINI_MASK_CNG | ADMOMINI_MASK_AGC);
    //prms.func_mask &= ~(ADMOMINI_MASK_ANS|ADMOMINI_MASK_AGC|ADMOMINI_MASK_CNG);
    prms.finit = 50;

    /* step size for adaptive beamforming filter */
    prms.u  = 0.003;
    /* step size for calibration filter */
    prms.uw = prms.u*init_prm.N;
    /* vad thd in dB */
    prms.vad_thd = 0.5;
    /* wind onset thd */
    prms.wind_thd = -.5;
    /* smoothing prm for reference psd calculation */
    prms.alpha_xpsd = 0.04;
    /* calibration low freq */
    prms.cflow=50;
    /* calibration high freq */
    prms.cfhigh=4000;
    /* wind noise detection freq region low */
    prms.wflow=200;
    /* wind noise detection freq region high */
    prms.wfhigh=1000;
    /* slow varied calibrated weight low pass cutoff freq */
    prms.lpfws=2;
    /* fast varied calibrated weight low pass cutoff freq */
    prms.lpfwf=20;

    prms.pf_Xi_opt = 9;//(float) 3.1622776601683793;   //pow(10,15./10);
    prms.pf_ph1_thd = (float) .99;
    prms.pf_alpha_npsd = 1 - (float) 0.8;
    prms.pf_alpha_PH1 = 1 - (float) 0.9;
    prms.pf_alpha_Xis = 1 - (float) 0.99;

    /* recursive wiener filter prm a */
    prms.Rwa = (float)1.;
    /* recursive wiener filter prm c */
    prms.Rwc = (float).75;
    /* decay time after onset false */
    prms.N_decay = 10;
    /* hold time after onset true */
    prms.N_hold = 2;

    prms.pf_alpha_Nmin_f = (float)0.7;
    prms.pf_inc_Nmin_t = (float)0.001;
    prms.pf_GHmin = -15;//(float) 0.1;   //pow(10,-10./10);
    prms.pf_Gcn = 0;
    prms.Npsd_bias           =   (float).9;
    prms.gain_smooth_thd     =   (float)0.4;
    prms.gain_smooth_width   =   (float)5;
    prms.cab_thd = (float)3;
    prms.SNR_s_Min           =   (float)-15;
    prms.psd_Min = -60;

    /* beam prms */
    prms.bflow = 350;
    prms.bfhigh = 1000;
    prms.alpha_pb = (float).25;
    prms.alpha_decayb = (float)0.996;

    bwe_rtprm.rtprm = &prms;
    if(aenc_bwe_cmd(hd->aenc, SLTK_CMD_PP_AENCBWE_SETRTPRM, (uint32_t)&bwe_rtprm)){
        loge("set aenc real time prms error!");
        Lmic_aenc_bwe_close(hd);
        return NULL;
    }
    if(aenc_bwe_cmd(hd->aenc, SLTK_CMD_PP_AENCBWE_SETTXAGCRTPRM, (uint32_t)&drc_txprm)){
        loge("set tx agc real time prms error!");
        Lmic_aenc_bwe_close(hd);
        return NULL;
    }
#endif
#elif AUDIO_MIC_NSBWE_AENC
    nsbwe_initprm_t initprm;
    nsbwe_rtprm_t bwe_rtprm;
    if (FRAMESIZE % period) {
        Lmic_aenc_bwe_close(hd);
        return NULL;
    }
    initprm.fs = fs;
    initprm.N = FRAMESIZE;
    initprm.R = 4;
    initprm.split = 0;
    hd->aenc = nsbwe_open(&initprm);

    if(hd->aenc == NULL) {
        loge("nsbwe_open fail");
        Lmic_aenc_bwe_close(hd);
    }

    bwe_rtprm.alpha_npsd = 0.005;
    bwe_rtprm.alpha_xis = .05;
    bwe_rtprm.finit = 50;
    bwe_rtprm.func_mask = NSBWE_MASK_NS;
    bwe_rtprm.GHmin = -12;
    bwe_rtprm.SNR_s_min = -25;
    bwe_rtprm.Npsd_bias = 1.0;
    bwe_rtprm.vthd = -55;
    bwe_rtprm.Psdmin = -80;

    if(nsbwe_cmd(hd->aenc, SLTK_CMD_PP_COMMON_SETRTPRM, (uint32_t)&bwe_rtprm)){
        loge("set aenc real time prms error!\n");
        Lmic_aenc_bwe_close(hd);
    }
    hd->ch = 1;

    hd->pcm_total_cnt = FRAMESIZE / period;
    hd->pcm_res_cnt[0] = 0;
    hd->pcm_res_cnt[1] = hd->pcm_total_cnt;
    hd->aenc_toggle = 0;
    hd->inbuf[0] = general_calloc(FRAMESIZE * sizeof(int16_t));
    hd->inbuf[1] = general_calloc(FRAMESIZE * sizeof(int16_t));
    hd->outbuf[0] = general_calloc(FRAMESIZE * sizeof(int16_t));
    hd->outbuf[1] = general_calloc(FRAMESIZE * sizeof(int16_t));
    if (!(hd->inbuf[0] && hd->inbuf[1] && hd->outbuf[0] && hd->outbuf[1])) {
        Lmic_aenc_bwe_close(hd);
        return NULL;
    }
    hd->period = period;

#if USB_AENC_TOOLS_EN
    aenc_h_bak = hd->aenc;
    memcpy(&nsbwe_rtprm_bak, &bwe_rtprm, sizeof(bwe_rtprm));
#endif
    
#endif
    hd->cpux_handle = OAL_cpux_work_create(1, 0, 256);
    if (hd->cpux_handle) {
        OAL_cpux_work_trigger(hd->cpux_handle, Lmic_aenc_proc, hd);
    } else {
        Lmic_aenc_proc(hd);
    }

    return hd;
}


#if USB_AENC_TOOLS_EN

#if AUDIO_MIC_AENC

AT_SRAM(.usbd_aenc)
aenc_drcrtprm_t* aenc_get_drc_tx_prm_tmp()
{
    return &drc_txprm;
}

AT_SRAM(.usbd_aenc)
void update_drc_tx_prm()
{
    logi("%s %x",__func__, aenc_h_bak);
    logi("mask %x, ahead %d, lt_att %d, %d %d %d %d %d %d %d %d %d %d %d %d %d",
        drc_txprm.func_mask, drc_txprm.look_ahead, 
        drc_txprm.lt_att,drc_txprm.lt_kw,drc_txprm.lt_ratio,drc_txprm.lt_rel,drc_txprm.lt_thd,
        drc_txprm.peak_att,drc_txprm.peak_rel,
        drc_txprm.ng_att,drc_txprm.ng_kw,drc_txprm.ng_ratio,drc_txprm.ng_rel,drc_txprm.ng_thd,
        drc_txprm.ng_hold,drc_txprm.makeup_gain);
    
    if(aenc_h_bak) {
        aenc_bwe_cmd(aenc_h_bak, SLTK_CMD_PP_AENCBWE_SETTXAGCRTPRM, (uint32_t)&drc_txprm);
    }
}

AT_SRAM(.usbd_aenc)
void reset_drc_tx_prm()
{
    aenc_bwe_cmd(aenc_h_bak, SLTK_CMD_PP_AENCBWE_SETTXAGCRTPRM, (uint32_t)&drc_txprm_default);
    memcpy(&drc_txprm,&drc_txprm_default,sizeof(drc_txprm_default));
}


AT_SRAM(.usbd_aenc)
aenc_rtprm_t* aenc_get_aenc_prm_tmp()
{
    return &aenc_rtprm;
}


AT_SRAM(.usbd_aenc)
void update_aenc_prm()
{
    logi("%s %x - %x",__func__, aenc_h_bak, &aenc_bwe_rtprm);
    if(aenc_h_bak) {
        logi("aenc_rtprm init %d, func_mask %x, gcn %d",aenc_rtprm.finit, aenc_rtprm.func_mask, (int32_t)aenc_rtprm.pf_Gcn);
        aenc_bwe_rtprm.rtprm = &aenc_rtprm;
        aenc_bwe_cmd(aenc_h_bak, SLTK_CMD_PP_AENCBWE_SETRTPRM, (uint32_t)&aenc_bwe_rtprm);
    }
}

AT_SRAM(.usbd_aenc)
aenc_eqrtprm_t* aenc_get_prm_eq()
{
    return &eqtxprm;
}

AT_SRAM(.usbd_aenc)
void update_aenc_prm_eq()
{
    logi("%s %x",__func__, aenc_h_bak);
    if(aenc_h_bak){
        aenc_bwe_cmd(aenc_h_bak, SLTK_CMD_PP_AENCBWE_SETTXEQ, (uint32_t)&eqtxprm);
    }
}
AT_SRAM(.usbd_aenc)
void reset_aenc_prm_eq()
{
    if(aenc_h_bak){
        aenc_bwe_cmd(aenc_h_bak, SLTK_CMD_PP_AENCBWE_SETTXEQ, (uint32_t)&eqtxprm_default);
    }
    memcpy(&eqtxprm, &eqtxprm_default,sizeof(eqtxprm_default));
    
}

AT_SRAM(.usbd_aenc)
void reset_aenc_prm()
{
    logi("%s %x",__func__, aenc_h_bak);
    if(aenc_h_bak){
        aenc_bwe_rtprm_default.rtprm = &aenc_rtprm_default;
        logi("aenc_rtprm init %d, func_mask %x, gcn %d",aenc_rtprm_default.finit, aenc_rtprm_default.func_mask, (int)aenc_rtprm_default.pf_Gcn);
        aenc_bwe_cmd(aenc_h_bak, SLTK_CMD_PP_AENCBWE_SETRTPRM, (uint32_t)&aenc_bwe_rtprm_default);
    }
    
    memcpy(&aenc_rtprm, &aenc_rtprm_default,sizeof(aenc_rtprm));
}


#elif AUDIO_MIC_NSBWE_AENC
AT_SRAM(.usbd_nsbwe)
nsbwe_rtprm_t* nsbwe_get_rtprm_tmp()
{
    return &nsbwe_rtprm_bak;
}

AT_SRAM(.usbd_nsbwe)
void update_nsbwe_rtprm()
{
    logi("%s - %x",__func__, aenc_h_bak);
    if(aenc_h_bak) {
        nsbwe_cmd(aenc_h_bak, SLTK_CMD_PP_COMMON_SETRTPRM, (uint32_t)&aenc_bwe_rtprm);
    }
}

#endif

#endif

AT_SRAM(.Lmic_aenc_proc)
void Lmic_aenc_bwe_proc(void *handle, void *rxinput, void *txinput,
    void *rxoutput, void *txoutput)
{
#if (AUDIO_MIC_AENC||AUDIO_MIC_NSBWE_AENC)
    Lmic_aenc_bwe_t *hd = handle;

    uint8_t io_toggle = hd->aenc_toggle ^ 1;
    uint32_t idx = hd->pcm_total_cnt - hd->pcm_res_cnt[io_toggle];
    uint32_t period_bytes = hd->period * sizeof(int16_t);
#if AUDIO_MIC_AENC
    if (1 == hd->ch) {
        stereo32_to_mono16(txinput, txinput, hd->period);
        //TODO: rxinput reformat
    } else {
        stereo32_to_stereo16(txinput, txinput, hd->period);
        //TODO: rxinput reformat
    }
    //uint32_t start_t = HAL_timestamp_to_us(HAL_get_timestamp());

    if (hd && hd->aenc) {
        memcpy(hd->rx_in[io_toggle] + idx * hd->period, rxinput, period_bytes);
        memcpy(hd->tx_in[io_toggle] + idx * hd->ch * hd->period, txinput, hd->ch * period_bytes);

        memcpy(rxoutput, hd->rx_out[io_toggle] + idx * hd->period, hd->period * sizeof(int16_t));
        memcpy(txoutput, hd->tx_out[io_toggle] + idx * hd->period, hd->period * sizeof(int16_t));

        hd->pcm_res_cnt[io_toggle]--;
        if (0 == hd->pcm_res_cnt[io_toggle]) {
            //wait aenc end
            if (hd->cpux_handle) {
                OAL_cpux_work_wait_finished(hd->cpux_handle);
            }
            hd->pcm_res_cnt[hd->aenc_toggle] = hd->pcm_total_cnt;

            hd->aenc_toggle = io_toggle;

            if (hd->cpux_handle) {
                OAL_cpux_work_trigger(hd->cpux_handle, Lmic_aenc_proc, hd);
            } else {
                Lmic_aenc_proc(hd);
            }
        }
    }

    //uint32_t end_t = HAL_timestamp_to_us(HAL_get_timestamp());
    //logx("{T:%d}", end_t-start_t);
    if (1 == hd->ch) {
        mono16_to_stereo32(txoutput, txoutput, hd->period);
        //TODO: rxoutput reformat
    } else {
        mono16_to_stereo32(txoutput, txoutput, hd->period);
        //TODO: rxoutput reformat
    }

#elif AUDIO_MIC_NSBWE_AENC
    stereo32_to_mono16(txinput, txinput, hd->period);
    if (hd && hd->aenc) {
        memcpy(hd->inbuf[io_toggle] + idx * hd->ch * hd->period, txinput, hd->ch * period_bytes);
        memcpy(txoutput, hd->outbuf[io_toggle] + idx * hd->period, hd->period * sizeof(int16_t));

        hd->pcm_res_cnt[io_toggle]--;
        if (0 == hd->pcm_res_cnt[io_toggle]) {
            //wait aenc end
            if (hd->cpux_handle) {
                OAL_cpux_work_wait_finished(hd->cpux_handle);
            }
            hd->pcm_res_cnt[hd->aenc_toggle] = hd->pcm_total_cnt;

            hd->aenc_toggle = io_toggle;

            if (hd->cpux_handle) {
                OAL_cpux_work_trigger(hd->cpux_handle, Lmic_aenc_proc, hd);
            } else {
                Lmic_aenc_proc(hd);
            }
        }
    }
    mono16_to_stereo32(txoutput, txoutput, hd->period);
#endif
#endif
}

//////////////////////////////////////////////////////////////////////////////
typedef struct RTPSM_FUNC_MASK_
{
    uint8_t item_type;
    uint32_t mask_bit;
}RtpsmFuncMask;
#define MASK_NUM    14

const RtpsmFuncMask rtpsmFuncMask[MASK_NUM] = {
    {e_aec ,	MASK_AEC},
	{e_aes,  MASK_AES},
	{e_ns,   MASK_NS},
	{e_dr,   MASK_DR},
	{e_avc,  MASK_AVC},
	{e_txagc,MASK_TXAGC},
	{e_rxagc,MASK_RXAGC},
	{e_cng,  MASK_CNG},
	{e_emp,  MASK_EMP},
	{e_txnot,MASK_TXNOT},
	{e_gssmooth,	MASK_GSMOOTH},
	{e_tx_eq,    MASK_TXEQ},
	{e_vad,  MASK_INTVAD},
	{e_smsnr,MASK_SMSNR}
};

void common_setprm(aencbwe_data* p)
{
    switch(p->item_type){
         //基础参数
        case e_finit:
            logi("finit %d",aenc_rtprm.finit);
            aenc_rtprm.finit = (uint8_t)p->fill.fdata;
            break;
        case e_pf_xi_opt:
            logi("pf_Xi_opt %d",(int)p->fill.fdata*1000);
            aenc_rtprm.pf_Xi_opt = p->fill.fdata;
            break;
        case e_pf_ph1_thd:
            logi("pf_ph1_thd %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.pf_ph1_thd = p->fill.fdata;
            break;
        case e_pf_GHmin:
            logi("pf_GHmin %d",(int)p->fill.fdata*1000);
            aenc_rtprm.pf_GHmin = p->fill.fdata;
            break;
        case e_SNR_s_Min:
            logi("SNR_s_Min %d",(int)p->fill.fdata*1000);
            aenc_rtprm.SNR_s_Min = p->fill.fdata;
            break;
        case e_SNR_ns_Min:
            logi("SNR_ns_Min %d",(int)p->fill.fdata*1000);
            aenc_rtprm.SNR_ns_Min = p->fill.fdata;
            break;
        case e_pf_Gcn:
            logi("pf_Gcn %d",(int)p->fill.fdata*1000);
            aenc_rtprm.pf_Gcn = p->fill.fdata;
            break;
        case e_rse_maxG:
            logi("rse_maxG %d",(int)p->fill.fdata*1000);
            aenc_rtprm.rse_maxG = p->fill.fdata;
            break;
        case e_ref_thd:
            logi("ref_thd %d",(int)p->fill.fdata);
            aenc_rtprm.ref_thd = (int32_t)p->fill.fdata;
            break;
        case e_dt_overest:
            logi("dt_overest %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.dt_overest = p->fill.fdata;
            break;
        case e_init_overest:
            logi("init_overest %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.init_overest = p->fill.fdata;
            break;
        //高级参数，很少用到，如果为了节省代码，可以选择去掉
        case e_alpha_slow:
            logi("alpha_slow %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.alpha_slow = p->fill.fdata;
            break;
        case e_alpha_fast:
            logi("alpha_fast %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.alpha_fast = p->fill.fdata;
            break;
        case e_res_overest:
            logi("res_overest %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.res_overest = p->fill.fdata;
            break;
        case e_alpha_fast_resG:
            logi("alpha_fast_resG %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.alpha_fast_resG = p->fill.fdata;
            break;
         case e_alpha_slow_resG:
            logi("alpha_slow_resG %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.alpha_slow_resG = p->fill.fdata;
            break;
        case e_dt_thd_norm:
            logi("dt_thd_norm %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.dt_thd_norm = p->fill.fdata;
            break;
        case e_dt_thd_limit:
            logi("dt_thd_limit %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.dt_thd_limit = p->fill.fdata;
            break;
        case e_dt_init_thd:
            logi("dt_init_thd %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.dt_init_thd = p->fill.fdata;
            break;
        case e_dt_steprise_decvar:
            logi("dt_steprise_decvar %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.dt_steprise_decvar = p->fill.fdata;
            break;
        case e_dt_stepfall_decvar:
            logi("dt_stepfall_decvar %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.dt_stepfall_decvar = p->fill.fdata;
            break;
        case e_pf_alpha_npsd:
            logi("pf_alpha_npsd %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.pf_alpha_npsd = p->fill.fdata;
            break;
        case e_pf_alpha_PH1:
            logi("pf_alpha_PH1 %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.pf_alpha_PH1 = p->fill.fdata;
            break;
        case e_pf_alpha_Xis:
            logi("pf_alpha_Xis %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.pf_alpha_Xis = p->fill.fdata;
            break;
        case e_pf_alpha_Xins:
            logi("pf_alpha_Xins %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.pf_alpha_Xins = p->fill.fdata;
            break;
        case e_pf_alpha_z:
            logi("pf_alpha_z %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.pf_alpha_z = p->fill.fdata;
            break;
        case e_pf_alpha_Nmin_f:
            logi("pf_alpha_Nmin_f %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.pf_alpha_Nmin_f = p->fill.fdata;
            break;
         case e_pf_inc_Nmin_t:
            logi("pf_inc_Nmin_t %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.pf_inc_Nmin_t = p->fill.fdata;
            break;
        case e_pf_decay:
            logi("pf_decay %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.pf_decay = p->fill.fdata;
            break;
        case e_avq_reftx:
            logi("avq_reftx %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.avq_reftx = p->fill.fdata;
            break;
        case e_avq_refrx:
            logi("avq_refrx %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.avq_refrx = p->fill.fdata;
            break;
        case e_avq_vadthd:
            logi("avq_vadthd %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.avq_vadthd = p->fill.fdata;
            break;
        case e_avq_offset:
            logi("avq_offset %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.avq_offset = p->fill.fdata;
            break;
        case e_avq_dec_step:
            logi("avq_dec_step %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.avq_dec_step = p->fill.fdata;
            break;
        case e_avq_sf_norm:
            logi("avq_sf_norm %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.avq_sf_norm = p->fill.fdata;
            break;
        case e_avq_alpha_fref:
            logi("avq_alpha_fref %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.avq_alpha_fref = p->fill.fdata;
            break;
        case e_avq_alpha_sref:
            logi("avq_alpha_sref %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.avq_alpha_sref = p->fill.fdata;
            break;
        case e_avq_alpha_fn:
            logi("avq_alpha_fn %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.avq_alpha_fn = p->fill.fdata;
            break;
        case e_avq_alpha_sn:
            logi("avq_alpha_sn %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.avq_alpha_sn = p->fill.fdata;
            break;
        case e_avq_Gmax:
            logi("avq_Gmax %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.avq_Gmax = p->fill.fdata;
            break;
        case e_Npsd_bias:
            logi("Npsd_bias %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.Npsd_bias = p->fill.fdata;
            break;
        case e_decay_lthd:
            logi("decay_lthd %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.decay_lthd = p->fill.fdata;
            break;
        case e_alpha_decay:
            logi("alpha_decay %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.alpha_decay = p->fill.fdata;
            break;
        case e_decay_km:
            logi("decay_km %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.decay_km = p->fill.fdata;
            break;
        case e_rise_km:
            logi("rise_km %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.rise_km = p->fill.fdata;
            break;
        case e_thd_km:
            logi("thd_km %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.thd_km= p->fill.fdata;
            break;
        case e_gain_smooth_thd:
            logi("gain_smooth_thd %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.gain_smooth_thd= p->fill.fdata;
            break;
        case e_gain_smooth_width:
            logi("gain_smooth_width %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.gain_smooth_width= p->fill.fdata;
            break;
        case e_rse_thd:
            logi("rse_thd %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.rse_thd= (int32_t)p->fill.fdata;
            break;
        case e_vadinc_time:
            logi("vadinc_time %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.vadinc_time= (int16_t)p->fill.fdata;
            break;
        case e_vaddec_time:
            logi("vaddec_time %d",(int)(p->fill.fdata*1000));
            aenc_rtprm.vaddec_time= (int16_t)p->fill.fdata;
            break;
        case e_vadNmin:
            logi("vadNmin %d",(int16_t)p->fill.fdata);
            aenc_rtprm.vadNmin= (int16_t)p->fill.fdata;
            break;
        case e_vadVmin:
            logi("vadVmin %d",(int16_t)p->fill.fdata);
            aenc_rtprm.vadVmin= (int16_t)p->fill.fdata;
            break;
    }
}

void common_set_aencbwe_data(uint8_t *buf, uint32_t len)
{
    logi("common_set_aencbwe_data %d", len);
    aencbwe_data* p_aencdata ;
    for(int i=0; i< len; i+=8) {
        p_aencdata = (aencbwe_data*)(buf + i);
        if(p_aencdata->item_type >= e_aec && p_aencdata->item_type <= e_smsnr) {
            //func mask
            for(uint8_t i=0; i< MASK_NUM; i++) {
                if(p_aencdata->item_type == rtpsmFuncMask[i].item_type) {
                    aenc_rtprm.func_mask &= ~rtpsmFuncMask[i].mask_bit;
                    if((bool)p_aencdata->fill.fdata) {
                        aenc_rtprm.func_mask |= rtpsmFuncMask[i].mask_bit;
                    }
                    
                    logi("mask %x,  %x",aenc_rtprm.func_mask, rtpsmFuncMask[i].mask_bit );
                    break;
                }
            }
        } else if(p_aencdata->item_type >= e_eq_fre && p_aencdata->item_type <= e_eq_predecay) {
            if(p_aencdata->item_index > 4){
                continue;   //非法的EQ索引
             }
            switch(p_aencdata->item_type) {
                case e_eq_fre:
                    logi("eq fre %d, %d",p_aencdata->item_index, (uint16_t)p_aencdata->fill.fdata);
                    eqtxprm.setting[p_aencdata->item_index].fc = (uint16_t)p_aencdata->fill.fdata;
                    break;
                case e_eq_q:
                    logi("eq q %d, %d",p_aencdata->item_index, (int16_t)(p_aencdata->fill.fdata*1000));
                    eqtxprm.setting[p_aencdata->item_index].Q_factor = p_aencdata->fill.fdata;
                    break;
                case e_eq_gain:
                    logi("eq g %d, %d",p_aencdata->item_index, (int16_t)(p_aencdata->fill.fdata*1000));
                    eqtxprm.setting[p_aencdata->item_index].G = p_aencdata->fill.fdata;
                    break;
                case e_eq_type:
                    logi("eq filter %d, %d",p_aencdata->item_index, (int16_t)p_aencdata->fill.fdata);
                    eqtxprm.setting[p_aencdata->item_index].filter_type = (int16_t)p_aencdata->fill.fdata;
                    break;
                case e_eq_enable:
                    logi("eq enable %d, %d",p_aencdata->item_index, (uint16_t)(p_aencdata->fill.fdata*1000));
                    if((bool)p_aencdata->fill.fdata) {
                        eqtxprm.setting[p_aencdata->item_index].G = 0;
                        eqtxprm.setting[p_aencdata->item_index].filter_type = 1;//balance
                    }
                    break;
                case e_eq_predecay:
                    logi("eq amp %d, %d",p_aencdata->item_index, (uint16_t)(p_aencdata->fill.fdata*1000));
                    eqtxprm.PreAmp = p_aencdata->fill.fdata;
                    break;
            }
            //EQ
        } else if(p_aencdata->item_type >= e_drc_switch && p_aencdata->item_type <= e_makeupgain) {
            //DRC
            switch(p_aencdata->item_type) {
                case e_drc_switch:
                    logi("drc switch %d %d",p_aencdata->item_index, (bool)p_aencdata->fill.fdata);
                    if(p_aencdata->item_index > 3) {
                        logi("mask error +++++++");
                     }
                    drc_txprm.func_mask &= ~(1 << p_aencdata->item_index);
                    if(p_aencdata->fill.fdata) {
                        drc_txprm.func_mask |= (1 << p_aencdata->item_index);
                    }
                    logi("drc_txprm.func_mask %x",drc_txprm.func_mask);
                    break;
                case e_drc_thd:
                    if(p_aencdata->item_index == 0) {
                        drc_txprm.lt_thd = (int8_t)p_aencdata->fill.fdata;
                        logi("lt_thd %d", drc_txprm.lt_thd);
                    } else if(p_aencdata->item_index == 3){
                        drc_txprm.ng_thd = (int8_t)p_aencdata->fill.fdata;
                        logi("ng_thd %d", drc_txprm.ng_thd);
                    }
                    
                    break;
                case e_drc_att:
                    if(p_aencdata->item_index == 0) {
                        drc_txprm.lt_att = (uint16_t)p_aencdata->fill.fdata;
                        logi("lt_att %d", drc_txprm.lt_att);
                    } else if(p_aencdata->item_index == 3){
                        drc_txprm.ng_att = (uint16_t)p_aencdata->fill.fdata;
                        logi("ng_att %d", drc_txprm.ng_att);
                    }
                    break;
                case e_drc_ratio:
                    if(p_aencdata->item_index == 0) {
                        drc_txprm.lt_ratio = (uint8_t)p_aencdata->fill.fdata;
                        logi("lt_ratio %d", drc_txprm.lt_ratio);
                    } else if(p_aencdata->item_index == 3){
                        drc_txprm.ng_ratio = (uint8_t)p_aencdata->fill.fdata;
                        logi("ng_ratio %d", drc_txprm.ng_ratio);
                    }
                    break;
                case e_drc_rel:
                    if(p_aencdata->item_index == 0) {
                        drc_txprm.lt_rel = (uint16_t)p_aencdata->fill.fdata;
                        logi("lt_rel %d", drc_txprm.lt_rel);
                    } else if(p_aencdata->item_index == 3){
                        drc_txprm.ng_rel = (uint16_t)p_aencdata->fill.fdata;
                        logi("ng_rel %d", drc_txprm.ng_rel);
                    }
                    break;
                case e_drc_hold:
                    if(p_aencdata->item_index == 0) {
                       // logi("lt_rel %d", drc_txprm.ng_rel);
                    } else if(p_aencdata->item_index == 3){
                        drc_txprm.ng_hold = (uint16_t)p_aencdata->fill.fdata;
                        logi("ng_hold %d", drc_txprm.ng_hold);
                    }
                    break;
                case e_drc_kw:
                    if(p_aencdata->item_index == 0) {
                        drc_txprm.lt_kw = (uint8_t)p_aencdata->fill.fdata;
                        logi("lt_kw %d", drc_txprm.lt_kw);
                    } else if(p_aencdata->item_index == 3){
                        drc_txprm.ng_kw = (uint8_t)p_aencdata->fill.fdata;
                        logi("ng_kw %d", drc_txprm.ng_kw);
                    }
                    break;
                case e_makeupgain:
                    drc_txprm.makeup_gain = (int8_t)p_aencdata->fill.fdata;
                    logi("makeup_gain %d", drc_txprm.makeup_gain);
                    break;
            }
        } else if(p_aencdata->item_type >= e_finit && p_aencdata->item_type <= e_vadVmin) {
            //aenc_rt_prm
            common_setprm(p_aencdata);
        } else if(p_aencdata->item_type >= e_bwe_finit && p_aencdata->item_type <= e_bwe_vthd) {
            //aencbwe,  not support at this timepoint
        } else {
            //unkown
        }
    }
}

bool load_aencbwe_setting(int8_t eqconfig_id)
{
    EQ_HEADER eq_hdr = {0};
    uint16_t buffer_offset = 0;
    uint8_t *eq_buf;
    uint32_t file_len = 0;
    //获取EQ2文件的地址
    uint32_t file_start = sys_flash_layout_address_get_by_tag("AENC", &file_len);
    file_start &= 0xffffff;
    logi("aenc addr : %x", file_start);
    //读取EQ2文件头
    HAL_norflash_read((uint32_t)&eq_hdr.eq_fhdr, file_start + buffer_offset, sizeof(EQ_FIXED_HEADER));
    buffer_offset += sizeof(EQ_FIXED_HEADER);
    eq_hdr.eq_hdr_len += sizeof(EQ_FIXED_HEADER);

    if (eqconfig_id >= eq_hdr.eq_fhdr.eq_num) {
        return false;
    }

    //variable header, at least have one eq profile
    if(eq_hdr.eq_fhdr.eq_num){
        eq_hdr.eq_lens = file_start + buffer_offset;
        eq_hdr.eq_hdr_len += sizeof(short)*eq_hdr.eq_fhdr.eq_num;
        buffer_offset += sizeof(short)*eq_hdr.eq_fhdr.eq_num;
    }
    /*
        这里就不检查crc了
    */
    //crc header
    HAL_norflash_read((uint32_t)&eq_hdr.crc_hdr, file_start + buffer_offset, sizeof(short));
    eq_hdr.eq_hdr_len += 2;
    buffer_offset += 2;
    //crc value
    HAL_norflash_read((uint32_t)&eq_hdr.crc_val, file_start + buffer_offset, sizeof(short));
    eq_hdr.eq_hdr_len += 2;
    buffer_offset += 2;

    if(eq_hdr.eq_fhdr.magic[0] != 'A' || eq_hdr.eq_fhdr.magic[1] != 'E' ||
        eq_hdr.eq_fhdr.magic[2] != 'N' || eq_hdr.eq_fhdr.magic[3] != 'C'){
        logi("magic wrong!");
        return false;
    }

    buffer_offset = eq_hdr.eq_fhdr.first_eq_off;

    //可能有多个EQ配置，遍历找到对应的配置
    for(int i = 0; i < eq_hdr.eq_fhdr.eq_num; i++) {
        uint16_t block_len = 0;
        //获取配置参数长度
        HAL_norflash_read((uint32_t)&block_len, eq_hdr.eq_lens + i*sizeof(short), 2);
        //logd("id:%d, block_len:%d", i, block_len);

        if(eqconfig_id == i){
            uint32_t pid = PAGE_NONE;
            int once_process_block_len = 0;
            uint16_t page_blck_size = 0;

            //总的数据长度可能比较大，分步设置
            while(block_len){
                //每一步最大设置MAX_BLOCK_PER_PAGE个block
                once_process_block_len = block_len > MAX_BLOCK_PER_PAGE?MAX_BLOCK_PER_PAGE:block_len;
                block_len -= once_process_block_len;
                while(once_process_block_len){
                    //遍历所有数据
                    page_blck_size = scan_page(file_start, buffer_offset, once_process_block_len, &pid);
                    //同一个eq_config_id会有多个page,匹配的才往下设置
                    //logd("pid:%d, page_blck_size:%d", pid, page_blck_size);
                    if(pid < 255){
                        //获取参数缓冲buffer
                        eq_buf = common_get_eq_controller_buf();
                        uint32_t total_read_sz = page_blck_size;
                        while (total_read_sz) {
                            uint32_t read_sz = MIN(MAX_BLOCK_PER_PAGE, total_read_sz);
                            //从flash读出EQ参数
                            HAL_norflash_read((uint32_t)eq_buf, file_start + buffer_offset, read_sz*BLOCK_SIZE);
                            //解析参数并往算法模块设置
                            common_set_aencbwe_data(eq_buf, read_sz*BLOCK_SIZE);
                            buffer_offset += read_sz*BLOCK_SIZE;
                            total_read_sz -= read_sz;
                        }
                    } else {
                        buffer_offset += page_blck_size*BLOCK_SIZE;
                    }

                    once_process_block_len -= page_blck_size;
                }
            }
            break;
        } else {
            //跳过前面的配置长度
            buffer_offset += block_len * BLOCK_SIZE;
        }
    }

    return true;
}

