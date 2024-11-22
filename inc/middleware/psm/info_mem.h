#ifndef _INFO_MEM_H_
#define _INFO_MEM_H_

int sys_info_init(void);
void sys_info_deinit();
int sys_info_read(const char *variable, void *value, uint32_t max_len);
int sys_info_write(const char *variable, const void *value, uint32_t len);
int sys_info_delete(const char *variable);

#endif

