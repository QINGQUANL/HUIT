#ifndef AMSG_H
#define AMSG_H

#include "sys_inc.h"
#include "sltk_mem.h"
#include "sltk_audio.h"
#include "oal.h"

typedef struct {
    OAL_SEM _lock;         // 锁，用于保护结构体的互斥访问
    uint32_t cmd;          // 命令字段，表示要执行的操作
    uint32_t req;          // 请求字段，表示请求的类型或状态
    uint32_t rly;          // 回复字段，存储响应结果或状态
    uint32_t user_data;    // 用户数据，存储与用户相关的附加信息
} amsg_t;


typedef enum {
    AMSG_MSG_SENDING = 0,
    AMSG_MSG_SENT,
}amsg_cmd_t;

typedef struct {
    notify_clbk cctrl;
    void*   user_data;
}amsg_init_t;

void  destroy_amsg_component(void* handle);
void* create_amsg_component(amsg_init_t *init);
int   amsg_block_msg_send(void* handle, uint32_t cmd, uint32_t val, uint32_t rev);
int   amsg_nonblock_msg_send(void* handle, uint32_t cmd, uint32_t val, uint32_t rev);

#endif