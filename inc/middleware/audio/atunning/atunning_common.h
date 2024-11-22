#ifndef ATUNNING_COMMON_H
#define ATUNNING_COMMON_H

#include "sys_inc.h"

#define COMMON_PARAS_MASK(para) ( 1 << para )
#define COMMON_PARAS_MASK_EXCLUDE(para) (~( 1 << para ))

typedef struct {
    uint16_t size;
    uint16_t offset;
}data_typesize_and_offset;

typedef struct{
    uint8_t   cmd_type;
    uint8_t   para_idx;
    //data_type dt;
}cmd_type2para_idx;

typedef struct{
    uint8_t   cmd_type;
    uint8_t   para_idx[2];
}cmd_type2doublepara_idx;

/*
    Smartlink - eq controller v2.0.6-beta-0.7.1控制器
    控制命令全指令

    一个eq指令唤作一个block
    一个block有8个字节
    一次至多接受64个block
*/
#define BLOCK_SIZE          (8)
#define MAX_BLOCK_PER_PAGE  (64)
/*
    格式解析
    offset        name               desc
        ----------------------------------------
*/
#define PAGE_ID_OFFSET      0
typedef uint32_t PageClass;
#define NEXT_PAGE(p) ( p + 1 )
#define THIZ_PAGE(p) ( p )

#define PAGE_RELOAD_EQ    0x000000FA
#define PAGE_RESET        0x000000FE
#define PAGE_RESET_WEAK   0x000000FD

#define PAGE_NONE    0xFFFFFFFF
#define PAGE_START   (0)

#define PAGE_MUSIC   THIZ_PAGE(PAGE_START)
#define PAGE_EFFECT  NEXT_PAGE(PAGE_MUSIC)
#define PAGE_MIC     NEXT_PAGE(PAGE_EFFECT)
#define PAGE_OUTPUT1 NEXT_PAGE(PAGE_MIC)
#define PAGE_OUTPUT2 NEXT_PAGE(PAGE_OUTPUT1)
#define PAGE_VSS     NEXT_PAGE(PAGE_OUTPUT2)
#define PAGE_HW      NEXT_PAGE(PAGE_VSS)
#define PAGE_PLG     NEXT_PAGE(PAGE_HW)

#define PAGE_END     THIZ_PAGE(PAGE_PLG)
#define PAGE_MAX     8

#define PAGE_ANC_FF     (51)
#define PAGE_ANC_FB     (52)
#define PAGE_ANC_COMP   (53)
#define PAGE_HT_FF      (54)
#define PAGE_HT_FB      (55)
#define PAGE_HT_COMP    (56)

//Check page num...
#if ( PAGE_END >= PAGE_MAX )
#error "pageID overflow..."
#endif
/*
        0    |    pageId      |   页面标识
        ----------------------------------------
*/
#define TYPE_OFFSET         1
/*
        1    |    type        |   指令类型，见TunningTypeClass
        ----------------------------------------
*/
#define ITEM_INDEX_OFFSET   2
/*
        2    |    itemIndex   |   特定type的sub子项id，如eq的band id
        ----------------------------------------
*/
#define RESERVE_AREA_OFFSET 3
/*
        3    |    reserve     |   待定
        ----------------------------------------
*/
#define DATA_OFFSET         4
/*
        4    |                |
        5    |    data        |   数据区域，4个字节，可数据，也可以放作函数指针，依具体情况而定
        6    |                |
        7    |                |
        ----------------------------------------
*/
typedef void (*user_tunning_callback)(void* cookie, uint8_t arg);

typedef union _eq_data_type {
    user_tunning_callback  clbk;
    float                  data;//float
    uint32_t               ctx;
    BOOL                   bool_swicth;
}eq_data_type;

typedef struct _eq_data {
    uint8_t      page_id;   //e.g. PAGE_MUSIC
    uint8_t      item_type; //参考@TunningTypeClass, 表示调的是什么参数，例如EQ中的某段的Q值
    uint8_t      item_index; //例如:EQ中表示调的是哪一段的参数
    uint8_t      reserve;
    eq_data_type fill;      //参数值
}eq_data;

