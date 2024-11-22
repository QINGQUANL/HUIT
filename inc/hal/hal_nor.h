#ifndef _HAL_NOR_LIB_H_
#define _HAL_NOR_LIB_H_

int32_t HAL_nor_read(uint32_t ram_addr, uint32_t flash_addr, uint32_t len);
int32_t HAL_nor_write(uint32_t ram_addr, uint32_t flash_addr, uint32_t len, uint32_t wait_busy);
int32_t HAL_nor_erase(uint32_t cmd, uint32_t flash_addr, uint8_t wait_busy);


#endif
