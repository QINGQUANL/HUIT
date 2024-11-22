#ifndef _HAL_CACHE_LIB_H_
#define _HAL_CACHE_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys_types.h"

enum nor_mode_e {
    NOR_MODE_INDIRECT,
    NOR_MODE_DIRECT,
    NOR_MODE_BANK,
    NOR_MODE_CACHE,
};

enum bank_addr_size_e {
    BANK_ADDR_SIZE_256K,
    BANK_ADDR_SIZE_512K,
    BANK_ADDR_SIZE_1M,
    BANK_ADDR_SIZE_2M,
    BANK_ADDR_SIZE_4M,
};

typedef struct cache_init_paras {
    uint32_t flash_size;
    bool auto_clk_gating;
    bool monitor_enable;
} cache_init_paras_t;

typedef struct bank_init_paras {
    enum bank_addr_size_e bank_addr_size;
    uint32_t bank_offset_addr;
} bank_init_paras_t;

typedef struct nor_mode_init_paras {
    enum nor_mode_e nor_mode;
    cache_init_paras_t cache_init;
    bank_init_paras_t bank_init;
} nor_mode_init_paras_t;

void HAL_cache_init(struct nor_mode_init_paras init);
void HAL_cache_deinit(void);
void HAL_cache_monior_timer(void);

#ifdef __cplusplus
}
#endif

#endif /* _HAL_CACHE_LIB_H_ */

