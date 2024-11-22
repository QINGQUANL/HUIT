#ifndef _BT_AUDIO_SERVICE_H_
#define _BT_AUDIO_SERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sys_inc.h"
#include "audio_service.h"

enum bt_audio_status_e {
    BT_AUDIO_STATUS_IDLE,
    BT_AUDIO_STATUS_A2DP,       // 表示当前处于蓝牙音乐模式
    BT_AUDIO_STATUS_SCO,        // 表示当前处于通话模式
    BT_AUDIO_STATUS_AIR,        // 表示当前处于AIR模式
    BT_AUDIO_STATUS_TRANSITION, // 表示正在处理command
    BT_AUDIO_STATUS_EXIT,
};

enum sco_process_type_e {
    SCO_PROCESS_TYPE_AENC,           /* 使能通话算法 */
    SCO_PROCESS_TYPE_MIC0_RAW_DATA,  /* 关闭通话算法，上行数据使用MIC0的数据 */
    SCO_PROCESS_TYPE_MIC1_RAW_DATA,  /* 关闭通话算法，上行数据使用MIC1的数据 */
    SCO_PROCESS_TYPE_MIC2_RAW_DATA,  /* 关闭通话算法，上行数据使用MIC2的数据 */
};

enum bt_audio_cmd_e {
    BT_AUDIO_CMD_NONE,

    /* 开始蓝牙音乐播放
     * 命令参数表示最大等待命令处理的时间
     * Example:
     *    as_btaudio_cmd(BT_AUDIO_CMD_A2DP_START, OAL_WAIT_FOREVER);
     */
    BT_AUDIO_CMD_A2DP_START,

    /* 蓝牙音乐淡出处理命令
     * 命令参数无意义
     * Example:
     *    as_btaudio_cmd(BT_AUDIO_CMD_A2DP_FADEOUT, 1);
     * NOTE: 这是内部command， APP不要使用它
     */
    BT_AUDIO_CMD_A2DP_FADEOUT,

    /* 停止播放蓝牙音乐
     * 命令参数表示最大等待命令处理的时间
     * Example:
     *    as_btaudio_cmd(BT_AUDIO_CMD_A2DP_STOP, OAL_WAIT_FOREVER);
     */
    BT_AUDIO_CMD_A2DP_STOP,

    /* 通话开始命令
     * 命令参数表示最大等待命令处理的时间
     * Example:
     *    as_btaudio_cmd(BT_AUDIO_CMD_SCO_START, OAL_WAIT_FOREVER);
     */
    BT_AUDIO_CMD_SCO_START,

    /* 通话停止命令
     * 命令参数表示最大等待命令处理的时间
     * Example:
     *    as_btaudio_cmd(BT_AUDIO_CMD_SCO_STOP, OAL_WAIT_FOREVER);
     */
    BT_AUDIO_CMD_SCO_STOP,

    BT_AUDIO_CMD_AIR_START,
    BT_AUDIO_CMD_AIR_STOP,

    BT_AUDIO_CMD_EXIT,
    BT_AUDIO_CMD_A2DP_0DATA_GET,

    BT_AUDIO_CMD_CALL_AENC_EN,
    BT_AUDIO_CMD_CALL_AENC_GET,
};



