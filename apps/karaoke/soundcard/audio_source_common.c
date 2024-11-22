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
#include "audio_source_common.h"
#include "sltk_math.h"
#include "id_worker.h"
#include "sltk_interp.h"

#define USE_NEW_INTERP

/*
#ifdef USE_NEW_INTERP
#define INTERP_TH 24
#else
#define INTERP_TH 12
#endif
*/
#define AUDIO_SOURCE_INTERP_OPT    1

#define MIN(a,b)    ((a) < (b) ? (a) : (b))

void audio_source_init(audio_source_common_t *src,
    const audio_source_init_params_t *params)
{
    if (NULL == src) {
        return;
    }

    memset(src, 0, sizeof(*src));

    src->fs = params->fs;
    src->sample_size = params->sample_size;
    src->ch = params->ch;

    ring_buffer_init(&src->bufctrl, params->buf, params->buf_bytes);
    src->active_th = params->trigger_th;
    src->trigger_th = params->trigger_th;
    src->period_bytes = params->period_bytes;
    src->type = params->type;

    src->use_new_interp_flag = params->use_new_interp_flag;
    src->interp_th = (src->use_new_interp_flag == 1) ? 24 : 12;
    if (AUDIO_SOURCE_TYPE_INTERP == src->type) {
#if AUDIO_SOURCE_INTERP_OPT
        if (src->use_new_interp_flag) {
            sltk_interp_init interp_init;
            interp_init.ch = src->ch;
            interp_init.sample_size = src->sample_size;
            interp_init.period = AUDIO_SOURCE_PROC_PERIOD;
            if (1 == src->use_new_interp_flag) {
                src->asrc = sltk_interp_open(&interp_init);
            } else {
                src->asrc = sltk_interp_open2(&interp_init);
            }
            src->asrc_inbuf = general_malloc(AUDIO_SOURCE_PROC_PERIOD * src->ch * src->sample_size);
            src->asrc_outbuf = general_malloc((AUDIO_SOURCE_PROC_PERIOD + MAX_INTERP_CNT) * src->ch * src->sample_size);
            src->asrc_out_res = 0;
        } else {
            id_init idi;
            idi.ch = src->ch;
            idi.period = AUDIO_SOURCE_PROC_PERIOD * 2; //参数给足够大避免一次调用处理不完input
            idi.sample_size = src->sample_size;
            idi.level = SRATE_QUALITY_CUBIC_LVL_1;
            idi.alpha = (float)1.0;
            src->asrc = id_worker_init(&idi);
            src->asrc_inbuf = general_malloc(AUDIO_SOURCE_PROC_PERIOD * src->ch * src->sample_size);
        }
#endif
    }
}

AT_SRAM(.audio_src)
void audio_source_reset_delta(audio_source_common_t *src)
{
    uint64_t sink_t;

    src->total_interp_samples = 0;
    sink_t = get_aout_play_samples();
    sink_t = (sink_t * (int32_t)1e6) / 48000; //samples to us
    src->base_delta_us = sink_t - (src->source_samples * (int32_t)1e6) / src->fs;
    src->last_interp_timesamp = sink_t;
}

AT_SRAM(.audio_src)
void audio_source_update_timestamp(audio_source_common_t *src, uint32_t samples)
{
    src->source_samples += samples;
    src->source_update_t = HAL_timestamp_to_us(HAL_get_timestamp());
}

AT_SRAM(.audio_src)
void audio_source_write(audio_source_common_t *src, void *buf, uint32_t samples)
{
    uint32_t avail;
    uint8_t *wptr;
    uint32_t w_bytes;
    uint32_t bytes;

    if (NULL == src) {
        return;
    }

    if (0 == src->sample_size || 0 == src->ch) {
        return;
    }

    bytes = samples * src->sample_size * src->ch;

    if (AUDIO_SOURCE_TYPE_INTERP ==  src->type) {
        audio_source_update_timestamp(src, samples);
        if (src->status == AUDIO_SOURCE_STA_IDLE) {
            audio_source_reset_delta(src);
        }
    }

    while (bytes) {
        ring_buffer_get_w_block(&src->bufctrl, &avail, &wptr);
        if (avail > 0) {
            w_bytes = MIN(bytes, avail);
            memcpy(wptr, buf, w_bytes);
            bytes -= w_bytes;
            buf = (uint8_t*)buf + w_bytes;

            ring_buffer_update_wpos(&src->bufctrl, w_bytes);
        } else {
            //logd("or");
            src->overrun_flag = 1;
            break;
        }
    }
}

