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
#include "audio_sink_common.h"
#include "sltk_math.h"
#include "id_worker.h"
#include "sltk_interp.h"

#define MIN(a,b)    ((a) < (b) ? (a) : (b))

extern float get_amp(int32_t vol);

/*
#define USE_NEW_INTERP

#ifdef USE_NEW_INTERP
#define INTERP_TH 24
#else
#define INTERP_TH 12
#endif*/

void audio_sink_init(audio_sink_common_t *sink,
    const audio_sink_init_params_t *params)
{
    if (NULL == sink) {
        return;
    }
    memset(sink, 0, sizeof(*sink));

    sink->fs = params->fs;
    sink->sample_size = params->sample_size;
    sink->ch = params->ch;
    sink->input_ch = params->input_ch;
    sink->input_sample_size = params->input_sample_size;

    if (sink->sample_size == sink->input_sample_size &&
        sink->ch == sink->input_ch) {
        sink->reformat = NULL;
    } else if ((sink->sample_size == 2) && (sink->input_sample_size == 2) &&
               (sink->ch == 1) && (sink->input_ch == 2)) {
        sink->reformat = (reformat_func_t)stereo16_to_mono16;
    } else {
        loge("audio sink params err");
    }

    ring_buffer_init(&sink->bufctrl, params->buf, params->buf_bytes);
    sink->active_th = params->trigger_th;
    sink->trigger_th = params->trigger_th;
    sink->period_bytes = params->period_bytes;
    sink->sink_update_t_th = params->sink_update_t_th;

    sink->use_new_interp_flag = params->use_new_interp_flag;
    if (sink->use_new_interp_flag) {
        sink->new_interp_th = 24;
        sltk_interp_init interp_init;
        interp_init.ch = sink->ch;
        interp_init.sample_size = sink->sample_size;
        interp_init.period = AUDIO_SINK_PROC_PERIOD;
        if (1 == sink->use_new_interp_flag) {
            sink->asrc = sltk_interp_open(&interp_init);
        } else {
            sink->asrc = sltk_interp_open2(&interp_init);
        }
    } else {
        sink->new_interp_th = 12;
        id_init idi;
        idi.ch = sink->ch;
        idi.period = AUDIO_SINK_PROC_PERIOD * 2; //参数给足够大避免一次调用处理不完input
        idi.sample_size = sink->sample_size;
        idi.level = SRATE_QUALITY_CUBIC_LVL_1;
        idi.alpha = (float)1.0;
        sink->asrc = id_worker_init(&idi);
    }
}

AT_SRAM(.audio_sink_reset_delta)
void audio_sink_reset_delta(audio_sink_common_t *sink)
{
    uint64_t sink_t;

    sink->total_interp_samples = 0;
    sink_t = get_aout_play_samples();
    //sink_t = (sink_t * (int32_t)1e6) / 48000; //samples to us
    sink_t = (sink_t * 125) / 6; //samples to us
    sink->base_delta_us = sink_t - (sink->sink_samples * (int32_t)1e6) / sink->fs;
    sink->last_interp_timesamp = sink_t;
}

AT_SRAM(.audio_sink_update_timestamp)
void audio_sink_update_timestamp(audio_sink_common_t *sink, uint32_t samples)
{
    sink->sink_samples += samples;
    sink->sink_update_t = HAL_timestamp_to_us(HAL_get_timestamp());
}

AT_SRAM(.audio_sink_write_buf)
static void audio_sink_write_buf(audio_sink_common_t *sink,
    void *buf, uint32_t input_bytes)
{
    uint32_t avail;
    uint32_t w_bytes;
    uint32_t input_w_bytes;
    uint32_t w_samples;
    uint8_t *wptr;
    uint32_t samples = input_bytes / (sink->input_ch * sink->input_sample_size);
    uint32_t bytes = samples * sink->ch * sink->sample_size;

    while (bytes) {//向环形缓冲区写数据
        ring_buffer_get_w_block(&sink->bufctrl, &avail, &wptr);
        if (avail > 0) {
            w_bytes = MIN(bytes, avail);
            w_samples = w_bytes / (sink->ch * sink->sample_size);
            input_w_bytes = w_samples * sink->input_ch * sink->input_sample_size;
            if (sink->reformat) {
                sink->reformat(wptr, buf, w_samples);
            } else {
                memcpy(wptr, buf, w_bytes);
            }
            bytes -= w_bytes;
            buf = (uint8_t*)buf + input_w_bytes;
            ring_buffer_update_wpos(&sink->bufctrl, w_bytes);
        } else {
            break;
        }
    }
}

AT_SRAM(.audio_sink_update_interp_info)
void audio_sink_update_interp_info(audio_sink_common_t *sink)
{
    int64_t play_t, sink_t, interp_t;
    int32_t delta_us;
    int32_t delta;

try_read:
    play_t = get_aout_play_samples();
    //play_t = (play_t * (int32_t)1e6) / 48000; //samples to us
    play_t = (play_t * 125) / 6; //samples to us
    int64_t sink_samples = sink->sink_samples;
    uint32_t update_t = sink->sink_update_t;

    int64_t sink_samples2 = sink->sink_samples;
    uint32_t update_t2 = sink->sink_update_t;
    if (sink_samples != sink_samples2 ||
        update_t != update_t2) {
        goto try_read;
    }

    sink_t = (sink_samples * (int32_t)1e6) / sink->fs;
    sink_t += HAL_timestamp_to_us(HAL_get_timestamp()) - update_t;
    interp_t = (sink->total_interp_samples * (int32_t)1e6) / sink->fs;

    //获得sink的delta值
    delta_us = play_t + interp_t - sink_t - sink->base_delta_us;
    delta = (delta_us * (int32_t)sink->fs) / (int32_t)1e6;
    if (delta > sink->new_interp_th) {
        sink->interp_deltaN = -1;
        sink->last_interp_timesamp = sink_t;
    } else if (delta < -sink->new_interp_th) {
        sink->interp_deltaN = 1;
        sink->last_interp_timesamp = sink_t;
    } else {
        //sink->interp_deltaN = 0;
        if (delta > 0 && sink->interp_deltaN < 0) {
        } else if (delta < 0 && sink->interp_deltaN > 0) {
        } else {
            sink->interp_deltaN = 0;
        }
    }

#if 0
    {
        static int count;
        if (count++% 200 == 0) {
            logx("{delta:%d}", delta);
        }
    }
#endif
}

