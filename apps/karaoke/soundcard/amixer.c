
/*****************************************************************************
* 混音线程
****************************************************************************/
#define LOG_LEV 4
#include "amixer.h"
#include "audio_in.h"
#include "audio_out.h"
#include "waudio_track.h"
#include "middleware.h"
#include "amsg.h"
#include "aeffect_component.h"
#include "audio_service.h"
#include "audio_out.h"
#include "volume_control.h"
#include "g24_api.h"
#include "soundcard.h"
#include "user_config.h"
#include "local_play_manager.h"
#include "tone_play_manager.h"
#include "app_psm.h"
#include "psm_manager.h"
#include "sltk_ducker.h"
#include "wrx_track.h"
#include "wrec.h"
#include "user_config.h"
#include "sltk_freq_shift.h"
#include "audio_afx_config.h"
#include "key_ctrl_led.h"
#include "app_audio_codec_config.h"
#include "sltk_vocalenhance.h"
#include "mic_i2s_afc.h"
#include "afc_manager.h"
#include "i2s_slaver_track.h"
#include "i2s_slaver_rec.h"
#include "usb_app_config.h"
#include "vocut.h"
#include "app_record.h"
#include "app_lcdc.h"
#include "sltk_hpemafc.h"
#include "sltk_howling_detect.h"
#include "mode_g24.h"
#include "spdif_play_track.h"
#include "spdif_master_rec.h"
#include "lcd_seg.h"

#define AMIXER_SRAM_FUNC AT_SRAM(.audiomixer)
#define AMIXER_BANK_FUNC

#define USE_EFFECTS             1
#define AMIXER_ENG_CALC         1       // 能量计算

#if G24_FUNC_ROLE && AUDIO_KARAOKE_LINEIN_EN  //dongle使用linein时使用，ram不够时可以适当减小USB_LISTENER_DELAY_CNT或使用USB_LISTENER_DELAY_16BIT
#define USB_LISTENER_DELAY          1       //音乐到usb上行延时补偿, 为了使handset声音和dongle端伴奏延时减少，伴奏放handset可以关掉节省空间
#define USB_LISTENER_DELAY_CNT      57      // USB_LISTENER_DELAY_CNT* 64/48 ms
#define USB_LISTENER_DELAY_16BIT    0       // 按照16bit缓存
#endif

#define AMIXER_FS    (48000)
#define AMIXER_CH    (2)
#define AMIXER_BPS   (sizeof(int32_t))
#define AMIXER_IO_PCM_INTERLEAVE   (true)

#define AMIXER_TASK_NAME           "amixer"
#define AMIXER_TASK_STACK_DEPTH    (768)
#define AMIXER_MSG_QUEUE_LENGTH    (10)
#define AMIXER_TASK_PRIO           (TRACK_TASK_PRIO)

#define AMIXER_AUDIOTRACK_MAX_NUM  (11)
//TODO : Should unify this paras
#define AMIXER_PROC_PERIOD         (G_AUDIO_PROC_PERIOD)

#define AMIXER_PLAY_0DATA_MS (2000)
#define AMIXER_PLAY_0DATA_CNT ((AMIXER_PLAY_0DATA_MS * AMIXER_FS) / (1000 * AMIXER_PROC_PERIOD))

#if AUDIO_MIC_EQ || AUDIO_MIC_DRC || AUDIO_MIC_TSPS || AUDIO_MIC_ECHO
#define MIC_EFFECTS_ENABLE          1
#endif

#if G24_DUAL_CHANNEL
#define WMIC_CH_NUM                 2
#else
#define WMIC_CH_NUM                 1
#endif

#define AMIXER_AIN_CH    4

#if EVB_LCD_DEMO
#undef AMIXER_ENG_CALC
#define AMIXER_ENG_CALC             1       // 能量计算
#endif

void i2s_slaver_rec_data_read(void *handle, void *buf, uint32_t bytes);
bool s_mic_opened = false;

typedef struct _amixer_t
{
    int64_t mix_stamp;
    //Drain to hal, the only output device
    aout_interface_t *aout;
    ain_interface_t *ain;

    int32_t adc_buf[AMIXER_PROC_PERIOD*AMIXER_AIN_CH];//adc buffer
#if AUDIO_KARAOKE_LINEIN_EN
    int32_t aux_buf[AMIXER_PROC_PERIOD*2]; //双声道aux buffer
#endif
#if AUDIO_KARAOKE_MIC_EN
    int32_t Lmic_buf[AMIXER_PROC_PERIOD*2]; //双声道Local mic buffer
#endif
    //Get from app
    void*         atrack[AMIXER_AUDIOTRACK_MAX_NUM];          //各个音频通路句柄
    void*         downlink_buffer[AMIXER_AUDIOTRACK_MAX_NUM]; //各个音频通路的数据buffer
    uint8_t       atrack_flags[AMIXER_AUDIOTRACK_MAX_NUM];    //各个音频通路的标志（例如标记需要做音乐音效处理)
    uint8_t       atrack_mode[AMIXER_AUDIOTRACK_MAX_NUM];     //音频通路的模式（用于区分数据类型：提示音,aux，无线mic,有线mic, usbplay等）
    uint8_t wrx_atrack_id[WMIC_CH_NUM];     //无线MIC通路句柄下标
    uint8_t usbplay_atrack_id;              //usbplay通路句柄下标
    uint8_t i2s_slaver_play_atrack_id;      //i2s slaver play通路句柄下标
    uint8_t spdif_play_atrack_id;

    //Total mix buffer
    void*        playback_buffer;  //播放buffer
    pcm_mixer_hook_t mixer_hook; //混音函数
    volatile uint8_t      status; //amixer状态
    void *listener_buffer; //usbrec监听数据buffer
    void *i2s_slaver_rec_buffer;   //i2s slaver录音数据buffer
    void *i2s_slaver_usb_buffer;
#if SPDIF_TX_EN
    void *spdif_rec_buffer;      //spdif输出buffer
#endif
#if SPDIF_RX_EN
    //void *spdif_rx_buffer;      //spdif输入buffer
#endif    

    void *listener_temp_buffer;  //监听临时 buffer
    void *listener_ring_buffer;  //监听ring buffer
    struct ring_buffer listener_ring_buffer_ctrl;

    volume_control_t Lmic_listener_dry_vol_ctrl; //有线MIC干声监听音量控制器
    volume_control_t Lmic_wet_vol_ctrl;          //有线MIC湿声音量控制器
    volume_control_t Lmic_play_dry_vol_ctrl;     //有线MIC干声播放音量控制器
    volume_control_t Lmic_g24_tx_dry_vol_ctrl;   //有线MIC干声2.4G发送音量控制器

    volume_control_t wrx_listener_dry_vol_ctrl;  //无线MIC干声监听音量控制器
    volume_control_t wrx_wet_vol_ctrl;           //无线MIC湿声音量控制器
    volume_control_t wrx_play_dry_vol_ctrl;      //无线MIC干声播放音量控制器

    volume_control_t mic_wet_listener_vol_ctrl;  //MIC(Lmic+wrx)湿声监听音量控制器
    volume_control_t mic_wet_play_vol_ctrl;      //MIC(Lmic+wrx)湿声音量控制器
    volume_control_t mic_wet_g24_tx_vol_ctrl;    //MIC(Lmic+wrx)湿声2.4G发送音量控制器

    volume_control_t tone_listener_vol_ctrl;     //提示音监听音量控制器
    volume_control_t tone_play_vol_ctrl;         //提示音播放音量控制器
    volume_control_t tone_g24_tx_vol_ctrl;       //提示音2.4G发送音量控制器

    volume_control_t aux_listener_vol_ctrl;      //aux监听音量控制器
    volume_control_t aux_play_vol_ctrl;          //aux播放音量控制器
    volume_control_t aux_g24_tx_vol_ctrl;        //aux到2.4G发送音量控制器
    
    volume_control_t playback_vol_ctrl;          //音乐音效输出音量控制器
    volume_control_t record_vol_ctrl;            //record输入音量控制器
#if USB_DEVICE_EN
    volume_control_t usbplay_listener_vol_ctrl;  //usbplay监听音量控制器
    volume_control_t usbplay_play_vol_ctrl;      //usbplay播放音量控制器
    volume_control_t usbplay_g24_tx_vol_ctrl;    //usbplay到2.4G发送音量控制器
#endif
    volume_control_t wrx_safe_mode_vol_ctrl;    //wrx safe mode音量控制器

    float Lmic_listener_dry_vol_db;              //有线MIC干声监听音量(dB)
    float Lmic_wet_vol_db;                       //有线MIC湿声音量(dB)
    float Lmic_play_dry_vol_db;                  //有线MIC干声播放音量(dB)
    float Lmic_g24_tx_dry_vol_db;                //有线MIC干声2.4G发送音量(dB)

    float wrx_listener_dry_vol_db;               //无线MIC干声监听音量(dB)
    float wrx_wet_vol_db;                        //无线MIC湿声音量(dB)
    float wrx_play_dry_vol_db;                   //无线MIC干声播放音量(dB)

    float mic_wet_listener_vol_db;               //MIC(Lmic+wrx)湿声监听音量(dB)
    float mic_wet_play_vol_db;                   //MIC(Lmic+wrx)湿声播放音量(dB)
    float mic_wet_g24_tx_vol_db;                 //MIC(Lmic+wrx)湿声2.4G发送音量(dB)

    float tone_listener_vol_db;                  //提示音监听音量(dB)
    float tone_play_vol_db;                      //提示音播放音量(dB)
    float tone_g24_tx_vol_db;                    //提示音2.4G发送音量(dB)

    float aux_listener_vol_db;                   //aux监听音量(dB)
    float aux_play_vol_db;                       //aux播放音量(dB)
    float aux_g24_tx_vol_db;                     //aux到2.4G发送音量(dB)
    
    float playback_vol_db;                       //音乐音效输出音量(dB)
    float record_vol_db;                         //record输入音量(dB)

    float usbplay_listener_vol_db;               //usbplay监听音量(dB)
    float usbplay_play_vol_db;                   //usbplay播放音量(dB)
    float usbplay_g24_tx_vol_db;                 //usbplay到2.4G发送音量(dB)

    float wrx_safe_mode_vol_db;                  //wrx safe mode音量(dB)

    uint8_t Lmic_listener_dry_vol_mute;          //有线MIC干声监听静音
    uint8_t Lmic_wet_vol_mute;                   //有线MIC湿声静音
    uint8_t Lmic_play_dry_vol_mute;              //有线MIC干声播放静音
    uint8_t Lmic_g24_tx_dry_vol_mute;            //有线MIC干声2.4G发送静音

    uint8_t wrx_listener_dry_vol_mute;           //无线MIC干声监听静音
    uint8_t wrx_wet_vol_mute;                    //无线MIC湿声静音
    uint8_t wrx_play_dry_vol_mute;               //无线MIC干声播放静音

    uint8_t mic_wet_listener_vol_mute;           //MIC(Lmic+wrx)湿声监听静音
    uint8_t mic_wet_play_vol_mute;               //MIC(Lmic+wrx)湿声播放静音
    uint8_t mic_wet_g24_tx_vol_mute;             //MIC(Lmic+wrx)湿声2.4G发送静音

    uint8_t tone_listener_vol_mute;              //提示音监听静音
    uint8_t tone_play_vol_mute;                  //提示音播放静音
    uint8_t tone_g24_tx_vol_mute;                //提示音2.4G发送静音

    uint8_t aux_listener_vol_mute;               //aux监听静音
    uint8_t aux_play_vol_mute;                   //aux播放静音
    uint8_t aux_g24_tx_vol_mute;                 //aux到2.4G发送静音
    
    uint8_t playback_vol_mute;                   //音乐音效输出静音
    uint8_t record_vol_mute;                     //record输入静音

    uint8_t usbplay_listener_vol_mute;           //usbplay监听静音
    uint8_t usbplay_play_vol_mute;               //usbplay播放静音
    uint8_t usbplay_g24_tx_vol_mute;             //usbplay到2.4G发送静音
    uint8_t usbplay_mux_sel;                     //0:音乐音效前混入; 1:音乐音效后混入

    uint8_t playback_volume;                     //音乐播放音量
    uint8_t record_volume;                       //record输入音量

    void *mic_wet_buffer; //MIC(Lmic+wrx)湿声buffer
    void *play_mic_dry_buffer; //本地+无线MIC干声播放buffer
    void *mic_dry_temp_buffer[2]; //本地+无线MIC干声临时buffer
    void *listener_mic_dry_buffer;//本地+无线MIC干声usb监听buffer
    void *usbrec_handle; //usb录音组件句柄
    void *i2s_usb_rec_handle;   //i2s传过来的数据
#if SPDIF_RX_EN
    void *spdif_rx_handle;  //spdif的传过来的数据
#endif
#if SPDIF_TX_EN
    void *spdif_tx_handle;  //spdif发送
#endif
#if I2S0_EN && I2S0_SLAVER_REC_EN
    void *i2s_slaver_rec_handle;    //i2s slaver录音组件句柄
#endif
    void *wrec_handle[2]; //无线录音组件
    void *g24_tx_buffer[2];//2.4G发送buffer
    
    uint8_t is_dongle; //用于区分dongle还是耳机

    void *wrx_track[WMIC_CH_NUM];
#if USB_DEVICE_EN
    void *usbplay_track;
#endif

#if I2S0_EN && I2S0_SLAVER_PLAY_EN
    void *i2s_slaver_play_track;        //i2s slaver播放
#endif
    void* dry_listener_duck_handle;     //干声监听buf的闪避句柄
    void* dry_play_duck_handle;         //干声播放buf的闪避句柄
    bool duck_enable;                   //闪避使能

    volatile uint32_t play_0data_cnt; //0数据播放计数器
    OAL_THREAD task_handle;           //线程句柄
    uint8_t      exit;                //线程退出标识变量

    OAL_MUTEX cmd_lock;               //cmd互斥锁避免并发调用cmd问题
    OAL_QUEUE cmd_queue;              //cmd消息队列
    void*        amsg_comp;           //音频消息组件

#if USE_EFFECTS
    void*        amixer_effects;     //音乐音效句柄
    void*        e_effects;
    uint8_t e_effects_ch_num;
#endif
    void*        mic_effects;

#if AMIXER_ENG_CALC
    pcm_reformat_hook_t mic2engcalc_refmt;
    pcm_mixer_hook_t    mic2engcalc_mixer;
    int16_t  eng_buf[AMIXER_PROC_PERIOD];
    uint8_t  eng_idx;
#endif

    void *aenc_handle;   //Local Mic降噪句柄
    uint8_t aenc_enable; //Local Mic降噪开关

    uint8_t wrx_ch_mix_mode;
#if AUDIO_KARAOKE_LINEIN_EN
    uint32_t aux_ch_cnt;//aux多少个声道
    uint32_t aux_chidx0;//aux声道0对应的ADC ch idx
    uint32_t aux_chidx1;//aux声道1对应的ADC ch idx
#endif
#if AUDIO_KARAOKE_MIC_EN
    uint32_t Lmic_ch_cnt;//Lmic多少个声道
    uint32_t Lmic_chidx0;//Lmic声道0对应的ADC ch idx
    uint32_t Lmic_chidx1;//Lmic声道1对应的ADC ch idx
#endif
    void *ve_handle;
#if AUDIO_MIC_VOCALENHANCE 
    void *ve_inbuf;
    void *ve_outbuf;
#endif    
    bool ve_enable;

#if AUDIO_MIC_AFC
    void *afc_manager_handle;
    int16_t *afc_rxin_buf;
    int16_t *afc_txin_buf;
    int16_t *afc_txout_buf;
#endif
#if AUDIO_DONGLE_BOARD_I2S_AFC
    void *dongle_mic_i2s_afc_handle;
#endif
#if AUDIO_BT_BOARD_I2S_AFC
    void *dsp_mic_i2s_afc_handle;
#endif
#if AUDIO_PLAYBACK_VOLCUT
    bool vocal_cut_en;
    void *vocut;
#endif
    //debug使用
    uint32_t proc_cnt;
}amixer;


//amixer设计成单例模式
amixer* g_amixer;
OAL_THREAD track_handler;

#if AUDIO_G24_RX_DECODER_DISABLE
#define AEFX_TYPE_REVERB_CPU0                   1
#else
#define AEFX_TYPE_REVERB_CPU0                   0
#endif

#if AUDIO_MIC_TSPS
#define AUDIO_TYPE_FC       1
#else 
#define AUDIO_TYPE_FC       0
#endif
const aeffect_list_info amixer_cpu0_aeffects = {
    .audiofx = {
        AEFX_TYPE_EQ14,
        AEFX_TYPE_DRC,
    },
    .audiofx_num = 2,
};

const aeffect_list_info amixer_cpu1_aeffects = {
    .audiofx = {
        AUDIO_PLAYBACK_VSS*(AEFX_TYPE_VSS),
    },
    .audiofx_num = AUDIO_PLAYBACK_VSS,
};

const aeffect_list_info e_cpu0_aeffects = {
    .audiofx = {
        AEFX_TYPE_REVERB_CPU0*AEFX_TYPE_REVERB,
    },
    .audiofx_num = AEFX_TYPE_REVERB_CPU0,
};

const aeffect_list_info e_cpu1_aeffects = {
    .audiofx = {
        (!AEFX_TYPE_REVERB_CPU0)*AEFX_TYPE_REVERB,
    },
    .audiofx_num = (!AEFX_TYPE_REVERB_CPU0),
};

const aeffect_list_info mic_cpu0_aeffects = {
    .audiofx = {
        AEFX_TYPE_EQ14,
        AEFX_TYPE_DRC,
        AEFX_TYPE_FREQ_SHIFT,
    },
    .audiofx_num = 3,
};

const aeffect_list_info mic_cpu1_aeffects = {
    .audiofx = {
        (AEFX_TYPE_TSPS),
        (AEFX_TYPE_ECHO),
        (AEFX_TYPE_FORMANT),
    },
    .audiofx_num = 3*(AUDIO_MIC_TSPS||AUDIO_MIC_ECHO||AUDIO_TYPE_FC),
};


typedef struct atrack_mount_info_s {
    void *atrack;
    void *downlink_buffer;
    uint8_t id;
    uint8_t flags;
    uint8_t mode;
} atrack_mount_info_t;

int client_request_aout_dev(amixer* am, uint8_t type);
int client_release_audio_track(amixer* am, atrack_mount_info_t *info);
atrack_init_rply client_request_audio_track(amixer* am, audio_track_init_t* init,
    atrack_mount_info_t *info);

void *usbrec_create(void);
void usbrec_store_data(void *handle, void *buf, uint32_t samples);
void usbrec_data_read(void *handle, void *buf, uint32_t bytes);

void *aux_track_create(void);
void aux_track_start(void *handle);
void aux_mute_set(void *handle, uint8_t mute);
uint8_t aux_mute_get(void *handle);
void aux_volume_set(void *handle, float vol_db);
float aux_volume_get(void *handle);

void *Lmic_track_create(void);
void Lmic_track_start(void *handle);
void Lmic_mute_set(void *handle, uint8_t mute);
uint8_t Lmic_mute_get(void *handle);
void Lmic_volume_set(void *handle, float vol_db);
float Lmic_volume_get(void *handle);

void *usbplay_track_create(void);
void usbplay_track_start(void *handle, uint32_t fs);
bool usbplay_ready_get(void *handle);

void usbplay_track_write(void *handle, void *buf, uint32_t samples);
void usbplay_mute_set(void *handle, uint8_t mute);
uint8_t usbplay_mute_get(void *handle);
void usbplay_volume_set(void *handle, float vol_db);
float usbplay_volume_get(void *handle);

void *Lmic_aenc_bwe_open(uint32_t fs, uint32_t period);
void Lmic_aenc_bwe_close(void *handle);
void Lmic_aenc_bwe_proc(void *handle, void *rxinput, void *txinput,
    void *rxoutput, void *txoutput);


#if 0
static int32_t data48k_0dB_stereo_32bit[96]={
 0x00000000,0x00000000,0x10B5150F,0x10B5150F,0x2120FB82,0x2120FB82,0x30FBC54C,0x30FBC54C,
 0x3FFFFFFE,0x3FFFFFFE,0x4DEBE4FD,0x4DEBE4FD,0x5A827998,0x5A827998,0x658C9A2B,0x658C9A2B,
 0x6ED9EB9F,0x6ED9EB9F,0x7641AF3A,0x7641AF3A,0x7BA3751B,0x7BA3751B,0x7EE7AA49,0x7EE7AA49,
 0x7FFFFFFD,0x7FFFFFFD,0x7EE7AA49,0x7EE7AA49,0x7BA3751B,0x7BA3751B,0x7641AF3A,0x7641AF3A,
 0x6ED9EB9F,0x6ED9EB9F,0x658C9A2B,0x658C9A2B,0x5A827998,0x5A827998,0x4DEBE4FD,0x4DEBE4FD,
 0x3FFFFFFE,0x3FFFFFFE,0x30FBC54C,0x30FBC54C,0x2120FB82,0x2120FB82,0x10B5150F,0x10B5150F,
 0x00000000,0x00000000,0xEF4AEAF0,0xEF4AEAF0,0xDEDF047D,0xDEDF047D,0xCF043AB3,0xCF043AB3,
 0xC0000001,0xC0000001,0xB2141B02,0xB2141B02,0xA57D8667,0xA57D8667,0x9A7365D4,0x9A7365D4,
 0x91261460,0x91261460,0x89BE50C5,0x89BE50C5,0x845C8AE4,0x845C8AE4,0x811855B6,0x811855B6,
 0x80000002,0x80000002,0x811855B6,0x811855B6,0x845C8AE4,0x845C8AE4,0x89BE50C5,0x89BE50C5,
 0x91261460,0x91261460,0x9A7365D4,0x9A7365D4,0xA57D8667,0xA57D8667,0xB2141B02,0xB2141B02,
 0xC0000001,0xC0000001,0xCF043AB3,0xCF043AB3,0xDEDF047D,0xDEDF047D,0xEF4AEAF0,0xEF4AEAF0
 };
