#ifndef _AREC_CLIENT_H_
#define _AREC_CLIENT_H_

#include "sys_inc.h"
#include "audio_track.h"
#include "ring_buffer.h"

enum arecord_cmd_e {
    ARECORD_CMD_NONE,
    ARECORD_CMD_STOP,
    ARECORD_CMD_START,
    ARECORD_CMD_VOLUME_SET,
    ARECORD_CMD_FADE_SET,
    ARECORD_CMD_HW_PATH_CHANGE,
    ARECORD_CMD_HW_PATH_OPEN,
    ARECORD_CMD_HW_PATH_CLOSE,
    ARECORD_CMD_CLEAR_DATA,
};

typedef struct {
    uint32_t fs;
    uint16_t sharebuf_samples;
    uint16_t mode;
    uint16_t fadein_ms;
    uint16_t fadeout_ms;
    uint8_t  ch;
    uint8_t  sample_size;
} arecord_init_t;

typedef struct {
    uint32_t track_id;
    uint32_t sharebuffer_ptr;
} arecord_init_rply;

typedef struct {
    uint32_t fade_ms; //fade时间设置
    uint8_t  fade_type; //FADE_IN or FADE_OUT
} arecord_fade_t;

void* arecord_open(arecord_init_t *init);
void arecord_close(void *handle);
int arecord_cmd(void *handle, uint32_t cmd, uint32_t val);
int arecord_read(void *handle, void *buf, uint32_t samples);


#endif