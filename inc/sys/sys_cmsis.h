#ifndef __CORE_H__
#define __CORE_H__

void INTC_SetVector(IRQn_Type IRQn, uint32_t vector);
uint32_t INTC_GetVector(IRQn_Type IRQn);
void INTC_SetPriorityGrouping(uint32_t PriorityGroup);
uint32_t INTC_GetPriorityGrouping(void);
void INTC_EnableIRQ(IRQn_Type IRQn);
bool INTC_Get_IRQEnableState(IRQn_Type IRQn);
void INTC_DisableIRQ(IRQn_Type IRQn);
uint32_t INTC_GetPendingIRQ(IRQn_Type IRQn);
void INTC_SetPendingIRQ(IRQn_Type IRQn);
void INTC_ClearPendingIRQ(IRQn_Type IRQn);
uint32_t INTC_GetActive(IRQn_Type IRQn);
void INTC_InitPriority(void);
void INTC_SetPriority(IRQn_Type IRQn, uint32_t priority);
uint32_t INTC_GetPriority(IRQn_Type IRQn);
uint32_t INTC_EncodePriority (uint32_t PriorityGroup, uint32_t PreemptPriority, uint32_t SubPriority);
void INTC_DecodePriority (uint32_t Priority, uint32_t PriorityGroup, uint32_t* const pPreemptPriority, uint32_t* const pSubPriority);
void INTC_SystemReset(void);
uint32_t SysTick_Config(uint32_t ticks);
uint32_t get_current_exception(void);
int32_t exception_entry(uint32_t *pstack, uint32_t fault_handler_lr);
uint32_t get_current_interrupt(void);
uint32_t get_current_exception(void);
void enable_irq(void);
void disable_irq(void);
uint32_t irq_save(void);
void irq_restore(uint32_t flag);
void clear_all_pending(void);
void INTC_IRQ_Suspend(void);
void INTC_IRQ_Suspend_except_bt(void);
void INTC_IRQ_Resume(void);
void sys_tick_enable(void);
void sys_tick_disable(void);
void sys_enable_trap_div(void);
void sys_wfi(void);
uint32_t sys_get_ipsr(void);

void sys_execption_callback_register( void(*exception_cb)(uint32_t cmd));

#endif // __CORE_H__
