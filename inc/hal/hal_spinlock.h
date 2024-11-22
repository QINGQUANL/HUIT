#ifndef _HAL_SPINLOCK_H_
#define _HAL_SPINLOCK_H_

#include "sys_inc.h"

#ifdef __cplusplus
extern "C" {
#endif

void HAL_spinlock_init(void);
void *HAL_spinlock_open(void);
void HAL_spinlock_close(void *spin_lock);
void HAL_spinlock_take(void *spin_lock);
void HAL_spinlock_give(void *spin_lock);

#ifdef __cplusplus
}
#endif

#endif /* _HAL_SPINLOCK_H_ */
