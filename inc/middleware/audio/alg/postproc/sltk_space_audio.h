#ifndef SPACE_AUDIO_H
#define SPACE_AUDIO_H

#include "sys_types.h"

typedef void (*space_audio_loader)(uint32_t dst_addr, uint32_t src_addr, uint32_t bytes); 

typedef struct {
    void *work;
    void (*async_work_trigger)(void *work,
        void (*work_func)(void *params), void *params);
    void (*async_work_wait_finished)(void *work);
} space_asyncwork_params_t;

typedef enum {
	SPACE_AUDIO_CMD_NONE,
	SPACE_AUDIO_CMD_SET_RADIU,
	SPACE_AUDIO_CMD_RESET_REF,
	SPACE_AUDIO_CMD_SET_LOADER,
	SPACE_AUDIO_CMD_SET_CROSSFADE_TIME,
	SPACE_AUDIO_CMD_GET_CUR_DIR,
	SPACE_AUDIO_CMD_GET_UPDATE_STA,
	SPACE_AUDIO_CMD_SETASYNCWORK,
	SPACE_AUDIO_CMD_REVERBENABLE,
}space_audio_cmd_t;

#define   SPACE_AUDIO_AZHMI_STEP  5
#define   SPACE_AUDIO_AZHMI_NUM   72

void* space_audio_init(uint32_t fs, uint32_t period, uint8_t out_ch_sel);
int   space_audio_proc(void* handle, void* left, void* right);
int   space_audio_cmd(void* handle, uint32_t cmd, uint32_t val);
void  space_audio_deinit(void* handle);

#endif
