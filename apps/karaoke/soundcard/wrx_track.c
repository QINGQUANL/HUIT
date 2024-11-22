/*****************************************************************************
* 无线MIC通路相关代码
****************************************************************************/
#define LOG_LEV 4
#include "amixer.h"
#include "audio_in.h"
#include "audio_out.h"
#include "middleware.h"
#include "amsg.h"
#include "aeffect_component.h"
#include "audio_service.h"
#include "volume_control.h"
#include "arec_client.h"
#include "sltk_sbc_dec.h"
#include "sltk_sbc_enc.h"
#include "sltk_plc.h"
#include "sltk_math.h"
#include "sltk_lc3.h"
#include "g24_api.h"
#include "wrx_track.h"
#include "user_config.h"
#include "hal_mbox.h"
#include "amixer_funcs.h"
#include "sltk_interp.h"
#include "id_worker.h"

#define MIN(a,b)    ((a) < (b) ? (a) : (b))
#define MAX(a,b)    ((a) > (b) ? (a) : (b))

#define WMIC_TRACK_FS    (48000)
#define WMIC_TRACK_CH    (2)

//#define USE_NEW_INTERP	//优化插值处理, 会更耗ram和cpu, 按需求开关
#ifdef USE_NEW_INTERP
#define INTERP_TYPE_ID_WORKER       0    //id_worker插值
#define INTERP_TYPE_SLTK_INTERP     1    //sltk_interp插值
#define INTERP_TYPE_SLTK_INTERP2    2    //sltk_interp2插值
#define INTERP_TYPE_SEL             INTERP_TYPE_SLTK_INTERP2
#endif


#define WRX_USE_CPU1

#define INTERP_FRAME_SCALE (4) //插值帧长减小因子（节约ram)

#define LC3_FRAME_MS            ((float)2.5)
#define LC3_FRAME_SAMPLES       (120)

#define WRX_USE_PLC             1
#define WRX_DEL_SMALL_SIGNAL    0

#if (1 == LC3_DEC_CH)
//1ch
#if 0
#define LC3_BS_LEN              (40)
#define LC3_BITRATE             (128000)
#endif

//长度和码率不可随意更改，需保证底层cmd和afh有足够的空间放下
#if G24_DUAL_SHORT_PACKET_3TIMES
#define LC3_BS_LEN              (25)
#define LC3_BITRATE             (80000)
#elif G24_DUAL_AUDIO_LONG_PACKET_4TIMES
#define LC3_BS_LEN              (31)
#define LC3_BITRATE             (100000)

#elif G24_MONITOR_EN && (G24_MONITOR_AUDIO_CH == 1)

#if G24_FUNC_ROLE           //dongle
#define LC3_BS_LEN              (23)        //(3 + 23) * 3 + afh 11 + 1 res = 90
#define LC3_BITRATE             (73600)
#else                       //handset
#define LC3_BS_LEN              (25)        //(3 + 25) * 3 + 6 (afh * 1/3) = 90
#define LC3_BITRATE             (80000)
#endif


#elif G24_MONITOR_EN && (G24_FUNC_ROLE == 1)    //DONGLE decode

//(36 + 3) * 2 +11 = 89
//36 * 1000/2.5*8 = 115200
#define LC3_BS_LEN              (36)
#define LC3_BITRATE             (115200)

#else
#define LC3_BS_LEN              (39)        //31
#define LC3_BITRATE             (124800)    //100000
//#define LC3_BS_LEN              (31)
//#define LC3_BITRATE             (100000)
#endif

#else   //stereo // (1 == LC3_DEC_CH)      
//2ch
//(3 + 80) * 1 + 6 (afh * 1/3) = 89
#define LC3_BS_LEN              (80)//(62)
#define LC3_BITRATE             (256000)//(200000)

#endif

#if G24_DUAL_AUDIO_LONG_PACKET_4TIMES
#define SAVE_BSBUF_CNT                  (12)

#elif G24_MONITOR_EN && (G24_MONITOR_AUDIO_CH == 1)
#define SAVE_BSBUF_CNT                  (6)

#elif G24_MONITOR_EN && (G24_FUNC_ROLE == 1)
#define SAVE_BSBUF_CNT                  (12)
#else
#define SAVE_BSBUF_CNT                  (6)
#endif
#define SAVE_BSBUF_SIZE                 90
#define BS_WAIT_TH_FOR_UNDERRUN         (2)
#define BS_WAIT_TH_FOR_NORMAL           (1)


#if !G24_DUAL_CHANNEL //参数待定 ： 1T1立体声 & 1拖多双向监听


#if G24_MONITOR_EN && (G24_MONITOR_AUDIO_CH == 1)
#define LOW_TH      (7500+500) //低数据量阈值（单位:us)
#define HIGH_TH     (7500+900) //高数据量阈值（单位:us)
#define NORMAL_TH   (7500+700) //正常数据量阈值（单位:us)

#elif G24_MONITOR_EN && (G24_FUNC_ROLE == 1)  //dongle
#define LOW_TH      (5000+500) //低数据量阈值（单位:us)
#define HIGH_TH     (5000+900) //高数据量阈值（单位:us)
#define NORMAL_TH   (5000+700) //正常数据量阈值（单位:us)

#else
#define LOW_TH      (2500+500) //低数据量阈值（单位:us)
#define HIGH_TH     (2500+900) //高数据量阈值（单位:us)
#define NORMAL_TH   (2500+700) //正常数据量阈值（单位:us)
#endif

