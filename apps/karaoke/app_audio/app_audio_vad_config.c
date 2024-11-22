#define LOG_LEV 4
#include "sys_inc.h"
#include "hal.h"
#include "kws_manager.h"
#include "user_config.h"
#include "audio_params_config.h"
#include "info_mem.h"
#include "psm_manager.h"


#if AUDIO_KWS_EN
void on_keyword_wakeup(const char *keyword)
{
    //logd("keyword: %s", keyword);
    event_put(AUDIO_KWS_EVENT_WAKEUP, keyword, strlen(keyword) + 1);
}

void app_audio_kws_init(void)
{
#if AUDIO_KWS_EN
    am_kws_init_params_t kws_prm;
    kws_prm.on_keyword_wakeup = on_keyword_wakeup;
#ifdef CFG_VAD_MIC_PGA_GAIN
    kws_prm.mic_pga_gain = CFG_VAD_MIC_PGA_GAIN;
#else
    kws_prm.mic_pga_gain = 4;
#endif
    /* minimum psd of noise in dBfs */
#ifdef CFG_VAD_LAMBDA_MIN
    kws_prm.lambda_min = CFG_VAD_LAMBDA_MIN;
#else
    kws_prm.lambda_min = -70;
#endif
    /* minimum psd of voice in concerned freq region in dBfs */
#ifdef CFG_VAD_VOICE_MIN
    kws_prm.voice_min = CFG_VAD_VOICE_MIN;
#else
    kws_prm.voice_min = -50;
#endif
    am_kws_create(&kws_prm);

#if BT_AIR_SEL
    uint8_t kws;
    int ret;
    ret = sys_info_read(KWS_INFO, &kws, 1);
    if (ret < 0) {
        kws = AIR_KWS_DEFAULT_EN;
        sys_info_write(KWS_INFO, &kws, 1);
    }
    logi("kws:%x", kws);
    am_kws_global_enable_set(kws);
#else
    am_kws_global_enable_set(1);
#endif

#endif
}
#endif //#if AUDIO_KWS_EN