static uint32_t data_index;

/* 使用正弦波数据代替原音频数据 */
AMIXER_SRAM_FUNC
void sin_debug(void * buf)
{
    int32_t *p32 = (int32_t*)buf;
    for (int i = 0; i < AMIXER_PROC_PERIOD; i++) {
        p32[2*i+1] = p32[2*i] = data48k_0dB_stereo_32bit[data_index]*0.01f;
        data_index = (data_index + 2) % 96;
    }
}
#endif

AMIXER_BANK_FUNC
void __get_hal_runtime_stu(amixer* am, uint32_t rev)
{
    uint32_t *stu = (uint32_t*)rev;
    if(stu && am->aout) {
        *stu = PCM_HW_STATE_IDLE;
        *stu = am->aout->get_status(am->aout);
    }

}

AMIXER_BANK_FUNC
void __amixer_push_waudiotrack_release_semlock(amixer* am, uint32_t val)
{
    int track_id = val;
    if(track_id >= 0 && track_id < AMIXER_AUDIOTRACK_MAX_NUM) {
        controlWAudioTrack(am->atrack[track_id], WAUDIOTRACK_CMD_GIVE_A_SHIT, 0);
    }
}

AMIXER_BANK_FUNC
void __amxier_decide_sharebuffer_size(amixer* am, uint32_t val, uint32_t rev)
{
    audio_track_init_t *init = (audio_track_init_t*)val;
    uint32_t *bufsize = (uint32_t*)rev;
    if(bufsize && init) {
        #if 0
        *bufsize = ((init->sharebuf_samples + AMIXER_PROC_PERIOD - 1) /
                AMIXER_PROC_PERIOD) * AMIXER_PROC_PERIOD *
                AMIXER_CH * AMIXER_BPS;
        if (*bufsize < 2 * AMIXER_PROC_PERIOD * AMIXER_CH * AMIXER_BPS) {
            *bufsize = 2 * AMIXER_PROC_PERIOD * AMIXER_CH * AMIXER_BPS;
        }
        #endif
        *bufsize = ((init->sharebuf_samples + AMIXER_PROC_PERIOD - 1) /
                AMIXER_PROC_PERIOD) * AMIXER_PROC_PERIOD *
                init->ch * init->sample_size;
    }
}

AMIXER_BANK_FUNC
int amixer_event_listener(void* handle, uint32_t who, uint32_t cmd, uint32_t val, uint32_t rev)
{
    amixer* am = (amixer*)handle;
    if(am) {
        if(AUDIO_NOTIFY_AMSG == who) {
            switch(cmd) {
                case AMSG_MSG_SENT:
                {
                    break;
                }
                case AMSG_MSG_SENDING:
                {
                    amsg_t   *msg = (amsg_t*)val;
                    uint32_t *msg_rev = (uint32_t*)rev;
                    if(msg && msg_rev) {
                        *msg_rev = OAL_queue_send(&am->cmd_queue, msg, 2000);
                    }
                    break;
                }
            }
        }
        else if(AUDIO_NOTIFY_AEFFECT_COMPONENT == who)
        {

        }
    }
    return 0;
}

void amixer_reload_eq()
{
    logi("reload eq ########## ");
    amixer_xet_ctrl(AMIXER_CMD_RELOAD_EQ, 0, 0);
}



AMIXER_SRAM_FUNC
int amixer_xet_ctrl(uint32_t cmd, uint32_t val, uint32_t rev)
{
    amixer *am = g_amixer;
    if(!am) {
        return -1;
    }
    int ret = 0;
    switch(cmd) {
        case AMIXER_CMD_PLAYSTAMP_INFO_GET:
        {
            audio_stamp_info_t *info = (audio_stamp_info_t *)rev;
            int64_t basestamp = 0;

            controlWAudioTrack(am->atrack[val],
                WAUDIOTRACK_CMD_BASESTAMP_GET, (uint32_t)&basestamp);

            if(am->aout && info) {
                OAL_enter_critical_section();
                //info->play_stamp = am->aout->get_play_samples(am->aout);
                //info->mix_stamp = am->mix_stamp;
                info->play_stamp = am->aout->get_play_samples(am->aout) - basestamp;
                info->mix_stamp = am->mix_stamp - basestamp;
                info->sys_stamp = HAL_timestamp_to_us(HAL_get_timestamp());
                info->bt_stamp = 0; //FIXME
                OAL_exit_critical_section();
            }

            break;
        }

        case AMIXER_CMD_TRACKPLAYSAMPLES_GET:
        {
            int64_t basestamp = 0;
            ret = controlWAudioTrack(am->atrack[val],
                WAUDIOTRACK_CMD_BASESTAMP_GET, (uint32_t)&basestamp);
            if (ret < 0) {
                *((int64_t*)rev) = 0;
            } else {
                *((int64_t*)rev) = am->aout->get_play_samples(am->aout) - basestamp;
            }
            break;
        }

        case AMIXER_CMD_AOUTPLAYSAMPLES_GET:
            if (am->aout && am->aout->get_play_samples) {
                *((int64_t*)rev) = am->aout->get_play_samples(am->aout);
            } else {
                *((int64_t*)rev) = 0;
            }
            break;

        case AMIXER_CMD_MIXSTAMP_GET:
        {
            *((int64_t*)rev) = am->mix_stamp;
            break;
        }


        case AMIXER_CMD_PLAYSPEED_SET:
        {
            //TODO
            break;
        }

        case AMIXER_CMD_TRACK_DATA_STA_GET:
        {
            uint32_t hal_stu = PCM_HW_STATE_IDLE;
            __get_hal_runtime_stu(am, (uint32_t)&hal_stu);
            if ((PCM_HW_STATE_RUNNING == hal_stu) || (PCM_HW_STATE_XRUN == hal_stu)) {
                controlWAudioTrack(am->atrack[val],
                    WAUDIOTRACK_CMD_GET_STU, (uint32_t)rev);
            } else {
                *((uint32_t*)rev) = WAUDIO_TRACK_DATA_STA_INIT;
            }
            break;
        }

        case AMIXER_CMD_TRACK_VOLUME_SET:
        {
            atrack_volume_info_t *info = (atrack_volume_info_t *)val;
            controlWAudioTrack(am->atrack[info->track_id],
                WAUDIOTRACK_CMD_VOLUME_SET, (uint32_t)&info->vol_f32);
            break;
        }

        case AMIXER_CMD_TRACK_MUTETIME_SET:
        {
            atrack_mute_info_t *info = (atrack_mute_info_t *)val;
            controlWAudioTrack(am->atrack[info->track_id],
                WAUDIOTRACK_CMD_MUTETIME_SET, (uint32_t)info->mute_ms);
            break;
        }

        case AMIXER_CMD_EFFECTS_VOL_SET:
            volume_control_set_volume(&am->aux_play_vol_ctrl, *((float*)val));
            break;

        case AMIXER_CMD_WAUDIOTRACK_GIVE_A_SHIT:
        {
            __amixer_push_waudiotrack_release_semlock(am, val);
            break;
        }
        case AMIXER_CMD_AUDIOTRACK_START:
        {
            amsg_block_msg_send(am->amsg_comp, AMIXER_CMD_AUDIOTRACK_START, val, rev);
            break;
        }
        case AMIXER_CMD_AUDIOTRACK_DRAINING:
        {
            amsg_block_msg_send(am->amsg_comp, AMIXER_CMD_AUDIOTRACK_DRAINING, val, rev);
            break;
        }
        case AMIXER_CMD_AUDIOTRACK_STOP:
        {
            amsg_block_msg_send(am->amsg_comp, AMIXER_CMD_AUDIOTRACK_STOP, val, rev);
            break;
        }

        case AMIXER_CMD_START:
        {
            amsg_nonblock_msg_send(am->amsg_comp, AMIXER_CMD_START, val, rev);
            break;
        }

        case AMIXER_CMD_STOP:
        {
            amsg_nonblock_msg_send(am->amsg_comp, AMIXER_CMD_STOP, val, rev);
            break;
        }

        case AMIXER_CMD_REQ_AUDIOTRACK:
        {
            OAL_mutex_lock(&am->cmd_lock);
            atrack_mount_info_t info;
            atrack_init_rply* air = (atrack_init_rply*)rev;
            *air = client_request_audio_track(am, (audio_track_init_t*)val, &info);
            if (info.atrack) {
                amsg_block_msg_send(am->amsg_comp, AMIXER_CMD_REQ_AUDIOTRACK, (uint32_t)&info, rev);
            }
            OAL_mutex_unlock(&am->cmd_lock);
            break;
        }
        case AMIXER_CMD_DES_AUDIOTRACK:
        {
            atrack_mount_info_t info;
            OAL_mutex_lock(&am->cmd_lock);
            amsg_block_msg_send(am->amsg_comp, AMIXER_CMD_DES_AUDIOTRACK, val, (uint32_t)&info);
            client_release_audio_track(am, &info);
            OAL_mutex_unlock(&am->cmd_lock);
            break;
        }
        case AMIXER_CMD_START_AEFFECT_ACPUX_SCHEDULE:
        {
            amsg_block_msg_send(am->amsg_comp, AMIXER_CMD_START_AEFFECT_ACPUX_SCHEDULE, val, rev);
            break;
        }
        case AMIXER_CMD_STOP_AEFFECT_ACPUX_SCHEDULE:
        {
            amsg_block_msg_send(am->amsg_comp, AMIXER_CMD_STOP_AEFFECT_ACPUX_SCHEDULE, val, rev);
            break;
        }

#if USE_EFFECTS
#if 0
        case AMIXER_CMD_ENABLE_AEFFECT_PROC:
        {
            aeffect_component_xet_ctrl(am->amixer_effects, AEFFECT_COMPONENT_CMD_ENABLE_PROC, 0, 0);
            break;
        }
        case AMIXER_CMD_DISABLE_AEFFECT_PROC:
        {

            aeffect_component_xet_ctrl(am->amixer_effects, AEFFECT_COMPONENT_CMD_DISABLE_PROC, 0, 0);
            break;
        }
#endif

        case AMIXER_CMD_GET_AEFFECT_TUNNING_AREA:
        {
            aeffect_type* atype = (aeffect_type*)val;
            void** tunning_area = (void**)rev;
            void *componet = NULL;
            switch (atype->io_type) {
            case AEFX_IO_TYPE_PLAYBACK:
                componet = am->amixer_effects;
                break;
            case AEFX_IO_TYPE_EFFECT:
                componet = am->e_effects;
                break;
            case AEFX_IO_TYPE_MIC:
                componet = am->mic_effects;
                break;
            }
            if(atype && componet && tunning_area) {
                aeffect_component_xet_ctrl(componet,
                    AEFFECT_COMPONENT_CMD_GET_ASSIGNED_TUNNINGAREA,
                    (uint32_t)atype, (uint32_t)tunning_area);
            }
            break;
        }
        case AMIXER_CMD_RESET_AEFFECT_TUNNING_AREA:
        {
            aeffect_type* atype = (aeffect_type*)val;
            void *componet = NULL;
            switch (atype->io_type) {
            case AEFX_IO_TYPE_PLAYBACK:
                componet = am->amixer_effects;
                break;
            case AEFX_IO_TYPE_EFFECT:
                componet = am->e_effects;
                break;
            case AEFX_IO_TYPE_MIC:
                componet = am->mic_effects;
                break;
            }
            if(atype && componet) {
                aeffect_component_xet_ctrl(componet,
                    AEFFECT_COMPONENT_CMD_RESET_ASSIGNED_TUNNINGAREA, (uint32_t)atype, 0);
            }
            break;
        }
        case AMIXER_CMD_LOAD_AEFFECT_TUNNING_AREA:
        {
            aeffect_type* atype = (aeffect_type*)val;
            void *componet = NULL;
            switch (atype->io_type) {
            case AEFX_IO_TYPE_PLAYBACK:
                componet = am->amixer_effects;
                break;
            case AEFX_IO_TYPE_EFFECT:
                componet = am->e_effects;
                break;
            case AEFX_IO_TYPE_MIC:
                componet = am->mic_effects;
                break;
            }
            if(atype && componet) {
                aeffect_component_xet_ctrl(componet,
                    AEFFECT_COMPONENT_CMD_LOAD_ASSIGNED_TUNNINGAREA, (uint32_t)atype, 0);
            }
            break;
        }
#endif
        case AMIXER_CMD_GET_APPO_SHAREBUFFER_SIZE:
        {
            __amxier_decide_sharebuffer_size(am, val, rev);
            break;
        }
        case AMIXER_CMD_GET_AMIXER_PCM_CHANNEL_CFG:
        {
            uint8_t *ch = (uint8_t*)rev;
            *ch = AMIXER_CH;
            break;
        }
        case AMIXER_CMD_GET_AMIXER_PCM_BPS_CFG:
        {
            uint8_t *bps = (uint8_t*)rev;
            *bps = AMIXER_BPS;
            break;
        }
        case AMIXER_CMD_GET_AMIXER_PCM_SRATE_CFG:
        {
            uint32_t *srate = (uint32_t*)rev;
            *srate = AMIXER_FS;
            break;
        }
        case AMIXER_CMD_GET_AMIXER_PCM_PROC_PREIOD_CFG:
        {
            uint32_t *period = (uint32_t*)rev;
            *period = AMIXER_PROC_PERIOD;
            break;
        }
        case AMIXER_CMD_GET_AMIXER_PCM_INTERLEAVE_FMT:
        {
            uint8_t *interleave = (uint8_t*)rev;
            *interleave = AMIXER_IO_PCM_INTERLEAVE;
            break;
        }
        case AMIXER_CMD_GET_HAL_RUNTIME_STU:
        {
            __get_hal_runtime_stu(am, rev);
            break;
        }
        case AMIXER_CMD_TRACK_PLAYTIME_SET:
        {
            atrack_playtime_info_t *info = (atrack_playtime_info_t *)val;
            controlWAudioTrack(am->atrack[info->track_id],
                WAUDIOTRACK_CMD_PLAYTIME_SET, info->playtime);
            break;
        }
        case AMIXER_CMD_TRACK_PLAYTIME_FLAG_GET:
        {
            controlWAudioTrack(am->atrack[val],
                WAUDIOTRACK_CMD_PLAYTIME_FLAG_GET, rev);
            break;
        }

        case AMIXER_CMD_TRACK_FADE_BOOK:
        {
            atrack_fade_book_t *info = (atrack_fade_book_t *)val;
            controlWAudioTrack(am->atrack[info->track_id],
                WAUDIOTRACK_CMD_FADE_BOOK, val);
            break;
        }
        case AMIXER_CMD_AOUT_REQ:
        {
            OAL_mutex_lock(&am->cmd_lock);
            amsg_block_msg_send(am->amsg_comp, AMIXER_CMD_AOUT_REQ, val, rev);
            OAL_mutex_unlock(&am->cmd_lock);
            break;
        }
        case AMIXER_CMD_AOUT_REL:
        {
            OAL_mutex_lock(&am->cmd_lock);
            amsg_block_msg_send(am->amsg_comp, AMIXER_CMD_AOUT_REL, val, rev);
            OAL_mutex_unlock(&am->cmd_lock);
            break;
        }
#if USB_EQ_EN
        case AMIXER_CMD_RELOAD_EQ:
       {
        void load_all_eq_setting();
        void audioeffect_enable(void);

        load_all_eq_setting();
        audioeffect_enable();
       }
            break;
#endif
    }
    return ret;
}

AMIXER_BANK_FUNC
int client_release_audio_track(amixer* am, atrack_mount_info_t *info)
{
    if (info->downlink_buffer) {
        general_free(info->downlink_buffer);
    }
    if (info->atrack) {
        destroyWAudioTrack(info->atrack);
    }

    return 0;
}

AMIXER_BANK_FUNC
atrack_init_rply client_request_audio_track(amixer* am, audio_track_init_t* init,
    atrack_mount_info_t *info)
{
    atrack_init_rply air;
    int at;
    struct audio_share_buffer* sb = 0;

    info->downlink_buffer = NULL;
    info->atrack = NULL;
    info->id = AMIXER_AUDIOTRACK_INSUFFICENT;
    info->flags = 0;

    at = AMIXER_AUDIOTRACK_MAX_NUM - 1;
    while (at >= 0 && am->atrack[at]) {
        at--;
    }

    if (at >= 0) {
        info->id = at;
        info->atrack = createWAudioTrack(init, at, &sb);
        if (info->atrack) {
            //downlink buffer for amixer
            info->downlink_buffer = general_calloc(AMIXER_CH * AMIXER_BPS * AMIXER_PROC_PERIOD);
            if(!info->downlink_buffer) {
                destroyWAudioTrack(info->atrack);
                info->atrack = 0;
                at = AMIXER_AUDIOTRACK_INSUFFICENT;
            }
        }
    }

    info->mode = init->mode;
    switch(init->mode) {
        case AUDIO_MODE_NATIVE_MUSIC:
        case AUDIO_MODE_A2DP_MUSIC:
        case AUDIO_MODE_LINEIN:
            info->flags |= AUDIOTRACK_FLAGS_MUSIC_EFFECTS;
            break;
        default:
            break;
    }

    air.track_id = at;
    air.sharebuffer_ptr = (uint32_t)sb;

    return air;
}

AMIXER_BANK_FUNC
int client_request_aout_dev(amixer* am, uint8_t type)
{
    int ret = -1;
    //logi("am->status %d", am->status);
    if (am->status == AMIXER_STU_STOP) {
        if (am->aout) {
            aout_init_t init;
            init.fs = AMIXER_FS;
            init.ch = AMIXER_CH;
            init.sample_size = AMIXER_BPS;
            #if 0
            #if !AUDIO_KARAOKE_EN
            init.period_size = AMIXER_PROC_PERIOD; /* use default config */
            init.period_count = 4; /* use default config */
            #else
            init.period_size = AMIXER_PROC_PERIOD/2; /* use default config */
            init.period_count = 3; /* use default config */
            #endif
            #else
            if(!AUDIO_KARAOKE_EN || (mode_sta_get() ==MODE_BT)) {
                init.period_size = AMIXER_PROC_PERIOD; /* use default config */
                init.period_count = 4; /* use default config */
            } else {
                init.period_size = AMIXER_PROC_PERIOD/2; /* use default config */
                init.period_count = 3; /* use default config */
            }
            
            #endif
            init.fadein_ms = 100;
            init.fadeout_ms = 100;
            init.headmute_ms = 0;
            init.mode = AUDIO_MODE_PASSTHRU;
            audio_service_add_features(AUDIO_FEATURE_AMIXER);
            auto_set_cpux_freq();
            ret = am->aout->init(am->aout, &init);
            am->mix_stamp = 0;
        }
    }
    return ret;
}

void release_aout(amixer* am)
{
    if(am->status == AMIXER_STU_STOP) {
        am->aout->deinit(am->aout);
        audio_service_clear_features(AUDIO_FEATURE_AMIXER);
    }
}

AMIXER_BANK_FUNC
int client_release_aout_dev(amixer* am)
{
    int ret = -1;
    if(am->status == AMIXER_STU_STOP) {
        ret = am->aout->deinit(am->aout);
        audio_service_clear_features(AUDIO_FEATURE_AMIXER);
        auto_set_cpux_freq();
    }
    return ret;
}