#elif G24_DUAL_SHORT_PACKET_3TIMES
#define LOW_TH      (8000) //低数据量阈值（单位:us)
#define HIGH_TH     (8400) //高数据量阈值（单位:us)
#define NORMAL_TH   (8200) //正常数据量阈值（单位:us)
#elif G24_DUAL_AUDIO_LONG_PACKET_4TIMES

#if 0
#define LOW_TH      (20500) //低数据量阈值（单位:us)
#define HIGH_TH     (20900) //高数据量阈值（单位:us)
#define NORMAL_TH   (20700) //正常数据量阈值（单位:us)
#endif

#define LOW_TH      (20200) //低数据量阈值（单位:us)
#define HIGH_TH     (20600) //高数据量阈值（单位:us)
#define NORMAL_TH   (20400) //正常数据量阈值（单位:us)

#else

#define LOW_TH      (5000+500) //低数据量阈值（单位:us)
#define HIGH_TH     (5000+900) //高数据量阈值（单位:us)
#define NORMAL_TH   (5000+700) //正常数据量阈值（单位:us)

#endif

typedef struct {
    uint8_t buf[SAVE_BSBUF_CNT][SAVE_BSBUF_SIZE];
    uint8_t valid[SAVE_BSBUF_CNT];
    uint32_t packet_timestamp[SAVE_BSBUF_CNT];
    volatile uint8_t rpos;
    volatile uint8_t wpos;
} save_bsbuf_t;

typedef struct {
    void *wrx_play_track_handle;   //用于播放
    void *wrx_drc_handle;          //无线mic drc算法句柄

    //解码接口
    wrx_decoder_plugin_t decoder_plugin;

    wrx_debug_t dbg;

    save_bsbuf_t *bsbuf_ctrl;       //bitstream数据缓存器(包含码流数据，丢包标志，当前读写位置等信息)
    uint32_t bs_wait_th;            //bitstream读阈值，当bitstream buffer中的包数大于等于bs_wait_th时才可以从中读出数据
    stream_info_t info;             //解码信息
    void *decoder;                  //解码器句柄
    uint32_t dec_proc_samples;      //解码一帧输出多少个采样点数据
    void *plc_tmpbuf;               //PLC补包buffer
    void *plc;                      //丢包补偿
    int16_t *outbuf[2];             //当前帧解码（或者补包）输出buffer
    uint32_t outbuf_res;             //输出buffer剩余的数据点数

    void *cpux_handle;
    OAL_THREAD task_handle;
    float vol_db;                   //当前的音量（dB)
    uint8_t muted;                  //0-不静音; 1-静音
    uint8_t ch;                     //声道数
    volatile uint8_t busy;
    uint8_t decode_type;            //解码类型 0:sbc;1:lc3
    uint8_t packet_type;            //数据包的类型(仅lc3解码需要) 0:正常包 1:非正常包
    uint32_t last_timestamp;
    uint32_t underrun_flag;

#ifdef USE_NEW_INTERP
    void * interp_handle; //新插值算法句柄
    int16_t *interpbuf; //新插值算法输入输出buffer
    int16_t *interpoutbuf;
    uint32_t last_interp_t;
#endif
    int32_t sum_delta_t; //缓存数据量累加值（单位：微秒)
    int32_t sum_cnt; //累加次数
    uint32_t h_th_cnt; //连续正常有包的个数
    int32_t adjust_sta; //表示当前应如何调节数据
    int32_t interp_adjust_sta; //插值状态(加/减/正常)
    int32_t interp_proc_cnt; //插值处理点数计数
    int32_t avg; //平均数据量
    int32_t delta_interp_t_th;
    int16_t hist[2]; //历史数据
    uint8_t use_interp_adjust; //0:使用pll调节; 1:使用interp调节

    uint8_t link; //链路ID
} wrx_track_t;


//获取bs buffer有多少可写空间
AT_G24_SRAM(.wrx_bsbuf_r_avail)
uint32_t wrx_bsbuf_r_avail(void *handle)
{
    wrx_track_t *wrx = handle;

    int32_t avail = wrx->bsbuf_ctrl->wpos - wrx->bsbuf_ctrl->rpos;

    if (avail < 0) {
        avail += (SAVE_BSBUF_CNT * 2);
    } else if (avail >= (SAVE_BSBUF_CNT * 2)) {
        avail -= (SAVE_BSBUF_CNT * 2);
    }

    return (uint32_t)avail;
}

AT_G24_SRAM(.put_g24_bs_to_wrx_track)
void put_g24_bs_to_wrx_track(void *handle, uint8_t *bs, uint32_t bytes, bool valid)
{
    wrx_track_t *wrx = handle;
    //存bs数据
    uint32_t index = 0;

    if (NULL == wrx) {
        return;
    }

    if (valid) {
        wrx->dbg.debug_wrx_normal_cnt++;
    } else {
        wrx->dbg.debug_wrx_loss_cnt++;
    }

    if (SAVE_BSBUF_CNT - wrx_bsbuf_r_avail(handle)) {
        index = wrx->bsbuf_ctrl->wpos % SAVE_BSBUF_CNT;
        if (valid) {
            memcpy(wrx->bsbuf_ctrl->buf[index], bs, bytes);
        }
        wrx->bsbuf_ctrl->valid[index] = valid;
        wrx->bsbuf_ctrl->packet_timestamp[index] = HAL_timestamp_to_us(HAL_get_timestamp());
        wrx->bsbuf_ctrl->wpos = (wrx->bsbuf_ctrl->wpos + 1) % (SAVE_BSBUF_CNT * 2);
    } else {
        logx("o228\n");
        wrx->dbg.debug_wrx_overrun_cnt++;
    }
}