AT_SRAM(.audio_sink_interp_proc)
void audio_sink_interp_proc(audio_sink_common_t *sink,
    void *interpbuf, void *inbuf, uint32_t inbytes)
{

    void *in = inbuf;
    void *out =  interpbuf;
    int32_t insamples = AUDIO_SINK_PROC_PERIOD;
    int32_t outsamples = 0;
    float alpha = 1.0f;
    if (sink->interp_deltaN > 0) {
        alpha = 1.0f - 1.f/1024;
    } else if (sink->interp_deltaN < 0) {
        alpha = 1.0f + 1.f/1024;
    } else {
        alpha = 1.0f;
    }
    if (sink->use_new_interp_flag) {
        if (1 == sink->use_new_interp_flag) {
            sltk_interp_proc(sink->asrc, &in, &insamples, &out, &outsamples, alpha);
        } else {
            sltk_interp_proc2(sink->asrc, &in, &insamples, &out, &outsamples, alpha);
        }
    } else {
        id_worker_interp_get_period(sink->asrc, &in, &insamples, &out, &outsamples, alpha);
    }

    out = interpbuf;
    if (outsamples > AUDIO_SINK_PROC_PERIOD) {
        sink->total_interp_samples += outsamples - AUDIO_SINK_PROC_PERIOD;
       //logx("{+}");
    } else if (outsamples < AUDIO_SINK_PROC_PERIOD) {
        sink->total_interp_samples += outsamples - AUDIO_SINK_PROC_PERIOD;
        //logx("{-}");
    }
    audio_sink_write_buf(sink, out, outsamples*2*sizeof(int16_t));

}

AT_SRAM(.audio_sink_write)
void audio_sink_write(audio_sink_common_t *sink,
    void *interpbuf, void *buf, uint32_t input_samples)
{
    uint32_t avail;
    uint32_t bytes;
    uint32_t input_bytes;

    if (NULL == sink) {
        return;
    }

    bytes = input_samples * sink->ch * sink->sample_size;
    input_bytes = input_samples * sink->input_ch * sink->input_sample_size;

{
    uint32_t update_t;
    uint32_t update2_t;
read_update:
    update_t = sink->sink_update_t;
    update2_t = sink->sink_update_t;
    if (update_t != update2_t) {
        goto read_update;
    }
    uint32_t delat_t = HAL_timestamp_to_us(HAL_get_timestamp()) - update_t;
    if (delat_t > sink->sink_update_t_th) {
        sink->status = AUDIO_SINK_STA_IDLE;
        return;
    }
}

    avail = ring_buffer_get_w_avail(&sink->bufctrl);
    if (avail < bytes+4) {
        logd("O247");
        sink->status = AUDIO_SINK_STA_IDLE;
    } else {
        sink->active_th = sink->period_bytes+4;
        //更新sink插补信息并处理
        audio_sink_update_interp_info(sink);
        audio_sink_interp_proc(sink, interpbuf, buf, input_bytes);
        //audio_sink_write_buf(sink, buf, input_bytes);
    }

    /* update status */
    avail = ring_buffer_get_r_avail(&sink->bufctrl);
    if (avail >= sink->active_th) {
        sink->active_th = sink->period_bytes+4;
        sink->status = AUDIO_SINK_STA_ACTIVE;
    } else {
        sink->active_th = sink->trigger_th;
        sink->status = AUDIO_SINK_STA_IDLE;
        logd("U");
    }
}

AT_SRAM(.audio_sink_read)
int audio_sink_read(audio_sink_common_t *sink, void *buf, uint32_t samples)
{
    uint32_t avail;
    uint8_t *rptr;
    uint32_t r_bytes;
    uint32_t bytes;

    if (NULL == sink) {
        return -1;
    }

    if (sink->ch == 0 || sink->sample_size == 0) {
        return -1;
    }

    bytes = samples * sink->ch * sink->sample_size;

    //更新sink时间并重设sink的delta值
    audio_sink_update_timestamp(sink, samples);
    if (AUDIO_SINK_STA_IDLE == sink->status) {
        audio_sink_reset_delta(sink);
    }

    avail = ring_buffer_get_r_avail(&sink->bufctrl);

    //logd("avail:%d, b:%d,  status:%d", avail, bytes, sink->status);
    if (avail >= bytes && (AUDIO_SINK_STA_ACTIVE == sink->status)) {
        while (bytes) {//从环形缓存区读取数据
            ring_buffer_get_r_block(&sink->bufctrl, &avail, &rptr);
            r_bytes = MIN(avail, bytes);
            memcpy(buf, rptr, r_bytes);
            bytes -= r_bytes;
            buf = (uint8_t*)buf + r_bytes;
            ring_buffer_update_rpos(&sink->bufctrl, r_bytes);
        }
    } else {
        memset(buf, 0, bytes);
    }

    return 0;
}