AMIXER_SRAM_FUNC
int amixer_cmd_process(amixer *am, uint32_t wait_ticks)
{
    amsg_t msg;

    if (OAL_SUCCESS != OAL_queue_recv(&am->cmd_queue, &msg, wait_ticks)) {
        return -1;
    }

    switch (msg.cmd) {
        case AMIXER_CMD_AUDIOTRACK_START:
        {
            amsg_t* rply = (amsg_t*)msg.user_data;
            int track_id = (int)rply->req;
            logd("start atrack : %d", track_id);
            if(track_id >= 0 && track_id < AMIXER_AUDIOTRACK_MAX_NUM) {
                controlWAudioTrack(am->atrack[track_id], WAUDIOTRACK_CMD_START, 0);
            }
            OAL_sem_post(&rply->_lock);
            break;
        }
        case AMIXER_CMD_AUDIOTRACK_STOP:
        {
            amsg_t* rply = (amsg_t*)msg.user_data;
            int track_id = (int)rply->req;
            logd("stop atrack : %d", track_id);
            if(track_id >= 0 && track_id < AMIXER_AUDIOTRACK_MAX_NUM) {
                controlWAudioTrack(am->atrack[track_id], WAUDIOTRACK_CMD_STOP, 0);
            }

            OAL_sem_post(&rply->_lock);
            break;
        }
        case AMIXER_CMD_AUDIOTRACK_DRAINING:
        {
            amsg_t* rply = (amsg_t*)msg.user_data;
            int track_id = (int)rply->req;
            logd("draining atrack : %d", track_id);
            if(track_id >= 0 && track_id < AMIXER_AUDIOTRACK_MAX_NUM) {
                controlWAudioTrack(am->atrack[track_id], WAUDIOTRACK_CMD_DRAINING, 0);
            }
            OAL_sem_post(&rply->_lock);
            break;
        }

        case AMIXER_CMD_START:
            am->status = AMIXER_STU_RUNNING;
            break;

        case AMIXER_CMD_STOP:
            am->status = AMIXER_STU_STOP;
            break;

        case AMIXER_CMD_REQ_AUDIOTRACK:
        {
            amsg_t* rply = (amsg_t*)msg.user_data;
            atrack_mount_info_t *info = (atrack_mount_info_t*)rply->req;
            if (info->id <= AMIXER_AUDIOTRACK_MAX_NUM) {
                am->downlink_buffer[info->id] = info->downlink_buffer;
                am->atrack[info->id] = info->atrack;
                am->atrack_flags[info->id] = info->flags;
                am->atrack_mode[info->id] = info->mode;

                switch (info->mode) {
                case AUDIO_MODE_WMIC:
                    am->wrx_atrack_id[0] = info->id;
                    break;
#if G24_DUAL_CHANNEL
                case AUDIO_MODE_WMIC1:
                    am->wrx_atrack_id[1] = info->id;
                    break;
#endif
                case AUDIO_MODE_USBPLAY:
                    am->usbplay_atrack_id = info->id;
                    break;

                case AUDIO_MODE_I2S_SLAVER_PLAY:
                    am->i2s_slaver_play_atrack_id = info->id;
                    break;
                case AUDIO_MODE_SPDIF:
                    logi("spdif set id... %d", info->id);
                    am->spdif_play_atrack_id = info->id;
                    break;
                }
            }
            if(am->status == AMIXER_STU_STOP) {
                client_request_aout_dev(am, 0);
                am->status = AMIXER_STU_RUNNING;
            }
            am->play_0data_cnt = 0;
            OAL_sem_post(&rply->_lock);
            break;
        }
        case AMIXER_CMD_DES_AUDIOTRACK:
        {
            amsg_t* rply = (amsg_t*)msg.user_data;
            atrack_mount_info_t *info = (atrack_mount_info_t *)rply->rly;
            if (rply->req < AMIXER_AUDIOTRACK_MAX_NUM) {
                logd("release %d track", rply->req);
                info->atrack = am->atrack[rply->req];
                info->downlink_buffer = am->downlink_buffer[rply->req];
                info->flags = am->atrack_flags[rply->req];
                info->id = rply->req;
                am->atrack[rply->req] = NULL;
                am->downlink_buffer[rply->req] = NULL;
                am->atrack_flags[rply->req] = 0;

                switch (am->atrack_mode[info->id]) {
                case AUDIO_MODE_WMIC:
                    am->wrx_atrack_id[0] = 0xFF;
                    break;
#if G24_DUAL_CHANNEL
                case AUDIO_MODE_WMIC1:
                    am->wrx_atrack_id[1] = 0xFF;
                    break;
#endif
                case AUDIO_MODE_USBPLAY:
                    am->usbplay_atrack_id = 0xFF;
                    break;

                case AUDIO_MODE_I2S_SLAVER_PLAY:
                    am->i2s_slaver_play_atrack_id = 0xFF;
                    break;
                }

                am->atrack_mode[info->id] = 0;
            } else {
                info->atrack = NULL;
                info->downlink_buffer = NULL;
                info->flags = 0;
                info->id = rply->req;
            }

            int at = AMIXER_AUDIOTRACK_MAX_NUM;
            int active_cnt = 0;
            while(at--) {
                if(am->atrack[at]) {
                    active_cnt++;
                }
            }
            if (0 == active_cnt) {
                am->play_0data_cnt = AMIXER_PLAY_0DATA_CNT;
            }

            OAL_sem_post(&rply->_lock);
            break;
        }
#if USE_EFFECTS
        case AMIXER_CMD_START_AEFFECT_ACPUX_SCHEDULE:
        {
            amsg_t* rply = (amsg_t*)msg.user_data;
            aeffect_component_xet_ctrl(am->amixer_effects, AEFFECT_COMPONENT_CMD_START_ACPUX_INSTANCE, 0, 0);
            OAL_sem_post(&rply->_lock);
            break;
        }
        case AMIXER_CMD_STOP_AEFFECT_ACPUX_SCHEDULE:
        {
            amsg_t* rply = (amsg_t*)msg.user_data;
            aeffect_component_xet_ctrl(am->amixer_effects, AEFFECT_COMPONENT_CMD_STOP_ACPUX_INSTANCE, 0, 0);
            OAL_sem_post(&rply->_lock);
            break;
        }
#endif
        case AMIXER_CMD_AOUT_REQ:
        {
            amsg_t* rply = (amsg_t*)msg.user_data;
            if(am->status == AMIXER_STU_STOP) {
                client_request_aout_dev(am, 0);
                am->status = AMIXER_STU_RUNNING;
            }
            am->play_0data_cnt = 0;

#if POWER_LPC_MODE_EN
            amixer_mode_lpc_off(am);
#endif

            OAL_sem_post(&rply->_lock);
            break;
        }
        case AMIXER_CMD_AOUT_REL:
        {
            amsg_t* rply = (amsg_t*)msg.user_data;
            //uint8_t type = rply->req;
            //if(type == 0) {
                am->play_0data_cnt = AMIXER_PLAY_0DATA_CNT;
            //} else if(type == 1) {
            //    am->play_1data_cnt = AMIXER_PLAY_1DATA_CNT;
            //}
            OAL_sem_post(&rply->_lock);
            break;
        }

        default:
            break;
    }

    return 0;
}


AMIXER_SRAM_FUNC
void amixer_debug(void *handle)
{
    amixer *am = (amixer *)handle;
    am->proc_cnt++;

    if (am->proc_cnt % 4000 == 0) {
#if G24_DUAL_CHANNEL
        __maybe_unused wrx_debug_t dbg, dbg1;
        dbg = wrx_debug_info_get(am->wrx_track[0]);
        dbg1 = wrx_debug_info_get(am->wrx_track[1]);
        logd("INFO:(w_rx:%d,%d,%d,%d,%d; %d,%d,%d,%d,%d), (w_tx:%d,%d,%d)",
            dbg.debug_wrx_normal_cnt, dbg.debug_wrx_loss_cnt, dbg.debug_wrx_bad_cnt,
            dbg.debug_wrx_overrun_cnt, dbg.debug_wrx_underrun_cnt,
            dbg1.debug_wrx_normal_cnt, dbg1.debug_wrx_loss_cnt, dbg1.debug_wrx_bad_cnt,
            dbg1.debug_wrx_overrun_cnt, dbg1.debug_wrx_underrun_cnt,
            debug_wrec_normal_cnt, debug_wrec_underrun_cnt, debug_wrec_overrun_cnt
        );
#else
        __maybe_unused wrx_debug_t dbg;
        dbg = wrx_debug_info_get(am->wrx_track[0]);
        logd("INFO:(w_rx:%d,%d,%d,%d,%d), (w_tx:%d,%d,%d)",
            dbg.debug_wrx_normal_cnt, dbg.debug_wrx_loss_cnt, dbg.debug_wrx_bad_cnt,
            dbg.debug_wrx_overrun_cnt, dbg.debug_wrx_underrun_cnt,
            debug_wrec_normal_cnt, debug_wrec_underrun_cnt, debug_wrec_overrun_cnt
        );
#endif
    }
}

#if AMIXER_ENG_CALC

#if EVB_LCD_DEMO
typedef struct _audio_eng_t
{
    uint32_t eng_cal_total;
    uint8_t eng_cal;
    uint8_t eng_last;
} audio_eng_t;

audio_eng_t mic_eng_ctrl[2];
#endif

#define ENG_UP_LIMIT     0.0f
#define ENG_LOW_LIMIT    -70.0f
float music_eng = ENG_LOW_LIMIT;
float mic_eng   = ENG_LOW_LIMIT;

#define ENG_CALC_DEBUG (1)

#define INT16_TO_REAL(val) ((float)(val) / (float)32768.0f)
extern float general_log2(float x);

AMIXER_SRAM_FUNC
void eng_buf_reset(void *handle)
{
    amixer *am = (amixer*)handle;
    int16_t *dst = am->eng_buf;
    libc_memset(dst, 0x00, sizeof(int16_t)*AMIXER_PROC_PERIOD);
}

AMIXER_SRAM_FUNC
void eng_calc_submit_data_stereo(void *handle, void *src, bool mix_pre)
{
    amixer *am = (amixer*)handle;
    int16_t *dst = am ->eng_buf;
    if(mix_pre) {
        if(am ->mic2engcalc_mixer) {
            am ->mic2engcalc_mixer(dst, src, AMIXER_PROC_PERIOD);
        }
    }
    else {
        if(am ->mic2engcalc_refmt) {
            am ->mic2engcalc_refmt(dst, src, AMIXER_PROC_PERIOD);
        }
    }
}

AMIXER_SRAM_FUNC
float eng_calcing(void *handle, int8_t dc_cut)
{
    amixer *am = (amixer*)handle;
    int16_t *calc_buf = am ->eng_buf;
    float power = 0.0f;

#if 0//ENG_USE_DC_CUT
    if(dc_cut)
    {
        afx_dc_cut(ok->eng_dc_cut, (void*)calc_buf, (void*)calc_buf, AMIXER_PROC_PERIOD);
    }
#endif

    for(int i = 0; i < AMIXER_PROC_PERIOD; i++)
    {
        float data = INT16_TO_REAL(calc_buf[i]);
        power += data*data;
    }
    power /= ((float)AMIXER_PROC_PERIOD);
    power = 3.01029995663981f*general_log2(power);
    return power;
}

#if ENG_CALC_DEBUG
static uint8_t cnt1 = 0, cnt2 = 0;
#endif

AMIXER_SRAM_FUNC
void music_eng_sync(float db)
{
    music_eng = db < ENG_LOW_LIMIT?
                ENG_LOW_LIMIT:(db > ENG_UP_LIMIT?ENG_UP_LIMIT:db);
#if ENG_CALC_DEBUG
    if((cnt1++ % 16) == 0) {
        loge("music : %d", (uint8_t)((int)(music_eng - ENG_LOW_LIMIT)));
    }
#endif
}

AMIXER_SRAM_FUNC
void mic_eng_sync(uint8_t index, float db)
{
    mic_eng = db < ENG_LOW_LIMIT?
                ENG_LOW_LIMIT:(db > ENG_UP_LIMIT?ENG_UP_LIMIT:db);
#if EVB_LCD_DEMO
    mic_eng_ctrl[index].eng_cal_total += (uint8_t)((int)(mic_eng - ENG_LOW_LIMIT));
    if((cnt2++ % 3) == 0) {
        mic_eng_ctrl[index].eng_cal_total += mic_eng_ctrl[index].eng_last;
        mic_eng_ctrl[index].eng_cal = mic_eng_ctrl[index].eng_cal_total/4;
        mic_eng_ctrl[index].eng_last = mic_eng_ctrl[index].eng_cal;
        mic_eng_ctrl[index].eng_cal_total = 0;
        
        //loge("mic[%d] : %d", index, (uint8_t)((int)(mic_eng - ENG_LOW_LIMIT)));
    }
#elif ENG_CALC_DEBUG
    if((cnt2++ % 16) == 0) {
        loge("mic : %d", (uint8_t)((int)(mic_eng - ENG_LOW_LIMIT)));
    }
#endif
}
#endif

/*
    0 - 128
*/
AMIXER_SRAM_FUNC
uint8_t music_eng_lvl_get()
{
    uint8_t ret = 0;
#if AMIXER_ENG_CALC
    ret = (uint8_t)((int)(music_eng - ENG_LOW_LIMIT));
#endif
    return ret;
}

AMIXER_SRAM_FUNC
uint8_t mic_eng_lvl_get(uint8_t index)
{
    uint8_t ret = 0;
#if AMIXER_ENG_CALC
#if EVB_LCD_DEMO
    ret = mic_eng_ctrl[index].eng_cal;
#else
    ret = (uint8_t)((int)(mic_eng - ENG_LOW_LIMIT));
#endif
#endif
    return ret;
}



#if AUDIO_PLAYBACK_VOLCUT

void audio_vocal_cut_set(uint8_t enable)
{
    amixer* am = g_amixer;

    if(am->vocut) {
        am->vocal_cut_en = enable;
        logi("am->vocal_cut :%d", am->vocal_cut_en);
    }
}

#endif


AMIXER_SRAM_FUNC
void amixer_all_atrack_data_get(amixer *am)
{
    int at = AMIXER_AUDIOTRACK_MAX_NUM;
    while(at--) {
        if(am->atrack[at] && am->downlink_buffer[at]) {
            debufferWAudioTrack(am->atrack[at], am->downlink_buffer[at], AMIXER_PROC_PERIOD);
        }
    }
}

AMIXER_SRAM_FUNC
void amixer_Lmic_effects_proc(amixer *am)
{
#if AUDIO_KARAOKE_MIC_EN

#if 0//MIC_EFFECTS_ENABLE      //这部分需要根据实际使用的音效和效果，调整放降噪前还是降噪后
    stereo32_to_mono32(am->Lmic_buf, am->Lmic_buf, AMIXER_PROC_PERIOD);
    aeffect_component_proc_period(am->mic_effects, am->Lmic_buf, AMIXER_PROC_PERIOD);
    mono32_to_stereo32(am->Lmic_buf, am->Lmic_buf, AMIXER_PROC_PERIOD);
#endif

#if (AUDIO_MIC_AENC || AUDIO_MIC_NSBWE_AENC)
    if (am->aenc_enable) {
        static int16_t tmp[AMIXER_PROC_PERIOD];
        Lmic_aenc_bwe_proc(am->aenc_handle, tmp, am->Lmic_buf,
                tmp, am->Lmic_buf);
    } else {
#if !INPUT_SEL_IS_1CH(AUDIO_REC_INPUT_SEL)
        stereo32_to_mono32(am->Lmic_buf, am->Lmic_buf, AMIXER_PROC_PERIOD);
        mono32_to_stereo32(am->Lmic_buf, am->Lmic_buf, AMIXER_PROC_PERIOD);
#endif
    }
#endif

#if MIC_EFFECTS_ENABLE      //这部分需要根据实际使用的音效和效果，调整放降噪前还是降噪后
    stereo32_to_mono32(am->Lmic_buf, am->Lmic_buf, AMIXER_PROC_PERIOD);
    aeffect_component_proc_period(am->mic_effects, am->Lmic_buf, AMIXER_PROC_PERIOD);
    mono32_to_stereo32(am->Lmic_buf, am->Lmic_buf, AMIXER_PROC_PERIOD);
#endif

#if AUDIO_MIC_VOCALENHANCE
    if (am->ve_enable) {
        
        stereo32_to_mono16(am->ve_inbuf, am->Lmic_buf, AMIXER_PROC_PERIOD);
        vocalenhance_proc(am->ve_handle, am->ve_inbuf, am->ve_outbuf);
        mono16_to_stereo32(am->Lmic_buf, am->ve_outbuf, AMIXER_PROC_PERIOD);
    }
#endif

#endif
}

AMIXER_SRAM_FUNC
void amixer_Lmic_volume_proc(amixer *am)
{
#if AUDIO_KARAOKE_MIC_EN

    //Lmic监听干声音量处理
    if(am->usbrec_handle) {
        volume_control_mix_proc(&am->Lmic_listener_dry_vol_ctrl, am->listener_mic_dry_buffer,
            am->Lmic_buf, AMIXER_PROC_PERIOD);
    }
    //Lmic湿声音量处理
    volume_control_mix_proc(&am->Lmic_wet_vol_ctrl, am->mic_wet_buffer,
        am->Lmic_buf, AMIXER_PROC_PERIOD);
    //Lmic播放干声音量处理
    volume_control_mix_proc(&am->Lmic_play_dry_vol_ctrl, am->play_mic_dry_buffer,
        am->Lmic_buf, AMIXER_PROC_PERIOD);
    //Lmic干声2.4G发送音量处理
    volume_control_mix_proc(&am->Lmic_g24_tx_dry_vol_ctrl, am->g24_tx_buffer[0],
        am->Lmic_buf, AMIXER_PROC_PERIOD);
    
#endif
}

AMIXER_SRAM_FUNC
void amixer_wrx_volume_proc(amixer *am)
{
    if (am->is_dongle) {
        // 无线MIC
        logi("Processing wireless MIC");
#if G24_DUAL_CHANNEL
        // 保存无线接收数据用于后面混入无线发送
        if (am->mic_dry_temp_buffer[0] && am->mic_dry_temp_buffer[1]) {
            logi("Mic dry temp buffer available");
            if (am->downlink_buffer[am->wrx_atrack_id[0]]) {
                logi("Mixing temp buffer[0] with downlink_buffer[0]");
                am->mixer_hook(am->mic_dry_temp_buffer[0], am->downlink_buffer[am->wrx_atrack_id[0]], AMIXER_PROC_PERIOD);
            }
            
            if (am->downlink_buffer[am->wrx_atrack_id[1]]) {
                logi("Mixing temp buffer[1] with downlink_buffer[1]");
                am->mixer_hook(am->mic_dry_temp_buffer[1], am->downlink_buffer[am->wrx_atrack_id[1]], AMIXER_PROC_PERIOD);
            }
            
#if EVB_LCD_DEMO && AMIXER_ENG_CALC && AUDIO_KARAOKE_MIC_EN
            if ((am->eng_idx % 32) == 0) {
                eng_buf_reset(am);
                eng_calc_submit_data_stereo(am, am->mic_dry_temp_buffer[0], 0);
                mic_eng_sync(0, eng_calcing(am, 0));
            }

            if ((am->eng_idx % 32) == 0) {
                eng_buf_reset(am);
                eng_calc_submit_data_stereo(am, am->mic_dry_temp_buffer[1], 0);
                mic_eng_sync(1, eng_calcing(am, 0));
            }
#endif
        }
        
        if ((am->wrx_atrack_id[0] < AMIXER_AUDIOTRACK_MAX_NUM &&
            am->atrack[am->wrx_atrack_id[0]] && am->downlink_buffer[am->wrx_atrack_id[0]]) &&
            (am->wrx_atrack_id[1] < AMIXER_AUDIOTRACK_MAX_NUM &&
            am->atrack[am->wrx_atrack_id[1]] && am->downlink_buffer[am->wrx_atrack_id[1]])) {
            logi("Processing audio tracks: %d and %d", am->wrx_atrack_id[0], am->wrx_atrack_id[1]);
            // 湿声会使LR声道交叉，使用无线接收stereo模式可考虑关闭对应通道湿声
            if ((wrx_info_ch_get(am->wrx_track[0]) == 1) || (wrx_info_ch_get(am->wrx_track[1]) == 1)) {
                switch (am->wrx_ch_mix_mode) {
                case AMIXER_WRX_CH_MIX_MODE_MONO:       // L = R = WRX0 + WRX1
                    logi("Mix mode: MONO");
                    am->mixer_hook(am->downlink_buffer[am->wrx_atrack_id[0]], am->downlink_buffer[am->wrx_atrack_id[1]], AMIXER_PROC_PERIOD);
                    break;
                case AMIXER_WRX_CH_MIX_MODE_STEREO:     // L = WRX0, R = WRX1
                    logi("Mix mode: STEREO");
                    {
                        int32_t *dst = am->downlink_buffer[am->wrx_atrack_id[0]];
                        int32_t *src = am->downlink_buffer[am->wrx_atrack_id[1]];
                        for (int k = 0; k < AMIXER_PROC_PERIOD; k++) {
                            dst[2 * k + 1] = src[2 * k];
                        }
                    }
                    break;
                case AMIXER_WRX_CH_MIX_MODE_SAFE:       // L = WRX0 + WRX1 0dB, R = WRX0 + WRX1 -6dB
                    logi("Mix mode: SAFE");
                    {
                        am->mixer_hook(am->downlink_buffer[am->wrx_atrack_id[0]], am->downlink_buffer[am->wrx_atrack_id[1]], AMIXER_PROC_PERIOD);
                        memcpy(am->downlink_buffer[am->wrx_atrack_id[1]], am->downlink_buffer[am->wrx_atrack_id[0]], AMIXER_PROC_PERIOD * sizeof(int32_t) * 2);
                        // wrx safe 通道音量处理
                        volume_control_proc(&am->wrx_safe_mode_vol_ctrl, am->downlink_buffer[am->wrx_atrack_id[1]], AMIXER_PROC_PERIOD);
                        
                        int32_t *dst = am->downlink_buffer[am->wrx_atrack_id[0]];
                        int32_t *src = am->downlink_buffer[am->wrx_atrack_id[1]];
                        for (int k = 0; k < AMIXER_PROC_PERIOD; k++) {
                            dst[2 * k + 1] = src[2 * k];
                        }
                    }
                    break;
                }
            } else {
                am->mixer_hook(am->downlink_buffer[am->wrx_atrack_id[0]], am->downlink_buffer[am->wrx_atrack_id[1]], AMIXER_PROC_PERIOD);
            }
        }
#endif
        
        if (am->wrx_atrack_id[0] < AMIXER_AUDIOTRACK_MAX_NUM &&
            am->atrack[am->wrx_atrack_id[0]] && am->downlink_buffer[am->wrx_atrack_id[0]]) {
            logi("Processing dry volume control for track: %d", am->wrx_atrack_id[0]);
            // wrx监听干声音量处理
            if (am->usbrec_handle) {
                volume_control_mix_proc(&am->wrx_listener_dry_vol_ctrl, am->listener_mic_dry_buffer,
                    am->downlink_buffer[am->wrx_atrack_id[0]], AMIXER_PROC_PERIOD);
            }
            // wrx湿声音量处理     
            volume_control_mix_proc(&am->wrx_wet_vol_ctrl, am->mic_wet_buffer,
                am->downlink_buffer[am->wrx_atrack_id[0]], AMIXER_PROC_PERIOD);
            // wrx播放干声音量处理
            volume_control_mix_proc(&am->wrx_play_dry_vol_ctrl, am->play_mic_dry_buffer,
                am->downlink_buffer[am->wrx_atrack_id[0]], AMIXER_PROC_PERIOD);
        }
    } else {
        // music
        // logi("Processing music");
        if (am->wrx_atrack_id[0] < AMIXER_AUDIOTRACK_MAX_NUM &&
            am->atrack[am->wrx_atrack_id[0]] && am->downlink_buffer[am->wrx_atrack_id[0]]) {
            // logi("Processing dry volume control for music track: %d", am->wrx_atrack_id[0]);
            // wrx监听干声音量处理
            // if (am->usbrec_handle) {
            //     volume_control_mix_proc(&am->wrx_listener_dry_vol_ctrl, am->listener_buffer,
            //         am->downlink_buffer[am->wrx_atrack_id[0]], AMIXER_PROC_PERIOD);
            // }
            
            // // wrx湿声音量处理
            // volume_control_mix_proc(&am->wrx_wet_vol_ctrl, am->mic_wet_buffer,
            //     am->downlink_buffer[am->wrx_atrack_id[0]], AMIXER_PROC_PERIOD);
        }
    }
}


