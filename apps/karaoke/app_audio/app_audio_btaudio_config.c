
#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "audio_service.h"
#include "sltk_audio.h"
#include "info_mem.h"
#include "bt_audio_service.h"
#include "audio_params_config.h"
#include "bt_audio_manager.h"
#include "psm_manager.h"
#include "tws_profile.h"
#include "sltk_nslite.h"
#include "bt_audio_service.h"
#include "app_pmu.h"
#include "audio_heap.h"
#include "audio_dump.h"
#if MODE_BT_EN

#if AUDIO_THIRD_PARTY_AENC
#include "third_party_api.h"

#define AUDIO_THIRD_PARTY_AENC_DUMP_ENABLE   0 //抓数据功能

static uint8_t g_mic_ch_count;

#if AUDIO_THIRD_PARTY_AENC_DUMP_ENABLE
static int16_t *mic_ch0_in;
static int16_t *mic_ch1_in;
#endif

/*------------------------------------------------------------------------------
* 函数功能: 通话算法初始化回调函数
* 函数名称: void on_third_party_aenc_open(uint32_t fs, uint8_t mic_ch_count, uint32_t proc_samples)
* 参    数:
*           fs: 采样率(具体值由手机决定，8000或者16000)
*           mic_ch_count: mic数量（由方案配置1或者2）
*           proc_samples: 一帧处理的点数(一般64或者128)
* 返 回 值: 无
* 说明 : 此函数由音频中间件自动调用, 应在此处实现算法的初始化(包括ram资源分配,参数初始化等)
*-------------------------------------------------------------------------------*/
void on_third_party_aenc_open(uint32_t fs, uint8_t mic_ch_count, uint32_t proc_samples)
{
    logi("on_third_party_aenc_open: fs:%d, mic_ch_count:%d, proc_samples:%d", fs, mic_ch_count, proc_samples);

    g_mic_ch_count = mic_ch_count;

#if AUDIO_THIRD_PARTY_AENC_DUMP_ENABLE
    if (2 == mic_ch_count) {
        if (NULL == mic_ch0_in) {
            mic_ch0_in = general_malloc(proc_samples * sizeof(int16_t));
        }
        if (NULL == mic_ch1_in) {
            mic_ch1_in = general_malloc(proc_samples * sizeof(int16_t));
        }
    }
    audio_pcm_dump_open((16000 == fs) ? AUDIO_DUMP_TYPE_16K_6CH_16BIT :
        AUDIO_DUMP_TYPE_8K_6CH_16BIT, proc_samples);
    pmu_vbus_spdif_set(1);
#endif
}

/*------------------------------------------------------------------------------
* 函数功能: 通话算法退出回调函数
* 函数名称: void on_third_party_aenc_close(void)
* 参    数:无
* 返 回 值: 无
* 说明 : 此函数由音频中间件自动调用, 应在此处实现算法的退出（包括ram资源释放等）
*-------------------------------------------------------------------------------*/
void on_third_party_aenc_close(void)
{
    logi("on_third_party_aenc_close");

#if AUDIO_THIRD_PARTY_AENC_DUMP_ENABLE
    audio_pcm_dump_close();
    pmu_vbus_spdif_set(0);
    if (mic_ch0_in) {
        general_free(mic_ch0_in);
        mic_ch0_in = NULL;
    }
    if (mic_ch1_in) {
        general_free(mic_ch1_in);
        mic_ch1_in = NULL;
    }
#endif
}

