
/*****************************************************************************
* audio_track服务端相关实现
****************************************************************************/
#define LOG_LEV 4
#define SLTK_LOG_TAG "at"
#include "sys_inc.h"
#include "stream_io.h"
#include "sltk_codecs_common.h"
#include "audio_out.h"
#include "sltk_mem.h"
#include "ring_buffer.h"
#include "middleware.h"
#include "audio_track.h"
#include "volume_control.h"
#include "waudio_track.h"
#include "amixer.h"
#include "audiosrate_conventor.h"
#include "amixer_funcs.h"
#include "fade.h"
#define MIN(a, b) ((a) > (b) ? (b) : (a))

typedef struct {
    struct audio_share_buffer sb;
    audio_track_init_t        info;
    audio_fade_t fade;              /* fade process context */
    volume_control_t vol_ctrl;      /* software volume process context */

    void*    afxc_handle;
    void*    asrc_handle;

    int64_t base_stamp;
    uint32_t playtime;
    uint32_t remain_samples;
    uint32_t out_fs;
    uint32_t wr_sem_wait_tick;

    uint32_t mute_samples;

    uint32_t fadein_sys_t;
    uint32_t fadeout_sys_t;
    uint16_t fadein_ms;
    uint16_t fadeout_ms;

    uint8_t id;
    uint8_t wait_set_base_stamp;
    uint8_t playtime_set;
    uint8_t  out_ch;
    uint8_t  out_samples_size;
    uint8_t  track_stu;
    uint8_t track_active;
    uint8_t data_sta;
    uint8_t  out_proc_period;
    uint8_t last_con_sta;
    uint8_t wait_fadeout : 1;
    uint8_t wait_fadein : 1;
} WaudioTrack;

typedef enum {
    WAUDIOTRACK_STU_STOP = 0,
    WAUDIOTRACK_STU_STOPPING,
    WAUDIOTRACK_STU_PLAYING,
}wat_track_stu;

void _notifyFormWAudioTrack(WaudioTrack* wat, uint32_t cmd, uint32_t val)
{
    if(wat->info.notify) {
        uint32_t __who = AUDIO_NOTIFY_ATRACK;
        uint32_t __cmd = 0;
        uint32_t __req = 0;
        uint32_t __rev = 0;
        //You can judge what's the notify is, then send a valid val
        switch(cmd) {
            case AMIXER_MSG_NOT_READY:
                __cmd = AUDIOTRACK_MSG_AMIXER_NOT_READY;
                break;
            case AMIXER_MSG_RE_READY:
                __cmd = AUDIOTRACK_MSG_AMIXER_RE_READY;
                break;
        }
        wat->info.notify(wat->info.user_data, __who, __cmd, __req, (uint32_t)&__rev);
    }
}

