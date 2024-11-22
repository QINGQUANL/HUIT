
/*****************************************************************************
* AUDIO DRC相关代码
****************************************************************************/
#define LOG_LEV 4
#include "soundcard.h"
#include "user_config.h"
#include "audio_afx_config.h"
#include "atunning_common.h"
#include "audio_service.h"
#include "hal.h"
#include "key_ctrl_led.h"

void audioeffect_register(void)
{
#if AUDIO_PLAYBACK_EQ
    audioeffect_eq14_register();
#endif
#if AUDIO_MIC_EQ
    audioeffect_eq14_register();
#endif

#if AUDIO_PLAYBACK_VSS
    audioeffect_vss_register();
#endif

#if AUDIO_MIC_REVERB
    audioeffect_fdnrev_register();
#endif

#if AUDIO_PLAYBACK_DRC
    audioeffect_drc_register();
#endif
#if AUDIO_MIC_DRC
    audioeffect_drc_register();
#endif

#if AUDIO_MIC_FREQ_SHIFT
    audioeffect_freqshift_register();
#endif
#if AUDIO_MIC_TSPS
    audioeffect_tsps_register();
    audioeffect_fc_register();
#endif
#if AUDIO_MIC_ECHO
    audioeffect_echo_register();
#endif
}

void audioeffect_enable(void)
{
#if AUDIO_PLAYBACK_VSS
    vss_enable_set(1);//VSS功能
#endif
#if AUDIO_MIC_REVERB
    fdnrev_config();
    mic_fdnrev_enable_set(1);//混响功能
#if MYME_M4_BOARD
    key_led_set_status(led4_green, 1);
#endif
#if EVB_RX_DEMO
    REVERB_DISP_ON();
#endif
#endif

#if AUDIO_MIC_DRC
    //mic_drc_natural_noise_gate_config();
    drc_enable_set(PAGE_MIC, 1);//DRC功能
#endif
#if AUDIO_PLAYBACK_DRC
    //music_drc_natural_noise_gate_config();
    //music_drc_lt_gate_config();
    drc_enable_set(PAGE_MUSIC, 1);//DRC功能
#endif

#if 0 //AUDIO_MIC_ECHO
    echo_setting_t kid_echo;
    kid_echo.cutoff_freq = 3500;
    kid_echo.dly = 200;     
    kid_echo.gDry = 0.5;
    kid_echo.gFbc =0.3;
    kid_echo.gWet = 0.6;
    kid_echo.need_lp = 1;
    kid_echo.onChange = true;
    kid_echo.enable = 1;
    echo_config_set(&kid_echo);
#endif

}

#if AUDIO_PLAYBACK_TSPS || AUDIO_MIC_TSPS

//男声
void mic_tsps_male_config(void)
{
#if AUDIO_MIC_TSPS
    tsps_config_set(1, (float)0.73713460864555060614578238386647, 0, 0, (float)1.0);
#endif
}

void mic_tsps_male_config_close(void)
{
#if AUDIO_MIC_TSPS
    tsps_config_set(0, (float)0.73713460864555060614578238386647, 0, 0, (float)1.0);
#endif
}

//女声
void mic_tsps_female_config(void)
{
#if AUDIO_MIC_TSPS
    tsps_config_set(1, (float)2.0, 0, 1, (float)0.6);
    fc_config_set(1,(float)0.6);
#endif
}

void mic_tsps_female_config_close(void)
{
#if AUDIO_MIC_TSPS
    tsps_config_set(0, (float)2.0, 0, 0, (float)1.0);
    fc_config_set(0,(float)1.0);
#endif
}

//机器人声
void mic_tsps_robot_config(void)
{
#if AUDIO_MIC_TSPS
    tsps_config_set(1, (float)0.65975395538644712968700098561482, 0, 0, (float)1.0);
#if AUDIO_MIC_ECHO
    echo_setting_t robot_echo;
    robot_echo.cutoff_freq = 2890;
    robot_echo.dly = 24; 
    robot_echo.gDry = 0.54;
    robot_echo.gFbc =0.85;
    robot_echo.gWet = 1.0;
    robot_echo.need_lp = 1;
    robot_echo.onChange = true;
    robot_echo.enable = 1;  
    echo_config_set(&robot_echo);
#endif
#endif
}