/*------------------------------------------------------------------------------
* 函数功能: 通话算法处理回调函数
* 函数名称: void on_third_party_aenc_proc(int16_t *spk_in, int16_t *mic_in, int16_t *spk_out, int16_t *mic_out)
* 参    数:
*           spk_in:   speaker的原始数据buffer
*           mic_in:   MIC的原始数据buffer
*           spk_out:  speaker算法处理后的数据buffer
*           mic_out:  mic算法处理后的数据buffer
*           proc_samples:  处理点数(恒等于on_third_party_aenc_open传入的proc_samples参数)
* 返 回 值: 无
* 说明 : 此函数由音频中间件自动调用, 应在此处实现算法的处理
*-------------------------------------------------------------------------------*/
void on_third_party_aenc_proc(int16_t *spk_in, int16_t *mic_in,
    int16_t *spk_out, int16_t *mic_out, int32_t proc_samples)
{
    //此处是算法不做任何处理的例子
    memcpy(spk_out, spk_in, proc_samples * sizeof(int16_t));
    if (1 == g_mic_ch_count) {
        memcpy(mic_out, mic_in, proc_samples * sizeof(int16_t));
    } else {
        for (int i = 0; i < proc_samples; i++) {
            mic_out[i] = mic_in[2*i+0];
            //mic_out[i] = mic_in[2*i+1];
        }
    }

#if AUDIO_THIRD_PARTY_AENC_DUMP_ENABLE
    if (2 == g_mic_ch_count) {
        for (int i = 0; i < proc_samples; i++) {
            mic_ch0_in[i] = mic_in[2*i+0];
            mic_ch1_in[i] = mic_in[2*i+1];
        }
        audio_pcm_dump_deinterleave(spk_in, spk_out, mic_ch0_in, mic_ch1_in, mic_out, NULL);
    } else if (1 == g_mic_ch_count) {
        audio_pcm_dump_deinterleave(spk_in, spk_out, mic_in, mic_out, NULL, NULL);
    }
#endif
}
#endif

/* TX EQ 参数 */
const phone_aenc_eq_params_t aenc_txeqprm =
{
#ifdef CFG_AENC_TXEQPRM
    //来自audio_params_config.h
    CFG_AENC_TXEQPRM
#else
    //默认配置
    0,
    {
        {PHONE_EQ_BIQUAD_LOWSHELVING, 450, -3, 0.707},
        {PHONE_EQ_BIQUAD_HIGHSHELVING, 300, -3, 0.707},
        {0,},
        {0,},
        {0,},
    }
#endif
};
/* TX AGC 参数 */
const phone_aenc_agc_params_t aenc_txagcprm =
{
#ifdef CFG_AENC_TXAGCPRM
    CFG_AENC_TXAGCPRM
#else
    .func_mask = PHONE_AGC_MASK_LIMITER | PHONE_AGC_MASK_NOISEGATE,
    .lev_mode = 0,
    .look_ahead = 0,
    .lt_thd = -24,
    .lt_ratio = 3,
    .lt_kw = 20,
    .lt_att = 1,
    .lt_rel = 800,
    .ng_thd = -60,
    .ng_ratio = 5,
    .ng_kw = 20,
    .ng_hold = 25,
    .ng_att = 50,
    .ng_rel = 150,
    .peak_att = 1,
    .peak_rel = 30,
    .makeup_gain = 12,
#endif
};
/* RX EQ 参数 */
const phone_aenc_eq_params_t aenc_rxeqprm =
{
#ifdef CFG_AENC_RXEQPRM
    CFG_AENC_RXEQPRM
#else
    0,
    {
        {PHONE_EQ_BIQUAD_LOWSHELVING, 450,  3, 0.707},
        {PHONE_EQ_BIQUAD_PEAKNOTCH, 900,  -0.5, 1.0},
        {PHONE_EQ_BIQUAD_PEAKNOTCH, 2650, 0.3, 2.0},
        {PHONE_EQ_BIQUAD_PEAKNOTCH, 3400,  -5,  2.7},
        {PHONE_EQ_BIQUAD_LOWSHELVING, 200,  3,  0.707},
    }
#endif
};
/* RX AGC 参数 */
const phone_aenc_agc_params_t aenc_rxagcprm =
{
#ifdef CFG_AENC_RXAGCPRM
    CFG_AENC_RXAGCPRM
#else
    .func_mask = PHONE_AGC_MASK_LIMITER | PHONE_AGC_MASK_NOISEGATE,
    .lev_mode = 0,
    .look_ahead = 0,
    .lt_thd = -9,
    .lt_ratio = 2,
    .lt_kw = 15,
    .lt_att = 1,
    .lt_rel = 800,
    .ng_thd = -50,
    .ng_ratio = 5,
    .ng_kw = 20,
    .ng_hold = 25,
    .ng_att = 50,
    .ng_rel = 150,
    .peak_att = 1,
    .peak_rel = 30,
    .makeup_gain = 0,
#endif
};
/* 基本通话降噪算法参数 */
const phone_aenc_config_params_t aenc_config_params = {
#ifdef CFG_AENC_CONFIG_PARAMS
    CFG_AENC_CONFIG_PARAMS
#else
    .func_mask = (/*PHONE_AEC |*/ PHONE_AES | PHONE_NS | PHONE_TXAGC | PHONE_RXAGC |
                  PHONE_CNG | PHONE_GSMOOTH /*| PHONE_TXEQ | PHONE_RXEQ*/),
    .rse_thd        =  0,           /* 0 ~ 10 */
    .ref_thd        = 250,          /* 0 ~ 255 */
    .alpha_slow     = (float)0.3,   /* 0 ~ 1 */
    .alpha_fast     = (float)0.5,   /* 0 ~ 1 */
    .pf_GHmin       = (float)-20,   /* -30 ~ 0 (dB) */

    .res_overest    = (float)0,     /* -10 ~ 10 (dB) */
    .rse_maxG       = (float)-18,   /* -30 ~ 30 (dB) */
    .dt_thd_norm    = (float)0.92,  /* 0 ~ 1 */

    .pf_Xi_opt      = (float)9,     /* 0 ~ 20 */
    .Npsd_bias      = (float).9,    /* 0 ~ 2 */

    .pf_Gcn         = (float)-15,   /* -30 ~ 0 (dB) */

    .avq_reftx      = (float)-27,
    .avq_refrx      = (float)-18,
    .avq_vadthd     = (float)-40,   /* -60 ~ 0 (dBFs) */
    .avq_offset     = (float)9,     /* 0 ~ 10 (dB) */
    .avq_Gmax       = (float)10,    /* 0 ~ 15 (dB) */
#endif
};

