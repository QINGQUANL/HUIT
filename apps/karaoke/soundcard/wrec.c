/*****************************************************************************
* 无线录音相关代码
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
#include "sltk_lc3.h"
#include "sltk_plc.h"
#include "sltk_math.h"
#include "g24_api.h"
#include "wrec.h"
#include "user_config.h"
#include "mode_g24.h"
#include "sltk_filter.h"
#include "mode_g24.h"
#include "amixer_funcs.h"
#include "sltk_interp.h"
#include "id_worker.h"

//#define USE_NEW_INTERP		//优化插值处理, 会更耗ram和cpu, 按需求开关
#ifdef USE_NEW_INTERP
#define INTERP_TYPE_ID_WORKER       0    //id_worker插值
#define INTERP_TYPE_SLTK_INTERP     1    //sltk_interp插值
#define INTERP_TYPE_SLTK_INTERP2    2    //sltk_interp2插值
#define INTERP_TYPE_SEL             INTERP_TYPE_SLTK_INTERP2
#endif

#define MIN(a,b)    ((a) < (b) ? (a) : (b))
#define MAX(a,b)    ((a) > (b) ? (a) : (b))

#define INTERP_FRAME_SCALE (4) //插值帧长减小因子（节约ram)

#if AUDIO_G24_RX_DECODER_DISABLE
#define WREC0_USE_CPU1
#define WREC1_USE_CPU1
#else
#if MODE_AFC_EN
#define WREC0_USE_CPU1
#endif
#define WREC1_USE_CPU1
#endif

#if G24_DUAL_CHANNEL //&& !G24_FUNC_ROLE
#if G24_DUAL_AUDIO_LONG_PACKET_4TIMES
#define BSBUF_CNT               (12)
#define BS_WAIT_TH_FOR_UNDERRUN (1)
#define BS_WAIT_TH_FOR_NORMAL   (1)
#else
#define BSBUF_CNT               (8)
#define BS_WAIT_TH_FOR_UNDERRUN (1)
#define BS_WAIT_TH_FOR_NORMAL   (1)
#endif
#else

#if G24_MONITOR_EN && (G24_MONITOR_AUDIO_CH == 1)
#define BSBUF_CNT               (8)

#elif G24_MONITOR_EN && (G24_FUNC_ROLE == 0)
#define BSBUF_CNT               (6)
#else
#define BSBUF_CNT               (3)
#endif
#define BS_WAIT_TH_FOR_UNDERRUN (1)
#define BS_WAIT_TH_FOR_NORMAL   (1)
#endif


#define LC3_FRAME_MS            ((float)2.5)
#define LC3_FRAME_SAMPLES       (120)

#if (1 == LC3_ENC_CH)
#if 0
#define LC3_BS_LEN              (40)
#define LC3_BITRATE             (128000)
#endif

//长度和码率不可随意更改，需保证底层cmd和afh有足够的空间放下
#if G24_DUAL_SHORT_PACKET_3TIMES        // (3 + 25) * 3 + 6 (afh * 1/3) = 90,  2个slot塞入3包数据
#define LC3_BS_LEN              (25)
#define LC3_BITRATE             (80000)
#elif G24_DUAL_AUDIO_LONG_PACKET_4TIMES
#define LC3_BS_LEN              (31)        //31
#define LC3_BITRATE             (100000)    //100000

#elif G24_MONITOR_EN && (G24_MONITOR_AUDIO_CH == 1)

#if G24_FUNC_ROLE           //dongle
#define LC3_BS_LEN              (25)        //(3 + 25) * 3 + 6 (afh * 1/3) = 90
#define LC3_BITRATE             (80000)
#else                       //handset
#define LC3_BS_LEN              (23)        //(3 + 23) * 3 + afh 11 + 1 res = 90
#define LC3_BITRATE             (73600)
#endif

#elif G24_MONITOR_EN && (G24_FUNC_ROLE == 0)    //handset 编码

//(36 + 3) * 2 +11 = 89
//36 * 1000/2.5*8 = 115200
#define LC3_BS_LEN              (36)
#define LC3_BITRATE             (115200)

#else   //2time
/* 
单声道： (3 + 39) * 2 + 6 = 90,     码率39* 1000/2.5ms * 8 = 124800
*/
#define LC3_BS_LEN              (39)        //31
#define LC3_BITRATE             (124800)    //100000

#endif

#else   //stereo

//2ch
//(3 + 80) * 1 + 6 (afh * 1/3) = 89
#define LC3_BS_LEN              (80)//(62)
#define LC3_BITRATE             (256000)//(200000)

#endif  //#if (1 == LC3_ENC_CH)

#define WREC_PCM_INTERP
#define WREC_DC_CUT_EN        0

//---------------------------------------
#if 0
#define LOW_TH      (9500) //低数据量阈值（单位:us)
#define HIGH_TH     (10500) //高数据量阈值（单位:us)
#define NORMAL_TH   (10000) //正常数据量阈值（单位:us)
#endif

#if !G24_DUAL_CHANNEL //参数待定 ： 1T1立体声 & 1拖多双向监听

#if G24_MONITOR_EN && (G24_MONITOR_AUDIO_CH == 1)

#define LOW_TH      (7500+1200) //低数据量阈值（单位:us)
#define HIGH_TH     (7500+2200) //高数据量阈值（单位:us)
#define NORMAL_TH   (7500+1700) //正常数据量阈值（单位:us)

//1拖多双向监听
#elif G24_MONITOR_EN && (G24_FUNC_ROLE == 0)  //handset
#define LOW_TH      (5000+1200) //低数据量阈值（单位:us)
#define HIGH_TH     (5000+2200) //高数据量阈值（单位:us)
#define NORMAL_TH   (5000+1700) //正常数据量阈值（单位:us)

#else
#define LOW_TH      (2500+1200) //低数据量阈值（单位:us)
#define HIGH_TH     (2500+2200) //高数据量阈值（单位:us)
#define NORMAL_TH   (2500+1700) //正常数据量阈值（单位:us)
#endif