void wrx_sbc_decoder_close(void *handle)
{
}

void wrx_lc3_decoder_close(void *handle)
{
}

void wrx_sbc_decoder_open(void *handle)
{
    wrx_track_t *wrx = handle;

    wrx->info.fs = WMIC_TRACK_FS;
    wrx->info.channels = wrx->ch;
    wrx->info.init_data = 0;
    wrx->info.chunk_size = 77;
#if G24_DUAL_CHANNEL
    if(!G24_DUAL_CHANNEL_UL_LONG_PKT) {
        wrx->info.chunk_size = 37;
    }
#endif

#if 0//G24_SBC_SEND_4TIMES
    if (2 == wrx->ch) {
        wrx->info.chunk_size = 37;
    } else {
        wrx->info.chunk_size = 38;
    }
#endif
    g24_rx_packet_size_set(wrx->info.chunk_size);

    //SBC解码每帧输出128个采样点
    wrx->dec_proc_samples = 128;
    logd("dec_proc_samples:%d", wrx->dec_proc_samples);

    //bitstream缓存器分配
    wrx->bsbuf_ctrl = general_calloc(sizeof(save_bsbuf_t));
    if (NULL == wrx->bsbuf_ctrl) {
        loge("wrx->bsbuf_ctrl malloc fail");
    }
    wrx->bs_wait_th = BS_WAIT_TH_FOR_UNDERRUN;
    //SBC解码器初始化
    wrx->decoder = sbc_decoder_open((sltk_codec_init_t*)&wrx->info);
    if (NULL == wrx->decoder) {
        loge("decoder open fail");
    }
    //解码输出buffer
    wrx->plc_tmpbuf = general_calloc(sizeof(int16_t) * wrx->ch  * wrx->dec_proc_samples);
}

void wrx_lc3_decoder_open(void *handle)
{
    wrx_track_t *wrx = handle;

    wrx->info.fs = WMIC_TRACK_FS;
    wrx->info.channels = LC3_DEC_CH;
    wrx->info.init_data = 0;
    wrx->info.chunk_size = LC3_BS_LEN;
    g24_rx_packet_size_set(wrx->info.chunk_size);

    //bitstream缓存器分配
    wrx->bsbuf_ctrl = general_calloc(sizeof(save_bsbuf_t));
    if (NULL == wrx->bsbuf_ctrl) {
        loge("wrx->bsbuf_ctrl malloc fail");
    }
    wrx->bs_wait_th = BS_WAIT_TH_FOR_UNDERRUN;

    //LC3解码器初始化
    int32_t plc_mode = 0;
    int32_t hr_mode = 0;
    float frame_ms = (float)LC3_FRAME_MS;
    wrx->decoder = lc3_dec_open(wrx->info.fs, wrx->info.channels, plc_mode, hr_mode, frame_ms);
    if (NULL == wrx->decoder) {
        loge("decoder open fail");
    }

    //lc3解码每帧输出120个采样点
    wrx->dec_proc_samples = LC3_FRAME_SAMPLES;
    logd("dec_proc_samples:%d", wrx->dec_proc_samples);
    //解码输出buffer
    wrx->plc_tmpbuf = general_calloc(sizeof(int16_t) * wrx->ch  * wrx->dec_proc_samples);

#ifdef WRX_USE_CPU1
    wrx->cpux_handle = OAL_cpux_work_create(1, 2, 256);
#endif
}

AT_G24_SRAM(.wrx_sbc_decode)
int32_t wrx_sbc_decode(void *handle, uint8_t *bs, int16_t **pcmbuf)
{
    wrx_track_t *wrx = handle;
    int ret = 0;
    uint32_t samples;

    sbc_get_stream_info(bs, &wrx->info);
    //SBC解码
    ret = sbc_frame_decode(wrx->decoder, (int16_t*)bs,
        wrx->info.chunk_size, &pcmbuf[0], &pcmbuf[1], (uint32_t*)&samples);
    return ret;
}

AT_G24_SRAM(.wrx_lc3_decode)
int32_t wrx_lc3_decode(void *handle, uint8_t *bs, int16_t **pcmbuf)
{
    wrx_track_t *wrx = handle;
    __maybe_unused int16_t* output16[2];
    uint32_t samples;
    int ret = 0;

    output16[0] = wrx->plc_tmpbuf;
    output16[1] = (int16_t*)wrx->plc_tmpbuf + wrx->dec_proc_samples;

    //OAL_enter_critical_section();
    //uint32_t start_t = HAL_timestamp_to_us(HAL_get_timestamp());
    if (wrx->packet_type) {
        //logd("非正常包");
        samples = lc3_dec_framedecode(wrx->decoder, bs, 0, output16);
        wrx->packet_type = 0;
    } else {
        samples = lc3_dec_framedecode(wrx->decoder, bs, wrx->info.chunk_size, output16);
        //logd("正常");
    }

    //samples = 120;
    //uint32_t end_t = HAL_timestamp_to_us(HAL_get_timestamp());
    //OAL_exit_critical_section();
    //logx("{t:%d}", end_t-start_t);

    if(samples <= 0){
        ret = -1;
    } else {
        ret = 0;
        pcmbuf[0] = output16[0];
        pcmbuf[1] = output16[1];
#if WRX_DEL_SMALL_SIGNAL
        for(uint32_t i=0; i<wrx->dec_proc_samples; i++) {
            if((pcmbuf[0][i]<=2) && (pcmbuf[0][i]>=-2)) {
                pcmbuf[0][i] = 0;
            }
        }
#endif
#if (1 == LC3_DEC_CH)
        //1ch时，令ch1的数据和ch0一致
        memcpy(pcmbuf[1], pcmbuf[0], wrx->dec_proc_samples * sizeof(int16_t));
#else
#if WRX_DEL_SMALL_SIGNAL
        for(uint32_t i=0; i<wrx->dec_proc_samples; i++) {
            if((pcmbuf[1][i]<=2) && (pcmbuf[1][i]>=-2)) {
                pcmbuf[1][i] = 0;
            }
        }
#endif
#endif
    }

    return ret;
}


