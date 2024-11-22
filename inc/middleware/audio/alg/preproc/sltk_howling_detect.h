#ifndef _HOWLING_DETECT_H_
#define _HOWLING_DETECT_H_

#include "sltk_preproc_common.h"

typedef struct {
    /* 采样率，单位：Hz */
    uint16_t fs;
    /* 帧长，每次处理的样本点数（暂不支持修改）*/
    uint16_t L;
    /* 进行FFT变换的大小 （暂不支持修改）*/
    int16_t M;
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
} hd_initprms_t;

typedef struct
{
    /* 实时的最大啸叫频点数，应小于初始化参数中的MaxNp */
    int8_t Np;
    /* 实时的最大陷波频点数，应小于初始化参数中的MaxNf */
    int8_t Nf;
    /* 啸叫频点最小的绝对功率(dBFS)，频点的绝对功率必须大于此值，否则不会被认为是啸叫频点 */
    int8_t Tptpr;
    /* 啸叫频点相对所有频带平均功率的最小比（dB), 频点对于所有频带的平均功率比值必须大于此值，否则不会被认为是啸叫频点 */
    int8_t Tpapr;
    /* 
        啸叫频点相对其谐波的功率的最小比(dB),频点(中高频, > 1KHz)相对于其谐波的功率比值必须大于此值，否则不会被认为是啸叫频点
        对于中低频信号(<1KHz),还需参考下面的Att参数
     */
    int8_t Tphpr;
    /* 啸叫频点相对相邻频点的功率最小比（dB），频点相对其相邻频点的功率比值必须大于此值，否则不会被认为是啸叫频点 */
    int8_t Tpnpr;
    /* 
        判断啸叫频点的下降幅度的最小值，当某个频点被认为可能是啸叫频点时，会尝试性对频点做陷波，
        在tdrop时间后检查频点功率是否至少下降了Tdrop,如果满足此条件，则不会被认为是啸叫频点，陷波也会释放 
    */
    int8_t Tdrop;
    /* 
        Tphpr对中高频信号(>1KHz)的放松值（dB）,频点(中低频, < 1KHz)相对于其谐波的功率比值必须Tphpr+Att，否则不会被认为是啸叫频点
        对于中高频信号(>1KHz),还需参考上面的TPhpr参数
     */
    int8_t Att;
    /* 检查可能啸叫频点的衰减，当频点被认为可能是啸叫频点起，tdrop时间后会检查衰减是否达到Tdrop，参考上面Tdrop的参数描述 */
    uint16_t tdrop;
    /* 
        啸叫检测确认时间，单位ms。如果某个频点要作为啸叫频点，那么至少要持续thist毫秒都被认为是候选的啸叫频点 
        thist应小于初始化参数中 maxthist
     */
    uint16_t thist;
    /* 
        检查可能啸叫频点的衰减保持时间，参考上述Tdrop及tdrop参数描述，在满足Tdrop及tdrop的条件后，
        啸叫频点的衰减至少还应保持thold时间，否则不会被认为是啸叫频点 
    */
    uint16_t thold;
    /* 
        陷波点释放阈值(dB)，如果当前啸叫点(包括候选啸叫点)的平均功率对比上次啸叫发生时的平均功率小于Treal,则会释放当前的陷波器
     */
    int8_t Trel;
    /* 单频判断阈值(dB), 如果当前频点的功率与信号总功率的比值超过了Ttone，则会被认为可能的啸叫频点 */
    float Ttone;
    /* 增益衰减/增加的步长(dB), 
        如果当前所有陷波器已经被占用，并且又有新的啸叫点被检测到,并且已经持续了tdes时间（见下面tdes参数说明），则总增益会按Gdstep步长衰减 
        如果当前有陷波器空闲，并且已经持续了超过taes（见下面taes参数说明）时间，总增益会按Gdstep步长增加
    */
    int8_t Gdstep;
    /* 增益最小值(dB), 总增益的最小值，小于或等于此值的增益将不会再被衰减  */
    int8_t Gfloor;
    /* 平均功率的平滑时间(ms) */
    uint16_t tav;
    /* 增益衰减前的持续时间，见上面Gdstep参数说明 */
    uint16_t tdes;
    /* 增益放大的补偿(dB) */
    uint16_t taes;
    /*
    频点判断为可能的啸叫频点的时间（ms)
        当上述Tdrop参数的判断排除为啸叫频点的情况下，会再判断频点被判定为可疑啸叫点的次数，如果连续判定为啸叫点的时间短于这个值，则会被认为不是啸叫点
     */
    uint16_t tent;
    /* 陷波器系数切换的平滑时间（ms） */
    uint16_t tsmooth;
}hd_rtprms_t;


typedef struct 
{
    /* gain in dB */
    float G;
    /* center frequency */
    float fc;
    /* state, 0: init, 1: trial, 2: determined */
    int8_t state;
}hd_sec_t;


typedef struct 
{
    /* valid output? */
    int8_t valid;
    /* occupied nocth filter number */
    int16_t NT;
    /* global gain */
    float g;
    /* howling, need allocated externally  */
    hd_sec_t* hd_state;
}hd_output_t;



void *hd_open(hd_initprms_t * prms);
int hd_proc(void *handle, int16_t * in, hd_output_t * output);
int hd_cmd(void *handle, uint32_t cmd, uint32_t arg);
void hd_close(void *handle);

void hd_fed_playback(void* handle, int16_t* in);
#endif