AT_SRAM(.audiotrack)
int controlWAudioTrack(void* handle, uint32_t cmd, uint32_t val)
{
    WaudioTrack* wat = (WaudioTrack *)handle;
    int ret = 0;
    if(wat) {
        switch(cmd) {
            case WAUDIOTRACK_CMD_BASESTAMP_GET:
            {
                if ((WAUDIOTRACK_STU_PLAYING == wat->track_stu) &&
                    !wat->wait_set_base_stamp) {
                    *(int64_t*)val = wat->base_stamp;
                } else {
                    *(int64_t*)val = 0;
                    ret = -1;
                }
                break;
            }
            case WAUDIOTRACK_CMD_GET_STU:
            {
                *((uint32_t*)val) = wat->data_sta;
                break;
            }
            case WAUDIOTRACK_CMD_GIVE_A_SHIT:
            {
                OAL_sem_post(&wat->sb.write_sem_sync);
                break;
            }
            case WAUDIOTRACK_CMD_START:
            {
                wat->vol_ctrl.amp = wat->vol_ctrl.target_volume;
                wat->track_stu = WAUDIOTRACK_STU_PLAYING;
                break;
            }
            case WAUDIOTRACK_CMD_STOP:
            {
                wat->track_stu = WAUDIOTRACK_STU_STOP;
                break;
            }
            case WAUDIOTRACK_CMD_DRAINING:
            {
                wat->track_stu = WAUDIOTRACK_STU_STOPPING;
                break;
            }
            case WAUDIOTRACK_CMD_NOTIFY:
            {
                _notifyFormWAudioTrack(wat, val, 0);
                break;
            }
            case WAUDIOTRACK_CMD_PLAYTIME_SET:
            {
                wat->playtime = val;
                wat->playtime_set = true;
                break;
            }

            case WAUDIOTRACK_CMD_PLAYTIME_FLAG_GET:
            {
                *((uint32_t*)val) = wat->playtime_set;
                break;
            }

            case WAUDIOTRACK_CMD_FADE_BOOK:
            {
                atrack_fade_book_t *info = (atrack_fade_book_t *)val;
                if (FADE_OUT == info->fade_type) {
                    wat->fadeout_sys_t = info->book_fade_sys_t;
                    wat->fadeout_ms = info->fade_ms;
                    wat->wait_fadeout = 1;
                    wat->wait_fadein = 0;
                } else {
                    wat->fadein_sys_t = info->book_fade_sys_t;
                    wat->fadein_ms = info->fade_ms;
                    wat->wait_fadein = 1;
                    wat->wait_fadeout = 0;
                }
                break;
            }
            case WAUDIOTRACK_CMD_VOLUME_SET:
            {
                if (AUDIO_MODE_A2DP_MUSIC == wat->info.mode) {
                    amixer_xet_ctrl(AMIXER_CMD_EFFECTS_VOL_SET, val, 0);
                } else {
                    volume_control_set_volume(&wat->vol_ctrl, *((float*)val));
                }
                break;
            }

            case WAUDIOTRACK_CMD_MUTETIME_SET:
            {
                logd("atrack set mute_t:%d", val);
                wat->mute_samples = wat->out_fs * val / 1000;
                break;
            }
        }
    }
    return ret;
}

AT_SRAM(.audiotrack)
int WAudioTrackPullData(void* handle, void* dst, uint32_t samples)
{
    WaudioTrack* wat = (WaudioTrack *)handle;
    uint32_t avail;
    void* buf = dst;

    if (wat->info.pulldata_from_user) {
        if(wat->info.notify) {
            uint32_t prm[2];
            prm[0] = (uint32_t)dst;
            prm[1] = samples;
            wat->info.notify(wat->info.user_data, AUDIO_NOTIFY_ATRACK,
                AUDIOTRACK_MSG_WATRACK_PULLDATA_REQ, (uint32_t)prm, 0);
            if (1 == wat->info.ch) {
                if (sizeof(int32_t) == wat->info.sample_size) {
                    mono32_to_stereo32(dst, dst, samples);
                } else {
                    mono16_to_stereo32(dst, dst, samples);
                }
            } else if (2 == wat->info.ch) {
                if (sizeof(int32_t) == wat->info.sample_size) {
                    //bypass
                } else {
                    stereo16_to_stereo32(dst, dst, samples);
                }
            }
        }
    } else {
        while(samples) {
            avail = audio_share_buffer_avail_read_samples(&wat->sb);
            if(avail >= samples) {
                audio_share_buffer_read(&wat->sb,
                    &buf, &samples, wat->out_ch, wat->out_samples_size);
                OAL_sem_post(&wat->sb.write_sem_sync);
            } else {
                if (OAL_SUCCESS != OAL_sem_wait_ticks_timeout(
                    &wat->sb.read_sem_sync, wat->wr_sem_wait_tick)) {
                    return 0;
                }
            }
        }
    }

    return 1;
}

AT_SRAM(.audiotrack)
void WAudioTrackPostProc(void* handle, void* buf, uint32_t samples)
{
    WaudioTrack* wat = (WaudioTrack *)handle;

    if(wat && wat->info.notify) {
        uint32_t prm[2];
        prm[0] = (uint32_t)buf;
        prm[1] = samples;
        wat->info.notify(wat->info.user_data, AUDIO_NOTIFY_ATRACK,
            AUDIOTRACK_MSG_WATRACK_POSTPROC_REQ, (uint32_t)prm, 0);
    }
}

