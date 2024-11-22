#ifndef _AENC_PARAMS_H_
#define _AENC_PARAMS_H_

#include "sys_inc.h"
#include "sltk_aenc.h"
#include "sltk_admaenc.h"
#include "sltk_admomini.h"
#include "sltk_admomni_aenc.h"
#
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
};

#define PHONE_AGC_MASK_LIMITER      (1)
#define PHONE_AGC_MASK_NOISEGATE    (8)


//降噪参数头
#define SPP_AENC_HEAD           0xC2400000

enum spp_aenc_cmd_e {
    SPP_AENC_CMD_1MIC_PRM     = 0x10,    //单MIC方案基本参数 cmd
    SPP_AENC_CMD_1MIC_EXPRM   = 0x11,    //单MIC方案扩展参数 cmd
    SPP_AENC_CMD_1MIC_SPKEQ   = 0x12,    //单MIC方案喇叭EQ cmd
    SPP_AENC_CMD_1MIC_SPKDRC  = 0x13,    //单MIC方案喇叭DRC cmd
    SPP_AENC_CMD_1MIC_MICEQ   = 0x14,    //单MIC方案MIC EQ cmd
    SPP_AENC_CMD_1MIC_MICDRC  = 0x15,    //单MIC方案MIC DRC cmd
    SPP_AENC_CMD_2MIC_PRM     = 0x20,    //双MIC方案基本参数 cmd
    SPP_AENC_CMD_2MIC_EXPRM   = 0x21,    //双MIC方案扩展参数 cmd
    SPP_AENC_CMD_2MIC_SPKEQ   = 0x22,    //双MIC方案喇叭EQ cmd
    SPP_AENC_CMD_2MIC_SPKDRC  = 0x23,    //双MIC方案喇叭DRC cmd
    SPP_AENC_CMD_2MIC_MICEQ   = 0x24,    //双MIC方案MIC EQ cmd
    SPP_AENC_CMD_2MIC_MICDRC  = 0x25,    //双MIC方案MIC DRC cmd
};

//EQ参数
typedef struct {
    float filter_type;
    float fc;
    float G;
    float Q_factor;
    float disable; //1:表示关闭， 0:表示开启
} aenc_sectprm_t;

// (1 + 5*5) * 4 = 104bytes
typedef struct {
    float PreAmp;//预增益
    aenc_sectprm_t setting[5];//五段EQ参数
} aenc_eq_prm_t;

//DRC参数:10*4 = 40bytes
typedef struct {
    float func_mask; //子模块开关
    float makeup_gain; //补偿增益

    float lt_thd;  //限幅器启动电平
    float lt_ratio; //限幅器压缩比例
    float lt_att; //限幅器启动时间
    float lt_rel; //限幅器释放时间

    float ng_thd;  //噪声门启动电平
    float ng_ratio; //噪声门压缩比例
    float ng_att; //噪声门启动时间
    float ng_rel; //噪声门释放时间
} aenc_drc_prm_t;

//单麦基本参数12*4 = 48bytes
typedef struct {
    float func_mask;//子模块开关
    float pf_GHmin;//最低增益（最大噪声抑制）
    float SNR_s_Min;//最小信号/平稳噪声比
    float SNR_ns_Min;//最小信号/非平稳噪声比
    float ref_thd;//初始参考帧数
    float rse_maxG;//残留回声最大量
    float finit;//噪声初始化的帧数
    float init_overest;//初始状态残留回声过估计参数
    float dt_overest;//双讲状态残留回声过估计参数
    float pf_Xi_opt;//NS（噪声抑制）最优信噪比
    float pf_ph1_thd;//语音检测最大可能
    float pf_Gcn;//CNG（舒适噪声) 舒适噪声量
} aenc_prm_t;