void mic_tsps_robot_config_close(void)
{
#if AUDIO_MIC_TSPS
    tsps_config_set(0, (float)0.65975395538644712968700098561482, 0, 0, (float)1.0);
#if AUDIO_MIC_ECHO
    echo_setting_t robot_echo;
    robot_echo.cutoff_freq = 2890;
    robot_echo.dly = 24; 
    robot_echo.gDry = 0.54;
    robot_echo.gFbc =0.85;
    robot_echo.gWet = 1.0;
    robot_echo.need_lp = 1;
    robot_echo.onChange = 0;
    robot_echo.enable = 0;  
    echo_config_set(&robot_echo);
#endif
#endif
}

//童声
void mic_tsps_kid_config(void)
{
#if AUDIO_MIC_TSPS
    tsps_config_set(1, (float)1.8403753012497501567808624764036, 0, 0, (float)1.0);
#if AUDIO_MIC_ECHO
    echo_setting_t kid_echo;
    kid_echo.cutoff_freq = 3500;
    kid_echo.dly = 92; 
    kid_echo.gDry = 0.5;
    kid_echo.gFbc =0.3;
    kid_echo.gWet = 0.6;
    kid_echo.need_lp = 1;
    kid_echo.onChange = true;
    kid_echo.enable = 1;
    echo_config_set(&kid_echo);
#endif
#endif
}

void mic_tsps_kid_config_close(void)
{
#if AUDIO_MIC_TSPS
    tsps_config_set(0, (float)1.8403753012497501567808624764036, 0, 0, (float)1.0);
#if AUDIO_MIC_ECHO
    echo_setting_t kid_echo;
    kid_echo.cutoff_freq = 3500;
    kid_echo.dly = 92; 
    kid_echo.gDry = 0.5;
    kid_echo.gFbc =0.3;
    kid_echo.gWet = 0.6;
    kid_echo.need_lp = 1;
    kid_echo.onChange = 0;
    kid_echo.enable = 0;
    echo_config_set(&kid_echo);
#endif
#endif
}

//变声
void mic_tsps_vc_config(void)
{
#if AUDIO_MIC_TSPS
    tsps_config_set(1, (float)1.569168195793501471449700633635, 0, 0, (float)1.0);
#endif
}

void mic_tsps_vc_config_close(void)
{
#if AUDIO_MIC_TSPS
    tsps_config_set(0, (float)1.569168195793501471449700633635, 0, 0, (float)1.0);
#endif
}

//电音参数
atune_setting_t default_atune = {
    .note = 0,      /* Notes base */
    .oct = 4,       /* octave idx */
    .scale = 1,     /* scale style */
    .speed = 10,    /* atune speed, 1~10 */
    .enable = true,
    .onChange = true
};
void mic_autotune_config(void)
{
    autotune_config_set(&default_atune);
}
#endif

#if AUDIO_PLAYBACK_EQ || AUDIO_MIC_EQ
//---------  Default eq14 setting -----

const biquad_setting_t app_eq_rt_bands_init[MAX_EQ_BANDS]={
    { BIQUAD_PEAKNOTCH, 25,    0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 50,    0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 75,    0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 150,   0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 300,   0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 600,   0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 1000,  0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 2000,  0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 3000,  0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 6000,  0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 8000,  0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 10000, 0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 12000, 0.0, 0.75 },
    { BIQUAD_PEAKNOTCH, 16000, 0.0, 0.75 }
};

const bool app_eq_rt_bands_disable_init[MAX_EQ_BANDS] = {
    false, false, false, false, false, false, false, false, false, false
};

const float app_eq_rt_channel_preamp_init = 0.0f;

aeffect_eq_sec14_tunningarea default_eq14_common;