AT_SRAM(.audiotrack)
int debufferWAudioTrack(void* handle, void* dst, uint32_t samples)
{
    int ret = 0;
    WaudioTrack* wat = (WaudioTrack *)handle;

    if(wat && samples == wat->out_proc_period && wat->track_stu != WAUDIOTRACK_STU_STOP) {
        if (wat->playtime_set) {
            audio_stamp_info_t info;
            amixer_xet_ctrl(AMIXER_CMD_PLAYSTAMP_INFO_GET, wat->id, (uint32_t)&info);
            int64_t play_stamp = info.mix_stamp -
                (int64_t)(info.mix_stamp - info.play_stamp) +
                (int64_t)(((int32_t)(wat->playtime - info.sys_stamp)) *
                ((float)wat->out_fs / 1000000));
            int32_t delta = play_stamp - info.mix_stamp;

            #if 0
            __maybe_unused uint32_t print_play_stamp = play_stamp & 0xFFFF;
            __maybe_unused uint32_t print_stamp = info.mix_stamp & 0xFFFF;
            __maybe_unused uint32_t print_cur_sys_t = info.sys_stamp & 0xFFFF;
            __maybe_unused uint32_t print_cur_sys_samp =
                ((float)wat->out_fs / 1000000) * info.sys_stamp;
            print_cur_sys_samp &= 0xFFFF;
            __maybe_unused uint32_t print_cur_play_stamp = info.play_stamp & 0xFFFF;
            logx("AA:%d, %d, %d, %d, %d, delta:%d\n", print_cur_sys_t,
            print_cur_sys_samp, print_cur_play_stamp, print_play_stamp,
            print_stamp, delta);
            #endif

            if (wat->track_active && delta <= 0) {
                //TODO: 偏差小于一帧还需做shift以便精准对齐播放时间点
                int32_t th = (wat->out_fs * audio_share_buffer_avail_read_samples(&wat->sb) / wat->info.fs) >> 1;
                if (delta >= -th && delta < 0) {
                    int32_t skip = 0;
                    if (wat->out_fs) {
                        skip = -delta * wat->info.fs / wat->out_fs;
                    }
                    loge("|||||||||skip:%d", skip);
                    uint32_t skip_cnt = 0;
                    while (skip) {
                        uint32_t this_step_skip = MIN(samples, skip);
                        ret = WAudioTrackPullData(wat, dst, this_step_skip);
                        if (ret) {
                            skip_cnt += this_step_skip;
                            skip -= this_step_skip;
                        } else {
                            loge("skip fail");
                            break;
                        }
                    }
                    info.mix_stamp -= skip_cnt * wat->out_fs / wat->info.fs;
                } else {
                    logd("Wtrack delta:%d, skip fail", delta);
                }

                if (wat->wait_set_base_stamp) {
                    wat->base_stamp = info.mix_stamp;
                    wat->wait_set_base_stamp = 0;
                }

                if(wat->asrc_handle) {
                    ret = audiosrate_conventor_get_period(wat->asrc_handle, dst);
                } else {
                    ret = WAudioTrackPullData(wat, dst, samples);
                }
                if (!ret) {
                    //没拿到数据意味卡顿，多播放的点数需要加到base_stamp
                    wat->base_stamp += samples;
                    if (WAUDIO_TRACK_DATA_STA_NORMAL == wat->data_sta) {
                        wat->data_sta = WAUDIO_TRACK_DATA_STA_UNDERRUN;
                    }
                } else {
                    if (WAUDIO_TRACK_DATA_STA_UNDERRUN == wat->data_sta) {
                        logd("at underrun");
                    }
                    wat->data_sta = WAUDIO_TRACK_DATA_STA_NORMAL;
                }
                wat->playtime_set = 0;
            } else {
                if (0 == audio_share_buffer_avail_write_samples(&wat->sb)) {
                    wat->track_active = 1;
                }
            }
        } else {
            if (wat->track_active) {
                if(wat->asrc_handle) {
                    ret = audiosrate_conventor_get_period(wat->asrc_handle, dst);
                } else {
                    ret = WAudioTrackPullData(wat, dst, samples);
                }
                if (wat->wait_set_base_stamp) {
                    int64_t mix_stamp;
                    amixer_xet_ctrl(AMIXER_CMD_MIXSTAMP_GET, 0, (uint32_t)&mix_stamp);
                    wat->base_stamp = mix_stamp;
                    wat->wait_set_base_stamp = 0;
                }
                if (!ret) {
                    //没拿到数据意味卡顿，多播放的点数需要加到base_stamp
                    wat->base_stamp += samples;
                    if (WAUDIO_TRACK_DATA_STA_NORMAL == wat->data_sta) {
                        wat->data_sta = WAUDIO_TRACK_DATA_STA_UNDERRUN;
                    }
                } else {
                    if (WAUDIO_TRACK_DATA_STA_UNDERRUN == wat->data_sta) {
                        logd("at underrun");
                    }
                    wat->data_sta = WAUDIO_TRACK_DATA_STA_NORMAL;
                }
            } else {
                if (0 == audio_share_buffer_avail_write_samples(&wat->sb) ||
                    wat->info.pulldata_from_user) {
                    wat->track_active = 1;
                }
            }
        }
    }

    if(!ret) {
        if(wat->track_stu == WAUDIOTRACK_STU_STOPPING) {
            //此时，在STOPPING的CASE下，拿不到数据，说明已经sharebuffer已经耗尽
            if(wat->info.notify) {
                wat->info.notify(wat->info.user_data, AUDIO_NOTIFY_ATRACK, AUDIOTRACK_MSG_WATRACK_STOPPING_FINISHED, 0, 0);
            }
        }
        memset(dst, 0x00, samples * wat->out_ch * wat->out_samples_size);
    }
    else {
        if (wat->wait_fadeout) {
            int32_t delta = HAL_timestamp_to_us(HAL_get_timestamp()) - wat->fadeout_sys_t;
            if (delta >= 0) {
                wat->wait_fadeout = 0;
                fade_start(&wat->fade, FADE_OUT, wat->fadeout_ms);
            }
        } else if (wat->wait_fadein) {
            int32_t delta = HAL_timestamp_to_us(HAL_get_timestamp()) - wat->fadein_sys_t;
            if (delta >= 0) {
                wat->wait_fadein = 0;
                fade_start(&wat->fade, FADE_IN, wat->fadein_ms);
            }
        }
        if (wat->mute_samples) {
            memset(dst, 0x00, samples * wat->out_ch * wat->out_samples_size);
            if (wat->mute_samples > samples) {
                wat->mute_samples -= samples;
            } else {
                wat->mute_samples = 0;
                fade_set_status(&wat->fade, FADE_STA_RESET);
                fade_start(&wat->fade, FADE_IN, wat->fadein_ms);
            }
        } else {
            fade_proc(&wat->fade, dst, samples);
        }
        volume_control_proc(&wat->vol_ctrl, dst, samples);
    }

    return ret;
}

