/*
*********************************************************************************************************
*
* Filename      : task_definition.h
* Description   : task_definition head
* Revision      : V1.0
* Author        : Tong Yan
* Created       : 2021/03/17
* Last Updated  : 2021/03/17
*
* Revision History:
* 2021/03/17   Revision 1.0
*
*
*********************************************************************************************************
*/
#ifndef __TASK_DEFINITION_H__
#define __TASK_DEFINITION_H__
#include "sys_inc.h"
#include "oal.h"

/********************************************************************************************************
*   DEFINE
********************************************************************************************************/
/* 任务最大数量 */
#define TASK_NUM                    15                      // 15个任务
/* cpu id */
#define CPUID_CORE0                 0
#define CPUID_CORE1                 1

#define FAILED                      0       /* 失败                */
#define SUCCESS                     1       /* 成功                */
#define NO                          0       /* 失败                */
#define OK                          1       /* 成功                */
#define REG_SUCCESS                 (0)     /* 成功                */
#define REG_ERROR                   (-1)    /* 失败                */
#define ID_NULL                     (-1)    /* 线程未注册          */


/*watch intern msg*/
#define PAYLOAD_MAX_LENGTH          26
#define MSG_MAX_LENGTH              (PAYLOAD_MAX_LENGTH+6)
/********************************************************************************************************
*   CONSTANTS
********************************************************************************************************/

/********************************************************************************************************
*   CUSTOMIZATION
********************************************************************************************************/

/********************************************************************************************************
*   DATA TYPES
********************************************************************************************************/

typedef struct  // 内部消息结构 256 byte
{
    uint8_t  src ;      // 发送方
    uint8_t  dir ;      // 接收方
    uint8_t  cmd ;      // 命令字
    uint8_t  sub_cmd ;  // 子命令字
    uint16_t len ;      // payload字长
    uint16_t resv;
    uint8_t  payload[PAYLOAD_MAX_LENGTH] ; // 数据  250
}intern_msg_t;


/*队列*/
typedef struct
{
    OAL_QUEUE qhandle ;
    char *name ;
    int msgsize ;
    OAL_QUEUE_POOL poolname ;
}queue_definition_t;

/*软件定时器*/
typedef struct
{
    OAL_TIMER timer_handle ;
    char *name ;
    int ticks ;
    OAL_TIMER_ROUTINE pfunc ;
    void *cb_arg ;
    OAL_TIMER_RELOAD reload ;
    OAL_TIMER_ACTIVATE activate ;
}soft_timer_definition_t;

/*任务定义*/
typedef struct
{
    uint8_t cpu_id ;                          //core0 or core1
    OAL_THREAD task_handler;
    void *cpu1_handler;
    OAL_THREAD_START_ROUTINE pFun ;           //core0 func
    cpux_thread_msg_proc_func_t cpu1_pFun ;   //core1 func
    uint8_t slave_core_id ;                   //core1
    void *pUserParameter ;
    char *name ;
    OAL_THREAD_PRIORITY pri ;
    uint32_t stacksize ;
    queue_definition_t queue_def ;        // 消息队列
    soft_timer_definition_t stimer_def ;  // 软件定时器
    int task_id ;
    uint8_t queue_id;
} task_definition_t;
/********************************************************************************************************
*   GLOBAL VARIABLES
********************************************************************************************************/
extern void *cpux_thread;

/********************************************************************************************************
*   FUNCTIONS
********************************************************************************************************/
int register_task(task_definition_t *taskp) ;
int register_queue(task_definition_t *taskp) ;
int register_stimer(task_definition_t *taskp) ;
int unregister_task(char* name) ;
int get_task_index(char* name) ;
task_definition_t *get_task_by_name(char* name);
task_definition_t *get_task_by_queue_id(uint8_t queue_id);
int register_yp3_task_commands(void) ;
void disp_task_info(void);
#endif