/* Must be synchronized with internal header files */
enum phone_featrues_e {
    /* acoustic echo cancellaltion (adaptive filter) mask */
    PHONE_AEC        = (1 << 0),
    /* acoutic echo suppression  mask */
    PHONE_AES        = (1 << 1),
    /* ambient noise suppression mask */
    PHONE_NS         = (1 << 2),
    /* dereverberation mask */
    PHONE_DR         = (1 << 3),
    /* noise-dependent volume control of Rx mask */
    PHONE_AVC        = (1 << 4),
    /* automatic gain control for Tx mask */
    PHONE_TXAGC      = (1 << 5),
    /* automatic gain control for Rx mask */
    PHONE_RXAGC      = (1 << 6),
    /* comfort noise generation mask */
    PHONE_CNG        = (1 << 7),
    /* deemphasis mask */
    PHONE_EMP        = (1 << 8),
    /* dc notch of tx mask */
    PHONE_TXNOT      = (1 << 9),
    /* gain smooth mask */
    PHONE_GSMOOTH    = (1 << 10),
    /* dual-mic calibration mask */
    PHONE_DMCAB      = (1 << 11),
    /* dual-mic noise suppression mask*/
    PHONE_DMANS      = (1 << 12),
    /* wind noise suppression mask */
    PHONE_DMWNS      = (1 << 13),
    /* dual-mic adaptive steering */
    PHONE_DMADP      = (1 << 14),
    /* TX equalizer */
    PHONE_TXEQ       = (1 << 15),
    /* RX equalizer */
    PHONE_RXEQ       = (1 << 16),
    /* internal vad */
    PHONE_VAD        = (1 << 17),
    /* snr smoothing */
    PHONE_SMSNR      = (1 << 18),
    /* coh-snr */
    PHONE_COHSNR     = (1 << 19),
};


#define PHONE_AGC_MASK_LIMITER      (1)
#define PHONE_AGC_MASK_NOISEGATE    (8)
/*
    通话AGC参数
*/
typedef struct {
    uint8_t func_mask;
    uint8_t lev_mode;
    uint8_t look_ahead;
    uint8_t lt_ratio;
    int8_t lt_thd;
    uint8_t lt_kw;
    uint8_t ng_ratio;
    int8_t  ng_thd;
    uint8_t ng_kw;
    uint16_t ng_hold;
    uint16_t peak_att;
    uint16_t peak_rel;
    uint16_t lt_att;
    uint16_t lt_rel;
    uint16_t ng_att;
    uint16_t ng_rel;
    int8_t makeup_gain;
}phone_aenc_agc_params_t;

#define PHONE_AENC_EQ_BANDS (5)


#define PHONE_EQ_BIQUAD_LOWSHELVING  0
#define PHONE_EQ_BIQUAD_PEAKNOTCH    1
#define PHONE_EQ_BIQUAD_HIGHSHELVING 2
#define PHONE_EQ_BIQUAD_LOWPASS      3
#define PHONE_EQ_BIQUAD_BANDPASS     4
#define PHONE_EQ_BIQUAD_HIGHPASS     5

/* parametrical biquad filter prms */
typedef struct {
    /* filter type shelf low, shelf high, peak/notch */
    int16_t filter_type;
    /* center frequency(peak/notch) or crossover frequency(shelving filter ) */
    uint16_t fc;
    /* Gain -12~12 dB, Q3 */
    float G;
    /* Qfactor, -64 ~ 64, Q10 */
    float Q_factor;
} phone_aenc_sectprm_t;

typedef struct
{
    float PreAmp;
    phone_aenc_sectprm_t setting[PHONE_AENC_EQ_BANDS];
}phone_aenc_eq_params_t;

typedef struct {
    uint16_t R;
    uint16_t func_mask;
    uint8_t finit;
    float pf_Xi_opt;
    float pf_ph1_thd;
    float pf_alpha_npsd;
    float pf_alpha_PH1;
    float pf_alpha_Xis;
    float pf_alpha_Nmin_f;
    float pf_inc_Nmin_t;
    float pf_GHmin;
    float pf_Gcn;
    float Npsd_bias;
    float gain_smooth_thd;
    float gain_smooth_width;
    float SNR_s_Min;
    float psd_Min;
} phone_nslite_params_t;

typedef struct{
    uint8_t enable;
    int32_t cutfreq;
    float snrmin;
    uint8_t txnotch_enable;
    int32_t startfreq;
}phone_admcoh_params_t;

/*
 * 基本的通话降噪算法参数
 */