//单麦扩展参数43 * 4 = 172bytes
typedef struct {
    float rse_thd;//连续单讲帧数
    float alpha_slow;//功率谱估计慢速平滑因子
    float alpha_fast;//功率谱估计快速平滑因子
    float Npsd_bias;//噪声估计偏置系数
    float gain_smooth_thd;//增益平滑阈值
    float gain_smooth_width;//增益平滑宽度
    float psd_Min;//最小信号功率
    float pf_alpha_Xis;//信号/平稳噪声比平滑因子
    float pf_alpha_Xins;//信号/非平稳噪声比平滑因子
    float res_overest;//残留回声过估计因子

    float alpha_fast_resG;//残留回声估计快速平滑因子
    float alpha_slow_resG;//残留回声估计慢速平滑因子
    float dt_thd_norm;//双讲检测阈值
    float dt_thd_limit;//双讲检测阈值低门限
    float dt_init_thd;//初始化双讲检测因子
    float dt_steprise_decvar;//双讲检测快速平滑因子
    float dt_stepfall_decvar;//双讲检测慢速平滑因子
    float pf_alpha_npsd;//噪声谱估计平滑因子
    float pf_alpha_PH1;//语音概率估计平滑因子
    float pf_alpha_Nmin_f;//舒适噪声估计平滑因子

    float pf_inc_Nmin_t;//舒适噪声调整因子
    float vadinc_time;//VAD从非活动到活动的时间常数
    float vaddec_time;//VAD从活动到非活动的时间常数
    float vadNmin;//噪声功率最小值
    float vadVmin;//语音功率最小值
    float pf_alpha_z;//混响估计平滑因子
    float pf_decay;//混响衰减因子
    float decay_lthd;//混响衰减阈值
    float alpha_decay;//混响衰减估计平滑因子
    float decay_km;//混响信混比慢速平滑因子

    float rise_km;//混响信混比快速平滑因子
    float thd_km;//混响信混比阈值
    float avq_dec_step;//控制衰减步长
    float avq_reftx;//90dB SPL麦克风信号功率
    float avq_refrx;//90dB SPL喇叭信号功率
    float avq_vadthd;//语音活动检测阈值
    float avq_offset;//控制附加增益
    float avq_Gmax;//自动音量控制最大增益
    float avq_sf_norm;//归一化音量功率
    float avq_alpha_fref;//喇叭信号功率估计快速平滑因子

    float avq_alpha_sref;//喇叭信号功率估计慢速平滑因子
    float avq_alpha_fn;//噪声信号功率估计快速平滑因子
    float avq_alpha_sn;//噪声信号功率估计慢速平滑因子
} aenc_exprm_t;

//双MIC基本参数13*4=52
typedef struct {
    float func_mask;//模块开关
    float ref_thd;//初始参考帧数
    float pf_GHmin;//最小频域增益
    float res_overest;//残留回声过估计参数
    float rse_maxG;//残留回声最大量
    float pf_Xi_opt;//语音大概率存在的信噪比条件
    float pf_Gcn;//舒适噪声量
    float finit;//初始帧数
    float pf_ph1_thd;//语音存在概率上限
    float SNR_s_Min;//最小平稳信噪比
    float SNR_ns_Min;//非平稳信噪比最小值
    float dt_overest;//双讲状态残留回声过估计参数
    float init_overest;//初始状态残留回声过估计参数
} admaenc_prm_t;