/* 其他通话降噪算法参数 */
const phone_aenc_params_t aenc_params = {
#ifdef CFG_AENC_PARAMS
    CFG_AENC_PARAMS
#else
    .gain_smooth_thd    = (float)0.4,
    .gain_smooth_width  = (float)5,
    .SNR_s_Min          = (float)-20,
    .SNR_ns_Min         = (float)-60,
    .psd_Min            = (float)-100,
    .pf_alpha_Xis       = 1-(float)0.99,
    .pf_alpha_Xins      = 1-(float)0.95,

    .alpha_fast_resG    = (float)0.5,
    .alpha_slow_resG    = (float)0.0,
    .dt_thd_limit       = (float)0.15,
    .dt_init_thd        = (float)0.5,
    .dt_steprise_decvar = (float)0.0145,
    .dt_stepfall_decvar = (float)8./30000,

    .finit              = (float)40,
    .pf_ph1_thd         = (float).995,
    .pf_alpha_npsd      = 1-(float)0.8,
    .pf_alpha_PH1       = 1-(float)0.9,

    .pf_alpha_Nmin_f    = (float)0.1,
    .pf_inc_Nmin_t      = (float)0.001,

    .pf_alpha_z         = 1-(float)0.95,
    .pf_decay           = (float)-8,
    .decay_lthd         = (float)-3.2,
    .alpha_decay        = (float)0.4,
    .decay_km           = (float)0.001,
    .rise_km            = (float)0.4,
    .thd_km             = (float)-40,

    .avq_dec_step       = (float)5,
    .avq_sf_norm        = (float)-30,
    .avq_alpha_fref     = (float)0.03,
    .avq_alpha_sref     = (float)0.01,
    .avq_alpha_fn       = (float)0.15,
    .avq_alpha_sn       = (float)0.05,
    .dt_overest         = (float)0,
    .init_overest       = (float)0,
#endif
};