AT_SRAM(.audio_src)
void audio_source_read_buf(audio_source_common_t *src, void *buf, uint32_t bytes)
{
    uint32_t avail;
    uint8_t *rptr;
    uint32_t r_bytes;

    while (bytes) {
        ring_buffer_get_r_block(&src->bufctrl, &avail, &rptr);
        r_bytes = MIN(avail, bytes);
        memcpy(buf, rptr, r_bytes);
        bytes -= r_bytes;
        buf = (uint8_t*)buf + r_bytes;
        ring_buffer_update_rpos(&src->bufctrl, r_bytes);
    }
}

AT_SRAM(.audio_src)
void audio_source_update_interp_info(audio_source_common_t *src)
{
    int64_t sink_t, source_t, interp_t;
    int32_t delta_us;
    int32_t delta;
try_read:
    sink_t = get_aout_play_samples();
    sink_t = (sink_t * (int32_t)1e6) / 48000; //samples to us
    int64_t source_samples = src->source_samples;
    uint32_t update_t = src->source_update_t;

    int64_t source_samples2 = src->source_samples;
    uint32_t update_t2 = src->source_update_t;
    if (source_samples != source_samples2 ||
        update_t != update_t2) {
        goto try_read;
    }

    source_t = (source_samples * (int32_t)1e6) / src->fs;
    source_t += HAL_timestamp_to_us(HAL_get_timestamp()) - update_t;
    interp_t = (src->total_interp_samples * (int32_t)1e6) /  src->fs;
    delta_us = sink_t - source_t - src->base_delta_us - interp_t;
    delta = (delta_us * (int32_t)src->fs) / (int32_t)1e6;
    //更新插补delta值和上次插补时间
    if (delta > src->interp_th) {
        src->interp_deltaN = 1;
        src->last_interp_timesamp = sink_t;
    } else if (delta < -src->interp_th) {
        src->interp_deltaN = -1;
        src->last_interp_timesamp = sink_t;
    } else {
        if (delta > 0 && src->interp_deltaN > 0) {
        } else if (delta < 0 && src->interp_deltaN < 0) {
        } else {
            src->interp_deltaN = 0;
        }
    }
}