const wrx_decoder_plugin_t wrx_sbc_plugin = {
    .open = wrx_sbc_decoder_open,
    .decode = wrx_sbc_decode,
    .close = wrx_sbc_decoder_close,
};

const wrx_decoder_plugin_t wrx_lc3_plugin = {
    .open = wrx_lc3_decoder_open,
    .decode = wrx_lc3_decode,
    .close = wrx_lc3_decoder_close,
};

uint16_t wrx_track_bs_len_get(void)
{
#if (0 == AUDIO_G24_RX_DECODER_SEL)
    uint16_t len = 77;
#if G24_DUAL_CHANNEL
    if(!G24_DUAL_CHANNEL_UL_LONG_PKT) {
        len = 37;
    }
#endif
    return len;
#else
    return LC3_BS_LEN;
#endif
}

void wrx_decoder_open(void *handle)
{
    wrx_track_t *wrx = handle;

    if (wrx->decoder_plugin.open) {
        wrx->decoder_plugin.open(wrx);
    }
}

void wrx_plc_open(void *handle)
{
    wrx_track_t *wrx = handle;
    plc_prms_t plc_prm;
    plc_prm.fs = WMIC_TRACK_FS;
    plc_prm.Fsize = wrx->dec_proc_samples;
    plc_prm.channels = wrx->ch;
    plc_prm.need_dly = 1;
    wrx->plc = sltkplc_open(&plc_prm);
    if (NULL == wrx->plc) {
        loge("wrx->plc open fail");
    }
}

