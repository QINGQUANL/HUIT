#ifndef _HAL_TIMER_LIB_H_
#define _HAL_TIMER_LIB_H_

typedef enum
{
    TIMER0  = 0x00U,
    TIMER1  = 0x01U,
    TIMER2  = 0x02U,
    TIMER3  = 0x03U
} HAL_TimerId;

typedef enum
{
    TIMER_CLK_LOSC  = 0x0U,
    TIMER_CLK_HOSC  = 0x1U
} HAL_TimerClkSrc;

typedef struct {
    uint32_t int_priority;  //timer interupt priority
    uint32_t period_in_us;  //timer period in us
    void (* callback)(void);//timer callback
    bool repeat;            //0:single mode, 1:repeat mode
    uint8_t clk_src;        //timer clk src sel, 0:LOSC, 1:HOSC
} timer_param_t;

/**
  * @brief  Initializes the timer specified by timer_id
  * @param  timer_id: timer_id
                  init_para:
  * @retval OAL_SUCCESS if success, OAL_FAILED if fail.
  */
uint8_t HAL_timer_init(HAL_TimerId timer_id, timer_param_t *init_para);

/**
  * @brief  De-initializes the timer specified by timer_id
  * @param  timer_id: timer_id
  * @retval none
  * note: before a timer can be re-initialized and reused, timer_deinit should firstly called, or timer_init will fail
  */
void HAL_timer_deinit(HAL_TimerId timer_id);

/**
  * @brief  start the timer specified by timer_id
  * @param  timer_id: timer_id
  * @retval none
  * note: before a timer can be started, timer_init should firstly called and success
  */
void HAL_timer_start(HAL_TimerId timer_id);

/**
  * @brief  stop the timer specified by timer_id
  * @param  timer_id: timer_id
  * @retval none
  */
void HAL_timer_stop(HAL_TimerId timer_id);

#endif