#elif G24_DUAL_SHORT_PACKET_3TIMES

#if 0
#define LOW_TH      (8500) //低数据量阈值（单位:us)
#define HIGH_TH     (9500) //高数据量阈值（单位:us)
#define NORMAL_TH   (9000) //正常数据量阈值（单位:us)
#endif

#define LOW_TH      (7500+1200) //低数据量阈值（单位:us)
#define HIGH_TH     (7500+2200) //高数据量阈值（单位:us)
#define NORMAL_TH   (7500+1700) //正常数据量阈值（单位:us)

#elif G24_DUAL_AUDIO_LONG_PACKET_4TIMES
#if 0
#define LOW_TH      (21000) //低数据量阈值（单位:us)
#define HIGH_TH     (22000) //高数据量阈值（单位:us)
#define NORMAL_TH   (21500) //正常数据量阈值（单位:us)
#endif

#define LOW_TH      (20500) //低数据量阈值（单位:us)
#define HIGH_TH     (21500) //高数据量阈值（单位:us)
#define NORMAL_TH   (21000) //正常数据量阈值（单位:us)

#else
//1拖2发两次
#define LOW_TH      (5000+1200) //低数据量阈值（单位:us)
#define HIGH_TH     (5000+2200) //高数据量阈值（单位:us)
#define NORMAL_TH   (5000+1700) //正常数据量阈值（单位:us)
#endif

typedef struct {
    wrec_encoder_plugin_t plugin;
    void *encoder;
    int16_t *enc_pcmbuf0; //pcm ch0 data
    int16_t *enc_pcmbuf1; //pcm ch1 data
    uint8_t *enc_out_buf;

    uint16_t enc_pcm_res_samples;
    uint16_t enc_proc_samples;
    uint8_t enc_ch;

    uint16_t bsbuf_sz;

    struct ring_buffer bsbuf_ctrl[2];
    uint8_t *bsbuf[2];
    uint8_t bsbuf_cnt[2];
    uint32_t wait_th[2];
    struct audio_share_buffer sb;
    int16_t *pcmstore_buf;
    OAL_THREAD task_handle;           //线程句柄
    uint8_t link;

    void *cpux_handle;

#ifdef USE_NEW_INTERP
    void * interp_handle; //新插值算法句柄
    int16_t *interpbuf; //新插值算法输入输出buffer
    int16_t *interpoutbuf; //新插值算法输入输出buffer
    int32_t interp_out_res; //interpbuf中还有多少个点数据没有被读走
    uint32_t last_interp_t;
#endif

    int32_t sum_data_t;
    int32_t sum_cnt;
    int32_t adjust_sta; //表示当前应如何调节数据
    int32_t interp_adjust_sta; //插值状态(加/减/正常)
    int32_t interp_proc_cnt; //插值处理点数计数
    int32_t avg; //平均数据量
    int16_t hist[2]; //历史数据
    uint8_t use_interp_adjust; //0:使用pll调节; 1:使用interp调节
    uint8_t hist_res;

#if WREC_DC_CUT_EN
    void* dc_cut;
#endif
} wrec_t;

uint32_t debug_wrec_normal_cnt;
uint32_t debug_wrec_underrun_cnt;
uint32_t debug_wrec_overrun_cnt;

void wrec_encode_proc(void *handle);


#if WREC_DC_CUT_EN

#define INT_SHIFT(x,Q)     ((Q) > 0 ? (int)(x) << (Q) : (int)(x) >> (-Q))
#define Q_EQ_SAMPLE        23
#define SATURATE_REAL(x,Q)          ((int)(x) > (int)((1<<(Q))-1) ? (int)((1<<(Q))-1) : ((x) < (int)(-(1<<(Q))) ? (int)(-(1<<(Q))) : (x)))

#define BAND_NEED 1

typedef struct _dc_cut {
    int16_t ch;
    int16_t shift;
    float delay_line[2][BAND_NEED+1][2];
    biquad_coeff_t coeff_array[BAND_NEED];
}dc_cut;

void* create_dc_worker(int32_t ch, int32_t Qbit)
{
    dc_cut * dc = general_calloc(sizeof(dc_cut));

    dc->ch = ch;
    dc->shift = 23 - Qbit;
    logi("dc : %d, %d", ch, Qbit);

#if (BAND_NEED == 2)
    dc->coeff_array[0].b0 = 0.598331809f;
    dc->coeff_array[0].b1 = -1.19666361f;
    dc->coeff_array[0].b2 = 0.598331809f;
    dc->coeff_array[0].a1 = -1.99581325f;
    dc->coeff_array[0].a2 = 0.995821535f;

    dc->coeff_array[1].b0 = 1.66518151f;
    dc->coeff_array[1].b1 = 0.0f;
    dc->coeff_array[1].b2 = -1.66518151f;
    dc->coeff_array[1].a1 = 0.00316190417f;
    dc->coeff_array[1].a2 = -0.996838092f;
#else
    dc->coeff_array[0].b0 = 0.998058140f;
    dc->coeff_array[0].b1 = -1.99611628f;
    dc->coeff_array[0].b2 = 0.998058140f;
    dc->coeff_array[0].a1 = -1.99611247f;
    dc->coeff_array[0].a2 = 0.996120155f;
#endif
    return (void*)dc;
}

void destroy_dc_worker(void* handle)
{
    dc_cut* dc = (dc_cut *)handle;
    if(dc) {
        general_free(dc);
    }
}

typedef union {
    int16_t *p16;
    int32_t *p32;
    void*    ctx;
}dc_data_type;