AMIXER_SRAM_FUNC
void amixer_usbplay_volume_proc(amixer *am)
{
#if  USB_DEVICE_EN
    if (am->usbplay_atrack_id < AMIXER_AUDIOTRACK_MAX_NUM &&
        am->atrack[am->usbplay_atrack_id] &&
        am->downlink_buffer[am->usbplay_atrack_id]) {
        //usbplay for play
        volume_control_mix_proc(&am->usbplay_play_vol_ctrl, am->playback_buffer,
                            am->downlink_buffer[am->usbplay_atrack_id], AMIXER_PROC_PERIOD);
        if(am->usbplay_mux_sel) {
            //usbplay for 2.4g tx
            volume_control_mix_proc(&am->usbplay_g24_tx_vol_ctrl, am->g24_tx_buffer[0],
                            am->downlink_buffer[am->usbplay_atrack_id], AMIXER_PROC_PERIOD);
            
            //usbplay for listener
            if(am->usbrec_handle) {
                volume_control_mix_proc(&am->usbplay_listener_vol_ctrl, am->listener_buffer,
                            am->downlink_buffer[am->usbplay_atrack_id], AMIXER_PROC_PERIOD);
            }
        }
    }
#endif
}

AMIXER_SRAM_FUNC
void amixer_ch_mask_x_pick(amixer *am, int32_t *outbuf,
    uint32_t ch_cnt, uint32_t ch0_idx, uint32_t ch1_idx)
{
    if (1 == ch_cnt) {
        for (int i = 0; i < AMIXER_PROC_PERIOD; i++) {
            outbuf[i] = am->adc_buf[AMIXER_AIN_CH*i+ch0_idx];
        }
    } else if (2 == ch_cnt) {
        for (int i = 0; i < AMIXER_PROC_PERIOD; i++) {
            outbuf[2*i] = am->adc_buf[AMIXER_AIN_CH*i+ch0_idx];
            outbuf[2*i+1] = am->adc_buf[AMIXER_AIN_CH*i+ch1_idx];
        }
    }
}

AMIXER_SRAM_FUNC
void amixer_aux_proc(amixer *am)
{
    int at;

    //收集aux数据到am->playback_buffer
    at = AMIXER_AUDIOTRACK_MAX_NUM;
    while(at--) {
        if(am->mixer_hook && am->downlink_buffer[at] &&
            (am->atrack_flags[at] & AUDIOTRACK_FLAGS_MUSIC_EFFECTS)) {
            am->mixer_hook(am->playback_buffer,
                am->downlink_buffer[at], AMIXER_PROC_PERIOD);
        }
    }
#if AUDIO_KARAOKE_LINEIN_EN
    am->mixer_hook(am->playback_buffer, am->aux_buf, AMIXER_PROC_PERIOD);
#endif
}

#if I2S0_EN && I2S0_SLAVER_PLAY_EN //slaver 播放

AMIXER_SRAM_FUNC
void amixer_i2s_slaver_play_proc(amixer *am)
{
    if (am->i2s_slaver_play_atrack_id < AMIXER_AUDIOTRACK_MAX_NUM &&
        am->atrack[am->i2s_slaver_play_atrack_id] &&
        am->downlink_buffer[am->i2s_slaver_play_atrack_id]) {
        am->mixer_hook(am->playback_buffer, am->downlink_buffer[am->i2s_slaver_play_atrack_id], AMIXER_PROC_PERIOD);
    }
}

#endif

#if SPDIF_RX_EN
AT_SRAM(.HAL_spdif_read)
void amixer_spdif_play_proc(amixer *am)
{
    if (am->spdif_play_atrack_id < AMIXER_AUDIOTRACK_MAX_NUM &&
        am->atrack[am->spdif_play_atrack_id] && am->downlink_buffer[am->spdif_play_atrack_id]) {
        am->mixer_hook(am->playback_buffer, am->downlink_buffer[am->spdif_play_atrack_id], AMIXER_PROC_PERIOD);
    }
}
#endif

AMIXER_SRAM_FUNC
void amixer_toneplay_volume_proc(amixer *am)
{
    int at = AMIXER_AUDIOTRACK_MAX_NUM;
    void *tone_buffer = NULL;
    int atrack_cnt = 0;

    while(at--) {
        if(am->mixer_hook && am->downlink_buffer[at] &&
            AUDIO_MODE_TONE == am->atrack_mode[at]) {
            if (0 == atrack_cnt) {
                //第一个提示音
                atrack_cnt++;
                tone_buffer = am->downlink_buffer[at];
            } else {
                //其他提示音都一起混入第一个提示音buffer中
                am->mixer_hook(tone_buffer, am->downlink_buffer[at], AMIXER_PROC_PERIOD);
                atrack_cnt++;
            }

        }
    }
    if (atrack_cnt) {
        if(mode_sta_get() == MODE_G24) {
            //提示音混入播放buffer
            volume_control_mix_proc(&am->tone_play_vol_ctrl, am->playback_buffer, tone_buffer, AMIXER_PROC_PERIOD);
            //提示音混入usb监听buffer
            if(am->usbrec_handle) {
                volume_control_mix_proc(&am->tone_listener_vol_ctrl, am->listener_buffer, tone_buffer, AMIXER_PROC_PERIOD);
            }
            //提示音混入2.4G发送buffer
            volume_control_mix_proc(&am->tone_g24_tx_vol_ctrl, am->g24_tx_buffer[0], tone_buffer, AMIXER_PROC_PERIOD);
        } else {
            am->mixer_hook(am->playback_buffer, tone_buffer, AMIXER_PROC_PERIOD);
        }
    }
}

AMIXER_SRAM_FUNC
void amixer_ducker_proc(void *handle)
{
#if AUDIO_KARAOKE_USE_DUCKER
    amixer *am = (amixer *)handle;
    if (am->is_dongle) {
        if(am->duck_enable) {
            //am->play_mic_dry_buffer 和 am->playback_buffer做闪避
            ducker_proc(am->dry_play_duck_handle, am->play_mic_dry_buffer,
                am->playback_buffer, AMIXER_PROC_PERIOD);
            //am->listener_mic_dry_buffer 和 am->listener_buffer做闪避
            if(am->usbrec_handle) {
                ducker_proc(am->dry_listener_duck_handle, am->listener_mic_dry_buffer,
                    am->listener_buffer, AMIXER_PROC_PERIOD);
            }
        }
    } else {
        //耳机端不做闪避
    }
#endif
}


#if USB_LISTENER_DELAY

AMIXER_SRAM_FUNC
void usb_listener_delay_proc(void *handle)
{
    amixer *am = (amixer *)handle;
    uint8_t bps = AMIXER_BPS;
#if USB_LISTENER_DELAY_16BIT
    bps = sizeof(int16_t);
#endif
    if (ring_buffer_get_w_avail(&am->listener_ring_buffer_ctrl) >= bps * AMIXER_CH * AMIXER_PROC_PERIOD) {
#if USB_LISTENER_DELAY_16BIT
        stereo32_to_stereo16((int16_t *)am->listener_temp_buffer, (int32_t *)am->playback_buffer, AMIXER_PROC_PERIOD);
        ring_buffer_write(&am->listener_ring_buffer_ctrl, am->listener_temp_buffer, bps * AMIXER_CH * AMIXER_PROC_PERIOD);
#else
        ring_buffer_write(&am->listener_ring_buffer_ctrl, am->playback_buffer, bps * AMIXER_CH * AMIXER_PROC_PERIOD);
#endif
    }

    if (ring_buffer_get_r_avail(&am->listener_ring_buffer_ctrl) >= bps * AMIXER_CH * AMIXER_PROC_PERIOD * (USB_LISTENER_DELAY_CNT-1)) {
        ring_buffer_read(&am->listener_ring_buffer_ctrl, am->listener_temp_buffer, bps * AMIXER_CH * AMIXER_PROC_PERIOD);
#if USB_LISTENER_DELAY_16BIT
        stereo16_to_stereo32((int32_t *)am->listener_temp_buffer, (int16_t *)am->listener_temp_buffer, AMIXER_PROC_PERIOD);
#endif
        volume_control_mix_proc(&am->aux_listener_vol_ctrl, am->listener_buffer,
                                        am->listener_temp_buffer, AMIXER_PROC_PERIOD);
    }
}

#endif

void duck_enbale_set(bool en)
{
    amixer* am = g_amixer;
    am->duck_enable = en;
}

bool duck_enbale_get(void)
{
    amixer* am = g_amixer;
    return am->duck_enable;
}

#if 0
AT_SRAM(.usbdev_loop)
void *amixer_usbdev_loop(void *handle)
{
    logi("amixer_usbdev_loop......");
    amixer *am = (amixer *)handle;
    __maybe_unused uint8_t usbplay_mux_sel;
    __maybe_unused int ret;
    while (am && !am->exit) {
        while(1) {
            HAL_msleep(1000);
            }
        amixer_cmd_process(am, (am->status == AMIXER_STU_RUNNING) ? 0 : OAL_WAIT_FOREVER);
        if(am->status == AMIXER_STU_RUNNING) {
            usbplay_mux_sel = am->usbplay_mux_sel;
            memset(am->playback_buffer, 0x00, AMIXER_PROC_PERIOD * AMIXER_CH * AMIXER_BPS);
            if(am->i2s_slaver_rec_buffer) {
                memset(am->i2s_slaver_rec_buffer, 0x00, AMIXER_PROC_PERIOD * AMIXER_CH * AMIXER_BPS);
            }

            am->ain->read(am->ain, am->adc_buf, AMIXER_PROC_PERIOD);

            amixer_ch_mask_x_pick(am, (int32_t*)am->Lmic_buf, am->Lmic_ch_cnt,
                am->Lmic_chidx0, am->Lmic_chidx1);
            //record输入音量处理
            volume_control_proc(&am->record_vol_ctrl, am->Lmic_buf, AMIXER_PROC_PERIOD);

            amixer_all_atrack_data_get(am);

            //本地mic音量处理
            amixer_Lmic_volume_proc(am);

            if (0 == usbplay_mux_sel) {
                amixer_usbplay_volume_proc(am);
            }

            //收集aux数据到am->playback_buffer
            amixer_aux_proc(am);

            //音乐音效输出音量处理
            volume_control_proc(&am->playback_vol_ctrl, am->playback_buffer, AMIXER_PROC_PERIOD);

            //aux混入usb监听buffer
            if(am->usbrec_handle) {
                volume_control_mix_proc(&am->aux_listener_vol_ctrl, am->listener_buffer,
                                                    am->playback_buffer, AMIXER_PROC_PERIOD);
            }

            if (usbplay_mux_sel) {         //usb下行不经过音效处理
                amixer_usbplay_volume_proc(am);
            }

            //提示音
            amixer_toneplay_volume_proc(am);

            //mic干声混入播放buffer
            am->mixer_hook(am->playback_buffer, am->play_mic_dry_buffer, AMIXER_PROC_PERIOD);

            if(am->usbrec_handle) {
                 //mic干声混入usb监听buffer
                am->mixer_hook(am->listener_buffer, am->listener_mic_dry_buffer, AMIXER_PROC_PERIOD);
            }

            int at;
            at = AMIXER_AUDIOTRACK_MAX_NUM;
            while(at--) {
                if(am->mixer_hook && am->downlink_buffer[at] &&
                    !(am->atrack_flags[at] & AUDIOTRACK_FLAGS_MUSIC_EFFECTS)) {
                    am->mixer_hook(am->playback_buffer,
                        am->downlink_buffer[at], AMIXER_PROC_PERIOD);
                }
            }

            am->mix_stamp += AMIXER_PROC_PERIOD;

            //播放
            if (am->aout) {
                ret = am->aout->write(am->aout, am->playback_buffer, AMIXER_PROC_PERIOD);
            }

            if (am->play_0data_cnt) {
                am->play_0data_cnt--;
                if (0 == am->play_0data_cnt) {
                    am->status = AMIXER_STU_STOP;
                    client_release_aout_dev(am);
                }
            }
        }
    }

    return NULL;
}
#endif

#if POWER_LPC_MODE_EN
static bool s_monitor_has_device_set = false;

AMIXER_SRAM_FUNC
void monitor_device_connect_set(bool is_connect)
{
    logi("monitor_device_connect_set %d", is_connect);
    s_monitor_has_device_set = is_connect;
}

bool monitor_device_is_connected()
{
    return s_monitor_has_device_set;
}

AMIXER_SRAM_FUNC
void monitor_LPC_enable(bool enable)
{
    static uint8_t lpc_enable = -1;
    if(enable) {
        HAL_clk_set_cpux_freq(32000000, 32000000);  //把CPU频率和CORE电压降下去
        lpc_enable = 1;
    } else {
        if(lpc_enable == 1) {
            lpc_enable = 0;
            uint32_t cpu_amixer[2] = DVFS_AMIXER_FREQ;
            HAL_clk_set_cpux_freq(cpu_amixer[0], cpu_amixer[1]);
        }
    }
}

#endif
void g24_bt_e_rspdsb_set(bool tx_dis);
void amixer_dac_switch(bool open);
void HAL_auss_dac_reset(void);
void app_audio_hw_init(void);
void wrx_pos_reset(void *handle);
void HAL_anc_reset();
bool is_auss_open = true;
__maybe_unused static bool amixer_stop = 0;

AT_SRAM(.amixer_lpc)
void amixer_mode_lpc_en(void* a)
{
#if POWER_LPC_MODE_EN
    amixer *am = (amixer*)a;
    if(am->status == AMIXER_STU_STOP) {
        if(mode_sta_get() == MODE_G24 && !s_monitor_has_device_set) {
            //if(am->status == AMIXER_STU_STOP) {
                logi("stop...");
                if(amixer_stop == 0) {
                    amixer_stop  = 1;
                    //release_aout(am);
                    //HAL_auss_deinit();
                }
                monitor_LPC_enable(true);
#if AUDIO_KARAOKE_LINEIN_EN
                amixer_close_linein();
#endif
#if AUDIO_KARAOKE_MIC_EN
                amixer_close_mic();
#endif
            //}
        }
    }
    #endif
}

AT_SRAM(.amixer_lpc)
void amixer_mode_lpc_off(void* a)
{
#if POWER_LPC_MODE_EN
    //amixer *am = (amixer*)a;
    monitor_LPC_enable(false);
#if AUDIO_KARAOKE_LINEIN_EN
    amixer_open_linein();
#endif
#if AUDIO_KARAOKE_MIC_EN
    amixer_open_mic();
#endif
    if(amixer_stop) {
        logi("aout init...");
        amixer_stop = 0;
    }
    #endif
}


