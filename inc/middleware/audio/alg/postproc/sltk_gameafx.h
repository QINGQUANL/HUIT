#ifndef _SLTK_GAMEAFX_H_
#define _SLTK_GAMEAFX_H_

#include "sys_types.h"


typedef struct {
    void *work;
    void (*async_work_trigger)(void *work,
        void (*work_func)(void *params), void *params);
    void (*async_work_wait_finished)(void *work);
} gameafx_asyncwork_params_t;

typedef enum {
    GAMEAFX_CMD_NONE,
    GAMEAFX_CMD_SETASYNCWORK,
} gameafx_audio_cmd_t;


void  gameafx_deinit(void* handle);
void* gameafx_init(uint32_t fs, uint32_t period, uint8_t out_ch_sel);
int gameafx_proc(void* handle, void* left, void* right);
int gameafx_cmd(void *handle, uint32_t cmd, uint32_t arg);


#endif //_SLTK_GAMEAFX_H_
