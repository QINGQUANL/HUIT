//#define LOG_LEV 4
#include "sys_inc.h"
#include "middleware.h"
#include "mode_manager.h"
#include "sleep.h"
#include "amixer.h"

enum {
    AFC_CMD_SET_NHS_RTPRM,
    AFC_CMD_SET_AFC_RTPRM,
};
typedef struct{
        /* ======= 初始化参数 ======= */
    /* 采样率，单位：Hz */
    uint32_t fs;
    /* 帧长，每次处理的样本点数*/
    uint16_t N;
    /* FFT长度，必须满足B >= 2*N, B越大，频域分辨率越好，但是计算量也会越大*/
    uint16_t B;
    /* 声学反馈路径的建模长度（帧数），即声学反馈路径建模总体对应的时间为 N*P/fs 秒 */
    uint16_t P;     //10
    /* 大块纯延时的长度（帧数），即算法内部会自行补偿这部分大段的纯延时，而不会用滤波器来建模，对应的时间为 N*O/fs 秒*/
    uint16_t O;     //30
    /* 用于内部音频预测的滤波器阶数，建议保持默认值 */
    uint16_t Q;     //20
    /* 更新内部音频滤波器的时间周期，单位：秒，建议保持默认值*/
    uint16_t L;     //6
    /* 建模的声学反馈路径的零响应部分长度（帧数），一般来自于内部 FIFO、DMA BUffer等带来的延时，建议保持默认值*/
    uint16_t PO;    //1

    //nhs
    /* 啸叫检测输入的样本数据小数位数，建议保持默认值 */
    int8_t Qbits;
    /* 最大可能的啸叫频点个数 */
    int8_t maxNp;
    /* 最大可能的陷波频点个数 */
    int8_t maxNf;
    /* 
        最大啸叫检测确认时间，单位ms。如果某个频点要作为啸叫频点，那么至少要持续thist毫秒都被认为是候选的啸叫频点 
        参考实时参数里面 thist参数的说明
    */
    uint16_t maxthist;
    /* 内部降采样比例，用于减少计算量，但是太大的话会降低啸叫检测的作用频宽，建议保持默认值 */
    int16_t Rdec;
}afc_params_t;

typedef enum {
    AFC_NONE = 0,
    AFC_ADAPTIVE = 1,
    //AFC_ADAPTIVE_NONE = 2,
    AFC_PROCESS = 3,
    AFC_PROCESS_NONE = 4,
}mic_afc_mode_e;

void *am_afc_open(afc_params_t afc_params);
void am_afc_proc(void* handle, int16_t* rxin, int16_t* txin, int16_t* txout);
void am_afc_close(void *handle);
void am_afc_mode_switch(void *handle, mic_afc_mode_e mode);
int8_t get_afc_mode();
void set_afc_params(int16_t param);
int16_t get_afc_params();
/*------------------------------------------------------------------------------
* 函数功能: 发送实时cmd
* 函数名称: am_afc_cmd
* 参    数: 
*        handle:句柄
*        cmd:命令(AFC_CMD_SET_NHS_RTPRM/AFC_CMD_SET_AFC_RTPRM);陷波/afc发送实时cmd
*        arg为结构体地址
* 例    子：
*       陷波：
*       hd_rtprms_t rtprm; rtprm.Tpapr = 15; rtprm.Tdrop = 12;rtprm.Tphpr = 26;
*       rtprm.Tpnpr = 3; rtprm.Tptpr = -60; rtprm.Att = -15; rtprm.Gdstep = -1;
*       rtprm.Gfloor = -25; rtprm.Nf = 10; rtprm.Np = 10; rtprm.taes = 200;
*       rtprm.tav = 2000; rtprm.tdes = 200; rtprm.tdrop = 100; rtprm.tent = 200;
*       rtprm.thist = 200; rtprm.thold = 100; rtprm.Trel = -10;rtprm.Ttone = -.5;
*       rtprm.tsmooth = 30;am_afc_cmd(am->afc_manager_handle, AFC_CMD_SET_NHS_RTPRM, (uint32_t)&rtprm);
*       AFC:       
*       hpemafc_rtprm_t rtprm; rtprm.beta = (float)1; rtprm.u1 = (float).006; rtprm.u2 = (float).3;
*       rtprm.uH = (float).015;rtprm.uL = (float).001;rtprm.uoH = (float)0.063;//0.015;
*       rtprm.uoL = (float)0.001;rtprm.gamma = (float).15;rtprm.minPsd = -60;rtprm.minH = -40;
*       rtprm.fc = 150;rtprm.func_mask =  HPEMAFC_MASK_HBD | HPEMAFC_MASK_PEM | HPEMAFC_MASK_SC | HPEMAFC_MASK_IPVSS |
*            HPEMAFC_MASK_RPVSS | HPEMAFC_MASK_LPF | HPEMAFC_MASK_LTP;
*       am_afc_cmd(am->afc_manager_handle, AFC_CMD_SET_AFC_RTPRM, (uint32_t)&rtprm);
* 返 回 值: 无
* 说    明：hpemafc_rtprm_t在sltk_hpemafc.h中；hd_rtprms_t sltk_howling_detect.h
*-------------------------------------------------------------------------------*/
void am_afc_cmd(void *handle,uint32_t cmd,uint32_t arg);