AT_G24_SRAM(.wrx_bitstream_decode)
void wrx_bitstream_decode(void *handle)
{
    wrx_track_t *wrx = handle;
    uint32_t index;
    bool valid = false;
    int16_t *pcmbuf[2];

    __maybe_unused int32_t cur_r_avail = wrx_bsbuf_r_avail(handle);
    __maybe_unused uint32_t cur_t = HAL_timestamp_to_us(HAL_get_timestamp());
    __maybe_unused uint32_t delta_t = cur_t - wrx->bsbuf_ctrl->packet_timestamp[wrx->bsbuf_ctrl->rpos % SAVE_BSBUF_CNT];

#if G24_DUAL_CHANNEL//1 || G24_DUAL_SHORT_PACKET_3TIMES || G24_DUAL_AUDIO_LONG_PACKET_4TIMES
    uint16_t trans_priod;
#if G24_MONITOR_EN && (G24_FUNC_ROLE == 1)
    trans_priod = 5000;
#elif G24_DUAL_SHORT_PACKET_3TIMES
    trans_priod = 7500;
#elif G24_DUAL_AUDIO_LONG_PACKET_4TIMES
    trans_priod = 20000;
#else
    trans_priod = 5000;     //1T2 2次
#endif

    if (0 == wrx->underrun_flag) {
        if (0 == cur_r_avail) {
            delta_t = cur_t - wrx->last_timestamp;
        }
        delta_t = cur_r_avail * 2500 + delta_t;
        wrx->sum_cnt++;
        wrx->sum_delta_t += delta_t;
        if (wrx->sum_cnt >= 100) {
            uint32_t avg = wrx->sum_delta_t/wrx->sum_cnt;
            wrx->avg = avg;
            //logi("{WRX %d}", avg); //打印平均数据量
            if (0 == wrx->use_interp_adjust) {
                //调pll方式
                if (avg < LOW_TH) {
                    uint32_t pll_freq = 24576000 * (1.0f-0.0003f);
                    if (HAL_get_audiopll_rate() != pll_freq) {
                        HAL_adjust_audiopll(pll_freq);
                        logd("slow");
                    }
                } else if (avg > HIGH_TH) {
                    uint32_t pll_freq = 24576000 * (1.0f+0.0003f);
                    if (HAL_get_audiopll_rate() != pll_freq) {
                        HAL_adjust_audiopll(pll_freq);
                        logd("fast");
                    }
                } else {
                    uint32_t pll_freq = 24576000;
                    if (HAL_get_audiopll_rate() > pll_freq) {
                        if (avg <= NORMAL_TH) {
                            HAL_adjust_audiopll(pll_freq);
                            logd("normal");
                        }
                    } else if (HAL_get_audiopll_rate() < pll_freq) {
                        if (avg >= NORMAL_TH) {
                            HAL_adjust_audiopll(pll_freq);
                            logd("normal");
                        }
                    } else {
                        if (HAL_get_audiopll_rate() != pll_freq) {
                            HAL_adjust_audiopll(pll_freq);
                            logd("normal");
                        }
                    }
                }
            } else {
                //插值方式
                if (avg < LOW_TH) {
                    int32_t adjust_sta = -1;
                    if (wrx->adjust_sta != adjust_sta) {
                        wrx->adjust_sta = adjust_sta;
                        logd("slow");
                    }
                } else if (avg > HIGH_TH) {
                    int32_t adjust_sta = 1;
                    if (wrx->adjust_sta != adjust_sta) {
                        wrx->adjust_sta = adjust_sta;
                        logd("fast");
                    }
                } else {
                    int32_t adjust_sta = 0;
                    if (wrx->adjust_sta > adjust_sta) {
                        if (avg <= NORMAL_TH) {
                            wrx->adjust_sta = adjust_sta;
                            logd("normal");
                        }
                    } else if (wrx->adjust_sta < adjust_sta) {
                        if (avg >= NORMAL_TH) {
                            wrx->adjust_sta = adjust_sta;
                            logd("normal");
                        }
                    } else {
                        if (wrx->adjust_sta != adjust_sta) {
                            wrx->adjust_sta = adjust_sta;
                            logd("normal");
                        }
                    }
                }
            }

            wrx->sum_cnt = 0;
            wrx->sum_delta_t = 0;
        }
    }
#endif

#if G24_DUAL_CHANNEL//1 || G24_DUAL_SHORT_PACKET_3TIMES || G24_DUAL_AUDIO_LONG_PACKET_4TIMES
    if (0 == wrx->underrun_flag) {
        if (cur_r_avail) {
            if (wrx->h_th_cnt > 20) {
            } else {
                wrx->h_th_cnt++;
            }
        } else {
            if (0 == cur_r_avail) {
                //logx("W");
                uint32_t start_t = HAL_timestamp_to_us(HAL_get_timestamp());
                uint32_t cur_t = start_t;
                uint32_t wait_th;
                if (wrx->h_th_cnt > 20) {
                    wait_th = trans_priod + 1200;
                } else {
                    wait_th = trans_priod + 200;
                }
                wrx->h_th_cnt = 0;
                do {
                    if (0 == get_cpu_id()) {
                        OAL_sleep_ticks(1);
                    } else {
                        ;
                    }
                    if (wrx_bsbuf_r_avail(handle)) {
                        break;
                    }
                    cur_t = HAL_timestamp_to_us(HAL_get_timestamp());
                } while (cur_t -  wrx->last_timestamp < wait_th && cur_t - start_t < 1500);
                //logx("[t %d,%d]", cur_t -  wrx->last_timestamp, cur_t - start_t );
            }
        }
    }
#endif

    if (wrx_bsbuf_r_avail(handle)) {
        uint32_t index0 = (wrx->bsbuf_ctrl->rpos) % SAVE_BSBUF_CNT;
        wrx->last_timestamp = wrx->bsbuf_ctrl->packet_timestamp[index0];
    }

    if (wrx_bsbuf_r_avail(handle) >= wrx->bs_wait_th) {
        wrx->underrun_flag = 0;
        wrx->bs_wait_th = BS_WAIT_TH_FOR_NORMAL;
        index = wrx->bsbuf_ctrl->rpos % SAVE_BSBUF_CNT;
        valid = wrx->bsbuf_ctrl->valid[index];
        if (!valid && wrx->decode_type) {
#if !WRX_USE_PLC
            wrx->packet_type = 1;
#endif
        }

#if WRX_USE_PLC
		if (!valid)
#else
        if (!valid && !wrx->decode_type) 
#endif    
        {
            //更新bitstream读位置
            wrx->bsbuf_ctrl->rpos = (wrx->bsbuf_ctrl->rpos + 1) % (SAVE_BSBUF_CNT * 2);
            //ch0数据地址
            pcmbuf[0] = wrx->plc_tmpbuf;
            if(2 == wrx->ch) {
                //ch1数据地址
                pcmbuf[1] = (int16_t*)wrx->plc_tmpbuf + wrx->dec_proc_samples;
            }
            //补包处理
            if (wrx->plc) {
                sltkplc_conceal2(wrx->plc, pcmbuf);
            } else {
                memset(wrx->plc_tmpbuf, 0, wrx->dec_proc_samples * wrx->ch * sizeof(int16_t));
            }
            //logx("L");
        } else {
            //正常包解码
            int32_t ret = 0;
            if (wrx->decoder_plugin.decode) {
                ret = wrx->decoder_plugin.decode(wrx, wrx->bsbuf_ctrl->buf[index], pcmbuf);
            }
            //更新bitstream读位置
            wrx->bsbuf_ctrl->rpos = (wrx->bsbuf_ctrl->rpos + 1) % (SAVE_BSBUF_CNT * 2);
            if (ret != 0) {
                logw("bad sbc");
                wrx->dbg.debug_wrx_bad_cnt++;
                //解码失败则走丢包补偿流程
                //ch0数据地址
                pcmbuf[0] = wrx->plc_tmpbuf;
                if(2 == wrx->ch) {
                    //ch1数据地址
                    pcmbuf[1] = (int16_t*)wrx->plc_tmpbuf + wrx->dec_proc_samples;
                }
                //补包处理
                if (wrx->plc) {
                    sltkplc_conceal2(wrx->plc, pcmbuf);
                } else {
                    memset(wrx->plc_tmpbuf, 0, wrx->dec_proc_samples * wrx->ch * sizeof(int16_t));
                }
            } else {
                //丢包处理模块需要记录正常包数据信息
                if (wrx->plc) {
                    sltkplc_addhist(wrx->plc, pcmbuf);
                }
            }
        }
    } else {
        //bitstream buffer空，则重新设置wrx->bs_wait_th,避免bitstre_buffer一直处于饥饿临界状态
        if (0 == wrx->underrun_flag) {
            logx("###u\n");
        }
        wrx->dbg.debug_wrx_underrun_cnt++;

#if G24_DUAL_CHANNEL//1 || G24_DUAL_SHORT_PACKET_3TIMES || G24_DUAL_AUDIO_LONG_PACKET_4TIMES
        wrx->last_timestamp = HAL_timestamp_to_us(HAL_get_timestamp()) - trans_priod*2;
#endif
        wrx->bs_wait_th = BS_WAIT_TH_FOR_UNDERRUN;
        wrx->underrun_flag = 1;

#if !WRX_USE_PLC
        if(wrx->decode_type) {
            wrx->packet_type = 1;
            if (wrx->decoder_plugin.decode) {
                wrx->decoder_plugin.decode(wrx, wrx->bsbuf_ctrl->buf[0], pcmbuf);
            }
        } else 
#endif
        {
            //走丢包处理流程
            //ch0数据地址
            pcmbuf[0] = wrx->plc_tmpbuf;
            if(2 == wrx->ch) {
                //ch1数据地址
                pcmbuf[1] = (int16_t*)wrx->plc_tmpbuf + wrx->dec_proc_samples;
            }
            //补包处理
            if (wrx->plc) {
                sltkplc_conceal2(wrx->plc, pcmbuf);
            } else {
                memset(wrx->plc_tmpbuf, 0, wrx->dec_proc_samples * wrx->ch * sizeof(int16_t));
            }
        }
    }

    wrx->outbuf[0] = pcmbuf[0];
    if(2 == wrx->ch) {
        wrx->outbuf[1] = pcmbuf[1];
    }
    wrx->outbuf_res = wrx->dec_proc_samples;
}