typedef struct {
    uint32_t func_mask;
    uint16_t rse_thd;
    uint16_t ref_thd;
    float alpha_slow;
    float alpha_fast;
    float pf_GHmin;

    float res_overest;
    float rse_maxG;
    float dt_thd_norm;

    float pf_Xi_opt;
    float Npsd_bias;

    float pf_Gcn;

    float avq_reftx;
    float avq_refrx;
    float avq_vadthd;
    float avq_offset;
    float avq_Gmax;


} phone_aenc_config_params_t;

/*
 * 其他通话降噪算法参数
 */
typedef struct {
    float gain_smooth_thd;
    float gain_smooth_width;
    float SNR_s_Min;
    float SNR_ns_Min;
    float psd_Min;
    float pf_alpha_Xis;
    float pf_alpha_Xins;

    float alpha_fast_resG;
    float alpha_slow_resG;
    float dt_thd_limit;
    float dt_init_thd;
    float dt_steprise_decvar;
    float dt_stepfall_decvar;

    uint8_t finit;
    float pf_ph1_thd;
    float pf_alpha_npsd;
    float pf_alpha_PH1;

    float pf_alpha_Nmin_f;
    float pf_inc_Nmin_t;

    float pf_alpha_z;
    float pf_decay;
    float decay_lthd;
    float alpha_decay;
    float decay_km;
    float rise_km;
    float thd_km;

    float avq_dec_step;
    float avq_sf_norm;
    float avq_alpha_fref;
    float avq_alpha_sref;
    float avq_alpha_fn;
    float avq_alpha_sn;
    float dt_overest;
    float init_overest;
    /* vad prms */
    /* vad inc time in ms */
    int16_t     vadinc_time;
    /* vad dec time in ms */
    int16_t     vaddec_time;
    /* vad noise psd min */
    int16_t     vadNmin;
    /* vad voice psd min */
    int16_t     vadVmin;
} phone_aenc_params_t;

/*
    双麦通话降噪算法基本参数
*/
typedef struct
{
    /* mask of dual-mic beamforming/ns */
    uint32_t func_mask;
    /* rse robust control */
    uint16_t rse_thd;
    /* reference counter update control*/
    uint16_t ref_thd;
    /* fast smoothing prm */
    float alpha_slow;
    /* slow smoothing prm */
    float alpha_fast;
    /* minimum gain in dB */
    float pf_GHmin;
    /* prms for res */
    float res_overest;
    /* rse */
    float rse_maxG;
    /* prms for double talk detection */
    float dt_thd_norm;
    /* optimal SNR in dB */
    float pf_Xi_opt;
    /* Ambient noise psd estimatin bias */
    float Npsd_bias;
    /* CNG level */
    float pf_Gcn;
    float cab_thd;

    /* mic distance in mm */
    uint16_t d;
    /* angle of desired voice in degree 0 ~ 90 */
    uint16_t theta;
    /* mic mode, 0: front mic first, rear mic second, 1: rear mic first, front mic second */
    uint8_t mode;
}phone_admaenc_config_params_t;