//双MIC扩展参数43*4 = 172
typedef struct {
    float rse_thd;//连续单讲帧数
    float alpha_slow;//功率谱慢速平滑系数
    float alpha_fast;//功率谱快速平滑系数
    float dt_thd_norm;//双讲检测判定阈值
    float Npsd_bias;//噪声功率谱估计偏置
    float cab_thd;//双麦最大校准幅度范围
    float uw;//双麦自适应校准步长
    float u;//双麦自适应波束形成步长
    float vad_thd;//VAD判定阈值
    float wind_thd;//风噪判定阈值

    float alpha_xpsd;//自适应校准参考功率谱平滑系数
    float cflow;//自适应校准频率范围下限
    float cfhigh;//自适应校准频率范围上限
    float wflow;//风噪检测频率范围下限
    float wfhigh;//风噪检测频率范围上限
    float lpfws;//慢变自适应校准权重低通频点
    float lpfwf;//快变自适应校准权重低通频点
    float pf_alpha_npsd;//噪声功率谱估计平滑系数
    float pf_alpha_PH1;//语音存在概率平滑系数
    float pf_alpha_Xis;//平稳信噪比平滑系数

    float psd_Min;//最小信号功率谱
    float Rwa;//递归维纳滤波器参数a
    float Rwc;//递归维纳滤波器参数c
    float N_decay;//风噪衰减时间
    float N_hold;//风噪保持时间
    float pf_alpha_Nmin_f;//舒适噪声功率谱频域平滑系数
    float pf_inc_Nmin_t;//舒适噪声递增步长
    float gain_smooth_thd;//增益平滑门限
    float gain_smooth_width;//增益平滑宽度
    float alpha_fast_resG;//残留回声快速平滑参数

    float alpha_slow_resG;//残留回声慢速平滑参数
    float dt_thd_limit;//双讲检测判定阈值下限
    float dt_init_thd;//双讲检测判定阈值初始值
    float dt_steprise_decvar;//双讲检测判定阈值上升步长
    float dt_stepfall_decvar;//双讲检测判定阈值上升步长
    float alpha_Xins;//非平稳信噪比平滑系数
    float vadinc_time;//VAD从非活动到活动的时间常数
    float vaddec_time;//VAD从活动到非活动的时间常数
    float vadNmin;//噪声功率最小值
    float vadVmin;//语音功率最小值

    float d;//MIC的距离(mm)
    float theta;//双MIC连线和嘴的角度
    float mode;//MIC摆放方式
} admaenc_exprm_t;

typedef struct {
    /* concerned freq region low bound */
    float bflow;
    /* concerned freq region upper bound */
    float bfhigh;
    /* beam power smoothing prm */
    float alpha_pb;
    /* beam power decay factor */
    float alpha_decayb;
} admomini_prm_t;

/*------------------------------------------------------------------------------
* 函数功能: 用于将上层调节参数转换成算法内部参数(全向双MIC(无AEC)算法)
* 函数名称: void admtunning_to_admomni_rtprm(admomini_rtprm_t *rtprms, uint32_t N,
                admaenc_prm_t *prm, admaenc_exprm_t *exprm, admomini_prm_t *admomini_prm);
* 参    数:
*           rtprms: 用于返回算法参数
*                N: N为输入参数， 表示帧长
*              prm: prm为输入参数
*            exprm: exprm为输入参数
*     admomini_prm: admomini_prm为输入参数, 如果传入NULL, 则内部自动填入默认参数
* 返 回 值:
* 说明 : 
*-------------------------------------------------------------------------------*/
void admtunning_to_admomni_rtprm(admomini_rtprm_t *rtprms, uint32_t N,
    admaenc_prm_t *prm, admaenc_exprm_t *exprm, admomini_prm_t *admomini_prm);

/*------------------------------------------------------------------------------
* 函数功能: 用于将上层调节参数转换成算法内部参数(全向双MIC(有AEC)算法)
* 函数名称: void tunning_to_admomni_aenc_rtprm(admomni_aenc_rtprm_t *rtprms, uint32_t N,
                admaenc_prm_t *prm, admaenc_exprm_t *exprm, admomini_prm_t *admomini_prm);
* 参    数:
*           rtprms: 用于返回算法参数
*                N: N为输入参数， 表示帧长
*              prm: prm为输入参数
*            exprm: exprm为输入参数
*     admomini_prm: admomini_prm为输入参数, 如果传入NULL, 则内部自动填入默认参数
* 返 回 值:
* 说明 : 
*-------------------------------------------------------------------------------*/
void tunning_to_admomni_aenc_rtprm(admomni_aenc_rtprm_t *rtprms, uint32_t N,
    admaenc_prm_t *prm, admaenc_exprm_t *exprm, admomini_prm_t *admomini_prm);

#endif