AMIXER_SRAM_FUNC
static void *amixer_loop(void *handle)
{
    amixer *am = (amixer *)handle;
    __maybe_unused uint8_t usbplay_mux_sel;
    __maybe_unused int ret;
    logd("hello amixer loop");
    uint8_t usbAudioMode = psm_usb_audio_mode_get();
#if POWER_LPC_MODE_EN
    event_put(G24_EVENT_LPC_ENTER, 0 , 0);
#endif
    while (am && !am->exit) {
        amixer_cmd_process(am, (am->status == AMIXER_STU_RUNNING) ? 0 : OAL_WAIT_FOREVER);
        if(am->status == AMIXER_STU_RUNNING) {
            usbplay_mux_sel = am->usbplay_mux_sel;
            memset(am->playback_buffer, 0x00, AMIXER_PROC_PERIOD * AMIXER_CH * AMIXER_BPS);
#if AUDIO_KARAOKE_LINEIN_EN
            memset(am->aux_buf, 0x00, AMIXER_PROC_PERIOD * AMIXER_CH * AMIXER_BPS);
#endif
#if AUDIO_KARAOKE_MIC_EN
            memset(am->Lmic_buf, 0x00, AMIXER_PROC_PERIOD * AMIXER_CH * AMIXER_BPS);
#endif
            if(am->i2s_slaver_rec_buffer) {
                memset(am->i2s_slaver_rec_buffer, 0x00, AMIXER_PROC_PERIOD * AMIXER_CH * AMIXER_BPS);
            }
#if AUDIO_KARAOKE_EN
            if(mode_sta_get() == MODE_G24){
                memset(am->listener_buffer, 0x00, AMIXER_PROC_PERIOD * AMIXER_CH * AMIXER_BPS);
                memset(am->mic_wet_buffer, 0x00, AMIXER_PROC_PERIOD * AMIXER_CH * AMIXER_BPS);
                memset(am->play_mic_dry_buffer, 0x00, AMIXER_PROC_PERIOD * AMIXER_CH * AMIXER_BPS);
                memset(am->listener_mic_dry_buffer, 0x00, AMIXER_PROC_PERIOD * AMIXER_CH * AMIXER_BPS);
                memset(am->g24_tx_buffer[0], 0x00, AMIXER_PROC_PERIOD * AMIXER_CH * AMIXER_BPS);
                if(am->g24_tx_buffer[1]) {
                    memset(am->g24_tx_buffer[1], 0x00, AMIXER_PROC_PERIOD * AMIXER_CH * AMIXER_BPS);
                }
                if(am->mic_dry_temp_buffer[0] && am->mic_dry_temp_buffer[1]) {
                    memset(am->mic_dry_temp_buffer[0], 0x00, AMIXER_PROC_PERIOD * AMIXER_CH * AMIXER_BPS);
                    memset(am->mic_dry_temp_buffer[1], 0x00, AMIXER_PROC_PERIOD * AMIXER_CH * AMIXER_BPS);
                }
#if AUDIO_MIC_AFC
    			memcpy(am->afc_rxin_buf, am->afc_txout_buf, AMIXER_PROC_PERIOD *sizeof(int16_t));
                memset(am->afc_txin_buf, 0x00, AMIXER_PROC_PERIOD  *sizeof(int16_t));
                memset(am->afc_txout_buf, 0x00, AMIXER_PROC_PERIOD  *sizeof(int16_t));
#endif

                am->ain->read(am->ain, am->adc_buf, AMIXER_PROC_PERIOD);
#if AUDIO_KARAOKE_LINEIN_EN
                amixer_ch_mask_x_pick(am, (int32_t*)am->aux_buf, am->aux_ch_cnt,
                    am->aux_chidx0, am->aux_chidx1);
#endif
#if AUDIO_KARAOKE_MIC_EN
                amixer_ch_mask_x_pick(am, (int32_t*)am->Lmic_buf, am->Lmic_ch_cnt,
                    am->Lmic_chidx0, am->Lmic_chidx1);

                //printf_array(am->Lmic_buf, 16);
                //logi("");
                //record输入音量处理
                volume_control_proc(&am->record_vol_ctrl, am->Lmic_buf, AMIXER_PROC_PERIOD);
#endif
            }
#endif //AUDIO_KARAOKE_EN

#if AMIXER_ENG_CALC && AUDIO_KARAOKE_MIC_EN && !EVB_LCD_DEMO
            if((am->eng_idx % 32) == 0) {
                eng_buf_reset(am);
                eng_calc_submit_data_stereo(am, am->Lmic_buf, 0);
                mic_eng_sync(0, eng_calcing(am, 0));
            }
#endif

            //获取所有atrack数据
            amixer_all_atrack_data_get(am);

            //对本地MIC做DRC/AENC处理
            if(mode_sta_get() == MODE_G24){
#if AUDIO_KARAOKE_EN
                //只有立体声的时候才能使用MIC音效，dolby开AENC暂时处理不过来
                if(usbAudioMode != USBAUDIO_MODE_DOLBY) {
                    amixer_Lmic_effects_proc(am);
                }
                
                //本地mic音量处理
                amixer_Lmic_volume_proc(am);

                //无线接收数据音量处理
                amixer_wrx_volume_proc(am);

                //if (am->is_dongle) 
                {
                    //对am->mic_wet_buffer做MIC湿声音效处理			
                    if(am->e_effects_ch_num == 1) {
                        stereo32_to_mono32(am->mic_wet_buffer, am->mic_wet_buffer, AMIXER_PROC_PERIOD);
                    }
                    aeffect_component_proc_period(am->e_effects, am->mic_wet_buffer, AMIXER_PROC_PERIOD);
                    if(am->e_effects_ch_num == 1) {
                        mono32_to_stereo32(am->mic_wet_buffer, am->mic_wet_buffer, AMIXER_PROC_PERIOD);
                    }
                }

                if (0 == usbplay_mux_sel) {
                    amixer_usbplay_volume_proc(am);
                }
#endif //AUDIO_KARAOKE_EN
            }

            //收集aux数据到am->playback_buffer
            amixer_aux_proc(am);

#if I2S0_EN && I2S0_SLAVER_PLAY_EN //slaver 播放
            amixer_i2s_slaver_play_proc(am);    //I2S混音到播放通路
#endif

#if I2S0_EN && (USB_MIC_CHANNEL  >= USB_MIC_CHANNEL_4CHANNEL)
             if(am->i2s_usb_rec_handle) {
                stereo32_to_stereo16((int16_t*)am->i2s_slaver_usb_buffer, am->downlink_buffer[am->i2s_slaver_play_atrack_id], AMIXER_PROC_PERIOD);
                usbrec_store_data(am->i2s_usb_rec_handle, am->i2s_slaver_usb_buffer, AMIXER_PROC_PERIOD);
            }
#endif

#if SPDIF_RX_EN
            amixer_spdif_play_proc(am);
#endif

#if USE_EFFECTS
            //音乐音效处理
            aeffect_component_proc_period(am->amixer_effects,
                am->playback_buffer, AMIXER_PROC_PERIOD);
#endif

#if AUDIO_PLAYBACK_VOLCUT
            if (am->vocal_cut_en) {
                vocal_cut_proc(am->vocut, am->playback_buffer, am->playback_buffer, AMIXER_PROC_PERIOD);
            }
#endif

#if AUDIO_KARAOKE_EN
            //aux混入usb监听buffer
            if(mode_sta_get() == MODE_G24){
                //音乐音效输出音量处理
                volume_control_proc(&am->playback_vol_ctrl, am->playback_buffer, AMIXER_PROC_PERIOD);
                
                if(am->usbrec_handle) {
#if USB_LISTENER_DELAY
                    usb_listener_delay_proc(am);        //延时上传到usb
#else
                    volume_control_mix_proc(&am->aux_listener_vol_ctrl, am->listener_buffer,
                                                        am->playback_buffer, AMIXER_PROC_PERIOD);
#endif
                }

    			//闪避处理
                amixer_ducker_proc(am);

                //aux到2.4G发送音量处理
                volume_control_mix_proc(&am->aux_g24_tx_vol_ctrl, am->g24_tx_buffer[0],
                                                        am->playback_buffer, AMIXER_PROC_PERIOD);

                //aux播放音量处理
                volume_control_proc(&am->aux_play_vol_ctrl, am->playback_buffer, AMIXER_PROC_PERIOD);

                if (usbplay_mux_sel) {         //usb下行不经过音效处理
                    amixer_usbplay_volume_proc(am);
                }

#if AMIXER_ENG_CALC && !EVB_LCD_DEMO
                if((am->eng_idx % 32) == 16) {
                    eng_buf_reset(am);
                    eng_calc_submit_data_stereo(am, am->playback_buffer, 1);
                    music_eng_sync(eng_calcing(am, 1));
                }
#endif

#if AUDIO_MIC_AFC
                if (am->afc_manager_handle) {
                    if (get_afc_mode() == AFC_ADAPTIVE) {
                        memset(am->playback_buffer, 0x00, AMIXER_PROC_PERIOD * AMIXER_CH * AMIXER_BPS);
                    }
                    stereo32_to_mono16(am->afc_txin_buf, am->play_mic_dry_buffer, AMIXER_PROC_PERIOD);
                    am_afc_proc(am->afc_manager_handle, am->afc_rxin_buf, am->afc_txin_buf, am->afc_txout_buf);
                    mono16_to_stereo32(am->play_mic_dry_buffer, am->afc_txout_buf, AMIXER_PROC_PERIOD);
                }
#endif
                //提示音
                amixer_toneplay_volume_proc(am);

                if (am->is_dongle) {
#if AUDIO_DONGLE_BOARD_I2S_AFC
                    dongle_mic_i2s_afc_send(am->dongle_mic_i2s_afc_handle,
                        am->play_mic_dry_buffer, am->playback_buffer, AMIXER_PROC_PERIOD);
                    dongle_mic_i2s_afc_received(am->dongle_mic_i2s_afc_handle,
                        am->play_mic_dry_buffer, AMIXER_PROC_PERIOD);
#endif
                    //mic湿声混入播放buffer
                    volume_control_mix_proc(&am->mic_wet_play_vol_ctrl, am->playback_buffer,
                                                        am->mic_wet_buffer, AMIXER_PROC_PERIOD);
                    //mic湿声混入2.4G发送buffer
                    volume_control_mix_proc(&am->mic_wet_g24_tx_vol_ctrl, am->g24_tx_buffer[0],
                                                        am->mic_wet_buffer, AMIXER_PROC_PERIOD);
                    //dongle端发送数据
                    if(am->mic_dry_temp_buffer[0] && am->mic_dry_temp_buffer[1]) {
                        am->mixer_hook(am->g24_tx_buffer[1], am->g24_tx_buffer[0], AMIXER_PROC_PERIOD);
                        am->mixer_hook(am->g24_tx_buffer[1], am->mic_dry_temp_buffer[0], AMIXER_PROC_PERIOD);
                        am->mixer_hook(am->g24_tx_buffer[0], am->mic_dry_temp_buffer[1], AMIXER_PROC_PERIOD);
                    }

                    wrec_encode(am->wrec_handle[0], am->g24_tx_buffer[0], AMIXER_PROC_PERIOD);
#if (AUDIO_G24_TX_ENCODER_NUM == 2)
                    wrec_encode(am->wrec_handle[1], am->g24_tx_buffer[1], AMIXER_PROC_PERIOD);
#endif
                    //mic干声混入播放buffer
                    am->mixer_hook(am->playback_buffer, am->play_mic_dry_buffer, AMIXER_PROC_PERIOD);
                } else {
                    //mic湿声混入播放buffer
                    volume_control_mix_proc(&am->mic_wet_play_vol_ctrl, am->playback_buffer,
                                                        am->mic_wet_buffer, AMIXER_PROC_PERIOD);
                    
                    //mic干声混入播放buffer
                    am->mixer_hook(am->playback_buffer, am->play_mic_dry_buffer, AMIXER_PROC_PERIOD);
                    //耳机端发送数据
                    wrec_encode(am->wrec_handle[0], am->g24_tx_buffer[0], AMIXER_PROC_PERIOD);

                    //wrx混入播放buffer
                    if (am->wrx_atrack_id[0] < AMIXER_AUDIOTRACK_MAX_NUM &&
                        am->atrack[am->wrx_atrack_id[0]] && am->downlink_buffer[am->wrx_atrack_id[0]]) {
                        volume_control_mix_proc(&am->wrx_play_dry_vol_ctrl, am->playback_buffer,
                                am->downlink_buffer[am->wrx_atrack_id[0]], AMIXER_PROC_PERIOD);
                    }
                }
#if USB_DEVICE_EN
                if(am->usbrec_handle) {
                    //mic湿声混入usb监听buffer
                    volume_control_mix_proc(&am->mic_wet_listener_vol_ctrl, am->listener_buffer,
                                                    am->mic_wet_buffer, AMIXER_PROC_PERIOD);
                    //mic干声混入usb监听buffer
                    am->mixer_hook(am->listener_buffer, am->listener_mic_dry_buffer, AMIXER_PROC_PERIOD);
                }
#endif
                
            } else 
#endif //AUDIO_KARAOKE_EN
            {
                volume_control_proc(&am->aux_play_vol_ctrl, am->playback_buffer, AMIXER_PROC_PERIOD);
            }

            if(!AUDIO_KARAOKE_EN || mode_sta_get() == MODE_BT) {
                int at;
                at = AMIXER_AUDIOTRACK_MAX_NUM;
                while(at--) {
                    if(am->mixer_hook && am->downlink_buffer[at] &&
                        !(am->atrack_flags[at] & AUDIOTRACK_FLAGS_MUSIC_EFFECTS)) {
                        am->mixer_hook(am->playback_buffer,
                            am->downlink_buffer[at], AMIXER_PROC_PERIOD);
                    }
                }
            } else if(mode_sta_get() != MODE_G24) {
                amixer_toneplay_volume_proc(am);
            }
            
            am->mix_stamp += AMIXER_PROC_PERIOD;

#if AUDIO_KARAOKE_EN
            //usb监听buffer输出
            if(mode_sta_get() == MODE_G24){
                if(am->usbrec_handle) {
                    stereo32_to_stereo16((int16_t*)am->listener_buffer, am->listener_buffer, AMIXER_PROC_PERIOD);
                    usbrec_store_data(am->usbrec_handle, am->listener_buffer, AMIXER_PROC_PERIOD);
                }
            }
#endif //AUDIO_KARAOKE_EN

#if I2S0_EN && I2S0_SLAVER_REC_EN //slaver 录音
            if(am->i2s_slaver_rec_handle) {
                if(i2s_slaver_rec_sample_size_get(am->i2s_slaver_rec_handle) == (sizeof(int16_t))) {
                    stereo32_to_stereo16((int16_t*)am->i2s_slaver_rec_buffer, am->playback_buffer, AMIXER_PROC_PERIOD);
                }
                i2s_slaver_rec_store_data(am->i2s_slaver_rec_handle, am->i2s_slaver_rec_buffer, AMIXER_PROC_PERIOD);
            }
#endif

#if SPDIF_TX_EN
            //sin_debug(am->playback_buffer);
            stereo32_to_stereo16((int16_t*)am->spdif_rec_buffer, am->playback_buffer, AMIXER_PROC_PERIOD);
            spdif_rec_store_data(am->spdif_tx_handle, am->spdif_rec_buffer,AMIXER_PROC_PERIOD);
#endif

            //设置数据
#if RECORD_FUNC_EN
#if 0       //这是一个正弦波，两个声道幅度不一致
            extern const int16_t data48k_0dB_mono_16bit[48];

            static int idx;
            for(int i = 0; i < AMIXER_PROC_PERIOD; i++) {
                ((int32_t*)am->playback_buffer)[2*i] = (data48k_0dB_mono_16bit[idx] << 16)  * 0.5f;
                ((int32_t*)am->playback_buffer)[2*i+1] = (data48k_0dB_mono_16bit[idx] << 16)  * 0.1f;
                ++idx;
                if(idx >= 48) {
                    idx =0;
                }
            }
#endif
            //am->mic_wet_buffer
            record_set_data(am->playback_buffer, AMIXER_PROC_PERIOD);   //64个点
#endif

            //播放
            if (am->aout) {
                //printf_array(am->playback_buffer, 16);
                //logi("");
                ret = am->aout->write(am->aout, am->playback_buffer, AMIXER_PROC_PERIOD);
            }
#if AUDIO_KARAOKE_EN
#if POWER_LPC_MODE_EN == 0
            if ( (mode_sta_get() == MODE_G24) && !G24_MONITOR_EN && (!MODE_AFC_EN) && (am->is_dongle || !AUDIO_G24_RX_DECODER_DISABLE))
#endif
#endif
			{
                if (am->play_0data_cnt) {
                    am->play_0data_cnt--;
                    if (0 == am->play_0data_cnt) {
                        am->status = AMIXER_STU_STOP;
                        client_release_aout_dev(am);

#if POWER_LPC_MODE_EN
                        amixer_mode_lpc_en(am);
#endif
                    }
                }
            }

#if AMIXER_ENG_CALC
            am->eng_idx++;
#endif

#if 0//AUDIO_KARAOKE_EN
            //debug
            if(mode_sta_get() == MODE_G24){
                amixer_debug(am);
            }
#endif //AUDIO_KARAOKE_EN
        }
    }

    return NULL;
}


AT_SRAM(.usbrec_read)
void usbrec_read(void *buf, uint32_t bytes)
{
    amixer* am = g_amixer;

    usbrec_data_read(am->usbrec_handle, buf, bytes);
}

AT_SRAM(.i2s_rec_read)
void i2susb_read(void *buf, uint32_t bytes)
{
    amixer* am = g_amixer;
    usbrec_data_read(am->i2s_usb_rec_handle, buf, bytes);
}

AT_SRAM(.wrec_read)
uint8_t wrec_read(uint8_t link, uint8_t *buf, uint32_t bytes)
{
#if MODE_G24_EN
    if(is_mode_g24_exit()) {
        return 0;
    }
#endif
    amixer* am = g_amixer;

    return wrec_bs_read(am->wrec_handle[link], link, buf, bytes);
}

AT_SRAM(.g24_bs_receive)
void g24_bs_receive(uint8_t link, uint8_t *bs, uint32_t bytes, bool valid)
{
    amixer* am = g_amixer;

    put_g24_bs_to_wrx_track(am->wrx_track[link], bs, bytes, valid);
}

AT_SRAM(.usbplay_write)
void usbplay_write(void *buf, uint32_t samples)
{
#if USB_DEVICE_EN
    amixer* am = g_amixer;

    if (am->usbplay_track) {
        usbplay_track_write(am->usbplay_track, buf, samples);
    }
#endif
}


AMIXER_BANK_FUNC
void destroyAmixer(void* handle)
{
    amixer* am = g_amixer;
    if(am) {
        //TODO : notify all of the audiotrack and audiorecord client proxy
        if(am->task_handle){
            OAL_thread_join(am->task_handle);
            am->task_handle = NULL;
            wait_idle_running_a_period(10000);
        }
#if USE_EFFECTS
        if(am->amixer_effects) {
            aeffect_component_destroy(am->amixer_effects);
            am->amixer_effects = 0;
        }
#endif
        if(am->e_effects) {
            aeffect_component_destroy(am->e_effects);
            am->e_effects = 0;
        }
        if(am->mic_effects) {
            aeffect_component_destroy(am->mic_effects);
            am->mic_effects = 0;
        }

        int at = AMIXER_AUDIOTRACK_MAX_NUM;
        while (at--) {
            if (am->downlink_buffer[at]) {
                general_free(am->downlink_buffer[at]);
                am->downlink_buffer[at] = NULL;
            }
        }
        if (am->playback_buffer) {
            general_free(am->playback_buffer);
            am->playback_buffer = NULL;
        }

#if AUDIO_MIC_VOCALENHANCE 
        if (am->ve_inbuf) {
            general_free(am->ve_inbuf);
            am->ve_inbuf = NULL;
        }
        if (am->ve_outbuf) {
            general_free(am->ve_outbuf);
            am->ve_outbuf = NULL;
        }
#endif        
        if (am->ve_handle) {
            vocalenhance_close(am->ve_handle);
            am->ve_handle = NULL;
        }
        if (am->cmd_lock) {
            OAL_mutex_destroy(&am->cmd_lock);
            am->cmd_lock = NULL;
        }
        if (am->cmd_queue) {
            OAL_queue_delete(&am->cmd_queue);
            am->cmd_queue = NULL;
        }
        if (am->amsg_comp) {
            destroy_amsg_component(am->amsg_comp);
            am->amsg_comp = NULL;
        }
        if (am->dry_play_duck_handle) {
            ducker_close(am->dry_play_duck_handle);
            am->dry_play_duck_handle = NULL;
        }
        if (am->dry_listener_duck_handle) {
            ducker_close(am->dry_listener_duck_handle);
            am->dry_listener_duck_handle = NULL;
        }
        if (am->g24_tx_buffer[0]) {
            general_free(am->g24_tx_buffer[0]);
            am->g24_tx_buffer[0] = NULL;
        }
        if (am->g24_tx_buffer[1]) {
            general_free(am->g24_tx_buffer[1]);
            am->g24_tx_buffer[1] = NULL;
        }
        if (am->mic_dry_temp_buffer[0]) {
            general_free(am->mic_dry_temp_buffer[0]);
            am->mic_dry_temp_buffer[0] = NULL;
        }
        if (am->mic_dry_temp_buffer[1]) {
            general_free(am->mic_dry_temp_buffer[1]);
            am->mic_dry_temp_buffer[1] = NULL;
        }
		
#if USB_LISTENER_DELAY
        if (am->listener_temp_buffer) {
            general_free(am->listener_temp_buffer);
            am->listener_temp_buffer = NULL;
        }

        if (am->listener_ring_buffer) {
            general_free(am->listener_ring_buffer);
            am->listener_ring_buffer = NULL;
        }
#endif
#if AUDIO_MIC_AFC
        if(am->afc_manager_handle) {
            am_afc_close(am->afc_manager_handle);
            am->afc_manager_handle = NULL;
        }
	    if (am->afc_rxin_buf) {
            general_free(am->afc_rxin_buf);
            am->afc_rxin_buf = NULL;
        }
		if (am->afc_txout_buf) {
            general_free(am->afc_txout_buf);
            am->afc_txout_buf = NULL;
        }
		if (am->afc_txin_buf) {
            general_free(am->afc_txin_buf);
            am->afc_txin_buf = NULL;
        }
#endif
#if I2S0_EN && I2S0_SLAVER_REC_EN //slaver 录音
        if(am->i2s_slaver_rec_buffer) {
            general_free(am->i2s_slaver_rec_buffer);
            am->i2s_slaver_rec_buffer = NULL;
        }
#endif

#if I2S0_EN && (USB_MIC_CHANNEL >= USB_MIC_CHANNEL_4CHANNEL )
		if(mode_sta_get() == MODE_G24) {
            if(am->i2s_slaver_usb_buffer) {
                general_free(am->i2s_slaver_usb_buffer);
                am->i2s_slaver_usb_buffer = NULL;
            }
		}
#endif
#if SPDIF_TX_EN
    if(am->spdif_rec_buffer) {
        general_free(am->spdif_rec_buffer);
        am->spdif_rec_buffer = NULL;
    }
#endif

#if SPDIF_RX_EN
    //if(am->spdif_rx_buffer) {
    //    general_free(am->spdif_rx_buffer);
    //    am->spdif_rx_buffer = NULL;
    //}
#endif

#if AUDIO_PLAYBACK_VOLCUT
        if(am->vocut) {
            vocal_cut_close(am->vocut);
            am->vocut = NULL;
        }
#endif

        general_free(am);
    }
    g_amixer = 0;
}

void get_ch_idx(uint32_t ch_mask, uint32_t *p_ch_cnt, uint32_t *p_idx0, uint32_t *p_idx1)
{
    uint8_t set_ch0_idx = 1;
    uint8_t set_ch1_idx = 1;

    *p_ch_cnt = 0;
    *p_idx0 = 0;
    *p_idx1 = 0;

    //没有那么多ch, 检查8位就可以了
    for (int i = 0; i < 8; i++) {
        if (ch_mask & (1 << i)) {
            if (set_ch0_idx) {
                *p_idx0 = i;
                set_ch0_idx = 0;
            } else {
                if (set_ch1_idx) {
                    *p_idx1 = i;
                    set_ch1_idx = 0;
                }
            }
            (*p_ch_cnt)++;
        }
    }
}

void amixer_init_aux_play_volume_control(amixer* am)
{
    float volume_init = 0.0f;
    am->aux_play_vol_db = 0;
    volume_init = am->aux_play_vol_mute? 0.0f:1.0f;
    volume_control_init(&am->aux_play_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);
    logi("vol ctronl %x",&am->aux_play_vol_ctrl);
}

