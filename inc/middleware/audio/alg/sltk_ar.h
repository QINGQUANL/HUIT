
#ifndef _SLTK_AR_H_
#define _SLTK_AR_H_

#include "sys_inc.h"
#include "oal.h"
#include "sltk_audio.h"
#include "bitstream_store.h"
#include "audio_in.h"
#include "audio_recorder.h"
#include "sltk_enc_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AR_MAX_CH               (2)
#define AR_MAX_SAMPLES          (128)
#define RECORDER_MAX_SAMPLES            256

typedef struct recorder_s {
    recorder_stream_params_t stream_params;
    recorder_event_listener_t event_listener;
    OAL_THREAD task_handle;

    OAL_QUEUE cmd_queue;
    void *enc;
    void *bsstore_handle;
    ain_interface_t *ain;   //这个接口暂时不用做录音
    void (*record_data_get)(int16_t* rec_data, uint32_t len);     //暂时用这个接口做录音
    void *src_url;
    void *dst_url;
    uint8_t *header_buf;

    int16_t *pcmbuf[2];

    uint32_t err_type;
    volatile uint32_t status;
    uint32_t header_sz;
    uint32_t sync_timestamp;
    int32_t samples;
    int16_t *interleave_buf;
    uint8_t ain_mode;
    uint8_t streamtype;
    
} recorder_t;

int32_t ar_send_cmd(recorder_t *recorder, cmd_msg_t msg);
int32_t ar_set_stream_info(recorder_t *recorder, stream_info_t *info);
int32_t ar_do_start(recorder_t *recorder);
int32_t ar_do_resume(recorder_t *recorder);
int32_t ar_do_pause(recorder_t *recorder);
int32_t ar_do_stop(recorder_t *recorder);
int32_t ar_do_open(recorder_t *recorder);
int32_t ar_ar_do_close_no_sync(recorder_t *recorder);
void ar_do_close(recorder_t *recorder);
void ar_do_destroy(recorder_t *recorder);
int32_t ar_process_cmd(recorder_t *recorder, uint32_t wait_ticks);
int32_t process_recording(recorder_t *recorder);
void *recorder_loop(void *handle);

#ifdef __cplusplus
}
#endif

#endif /* _SLTK_AR_H_ */
