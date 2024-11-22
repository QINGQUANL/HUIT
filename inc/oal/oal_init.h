/*****************************************************************************
* Project:
* All project
* Copyright (C) 2018-2021,  shenju Ltd.
* All Rights Reserved.
* Description:

* Author:
* yangyang
****************************************************************************/
#ifndef _OAL_INIT_H_
#define _OAL_INIT_H_

void OAL_init(void *heap_addr, uint32_t heap_size, uint32_t time_before_wfi);
void OAL_set_time_before_sleep(uint32_t time);
void check_os_mem(void);

#endif
