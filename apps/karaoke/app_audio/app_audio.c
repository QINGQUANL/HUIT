#define LOG_LEV 4
#include "sys_inc.h"
#include "audio_service.h"
#include "hal.h"
#include "middleware.h"
#include "user_config.h"
#include "app_audio_codec_config.h"
#include "bt_audio_service.h"
#include "app_audio_btaudio_config.h"
#include "app_audio_tone_config.h"
#include "app_audio_anc_config.h"
#include "app_audio_vad_config.h"
#include "local_play_manager.h"
#include "audio_out.h"
#include "audio_in.h"
#include "eq_loader_common.h"
#include "app_audio_mplayer.h"
#include "app_audio_dbg.h"
#include "soundcard.h"
#include "cpu_freq.h"
#include "hal_eq.h"
#include "audio_afx_config.h"
#include "app_psm.h"
#include "app_lcdc.h"
#include "mode_manager.h"
#include "app_spdif.h"

uint32_t mode_sta_get(void);

void app_audio_malloc_msg(uint32_t cmd, uint32_t param)
{
    switch(cmd) {
    case AUDIO_MALLOC_MSG_FAIL:
        loge("err audio_malloc fail(size=%d)", param);
        break;
    default:
        break;
    }
}


void app_audio_set_cpu_freq(void)
{
    cpu_freq_table cpu_freq_tab = {
        .amixer_freq       = DVFS_AMIXER_FREQ,
        .effects_freq      = DVFS_EFFECTS_FREQ,
        .tone_mixer_freq   = DVFS_TONEMIXER_FREQ,
        .a2dp_audio_freq   = DVFS_A2DPAUDIO_FREQ,
        .sco_audio_freq    = DVFS_SCOAUDIO_FREQ,
        .sco_anec_freq     = DVFS_SCOAENC_FREQ,
        .usb_audio_freq    = DVFS_USBAUDIO_FREQ,
        .linein_audio_freq = DVFS_LINEINAUDIO_FREQ,

        .encode_wav_freq   = DVFS_LOCAL_ENCODE_WAV_FREQ,
        .encode_mp3_freq   = DVFS_LOCAL_ENCODE_MP3_FREQ,
        .decode_wav_freq   = DVFS_LOCAL_DECODE_WAV_FREQ,
        .decode_mp3_freq   = DVFS_LOCAL_DECODE_MP3_FREQ,
        .default_freq      = DVFS_DEFAULT_FREQ,
    };

#if AUDIO_IN_DEVICE_SEL
    if (1 == audio_service_get_input_channels_count(AUDIO_REC_INPUT_SEL)) {
        uint32_t freq[2] = DVFS_SCOAENC_FREQ;
        cpu_freq_tab.sco_anec_freq[0] = freq[0];
        cpu_freq_tab.sco_anec_freq[1] = freq[1];
    } else {
        uint32_t freq[2] = DVFS_SCOADMAENC_FREQ;
        cpu_freq_tab.sco_anec_freq[0] = freq[0];
        cpu_freq_tab.sco_anec_freq[1] = freq[1];
    }
#endif
    cpufreq_init_table(&cpu_freq_tab);
}

void load_all_eq_setting()
{
    logi("load all eq setting");
    //load eq
    load_eq_setting(0, PAGE_MUSIC);
    load_eq_setting(0, PAGE_VSS);
    load_eq_setting(0, PAGE_EFFECT);
    load_eq_setting(0, PAGE_MIC);
    load_eq_setting(0, PAGE_PLG);
    load_eq_setting(0, PAGE_HW);
}