void eq_data_init()
{
    for(int ch = 0; ch < MAX_EQ_CHANNELS; ch++)
    {
        for(int b = 0; b < MAX_EQ_BANDS; b++)
        {
            libc_memcpy(&default_eq14_common.eq_bands[ch][b].bs, &app_eq_rt_bands_init[b], sizeof(biquad_setting_t));
            default_eq14_common.eq_bands[ch][b].disable = app_eq_rt_bands_disable_init[b];
            default_eq14_common.eq_bands[ch][b].onChange = true;
        }
        default_eq14_common.eq_preamp[ch].preamp   = app_eq_rt_channel_preamp_init;
        default_eq14_common.eq_preamp[ch].onChange = true;
    }
}

aeffect_eq_sec14_tunningarea*   eq14_common_focus = &default_eq14_common;

static void default_eq14_common_focus_get(void)
{
    eq_data_init();
    eq14_common_focus = &default_eq14_common;
}

/* eq14配置用户参数函数
 */
void mic_eq14_config(void)
{
#if AUDIO_MIC_EQ
    default_eq14_common_focus_get();
    uint32_t page_id = PAGE_MIC;
    eq14_config_set(page_id, eq14_common_focus);
#endif
}

void music_eq14_config(void)
{
#if AUDIO_PLAYBACK_EQ
    default_eq14_common_focus_get();
    uint32_t page_id = PAGE_MUSIC;    //0为music page;3、4分别为OUTPUT1、OUTPUT2 page
    eq14_config_set(page_id, eq14_common_focus);
#endif
}


#endif

#if AUDIO_PLAYBACK_VSS
//---------  Default vss setting -----
#define DEFAULT_VSS_MODE            VSS_MODE_SPEAKER    //FIXME VSS_MODE_HEADPHONE卡顿
#define VSS_PARA_DECIDED_BY_DRIVER  0

vss_tunningarea defualt_vss_common = {
    .eq5 = {
        //filter_tye, fc, G, Q, disable, onChange
        {{BIQUAD_PEAKNOTCH, 25,     0.0, 0.75}, false, true},
        {{BIQUAD_PEAKNOTCH, 300,    0.0, 0.75}, false, true},
        {{BIQUAD_PEAKNOTCH, 3000,   0.0, 0.75}, false, true},
        {{BIQUAD_PEAKNOTCH, 8000,   0.0, 0.75}, false, true},
        {{BIQUAD_PEAKNOTCH, 16000,  0.0, 0.75}, false, true},
    },

    .limiter = {
        //start threshold, subfunc ratio, attack time, release time, enable switch
        -6, 32, 10, 200, false, true
    },

    .hp_and_spk = {
        //speaker
        {3, { 1 }, 10, 6, 6, 100, true},
        //headphone
        {3, { 5 }, 10, 6, 6, 100, true}
    },

    .mode_setting = {DEFAULT_VSS_MODE, true},
    .enable_setting = {VSS_PARA_DECIDED_BY_DRIVER, VSS_PARA_DECIDED_BY_DRIVER}
};

vss_tunningarea*        vss_common_focus = &defualt_vss_common;

static void default_vss_common_focus_get(void)
{
    vss_common_focus = &defualt_vss_common;
}

/* VSS配置用户参数函数
 */
void vss_config(void)
{
    default_vss_common_focus_get();
    vss_config_set(vss_common_focus);
}
#endif

#if AUDIO_MIC_REVERB
//---------  Default reverb setting -----
#define DEFAULT_REVB_MAX_RT60   1000//2000
#define DEFAULT_REVB_MAX_GWET   0.25f//0.35f


fdnrev_pro_setting default_fdnrev = {
    .revb_time = DEFAULT_REVB_MAX_RT60,
    .damp_freq_lo = 20,
    .damp_freq_hi = 8000,
    .lpf_freq = 10000,
    .hpf_freq = 20,
    .pre_delay = 100,
    .diff = 0.6,
    .damp_ratio_lo = 0.52,
    .damp_ratio_hi = 0.52,
    .g_wet = DEFAULT_REVB_MAX_GWET,
    .g_dry = 0.0,       //默认default_fdnrev内部不出干声，在外部混干声，否则可以设置该值为1.0
    .m2s = AUDIO_EFFECT_FDNREV_STEREO,
    .input_diff = 0,
    .onChange = true
};