void createAmixer_g24_karaoke_control(amixer* am)
{
    __maybe_unused float volume_init = 0.0f;
    //amixer mute通路
    amixer_mute_config();

    //aux播放音量控制器初始化
    amixer_init_aux_play_volume_control(am);
    //am->aux_play_vol_db = 0;
    //volume_init = am->aux_play_vol_mute? 0.0f:1.0f;
    //volume_control_init(&am->aux_play_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);
    //aux监听音量控制器初始化
    am->aux_listener_vol_db = 0;
    volume_init = am->aux_listener_vol_mute? 0.0f:1.0f;
    volume_control_init(&am->aux_listener_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);
    //aux到2.4G发送音量控制器初始化
    am->aux_g24_tx_vol_db = 0;
    volume_init = am->aux_g24_tx_vol_mute? 0.0f:1.0f;
    volume_control_init(&am->aux_g24_tx_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);

    //音乐音效输出音量控制器初始化
    am->playback_vol_db = vol2decibels(am_volume_info_get(AM_VOLUME_INFO_PLAYBACK));
    volume_init = am->playback_vol_mute? 0.0f:volume_db_to_volf32(am->playback_vol_db);
    volume_control_init(&am->playback_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);

    //record输入音量控制器初始化
    am->record_vol_db = vol2decibels(am_volume_info_get(AM_VOLUME_INFO_RECORD));
    volume_init = am->record_vol_mute? 0.0f:volume_db_to_volf32(am->record_vol_db);
    volume_control_init(&am->record_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);

    //提示音播放音量控制器初始化
    am->tone_play_vol_db = 0;
    volume_init = am->tone_play_vol_mute? 0.0f:1.0f;
    volume_control_init(&am->tone_play_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);
    //提示音监听音量控制器初始化
    am->tone_listener_vol_db = 0;
    volume_init = am->tone_listener_vol_mute? 0.0f:1.0f;
    volume_control_init(&am->tone_listener_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);
    //提示音2.4G发送音量控制器初始化
    am->tone_g24_tx_vol_db = 0;
    volume_init = am->tone_g24_tx_vol_mute? 0.0f:1.0f;
    volume_control_init(&am->tone_g24_tx_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);

#if USB_DEVICE_EN
    //usbplay播放音量控制器初始化
    am->usbplay_mux_sel = 1;   //为1时usb在音效后混入
    am->usbplay_play_vol_db = 0;
    volume_init = am->usbplay_play_vol_mute? 0.0f:1.0f;
    volume_control_init(&am->usbplay_play_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);
    //usbplay监听音量控制器初始化
    am->usbplay_listener_vol_db = 0;
    volume_init = am->usbplay_listener_vol_mute? 0.0f:1.0f;
    volume_control_init(&am->usbplay_listener_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);
    //usbplay监听音量控制器初始化
    am->usbplay_g24_tx_vol_db = 0;
    volume_init = am->usbplay_g24_tx_vol_mute? 0.0f:1.0f;
    volume_control_init(&am->usbplay_g24_tx_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);
#endif
    //本地MIC湿声音量初始化
    am->Lmic_wet_vol_db = 0;
    volume_init = am->Lmic_wet_vol_mute? 0.0f:1.0f;
    volume_control_init(&am->Lmic_wet_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);
    //本地MIC播放干声音量初始化
    am->Lmic_play_dry_vol_db = 0;
    volume_init = am->Lmic_play_dry_vol_mute? 0.0f:1.0f;
    volume_control_init(&am->Lmic_play_dry_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);
    //本地MIC监听干声音量初始化
    am->Lmic_listener_dry_vol_db = 0;
    volume_init = am->Lmic_listener_dry_vol_mute? 0.0f:1.0f;
    volume_control_init(&am->Lmic_listener_dry_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);
    //本地MIC 2.4G发送干声音量初始化
    am->Lmic_g24_tx_dry_vol_db = 0;
    volume_init = am->Lmic_g24_tx_dry_vol_mute? 0.0f:1.0f;
    volume_control_init(&am->Lmic_g24_tx_dry_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);

    //无线MIC湿声音量初始化
    am->wrx_wet_vol_db = 0;
    volume_init = am->wrx_wet_vol_mute? 0.0f:1.0f;
    volume_control_init(&am->wrx_wet_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);
    //无线MIC播放干声音量初始化
    am->wrx_play_dry_vol_db = 0;
    volume_init = am->wrx_play_dry_vol_mute? 0.0f:1.0f;
    volume_control_init(&am->wrx_play_dry_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);
    //无线MIC监听干声音量初始化
    am->wrx_listener_dry_vol_db = 0;
    volume_init = am->wrx_listener_dry_vol_mute? 0.0f:1.0f;
    volume_control_init(&am->wrx_listener_dry_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);

    //MIC(Lmic+wrx)湿声监听音量控制器初始化
    am->mic_wet_listener_vol_db = 0;
    volume_init = am->mic_wet_listener_vol_mute? 0.0f:1.0f;
    volume_control_init(&am->mic_wet_listener_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);
    //MIC(Lmic+wrx)湿声监听音量控制器初始化
    am->mic_wet_play_vol_db = 0;
    volume_init = am->mic_wet_play_vol_mute? 0.0f:1.0f;
    volume_control_init(&am->mic_wet_play_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);
    //MIC(Lmic+wrx)湿声2.4G发送音量初始化
    am->mic_wet_g24_tx_vol_db = 0;
    volume_init = am->mic_wet_g24_tx_vol_mute? 0.0f:1.0f;
    volume_control_init(&am->mic_wet_g24_tx_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);

    //wrx safe mode音量初始化
    am->wrx_safe_mode_vol_db = -6;
    volume_init = volume_db_to_volf32(am->wrx_safe_mode_vol_db);
    volume_control_init(&am->wrx_safe_mode_vol_ctrl, AMIXER_FS, AMIXER_CH, AMIXER_BPS, volume_init);

#if USB_DEVICE_EN
    am->usbrec_handle = usbrec_create(); //usb录音相关
#endif

#if USB_MIC_CHANNEL >= USB_MIC_CHANNEL_4CHANNEL
    am->i2s_usb_rec_handle = usbrec_create();
#endif

#if AUDIO_G24_TX_ENCODER_DISABLE
#if G24_MONITOR_EN
    g24_tx_packet_size_set(wrec_bs_len_get());
#else
    g24_tx_packet_size_set(0);
#endif
#else
    if(mode_sta_get() == MODE_G24) {
        wrec_init_params_t wrec_prm;
#if (0 == AUDIO_G24_TX_ENCODER_SEL)
    wrec_prm.plugin = wrec_sbc_plugin;
#else
    wrec_prm.plugin = wrec_lc3_plugin;
#endif
    g24_encode_sel_set(AUDIO_G24_TX_ENCODER_SEL);
    wrec_prm.link = 0;
    
    am->wrec_handle[0] = wrec_create(&wrec_prm); //无线录音相关
#if G24_DUAL_CHANNEL
        am->wrec_handle[1] = am->wrec_handle[0];
#if (AUDIO_G24_TX_ENCODER_NUM==2)
        if(G24_FUNC_ROLE == G24_FUNC_ROLE_DONGLE) {
            wrec_prm.link = 1;
            am->wrec_handle[1] = wrec_create(&wrec_prm); //无线录音相关
        }
#endif
#endif
    }
#endif

#if (AUDIO_MIC_AENC || AUDIO_MIC_NSBWE_AENC)
    //local mic降噪
    am->aenc_handle = Lmic_aenc_bwe_open(AMIXER_FS, AMIXER_PROC_PERIOD);
    Lmic_aenc_enable_set(true);
#if MYME_M4_BOARD
    key_led_set_status(led4_green, 1);
#endif
#if EVB_TX_DEMO
    AENC_DISP_ON();
#endif
#endif

#if AUDIO_MIC_VOCALENHANCE
    ve_initprm_t init_prm;
    ve_rtprm_t rt_prm;

    init_prm.chnum = 1;
    init_prm.fs = AMIXER_FS;
    init_prm.L = AMIXER_PROC_PERIOD;
    am->ve_handle = vocalenhance_open(&init_prm);
    if (am->ve_handle == NULL) {
        destroyAmixer(am);
        return ;
    }

    rt_prm.brightness = 0;
    rt_prm.gain = 0;
    rt_prm.richness = 5;
    rt_prm.style = VE_STYLE_MALE;
    vocalenhance_cmd(am->ve_handle, SLTK_CMD_AFX_COMMON_SETRTPRM, (uint32_t)&rt_prm);
    am->ve_enable = true;
#endif
#if AUDIO_KARAOKE_USE_DUCKER
    //闪避初始化
    duck_initprm_t duck_init_prm;
    duck_init_prm.Qbits = 8 * AMIXER_BPS - 1;
    duck_init_prm.dked_chnum = AMIXER_CH;
    duck_init_prm.dking_chnum = AMIXER_CH;
    duck_init_prm.max_dly = 0;
    duck_init_prm.fs = AMIXER_FS;
    am->dry_play_duck_handle = ducker_open(&duck_init_prm);
    am->dry_listener_duck_handle = ducker_open(&duck_init_prm);
    if (am->dry_play_duck_handle && am->dry_listener_duck_handle) {
        logd("init ducker sucessful");
        duck_rtprm_t duck_prm = {
            50,  //att_time
            500, //rel_time
            200,//avg_time
            0,   //look_ahead
            200, //hold_time
            -25, //duck_thd
            -25, //duck_gain
        };
        ducker_cmd(am->dry_play_duck_handle, SLTK_CMD_AFX_COMMON_SETRTPRM, (uint32_t)&duck_prm);
        ducker_cmd(am->dry_listener_duck_handle, SLTK_CMD_AFX_COMMON_SETRTPRM, (uint32_t)&duck_prm);
        am->duck_enable = true;
    } else {
        loge("init ducker fail");
    }
#endif

    g24_put_bs_func_register(g24_bs_receive);
    g24_get_bs_func_register(wrec_read);
}

bool createAmixer_component(amixer* am, aeffect_comp_init* aefx_init)
{
#if AUDIO_EFFECT_FDNREV_STEREO && AUDIO_MIC_REVERB
    aefx_init->asource_common.ch = 2;
#else
	aefx_init->asource_common.ch = 1;//AMIXER_CH;
#endif
    aefx_init->asource_common.fs = AMIXER_FS;
    aefx_init->asource_common.period = AMIXER_PROC_PERIOD;
    aefx_init->asource_common.sample_size = AMIXER_BPS;
    aefx_init->user_data = am;
    aefx_init->notify    = (notify_clbk)amixer_event_listener;
    aefx_init->cpu0_list = &e_cpu0_aeffects;
    aefx_init->cpu1_list = &e_cpu1_aeffects;
#if DUAL_CPU_EN
    aefx_init->acpux_accelerate = 1;
#else
    aefx_init->acpux_accelerate = 0;
#endif
    am->e_effects = aeffect_component_create(aefx_init);
    am->e_effects_ch_num = aefx_init->asource_common.ch;

    if(!am->e_effects) {
        logi("e efx fail");
        destroyAmixer(am);
        return false;
    } else {
        logi("e efx sucessful");
    }
    return true;
}

bool s_linein_opened = false;
#if AUDIO_KARAOKE_LINEIN_EN & POWER_LPC_MODE_EN
AMIXER_SRAM_FUNC
#endif
void amixer_open_linein(void* a)
{
#if AUDIO_KARAOKE_LINEIN_EN
    if(s_linein_opened) {
        return;
    }
    s_linein_opened = true;

    amixer_audio_linein_mode_input_sel_set(AUDIO_LINEIN_MODE_INPUT_SEL);
    logi("linein open ....");
#endif
}

#if AUDIO_KARAOKE_LINEIN_EN & POWER_LPC_MODE_EN
AMIXER_SRAM_FUNC
void amixer_close_linein()
{
    //AUDIO_LINEIN_MODE_INPUT_SEL
    if(!s_linein_opened) {
        return;
    }
    
    audio_service_disable_input_source(audio_linein_mode_input_sel_get());
    s_linein_opened = false;
    logi("linein close ....");
}
#endif

#if AUDIO_KARAOKE_MIC_EN & POWER_LPC_MODE_EN
AMIXER_SRAM_FUNC
#endif
void amixer_open_mic()
{
#if AUDIO_KARAOKE_MIC_EN
    
    if(s_mic_opened) {
        return ;
    }
    logi("open mic..");

    s_mic_opened = true;
    amixer_audio_rec_input_sel_set(AUDIO_REC_INPUT_SEL);
    //logd("@@@@ Lmic ch idx:%d, %d", am->Lmic_chidx0, am->Lmic_chidx1);
#endif
}


#if AUDIO_KARAOKE_MIC_EN & POWER_LPC_MODE_EN
AMIXER_SRAM_FUNC
void amixer_close_mic()
{
    if(!s_mic_opened) {
        return;
    }
#if 0
    amixer* am = (amixer*)a;
    
    __maybe_unused ain_hw_path_params_t path_prm;
    __maybe_unused uint32_t ain_session;
    //Lmic通路打开
    am->ain->cmd(am->ain, SLTK_CMD_AI_SESSION_DES, (int32_t)&ain_session);
    //根据mode来打开硬件通路
    path_prm.session = ain_session;
    path_prm.prm_type = AIN_HW_PATH_PRM_TYPE_CONFIG_BY_MODE;
    path_prm.mode = AUDIO_IN_MODE_NATIVE_REC;
    am->ain->cmd(am->ain, SLTK_CMD_AI_HW_PATH_CLOSE, (int32_t)&path_prm);
#else
    audio_service_disable_input_source(audio_rec_input_sel_get());
#endif
    s_mic_opened = false;
    logi("mic hw close ....");
}
#endif

void createAmixer_ain(amixer* am)
{
    am->ain = create_ain(AIN_PRIMARY);
    ain_init_t init;
    init.fs = AMIXER_FS;
    init.fadein_ms = 300;       //避免啪声
    init.fadeout_ms = 0;
    init.headmute_ms = 0;
    #if 0
    init.period_size = AMIXER_PROC_PERIOD;
    init.period_count = 2;
    #else
    init.period_size = AMIXER_PROC_PERIOD/2;
    init.period_count = 3;
    #endif
    init.ch = 4;
    init.sample_size = sizeof(int32_t);
    init.mode = AUDIO_IN_MODE_SCO;
    am->ain->init(am->ain, &init);

    __maybe_unused ain_hw_path_params_t path_prm;
    __maybe_unused uint32_t ain_session;
    __maybe_unused uint32_t val;
#if AUDIO_KARAOKE_LINEIN_EN
    //aux通路打开
    am->ain->cmd(am->ain, SLTK_CMD_AI_SESSION_REQ, (int32_t)&ain_session);
    //根据mode来打开硬件通路
    path_prm.session = ain_session;
    path_prm.prm_type = AIN_HW_PATH_PRM_TYPE_CONFIG_BY_MODE;
    path_prm.mode = AUDIO_IN_MODE_LINEIN;
    am->ain->cmd(am->ain, SLTK_CMD_AI_HW_PATH_OPEN, (int32_t)&path_prm);
    //获取此模式的ch_mask
    val = AUDIO_IN_MODE_LINEIN;
    am->ain->cmd(am->ain, SLTK_CMD_AI_CH_MASK_GET, (uint32_t)&val);
    get_ch_idx(val, &am->aux_ch_cnt, &am->aux_chidx0, &am->aux_chidx1);
    //声道必须固定为2,如果是单声道则aux_chidx0和aux_chidx1配相同值
    if (1 == am->aux_ch_cnt) {
        am->aux_ch_cnt = 2;
        am->aux_chidx1 = am->aux_chidx0;
    }
    s_linein_opened = true;
    //logd("@@@@ aux ch idx:%d, %d", am->aux_chidx0, am->aux_chidx1);
#endif
#if AUDIO_KARAOKE_MIC_EN
    //Lmic通路打开
    am->ain->cmd(am->ain, SLTK_CMD_AI_SESSION_REQ, (int32_t)&ain_session);
    //根据mode来打开硬件通路
    path_prm.session = ain_session;
    path_prm.prm_type = AIN_HW_PATH_PRM_TYPE_CONFIG_BY_MODE;
    path_prm.mode = AUDIO_IN_MODE_NATIVE_REC;
    am->ain->cmd(am->ain, SLTK_CMD_AI_HW_PATH_OPEN, (int32_t)&path_prm);
    //获取此模式的ch_mask
    val = AUDIO_IN_MODE_NATIVE_REC;
    am->ain->cmd(am->ain, SLTK_CMD_AI_CH_MASK_GET, (uint32_t)&val);
    get_ch_idx(val, &am->Lmic_ch_cnt, &am->Lmic_chidx0, &am->Lmic_chidx1);
    //声道必须固定为2,如果是单声道则Lmic_chidx0和Lmic_chidx1配相同值
    if (1 == am->Lmic_ch_cnt) {
        am->Lmic_ch_cnt = 2;
        am->Lmic_chidx1 = am->Lmic_chidx0;
    }
    s_mic_opened = true;
    logi("mic hw open ....");
#endif
}

bool createAmixer_buff(amixer* am)
{
    am->listener_buffer = general_calloc(AMIXER_BPS * AMIXER_CH * AMIXER_PROC_PERIOD);
    am->mic_wet_buffer = general_calloc(AMIXER_BPS * AMIXER_CH * AMIXER_PROC_PERIOD);
    am->play_mic_dry_buffer = general_calloc(AMIXER_BPS * AMIXER_CH * AMIXER_PROC_PERIOD);
    am->listener_mic_dry_buffer = general_calloc(AMIXER_BPS * AMIXER_CH * AMIXER_PROC_PERIOD);
    am->g24_tx_buffer[0] = general_calloc(AMIXER_BPS * AMIXER_CH * AMIXER_PROC_PERIOD);
#if AUDIO_MIC_VOCALENHANCE    
    am->ve_inbuf = general_calloc(sizeof(int16_t) * AMIXER_PROC_PERIOD);
    am->ve_outbuf = general_calloc(sizeof(int16_t) * AMIXER_PROC_PERIOD);
#endif    
#if AUDIO_MIC_AFC
	am->afc_rxin_buf = general_calloc(sizeof(int16_t)  * AMIXER_PROC_PERIOD);
    am->afc_txout_buf = general_calloc(sizeof(int16_t)  * AMIXER_PROC_PERIOD);
    am->afc_txin_buf = general_calloc(sizeof(int16_t)  * AMIXER_PROC_PERIOD);
#endif
#if G24_DUAL_CHANNEL
    am->g24_tx_buffer[1] = am->g24_tx_buffer[0];
#if (AUDIO_G24_TX_ENCODER_NUM==2)
    if(G24_FUNC_ROLE == G24_FUNC_ROLE_DONGLE) {
        am->g24_tx_buffer[1] = general_calloc(AMIXER_BPS * AMIXER_CH * AMIXER_PROC_PERIOD);
        if (NULL == am->g24_tx_buffer[1]) {
            am->g24_tx_buffer[0] = NULL;
        }
    }
    am->mic_dry_temp_buffer[0] = general_calloc(AMIXER_BPS * AMIXER_CH * AMIXER_PROC_PERIOD);
    am->mic_dry_temp_buffer[1] = general_calloc(AMIXER_BPS * AMIXER_CH * AMIXER_PROC_PERIOD);
#endif
#endif

    if (
#if USB_LISTENER_DELAY
        NULL == am->listener_temp_buffer ||
        NULL == am->listener_ring_buffer ||
#endif
        NULL ==am->playback_buffer ||
        NULL == am->listener_buffer ||
        NULL == am->mic_wet_buffer ||
        NULL == am->play_mic_dry_buffer ||
        NULL == am->listener_mic_dry_buffer ||
        NULL == am->g24_tx_buffer[0]) {
        destroyAmixer(am);
        return false;
    }
    return true;
}

#if 0
void* create_usb_headphone_Amixer(uint32_t paras)
{
    logi("create_usb_headphone_Amixer... ");
    int ret;
    OAL_QUEUE_POOL pool;
    //aeffect_comp_init aefx_init;

    amixer* am = (amixer*)general_calloc(sizeof(amixer));
    if (!am) {
        logd("amixer create fail!");
        return NULL;
    }
    g_amixer = am;

    amsg_init_t ai;
    ai.cctrl = (notify_clbk)amixer_event_listener;
    ai.user_data = am;
    am->amsg_comp = create_amsg_component(&ai);
    if (NULL == am->amsg_comp) {
        destroyAmixer(am);
        return NULL;
    }

    am->cmd_lock = OAL_mutex_create();
    if (NULL == am->cmd_lock) {
        destroyAmixer(am);
        return NULL;
    }

    pool.size = AMIXER_MSG_QUEUE_LENGTH * sizeof(amsg_t);
    ret = OAL_queue_create(&am->cmd_queue, NULL, sizeof(amsg_t), &pool);
    if (OAL_SUCCESS != ret) {
        destroyAmixer(am);
        return NULL;
    }

    am->status = AMIXER_STU_STOP;
    am->playback_buffer = general_calloc(AMIXER_BPS * AMIXER_CH * AMIXER_PROC_PERIOD);

    am->aout = create_aout(AOUT_DAC);
    if(!am->aout) {
        destroyAmixer(am);
        return NULL;
    }

    app_audio_codec_analog_gain_config();                       //需要在通路使能后再配置，否则会配不上去

    //混音函数
    am->mixer_hook = mixer_get_hook(GET_MIXER_TYPE(AMIXER_CH, AMIXER_BPS, AMIXER_CH, AMIXER_BPS));

    /* create audiomixer task */
    am->task_handle = OAL_thread_create(amixer_usbdev_loop, am,
        AMIXER_TASK_NAME, AMIXER_TASK_PRIO, AMIXER_TASK_STACK_DEPTH);
    track_handler = am->task_handle;
    if (NULL == am->task_handle) {
        destroyAmixer(am);
        return NULL;
    }

#if USB_DEVICE_EN
    am->usbrec_handle = usbrec_create(); //usb录音相关
    am->usbplay_track = usbplay_track_create(); //usbplay播放通路
#endif

    logd("usbd amixer create ok!");

    return am;
}
#endif