#ifdef USE_NEW_INTERP
AT_G24_SRAM(.wrx_interp_old_proc)
void wrx_interp_new_proc(void *handle)
{
    const void *src[2];
    void *in, *out;
    int32_t insamples, outsamples;
    float alpha = (float)1.0;
    aoutinfo_t info;
    wrx_track_t *wrx = handle;
    int32_t interp_frame_len = wrx->outbuf_res / INTERP_FRAME_SCALE;

    wrx->interp_adjust_sta = wrx->adjust_sta;

    for (int i = 0; i < INTERP_FRAME_SCALE; i++) {
        src[0] = wrx->outbuf[0] + i * interp_frame_len;
        src[1] = wrx->outbuf[1] + i * interp_frame_len;
        __16_to_interleave_stereo16(wrx->interpbuf, src, interp_frame_len);

        if (wrx->interp_adjust_sta < 0) {
            //+
            if (HAL_timestamp_to_ms(HAL_get_timestamp()) - wrx->last_interp_t > wrx->delta_interp_t_th ||
                wrx->avg < LOW_TH - (NORMAL_TH - LOW_TH) / 2) {
                alpha = (float)1.0 - (float)0.0001;
            }
        } else if (wrx->interp_adjust_sta > 0) {
            //-
            if (HAL_timestamp_to_ms(HAL_get_timestamp()) - wrx->last_interp_t > wrx->delta_interp_t_th ||
                wrx->avg > HIGH_TH + (HIGH_TH - NORMAL_TH) / 2) {
                alpha = (float)1.0 + (float)0.0001;
            }
        } else {
            //normal
            alpha = (float)1.0;
        }

        in = wrx->interpbuf;
        out = wrx->interpbuf;
        insamples = interp_frame_len;
        outsamples = 0;
#if (INTERP_TYPE_SEL == INTERP_TYPE_SLTK_INTERP || INTERP_TYPE_SEL == INTERP_TYPE_SLTK_INTERP2)
#if (INTERP_TYPE_SEL == INTERP_TYPE_SLTK_INTERP)
        sltk_interp_proc(wrx->interp_handle, &in, &insamples, &out, &outsamples, alpha);
#else
        sltk_interp_proc2(wrx->interp_handle, &in, &insamples, &out, &outsamples, alpha);
#endif
#else
		out = wrx->interpoutbuf;
        id_worker_interp_get_period(wrx->interp_handle, &in, &insamples, &out, &outsamples, alpha);
        memcpy(wrx->interpbuf, wrx->interpoutbuf, sizeof(int16_t)*wrx->ch*outsamples);
#endif
        if (interp_frame_len != outsamples) {
            //logd("{rx interp %d -> %d}", interp_frame_len, outsamples);
            int32_t delta_val = interp_frame_len - outsamples;
            if (delta_val > 5 || delta_val < -5) {
                wrx->delta_interp_t_th = 5000;
            } else {
                wrx->delta_interp_t_th = 1000;
            }
            wrx->last_interp_t = HAL_timestamp_to_ms(HAL_get_timestamp());
        }

        info.pcm[0] = wrx->interpbuf;
        info.channels = 2;
        info.samples = outsamples;
        info.sample_bits = 16;
        audio_track_write(wrx->wrx_play_track_handle, &info, true);
    }
}

#else