AT_G24_SRAM(.wrec)
int afx_dc_cut(void *handle, void* inbuf, void* outbuf, int32_t samples)
{
    //To Dc cut
    uint8_t band;
    int32_t c, i, xt;
    float acc;
    __maybe_unused float x1, d0, d1;
    dc_cut* dc = (dc_cut *)handle;
    int16_t ch = dc->ch;
    int16_t shift = dc->shift;
    dc_data_type din, dout;

    din.ctx  = inbuf;
    dout.ctx = outbuf;

    for (i = 0; i < samples; i++) {
        for(c = 0; c < ch; c++) {
            if(shift > 0) {
                xt = (int32_t)din.p16[i*ch + c];
                xt <<= shift;
            }
            else {
                xt = (int32_t)din.p32[i*ch + c];
                xt >>= (-shift);
            }
            x1 = xt;

            for (band = 0; band < BAND_NEED; band++)
            {
                acc = x1 * dc->coeff_array[band].b0;
                acc += (dc->delay_line[c][band][0] * dc->coeff_array[band].b1 + dc->delay_line[c][band][1] * dc->coeff_array[band].b2);

                dc->delay_line[c][band][1] = dc->delay_line[c][band][0];
                dc->delay_line[c][band][0] = x1;

                acc -= (dc->delay_line[c][band + 1][0] * dc->coeff_array[band].a1 + dc->delay_line[c][band+1][1] * dc->coeff_array[band].a2);
                x1 = acc;

                if (band == (BAND_NEED - 1))
                {
                    dc->delay_line[c][BAND_NEED][1] = dc->delay_line[c][BAND_NEED][0];
                    dc->delay_line[c][BAND_NEED][0] = x1;
                }
            }

            xt = x1;
            xt = SATURATE_REAL(xt, 23);
            if(shift > 0) {
                xt >>= shift;
                dout.p16[i*ch + c] = (int16_t)xt;
            }
            else {
                xt <<= (-shift);
                dout.p32[i*ch + c] = (int32_t)xt;
            }
        }
    }
    return samples;
}
#endif


AT_G24_SRAM(.wrec_enc_loop)
static void *wrec_enc_loop(void *handle)
{
    wrec_t *wrec = handle;
    logd("wrec enc loop");

    while (wrec) {
        wrec_encode_proc(wrec);
    }

    return NULL;
}

void wrec_sbc_encoder_close(void *handle)
{

}

void wrec_lc3_encoder_close(void *handle)
{
    //TODO: encoder没有close动作
    __maybe_unused wrec_t *wrec = handle;
#if WREC_DC_CUT_EN
    if(wrec->dc_cut) {
        destroy_dc_worker(wrec->dc_cut);
        wrec->dc_cut = 0;
    }
#endif
}

void wrec_sbc_encoder_open(void *handle)
{
    wrec_t *wrec = handle;

    sbcenc_prms_t sbcenc_prm;
    sbcenc_prm.frequency = SBC_FREQ_48000;
    sbcenc_prm.blocks = SBC_BLK_16;//SBC_BLK_8;
    sbcenc_prm.subbands = SBC_SB_8;
    if(2 ==  wrec->enc_ch) {
        sbcenc_prm.mode = SBC_MODE_JOINT_STEREO;
    } else {
        sbcenc_prm.mode = SBC_MODE_MONO;
    }
    sbcenc_prm.allocation = SBC_AM_LOUDNESS;
    sbcenc_prm.bitpool = 32;
    wrec->bsbuf_sz = 77;
#if G24_DUAL_CHANNEL
    if(!G24_DUAL_CHANNEL_UL_LONG_PKT) {
        sbcenc_prm.bitpool = 12;
        wrec->bsbuf_sz = 37;
    }
#else
#if 0//(G24_MONITOR_EN)
    sbcenc_prm.bitpool = 12;
    wrec->bsbuf_sz = 36;
#endif
#endif

#if G24_SBC_SEND_4TIMES
    if(wrec->enc_ch == 2) {
        sbcenc_prm.bitpool = 12;
        wrec->bsbuf_sz = 37;
    } else {
        sbcenc_prm.bitpool = 15;
        wrec->bsbuf_sz = 38;
    }
#endif
    g24_tx_packet_size_set(wrec->bsbuf_sz);

    sbcenc_prm.msbc = 0;
    wrec->encoder = sbc_encode_open(&sbcenc_prm);
    if (NULL == wrec->encoder) {
        loge("sbc enc open fail");
    }
    wrec->enc_proc_samples = 128;
    wrec->enc_pcm_res_samples = 0;
    wrec->enc_pcmbuf0 = general_calloc(sizeof(int16_t) * wrec->enc_proc_samples);
    if (2 == wrec->enc_ch) {
        wrec->enc_pcmbuf1 = general_calloc(sizeof(int16_t) * wrec->enc_proc_samples);
    }
    if(wrec->link == 0) {
        wrec->bsbuf_cnt[0] = BSBUF_CNT;
        wrec->wait_th[0] = BS_WAIT_TH_FOR_UNDERRUN;
        wrec->bsbuf[0] = general_calloc(wrec->bsbuf_cnt[0] * (wrec->bsbuf_sz+sizeof(uint32_t)));
    	ring_buffer_init(&wrec->bsbuf_ctrl[0], wrec->bsbuf[0], wrec->bsbuf_cnt[0] * (wrec->bsbuf_sz+sizeof(uint32_t)));
#if G24_DUAL_CHANNEL && (AUDIO_G24_TX_ENCODER_NUM==1)                       //只用一个编码器时，两个bsbuf一起初始化
        if(G24_FUNC_ROLE == G24_FUNC_ROLE_DONGLE) {
            wrec->bsbuf_cnt[1] = BSBUF_CNT;
        	wrec->wait_th[1] = BS_WAIT_TH_FOR_UNDERRUN;
        	wrec->bsbuf[1] = general_calloc(wrec->bsbuf_cnt[1] * (wrec->bsbuf_sz+sizeof(uint32_t)));
        	ring_buffer_init(&wrec->bsbuf_ctrl[1], wrec->bsbuf[1], wrec->bsbuf_cnt[1] * (wrec->bsbuf_sz+sizeof(uint32_t)));
        }
#endif
    } else {
#if G24_DUAL_CHANNEL && (AUDIO_G24_TX_ENCODER_NUM==2)
        wrec->bsbuf_cnt[1] = BSBUF_CNT;
        wrec->wait_th[1] = BS_WAIT_TH_FOR_UNDERRUN;
        wrec->bsbuf[1] = general_calloc(wrec->bsbuf_cnt[1] * (wrec->bsbuf_sz+sizeof(uint32_t)));
        ring_buffer_init(&wrec->bsbuf_ctrl[1], wrec->bsbuf[1], wrec->bsbuf_cnt[1] * (wrec->bsbuf_sz+sizeof(uint32_t)));
#endif
    }
}