/*
    双麦通话降噪算法附加参数
*/
typedef struct
{
    /* initial ambient noise recursive estimation duration in frames */
    uint8_t finit;
    /* prms for calibration */
    /* step size for calibration filter */
    float uw;
    /* step size for adaptive beamforming filter */
    float u;
    /* vad thd in dB */
    float vad_thd;
    /* wind onset thd */
    float wind_thd;
    /* smoothing prm for reference psd calculation */
    float alpha_xpsd;
    /* calibration low freq */
    uint16_t cflow;
    /* calibration high freq */
    uint16_t cfhigh;
   /* wind noise detection freq region low */
    uint16_t wflow;
    /* wind noise detection freq region high */
    uint16_t wfhigh;
    /* slow varied calibrated weight low pass cutoff freq */
    uint16_t lpfws;
    /* fast varied calibrated weight low pass cutoff freq */
    uint16_t lpfwf;
    /* prms for ANS */

    /* maximum of allowable SPP */
    float pf_ph1_thd;
    /* ambient noise psd smoothing prm*/
    float pf_alpha_npsd;
    /* SPP smoothing prm */
    float pf_alpha_PH1;
    /* SNR smoothing prm */
    float pf_alpha_Xis;

    /* minimum SNR for signal to ambient noise */
    float SNR_s_Min;
    /* allowable mimum psd */
    float psd_Min;

    /* Wind Noise prms */
    /* recursive wiener filter prm a */
    float Rwa;
    /* recursive wiener filter prm c */
    float Rwc;

     /* decay time after onset false */
    int8_t N_decay;
    /* hold time after onset true */
    int8_t N_hold;

    /* CNG module */
    /* CNG smoothing prm for frequency domain */
    float pf_alpha_Nmin_f;
    /* CNG increment step */
    float pf_inc_Nmin_t;


    /*gain smoothing module */
    /* up limit for snr to use gain smoothing */
    float gain_smooth_thd;
    /* gain smoothing width in frequency bin num */
    float gain_smooth_width;
    float alpha_fast_resG;
    float alpha_slow_resG;

    float dt_thd_limit;
    float dt_init_thd;
    float dt_steprise_decvar;
    float dt_stepfall_decvar;
    /* non-stationary SNR low bound */
    float SNR_ns_Min;
    float alpha_Xins;
    float dt_overest;
    float init_overest;
    /* vad prms */
    /* vad inc time in ms */
    int16_t     vadinc_time;
    /* vad dec time in ms */
    int16_t     vaddec_time;
    /* vad noise psd min */
    int16_t     vadNmin;
    /* vad voice psd min */
    int16_t     vadVmin;
} phone_admaenc_params_t;


typedef struct {
    const phone_aenc_config_params_t *p_aenc_config_params;
    const phone_aenc_params_t *p_aenc_params;
    const phone_admaenc_config_params_t *p_admaenc_config_params;
    const phone_admaenc_params_t *p_admaenc_params;
    const phone_admcoh_params_t *p_admcoh_params;
    const phone_aenc_eq_params_t *p_aenc_txeqprm;
    const phone_aenc_agc_params_t *p_aenc_txagcprm;
    const phone_aenc_eq_params_t *p_aenc_rxeqprm;
    const phone_aenc_agc_params_t *p_aenc_rxagcprm;
    const phone_nslite_params_t *p_nslite_params;
    uint8_t phone_mic_ch;
    uint8_t phone_mic_test0;
    uint8_t phone_mic_test1;
    uint8_t phone_mic_test2;
    uint8_t tws_sco_packet_send_enable;
    uint8_t aenc_nslite_enable;
} as_btaudio_init_params_t;


/* 给bt_audio发送一个命令
 * @param cmd: 参考 @enum bt_audio_cmd_e
 * @param val: 命令参数
 */
void as_btaudio_cmd(uint32_t cmd, uint32_t val);


/***************************************************************************
 *               bt audio API
 ***************************************************************************/
//创建一个bt_audio实例
void as_btaudio_create(void);

//销毁一个bt_audio实例
void as_btaudio_destroy(void);

/* 获取bt_audio当前的状态
 * @return: 返回状态，参考@enum bt_audio_status_e
 */
uint8_t as_btaudio_sta_get(void);

//开始播放蓝牙音乐
#define as_btaudio_a2dp_start()  \
    ({ \
        as_btaudio_cmd(BT_AUDIO_CMD_A2DP_START, OAL_WAIT_FOREVER); \
    })

//停止播放蓝牙音乐
#define as_btaudio_a2dp_stop()                as_btaudio_cmd(BT_AUDIO_CMD_A2DP_STOP, OAL_WAIT_FOREVER)

//开始通话
#define as_btaudio_sco_start()  \
    ({ \
        as_btaudio_cmd(BT_AUDIO_CMD_SCO_START, OAL_WAIT_FOREVER); \
    })

//停止通话
#define as_btaudio_sco_stop()                 as_btaudio_cmd(BT_AUDIO_CMD_SCO_STOP, OAL_WAIT_FOREVER)

