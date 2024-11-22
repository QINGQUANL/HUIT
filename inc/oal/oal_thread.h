/*****************************************************************************
* Project:
* All project
* Copyright (C) 2018-2021,  shenju Ltd.
* All Rights Reserved.
* Description:

* Author:
* yangyang
****************************************************************************/
#ifndef _OAL_THREAD_H_
#define _OAL_THREAD_H_

#include "oal_type.h"

#define OS_USER_APPLICATION_IDLE                1
#define OS_USER_APPLICATION_TICK                2
#define OS_USER_ASSERT_CBCK                     3
#define OS_USER_MALLOC_FAILED                   4
#define OS_USER_STACK_OVERFLOW                  5
#define OS_USER_PRE_SLEEP                       6
#define OS_USER_POST_SLEEP                      7

/*
 * Reader Writer Locks
 * This is a generic implementation of reader writer locks
 * which is reader priority.
 * Not only it provides mutual exclusion but also synchronization.
 * Six APIs are exposed to user which include.
 * -# Create a reader writer lock
 * -# Delete a reader writer lock
 * -# Reader lock
 * -# Reader unlock
 * -# Writer lock
 * -# Writer unlock
 * The locking operation is timeout based.
 * Caller can give a timeout from 0 (no wait) to
 * infinite (wait forever)
 */

typedef struct _rw_lock OAL_rw_lock_t;
/** This is prototype of reader callback */
typedef int (*cb_fn) (OAL_rw_lock_t *plock, unsigned int wait_time);

struct _rw_lock {
    /** Mutex for reader mutual exclusion */
    OAL_MUTEX  reader_mutex;
    /** Lock which when held by reader,
     *  writer cannot enter critical section
     */
    OAL_SEM rw_lock;
    /** Function being called when first reader gets
     *  the lock
     */
    cb_fn reader_cb;
    /** Counter to maintain number of readers
     *  in critical section
     */
    unsigned int reader_count;
};


int OAL_rwlock_create_with_cb(OAL_rw_lock_t *lock,
                 const char *mutex_name,
                 const char *lock_name,
                 cb_fn r_fn);

/** Create reader-writer lock
 *
 * This function creates a reader-writer lock.
 *
 * @param[in] lock Pointer to a reader-writer lock handle
 * @param[in] mutex_name Name of the mutex
 * @param[in] lock_name Name of the lock
 *
 * @return OAL_SUCCESS on success
 * @return OAL_FAILED on error
 */
int OAL_rwlock_create(OAL_rw_lock_t *lock,
                 const char *mutex_name,
                 const char *lock_name);

/** Delete a reader-write lock
 *
 * This function deletes a reader-writer lock.
 *
 * @param[in] lock Pointer to the reader-writer lock handle
 *
 */
void OAL_rwlock_delete(OAL_rw_lock_t *lock);

/** Acquire writer lock
 *
 * This function acquires a writer lock. While readers can acquire the lock on a
 * sharing basis, writers acquire the lock in an exclusive manner.
 *
 * @param[in] lock Pointer to the reader-writer lock handle
 * @param[in] wait_time The maximum amount of time, in OS ticks, the task should
 * block waiting for the lock to be acquired. The function os_msec_to_ticks()
 * can be used to convert from real-time to OS ticks. The special values \ref
 * OS_WAIT_FOREVER and \ref OS_NO_WAIT are provided to respectively wait
 * infinitely or return immediately.
 *
 * @return  OAL_SUCCESS on success
 * @return  OAL_FAILED on error
 *
 */
int OAL_rwlock_write_lock(OAL_rw_lock_t *lock,
               unsigned int wait_time);

/** Release writer lock
 *
 * This function releases a writer lock previously acquired using
 * os_rwlock_write_lock().
 *
 * @param[in] lock Pointer to the reader-writer lock handle
 */
void OAL_rwlock_write_unlock(OAL_rw_lock_t *lock);

/** Acquire reader lock
 *
 * This function acquires a reader lock. While readers can acquire the lock on a
 * sharing basis, writers acquire the lock in an exclusive manner.
 *
 * @param[in] lock pointer to the reader-writer lock handle
 * @param[in] wait_time The maximum amount of time, in OS ticks, the task should
 * block waiting for the lock to be acquired. The function os_msec_to_ticks()
 * can be used to convert from real-time to OS ticks. The special values \ref
 * OS_WAIT_FOREVER and \ref OS_NO_WAIT are provided to respectively wait
 * infinitely or return immediately.
 *
 * @return  OAL_SUCCESS on success
 * @return  OAL_FAILED on error
 *
 */
int OAL_rwlock_read_lock(OAL_rw_lock_t *lock,
            unsigned int wait_time);

