#ifndef AEFFECT_PLUGIN_H
#define AEFFECT_PLUGIN_H

#include "sys_inc.h"
#include "sltk_mem.h"
#include "sltk_audio.h"

typedef enum {
    AEFX_TYPE_NONE = 0,
    AEFX_TYPE_TSPS,
    AEFX_TYPE_EQ14,
    AEFX_TYPE_SPACE_AUDIO,
    AEFX_TYPE_VSS,
    AEFX_TYPE_REVERB,
    AEFX_TYPE_DRC,
    AEFX_TYPE_ECHO,
    AEFX_TYPE_FREQ_SHIFT,
    AEFX_TYPE_FORMANT,
    AEFX_TYPE_HLPASS_DRC,
    AEFX_TYPE_NUM
}aeffect_type_main;

typedef enum {
    AEFX_IO_TYPE_PLAYBACK,
    AEFX_IO_TYPE_EFFECT,
    AEFX_IO_TYPE_MIC,
} aeffect_io_type;

typedef struct {
    uint16_t type_main;
    uint16_t synposition;
    uint8_t io_type;
}aeffect_type;

typedef struct {
    uint8_t  ch;
    uint8_t  sample_size;
    uint16_t period;
    uint32_t fs;
}acommon_init_para;

typedef struct {
    acommon_init_para asource_common;
    notify_clbk notify;
    void*    user_data;
}aeffect_plugin_init_para;

typedef enum {
    AEFFECT_PLUGIN_NOTIFY_NONE = 0,
    AEFFECT_PLUGIN_NOTIFY_REQ_LOCK_AFX_LIST,
    AEFFECT_PLUGIN_NOTIFY_REQ_UNLOCK_AFX_LIST,
    AEFFECT_PLUGIN_NOTIFY_REFRESH_CPULOAD_INFO
}aeffect_plugin_notify;

typedef enum {
    AEFX_CMD_NONE,
    AEFX_CMD_LOADING_PLG_SETTING,
    AEFX_CMD_RESET_PLG_SETTING,
    AEFX_CMD_CLEAR_PLG_DATA,
    AEFX_CMD_GET_PLG_TUNNINGAREA_SIZE,
}aeffect_common_cmd;

typedef int32_t ( *aeffect_node_proc)(void *handle, void* inb, void* outb, int32_t samples);
typedef int32_t (  *aeffect_node_cmd)(void *handle, uint32_t cmd, uint32_t arg);
typedef void*   ( *aeffect_node_open)(aeffect_plugin_init_para *init_para);
typedef void    (*aeffect_node_close)(void *handle);

typedef struct aeffect_node_s aeffect_node_t;
struct aeffect_node_s {
    aeffect_type_main  type;
    aeffect_node_open  open;
    aeffect_node_cmd   cmd;
    aeffect_node_proc  proc;
    aeffect_node_close close;
};

void aeffect_plugin_register(aeffect_node_t* node);
aeffect_node_t* aeffect_plugin_find(aeffect_type_main type);

#define AUDIOEFXS_PLG_MAX_NODE_NUM 7

typedef struct {
    uint8_t  audiofx[AUDIOEFXS_PLG_MAX_NODE_NUM];
    uint8_t  audiofx_num;
}aeffect_list_info;

#endif