AMIXER_BANK_FUNC
void* createAmixer(uint32_t paras)
{
    int ret;
    OAL_QUEUE_POOL pool;
    aeffect_comp_init aefx_init;

    amixer* am = (amixer*)general_calloc(sizeof(amixer));
    if (!am) {
        logd("amixer create fail!");
        return NULL;
    }
    g_amixer = am;

    amsg_init_t ai;
    ai.cctrl = (notify_clbk)amixer_event_listener;
    ai.user_data = am;
    am->amsg_comp = create_amsg_component(&ai);
    if (NULL == am->amsg_comp) {
        destroyAmixer(am);
        return NULL;
    }

    am->cmd_lock = OAL_mutex_create();
    if (NULL == am->cmd_lock) {
        destroyAmixer(am);
        return NULL;
    }

    pool.size = AMIXER_MSG_QUEUE_LENGTH * sizeof(amsg_t);
    ret = OAL_queue_create(&am->cmd_queue, NULL, sizeof(amsg_t), &pool);
    if (OAL_SUCCESS != ret) {
        destroyAmixer(am);
        return NULL;
    }

    am->status = AMIXER_STU_STOP;
    
#if USB_LISTENER_DELAY
    am->listener_temp_buffer = general_calloc(AMIXER_BPS * AMIXER_CH * AMIXER_PROC_PERIOD);
    uint32_t listener_ring_buffer_sz = AMIXER_BPS * AMIXER_CH * AMIXER_PROC_PERIOD * USB_LISTENER_DELAY_CNT;
#if USB_LISTENER_DELAY_16BIT
    listener_ring_buffer_sz = sizeof(int16_t) * AMIXER_CH * AMIXER_PROC_PERIOD * USB_LISTENER_DELAY_CNT;
#endif
    am->listener_ring_buffer = general_calloc(listener_ring_buffer_sz);
    ring_buffer_init(&am->listener_ring_buffer_ctrl, am->listener_ring_buffer, listener_ring_buffer_sz);
#endif

    am->playback_buffer = general_calloc(AMIXER_BPS * AMIXER_CH * AMIXER_PROC_PERIOD);
#if AUDIO_KARAOKE_EN
    if(mode_sta_get() == MODE_G24) {
        if(!createAmixer_buff(am)) {
            return NULL;
        }
    }
#endif //AUDIO_KARAOKE_EN

    am->aout = create_aout(AOUT_DAC);
    if(!am->aout) {
        destroyAmixer(am);
        return NULL;
    }
    
#if AUDIO_PLAYBACK_VSS
    vss_enable_revb_set(true);  //虚拟环绕3D的耳机模式使用混响
#endif    

#if AUDIO_KARAOKE_EN
    if(mode_sta_get() == MODE_G24) {
        createAmixer_ain(am);
    }
#endif
    app_audio_codec_analog_gain_config();                       //需要在通路使能后再配置，否则会配不上去

    am->is_dongle = G24_FUNC_ROLE;
    //混音函数
    am->mixer_hook = mixer_get_hook(GET_MIXER_TYPE(AMIXER_CH, AMIXER_BPS, AMIXER_CH, AMIXER_BPS));
#if AMIXER_ENG_CALC
    am->mic2engcalc_refmt    = reformat_to_hook(GET_REFORMAT_TYPE(AMIXER_CH, AMIXER_BPS, 1, sizeof(int16_t)));
    am->mic2engcalc_mixer    = mixer_get_hook(GET_MIXER_TYPE(AMIXER_CH, AMIXER_BPS, 1, sizeof(int16_t)));
#endif

#if AUDIO_KARAOKE_EN
    if(mode_sta_get() == MODE_G24) {
        createAmixer_g24_karaoke_control(am);
    } 
#endif

    if(mode_sta_get() == MODE_BT) {
        amixer_init_aux_play_volume_control(am);
    }

#if I2S0_EN && I2S0_SLAVER_REC_EN
    am->i2s_slaver_rec_buffer = general_calloc(AMIXER_BPS * AMIXER_CH * AMIXER_PROC_PERIOD);
    am->i2s_slaver_rec_handle = i2s_slaver_rec_create();   //i2s slaver 录音相关
    if(NULL == am->i2s_slaver_rec_buffer ||
        NULL == am->i2s_slaver_rec_handle) {
        loge("i2s slaver rec fail\n");
        destroyAmixer(am);
        return NULL;
    }    
#endif

#if SPDIF_TX_EN
    am->spdif_rec_buffer = general_calloc(AMIXER_BPS * AMIXER_CH * AMIXER_PROC_PERIOD);
    am->spdif_tx_handle = spdif_rec_create();   //spdif tx 录音相关
    if(NULL == am->spdif_tx_handle ||
        NULL == am->spdif_tx_handle) {
        loge("spdif tx fail\n");
        destroyAmixer(am);
        return NULL;
    }
    spdif_rec_start();
#endif

#if USE_EFFECTS
    aefx_init.asource_common.ch = AMIXER_CH;
    aefx_init.asource_common.fs = AMIXER_FS;
    aefx_init.asource_common.period = AMIXER_PROC_PERIOD;
    aefx_init.asource_common.sample_size = AMIXER_BPS;
    aefx_init.user_data = am;
    aefx_init.notify    = (notify_clbk)amixer_event_listener;

    aefx_init.cpu0_list = &amixer_cpu0_aeffects;
    aefx_init.cpu1_list = &amixer_cpu1_aeffects;
#if DUAL_CPU_EN
    aefx_init.acpux_accelerate = 1;
#else
    aefx_init.acpux_accelerate = 0;
#endif
    am->amixer_effects = aeffect_component_create(&aefx_init);
    if(!am->amixer_effects) {
        logi("amixer efx fail");
        destroyAmixer(am);
        return NULL;
    } else {
        logi("amixer efx sucessful");
    }

#if AUDIO_KARAOKE_EN
    if(mode_sta_get() == MODE_G24) {
        if(!createAmixer_component(am, &aefx_init)) {
            return NULL;
        }
    }
#endif
#if AUDIO_MIC_AFC
    afc_params_t prms;

    prms.fs = 48000;
    //prms.Qbits = 31;
    prms.N = 64;
    prms.O = 30;
    prms.P = 10;
    prms.Q = 20;
    prms.B = 64;
    prms.L = 6;
    prms.PO = 1;

    prms.Qbits = 15;
    prms.maxNf = 12;
    prms.maxNp = 10;
    prms.maxthist = 100;
    prms.Rdec = 1;

    uint16_t param = 0;
    g24_afc_param_get(&param);

    prms.O = param;
    logi("afc set param %d ++++++++++++++++++", param);

    am->afc_manager_handle = am_afc_open(prms);
    if (am->afc_manager_handle) {
        logi("afc open sucessful");
    } else {
        logi("afc open fail");
    }
#endif
#if MIC_EFFECTS_ENABLE
    aefx_init.asource_common.ch = 1;//AMIXER_CH;
    aefx_init.asource_common.fs = AMIXER_FS;
    aefx_init.asource_common.period = AMIXER_PROC_PERIOD;
    aefx_init.asource_common.sample_size = AMIXER_BPS;
    aefx_init.user_data = am;
    aefx_init.notify    = (notify_clbk)amixer_event_listener;
    aefx_init.cpu0_list = &mic_cpu0_aeffects;
    aefx_init.cpu1_list = &mic_cpu1_aeffects;
#if DUAL_CPU_EN
    aefx_init.acpux_accelerate = 1;
#else
    aefx_init.acpux_accelerate = 0;
#endif
    am->mic_effects = aeffect_component_create(&aefx_init);
    if(!am->mic_effects) {
        logi("mic efx fail");
        destroyAmixer(am);
        return NULL;
    } else {
        logi("mic efx sucessful");
    }
#endif
#endif

#if AUDIO_DONGLE_BOARD_I2S_AFC
    am->dongle_mic_i2s_afc_handle = dongle_mic_i2s_afc_open();
#endif
#if AUDIO_BT_BOARD_I2S_AFC
    am->dsp_mic_i2s_afc_handle = dsp_mic_i2s_afc_open();
#endif

#if AUDIO_PLAYBACK_VOLCUT
    vocut_prm_t vocut_prm;
    memset(&vocut_prm, 0, sizeof(vocut_prm_t));
    vocut_prm.cfL.f.alpha1 = -0.97415715;
    vocut_prm.cfL.f.alpha21 = -1.9694436;
    vocut_prm.cfL.f.alpha22 = 0.97011870;
    vocut_prm.cfL.f.alpha31 = -1.9917084;
    vocut_prm.cfL.f.alpha32 = 0.99239111;

    vocut_prm.cfH.f.alpha1  = -0.49314541;
    vocut_prm.cfH.f.alpha21 = -1.1729678;
    vocut_prm.cfH.f.alpha22 = 0.47849357;
    vocut_prm.cfH.f.alpha31 = -1.4572802;
    vocut_prm.cfH.f.alpha32 = 0.83686155;

    vocut_prm.sample_size = AMIXER_BPS;
    vocut_prm.period      = AMIXER_PROC_PERIOD;
    vocut_prm.chnum       = AMIXER_CH;

    am->vocut = vocal_cut_open(&vocut_prm);
    am->vocal_cut_en = true;
#endif

    /* create audiomixer task */
    am->task_handle = OAL_thread_create(amixer_loop, am,
        AMIXER_TASK_NAME, AMIXER_TASK_PRIO, AMIXER_TASK_STACK_DEPTH);
    track_handler = am->task_handle;
    if (NULL == am->task_handle) {
        destroyAmixer(am);
        return NULL;
    }

#if I2S0_EN && I2S0_SLAVER_PLAY_EN //slaver 播放
    am->i2s_slaver_play_track = i2s_slaver_play_track_create(); //i2s slaver play播放通路
    i2s_slaver_play_track_start(am->i2s_slaver_play_track);
#endif

#if SPDIF_RX_EN
    //am->spdif_rx_buffer = general_calloc(AMIXER_BPS * AMIXER_CH * AMIXER_PROC_PERIOD);
    am->spdif_rx_handle = spdif_play_track_create();
    spdif_play_track_start(am->spdif_rx_handle, 48000);
#endif

    logd("amixer create ok!");

    return am;
}

//切换输入single end
void amixer_audio_rec_single_end_set(uint8_t sel)
{
    audio_service_disable_input_source(audio_rec_input_sel_get());
    HAL_mixer_set_ctl(SND_CODEC, AMIC2_SINGLE_END, sel);
    audio_service_enable_input_source(audio_rec_input_sel_get());
}


void amixer_audio_rec_input_sel_set(uint8_t sel)
{
#if AUDIO_KARAOKE_MIC_EN
    uint32_t val;
    amixer* am = g_amixer;
    audio_service_disable_input_source(audio_rec_input_sel_get());
    audio_rec_input_sel_set(sel);
    audio_service_enable_input_source(audio_rec_input_sel_get());
    //获取此模式的ch_mask
    val = AUDIO_IN_MODE_NATIVE_REC;
    am->ain->cmd(am->ain, SLTK_CMD_AI_CH_MASK_GET, (uint32_t)&val);
    get_ch_idx(val, &am->Lmic_ch_cnt, &am->Lmic_chidx0, &am->Lmic_chidx1);
    //声道必须固定为2,如果是单声道则Lmic_chidx0和Lmic_chidx1配相同值
    if (1 == am->Lmic_ch_cnt) {
        am->Lmic_ch_cnt = 2;
        am->Lmic_chidx1 = am->Lmic_chidx0;
    }
#endif
}

void amixer_audio_linein_mode_input_sel_set(uint8_t sel)
{
#if AUDIO_KARAOKE_LINEIN_EN
    uint32_t val;
    amixer* am = g_amixer;
    audio_service_disable_input_source(audio_linein_mode_input_sel_get());
    audio_linein_mode_input_sel_set(sel);
    audio_service_enable_input_source(audio_linein_mode_input_sel_get());
    //获取此模式的ch_mask
    val = AUDIO_IN_MODE_LINEIN;
    am->ain->cmd(am->ain, SLTK_CMD_AI_CH_MASK_GET, (uint32_t)&val);
    get_ch_idx(val, &am->aux_ch_cnt, &am->aux_chidx0, &am->aux_chidx1);
    //声道必须固定为2,如果是单声道则aux_chidx0和aux_chidx1配相同值
    if (1 == am->aux_ch_cnt) {
        am->aux_ch_cnt = 2;
        am->aux_chidx1 = am->aux_chidx0;
    }
#endif
}


void soundcard_tracks_create(void)
{
    amixer* am = g_amixer;

    am_playback_volume_set(am_volume_info_get(AM_VOLUME_INFO_PLAYBACK));
    am_record_volume_set(am_volume_info_get(AM_VOLUME_INFO_RECORD));
    #if LCD_SEG_EN
    disp_lcd_volume_update(am_record_volume_get()/10);//音量
    #endif

    if(mode_sta_get() == MODE_G24) {
#if AUDIO_G24_RX_DECODER_DISABLE
#if G24_MONITOR_EN
        g24_rx_packet_size_set(wrx_track_bs_len_get());
#else
        g24_rx_packet_size_set(0);
#endif
        amixer_xet_ctrl(AMIXER_CMD_AOUT_REQ, 0, 0);
#else
        wrx_track_init_params_t prm;
        prm.link = 0;
#if (0 == AUDIO_G24_RX_DECODER_SEL)
        prm.plugin = wrx_sbc_plugin;
#else
        prm.plugin = wrx_lc3_plugin;
#endif
        g24_decode_sel_set(AUDIO_G24_RX_DECODER_SEL);
        am->wrx_track[0] = wrx_track_create(&prm); //无线接收播放通路0
#if G24_DUAL_CHANNEL
        if(G24_FUNC_ROLE == G24_FUNC_ROLE_DONGLE) {
            prm.link = 1;
            am->wrx_track[1] = wrx_track_create(&prm); //无线接收播放通路1
        }
#endif
#endif

#if USB_DEVICE_EN
    am->usbplay_track = usbplay_track_create(); //usbplay播放通路
#endif
    wrx_track_start(am->wrx_track[0]);
#if G24_DUAL_CHANNEL
    if(G24_FUNC_ROLE == G24_FUNC_ROLE_DONGLE) {
        wrx_track_start(am->wrx_track[1]);
    }
#endif
    //wrx_mute_set(am->wrx_track[0], 1);
    //usbplay_mute_set(am->usbplay_track, 1);
    }//if(mode_sta_get() == MODE_G24) 
}

void usbplay_start(uint32_t fs)
{
#if USB_DEVICE_EN
    amixer* am = g_amixer;

    usbplay_track_start(am->usbplay_track, fs);
#endif    
}

AMIXER_SRAM_FUNC
bool is_usbplay_ready(void)
{
#if USB_DEVICE_EN
    amixer* am = g_amixer;

    return usbplay_ready_get(am->usbplay_track);
#else
    return false;
#endif
}


/////////////////////////音量控制相关函数////////////////////////////////

void Lmic_listener_dry_vol_set(float vol_db)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->Lmic_listener_dry_vol_db = vol_db;

    if (!am->Lmic_listener_dry_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->Lmic_listener_dry_vol_ctrl, val);
}

float Lmic_listener_dry_vol_get(void)
{
    amixer* am = g_amixer;

    return am->Lmic_listener_dry_vol_db;
}

void Lmic_listener_dry_vol_mute_set(uint8_t mute)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->Lmic_listener_dry_vol_mute = mute;
    if (!am->Lmic_listener_dry_vol_mute) {
        val = volume_db_to_volf32(am->Lmic_listener_dry_vol_db);
    };
    volume_control_set_volume(&am->Lmic_listener_dry_vol_ctrl, val);
}

uint8_t Lmic_listener_dry_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->Lmic_listener_dry_vol_mute;
}


void Lmic_wet_vol_set(float vol_db)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->Lmic_wet_vol_db = vol_db;

    if (!am->Lmic_wet_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->Lmic_wet_vol_ctrl, val);
}

float Lmic_wet_vol_get(void)
{
    amixer* am = g_amixer;

    return am->Lmic_wet_vol_db;
}

void Lmic_wet_vol_mute_set(uint8_t mute)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->Lmic_wet_vol_mute = mute;
    if (!am->Lmic_wet_vol_mute) {
        val = volume_db_to_volf32(am->Lmic_wet_vol_db);
    };
    volume_control_set_volume(&am->Lmic_wet_vol_ctrl, val);
}

uint8_t Lmic_wet_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->Lmic_wet_vol_mute;
}

void Lmic_play_dry_vol_set(float vol_db)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->Lmic_play_dry_vol_db = vol_db;

    if (!am->Lmic_play_dry_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->Lmic_play_dry_vol_ctrl, val);
}

float Lmic_play_dry_vol_get(void)
{
    amixer* am = g_amixer;

    return am->Lmic_play_dry_vol_db;
}

void Lmic_play_dry_vol_mute_set(uint8_t mute)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->Lmic_play_dry_vol_mute = mute;
    if (!am->Lmic_play_dry_vol_mute) {
        val = volume_db_to_volf32(am->Lmic_play_dry_vol_db);
    };
    volume_control_set_volume(&am->Lmic_play_dry_vol_ctrl, val);
}

uint8_t Lmic_play_dry_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->Lmic_play_dry_vol_mute;
}

void Lmic_g24_tx_dry_vol_set(float vol_db)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->Lmic_g24_tx_dry_vol_db = vol_db;

    if (!am->Lmic_g24_tx_dry_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->Lmic_g24_tx_dry_vol_ctrl, val);
}

float Lmic_g24_tx_dry_vol_get(void)
{
    amixer* am = g_amixer;

    return am->Lmic_g24_tx_dry_vol_db;
}

void Lmic_g24_tx_dry_vol_mute_set(uint8_t mute)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->Lmic_g24_tx_dry_vol_mute = mute;
    if (!am->Lmic_g24_tx_dry_vol_mute) {
        val = volume_db_to_volf32(am->Lmic_g24_tx_dry_vol_db);
    };
    volume_control_set_volume(&am->Lmic_g24_tx_dry_vol_ctrl, val);
}

uint8_t Lmic_g24_tx_dry_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->Lmic_g24_tx_dry_vol_mute;
}

bool Lmic_aenc_enable_get(void)
{
    amixer* am = g_amixer;

    if(NULL == am->aenc_handle) {
        return false;
    }
    
    return am->aenc_enable;
}


void Lmic_aenc_enable_set(uint8_t enable)
{
    amixer* am = g_amixer;

    if(NULL == am->aenc_handle) {
        return;
    }
    
    am->aenc_enable = enable;
    if(am->aenc_enable) {
        audio_service_add_features(AUDIO_FEATURE_MIC_AENC);
    } else {
        audio_service_clear_features(AUDIO_FEATURE_MIC_AENC);
    }
    auto_set_cpux_freq();
}

void wrx_listener_dry_vol_set(float vol_db)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->wrx_listener_dry_vol_db = vol_db;

    if (!am->wrx_listener_dry_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->wrx_listener_dry_vol_ctrl, val);
}

float wrx_listener_dry_vol_get(void)
{
    amixer* am = g_amixer;

    return am->wrx_listener_dry_vol_db;
}

void wrx_listener_dry_vol_mute_set(uint8_t mute)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->wrx_listener_dry_vol_mute = mute;
    if (!am->wrx_listener_dry_vol_mute) {
        val = volume_db_to_volf32(am->wrx_listener_dry_vol_db);
    };
    volume_control_set_volume(&am->wrx_listener_dry_vol_ctrl, val);
}

uint8_t wrx_listener_dry_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->wrx_listener_dry_vol_mute;
}


void wrx_wet_vol_set(float vol_db)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->wrx_wet_vol_db = vol_db;

    if (!am->wrx_wet_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->wrx_wet_vol_ctrl, val);
}

float wrx_wet_vol_get(void)
{
    amixer* am = g_amixer;

    return am->wrx_wet_vol_db;
}

void wrx_wet_vol_mute_set(uint8_t mute)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->wrx_wet_vol_mute = mute;
    if (!am->wrx_wet_vol_mute) {
        val = volume_db_to_volf32(am->wrx_wet_vol_db);
    };
    volume_control_set_volume(&am->wrx_wet_vol_ctrl, val);
}

uint8_t wrx_wet_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->wrx_wet_vol_mute;
}

void wrx_play_dry_vol_set(float vol_db)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->wrx_play_dry_vol_db = vol_db;

    if (!am->wrx_play_dry_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->wrx_play_dry_vol_ctrl, val);
}

float wrx_play_dry_vol_get(void)
{
    amixer* am = g_amixer;

    return am->wrx_play_dry_vol_db;
}

void wrx_play_dry_vol_mute_set(uint8_t mute)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->wrx_play_dry_vol_mute = mute;
    if (!am->wrx_play_dry_vol_mute) {
        val = volume_db_to_volf32(am->wrx_play_dry_vol_db);
    };
    volume_control_set_volume(&am->wrx_play_dry_vol_ctrl, val);
}

uint8_t wrx_play_dry_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->wrx_play_dry_vol_mute;
}

void mic_wet_listener_vol_set(float vol_db)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->mic_wet_listener_vol_db = vol_db;

    if (!am->mic_wet_listener_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->mic_wet_listener_vol_ctrl, val);
}

float mic_wet_listener_vol_get(void)
{
    amixer* am = g_amixer;

    return am->mic_wet_listener_vol_db;
}

