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
#include "audio_heap.h"


#define I2S_CH (4)

/////////////////////////////////////////dongle mic i2s afc /////////////////////////////////
typedef struct {
    struct pcm *out;
    struct pcm *in;
    int32_t *pcmbuf;
} dongle_mic_i2s_afc_t;

void dongle_mic_i2s_afc_close(void *handle)
{
    dongle_mic_i2s_afc_t *hd = handle;

    if (hd) {
        if (hd->out) {
            pcm_close(hd->out);
        }
        if (hd->in) {
            pcm_close(hd->in);
        }
        if (hd->pcmbuf) {
            general_free(hd->pcmbuf);
        }

        general_free(hd);
    }
}


void dongle_mic_i2s_afc_send(void *handle, int32_t *micbuf, int32_t *refbuf, uint32_t samples)
{
    dongle_mic_i2s_afc_t *hd = handle;

    if (NULL == hd) {
        logd("NULL");
        return;
    }

    for (int i = 0; i < samples; i++) {
        hd->pcmbuf[4*i+0] = micbuf[2*i];
        hd->pcmbuf[4*i+1] = micbuf[2*i+1];
        hd->pcmbuf[4*i+2] = refbuf[2*i];
        hd->pcmbuf[4*i+3] = refbuf[2*i+1];
    }
    pcm_write(hd->out, hd->pcmbuf, samples);
}

void dongle_mic_i2s_afc_received(void *handle, int32_t *micbuf, uint32_t samples)
{
    dongle_mic_i2s_afc_t *hd = handle;

    if (NULL == hd) {
        return;
    }

    pcm_read(hd->in, hd->pcmbuf, samples);

    for (int i = 0; i < samples; i++) {
        micbuf[2*i+0] = hd->pcmbuf[4*i+0];
        micbuf[2*i+1] = hd->pcmbuf[4*i+1];
    }
}

void *dongle_mic_i2s_afc_open(void)
{
    dongle_mic_i2s_afc_t *hd;
    struct pcm_config out_config;
    struct pcm_config in_config;

    hd = general_calloc(sizeof(*hd));
    if (hd) {
        out_config.ch = I2S_CH;
        out_config.sample_size = sizeof(int32_t);
        out_config.rate = 48000;
        out_config.period_size = G_AUDIO_PROC_PERIOD;
        out_config.period_count = 2;
        out_config.start_threshold = 0;
        out_config.stop_threshold = 0;
        hd->out = pcm_open(SND_I2S, PCM_OUT, &out_config);

        in_config.ch = I2S_CH;
        in_config.sample_size = sizeof(int32_t);
        in_config.rate = 48000;
        in_config.period_size = G_AUDIO_PROC_PERIOD;
        in_config.period_count = 2;
        in_config.start_threshold = 0;
        in_config.stop_threshold = 0;
        hd->in = pcm_open(SND_I2S, PCM_IN, &in_config);

        hd->pcmbuf = general_calloc(G_AUDIO_PROC_PERIOD*I2S_CH*sizeof(int32_t));

        logd("@@ I2S out:%#X, in:%#X, pcmbuf:%#X", hd->out, hd->in, hd->pcmbuf);
        if (!hd->out || !hd->in || !hd->pcmbuf) {
            dongle_mic_i2s_afc_close(hd);
            return NULL;
        }
    }

    return hd;
}

/////////////////////////////////////////dsp mic i2s afc /////////////////////////////////
typedef struct {
    struct pcm *out;
    struct pcm *in;
    int32_t *pcmbuf;
    OAL_THREAD task_handle;           //线程句柄
} dsp_mic_i2s_afc_t;

void dsp_mic_i2s_afc_close(void *handle)
{
    dsp_mic_i2s_afc_t *hd = handle;

    if (hd) {
        if (hd->out) {
            pcm_close(hd->out);
        }
        if (hd->in) {
            pcm_close(hd->in);
        }
        if (hd->pcmbuf) {
            general_free(hd->pcmbuf);
        }

        general_free(hd);
    }
}

void *dsp_mic_i2s_afc_loop(void *handle)
{
    dsp_mic_i2s_afc_t *hd = handle;

    while (1) {
        pcm_read(hd->in, hd->pcmbuf, G_AUDIO_PROC_PERIOD);

        //TODO: afc proc


        pcm_write(hd->out, hd->pcmbuf, G_AUDIO_PROC_PERIOD);
    }

    return NULL;
}



void *dsp_mic_i2s_afc_open(void)
{
    dsp_mic_i2s_afc_t *hd;
    struct pcm_config out_config;
    struct pcm_config in_config;

    hd = general_calloc(sizeof(*hd));
    if (hd) {
        out_config.ch = I2S_CH;
        out_config.sample_size = sizeof(int32_t);
        out_config.rate = 48000;
        out_config.period_size = G_AUDIO_PROC_PERIOD;
        out_config.period_count = 2;
        out_config.start_threshold = 0;
        out_config.stop_threshold = 0;
        hd->out = pcm_open(SND_I2S, PCM_OUT, &out_config);

        in_config.ch = I2S_CH;
        in_config.sample_size = sizeof(int32_t);
        in_config.rate = 48000;
        in_config.period_size = G_AUDIO_PROC_PERIOD;
        in_config.period_count = 2;
        in_config.start_threshold = 0;
        in_config.stop_threshold = 0;
        hd->in = pcm_open(SND_I2S, PCM_IN, &in_config);

        hd->pcmbuf = general_calloc(G_AUDIO_PROC_PERIOD*I2S_CH*sizeof(int32_t));

        logd("@@ I2S out:%#X, in:%#X, pcmbuf:%#X", hd->out, hd->in, hd->pcmbuf);
        if (!hd->out || !hd->in || !hd->pcmbuf) {
            dsp_mic_i2s_afc_close(hd);
            return NULL;
        }

        hd->task_handle = OAL_thread_create(dsp_mic_i2s_afc_loop, hd, "afc", 9, 256);
    }

    return hd;
}