/* 基本双麦通话降噪算法参数 */
const phone_admaenc_config_params_t admaenc_config_params = {
#ifdef CFG_ADMAENC_CONFIG_PARAMS
    CFG_ADMAENC_CONFIG_PARAMS
#else
    .func_mask = (/*PHONE_AEC |*/ PHONE_AES | PHONE_NS | PHONE_TXAGC | PHONE_RXAGC | PHONE_TXEQ | PHONE_RXEQ
                  | PHONE_CNG | PHONE_GSMOOTH /*| PHONE_DMCAB*/ | PHONE_DMANS | PHONE_DMWNS | PHONE_DMADP),
    .rse_thd        =  0,           /* 0 ~ 10 */
    .ref_thd        = 125,          /* 0 ~ 255 */
    .alpha_slow     = (float)0.3,   /* 0 ~ 1 */
    .alpha_fast     = (float)0.5,   /* 0 ~ 1 */
    .pf_GHmin       = (float)-30,   /* -30 ~ 0 (dB) */

    .res_overest    = (float)0,     /* -10 ~ 10 (dB) */
    .rse_maxG       = (float)-6,   /* -30 ~ 30 (dB) */
    .dt_thd_norm    = (float)0.92,  /* 0 ~ 1 */

    .pf_Xi_opt      = (float)9,     /* 0 ~ 20 */
    .Npsd_bias      = (float).9,    /* 0 ~ 2 */

    .pf_Gcn         = (float)-25,   /* -30 ~ 0 (dB) */
    .cab_thd        = (float)3,     /*calibration threshold */
#endif

    .d = AUDIO_ADMAENC_MIC_DISTANCE,     //两个MIC的距离(mm)
    .theta = AUDIO_ADMAENC_MIC_ANGLE, //双MIC连线和嘴的角度
    .mode = AUDIO_ADMAENC_MIC_MODE,   //0:序号小的MIC靠近嘴(如:使用MIC0，MIC2,则序号小的MIC为MIC0; 使用MIC1，MIC2,则序号小的MIC为MIC1)； 1：序号大的MIC靠近嘴
};