#define GET_F32_VAL(ptr)    (*((float*)(ptr)))
#define GET_S16_FOR_F32(f)  ((int16_t)((int32_t)f))
#define GET_U16_FOR_F32(f)  ((uint16_t)((int32_t)f))
#define GET_U32_FOR_F32(f)  ((uint32_t)f)
#define GET_S32_FOR_F32(f)  ((int32_t)f)
#define GET_S8_FOR_F32(f)   ((int8_t)((int32_t)f))
#define GET_U8_FOR_F32(f)   ((int8_t)((int32_t)f))

//定义上位机发下来的各种调节命令
typedef enum {
    //Total start
    TUNNING_RELOAD_RAW_EQ = 0xFA,
    TUNNING_TYPE_REVERB_ECHO_SWITCH = 0xFB,
    TUNNING_TYPE_BAND_STATE_SWITCH = 0xFC,
    TUNNING_TYPE_RESET = 0xFE,

    TUNNING_TYPE_START = 0,

    //EQ调节相关命令
    TUNNING_TYPE_EQ_START = TUNNING_TYPE_START,
    //Eq start
    TUNNING_TYPE_FREQ = TUNNING_TYPE_EQ_START,
    TUNNING_TYPE_Q,
    TUNNING_TYPE_GAIN,
    TUNNING_TYPE_FILTER,
    TUNNING_TYPE_EQ_SETTING_ALL,
    TUNNING_TYPE_EQ_PRE_SETTING,
    //Eq end
    TUNNING_TYPE_EQ_END = TUNNING_TYPE_EQ_PRE_SETTING,

    e_EQTOOL_AUDIO_DAC_GAIN = 6,	//6	,		和底层代码对应
	e_EQTOOL_AUDIO_LPGA_GAIN,	//7
	e_EQTOOL_AUDIO_RPGA_GAIN,	//8
	e_EQTOOL_AUDIO_XPGA_GAIN,	//9
	e_EQTOOL_AUDIO_AMIC0_DIGITAL_GAIN,	//10
	e_EQTOOL_AUDIO_AMIC1_DIGITAL_GAIN,	//11
	e_EQTOOL_AUDIO_AMIC2_DIGITAL_GAIN,	//12
	e_EQTOOL_AUDIO_AMIC0_ANALOG_GAIN,	//13
	e_EQTOOL_AUDIO_AMIC1_ANALOG_GAIN,	//14
	e_EQTOOL_AUDIO_AMIC2_ANALOG_GAIN,	//15

	e_EQTOOL_AUDIO_LINEIN0_ANALOG,		//16
	e_EQTOOL_AUDIO_LINEIN1_ANALOG,		//17
	e_EQTOOL_AUDIO_AUX0_ANALOG_GAIN,    //18
	e_EQTOOL_AUDIO_AUX1_ANALOG_GAIN,	//19
	e_EQTOOL_AUDIO_AUX2_ANALOG_GAIN,	//20

	//混音的音量控制
	e_EQTOOL_MIXVOLUME_SPDIF,	//21
	e_EQTOOL_MIXVOLUME_LOCAL_MIC,	//22
	e_EQTOOL_MIXVOLUME_WIRELESS_MIC,	//23
	e_EQTOOL_MIXVOLUME_USB_AUDIO,	//24
	e_EQTOOL_MIXVOLUME_I2S,			//25
	e_EQTOOL_MIXVOLUME_TONE,		//26
	e_EQTOOL_MIXVOLUME_AUX,			//27

	e_EQTOOL_MODE_G24_PLAY_VOLUME,	//28
	e_EQTOOL_RF_POWER_VAL,			//29

    //变速变调调节相关命令
    //Tsps start
    TUNNING_TYPE_TSPS_START = 30,
    TUNNING_TYPE_TSPS_FC_SWITCH = TUNNING_TYPE_TSPS_START,
    TUNNING_TYPE_MAPPING_TSPS_PARA_SETTING_START,
    TUNNING_TYPE_TSPS_PS = TUNNING_TYPE_MAPPING_TSPS_PARA_SETTING_START,
    TUNNING_TYPE_TSPS_TS,
    TUNNING_TYPE_TSPS_FC,
    TUNNING_TYPE_MAPPING_TSPS_PARA_SETTING_END = TUNNING_TYPE_TSPS_FC,
    TUNNING_TYPE_TSPS_ATUNE_SETTINGS,
    TUNNING_TYPE_TSPS_SETTING_ALL,
    TUNNING_TYPE_TSPS_ATUNE_PRE_SETTING,
    TUNNING_TYPE_FC_SETTING_ALL,
    TUNNING_TYPE_FC_PRE_SETTING,
    TUNNING_TYPE_TSPS_END = TUNNING_TYPE_FC_PRE_SETTING,
    //Tsps end

    //DRC调节相关命令
    //Drc start
    TUNNING_TYPE_DRC_START = 60,
    TUNNING_TYPE_DRC_MAKEUP_GAIN = TUNNING_TYPE_DRC_START,
    TUNNING_TYPE_DRC_SUB_PROCESSOR_SWITCH,
    TUNNING_TYPE_MAPPING_DRC_PARA_SETTING_START,
    TUNNING_TYPE_DRC_SUB_PROCESSOR_THRESHOLD = TUNNING_TYPE_MAPPING_DRC_PARA_SETTING_START,
    TUNNING_TYPE_DRC_SUB_PROCESSOR_ATTACK_TIME,
    TUNNING_TYPE_DRC_SUB_PROCESSOR_RATIO,
    TUNNING_TYPE_DRC_SUB_PROCESSOR_RELEASE_TIME,
    TUNNING_TYPE_MAPPING_DRC_PARA_SETTING_END = TUNNING_TYPE_DRC_SUB_PROCESSOR_RELEASE_TIME,
    TUNNING_TYPE_DRC_SETTING_ALL,
    TUNNING_TYPE_DRC_PRE_SETTING,
    //Drc end
    TUNNING_TYPE_DRC_END = TUNNING_TYPE_DRC_PRE_SETTING,

    //混响调节相关命令
    //Reverb start
    TUNNING_TYPE_REVERB_START = 90,
    TUNNING_TYPE_REVERB_SWITCH = TUNNING_TYPE_REVERB_START, //混响总开关
    TUNNING_TYPE_REVERB_PREDLY,                             //混响预延时
    TUNNING_TYPE_REVERB_TIME,                               //混响时间
    TUNNING_TYPE_REVERB_DIFFUSION,                          //扩散系数
    TUNNING_TYPE_REVERB_G_WET,                              //混响(湿声)音量
    TUNNING_TYPE_REVERB_G_DRY,                              //直达(干声)音量
    TUNNING_TYPE_REVERB_DAMPING_FREQ_LO,                    //低频阻尼频点
    TUNNING_TYPE_REVERB_DAMPING_RATIO_LO,                   //低频阻尼
    TUNNING_TYPE_REVERB_DAMPING_FREQ_HI,                    //高频阻尼频点
    TUNNING_TYPE_REVERB_DAMPING_RATIO_HI,                   //高频阻尼
    TUNNING_TYPE_REVERB_LP_FREQ,                            //低通滤波截止频率
    TUNNING_TYPE_REVERB_HP_FREQ,                            //高通滤波截止频率
    TUNNING_TYPE_REVERB_REC_MS_SWITCH,                      //单双通道模式切换
    TUNNING_TYPE_REVERB_SETTING_ALL,
    TUNNING_TYPE_REVERB_PRE_SETTING,
    //Reverb end
    TUNNING_TYPE_REVERB_END = TUNNING_TYPE_REVERB_PRE_SETTING,

    //echo(回声)调节相关命令
    //Echo start
    TUNNING_TYPE_ECHO_START = 120,
    TUNNING_TYPE_ECHO_SWITCH = TUNNING_TYPE_ECHO_START, //回声总开关
    TUNNING_TYPE_ECHO_LP,                               //低通滤波
    TUNNING_TYPE_ECHO_G_DRY,                            //回声直达音量
    TUNNING_TYPE_ECHO_G_WET,                            //回声混响音量
    TUNNING_TYPE_ECHO_LP_FREQ,                          //低通频率
    TUNNING_TYPE_ECHO_DELAY,                            //回声延时
    TUNNING_TYPE_ECHO_FEEDBACK_COFF,                    //回声重复率
    TUNNING_TYPE_ECHO_SETTING_ALL,
    TUNNING_TYPE_ECHO_PRE_SETTING,
    //Echo end
    TUNNING_TYPE_ECHO_END = TUNNING_TYPE_ECHO_PRE_SETTING,

    //DR(去相关)算法调节相关命令
    TUNNING_TYPE_DR_START = 150,
    TUNNING_TYPE_DR_SWITCH = TUNNING_TYPE_DR_START,
    TUNNING_TYPE_DR_DLY,
    TUNNING_TYPE_DR_REVERSE,
    TUNNING_TYPE_DR_SETTING_ALL,
    TUNNING_TYPE_DR_PRE_SETTING,
    TUNNING_TYPE_DR_END = TUNNING_TYPE_DR_PRE_SETTING,

    //虚拟环绕声（3D)调节相关命令
    TUNNING_TYPE_VSS_START = 160,
    TUNNING_TYPE_VSS_HP_SPK_SWITCH = TUNNING_TYPE_VSS_START,//HP default,0:耳机模式  1:扬声器模式
    TUNNING_TYPE_VSS_SYNTHESIS_WIDTH,                       //综合宽度0-7
    TUNNING_TYPE_VSS_SPAN_WIDTH,                            //扩展宽度0-7
    TUNNING_TYPE_VSS_DEPTH_WIDTH,                           //空间深度
    TUNNING_TYPE_VSS_SPATIAL_INTENSITY,                     //空间强度0-10
    TUNNING_TYPE_VSS_BASS_INTENSITY,                        //低音强度0-10
    TUNNING_TYPE_VSS_DEFINITION,                            //解析度0-10
    TUNNING_TYPE_VSS_BASS_COFREQ,                           //低音频点20-200Hz
    TUNNING_TYPE_VSS_SWITCH,                                //3D开关
    TUNNING_TYPE_VSS_PRE_SETTING,
    TUNNING_TYPE_VSS_END = TUNNING_TYPE_VSS_PRE_SETTING,

    TUNNING_TYPE_PLG_SETTING_START = 190,
    //MIC调节相关命令
    TUNNING_TYPE_PLG_AMICX_SETTING_START = TUNNING_TYPE_PLG_SETTING_START,
    TUNNING_TYPE_PLG_AMIC0_GAIN = TUNNING_TYPE_PLG_AMICX_SETTING_START,
    TUNNING_TYPE_PLG_AMIC1_GAIN,
    TUNNING_TYPE_PLG_AMIC2_GAIN,
    TUNNING_TYPE_PLG_AMIC0_ENABLE,
    TUNNING_TYPE_PLG_AMIC1_ENABLE,
    TUNNING_TYPE_PLG_AMIC2_ENABLE,
    TUNNING_TYPE_PLG_AMICX_SETTING_END = TUNNING_TYPE_PLG_AMIC2_ENABLE,

    //FS啸叫预制调节相关命令
    TUNNING_TYPE_PLG_FS_SETTING_START,
    TUNNING_TYPE_PLG_FS_SETTING_DELTAFREQ = TUNNING_TYPE_PLG_FS_SETTING_START,
    TUNNING_TYPE_PLG_FS_SETTING_END = TUNNING_TYPE_PLG_FS_SETTING_DELTAFREQ,

    //NHS啸叫预制调节相关命令
    TUNNING_TYPE_PLG_MIC_NHS_SETTING_START = 197,
    TUNNING_TYPE_PLG_MIC_NHS_NP = TUNNING_TYPE_PLG_MIC_NHS_SETTING_START,
    TUNNING_TYPE_PLG_MIC_NHS_NF = 198,
    TUNNING_TYPE_PLG_MIC_NHS_THD_PTPR = 199,
    TUNNING_TYPE_PLG_MIC_NHS_THD_PAPR = 200,
    TUNNING_TYPE_PLG_MIC_NHS_THD_PHPR = 201,
    TUNNING_TYPE_PLG_MIC_NHS_THD_PNPR = 202,
    TUNNING_TYPE_PLG_MIC_NHS_THD_DROP = 203,
    TUNNING_TYPE_PLG_MIC_NHS_ATT = 204,
    TUNNING_TYPE_PLG_MIC_NHS_TIME_DROP = 205,
    TUNNING_TYPE_PLG_MIC_NHS_TIME_HISTORY = 206,
    TUNNING_TYPE_PLG_MIC_NHS_TIME_HOLD = 207,
    TUNNING_TYPE_PLG_MIC_NHS_THD_REL = 208,
    TUNNING_TYPE_PLG_MIC_NHS_THD_SINGLE_TONE = 209,
    TUNNING_TYPE_PLG_MIC_NHS_GAIN_DSTEP = 210,
    TUNNING_TYPE_PLG_MIC_NHS_GAIN_FLOOR_DECREASE = 211,
    TUNNING_TYPE_PLG_MIC_NHS_TIME_AVG_CONST = 212,
    TUNNING_TYPE_PLG_MIC_NHS_TIME_DESCEND = 213,
    TUNNING_TYPE_PLG_MIC_NHS_TIME_AESEND = 214,
    TUNNING_TYPE_PLG_MIC_NHS_TIME_ENTER = 215,
    TUNNING_TYPE_PLG_MIC_NHS_TIME_COFF_SMOOTH = 216,
    TUNNING_TYPE_PLG_MIC_NHS_SETTING_END = TUNNING_TYPE_PLG_MIC_NHS_TIME_COFF_SMOOTH,
    TUNNING_TYPE_PLG_SETTING_END = TUNNING_TYPE_PLG_MIC_NHS_SETTING_END,

    //input调节相关命令
    TUNNING_TYPE_INPUT_START  = 220,
    TUNNING_TYPE_INPUT_SOURCE = TUNNING_TYPE_INPUT_START, //输入源
    TUNNING_TYPE_INPUT_VOL,                               //输入音量

    //分频器调节相关命令
    TUNNING_TYPE_DIVIDER_HLP_START,
    TUNNING_TYPE_DIVIDER_HP_FRE = TUNNING_TYPE_DIVIDER_HLP_START, //分频器高通频率
    TUNNING_TYPE_DIVEDER_HP_TYPE,                                 //分频器高通类型

    TUNNING_TYPE_DIVIDER_LP_FRE,    //分频器低通频率
    TUNNING_TYPE_DIVEDER_LP_TYPE,   //分频器低通类型
    TUNNING_TYPE_DIVIDER_HLP_END = TUNNING_TYPE_DIVEDER_LP_TYPE,

    TUNNING_TYPE_INPUT_END = TUNNING_TYPE_DIVIDER_HLP_END,

    //高低通drc调节相关命令
    TUNNING_TYPE_HLDRC_START = 230,
    TUNNING_TYPE_HLDRC_SWITCH = TUNNING_TYPE_HLDRC_START,
    TUNNING_TYPE_HLDRC_FC,
    TUNNING_TYPE_LDRC_MAKEUP_GAIN,
    TUNNING_TYPE_LDRC_SUB_PROCESSOR_START,
    TUNNING_TYPE_LDRC_SUB_PROCESSOR_THRESHOLD = TUNNING_TYPE_LDRC_SUB_PROCESSOR_START,
    TUNNING_TYPE_LDRC_SUB_PROCESSOR_ATTACK_TIME,
    TUNNING_TYPE_LDRC_SUB_PROCESSOR_RATIO,
    TUNNING_TYPE_LDRC_SUB_PROCESSOR_RELEASE_TIME,
    TUNNING_TYPE_LDRC_SUB_PROCESSOR_END = TUNNING_TYPE_LDRC_SUB_PROCESSOR_RELEASE_TIME,
    TUNNING_TYPE_HDRC_MAKEUP_GAIN,
    TUNNING_TYPE_HDRC_SUB_PROCESSOR_START,
    TUNNING_TYPE_HDRC_SUB_PROCESSOR_THRESHOLD = TUNNING_TYPE_HDRC_SUB_PROCESSOR_START,
    TUNNING_TYPE_HDRC_SUB_PROCESSOR_ATTACK_TIME,
    TUNNING_TYPE_HDRC_SUB_PROCESSOR_RATIO,
    TUNNING_TYPE_HDRC_SUB_PROCESSOR_RELEASE_TIME,
    TUNNING_TYPE_HDRC_SUB_PROCESSOR_END = TUNNING_TYPE_HDRC_SUB_PROCESSOR_RELEASE_TIME,
    TUNNING_TYPE_HLDRC_END = TUNNING_TYPE_HDRC_SUB_PROCESSOR_END,
    

    //Total end
    TUNNING_TYPE_END = TUNNING_TYPE_HLDRC_END,
}TunningTypeClass;

typedef enum {
    TUNNING_SET_NO_ERR = 0,
    TUNNING_SET_INVALID_STU = 0x80000000,
    TUNNING_SET_PATCH_WELCOME = 0x7FFF0000,
    TUNNING_XET_NONE_RESPONSE
}TUNNING_SETTING_STU;

#define TUNNING_SET_DATA true
#define TUNNING_GET_DATA false

#endif