AT_G24_SRAM(.wrx_interp_old_proc)
void wrx_interp_proc(void *handle)
{
    aoutinfo_t info;
    wrx_track_t *wrx = handle;

    __maybe_unused int32_t new_adjust_sta = wrx->adjust_sta;
    __maybe_unused int32_t total_cnt = (((1024*4 + wrx->outbuf_res - 1)/wrx->outbuf_res) * wrx->outbuf_res);
    __maybe_unused float adjust_speed = (float)1 / total_cnt;
    if ((wrx->interp_proc_cnt && wrx->interp_adjust_sta < 0) ||
        (!wrx->interp_proc_cnt && new_adjust_sta < 0)) {
        if (!wrx->interp_proc_cnt) {
            //start slow
            wrx->interp_adjust_sta = new_adjust_sta;
        }

        int16_t h[2];
        int16_t tmp_h[2];
        h[0] = wrx->hist[0];
        h[1] = wrx->hist[1];
        for (int n = 0; n < wrx->outbuf_res; n++) {
            wrx->interp_proc_cnt++;
            float s = wrx->interp_proc_cnt*adjust_speed;

            tmp_h[0] = wrx->outbuf[0][n];
            tmp_h[1] = wrx->outbuf[1][n];
            float a, b;
            a = h[0] << 16;
            b = wrx->outbuf[0][n] << 16;
            wrx->outbuf[0][n] = ((int32_t)(a * s + b * (1.0f - s))) >> 16;
            a = h[1] << 16;
            b = wrx->outbuf[1][n] << 16;
            wrx->outbuf[1][n] = ((int32_t)(a * s + b * (1.0f - s))) >> 16;

            h[0] = tmp_h[0];
            h[1] = tmp_h[1];
        }
        wrx->hist[0] = h[0];
        wrx->hist[1] = h[1];
        info.pcm[0] = wrx->outbuf[0];
        info.pcm[1] = wrx->outbuf[1];
        info.channels = 2;
        info.samples = wrx->outbuf_res;
        info.sample_bits = 16;
        audio_track_write(wrx->wrx_play_track_handle, &info, false);

        if (wrx->interp_proc_cnt >= total_cnt) {
            wrx->interp_proc_cnt = 0;

            info.pcm[0] = &wrx->hist[0];
            info.pcm[1] = &wrx->hist[1];
            info.channels = 2;
            info.samples = 1;
            info.sample_bits = 16;
            audio_track_write(wrx->wrx_play_track_handle, &info, false);
        }
    } else if ((wrx->interp_proc_cnt && wrx->interp_adjust_sta > 0) ||
                (!wrx->interp_proc_cnt && new_adjust_sta > 0)) {
        int32_t samples;
        int16_t *src16_ch0 = wrx->outbuf[0];
        int16_t *src16_ch1 = wrx->outbuf[1];
        if (!wrx->interp_proc_cnt) {
            //start fast
            wrx->interp_adjust_sta = new_adjust_sta;

            wrx->hist[0] = wrx->outbuf[0][0];
            wrx->hist[1] = wrx->outbuf[1][0];
            src16_ch0++;
            src16_ch1++;
            samples = wrx->outbuf_res - 1;
        } else {
            samples = wrx->outbuf_res;
        }

        int16_t h0, h1;
        int16_t tmp0, tmp1;
        h0 = wrx->hist[0];
        h1 = wrx->hist[1];
        for (int n = 0; n < samples; n++) {
            wrx->interp_proc_cnt++;
            float s = wrx->interp_proc_cnt*adjust_speed;
            tmp0 = src16_ch0[n];
            tmp1 = src16_ch1[n];
            float a, b;
            a = h0 << 16;
            b = src16_ch0[n] << 16;
            src16_ch0[n] = ((int32_t)(a * (1.0f - s) + b * s)) >> 16;
            a = h1 << 16;
            b = src16_ch1[n] << 16;
            src16_ch1[n] = ((int32_t)(a * (1.0f - s) + b * s)) >> 16;
            h0 = tmp0;
            h1 = tmp1;
        }
        wrx->hist[0] = h0;
        wrx->hist[1] = h1;
        if (wrx->interp_proc_cnt >= total_cnt-1) {
            wrx->interp_proc_cnt = 0;
        }

        info.pcm[0] = src16_ch0;
        info.pcm[1] = src16_ch1;
        info.channels = 2;
        info.samples = samples;
        info.sample_bits = 16;
        audio_track_write(wrx->wrx_play_track_handle, &info, false);
    } else {
        wrx->interp_adjust_sta = new_adjust_sta;
        wrx->hist[0] = wrx->outbuf[0][wrx->outbuf_res-1];
        wrx->hist[1] = wrx->outbuf[1][wrx->outbuf_res-1];

        info.pcm[0] = wrx->outbuf[0];
        info.pcm[1] = wrx->outbuf[1];
        info.channels = 2;
        info.samples = wrx->outbuf_res;
        info.sample_bits = 16;
        audio_track_write(wrx->wrx_play_track_handle, &info, false);
    }
}
#endif

AT_G24_SRAM(.wrx_dec_loop)
static void *wrx_dec_loop(void *handle)
{
    aoutinfo_t info;
    wrx_track_t *wrx = handle;
    logd("wrx dec loop");

    while (wrx) {
        if (wrx->busy) {
            if (wrx->cpux_handle) {
                OAL_cpux_work_request(wrx->cpux_handle, wrx_bitstream_decode, handle);
            } else {
                wrx_bitstream_decode(handle);
            }

            if (0 == wrx->use_interp_adjust) {
                //调pll方式
                info.pcm[0] = wrx->outbuf[0];
                info.pcm[1] = wrx->outbuf[1];
                info.channels = 2;
                info.samples = wrx->outbuf_res;
                info.sample_bits = 16;
                audio_track_write(wrx->wrx_play_track_handle, &info, false);
            } else {
                //插值方式
#ifdef USE_NEW_INTERP
                wrx_interp_new_proc(handle);
#else
                wrx_interp_proc(handle);
#endif
            }

            wrx->outbuf_res = 0;
        } else {
            OAL_sleep(1);
        }
    }

    return NULL;
}

void wrx_track_destory(void *handle)
{
    wrx_track_t *wrx = handle;

    if (NULL == wrx) {
        return;
    }

    OAL_free(wrx);
}