/** Release reader lock
 *
 * This function releases a reader lock previously acquired using
 * os_rwlock_read_lock().
 *
 * @param[in] lock pointer to the reader-writer lock handle
 *
 * @return OAL_SUCCESS if unlock operation successful.
 * @return OAL_FAILED if unlock operation failed.
 */
int OAL_rwlock_read_unlock(OAL_rw_lock_t *lock);

typedef enum {
    OAL_THREAD_PRIORITY_REAL_TIME = 6,
    OAL_THREAD_PRIORITY_HIGH = 5,
    OAL_THREAD_PRIORITY_LOW_HIGH = 4,
    OAL_THREAD_PRIORITY_NORMAL = 3,
    OAL_THREAD_PRIORITY_LOW_NORMAL = 2,
    OAL_THREAD_PRIORITY_LOW = 1,
    OAL_THREAD_PRIORITY_IDLE = 0
}OAL_THREAD_PRIORITY;

typedef enum {
    OAL_THREAD_STATE_RUNNING,
    OAL_THREAD_STATE_READY,
    OAL_THREAD_STATE_BLOCKED,
    OAL_THREAD_STATE_SUSPENDED,
    OAL_THREAD_STATE_DELETED,
} OAL_THREAD_STATE;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// thread
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OAL_sleep(uint64_t milliseconds);
void OAL_sleep_ticks(uint32_t ticks);
uint32_t OAL_ms_to_ticks(uint32_t msecs);
unsigned int OAL_get_tick_count(void);

#define OAL_WAIT_FOREVER	0xFFFFFFFF
OAL_SEM OAL_sem_create_binary(void);
uint32_t OAL_in_handler_mode(void);
OAL_SEM OAL_sem_create_count(int32_t initcount, int32_t maxcount);
OAL_RESULT OAL_sem_destroy(OAL_SEM *sem);
OAL_RESULT OAL_sem_wait(OAL_SEM *sem);
OAL_RESULT OAL_sem_wait_ticks_timeout(OAL_SEM *sem, uint32_t ticks);
OAL_RESULT OAL_sem_wait_timeout(OAL_SEM *sem, uint32_t milliseconds);
void OAL_sem_post(OAL_SEM *sem);

OAL_MUTEX OAL_mutex_create(void);
OAL_RESULT OAL_mutex_destroy(OAL_MUTEX *mutex);
OAL_RESULT OAL_mutex_lock(OAL_MUTEX *mutex);
void OAL_mutex_unlock(OAL_MUTEX *mutex);

void OAL_enter_critical_section(void);
void OAL_exit_critical_section(void);

typedef void * (*OAL_THREAD_START_ROUTINE)(void*);

typedef struct {
    OAL_THREAD_START_ROUTINE pFunc;
    void*		pUserParameter;
    void*		handle;
    OAL_SEM		exitSem;
    int         priority;
    OAL_SEM		joinSem;
}OAL_THREAD_HANDLE;

OAL_THREAD OAL_thread_create(OAL_THREAD_START_ROUTINE pFun, void *pUserParameter, char *name, OAL_THREAD_PRIORITY pri, unsigned int stackSize);
OAL_RESULT OAL_thread_wait_join(OAL_THREAD thread, uint32_t tick);
void OAL_thread_notify_wait(uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *notifyValue, uint32_t xTicks);

void OAL_thread_join(OAL_THREAD thread);
void OAL_start_scheduler(void);
void OAL_thread_delete(OAL_THREAD thread);
void OAL_thread_delete_self(OAL_THREAD thread);
void OAL_thread_notify(OAL_THREAD thread, uint32_t ulValue, uint8_t eAction);

void OAL_thread_all_suspend(void);
void OAL_thread_all_resume(void);

OAL_RESULT OAL_thread_priority_set(OAL_THREAD thread, int32_t nPriority);
int32_t OAL_thread_priority_get(OAL_THREAD thread);
char* OAL_thread_name_get(OAL_THREAD thread);
uint32_t OAL_thread_stack_addr_get(void);
OAL_THREAD_STATE OAL_thread_state_get(OAL_THREAD thread);
void OAL_thread_task_list(char * pcWriteBuffer);  //ty^2021.03.18
void OAL_thread_yield();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// cpux thread
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OAL_cpux_mbox_destroy(void *mbox);
void *OAL_cpux_mbox_create(uint8_t src_core_id, uint8_t dst_core_id,
    uint32_t msgsize, uint32_t max_msg_count);
OAL_RESULT OAL_cpux_mbox_send(void *mbox, const void *msg);
int32_t OAL_cpux_mbox_get_msgs_waiting(void *mbox);
OAL_RESULT OAL_cpux_mbox_recv(void *mbox, void *msg);

typedef void (*cpux_thread_msg_proc_func_t)(void *user_parameter, void *msg);