void wrec_lc3_encoder_open(void *handle)
{
    wrec_t *wrec = handle;

    wrec->bsbuf_sz = LC3_BS_LEN;
    wrec->enc_out_buf = general_calloc(wrec->bsbuf_sz);

    g24_tx_packet_size_set(wrec->bsbuf_sz);

    int32_t fs = 48000;
    int32_t ch = LC3_ENC_CH;
    int32_t bitrate = LC3_BITRATE;
    float frame_ms = LC3_FRAME_MS;
    int32_t hrmode = 0;
    wrec->encoder = lc3_enc_open(fs, ch, bitrate, frame_ms, hrmode);
    if (NULL == wrec->encoder) {
        loge("sbc enc open fail");
    }
    wrec->enc_proc_samples = LC3_FRAME_SAMPLES;

    wrec->enc_pcm_res_samples = 0;
    wrec->enc_pcmbuf0 = general_calloc(sizeof(int16_t) * wrec->enc_proc_samples);
    if (2 == wrec->enc_ch) {
        wrec->enc_pcmbuf1 = general_calloc(sizeof(int16_t) * wrec->enc_proc_samples);
    }
    
    if(wrec->link == 0) {
        wrec->bsbuf_cnt[0] = BSBUF_CNT;
    	wrec->wait_th[0] = BS_WAIT_TH_FOR_UNDERRUN;
    	wrec->bsbuf[0] = general_calloc(wrec->bsbuf_cnt[0] * (wrec->bsbuf_sz+sizeof(uint32_t)));
    	ring_buffer_init(&wrec->bsbuf_ctrl[0], wrec->bsbuf[0], wrec->bsbuf_cnt[0] * (wrec->bsbuf_sz+sizeof(uint32_t)));
#if G24_DUAL_CHANNEL && (AUDIO_G24_TX_ENCODER_NUM==1)                       //只用一个编码器时，两个bsbuf一起初始化
        if(G24_FUNC_ROLE == G24_FUNC_ROLE_DONGLE) {
            wrec->bsbuf_cnt[1] = BSBUF_CNT;
       	    wrec->wait_th[1] = BS_WAIT_TH_FOR_UNDERRUN;
            wrec->bsbuf[1] = general_calloc(wrec->bsbuf_cnt[1] * (wrec->bsbuf_sz+sizeof(uint32_t)));
            ring_buffer_init(&wrec->bsbuf_ctrl[1], wrec->bsbuf[1], wrec->bsbuf_cnt[1] * (wrec->bsbuf_sz+sizeof(uint32_t)));
        }
#endif
    } else {
#if G24_DUAL_CHANNEL && (AUDIO_G24_TX_ENCODER_NUM==2)
        wrec->bsbuf_cnt[1] = BSBUF_CNT;
        wrec->wait_th[1] = BS_WAIT_TH_FOR_UNDERRUN;
        wrec->bsbuf[1] = general_calloc(wrec->bsbuf_cnt[1] * (wrec->bsbuf_sz+sizeof(uint32_t)));
        ring_buffer_init(&wrec->bsbuf_ctrl[1], wrec->bsbuf[1], wrec->bsbuf_cnt[1] * (wrec->bsbuf_sz+sizeof(uint32_t)));
#endif
    }

#ifdef WREC0_USE_CPU1
    if(wrec->link == 0) {
        wrec->cpux_handle = OAL_cpux_work_create(1, 2, 256);
    }
#endif

#ifdef WREC1_USE_CPU1
    if(wrec->link == 1) {
        wrec->cpux_handle = OAL_cpux_work_create(1, 2, 256);
    }
#endif

#if WREC_DC_CUT_EN
    wrec->dc_cut = create_dc_worker(wrec->enc_ch, 8*sizeof(int16_t) - 1);
#endif
}

AT_G24_SRAM(.wrec_encode)
int32_t wrec_sbc_encode_proc(void *handle, uint8_t **p_enc_out_buf)
{
    wrec_t *wrec = handle;
    short *inbuf[2];
    inbuf[0] = wrec->enc_pcmbuf0;
    inbuf[1] = wrec->enc_pcmbuf1;
    int readitems = wrec->enc_proc_samples;
    int outbytes;
    int ret = -1;

    if (wrec->encoder) {
        ret = sbc_encode_proc(wrec->encoder,
            inbuf, &readitems, p_enc_out_buf, &outbytes);
        if(ret >= 0 && outbytes > 0) {
            ret = 0;
        }
    }

    return ret;
}

AT_G24_SRAM(.wrec_encode)
int32_t wrec_lc3_encode_proc(void *handle, uint8_t **p_enc_out_buf)
{
    wrec_t *wrec = handle;
    int32_t ret = -1;
    int16_t* input16[2];
    input16[0] = wrec->enc_pcmbuf0;
    input16[1] = wrec->enc_pcmbuf1;
#if (1 == LC3_ENC_CH)
    //双声道转单声道编码
    for (int i = 0; i < wrec->enc_proc_samples; i++) {
        wrec->enc_pcmbuf0[i] = (wrec->enc_pcmbuf0[i] + wrec->enc_pcmbuf1[i]) >> 1;
    }
#endif
    int32_t nbytes = lc3_enc_frameenc(wrec->encoder, input16, wrec->enc_out_buf);
    if (nbytes == wrec->bsbuf_sz) {
        ret = 0;
        *p_enc_out_buf = wrec->enc_out_buf;
    } else {
        loge("lc3 enc err:%d",nbytes);
    }

    return ret;
}

