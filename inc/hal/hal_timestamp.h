#ifndef _HAL_TIMESTAMP_H_
#define _HAL_TIMESTAMP_H_


void HAL_timestamp_init(uint32_t div);
uint64_t HAL_us_to_timestamp(uint32_t us);
uint32_t HAL_timestamp_to_us(uint64_t ts);
uint64_t HAL_ms_to_timestamp(uint32_t ms);
uint32_t HAL_timestamp_to_ms(uint64_t ts);
uint64_t HAL_get_timestamp(void);
void HAL_set_timestamp(uint64_t val);
uint64_t HAL_get_timestamp32(void);
void HAL_delay_nop(uint32_t cnt);

/*
 * loop delay, not suspend task, can use in isr
 * delay in cycle
 */
void HAL_cdelay(uint32_t cycle);

/*
 * loop delay, not suspend task, can use in isr
 * delay in us
 */
void HAL_udelay(uint32_t us);

/*
 * loop delay, not suspend task, can use in isr
 * delay in ms
 */
void HAL_mdelay(uint32_t ms);

/*
 * schedule delay, suspend task, can not use in isr
 * delay in ms
 */
void HAL_msleep(uint32_t ms);



#endif