void *wrx_track_create(wrx_track_init_params_t *prm)
{
    wrx_track_t *wrx;
    audio_track_init_t track_init;

    wrx = OAL_calloc(1, sizeof(*wrx));
    if (NULL == wrx) {
        return NULL;
    }

    wrx->ch = WMIC_TRACK_CH;

    wrx->decoder_plugin = prm->plugin;
    wrx->decode_type = AUDIO_G24_RX_DECODER_SEL;
    wrx->packet_type = 0;
    wrx->link = prm->link;
    wrx->use_interp_adjust = (0 == wrx->link) ? 0 : 1;
    wrx->delta_interp_t_th = 1000;

    wrx_decoder_open(wrx);
#if WRX_USE_PLC
    wrx_plc_open(wrx);
#endif

#ifdef USE_NEW_INTERP
    if (wrx->use_interp_adjust) {
#if (INTERP_TYPE_SEL == INTERP_TYPE_SLTK_INTERP || INTERP_TYPE_SEL == INTERP_TYPE_SLTK_INTERP2)
        sltk_interp_init interp_init;
        interp_init.ch = wrx->ch;
        interp_init.sample_size = sizeof(int16_t);
        interp_init.period = wrx->dec_proc_samples / INTERP_FRAME_SCALE;
#if (INTERP_TYPE_SEL == INTERP_TYPE_SLTK_INTERP)
        wrx->interp_handle = sltk_interp_open(&interp_init);
#else
        wrx->interp_handle = sltk_interp_open2(&interp_init);
#endif

#else
        id_init idi;
        idi.ch = wrx->ch;
        idi.alpha = 1.0;
        idi.level = SRATE_QUALITY_CUBIC_LVL_1;
        idi.period = wrx->dec_proc_samples / INTERP_FRAME_SCALE + MAX_INTERP_CNT;
        idi.sample_size = sizeof(int16_t);
        wrx->interp_handle = id_worker_init(&idi);
#endif
        if (NULL == wrx->interp_handle) {
            loge("interp open fail");
        }
        wrx->interpbuf = general_malloc(((wrx->dec_proc_samples / INTERP_FRAME_SCALE) + MAX_INTERP_CNT) * wrx->ch * sizeof(int16_t));
        wrx->interpoutbuf = general_malloc(((wrx->dec_proc_samples / INTERP_FRAME_SCALE) + MAX_INTERP_CNT) * wrx->ch * sizeof(int16_t));
        if (NULL == wrx->interpbuf) {
            loge("interpbuf malloc fail");
        }
    }
#endif
    if(prm->link == 0) {
        track_init.mode = AUDIO_MODE_WMIC;
    } else {
        track_init.mode = AUDIO_MODE_WMIC1;
    }
    track_init.fs = WMIC_TRACK_FS;
    track_init.ch = wrx->ch;
    track_init.sample_size = sizeof(int16_t);
#ifdef USE_NEW_INTERP
    track_init.input_interleave = (0 == wrx->use_interp_adjust) ? false : true;
#else
    track_init.input_interleave = false;
#endif
    track_init.fadein_ms = 100;
    track_init.fadeout_ms = 100;
    if (1 == wrx->decode_type) {
        //lc3
        track_init.sharebuf_samples = wrx->dec_proc_samples;
    } else {
        //sbc
        track_init.sharebuf_samples = wrx->dec_proc_samples*2;
    }
    track_init.user_data = wrx;
    track_init.notify    = NULL;
    track_init.pulldata_from_user = 0;
    wrx->wrx_play_track_handle = audio_track_open(&track_init);
    wrx->task_handle = OAL_thread_create(wrx_dec_loop, wrx, "wrx", 9, 768);

    return wrx;
}

void wrx_track_start(void *handle)
{
    wrx_track_t *wrx = handle;

    if (NULL == wrx) {
        return;
    }

    audio_track_cmd(wrx->wrx_play_track_handle, TRACK_CMD_START, 1);
    wrx->busy = 1;
}

wrx_debug_t wrx_debug_info_get(void *handle)
{
    wrx_track_t *wrx = handle;

    if (NULL == wrx) {
        wrx_debug_t dbg = {0};
        return dbg;
    }

    return wrx->dbg;
}

void wrx_mute_set(void *handle, uint8_t mute)
{
    wrx_track_t *wrx = handle;
    float vol = (float)0.0;

    if (NULL == wrx) {
        return;
    }

    wrx->muted = mute;
    if (!mute) {
        vol = volume_db_to_volf32(wrx->vol_db);
    }

    audio_track_cmd(wrx->wrx_play_track_handle, TRACK_CMD_VOLUME_SET, (uint32_t)&vol);
}

uint8_t wrx_mute_get(void *handle)
{
    wrx_track_t *wrx = handle;

    if (NULL == wrx) {
        return 0;
    }

    return wrx->muted;
}

void wrx_volume_set(void *handle, float vol_db)
{
    wrx_track_t *wrx = handle;
    float vol = (float)0.0;

    if (NULL == wrx) {
        return;
    }

    wrx->vol_db = vol_db;
    if (!wrx->muted) {
        vol = volume_db_to_volf32(wrx->vol_db);
    }
    audio_track_cmd(wrx->wrx_play_track_handle, TRACK_CMD_VOLUME_SET, (uint32_t)&vol);
}

float wrx_volume_get(void *handle)
{
    wrx_track_t *wrx = handle;

    if (NULL == wrx) {
        return (float)0.0;
    }

    return wrx->vol_db;
}

uint8_t wrx_ch_get(void *handle)
{
    wrx_track_t *wrx = handle;

    return wrx->ch;
}

AT_G24_SRAM(.wrx_bitstream_decode)
uint8_t wrx_info_ch_get(void *handle)
{
    wrx_track_t *wrx = handle;

    return wrx->info.channels;
}

