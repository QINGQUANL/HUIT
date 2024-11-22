#ifndef	_SYS_EXCEPTION_H_
#define _SYS_EXCEPTION_H_

void wait_idle_running_a_period(uint32_t wait_ms);
void sys_register_task_idle_hook(void (*hook)(void));

uint32_t sys_get_PSP(void);
void exception_fault_restart_set(bool en);

#endif