fdnrev_pro_setting* fdnrev_focus          = &default_fdnrev;

static void default_fdnrev_focus_get(void)
{
    fdnrev_focus = &default_fdnrev;
}

/* REVERB配置用户参数函数
 */
void fdnrev_config(void)
{
    default_fdnrev_focus_get();
    fdnrev_config_set(fdnrev_focus);
}

bool mic_fdnrev_enable_set(uint8_t enable)
{
    bool fdnrev_enable;
    if(enable) {
        audio_service_add_features(AUDIO_FEATURE_RECORD_EFFECTS);
    } else {
        audio_service_clear_features(AUDIO_FEATURE_RECORD_EFFECTS);
    }
    auto_set_cpux_freq();
    
    fdnrev_enable_set(enable);
            
    fdnrev_enable = fdnrev_enable_get();
    if(fdnrev_enable != enable) {
        if(fdnrev_enable) {
            audio_service_add_features(AUDIO_FEATURE_RECORD_EFFECTS);
        } else {
            audio_service_clear_features(AUDIO_FEATURE_RECORD_EFFECTS);
        }
        auto_set_cpux_freq();
    }
    if(fdnrev_enable) {
        Lmic_wet_vol_mute_set(false);               //有线MIC干声到音效
        wrx_wet_vol_mute_set(false);                //无线接收(无线MIC干声)到音效
    } else {
        Lmic_wet_vol_mute_set(true);               //有线MIC干声到音效
        wrx_wet_vol_mute_set(true);                //无线接收(无线MIC干声)到音效
    }
    
    return fdnrev_enable;
}

void mic_fdnrev_refresh(void)
{
    bool fdnrev_enable = fdnrev_enable_get();
    if(fdnrev_enable) {
        audio_service_add_features(AUDIO_FEATURE_RECORD_EFFECTS);
    } else {
        audio_service_clear_features(AUDIO_FEATURE_RECORD_EFFECTS);
    }
    auto_set_cpux_freq();
    if(fdnrev_enable) {
        Lmic_wet_vol_mute_set(false);               //有线MIC干声到音效
        wrx_wet_vol_mute_set(false);                //无线接收(无线MIC干声)到音效
    } else {
        Lmic_wet_vol_mute_set(true);               //有线MIC干声到音效
        wrx_wet_vol_mute_set(true);                //无线接收(无线MIC干声)到音效
    }
}


#endif

#if AUDIO_MIC_DRC || AUDIO_PLAYBACK_DRC
//---------  Default drc setting -----

//限幅器
drc_subfunc_setting mic_drc_limiter = {
    .x_thd    = -40,
    .x_ratio  = 1,
    .x_att    = 0,
    .x_rel    = 0,
    .enable   = true,
    .onChange = true,
};

//压缩器
drc_subfunc_setting mic_drc_compress = {
    .x_thd    = -20,//-40,
    .x_ratio  = 2,//1,
    .x_att    = 20,//0,
    .x_rel    = 800,//0,
    .enable   = true,
    .onChange = true,
};

//扩展器
drc_subfunc_setting mic_drc_expander = {
    .x_thd    = -80,
    .x_ratio  = 1,
    .x_att    = 0,
    .x_rel    = 0,
    .enable   = true,
    .onChange = true,
};

//噪声门
drc_subfunc_setting mic_drc_natual_noise_gate = {
    .x_thd    = -50,
    .x_ratio  = 3,
    .x_att    = 3,
    .x_rel    = 50,
    .enable   = true,
    .onChange = true,
};


drc_subfunc_setting music_drc_natual_noise_gate = {
    .x_thd    = -65,
    .x_ratio  = 3,
    .x_att    = 3,
    .x_rel    = 50,
    .enable   = true,
    .onChange = true,
};

