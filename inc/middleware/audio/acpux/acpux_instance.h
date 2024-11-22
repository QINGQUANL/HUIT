#ifndef ACPUX_INSTANCE_H
#define ACPUX_INSTANCE_H

#include "sys_inc.h"
#include "sltk_audio.h"
#include "audio_heap.h"

typedef int32_t (*cpux_running_func)(void* handle, void* io_buffer, int32_t samples);

enum cpu0msg_e {
    CPU0MSG_INIT_REQ,
    CPU0MSG_DEINIT_REQ,
    CPU0MSG_INPUT_ACK,
    CPU0MSG_OUTPUT_ACK,

    CPU0MSG_FRAME_START,
    CPU0MSG_FRAME_END_ACK,

    CPU0MSG_EN_ANC_IRQ,
    CPU0MSG_DIS_ANC_IRQ,
};

enum cpu1msg_e {
    CPU1MSG_INIT_ACK,
    CPU1MSG_DEINIT_ACK,
    CPU1MSG_INPUT_REQ,
    CPU1MSG_OUTPUT_REQ,
    CPU1MSG_FRAME_END,
};


typedef struct {
    uint8_t ch;
    uint8_t samples_size;
    uint16_t period;
    cpux_running_func user_cpu0func;
    cpux_running_func user_cpu1func;
    void* user_data;
    notify_clbk notify;
}acpux_instance_init;

typedef struct {
    void* io_buffer;
    int32_t samples;
}acpux_user_interface_packet;

typedef enum {
    ACPUX_INSTANCE_CMD_NONE = 0,
    ACPUX_INSTANCE_CMD_FREE_LAST_ACPUX_PERIOD,
}acpux_cmd;

typedef enum {
    ACPUX_INSTANCE_NOTIFY_NONE = 0,
    ACPUX_INSTANCE_NOTIFY_LOCK_USER_DATAFLOW,
    ACPUX_INSTANCE_NOTIFY_UNLOCK_USER_DATAFLOW
}acpux_instance_notify;

void acpux_instance_xet_ctrl(void* handle, uint32_t cmd, uint32_t val, uint32_t rev);
void acpux_instance_destroy(void* handle);
void* acpux_instance_create(acpux_instance_init* init);
int32_t acpux_instance_proc(void* handle, void* io_buffer, int32_t samples);

#endif