const phone_admaenc_params_t admaenc_params =
{
#ifdef CFG_ADMAENC_PARAMS
    CFG_ADMAENC_PARAMS
#else
   /* initial ambient noise recursive estimation duration in frames */
    .finit          = 50,
    /* prms for calibration */
    /* step size for calibration filter */
    .uw             =   (float)0.003,
    /* step size for adaptive beamforming filter */
    .u              =   (float)0.003,
    /* vad thd in dB */
    .vad_thd        =   (float)0,
    /* wind onset thd */
    .wind_thd       =   (float)-0.5,
    /* smoothing prm for reference psd calculation */
    .alpha_xpsd     =   (float)0.04,
    /* calibration low freq */
    .cflow          =   50,
    /* calibration high freq */
    .cfhigh         =   4000,
   /* wind noise detection freq region low */
    .wflow          =   200,
    /* wind noise detection freq region high */
    .wfhigh         =   1000,
    /* slow varied calibrated weight low pass cutoff freq */
    .lpfws          =   2,
    /* fast varied calibrated weight low pass cutoff freq */
    .lpfwf          =   20,
    /* prms for ANS */

    /* maximum of allowable SPP */
    .pf_ph1_thd     =   (float)0.995,
    /* ambient noise psd smoothing prm*/
    .pf_alpha_npsd  =   1 - (float)0.8,
    /* SPP smoothing prm */
    .pf_alpha_PH1   =   1 - (float)0.9,
    /* SNR smoothing prm */
    .pf_alpha_Xis   =   1 - (float)0.99,

    /* minimum SNR for signal to ambient noise */
    .SNR_s_Min      =   (float)-30,
    /* allowable mimum psd */
    .psd_Min        =   (float)-100,


    /* Wind Noise prms */
    /* recursive wiener filter prm a */
    .Rwa            =   (float)1.0,
    /* recursive wiener filter prm c */
    .Rwc            =   (float)0.75,


     /* decay time after onset false */
    .N_decay        =   10,
    /* hold time after onset true */
    .N_hold         =   2,

    /* CNG module */
    /* CNG smoothing prm for frequency domain */
    .pf_alpha_Nmin_f    =   (float)0.7,
    /* CNG increment step */
    .pf_inc_Nmin_t      =   (float)0.001,


    /*gain smoothing module */
    /* up limit for snr to use gain smoothing */
    .gain_smooth_thd    =   (float)0.1,
    /* gain smoothing width in frequency bin num */
    .gain_smooth_width  =   5,
    .alpha_fast_resG    =   (float)0.5,
    .alpha_slow_resG    =   (float)0,

    .dt_thd_limit       =   (float)0.15,
    .dt_init_thd        =   (float)0.5,
    .dt_steprise_decvar =   (float)0.0145,
    .dt_stepfall_decvar =   (float)8./30000,
    /* non-stationary SNR low bound */
    .SNR_ns_Min         =   (float)-60,
    .alpha_Xins         =   1-(float)0.95,
    .dt_overest         =   (float)0,
    .init_overest       =   (float)0,
    .vadinc_time        =   10,
    .vaddec_time        =   100,
    .vadNmin            =   -80,
    .vadVmin            =   -50,
#endif
};
const phone_admcoh_params_t admcoh_config_params ={
#ifdef CFG_ADMCOH_COFNIG_PARAMS
    CFG_ADMCOH_COFNIG_PARAMS
#else
    .enable = 0,
    .cutfreq = 8000,
    .snrmin = -40,
    .txnotch_enable = 1,
    .startfreq = 0,
#endif
};
const phone_nslite_params_t nslite_config_params = {
#ifdef CFG_NSLITE_COFNIG_PARAMS
    CFG_NSLITE_COFNIG_PARAMS
#else
    .R                  = 2,
    .func_mask          = (NSLITE_MASK_ANS  | NSLITE_MASK_GSM  | NSLITE_MASK_CNG),
    .finit              = 10,
    .pf_Xi_opt          = 15,
    .pf_ph1_thd         = (float).99,
    .pf_alpha_npsd      = 1 - (float)0.8,
    .pf_alpha_PH1       = 1 - (float)0.9,
    .pf_alpha_Xis       = 1 - (float)0.99,
    .pf_alpha_Nmin_f    = (float)0.3,
    .pf_inc_Nmin_t      = (float)0.001,
    .pf_GHmin           = -15,
    .pf_Gcn             = -10,
    .Npsd_bias          = (float).9,
    .gain_smooth_thd    = (float)0.4,
    .gain_smooth_width  = (float)5,
    .SNR_s_Min          = (float)-20,
    .psd_Min            = -80,
#endif
};

#define BTMUSIC_VOL_MAP_SIZE        17
#define BTMUSIC_VOL_AIR_MAP_SIZE    17
#define CALL_VOL_MAP_SIZE           17