AT_SRAM(.audio_src)
void audio_source_interp_read(audio_source_common_t *src, void *buf, uint32_t bytes)
{
#if AUDIO_SOURCE_INTERP_OPT

    if (src->use_new_interp_flag) {
        void *in;
        void *out;
        int32_t insamples;
        int32_t outsamples = 0;
        int32_t total_interp = 0;
        int32_t period_samples = bytes / (src->ch * src->sample_size);
        float alpha = 1.0f;
        if (src->interp_deltaN > 0) {
            alpha = 1.0f - 1.f/1024;
        } else if (src->interp_deltaN < 0) {
            alpha = 1.0f + 1.f/1024;
        } else {
            alpha = 1.0f;
        }

        if (period_samples != AUDIO_SOURCE_PROC_PERIOD) {
            //fuck you
            return;
        }

        while (src->asrc_out_res < period_samples) {
            insamples = period_samples - src->asrc_out_res;
            in = src->asrc_inbuf;
            int tmpinsamples = insamples;

            audio_source_read_buf(src, src->asrc_inbuf, insamples * src->ch * src->sample_size);

            outsamples = 0;
            out = (uint8_t*)src->asrc_outbuf + src->asrc_out_res * src->ch * src->sample_size;
            if (1 == src->use_new_interp_flag) {
                sltk_interp_proc(src->asrc, &in, &insamples, &out, &outsamples, alpha);
            } else {
                sltk_interp_proc2(src->asrc, &in, &insamples, &out, &outsamples, alpha);
            }
            //logx("[%d,%d]", tmpinsamples, outsamples);
            if (outsamples != tmpinsamples) {
                total_interp += outsamples - tmpinsamples;
            }
            src->asrc_out_res += outsamples;
        }

        memcpy(buf, src->asrc_outbuf, bytes);
        src->asrc_out_res -= period_samples;
        memcpy(src->asrc_outbuf, (uint8_t*)src->asrc_outbuf + bytes,
            src->asrc_out_res * src->ch * src->sample_size);

        if (total_interp > 0) {
            src->total_interp_samples += total_interp;
            //logx("{R+}");
        } else if (total_interp < 0) {
            src->total_interp_samples += total_interp;
            //logx("{R-}");
        }
    }else {
        void *in;
        void *out;
        int32_t insamples;
        int32_t outsamples = 0;
        int32_t total_insamples = 0, total_outsamples = 0;
        int32_t period_samples = bytes / (src->ch * src->sample_size);
        float alpha = 1.0f;
        if (src->interp_deltaN > 0) {
            alpha = 1.0f - 1.f/1024;
        } else if (src->interp_deltaN < 0) {
            alpha = 1.0f + 1.f/1024;
        } else {
            alpha = 1.0f;
        }

        if (period_samples != AUDIO_SOURCE_PROC_PERIOD) {
            //fuck you
            return;
        }

        audio_source_read_buf(src, src->asrc_inbuf, bytes - src->ch * src->sample_size);
        in = src->asrc_inbuf;
        out = buf;
        insamples = period_samples - 1;
        total_insamples += insamples;
        outsamples = 0;
        id_worker_interp_get_period(src->asrc, &in, &insamples, &out, &outsamples, alpha);
        buf = (uint8_t*)buf + outsamples * src->ch * src->sample_size;
        total_outsamples += outsamples;

        while (total_outsamples < period_samples) {
            audio_source_read_buf(src, src->asrc_inbuf, src->ch * src->sample_size);
            in = src->asrc_inbuf;
            out = buf;
            insamples = 1;
            total_insamples += insamples;
            outsamples = 0;
            id_worker_interp_get_period(src->asrc, &in, &insamples, &out, &outsamples, alpha);
            buf = (uint8_t*)buf + outsamples * src->ch * src->sample_size;
            total_outsamples += outsamples;
        }

        if (total_outsamples > total_insamples) {
            src->total_interp_samples += total_outsamples - total_insamples;
            //logx("{+}");
        } else if (total_outsamples < total_insamples) {
            src->total_interp_samples += total_outsamples - total_insamples;
            //logx("{-}");
        }
    }

#else
    int16_t *p16 = (int16_t*)buf;
    int32_t total_cnt = (((1024 + AUDIO_SOURCE_PROC_PERIOD - 1)/AUDIO_SOURCE_PROC_PERIOD) * AUDIO_SOURCE_PROC_PERIOD);
    float adjust_speed = (float)1 / total_cnt;

    int16_t h[2];
    int16_t tmp_h[2];

    if ((src->interp_deltaN > 0 || src->n_cnt) && !src->sub_n_cnt) {
        h[0] = src->hist[0];
        h[1] = src->hist[1];
        if (src->hist_res) {//上个total_cnt个点是增加点且当前为首个AUDIO_SOURCE_PROC_PERIOD
            p16[0] = src->hist[0];
            p16[1] = src->hist[1];
            audio_source_read_buf(src, p16+2, bytes - src->sample_size * src->ch);
            src->hist_res = 0;
        } else {
            audio_source_read_buf(src, p16, bytes);
        }

        //将所有的点往前移，
        //原total_cnt最后一组AUDIO_SOURCE_PROC_PERIOD的p16[2*n+1]位置上存放p16[2*(n-1)+1]的值，
        //多出一个原total_cnt最后一个点的值,samples+1,达到增加数据的效果
        for (int n = 0; n < AUDIO_SOURCE_PROC_PERIOD; n++) {
            src->n_cnt++;
            float s = src->n_cnt*adjust_speed;

            tmp_h[0] = p16[2*n];
            tmp_h[1] = p16[2*n+1];
            #if 0
            p16[2*n] = h[0] * s + p16[2*n] * (1.0f - s);
            p16[2*n+1] = h[1] * s + p16[2*n+1] * (1.0f - s);
            #else
            float a, b;
            a = h[0] << 16;
            b = p16[2*n] << 16;
            p16[2*n] = ((int32_t)(a * s + b * (1.0f - s))) >> 16;
            a = h[1] << 16;
            b = p16[2*n+1] << 16;
            p16[2*n+1] = ((int32_t)(a * s + b * (1.0f - s))) >> 16;
            #endif
    
            h[0] = tmp_h[0];
            h[1] = tmp_h[1];
        }
        //记录每个AUDIO_SOURCE_PROC_PERIOD最后的值，以便下次使用
        src->hist[0] = h[0];
        src->hist[1] = h[1];

        if (src->n_cnt >= total_cnt) {
             src->n_cnt = 0;
             src->hist_res = 1;
             src->total_interp_samples += 1;
        }
        //logx("+");
    } else if (src->interp_deltaN < 0 || src->sub_n_cnt) {
        if (0 == src->sub_n_cnt) {//减少数据中total_cnt里的的第一组AUDIO_SOURCE_PROC_PERIOD
            if (src->hist_res) {//上次为增加数据
                //获取上次增加数据记录最后的数据
                src->sub_hist[0] = src->hist[0];
                src->sub_hist[1] = src->hist[1];
                src->hist_res = 0;
            } else {
                audio_source_read_buf(src, src->sub_hist, src->sample_size * src->ch);
            }
            src->total_interp_samples += -1;
        }
        #if 0
        if (src->hist_res) {
            p16[0] = src->hist[0];
            p16[1] = src->hist[1];
            audio_source_read_buf(src, p16+2, bytes - src->sample_size * src->ch);
            src->hist_res = 0;
        } else {
            audio_source_read_buf(src, p16, bytes);
        }
        #endif
        audio_source_read_buf(src, p16, bytes);

        h[0] = src->sub_hist[0];
        h[1] = src->sub_hist[1];
        //所有的点往前移，total_cnt的最后一个点保持原值，将samples-1,从而达到减少数据的效果
        for (int n = 0; n < AUDIO_SOURCE_PROC_PERIOD; n++) {
            src->sub_n_cnt++;
            float s = src->sub_n_cnt*adjust_speed;
            tmp_h[0] = p16[2*n];
            tmp_h[1] = p16[2*n+1];
            #if 0
            p16[2*n] = h0 * (1.0f - s) + p16[2*n] * s;
            p16[2*n+1] = h1 * (1.0f - s) + p16[2*n+1] * s;
            #else
            float a, b;
            a = h[0] << 16;
            b = p16[2*n] << 16;
            p16[2*n] = ((int32_t)(a * (1.0f - s) + b * s)) >> 16;
            a = h[1] << 16;
            b = p16[2*n+1] << 16;
            p16[2*n+1] = ((int32_t)(a * (1.0f - s) + b * s)) >> 16;
            #endif
            h[0] = tmp_h[0];
            h[1] = tmp_h[1];
        }
        //记录每个AUDIO_SOURCE_PROC_PERIOD最后的数据，以便下次使用
        src->sub_hist[0] = h[0];
        src->sub_hist[1] = h[1];
        if (src->sub_n_cnt >= total_cnt) {
             src->sub_n_cnt = 0;
             //logx("source");
        }
        //记录每组最后值，以备下次使用
        src->hist[0] = p16[2*(AUDIO_SOURCE_PROC_PERIOD - 1)];
        src->hist[1] = p16[2*(AUDIO_SOURCE_PROC_PERIOD - 1)+1];
        //logx("-");
    } else {
        if (src->hist_res) {
            p16[0] = src->hist[0];
            p16[1] = src->hist[1];
            audio_source_read_buf(src, p16+2, bytes - src->sample_size * src->ch);
            src->hist_res = 0;
        } else {
            audio_source_read_buf(src, p16, bytes);
        }
        //更新加数据的数据记录
        src->hist[0] = p16[2*(AUDIO_SOURCE_PROC_PERIOD - 1)];
        src->hist[1] = p16[2*(AUDIO_SOURCE_PROC_PERIOD - 1)+1];
    }
#endif
}

