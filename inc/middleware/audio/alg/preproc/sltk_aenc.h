#ifndef _SLTK_AENC_H_
#define _SLTK_AENC_H_

#include "sltk_math.h"
#include "sltk_filter.h"

/* acoustic echo cancellaltion (adaptive filter) mask */
#define MASK_AEC	(1<<0)
/* acoutic echo suppression  mask */
#define MASK_AES	(1<<1)
/* ambient noise suppression mask */
#define MASK_NS		(1<<2)
/* dereverberation mask */
#define MASK_DR		(1<<3)
/* noise-dependent volume control of Rx mask */
#define MASK_AVC	(1<<4)
/* automatic gain control for Tx mask */
#define MASK_TXAGC	(1<<5)
/* automatic gain control for Rx mask */
#define MASK_RXAGC	(1<<6)
/* comfort noise generation mask */
#define MASK_CNG	(1<<7)
/* deemphasis mask */
#define MASK_EMP	(1<<8)
/* dc notch of tx mask */
#define MASK_TXNOT	(1<<9)
/* gain smooth mask */
#define MASK_GSMOOTH (1<<10)
/* equalizer for TX */
#define MASK_TXEQ   (1<<11)
/* equalizer for RX */
#define MASK_RXEQ   (1<<12)
/* internal VAD */
#define MASK_INTVAD (1<<13)
/* snr smooth */
#define MASK_SMSNR (1<<14)
/* smooth white noise*/
#define MASK_SMOOTH_NOISE (1<<15)



#define AENC_EQ_BANDS   (5)

/* command for AGC and AENC */
typedef struct {
    uint8_t func_mask;
    uint8_t lev_mode;   //瞬时或者平均模式
    uint8_t look_ahead; //前视，会有点延时  
    uint8_t lt_ratio;   //限幅器压缩比例
    int8_t lt_thd;      //限幅器启动电平
    uint8_t lt_kw;      //过渡宽度
    uint8_t ng_ratio;   //噪声门压缩比例
    int8_t  ng_thd;     //噪声门启动电平，阈值
    uint8_t ng_kw;      //噪声门过渡宽度
    uint16_t ng_hold;   //噪声门保持时间
    uint16_t peak_att;  //峰值跟踪启动时间
    uint16_t peak_rel;  //峰值跟踪释放时间
    uint16_t lt_att;    //限幅器启动时间
    uint16_t lt_rel;    //限幅器释放时间
    uint16_t ng_att;    //噪声门启动时间
    uint16_t ng_rel;    //噪声门释放时间
    int8_t makeup_gain; /*-50- 50*/
}aenc_drcrtprm_t;

typedef struct
{
    float PreAmp;
    biquad_setting_t setting[AENC_EQ_BANDS];
}aenc_eqrtprm_t;

typedef struct {
    /* time constant for fast tracking rise edge in ms */
    float alpha_fr;
    /* time constant for fast tracking falling edge in ms */
    float alpha_ff;
    /* time constant for slow tracking rise edge in ms */
    float alpha_sr;
    /* time constant for slow tracking falling edge in ms */
    float alpha_sf;
    /* smoothing prm for short term statistics of vad */
    float vad_alpha_short;
    /* smoothing prm for long term statistics of vad */
    float vad_alpha_long;
    /* smoothing prm for log ration of vad */
    float vad_alpha_ratio;
    /* max gain */
    float gmax;
    /* min gain */
    float gmin;
    /* target level */
    float xtgt;
    /* thd of log ratio (up limit)*/
    float vad_log_ratio_uthd;
    /* thd of log ratio (low limit)*/
    float vad_log_ratio_lthd;
    /* min of log ratio */
    float vad_log_ratio_min;
    /* max of log ratio */
    float vad_log_ratio_max;
    /* min of log var */
    float vad_var_min;
    /* min of amplitude dB */
    float amp_min;
} agc_rtprm_t;

typedef struct
{
    uint16_t fs;
    uint16_t N;
    uint8_t P;
    uint8_t L;
    uint8_t M;
    /* bulk delay block count */
    uint8_t O;
}aenc_initprm_t;