//音乐音量映射曲线
const vol_map_t btmusic_vol_map_list[BTMUSIC_VOL_MAP_SIZE] = {
#ifdef BTMUSIC_VOL_MAP_CFG
    BTMUSIC_VOL_MAP_CFG
#else
    {100, 0 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},       //127
    {90, -3 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},       //119
    {85, -6 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},       //111
    {80, -9 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},       //103
    {75, -13 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      //95
    {65, -17 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      //87
    {60, -21 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      //79
    {55, -25 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      //71
    {50, -29 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      //63
    {40, -33 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      //55
    {35, -37.5 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},    //47
    {30, -42 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      //39
    {25, -47 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      //31
    {15, -52 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      //23
    {10, -59 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},      //15
    {5, -66 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},       //7
    {0, -73 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},       //0
#endif
};

const vol_map_t btmusic_vol_air_map_list[BTMUSIC_VOL_AIR_MAP_SIZE] = {
    {100, 0 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},       //127
    {90, 0 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},        //119
    {85, 0 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},        //111
    {80, 0 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},        //103
    {75, 0 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},        //95
    {65, 0 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},        //87
    {60, 0 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},        //79
    {55, 0 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},        //71
    {50, 0 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},        //63
    {40, 0 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},        //55
    {35, 0 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},        //47
    {30, 0 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},        //39
    {25, 0 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},        //31
    {15, 0 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},        //23
    {10, 0 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},        //15
    {5, 0 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},         //7
    {0, 0 + AUDIO_MUSIC_MAX_VOLUME_DECIBELS},         //0
};


//通话音量映射曲线
const vol_map_t call_vol_map_list[CALL_VOL_MAP_SIZE] = {
#ifdef CALL_VOL_MAP_CFG
    CALL_VOL_MAP_CFG
#else
    {100, 0 + AUDIO_CALL_MAX_VOLUME_DECIBELS},       //127
    {90, -3 + AUDIO_CALL_MAX_VOLUME_DECIBELS},       //119
    {85, -6 + AUDIO_CALL_MAX_VOLUME_DECIBELS},       //111
    {80, -9 + AUDIO_CALL_MAX_VOLUME_DECIBELS},       //103
    {75, -13 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      //95
    {65, -17 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      //87
    {60, -21 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      //79
    {55, -25 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      //71
    {50, -29 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      //63
    {40, -33 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      //55
    {35, -37.5 + AUDIO_CALL_MAX_VOLUME_DECIBELS},    //47
    {30, -42 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      //39
    {25, -47 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      //31
    {15, -52 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      //23
    {10, -59 + AUDIO_CALL_MAX_VOLUME_DECIBELS},      //15
    {5, -66 + AUDIO_CALL_MAX_VOLUME_DECIBELS},       //7
    {0, -73 + AUDIO_CALL_MAX_VOLUME_DECIBELS},       //0
#endif
};

void app_audio_btaudio_init(void)
{
    as_btaudio_init_params_t prm;

    as_btaudio_music_volume_map_set(btmusic_vol_map_list, BTMUSIC_VOL_MAP_SIZE);
    as_btaudio_music_volume_air_map_set(btmusic_vol_air_map_list, BTMUSIC_VOL_AIR_MAP_SIZE);
    as_btaudio_call_volume_map_set(call_vol_map_list, CALL_VOL_MAP_SIZE);
    am_btaudio_volume_step_set(VOLUME_STEP);
    bt_volume_all_info_get(VOLUME_DEFAULT);
    am_btaudio_music_volume_value_set(VOLUME_DEFAULT);
    am_btaudio_call_volume_value_set(VOLUME_DEFAULT);
#if (2 == AUDIO_GAME_MODE_EN)
    am_a2dp_gameafx_plugin_register();
#endif
#if AUDIO_SPACE_MODE_EN
    am_a2dp_space_audio_register();
#endif

#if AUDIO_THIRD_PARTY_AENC
    //第三方通话算法库
    am_third_party_aenc_plugin_register(
        on_third_party_aenc_open,
        on_third_party_aenc_close,
        on_third_party_aenc_proc);
#else
#if (5 == AUDIO_REC_INPUT_SEL) || (6 == AUDIO_REC_INPUT_SEL) || (7 == AUDIO_REC_INPUT_SEL) || (8 == AUDIO_REC_INPUT_SEL) || (11 == AUDIO_REC_INPUT_SEL)
    am_2mic_aenc_plugin_register();
#else
    am_1mic_aenc_plugin_register();
#endif
#endif

    prm.p_aenc_config_params = &aenc_config_params;
    prm.p_aenc_params = &aenc_params;
    prm.p_admaenc_config_params = &admaenc_config_params;
    prm.p_admaenc_params = &admaenc_params;
    prm.p_admcoh_params = &admcoh_config_params;
    prm.p_aenc_txeqprm = &aenc_txeqprm;
    prm.p_aenc_txagcprm = &aenc_txagcprm;
    prm.p_aenc_rxeqprm = &aenc_rxeqprm;
    prm.p_aenc_rxagcprm = &aenc_rxagcprm;

    prm.phone_mic_test0 = SCO_PROCESS_TYPE_MIC0_RAW_DATA;
    prm.phone_mic_test1 = SCO_PROCESS_TYPE_MIC1_RAW_DATA;
    prm.phone_mic_test2 = SCO_PROCESS_TYPE_MIC2_RAW_DATA;
#if (0 == AUDIO_IN_DEVICE_SEL)
    prm.phone_mic_ch = 1;
#else
    prm.phone_mic_ch = audio_service_get_input_channels_count(AUDIO_REC_INPUT_SEL);

    //NOTE:此处重新定义mictest0, mictest1, mictest2,测试对应的MIC是哪个MIC,目的是为了兼容旧的定义(不考虑兼容可删除)
    //旧的定义：mictest0为通话的第一个mic, mictest1为通话的第二个mic
    switch (AUDIO_REC_INPUT_SEL) {
    case AUDIO_INPUT_AMIC0:
        prm.phone_mic_test0 = SCO_PROCESS_TYPE_MIC0_RAW_DATA;
        prm.phone_mic_test1 = SCO_PROCESS_TYPE_MIC1_RAW_DATA;
        prm.phone_mic_test2 = SCO_PROCESS_TYPE_MIC2_RAW_DATA;
        break;

    case AUDIO_INPUT_AMIC1:
        prm.phone_mic_test0 = SCO_PROCESS_TYPE_MIC1_RAW_DATA;
        prm.phone_mic_test1 = SCO_PROCESS_TYPE_MIC0_RAW_DATA;
        prm.phone_mic_test2 = SCO_PROCESS_TYPE_MIC2_RAW_DATA;
        break;

    case AUDIO_INPUT_AMIC2:
        prm.phone_mic_test0 = SCO_PROCESS_TYPE_MIC2_RAW_DATA;
        prm.phone_mic_test1 = SCO_PROCESS_TYPE_MIC0_RAW_DATA;
        prm.phone_mic_test2 = SCO_PROCESS_TYPE_MIC1_RAW_DATA;
        break;

    case AUDIO_INPUT_AMIC0_1:
        prm.phone_mic_test0 = SCO_PROCESS_TYPE_MIC0_RAW_DATA;
        prm.phone_mic_test1 = SCO_PROCESS_TYPE_MIC1_RAW_DATA;
        prm.phone_mic_test2 = SCO_PROCESS_TYPE_MIC2_RAW_DATA;
        break;

    case AUDIO_INPUT_AMIC0_2:
        prm.phone_mic_test0 = SCO_PROCESS_TYPE_MIC0_RAW_DATA;
        prm.phone_mic_test1 = SCO_PROCESS_TYPE_MIC2_RAW_DATA;
        prm.phone_mic_test2 = SCO_PROCESS_TYPE_MIC1_RAW_DATA;
        break;

    case AUDIO_INPUT_AMIC1_2:
        prm.phone_mic_test0 = SCO_PROCESS_TYPE_MIC1_RAW_DATA;
        prm.phone_mic_test1 = SCO_PROCESS_TYPE_MIC2_RAW_DATA;
        prm.phone_mic_test2 = SCO_PROCESS_TYPE_MIC0_RAW_DATA;
        break;
    }

#endif
    prm.tws_sco_packet_send_enable = 0;
#ifdef CFG_AENC_RX_NSLITE_ENABLE
    prm.aenc_nslite_enable = CFG_AENC_RX_NSLITE_ENABLE;
    prm.p_nslite_params = &nslite_config_params;
#else
    prm.aenc_nslite_enable = 0;
    prm.p_nslite_params = NULL;
#endif
    as_btaudio_service_init(&prm);
}

#endif //#if MODE_BT_EN