const wrec_encoder_plugin_t wrec_sbc_plugin = {
    .open = wrec_sbc_encoder_open,
    .encode = wrec_sbc_encode_proc,
    .close = wrec_sbc_encoder_close,
};

const wrec_encoder_plugin_t wrec_lc3_plugin = {
    .open = wrec_lc3_encoder_open,
    .encode = wrec_lc3_encode_proc,
    .close = wrec_lc3_encoder_close,
};

uint16_t wrec_bs_len_get(void)
{
#if (0 == AUDIO_G24_TX_ENCODER_SEL)
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

void *wrec_create(wrec_init_params_t *prm)
{
    wrec_t *wrec;

    wrec = OAL_calloc(1, sizeof(*wrec));


    wrec->plugin = prm->plugin;
    wrec->enc_ch = 2;
    wrec->link = prm->link;

    wrec->plugin.open(wrec);
    int32_t pcm_store_bytes = sizeof(int16_t) * wrec->enc_ch * (wrec->enc_proc_samples);
    wrec->pcmstore_buf = general_calloc(pcm_store_bytes);
    audio_share_buffer_params_t sharebuf_prams;
    sharebuf_prams.buf = wrec->pcmstore_buf;
    sharebuf_prams.size = pcm_store_bytes;
    sharebuf_prams.store_ch = wrec->enc_ch;
    sharebuf_prams.store_sample_size = sizeof(int16_t);
    sharebuf_prams.input_ch = 2;
    sharebuf_prams.input_sample_size = sizeof(int32_t);
    sharebuf_prams.input_interleave  = 1;
    sharebuf_prams.write_refmt.func = NULL;
    sharebuf_prams.read_refmt.func = NULL;
    sharebuf_prams.write_refmt.interleave2interleave_fmt =
                reformat_to_hook(GET_REFORMAT_TYPE(sharebuf_prams.input_ch, sharebuf_prams.input_sample_size,
                    sharebuf_prams.store_ch, sharebuf_prams.store_sample_size));
    audio_share_buffer_init(&wrec->sb, &sharebuf_prams);

#ifdef USE_NEW_INTERP
    wrec->interp_out_res = 0;
#if (INTERP_TYPE_SEL == INTERP_TYPE_SLTK_INTERP || INTERP_TYPE_SEL == INTERP_TYPE_SLTK_INTERP2)
    sltk_interp_init interp_init;
    interp_init.ch = wrec->enc_ch;
    interp_init.sample_size = sizeof(int16_t);
    interp_init.period = wrec->enc_proc_samples / INTERP_FRAME_SCALE;
#if (INTERP_TYPE_SEL == INTERP_TYPE_SLTK_INTERP)
    wrec->interp_handle = sltk_interp_open(&interp_init);
#else
    wrec->interp_handle = sltk_interp_open2(&interp_init);
#endif

#else
    id_init idi;
    idi.ch = wrec->enc_ch;
    idi.alpha = 1.0;
    idi.level = SRATE_QUALITY_CUBIC_LVL_1;
    idi.period = wrec->enc_proc_samples / INTERP_FRAME_SCALE + MAX_INTERP_CNT;
    idi.sample_size = sizeof(int16_t);
    wrec->interp_handle = id_worker_init(&idi);
#endif
    if (NULL == wrec->interp_handle) {
        loge("interp open fail");
    }
    wrec->interpbuf = general_malloc((wrec->enc_proc_samples + MAX_INTERP_CNT) * wrec->enc_ch * sizeof(int16_t));
    wrec->interpoutbuf = general_malloc((wrec->enc_proc_samples + MAX_INTERP_CNT) * wrec->enc_ch * sizeof(int16_t));
    if (NULL == wrec->interpbuf) {
        loge("interpbuf malloc fail");
    }
#endif

    wrec->task_handle = OAL_thread_create(wrec_enc_loop, wrec,
        "wrec", 9, 768);

    return wrec;
}

AT_G24_SRAM(.wrec_bs_read)
uint8_t wrec_bs_read(void *handle, uint8_t link, uint8_t *bs, uint32_t bytes)
{
    wrec_t *wrec = handle;
    uint8_t ret = 0;
    uint32_t stamp;

    //logi("wrec_bs_read %d %d", bytes, wrec->bsbuf_sz);
    //从bs buffer中取数据
    if (NULL == wrec || bytes != wrec->bsbuf_sz) {
        return 0;
    }

    uint16_t r_avail = ring_buffer_get_r_avail(&wrec->bsbuf_ctrl[link]);
    //logi("r_avail %d, %d", r_avail, (wrec->bsbuf_sz+sizeof(uint32_t)) * BSBUF_CNT);
    if (r_avail >= (wrec->bsbuf_sz+sizeof(uint32_t)) * BSBUF_CNT) {
        logd("O515");
        //overrun
        ring_buffer_update_rpos(&wrec->bsbuf_ctrl[link], (wrec->bsbuf_sz+sizeof(uint32_t))*(BSBUF_CNT-1));
    }

    //logi("wrec_bs_read %d ", ring_buffer_get_r_avail(&wrec->bsbuf_ctrl[link]));

    if (ring_buffer_get_r_avail(&wrec->bsbuf_ctrl[link]) >= (wrec->bsbuf_sz+sizeof(uint32_t)) * wrec->wait_th[link]) {
        //logx("R ");
        debug_wrec_normal_cnt++;

        wrec->wait_th[link] = BS_WAIT_TH_FOR_NORMAL;
        ring_buffer_read(&wrec->bsbuf_ctrl[link], (uint8_t*)&stamp, sizeof(uint32_t));

#ifdef WREC_PCM_INTERP
#if G24_DUAL_CHANNEL && (AUDIO_G24_TX_ENCODER_NUM==1)
        if (link == 0)
#endif
        {
            //数据量监控
            uint32_t cur_t = HAL_timestamp_to_us(HAL_get_timestamp());
            uint32_t data_t = cur_t - stamp;
            data_t += (ring_buffer_get_r_avail(&wrec->bsbuf_ctrl[link])/wrec->bsbuf_sz) * 2500;
            wrec->sum_cnt++;
            wrec->sum_data_t += data_t;
            if (wrec->sum_cnt >= 100) {
                int32_t avg = wrec->sum_data_t / wrec->sum_cnt;
                wrec->avg = avg;
                //logi("{wreck Avg:%d}", avg);
                if (avg < LOW_TH) {
                    int32_t adjust_sta = -1;
                    if (wrec->adjust_sta != adjust_sta) {
                        wrec->adjust_sta = adjust_sta;
                        logd("Slow[%d]", link);
                    }
                } else if (avg > HIGH_TH) {
                    int32_t adjust_sta = 1;
                    if (wrec->adjust_sta != adjust_sta) {
                        wrec->adjust_sta = adjust_sta;
                        logd("Fast[%d]", link);
                    }
                } else {
                    int32_t adjust_sta = 0;
                    if (wrec->adjust_sta > adjust_sta) {
                        if (avg <= NORMAL_TH) {
                            wrec->adjust_sta = adjust_sta;
                            logd("Normal[%d]", link);
                        }
                    } else if (wrec->adjust_sta < adjust_sta) {
                        if (avg >= NORMAL_TH) {
                            wrec->adjust_sta = adjust_sta;
                            logd("Normal[%d]", link);
                        }
                    } else {
                        if (wrec->adjust_sta != adjust_sta) {
                            wrec->adjust_sta = adjust_sta;
                            logd("Normal[%d]", link);
                        }
                    }
                }

                wrec->sum_data_t = 0;
                wrec->sum_cnt = 0;
            }
        }
#endif
        ring_buffer_read(&wrec->bsbuf_ctrl[link], bs, wrec->bsbuf_sz);
        ret = 1;
    } else {
        // logx("U++ [%d]", link);
        debug_wrec_underrun_cnt++;
        memset(bs, 0, bytes);
        wrec->wait_th[link] = BS_WAIT_TH_FOR_UNDERRUN;
    }

    return ret;
}


AT_G24_SRAM(.wrec_encode)
void wrec_encode(void *handle, int32_t *buf, uint32_t samples)
{
    wrec_t *wrec = handle;
    uint32_t w_avail;
    uint32_t write_samples;
    if (NULL == wrec || 0 == wrec->enc_proc_samples || NULL == wrec->task_handle) {
        return;
    }

#if MODE_G24_EN  //编码器对应的link连接上后才编码，可节省功耗
    g24_mode_t *g24_mode = g24_mode_param_get();
#if G24_DUAL_CHANNEL && (AUDIO_G24_TX_ENCODER_NUM == 1)
    if(!(g24_mode->connect[0] || g24_mode->connect[1])) {
        return;
    }
#else
    if(!g24_mode->connect[wrec->link]) {
        return;
    }
#endif
#endif

    //仅保存数据，编码交由编码线程处理
    int16_t *dst_p;
    int32_t *src = buf;
    while (samples > 0) {
        while (1) {
            ring_buffer_get_w_block(&wrec->sb.ctrl, &w_avail, (uint8_t**)&dst_p);
            if (w_avail == 0) {
                OAL_sem_wait(&wrec->sb.write_sem_sync);
            } else {
                break;
            }
        }
        write_samples = MIN(w_avail / (wrec->enc_ch * sizeof(int16_t)), samples);
        write_samples = MIN(G_AUDIO_PROC_PERIOD, write_samples); /* period limit */
        samples -= write_samples;
        for (int i = 0; i < write_samples * wrec->enc_ch; i++) {
            dst_p[i] = src[i] >> 16;
        }

#if WREC_DC_CUT_EN
        if(wrec->dc_cut) {
            afx_dc_cut(wrec->dc_cut, (void*)dst_p, (void*)dst_p, write_samples);
        }
#endif

        src += write_samples * wrec->enc_ch;
        ring_buffer_update_wpos(&wrec->sb.ctrl, write_samples * wrec->enc_ch * sizeof(int16_t));
        OAL_sem_post(&wrec->sb.read_sem_sync);
    }
}


AT_G24_SRAM(.wrec_encode)
void wrec_bitstream_encode(void *handle)
{
    wrec_t *wrec = handle;
    int ret;
    uint8_t *enc_out_buf;
    //编码
    ret = wrec->plugin.encode(wrec, &enc_out_buf);
    
    if (ret < 0) {
        //encode err
    } else {
        if(wrec->link == 0) {
            //保存码流数据
            int rigng  = ring_buffer_get_w_avail(&wrec->bsbuf_ctrl[0]);
            //logi("rigng %d, %d", rigng, wrec->bsbuf_sz);
            if (rigng >= (wrec->bsbuf_sz+sizeof(uint32_t))) {
                uint32_t sys_t = HAL_timestamp_to_us(HAL_get_timestamp());
                ring_buffer_write(&wrec->bsbuf_ctrl[0], (uint8_t*)&sys_t, sizeof(uint32_t));
                ring_buffer_write(&wrec->bsbuf_ctrl[0], enc_out_buf, wrec->bsbuf_sz);
            } else {
                logx("O750 ");
                debug_wrec_overrun_cnt++;
            }
#if G24_DUAL_CHANNEL && (AUDIO_G24_TX_ENCODER_NUM==1)                               //只有一个编码器，用同样的数据
            //保存码流数据
            if(G24_FUNC_ROLE == G24_FUNC_ROLE_DONGLE) {
				if (ring_buffer_get_w_avail(&wrec->bsbuf_ctrl[1]) >= (wrec->bsbuf_sz+sizeof(uint32_t))) {
                	uint32_t sys_t = HAL_timestamp_to_us(HAL_get_timestamp());
                	ring_buffer_write(&wrec->bsbuf_ctrl[1], (uint8_t*)&sys_t, sizeof(uint32_t));
	            	ring_buffer_write(&wrec->bsbuf_ctrl[1], enc_out_buf, wrec->bsbuf_sz);
	            } else {
	            	logx("O761 ");
	            	debug_wrec_overrun_cnt++;
	        	}
            }  
#endif
        } else {
            //保存码流数据
            if (ring_buffer_get_w_avail(&wrec->bsbuf_ctrl[1]) >= (wrec->bsbuf_sz+sizeof(uint32_t))) {
            	uint32_t sys_t = HAL_timestamp_to_us(HAL_get_timestamp());
            	ring_buffer_write(&wrec->bsbuf_ctrl[1], (uint8_t*)&sys_t, sizeof(uint32_t));
            	ring_buffer_write(&wrec->bsbuf_ctrl[1], enc_out_buf, wrec->bsbuf_sz);
            } else {
            	logx("O773 ");
            	debug_wrec_overrun_cnt++;
        	}
        }
    }
}

AT_G24_SRAM(.wrec_pcm_read)
void wrec_pcm_read(wrec_t *wrec, int16_t *dst0, int16_t *dst1, int32_t samples)
{
    int16_t *src_p;
    uint32_t r_samples;
    uint32_t avail;

    //读出一帧数据
    while (samples) {
        ring_buffer_get_r_block(&wrec->sb.ctrl, &avail, (uint8_t**)&src_p);
        if (avail) {
            r_samples = avail / (wrec->enc_ch * sizeof(int16_t));
            r_samples = MIN(r_samples, samples);
            if (2 == wrec->enc_ch) {
                for (int i = 0; i < r_samples; i++) {
                    dst0[i] = src_p[2*i];
                    dst1[i] = src_p[2*i+1];
                }
                dst0 += r_samples;
                dst1 += r_samples;
            } else {
                for (int i = 0; i < r_samples; i++) {
                    dst0[i] = src_p[i];
                }
                dst0 += r_samples;
            }
            ring_buffer_update_rpos(&wrec->sb.ctrl, r_samples * wrec->enc_ch * sizeof(int16_t));
            samples -= r_samples;
            OAL_sem_post(&wrec->sb.write_sem_sync);
        } else {
            OAL_sem_wait(&wrec->sb.read_sem_sync);
        }
    }

}

#ifdef USE_NEW_INTERP
AT_G24_SRAM(.wrec_pcm_interp_new_read)
void wrec_pcm_interp_new_read(wrec_t *wrec, int16_t *dst0, int16_t *dst1, int32_t samples)
{
    const void *src[2];
    void *in, *out;
    int32_t r_samples, insamples, outsamples;
    float alpha =(float)1.0;
    int32_t interp_frame_len = wrec->enc_proc_samples / INTERP_FRAME_SCALE;

    wrec->interp_adjust_sta = wrec->adjust_sta;

    //logd("{ %d}", wrec->interp_adjust_sta);
    while (wrec->interp_out_res < samples) {
        src[0] = dst0;
        src[1] = dst1;
        r_samples = MIN(samples - wrec->interp_out_res, interp_frame_len);
        wrec_pcm_read(wrec, dst0, dst1, r_samples);
        __16_to_interleave_stereo16(wrec->interpbuf + wrec->interp_out_res * wrec->enc_ch,
            src, r_samples);

        if (wrec->interp_adjust_sta < 0) {
            //+
            if (HAL_timestamp_to_ms(HAL_get_timestamp()) - wrec->last_interp_t > 1000 ||
                wrec->avg < LOW_TH - (NORMAL_TH - LOW_TH) / 2) {
                alpha = (float)1.0 - (float)0.0001;
            }
        } else if (wrec->interp_adjust_sta > 0) {
            //-
            if (HAL_timestamp_to_ms(HAL_get_timestamp()) - wrec->last_interp_t > 1000 ||
                wrec->avg > HIGH_TH + (HIGH_TH - NORMAL_TH) / 2) {
                alpha = (float)1.0 + (float)0.0001;
            }
        } else {
            //normal
            alpha = (float)1.0;
        }

        in = wrec->interpbuf + wrec->interp_out_res * wrec->enc_ch;
        out = in;
        insamples = r_samples;
        outsamples = 0;
#if (INTERP_TYPE_SEL == INTERP_TYPE_SLTK_INTERP || INTERP_TYPE_SEL == INTERP_TYPE_SLTK_INTERP2)
#if (INTERP_TYPE_SEL == INTERP_TYPE_SLTK_INTERP)
        sltk_interp_proc(wrec->interp_handle, &in, &insamples, &out, &outsamples, alpha);
#else
        sltk_interp_proc2(wrec->interp_handle, &in, &insamples, &out, &outsamples, alpha);
#endif
#else
		out = wrec->interpoutbuf;
        id_worker_interp_get_period(wrec->interp_handle, &in, &insamples, &out, &outsamples, alpha);
        memcpy(wrec->interpbuf + wrec->interp_out_res * wrec->enc_ch, wrec->interpoutbuf, outsamples*wrec->enc_ch*sizeof(int16_t));
#endif
        if (r_samples != outsamples) {
            //logd("{t %d -> %d}", r_samples, outsamples);
            wrec->last_interp_t = HAL_timestamp_to_ms(HAL_get_timestamp());
        }
        wrec->interp_out_res += outsamples;
    }

    //已知ch = 2
    for (int i = 0; i < samples; i++) {
        dst0[i] = wrec->interpbuf[2*i];
        dst1[i] = wrec->interpbuf[2*i+1];
    }
    wrec->interp_out_res -= samples;
    memcpy(wrec->interpbuf, wrec->interpbuf + 2 * samples, wrec->interp_out_res * 2 * sizeof(int16_t));
}
#else

AT_G24_SRAM(.wrec_pcm_interp_read)
void wrec_pcm_interp_read(wrec_t *wrec, int16_t *dst0, int16_t *dst1, int32_t samples)
{
    int32_t total_cnt = (((1024*4 + samples - 1)/samples) * samples);
    float adjust_speed = (float)1 / total_cnt;
    __maybe_unused int32_t new_adjust_sta = wrec->adjust_sta;

    int16_t h[2];
    int16_t tmp_h[2];
    if ((wrec->interp_proc_cnt && wrec->interp_adjust_sta < 0) ||
        (!wrec->interp_proc_cnt && new_adjust_sta < 0)) {

        if (!wrec->interp_proc_cnt) {
            //start slow
            wrec->interp_adjust_sta = new_adjust_sta;
        }

        h[0] = wrec->hist[0];
        h[1] = wrec->hist[1];
        if (wrec->hist_res) {
		    //第一个点来自hist
            dst0[0] = wrec->hist[0];
            dst1[0] = wrec->hist[1];
        
            //读出samples - 1个点
            wrec_pcm_read(wrec, dst0+1, dst1+1, samples - 1);

            wrec->hist_res = 0;
        } else {
            wrec_pcm_read(wrec, dst0, dst1, samples);
        }

        for (int n = 0; n < samples; n++) {
            wrec->interp_proc_cnt++;
            float s = wrec->interp_proc_cnt*adjust_speed;
            tmp_h[0] = dst0[n];
            tmp_h[1] = dst1[n];
            float a, b;
            a = h[0] << 16;
            b = dst0[n] << 16;
            dst0[n] = ((int32_t)(a * s + b * (1.0f - s))) >> 16;
            a = h[1] << 16;
            b = dst1[n] << 16;
            dst1[n] = ((int32_t)(a * s + b * (1.0f - s))) >> 16;
    
            h[0] = tmp_h[0];
            h[1] = tmp_h[1];
        }
        wrec->hist[0] = h[0];
        wrec->hist[1] = h[1];

        if (wrec->interp_proc_cnt >= total_cnt) {
             wrec->interp_proc_cnt = 0;
             wrec->hist_res = 1; //标记wrec->hist有一个有效数据，下次需要读出
        }
        //logx("+");
    } else if ((wrec->interp_proc_cnt && wrec->interp_adjust_sta > 0) ||
               (!wrec->interp_proc_cnt && new_adjust_sta > 0)) {
        if (0 == wrec->interp_proc_cnt) {
            //start fast
            wrec->interp_adjust_sta = new_adjust_sta;
            if (wrec->hist_res) {
                //wrec->hist[0] = wrec->hist[0];
                //wrec->hist[1] = wrec->hist[1];
                wrec->hist_res = 0;
            } else {
                wrec_pcm_read(wrec, &wrec->hist[0], &wrec->hist[1], 1);
            }
        }
        wrec_pcm_read(wrec, dst0, dst1, samples);

        h[0] = wrec->hist[0];
        h[1] = wrec->hist[1];
        for (int n = 0; n < samples; n++) {
            wrec->interp_proc_cnt++;
            float s = wrec->interp_proc_cnt*adjust_speed;
            tmp_h[0] = dst0[n];
            tmp_h[1] = dst1[n];
            float a, b;
            a = h[0] << 16;
            b = dst0[n] << 16;
            dst0[n] = ((int32_t)(a * (1.0f - s) + b * s)) >> 16;
            a = h[1] << 16;
            b = dst1[n] << 16;
            dst1[n] = ((int32_t)(a * (1.0f - s) + b * s)) >> 16;
            h[0] = tmp_h[0];
            h[1] = tmp_h[1];
        }
        wrec->hist[0] = h[0];
        wrec->hist[1] = h[1];
        if (wrec->interp_proc_cnt >= total_cnt) {
             wrec->interp_proc_cnt = 0;
        }
        //记录每组最后值，以备下次使用
        wrec->hist[0] = dst0[samples-1];
        wrec->hist[1] = dst1[samples-1];
        //logx("-");
    } else {
        wrec->interp_adjust_sta = new_adjust_sta;
        if (wrec->hist_res) {
            dst0[0] = wrec->hist[0];
            dst1[0] = wrec->hist[1];
        
            wrec_pcm_read(wrec, dst0+1, dst1+1, samples - 1);

            wrec->hist_res = 0;
        } else {
            wrec_pcm_read(wrec, dst0, dst1, samples);
        }
        wrec->hist[0] = dst0[samples-1];
        wrec->hist[1] = dst1[samples-1];
    }
}

#endif

AT_G24_SRAM(.wrec_encode)
void wrec_encode_proc(void *handle)
{
    wrec_t *wrec = handle;
    uint32_t samples = wrec->enc_proc_samples;
    int16_t *dst0 = wrec->enc_pcmbuf0;
    int16_t *dst1 = wrec->enc_pcmbuf1;

    if (NULL == wrec || 0 == wrec->enc_proc_samples) {
        return;
    }

#ifdef WREC_PCM_INTERP
#ifdef USE_NEW_INTERP
    wrec_pcm_interp_new_read(wrec, dst0, dst1, samples); //插值同步方式(新插值算法)
#else
    wrec_pcm_interp_read(wrec, dst0, dst1, samples); //插值同步方式
#endif
#else
    wrec_pcm_read(wrec, dst0, dst1, samples); //正常读
#endif

    if (wrec->cpux_handle) {
        OAL_cpux_work_request(wrec->cpux_handle, wrec_bitstream_encode, handle);
    } else {
        wrec_bitstream_encode(handle);
    }
}