drc_subfunc_setting music_drc_natual_lt = {
    .x_thd    = 0,
    .x_ratio  = 10,
    .x_att    = 3,
    .x_rel    = 50,
    .enable   = true,
    .onChange = true,
};

drc_comm_param_setting drc_comm_param = {
    .makeup_gain = 0,
    .channel_gain = 0,          //为1时左右声道使用各自的gain
    .onChange    = true,
};


drc_subfunc_setting* mic_drc_subfunc_focus      = &mic_drc_natual_noise_gate;
drc_subfunc_setting* music_drc_subfunc_focus    = &music_drc_natual_noise_gate;
drc_comm_param_setting* drc_comm_param_focus    = &drc_comm_param;


static void mic_drc_limiter_focus_get(void)
{
    mic_drc_subfunc_focus = &mic_drc_limiter;
}


/* DRC 限幅器配置用户参数函数
 */
void mic_drc_limiter_config(void)
{
    mic_drc_limiter_focus_get();
    drc_config_set(PAGE_MIC, DRC_MASK_LIMITER, mic_drc_subfunc_focus);
}

static void mic_drc_compress_focus_get(void)
{
    mic_drc_subfunc_focus = &mic_drc_compress;
}

/* DRC 压缩器配置用户参数函数
 */
void mic_drc_compress_config(void)
{
    mic_drc_compress_focus_get();
    drc_config_set(PAGE_MIC, DRC_MASK_COMPRESSOR, mic_drc_subfunc_focus);
}

static void mic_drc_expander_focus_get(void)
{
    mic_drc_subfunc_focus = &mic_drc_expander;
}

/* DRC 扩展器配置用户参数函数
 */
void mic_drc_expander_config(void)
{
    mic_drc_expander_focus_get();
    drc_config_set(PAGE_MIC, DRC_MASK_EXPANDER, mic_drc_subfunc_focus);
}


static void mic_drc_natural_noise_gate_focus_get(void)
{
    mic_drc_subfunc_focus = &mic_drc_natual_noise_gate;
}

/* DRC噪声门配置用户参数函数
 */
void mic_drc_natural_noise_gate_config(void)
{
    mic_drc_natural_noise_gate_focus_get();
    drc_config_set(PAGE_MIC, DRC_MASK_NOISEGATE, mic_drc_subfunc_focus);
}

static void music_drc_natural_noise_gate_focus_get(void)
{
    music_drc_subfunc_focus = &music_drc_natual_noise_gate;
}

static void music_drc_natural_lt_focus_get(void)
{
    music_drc_subfunc_focus = &music_drc_natual_lt;
}

/* DRC配置用户参数函数
 */
void music_drc_natural_noise_gate_config(void)
{
    music_drc_natural_noise_gate_focus_get();
    drc_config_set(PAGE_MUSIC, DRC_MASK_NOISEGATE, music_drc_subfunc_focus);
}

/* DRC配置用户参数函数
 */
void music_drc_lt_gate_config(void)
{
    music_drc_natural_lt_focus_get();
    drc_config_set(PAGE_MUSIC, DRC_MASK_LIMITER, music_drc_subfunc_focus);
}

/* DRC comm 配置用户参数函数
 */
void drc_comm_param_config(void)
{
    drc_comm_param_set(PAGE_MIC, drc_comm_param_focus);
}


#endif

#if AUDIO_MIC_FREQ_SHIFT
freqshift_subfunc_setting freqshift_natual_fs = {
    .deltafreq = 0,
    .onChange = true,
};

freqshift_subfunc_setting* freqshift_subfunc_focus      = &freqshift_natual_fs;

static void freqshift_get(void)
{
    freqshift_subfunc_focus = &freqshift_natual_fs;
}

/* 移频配置用户参数函数
 */
void freqshift_config(void)
{
    freqshift_get();
    freqshift_config_set(freqshift_subfunc_focus);
}

#endif