AT_SRAM(.audio_src)
int audio_source_read(audio_source_common_t *src, void *buf, uint32_t samples)
{
    uint32_t avail;
    uint32_t bytes;

    bytes = samples * src->sample_size * src->ch;

    if (NULL == src) {
        memset(buf, 0, bytes);
        return -1;
    }

    if (src->overrun_flag) {
        src->overrun_flag = 0;
        avail = ring_buffer_get_r_avail(&src->bufctrl);
        if (avail > src->trigger_th) {
            ring_buffer_update_rpos(&src->bufctrl, avail - src->trigger_th);
        }
    }

    if (AUDIO_SOURCE_TYPE_INTERP == src->type) {
        #if 0
        uint32_t update_t;
        uint32_t update2_t;
        while (1) {
            avail = ring_buffer_get_r_avail(&src->bufctrl);
            do {
                update_t = src->source_update_t;
                update2_t = src->source_update_t;
            } while (update_t != update2_t);
            if (timestamp_to_us(get_timestamp()) - update_t < 1400 &&
                avail < bytes+4) {
                msleep(1);
            } else {
                break;
            }
        }
        #else
        avail = ring_buffer_get_r_avail(&src->bufctrl);
        #endif
    } else {
        avail = ring_buffer_get_r_avail(&src->bufctrl);
    }

    if (avail < bytes+4 || avail < src->active_th) {
        src->active_th = src->trigger_th;
    } else {
        src->status = AUDIO_SOURCE_STA_ACTIVE;
        if (src->use_new_interp_flag) {
            if (AUDIO_SOURCE_TYPE_INTERP == src->type) {
                src->active_th = src->period_bytes+ 16 * src->ch * src->sample_size;
            } else {
                src->active_th = src->period_bytes+4;
            }
        } else {
            src->active_th = src->period_bytes+4;
        }

        if (AUDIO_SOURCE_TYPE_INTERP == src->type) {
            audio_source_update_interp_info(src);
            audio_source_interp_read(src, buf, bytes);
        } else {
            audio_source_read_buf(src, buf, bytes);
        }
        return 0;
    }

    src->status = AUDIO_SOURCE_STA_IDLE;
    //logd("ur");
    memset(buf, 0, bytes);
    return -1;
}