void  destroyWAudioTrack(void* handle)
{
    WaudioTrack* wat = (WaudioTrack *)handle;
    if(wat) {
        audio_share_buffer_deinit(&wat->sb);
        if(wat->sb.ctrl.base) {
            general_free(wat->sb.ctrl.base);
            wat->sb.ctrl.base = 0;
        }
        if(wat->asrc_handle) {
            destroy_audiosrate_conventor(wat->asrc_handle);
            wat->asrc_handle = NULL;
        }
        if(wat->afxc_handle) {
            //TODO
        }
        general_free(wat);
    }
}

void* createWAudioTrack(audio_track_init_t *init, int32_t id, struct audio_share_buffer** sb)
{
    void *buf;
    uint32_t bufsize;

    WaudioTrack* wat = general_calloc(sizeof(WaudioTrack));
    if(!wat) {
        return NULL;
    }

    wat->info = *init;
    wat->remain_samples = 0;
    wat->track_active = 0;
    wat->wait_set_base_stamp = 1;
    wat->id = id;

    logd("new audio track");

    amixer_xet_ctrl(AMIXER_CMD_GET_AMIXER_PCM_CHANNEL_CFG, 0, (uint32_t)&wat->out_ch);
    amixer_xet_ctrl(AMIXER_CMD_GET_AMIXER_PCM_BPS_CFG, 0, (uint32_t)&wat->out_samples_size);
    amixer_xet_ctrl(AMIXER_CMD_GET_AMIXER_PCM_PROC_PREIOD_CFG, 0, (uint32_t)&wat->out_proc_period);
    amixer_xet_ctrl(AMIXER_CMD_GET_AMIXER_PCM_SRATE_CFG, 0, (uint32_t)&wat->out_fs);

    if(wat->out_fs != wat->info.fs){
        asrc_init_param aip;
        aip.io_period = wat->out_proc_period;
        aip.ch        = wat->out_ch;
        aip.sample_size = wat->out_samples_size;
        aip.srate_in    = wat->info.fs;
        aip.srate_out   = wat->out_fs;
        aip.quality     = ASRC_QUALITY_HIGH;
        aip.user = (void*)wat;
        aip.clbk = (user_xet_data_clbk)WAudioTrackPullData;
        wat->asrc_handle = create_audiosrate_conventor(&aip);
        if(!wat->asrc_handle) {
            logd("src init fail!");
            goto create_audio_track_fail;
        }
    }

    amixer_xet_ctrl(AMIXER_CMD_GET_APPO_SHAREBUFFER_SIZE, (uint32_t)&wat->info, (uint32_t)&bufsize);

    if (0 == bufsize) {
        logd("get bufsize fail");
        goto create_audio_track_fail;
    }

    buf = general_malloc(bufsize);
    if (buf == NULL) {
        logd("audio track malloc ring buffer fail!");
        goto create_audio_track_fail;
    }

    audio_share_buffer_params_t sharebuf_prams;
    sharebuf_prams.buf = buf;
    sharebuf_prams.size = bufsize;
    sharebuf_prams.store_ch = wat->info.ch;
    sharebuf_prams.store_sample_size = wat->info.sample_size;
    sharebuf_prams.input_ch = wat->info.ch;
    sharebuf_prams.input_sample_size = wat->info.sample_size;
    sharebuf_prams.input_interleave  = wat->info.input_interleave;
    sharebuf_prams.write_refmt.func = NULL;
    sharebuf_prams.read_refmt.func = NULL;

    if (wat->info.input_interleave) {
        if(wat->info.ch != sharebuf_prams.store_ch ||
           wat->info.sample_size != sharebuf_prams.store_sample_size) {
            sharebuf_prams.write_refmt.interleave2interleave_fmt =
            reformat_to_hook(GET_REFORMAT_TYPE(init->ch, init->sample_size,
                sharebuf_prams.store_ch, sharebuf_prams.store_sample_size));
        }
    } else {
        sharebuf_prams.write_refmt.noninterleave2interleave_fmt =
            reformat_to_interleave_hook(GET_REFORMAT_TYPE(init->ch,
                init->sample_size, sharebuf_prams.store_ch,
                sharebuf_prams.store_sample_size));
    }
    if (sharebuf_prams.store_ch != wat->out_ch ||
       sharebuf_prams.store_sample_size != wat->out_samples_size) {
        sharebuf_prams.read_refmt.interleave2interleave_fmt =
            reformat_to_hook(GET_REFORMAT_TYPE(sharebuf_prams.store_ch,
                sharebuf_prams.store_sample_size,
                wat->out_ch, wat->out_samples_size));
    }
    logd("refmt : %x", sharebuf_prams.read_refmt.func);

    audio_share_buffer_init(&wat->sb, &sharebuf_prams);

    *sb = &wat->sb;
    wat->fadein_ms = init->fadein_ms == 0 ? 100 : init->fadein_ms;
    wat->fadeout_ms = init->fadeout_ms == 0 ? 100 : init->fadeout_ms;

    fade_reset(&wat->fade);
    fade_config(&wat->fade, wat->out_fs, wat->out_samples_size, wat->out_ch);

    volume_control_init(&wat->vol_ctrl,
        wat->out_fs, wat->out_ch, wat->out_samples_size, (float)1.0);

    logd("wat mode : %d", wat->info.mode);
    switch(wat->info.mode) {
        case AUDIO_MODE_NATIVE_MUSIC:
        case AUDIO_MODE_A2DP_MUSIC:
        case AUDIO_MODE_PASSTHRU:
        {
            wat->wr_sem_wait_tick = OAL_ms_to_ticks(1);
            fade_start(&wat->fade, FADE_IN, wat->fadein_ms);
            break;
        }
        case AUDIO_MODE_SCO:
        {
            fade_start(&wat->fade, FADE_IN, wat->fadein_ms);
        }
        case AUDIO_MODE_TONE:
        {
            wat->wr_sem_wait_tick = 0;
            break;
        }
    }
    if (wat->info.mode != AUDIO_MODE_TONE) {
        fade_start(&wat->fade, FADE_IN, wat->fadein_ms);
    }
    return wat;

create_audio_track_fail:
    destroyWAudioTrack(wat);
    return NULL;
}