//开始AIR
#define bt_audio_air_start()                as_btaudio_cmd(BT_AUDIO_CMD_AIR_START, OAL_WAIT_FOREVER)
//停止AIR
#define bt_audio_air_stop()                 as_btaudio_cmd(BT_AUDIO_CMD_AIR_STOP, OAL_WAIT_FOREVER)

int32_t as_air_bs_read(void *buf);
void audio_air_start(void);
void audio_air_stop(void);

/* 设置通话音量
 * @param volume: call volume: 0 ~ 100
 *
 * Example:
 *    //set call volume as 50
 *    as_btaudio_call_volume_set(50);
 */
void as_btaudio_call_volume_set(uint8_t volume);

/* 获取通话音量
 * @return: current call volume(0~100)
 *
 * Example:
 *    uint8_t vol = as_btaudio_call_volume_get();
 *    logi("current call volume is %d", vol);
 */
uint8_t as_btaudio_call_volume_get(void);

void as_btaudio_incoming_ringtone_volume_set(uint8_t volume);
uint8_t as_btaudio_incoming_ringtone_volume_get(void);


uint8_t as_btaudio_mictest_type_to_sco_process_type(uint8_t mic_test_type);

/* 设置通话处理类型，可使用此接口使能通话算法或者关闭通话算法
 * @param type: 通话处理类型，参考@enum sco_process_type_e
 */
void as_btaudio_sco_process_type_set(uint8_t type);

/* 获取通话处理类型
 * @return: 返回当前的通话处理类型，参考@enum sco_process_type_e
 */
uint8_t as_btaudio_sco_process_type_get(void);

void as_btaudio_sco_audio_irdata_store(void);

void *as_btaudio_a2dp_atrack_get(void);
void as_btaudio_a2dp_atrack_set(void *atrack);
void *as_btaudio_sco_atrack_get(void);
void as_btaudio_sco_atrack_set(void *atrack);

void as_btaudio_siri_enable_set(bool enable);
bool as_btaudio_siri_enable_get(void);

float as_btaudio_call_volume_f32_get(void);
void as_btaudio_music_volume_map_set(const vol_map_t *map, uint32_t map_size);
void as_btaudio_music_volume_air_map_set(const vol_map_t *map, uint32_t map_size);
void as_btaudio_call_volume_map_set(const vol_map_t *map, uint32_t map_size);

void as_btaudio_btmusic_volume_set(uint8_t volume);
float as_btaudio_btmusic_volume_f32_get(void);
uint8_t as_btaudio_btmusic_volume_get(void);

bool as_btaudio_sco_audio_irdata_get(uint32_t fs, float **ppdata, int32_t *plen);
void as_btaudio_sco_audio_irdata_set(uint32_t fs, float *pdata, int32_t len);
uint8_t as_btaudio_aenc_nslite_enable_get(void);
void as_btaudio_calling_state_set(uint8_t state);
uint8_t as_btaudio_calling_state_get(void);
const void *as_btaudio_aenc_config_params_get(void);
const void *as_btaudio_aenc_params_get(void);
const void *as_btaudio_admaenc_config_params_get(void);
const void *as_btaudio_admaenc_params_get(void);
const void *as_btaudio_aenc_txeqprm_get(void);
const void *as_btaudio_aenc_txagcprm_get(void);
const void *as_btaudio_aenc_rxeqprm_get(void);
const void *as_btaudio_aenc_rxagcprm_get(void);
const void *as_btaudio_nslite_params_get(void);
const void *as_btaudio_admcoh_params_get(void);
uint8_t as_btaudio_mic_ch_get(void);
uint8_t as_btaudio_tws_sco_packet_send_enable_get(void);

void as_btaudio_service_init(as_btaudio_init_params_t *init_params);
void as_btaudio_service_deinit(void);

bool as_btaudio_is_call_aenc_disable();

#ifdef __cplusplus
}
#endif

#endif /* _BT_AUDIO_SERVICE_H_ */