/* init prms */
typedef struct
{
    uint16_t func_mask; //base：
    uint8_t finit;      //base:噪声初始化帧数
    float alpha_slow;       //advance:功率谱估计慢速平滑因子
    float alpha_fast;       //advance:功率谱估计快速平滑因子
    /* prms for res */
    float res_overest;      //advance:残留回声过估计因子
    float alpha_fast_resG;  //advance:残留回声估计快速平滑因子
    float alpha_slow_resG;  //advance:残留回声估计慢速平滑因子
    /* prms for double talk detection */
    float dt_thd_norm;      //advance:双讲检测阈值
    float dt_thd_limit;     //advance:双讲检测阈值低门限
    float dt_init_thd;      //advance:初始化双讲检测因子
    float dt_steprise_decvar;   //advance:双讲检测快速平滑因子
    float dt_stepfall_decvar;   //advance:双讲检测慢速平滑因子
    /* prms for post filter(ns+dr) */
    float pf_Xi_opt;        //base: NS（噪声抑制）最优信噪比
    float pf_ph1_thd;       //base: 语音检测最大阈值
    float pf_alpha_npsd;        //advance:噪声谱估计平滑因子
    float pf_alpha_PH1;         //advance:语音概率平滑因子
    float pf_alpha_Xis;         //advance:信号/平稳噪声比平滑因子
    float pf_alpha_Xins;        //advance:信号/平稳噪声比非平滑因子
    float pf_alpha_z;           //advance:混响估计平滑因子
    float pf_alpha_Nmin_f;      //advance:舒适噪声估计平滑因子
    float pf_inc_Nmin_t;        //advance:舒适噪声调整因子
    float pf_GHmin;     //base: 最低增益（最大噪声抑制）
    float pf_decay;             //advance:混响衰减因子

    /* prms for avq */
    float avq_reftx;            //advance:90db SPL麦克风信号功率
    float avq_refrx;            //advance:90db SPL喇叭信号功率
    float avq_vadthd;           //advance:语音活动检查阈值
    float avq_offset;           //advance:控制附加增益
    float avq_dec_step;         //advance:控制衰减步长
    float avq_sf_norm;          //advance:归一化音量功率
    /* new added prm */
    float avq_alpha_fref;       //advance:喇叭信号功率估计快速平滑因子
    float avq_alpha_sref;       //advance:喇叭信号功率估计慢速平滑因子
    float avq_alpha_fn;         //advance:噪声信号功率估计快速平滑因子
    float avq_alpha_sn;         //advance:噪声信号功率估计慢速平滑因子
    float avq_Gmax;             //advance:自动音量控制最大增益
    float Npsd_bias;            //advance:噪声估计偏置系数
    float decay_lthd;           //advance:混响衰减阈值
    float alpha_decay;          //advance:混响预衰减估计平滑因子
    float decay_km;             //advance:混响信混比慢速平滑因子
    float rise_km;              //advance:混响信混比快速平滑因子
    float thd_km;               //advance:混响信混比阈值
    float gain_smooth_thd;      //advance:增益平滑阈值
    float gain_smooth_width;    //advance:增益平滑宽度
    float SNR_s_Min;        //base: 最小信号，平稳噪声比
    float SNR_ns_Min;       //base: 最小信号，非平稳噪声比
    float psd_Min;
    float pf_Gcn;           //base: CNG(舒适噪声)舒适噪声量
    float rse_maxG;         //base: 残留回声最大量
    int32_t     rse_thd;        //advance: 连续单讲帧数
    int32_t     ref_thd;    //base: 初始化参考帧数
    float dt_overest;       //base: 双拼状态残留回声过估计参数
    float init_overest;     //base: 初始化残留回声过估计参数
    /* vad prms */
    /* vad inc time in ms */
    int16_t     vadinc_time;    //advance:VAD从非活动到活动的时间常数
    /* vad dec time in ms */
    int16_t     vaddec_time;    //advance:VAD从活动到非活动的时间常数
    /* vad noise psd min */
    int16_t     vadNmin;        //advance:噪声功率最小值
    /* vad voice psd min */
    int16_t     vadVmin;        //advance:语音功率最小值
} aenc_rtprm_t;

typedef struct {
    float *hr;
    int32_t *pL;
} aenc_irprm_t;

typedef struct {
    void *work;
    void (*async_work_trigger)(void *work,
        void (*work_func)(void *params), void *params);
    void (*async_work_wait_finished)(void *work);
} aenc_asyncwork_params_t;

void *aenc_open(aenc_initprm_t * init_prms);
int aenc_blockproc(void *enc, int16_t * rxin, int16_t * txin, int16_t * rxout,
                    int16_t * txout);
int aenc_blockproc_ex(void *enc, int16_t * rxin, int16_t * txin, int16_t * rxout,
                    int16_t * txout, int32_t step);
int aenc_cmd(void *enc, uint32_t cmd, uint32_t cmd_prm);
void aenc_close(void *enc);

enum aenc_cmd_ex_e {
    SLTK_CMDEX_PP_AENC_RXGAIN,
    SLTK_CMDEX_PP_AENC_TXGAIN,
};

int aenc_check_ir(float* ir, int N);

#endif