//cpux_thread初始化参数
typedef struct {
    cpux_thread_msg_proc_func_t proc_func; //线程处理函数，当CPU0发送消息给此cpux_thread时，系统自动读出此消息，然后此函数被调用
    void *user_parameter;                  //用户参数
    char *name;                            //thread名字
    uint8_t slave_core_id;                 //从cpu核ID (slave_core_id > 0)
    int32_t priority;                      //线程优先级，priority >= 0, priority越大优先级越高
    uint32_t stack_size;                   //线程堆栈大小
    uint32_t msg_size;                     //每一个消息的大小
    uint32_t msg_count;                    //消息队列最大可存放的消息个数
} OAL_CPUX_THREAD_PARAMS;

/*------------------------------------------------------------------------------
* 函数功能: 启动从CPU
* 函数名称: OAL_RESULT OAL_cpux_startup(uint8_t slave_core_id)
* 参    数: slave_core_id: 从cpu核ID号（slave_core_id > 0)
* 返 回 值: 成功返回OAL_SUCCESS，失败返回错误码
* 说明 :此函数必须由主CPU核(CPU0)调用
*-------------------------------------------------------------------------------*/
OAL_RESULT OAL_cpux_startup(uint8_t slave_core_id);

/*------------------------------------------------------------------------------
* 函数功能: 在cpux上创建一个线程
* 函数名称: void *OAL_cpux_thread_create(OAL_CPUX_THREAD_PARAMS *params)
* 参    数: 参考@OAL_CPUX_THREAD_PARAMS
* 返 回 值: 成功返回线程句柄，失败返回NULL
* 说明 : 此函数必须由主CPU核(CPU0)调用
*-------------------------------------------------------------------------------*/
void *OAL_cpux_thread_create(OAL_CPUX_THREAD_PARAMS *params);

/*------------------------------------------------------------------------------
* 函数功能: 删除一个cpux线程
* 函数名称: OAL_RESULT OAL_cpux_thread_join(void *cpux_thread)
* 参    数:
*           cpux_thread: 线程句柄
* 返 回 值: 成功返回OAL_SUCCESS，失败返回错误码
* 说明 : 此函数必须由主CPU核(CPU0)调用
*-------------------------------------------------------------------------------*/
OAL_RESULT OAL_cpux_thread_join(void *cpux_thread);

/*------------------------------------------------------------------------------
* 函数功能: 主cpu给cpux_thread发送（写入）一个消息
* 函数名称: OAL_RESULT OAL_cpux_thread_master_msg_send(void *cpux_thread, void *msg)
* 参    数:
*           cpux_thread: 线程句柄
*           msg: 要发送的消息
* 返 回 值: 成功返回OAL_SUCCESS，失败返回错误码
* 说明 : 此函数必须由主CPU核(CPU0)调用;如果消息队列满，则一直阻塞等待队列有空间后写入才退出
*-------------------------------------------------------------------------------*/
OAL_RESULT OAL_cpux_thread_master_msg_send(void *cpux_thread, void *msg);

/*------------------------------------------------------------------------------
* 函数功能: 获取从CPU发往主CPU的消息队列中有多少个消息
* 函数名称: uint32_t OAL_cpux_thread_master_msg_waiting_get(void *cpux_thread)
* 参    数:
*       cpux_thread: 线程句柄
* 返 回 值: 返回消息个数
* 说明 : 此函数由主CPU核(CPU0)调用
*-------------------------------------------------------------------------------*/
uint32_t OAL_cpux_thread_master_msg_waiting_get(void *cpux_thread);

/*------------------------------------------------------------------------------
* 函数功能: 主核接收（读出）一个消息
* 函数名称: OAL_RESULT OAL_cpux_thread_master_msg_recv(void *cpux_thread, void *msg)
* 参    数:
*           cpux_thread: 线程句柄
*           msg: 要读出的消息
* 返 回 值: 成功返回OAL_SUCCESS，失败返回错误码
* 说明 : 此函数必须由主CPU核(CPU0)调用. 如果队列无消息，则一直阻塞到有消息;
*        消息的来源是从CPU调用OAL_cpux_thread_slave_msg_send写入
*-------------------------------------------------------------------------------*/
OAL_RESULT OAL_cpux_thread_master_msg_recv(void *cpux_thread, void *msg);

/*------------------------------------------------------------------------------
* 函数功能: 从CPU给主CPU（CPU0）发送（写入）一个消息
* 函数名称: OAL_RESULT OAL_cpux_thread_slave_msg_send(void *cpux_thread, void *msg)
* 参    数:
*           cpux_thread: 线程句柄
*           msg: 要发送的消息
* 返 回 值: 成功返回OAL_SUCCESS，失败返回错误码
* 说明 : 此函数必须由从CPU调用; 如果消息队列满，则一直阻塞等待队列有空间后写入才退出
*-------------------------------------------------------------------------------*/
OAL_RESULT OAL_cpux_thread_slave_msg_send(void *cpux_thread, void *msg);