void mic_wet_listener_vol_mute_set(uint8_t mute)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->mic_wet_listener_vol_mute = mute;
    if (!am->mic_wet_listener_vol_mute) {
        val = volume_db_to_volf32(am->mic_wet_listener_vol_db);
    };
    volume_control_set_volume(&am->mic_wet_listener_vol_ctrl, val);
}

uint8_t mic_wet_listener_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->mic_wet_listener_vol_mute;
}

void mic_wet_play_vol_set(float vol_db)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->mic_wet_play_vol_db = vol_db;

    if (!am->mic_wet_play_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->mic_wet_play_vol_ctrl, val);
}

float mic_wet_play_vol_get(void)
{
    amixer* am = g_amixer;

    return am->mic_wet_play_vol_db;
}

void mic_wet_play_vol_mute_set(uint8_t mute)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->mic_wet_play_vol_mute = mute;
    if (!am->mic_wet_play_vol_mute) {
        val = volume_db_to_volf32(am->mic_wet_play_vol_db);
    };
    volume_control_set_volume(&am->mic_wet_play_vol_ctrl, val);
}

uint8_t mic_wet_play_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->mic_wet_play_vol_mute;
}

void mic_wet_g24_tx_vol_set(float vol_db)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->mic_wet_g24_tx_vol_db = vol_db;

    if (!am->mic_wet_g24_tx_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->mic_wet_g24_tx_vol_ctrl, val);
}

float mic_wet_g24_tx_vol_get(void)
{
    amixer* am = g_amixer;

    return am->mic_wet_g24_tx_vol_db;
}

void mic_wet_g24_tx_vol_mute_set(uint8_t mute)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->mic_wet_g24_tx_vol_mute = mute;
    if (!am->mic_wet_g24_tx_vol_mute) {
        val = volume_db_to_volf32(am->mic_wet_g24_tx_vol_db);
    };
    volume_control_set_volume(&am->mic_wet_g24_tx_vol_ctrl, val);
}

uint8_t mic_wet_g24_tx_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->mic_wet_g24_tx_vol_mute;
}

void tone_listener_vol_set(float vol_db)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->tone_listener_vol_db = vol_db;

    if (!am->tone_listener_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->tone_listener_vol_ctrl, val);
}

float tone_listener_vol_get(void)
{
    amixer* am = g_amixer;

    return am->tone_listener_vol_db;
}

void tone_listener_vol_mute_set(uint8_t mute)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->tone_listener_vol_mute = mute;
    if (!am->tone_listener_vol_mute) {
        val = volume_db_to_volf32(am->tone_listener_vol_db);
    };
    volume_control_set_volume(&am->tone_listener_vol_ctrl, val);
}

uint8_t tone_listener_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->tone_listener_vol_mute;
}

void tone_play_vol_set(float vol_db)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->tone_play_vol_db = vol_db;

    if (!am->tone_play_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->tone_play_vol_ctrl, val);
}

float tone_play_vol_get(void)
{
    amixer* am = g_amixer;

    return am->tone_play_vol_db;
}

void tone_play_vol_mute_set(uint8_t mute)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->tone_play_vol_mute = mute;
    if (!am->tone_play_vol_mute) {
        val = volume_db_to_volf32(am->tone_play_vol_db);
    };
    volume_control_set_volume(&am->tone_play_vol_ctrl, val);
}

uint8_t tone_play_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->tone_play_vol_mute;
}

void tone_g24_tx_vol_set(float vol_db)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->tone_g24_tx_vol_db = vol_db;

    if (!am->tone_g24_tx_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->tone_g24_tx_vol_ctrl, val);
}

float tone_g24_tx_vol_get(void)
{
    amixer* am = g_amixer;

    return am->tone_g24_tx_vol_db;
}

void tone_g24_tx_vol_mute_set(uint8_t mute)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->tone_g24_tx_vol_mute = mute;
    if (!am->tone_listener_vol_mute) {
        val = volume_db_to_volf32(am->tone_g24_tx_vol_db);
    };
    volume_control_set_volume(&am->tone_g24_tx_vol_ctrl, val);
}

uint8_t tone_g24_tx_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->tone_g24_tx_vol_mute;
}


void aux_listener_vol_set(float vol_db)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->aux_listener_vol_db = vol_db;

    if (!am->aux_listener_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->aux_listener_vol_ctrl, val);
}

float aux_listener_vol_get(void)
{
    amixer* am = g_amixer;

    return am->aux_listener_vol_db;
}

void aux_listener_vol_mute_set(uint8_t mute)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->aux_listener_vol_mute = mute;
    if (!am->aux_listener_vol_mute) {
        val = volume_db_to_volf32(am->aux_listener_vol_db);
    };
    volume_control_set_volume(&am->aux_listener_vol_ctrl, val);
}

uint8_t aux_listener_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->aux_listener_vol_mute;
}

void aux_play_vol_set(float vol_db)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->aux_play_vol_db = vol_db;

    if (!am->aux_play_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->aux_play_vol_ctrl, val);
}

float aux_play_vol_get(void)
{
    amixer* am = g_amixer;

    return am->aux_play_vol_db;
}

void aux_play_vol_mute_set(uint8_t mute)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->aux_play_vol_mute = mute;
    if (!am->aux_play_vol_mute) {
        val = volume_db_to_volf32(am->aux_play_vol_db);
    };
    volume_control_set_volume(&am->aux_play_vol_ctrl, val);
}

uint8_t aux_play_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->aux_play_vol_mute;
}

void aux_g24_tx_vol_set(float vol_db)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->aux_g24_tx_vol_db = vol_db;

    if (!am->aux_g24_tx_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->aux_g24_tx_vol_ctrl, val);
}

float aux_g24_tx_vol_get(void)
{
    amixer* am = g_amixer;

    return am->aux_g24_tx_vol_db;
}

void aux_g24_tx_vol_mute_set(uint8_t mute)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->aux_g24_tx_vol_mute = mute;
    if (!am->aux_g24_tx_vol_mute) {
        val = volume_db_to_volf32(am->aux_g24_tx_vol_db);
    };
    volume_control_set_volume(&am->aux_g24_tx_vol_ctrl, val);
}

uint8_t aux_g24_tx_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->aux_g24_tx_vol_mute;
}

void playback_vol_set(float vol_db)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->playback_vol_db = vol_db;

    if (!am->playback_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->playback_vol_ctrl, val);
}

float playback_vol_get(void)
{
    amixer* am = g_amixer;

    return am->playback_vol_db;
}

void playback_vol_mute_set(uint8_t mute)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->playback_vol_mute = mute;
    if (!am->playback_vol_mute) {
        val = volume_db_to_volf32(am->playback_vol_db);
    };
    volume_control_set_volume(&am->playback_vol_ctrl, val);
}

uint8_t playback_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->playback_vol_mute;
}

void record_vol_set(float vol_db)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->record_vol_db = vol_db;

    if (!am->record_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->record_vol_ctrl, val);
}

float record_vol_get(void)
{
    amixer* am = g_amixer;

    return am->record_vol_db;
}

void record_vol_mute_set(uint8_t mute)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->record_vol_mute = mute;
    if (!am->record_vol_mute) {
        val = volume_db_to_volf32(am->record_vol_db);
    };
    volume_control_set_volume(&am->record_vol_ctrl, val);
}

uint8_t record_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->record_vol_mute;
}

void usbplay_listener_vol_set(float vol_db)
{
#if USB_DEVICE_EN
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->usbplay_listener_vol_db = vol_db;

    if (!am->usbplay_listener_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->usbplay_listener_vol_ctrl, val);
#endif    
}

float usbplay_listener_vol_get(void)
{
#if USB_DEVICE_EN
    amixer* am = g_amixer;

    return am->usbplay_listener_vol_db;
#else
    return 0.0;
#endif
}

void usbplay_listener_vol_mute_set(uint8_t mute)
{
#if USB_DEVICE_EN
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->usbplay_listener_vol_mute = mute;
    if (!am->usbplay_listener_vol_mute) {
        val = volume_db_to_volf32(am->usbplay_listener_vol_db);
    };
    volume_control_set_volume(&am->usbplay_listener_vol_ctrl, val);
#endif    
}

uint8_t usbplay_listener_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->usbplay_listener_vol_mute;
}

void usbplay_play_vol_set(float vol_db)
{
#if USB_DEVICE_EN
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->usbplay_play_vol_db = vol_db;

    if (!am->usbplay_play_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->usbplay_play_vol_ctrl, val);
#endif    
}

float usbplay_play_vol_get(void)
{
    amixer* am = g_amixer;

    return am->usbplay_play_vol_db;
}

void usbplay_play_vol_mute_set(uint8_t mute)
{
#if USB_DEVICE_EN
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->usbplay_play_vol_mute = mute;
    if (!am->usbplay_play_vol_mute) {
        val = volume_db_to_volf32(am->usbplay_play_vol_db);
    };
    volume_control_set_volume(&am->usbplay_play_vol_ctrl, val);
#endif    
}

uint8_t usbplay_play_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->usbplay_play_vol_mute;
}

void usbplay_g24_tx_vol_set(float vol_db)
{
#if USB_DEVICE_EN
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->usbplay_g24_tx_vol_db = vol_db;

    if (!am->usbplay_g24_tx_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->usbplay_g24_tx_vol_ctrl, val);
#endif    
}

float usbplay_g24_tx_vol_get(void)
{
    amixer* am = g_amixer;

    return am->usbplay_g24_tx_vol_db;
}

void usbplay_g24_tx_vol_mute_set(uint8_t mute)
{
#if USB_DEVICE_EN
    amixer* am = g_amixer;
    float val = (float)0.0;

    am->usbplay_g24_tx_vol_mute = mute;
    if (!am->usbplay_g24_tx_vol_mute) {
        val = volume_db_to_volf32(am->usbplay_g24_tx_vol_db);
    };
    volume_control_set_volume(&am->usbplay_g24_tx_vol_ctrl, val);
#endif    
}

uint8_t usbplay_g24_tx_vol_mute_get(void)
{
    amixer* am = g_amixer;

    return am->usbplay_g24_tx_vol_mute;
}

void playback_percent_vol_set(uint8_t vol)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    logi("playback_vol:%d\n",vol);
    float vol_db = vol2decibels(vol);

    am->playback_vol_db = vol_db;

    if (!am->playback_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->playback_vol_ctrl, val);
}

int am_playback_volume_set(uint8_t vol)
{
    amixer* am = g_amixer;
    am->playback_volume = vol;
    playback_percent_vol_set(am->playback_volume);
    am_volume_info_set(AM_VOLUME_INFO_PLAYBACK,am->playback_volume);
    return PLAYER_RET_SUCCESS;
}

uint8_t am_playback_volume_get(void)
{
    amixer* am = g_amixer;
    return am->playback_volume;
}

int am_playback_volume_down(void)
{
    amixer* am = g_amixer;
    if (am->playback_volume >= VOLUME_STEP) {
        am->playback_volume -= VOLUME_STEP;
        playback_percent_vol_set(am->playback_volume);
        am_volume_info_set(AM_VOLUME_INFO_PLAYBACK,am->playback_volume);
    } else if (am->playback_volume != 0) {  //PC mode np.volume step is 2, because PC have 50 steps
        am->playback_volume = 0;
        playback_percent_vol_set(am->playback_volume);
        am_volume_info_set(AM_VOLUME_INFO_PLAYBACK,am->playback_volume);
    }

#if (TONE_EN && TONE_MIN_VOLUME_EN)
    if (am->playback_volume == 0) {
        am_tone_play(SYS_TONE_VOL_MIN_NAME, 0, 0, TONE_G24_MODE_VOL, 0, 0); /* 播放最小音量提示音 */
    }
#endif

    return PLAYER_RET_SUCCESS;
}

int am_playback_volume_up(void)
{
    amixer* am = g_amixer;
    if (am->playback_volume <= VOLUME_MAX - VOLUME_STEP) {
        am->playback_volume += VOLUME_STEP;
        playback_percent_vol_set(am->playback_volume);
        am_volume_info_set(AM_VOLUME_INFO_PLAYBACK,am->playback_volume);
    } else if (am->playback_volume != VOLUME_MAX) {  //PC mode np.volume step is 2, because PC have 50 steps
        am->playback_volume = VOLUME_MAX;
        playback_percent_vol_set(am->playback_volume);
        am_volume_info_set(AM_VOLUME_INFO_PLAYBACK,am->playback_volume);
    }

#if (TONE_EN && TONE_MAX_VOLUME_EN)
    if (am->playback_volume == VOLUME_MAX) {
        am_tone_play(SYS_TONE_VOL_MAX_NAME, 0, 0, TONE_G24_MODE_VOL, 0, 0); /* 播放最大音量提示音 */
    }
#endif

    return PLAYER_RET_SUCCESS;
}

void record_percent_vol_set(uint8_t vol)
{
    amixer* am = g_amixer;
    float val = (float)0.0;

    logi("record_vol:%d\n",vol);
    #if LCD_SEG_EN
    disp_lcd_volume_update(vol / 10);//音量显示
    #endif
    float vol_db = vol2decibels(vol);

    am->record_vol_db = vol_db;

    if (!am->record_vol_mute) {
        val = volume_db_to_volf32(vol_db);
    };
    volume_control_set_volume(&am->record_vol_ctrl, val);
}

int am_record_volume_set(uint8_t vol)
{
    amixer* am = g_amixer;
    am->record_volume = vol;
    record_percent_vol_set(am->record_volume);
    am_volume_info_set(AM_VOLUME_INFO_RECORD,am->record_volume);
    return PLAYER_RET_SUCCESS;
}

uint8_t am_record_volume_get(void)
{
    amixer* am = g_amixer;
    return am->record_volume;
}

int am_record_volume_down(void)
{
    amixer* am = g_amixer;
    if (am->record_volume >= VOLUME_STEP) {
        am->record_volume -= VOLUME_STEP;
        record_percent_vol_set(am->record_volume);
        am_volume_info_set(AM_VOLUME_INFO_RECORD,am->record_volume);
    } else if (am->record_volume != 0) {
        am->record_volume = 0;
        record_percent_vol_set(am->record_volume);
        am_volume_info_set(AM_VOLUME_INFO_RECORD,am->record_volume);
    }

    // disp_lcd_volume_update(am->record_volume / 10);//音量显示

#if (TONE_EN && TONE_MIN_VOLUME_EN)
    if (am->record_volume == 0) {
        am_tone_play(SYS_TONE_VOL_MIN_NAME, 0, 0, TONE_G24_MODE_VOL, 0, 0); /* 播放最小音量提示音 */
    }
#endif

    return PLAYER_RET_SUCCESS;
}

int am_record_volume_up(void)
{
    amixer* am = g_amixer;
    if (am->record_volume <= VOLUME_MAX - VOLUME_STEP) {
        am->record_volume += VOLUME_STEP;
        record_percent_vol_set(am->record_volume);
        am_volume_info_set(AM_VOLUME_INFO_RECORD,am->record_volume);
    } else if (am->record_volume != VOLUME_MAX) {  //PC mode np.volume step is 2, because PC have 50 steps
        am->record_volume = VOLUME_MAX;
        record_percent_vol_set(am->record_volume);
        am_volume_info_set(AM_VOLUME_INFO_RECORD,am->record_volume);
    }

    // disp_lcd_volume_update(am->record_volume / 10);//音量显示

#if (TONE_EN && TONE_MAX_VOLUME_EN)
    if (am->record_volume == VOLUME_MAX) {
        am_tone_play(SYS_TONE_VOL_MAX_NAME, 0, 0, TONE_G24_MODE_VOL, 0, 0); /* 播放最大音量提示音 */
    }
#endif

    return PLAYER_RET_SUCCESS;
}


void am_usbplay_volume_set(uint8_t vol)
{
    //logi("usbplay_volume :%d",vol);
    amixer* am = g_amixer;
    float vol_db = vol2decibels(vol);
    
    if(am->usbplay_mux_sel) {
        usbplay_play_vol_set(vol_db);
        usbplay_g24_tx_vol_set(vol_db);
        usbplay_listener_vol_set(vol_db);
    } else {
        usbplay_play_vol_set(vol_db);
    }
}


void amixer_wrx_ch_mix_mode_set(uint8_t mode)
{
    amixer* am = g_amixer;

    am->wrx_ch_mix_mode = mode;
}

void amixer_vocalenhance_enable_set(bool enable)
{
    amixer* am = g_amixer;

    if (am->ve_handle) {
        am->ve_enable = enable;
    }
}

void amixer_dac_switch(bool open)
{
    //amixer* am = g_amixer;
    if(open) {
        amixer_xet_ctrl(AMIXER_CMD_AOUT_REQ, 1, 0);
    } else {
        //am->aout->cmd(am->aout, SLTK_CMD_AO_START_FADE, FADE_OUT);
        HAL_auss_ioctl(NULL, DAC_STOP_DAC_RESET, 0);        //dac stop时不复位DAC
        amixer_xet_ctrl(AMIXER_CMD_AOUT_REL, 1, 0);
    }
}

void amixer_spdif_volume_set(int8_t volume)
{

}

void amixer_localmic_volume_set(int8_t volume)
{
}

void amixer_wirelessmic_volume_set(int8_t volume)
{
}

void amixer_usb_audio_volume_set(int8_t volume)
{
    am_usbplay_volume_set(volume);
}

void amixer_i2s_volume_set(int8_t volume)
{
}

void amixer_tone_volume_set(int8_t volume)
{
}

void amixer_aux_volume_set(int8_t volume)
{
}

void amixer_play_set(int8_t volume)
{
    logi("play volume set %d", volume);
    playback_vol_set(volume);
}


bool is_afc_learnt_cpl();
#if AUDIO_MIC_AFC
void afc_adaptive()
{
    amixer* am = g_amixer;
    if (is_afc_learnt_cpl()) {
		logi("afc adaptive open");
        am_afc_mode_switch(am->afc_manager_handle, AFC_ADAPTIVE);
    }
    
}

void afc_proc()
{
    amixer* am = g_amixer;
    am_afc_mode_switch(am->afc_manager_handle, AFC_PROCESS);
}

void afc_proc_close()
{
    amixer* am = g_amixer;
    am_afc_mode_switch(am->afc_manager_handle, AFC_PROCESS_NONE);
}
#endif

void amixer_mute_config(void)
{
#if G24_FUNC_ROLE       //dongle
    //mic音效处理源数据，输出：MIC(Lmic+wrx)湿声
    Lmic_wet_vol_mute_set(true);               //有线MIC干声到音效
    wrx_wet_vol_mute_set(true);                //无线接收(无线MIC干声)到音效

    //音乐
    usbplay_play_vol_mute_set(false);           //usbplay_mux_sel控制，0:usbplay到音乐，音效前混入，1:usbplay到播放，音效后混入)
    playback_vol_mute_set(false);               //音乐输出

    //播放输出
    Lmic_play_dry_vol_mute_set(false);          //有线MIC干声到播放
    wrx_play_dry_vol_mute_set(false);           //无线接收(无线MIC干声)到播放
    aux_play_vol_mute_set(false);               //音乐(aux + usbplay)到播放
    tone_play_vol_mute_set(false);              //提示音到播放
    mic_wet_play_vol_mute_set(false);           //MIC(Lmic+wrx)湿声到播放

    //2.4G发送
    Lmic_g24_tx_dry_vol_mute_set(false);        //有线MIC干声到2.4G发送
    aux_g24_tx_vol_mute_set(false);             //音乐(aux + usbplay)到2.4G发送
    usbplay_g24_tx_vol_mute_set(false);         //usbplay到2.4G发送(usbplay_mux_sel为1时有效)
    tone_g24_tx_vol_mute_set(false);            //提示音到2.4G发送
    mic_wet_g24_tx_vol_mute_set(false);         //MIC(Lmic+wrx)湿声到2.4G发送

    //USB上行(监听)
    Lmic_listener_dry_vol_mute_set(false);      //有线MIC干声到监听
    wrx_listener_dry_vol_mute_set(false);       //无线接收(无线MIC干声)到监听
    aux_listener_vol_mute_set(false);           //音乐(aux + usbplay)到监听
    usbplay_listener_vol_mute_set(true);       //usbplay到监听(usbplay_mux_sel为1时有效)
    tone_listener_vol_mute_set(false);          //提示音到监听
    mic_wet_listener_vol_mute_set(false);       //MIC(Lmic+wrx)湿声到监听
#else
    //mic音效处理源数据，输出：MIC(Lmic)湿声
    Lmic_wet_vol_mute_set(true);               //有线MIC干声到音效

    //音乐
    playback_vol_mute_set(false);               //音乐输出

    //播放输出
    Lmic_play_dry_vol_mute_set(false);          //有线MIC干声到播放
    wrx_play_dry_vol_mute_set(false);           //无线接收到播放
    aux_play_vol_mute_set(false);               //音乐(aux)到播放
    tone_play_vol_mute_set(false);              //提示音到播放
	mic_wet_play_vol_mute_set(false);           //MIC(Lmic)湿声到播放

    //2.4G发送
    Lmic_g24_tx_dry_vol_mute_set(false);        //有线MIC干声到2.4G发送
    aux_g24_tx_vol_mute_set(false);             //音乐(aux)到2.4G发送
    tone_g24_tx_vol_mute_set(false);            //提示音到2.4G发送
#endif
}


