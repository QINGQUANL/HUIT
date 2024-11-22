#ifndef _HAL_IRRX_LIB_H_
#define _HAL_IRRX_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif


typedef struct
{
    uint32_t logic_threshold;
    uint32_t noise_threshold;
    uint32_t rppi;              /*Receiver Pulse Polarity Invert */
    uint32_t wk_t9ms_max;
    uint32_t wk_t9ms_min;
    uint32_t t9ms_max;          /* 9ms:the part 1 time of start code */
    uint32_t t9ms_min;
    uint32_t t2d25ms_max;       /* 2.25ms:whole time of the logic 1 */
    uint32_t t2d25ms_min;
    uint32_t t4d5ms_max;        /* 4.5ms:the part 2 time of start code */
    uint32_t t4d5ms_min;
    uint32_t t1d69ms_max;       /* 1.69ms:the part 2 time of the logic 1 */
    uint32_t t1d69ms_min;
    uint32_t t0d56ms_max;       /* 0.56ms:the part time of the logic 1 or 0 */
    uint32_t t0d56ms_min;
    uint32_t irq;               /* interrupt irq */
    uint32_t io_sel;
} IRRX_InitTypeDef;


uint8_t HAL_irrx_get_datacode(void);
uint16_t HAL_irrx_get_usercode(void);
void HAL_irrx_module_init(IRRX_InitTypeDef *param , void(*data_cb)(void));




#ifdef __cplusplus
}
#endif


#endif