/*------------------------------------------------------------------------------
* 函数功能: 获取主CPU发往从CPU的消息队列中有多少个消息
* 函数名称: uint32_t OAL_cpux_thread_slave_msg_waiting_get(void *cpux_thread)
* 参    数:
*       cpux_thread: 线程句柄
* 返 回 值: 返回消息个数
* 说明 : 此函数由从CPU(CPU0)调用
*-------------------------------------------------------------------------------*/
uint32_t OAL_cpux_thread_slave_msg_waiting_get(void *cpux_thread);

/*------------------------------------------------------------------------------
* 函数功能: 从CPU接收（读出）一个消息
* 函数名称: OAL_RESULT OAL_cpux_thread_slave_msg_recv(void *cpux_thread, void *msg)
* 参    数:
*           cpux_thread: 线程句柄
*           msg: 要读出的消息
* 返 回 值: 成功返回OAL_SUCCESS，失败返回错误码
* 说明 : 此函数必须由从CPU核(CPU0)调用。如果队列无消息，则一直阻塞到有消息;
*        消息的来源是主CPU调用OAL_cpux_thread_master_msg_send写入
*-------------------------------------------------------------------------------*/
OAL_RESULT OAL_cpux_thread_slave_msg_recv(void *cpux_thread, void *msg);


/*------------------------------------------------------------------------------
* 函数功能: 在cpux上创建一个work线程
* 函数名称: void *OAL_cpux_work_create(uint8_t slave_core_id, int32_t priority, uint32_t stack_size)
* 参    数:
*           slave_cord_id: 从cpu编号（slave_cord_id > 0)
*           priority: 线程优先级，priority >= 0, priority越大优先级越高
*           stack_size: 线程堆栈大小
* 返 回 值: 成功返回线程句柄，失败返回NULL
* 说明 : 此函数必须由主CPU核(CPU0)调用. wrok线程是一种简单的cpux_thread, 不需要用户关心消息的发送接收
*-------------------------------------------------------------------------------*/
void *OAL_cpux_work_create(uint8_t slave_core_id, int32_t priority, uint32_t stack_size);

/*------------------------------------------------------------------------------
* 函数功能: 删除一个cpux work线程
* 函数名称: OAL_RESULT OAL_cpux_work_destory(void *work)
* 参    数:
*           work: 线程句柄
* 返 回 值: 成功返回OAL_SUCCESS，失败返回错误码
* 说明 : 此函数必须由主CPU核(CPU0)调用
*-------------------------------------------------------------------------------*/
OAL_RESULT OAL_cpux_work_destory(void *work);

/*------------------------------------------------------------------------------
* 函数功能: 请求cpux运行某一个函数
* 函数名称: OAL_RESULT OAL_cpux_work_request(void *work, void (*work_func)(void *params), void *params)
* 参    数:
*           work: 线程句柄
*           work_func: 需要cpux运行的函数
*           params: work_func的参数
* 返 回 值: 成功返回OAL_SUCCESS，失败返回错误码
* 说明 : 此函数必须由主CPU核(CPU0)调用。此函数一直阻塞直到cpux处理完work_func
*-------------------------------------------------------------------------------*/
OAL_RESULT OAL_cpux_work_request(void *work, void (*work_func)(void *params), void *params);

/*------------------------------------------------------------------------------
* 函数功能: 触发cpux运行某一个函数
* 函数名称: OAL_RESULT OAL_cpux_work_trigger(void *work, void (*work_func)(void *params), void *params)
* 参    数:
*           work: 线程句柄
*           work_func: 需要cpux运行的函数
*           params: work_func的参数
* 返 回 值: 成功返回OAL_SUCCESS，失败返回错误码
* 说明 : 此函数必须由主CPU核(CPU0)调用。此函数仅仅通知cpux处理，但不会阻塞等待cpux处理完成,。
*        一般在异步双核处理时用到， 一般配合OAL_cpux_work_wait_finished使用
*-------------------------------------------------------------------------------*/
OAL_RESULT OAL_cpux_work_trigger(void *work, void (*work_func)(void *params), void *params);

/*------------------------------------------------------------------------------
* 函数功能: 等待cpux处理work_func完成
* 函数名称: OAL_RESULT OAL_cpux_work_wait_finished(void *work)
* 参    数:
*           work: 线程句柄
* 返 回 值: 成功返回OAL_SUCCESS，失败返回错误码
* 说明 : 此函数必须由主CPU核(CPU0)调用。此函数阻塞等待cpux处理完成,。
*        一般在异步双核处理时用到， 一般配合OAL_cpux_work_trigger使用
*-------------------------------------------------------------------------------*/
OAL_RESULT OAL_cpux_work_wait_finished(void *work);

#endif