void app_audio_init(void)
{
    audio_malloc_msg_callback_register(app_audio_malloc_msg);
    
    //关闭耳放
    HP_AMP_OFF();
    
    //硬件模块配置
    app_audio_hw_init();
    HAL_eq_init(1);
    HAL_asrc_init(3);
#if USB_MIC_NEED_RESAMPLE
    HAL_asrc_ioctrl(NULL, "clkdiv", 2);
#elif SPDIF_RX_EN
    HAL_asrc_ioctrl(NULL, "clkdiv", 3);
#endif
    //aout配置
    aout_params_t aout_params;
    #if (0 == AUDIO_OUT_DEVICE_SEL)
    aout_params.snd_id = SND_I2S;
    aout_params.hw_ch_mask = 0x3;
    #else
    aout_params.snd_id = SND_CODEC;
    aout_params.hw_ch_mask = AUDIO_DAC_OUT_CH_MASK;
    #endif
    aout_params_set(&aout_params);

    //ain配置
    ain_params_t ain_params;
    #if (0 == AUDIO_IN_DEVICE_SEL)
    ain_params.snd_id = SND_I2S;
    ain_params.get_input_source_func = NULL;
    #else
    ain_params.snd_id = SND_CODEC;
    ain_params.get_input_source_func = codec_adc_input_source_get;
    #endif
    ain_params_set(&ain_params);

    //audio基础服务初始化
    audio_service_init_params_t params;

#ifdef AUDIO_HEAP_SIZE_TURNING
    params.alg_memory_size = AUDIO_HEAP_SIZE_TURNING * 1024;
#else
    params.alg_memory_size = 246 * 1024;
#endif
    params.vbus_in_mute = AUDIO_MUTE_EN;
    params.vbus_in_pause = AUDIO_PAUSE_WHEN_VBUS_IN_EN;
    params.incoming_ringtone_fix_volume = AUDIO_INCOMING_RINGTONE_VOL_EN ? AUDIO_INCOMING_RINGTONE_VOL : 0;
    audio_service_init(&params);

    //注册音效
    if(!nor_mp_dut_flag_get()) {
        audioeffect_register();
    }

    app_audio_set_cpu_freq();
    
#if LCDC_EN
    lcd_disp_init();
#endif

    audio_service_thread_init();
//#if !AUDIO_KARAOKE_EN
    if(!AUDIO_KARAOKE_EN || (mode_sta_get() == MODE_BT)) {
        audio_arec_thread_init();
    }
//#endif

    //注册mp3解码器
    parser_plugin_register(SLTK_CTYPE_MP3, &mp3_parser_plugin);
    decoder_plugin_register(SLTK_ATYPE_MP3, &mp3_decoder_plugin);
    //注册wav解码器
    parser_plugin_register(SLTK_CTYPE_WAV, &wav_parser_plugin);
    decoder_plugin_register(SLTK_ATYPE_PCM, &pcm_decoder_plugin);

    //音效服务初始化

#if AUDIO_ANC_EN
    //ANC服务初始化
    app_audio_anc_init();
#endif

#if SPDIF_RX_EN
    spdif_input_sel(SEL_OPT);
#endif

#if SPDIF_TX_EN
    HAL_gpio_config(SPDIF_TX_PIN_CFG);
#endif


#if MODE_BT_EN
    //蓝牙音频服务初始化
    if(mode_sta_get() == MODE_BT) {
        app_audio_btaudio_init();
    }
#endif

    //提示音服务初始化
    app_audio_tone_config();

    //本地播放服务初始化
#if MODE_MUSIC_EN
    app_audio_mplayer_init();
#endif

    load_all_eq_setting();

    //dbg init
    app_audio_debug_init();

#if AUDIO_KWS_EN
    //kws服务初始化
    app_audio_kws_init();
#endif

    if(!nor_mp_dut_flag_get()) {
//#if AUDIO_KARAOKE_EN
        if(AUDIO_KARAOKE_EN && (mode_sta_get() ==MODE_G24)) {
            soundcard_tracks_create();
        }
//#endif

//#if AUDIO_KARAOKE_EN
        //打开音效
        if(AUDIO_KARAOKE_EN && (mode_sta_get() ==MODE_G24) ) {
            audioeffect_enable();
        }
//#endif
    }

}
