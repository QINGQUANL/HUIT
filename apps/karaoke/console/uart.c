#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "console.h"
#include "oal.h"
#include "ring_buffer.h"
#include "hal_dma.h"
#include "hal_uart.h"
#include "hal_gpio.h"
#include "stdarg.h"
#include "stdio.h"

//#include "app_afx_config.h"

#define UART2_BUFFER_SIZE 256  // 定义UART发送缓冲区的大小

uart2_buffer_t uart2_rx_buffer = {.head = 0, .tail = 0};

uint32_t uart2_data;

#if EQ_AFX_API_TEST
#include "eq_process.h"
#include "audio_service.h"

#define MAX_SUBFUNC_APIS_NUM  16

typedef enum {
    STATE_PREPARE,
    STATE_GET_FID,
    STATE_GET_PARA,
}FUNC_GET_STATE;

uint8_t   cmd_cache_state   = 0;
uint8_t   cmd_cache_it      = 0;
uint8_t   func_get_state    = STATE_PREPARE;
uint8_t   func_id           = 0;
uint32_t  para[8]           = {0};
uint32_t  para_it           = 0;

uint8_t   cmd_cache[32]     = {0};

AFX_API_TEST_FUNC void reset_afx_test()
{
    func_get_state = STATE_PREPARE;
    func_id = 0;
    libc_memset(para, 0x00, 8*sizeof(uint32_t));
    para_it = 0;
    libc_memset(cmd_cache, 0x00, 32);
    cmd_cache_it = 0;
}

AFX_API_TEST_FUNC void call_test_func()
{
    switch(func_id) {
        case API_NONE:
        {
            loge("bypass");
            on_set_bypass_profile();
            break;
        }
        case API_CLEAR_HOLDERS:
        {
            loge("clear holders");
            clear_all_holder();
            break;
        }
        case API_REVB_ON_SET_DEFAULT_REVB:
        {
            loge("revb(%d,%d)", para[0], para[1]);
            on_set_default_reverb_profile(para[0], para[1]);
            break;
        }
        case API_REVB_ON_SWITCH_MS:
        {
            loge("revb ms(%d)", para[0]);
            on_set_rec_reverb_ms_switch(para[0]);
            break;
        }
        case API_REVB_ON_SET_GWET:
        {
            loge("revb-gwet(%d)", para[0]);
            on_set_reverb_gwet(para[0]);
            break;
        }
        case API_REVE_ON_SET_RT60:
        {
            loge("revb-rt60(%d)", para[0]);
            on_set_reverb_time(para[0]);
            break;
        }
        case API_ECHO_ON_SET_DEFAULT_ECHO:
        {
            loge("echo(%d, %d)", para[0], para[1]);
            on_set_default_echo_profile(para[0], para[1]);
            break;
        }
        case API_ECHO_ON_SET_DELAY:
        {
            loge("echo - dly(%d)", para[0]);
            on_set_echo_delay(para[0]);
            break;
        }
        case API_ECHO_ON_SET_GWET:
        {
            loge("echo- gwet", para[0]);
            on_set_echo_gwet(para[0]);
            break;
        }
        case API_TSPS_ON_SET_FEMALE:
        {
            loge("female(%d)");
            on_set_female_profile(para[0]);
            break;
        }
        case API_TSPS_ON_SET_MALE:
        {
            loge("male(%d)");
            on_set_male_profile(para[0]);
            break;
        }
        case API_TSPS_ON_SET_ROBOT:
        {
            loge("robot(%d)");
            on_set_robot_profile(para[0]);
            break;
        }
        case API_TSPS_ON_SET_KID:
        {
            loge("kid(%d)");
            on_set_kid_profile(para[0]);
            break;
        }
        case API_TSPS_ON_SET_VC_CHANGE:
        {
            loge("vc(%d)");
            on_set_voc_change_profile(para[0]);
            break;
        }
        case API_EQ_ON_SET_RADIO_PROFILE:
        {
            loge("radio(%d)");
            on_set_radio_profile(para[0]);
            break;
        }
        case API_EQ_ON_SET_TREB_BASS_GAIN:
        {
            loge("ebg(%d, %d, %d)",para[0], para[1], para[2]);
            on_set_treb_bass_gain(para[0], para[1], para[2]);
            break;
        }
        case API_EQ_ON_SET_PREAMP:
        {
            loge("epreamp(%d)", para[0]);
            on_set_eq_focus_preamp(para[0]);
            break;
        }
        case API_EQ_ON_SET_ATUNE:
        {
            loge("atune (%d, %d)", para[0], para[1]);
            on_set_autotune_profile(para[0], para[1]);
            atune_setting as = {0};
            on_x_atune_for_pageid(PAGE_MIC, ATUNE_ALL_PAPAS_MASK, &as, EQ_GET_DATA);
            loge("%s", as.enable?"on":"dis");
            loge("note:%d", as.note);
            loge("oct:%d",  as.oct);
            loge("scale:%d",as.scale);
            loge("speed:%d",      as.speed);
            break;
        }
        case API_DRC_ON_SET_NGATE:
        {
            loge("ng (%d, %d)", para[0], para[1]);
            on_set_natural_noise_gate(para[0], para[1]);
            break;
        }
        case API_VSS_ON_SET_SPK_PROFILE:
        {
            loge("vss spk(%d)");
            on_set_default_vss_spk_profile(para[0]);
            break;
        }
        case API_VSS_ON_SET_HP_PROFILE:
        {
            loge("vss hp(%d)");
            on_set_default_vss_hp_profile(para[0]);
            break;
        }
        case API_EQ5_ROCK_MIC:
        {
            loge("rock mic");
            on_set_rock_mic_eq_profile(para[0]);
            break;
        }
        case API_EQ5_SONIC_BOOM:
        {
            loge("sonic boom");
            on_set_sonic_boom_eq_profile(para[0]);
            break;
        }
        case API_AS_TEST:
        {
            loge("as test");
            audio_service_cmd(para[0], para[1]);
            break;
        }
    }
}

AFX_API_TEST_FUNC void parse_cmd_cache()
{
    int data = 0;
    func_get_state = STATE_PREPARE;
    for(int i = 0; i < cmd_cache_it; i++) {
        data = cmd_cache[i];
        //loge("data : %d", data);
        if(func_get_state == STATE_PREPARE)
        {
            if(data == 'f') {
                func_get_state = STATE_GET_FID;
                func_id = API_NONE;
            }
            else {
                reset_afx_test();
            }
        }
        else if(func_get_state == STATE_GET_FID) {
            //loge("fid");
            if(data >= '0' && data <= '9') {
                func_id = 10*func_id + (data - '0');
            }
            else if( data == '(' ) {
                func_get_state = STATE_GET_PARA;
                para_it = 0;
            }
            else {
                reset_afx_test();
            }
        }
        else if(func_get_state == STATE_GET_PARA) {
            //loge("para");
            if(data == ',') {//next
                para_it++;
            }
            else if(data == ')') {//end to call func
                //loge("call, func : %d", func_id);
                call_test_func();
                reset_afx_test();
            }
            else {
                para[para_it] = 10*para[para_it] + (data - '0');
            }
        }
        else {
            reset_afx_test();
        }
    }
}
#endif

#if (UART0_EN || UART1_EN || UART2_EN)

static bool rx_flag = 0;

// UART2 的接收回调函数
void uart2_rx_callback(uint32_t rx_data) {
    char received_char = (char)rx_data;
    static char recchar[4] = {0};

    recchar[0] = recchar[1];
    recchar[1] = recchar[2];
    recchar[2] = received_char;
    recchar[3] = '\0';
    if(rx_flag == 0 && strcmp(recchar, "END") == 0)
    {
        rx_flag = 1;
        uart2_rx_buffer.buffer[uart2_rx_buffer.tail - 2] = '\0';//更改为\0
        uart2_rx_buffer.tail = 0;
        // uart2_printf("uart2_rx_buffer.buffer: %s\r\n", uart2_rx_buffer.buffer);
    }

    // 检查缓冲区是否已满
    if(rx_flag == 0)
    {
        if ((uart2_rx_buffer.tail + 1) % UART2_BUFFER_SIZE == uart2_rx_buffer.head) {
            // 缓冲区已满，无法存储新数据
            // uart2_printf("UART2 buffer overflow\r\n");
        } else {
            // 将接收到的字符存储到缓冲区
            uart2_rx_buffer.buffer[uart2_rx_buffer.tail] = received_char;
            uart2_rx_buffer.tail = (uart2_rx_buffer.tail + 1) % UART2_BUFFER_SIZE;
            // uart2_printf("uart2_rx_buffer.buffer: %s\r\n", uart2_rx_buffer.buffer);
        }
    }
}

char* uart2_read() {
    static char read_buffer[UART2_BUFFER_SIZE];
    if(rx_flag == 1)
    {
        strcpy(read_buffer,uart2_rx_buffer.buffer);
        memset(uart2_rx_buffer.buffer, 0, sizeof(uart2_rx_buffer.buffer));
        rx_flag = 0;
        return read_buffer;
    }
    return NULL;  // 没有完整的数据包可读
}


void uart_init(void)
{
    __maybe_unused uart_param_t params;
#if UART0_EN
    HAL_gpio_config(UART0_RX_PIN_CFG);
    HAL_gpio_config(UART0_TX_PIN_CFG);

    params.txbaudrate = UART0_TXBAUDRATE;
    params.rxbaudrate = UART0_RXBAUDRATE;
    params.clksrc = UART0_SCLK_FREQ;
    params.flowctl = 0;  //uart0 not support flowctl
    params.fifo = 0;     //uart0 not support fifo mode
    params.dma = 0;
    params.txinvert = 0;
    params.rxinvert = 0;
    HAL_uart0_module_init(&params);
#endif

#if UART1_EN
    HAL_gpio_config(UART1_RX_PIN_CFG);
    HAL_gpio_config(UART1_TX_PIN_CFG);

    params.txbaudrate = UART1_TXBAUDRATE;
    params.rxbaudrate = UART1_RXBAUDRATE;
    params.clksrc = UART1_SCLK_FREQ;
    params.flowctl = UART1_HWFLOWCTL;
    params.fifo = UART1_FIFO_EN;
    params.dma  = UART1_DMA_EN;
    params.txinvert = 0;
    params.rxinvert = 0;

    if (UART1_HWFLOWCTL) {
        HAL_gpio_config(UART1_RTS_PIN_CFG);
        HAL_gpio_config(UART1_CTS_PIN_CFG);
    }

    HAL_uart1_module_init(&params);
#endif

#if UART2_EN
    HAL_gpio_config(UART2_RX_PIN_CFG);
    HAL_gpio_config(UART2_TX_PIN_CFG);

    params.txbaudrate = UART2_TXBAUDRATE;
    params.rxbaudrate = UART2_RXBAUDRATE;
    params.clksrc = UART2_SCLK_FREQ;
    params.flowctl = UART2_HWFLOWCTL;
    params.fifo = UART2_FIFO_EN;
    params.dma = UART2_DMA_EN;
    params.txinvert = 0;
    params.rxinvert = 0;

    if (UART2_HWFLOWCTL) {
        HAL_gpio_config(UART2_RTS_PIN_CFG);
        HAL_gpio_config(UART2_CTS_PIN_CFG);
    }

    HAL_uart2_module_init(&params);
    // uart2_printf("HAL_uart_register_rx_callback\r\n");
    HAL_uart_register_rx_callback(UART_ID_02, uart2_rx_callback);

#endif

    uart_dma_mode_init();

#if (DEBUG_LOG_EN || DEBUG_DEDICATE_LOG_EN)
#if ((UART_DEBUG == 0) || (UART_DEBUG == 1) || (UART_DEBUG == 2))
    HAL_uart_dbg_init(UART_DEBUG);
#endif
#endif

}

AT_SRAM(.uart)
void uart_suspend(bool suspend)
{
    if (suspend) {
#if UART0_EN
        //HAL_gpio_func_set(UART0_RX_PIN_NUM, PIN_FUNCDISABLE);
        //HAL_gpio_func_set(UART0_TX_PIN_NUM, PIN_FUNCDISABLE);
#endif
#if UART1_EN
        HAL_gpio_func_set(UART1_CTS_PIN_NUM, PIN_FUNCDISABLE);
        HAL_gpio_func_set(UART1_RTS_PIN_NUM, PIN_FUNCDISABLE);
        HAL_gpio_func_set(UART1_RX_PIN_NUM, PIN_FUNCDISABLE);
        HAL_gpio_func_set(UART1_TX_PIN_NUM, PIN_FUNCDISABLE);
#endif
#if UART2_EN
        HAL_gpio_func_set(UART2_CTS_PIN_NUM, PIN_FUNCDISABLE);
        HAL_gpio_func_set(UART2_RTS_PIN_NUM, PIN_FUNCDISABLE);
        HAL_gpio_func_set(UART2_RX_PIN_NUM, PIN_FUNCDISABLE);
        HAL_gpio_func_set(UART2_TX_PIN_NUM, PIN_FUNCDISABLE);
#endif
    } else {
#if UART0_EN
        //HAL_gpio_func_set(UART0_RX_PIN_NUM, UART0_RX_PIN_FUNC);
        //HAL_gpio_func_set(UART0_TX_PIN_NUM, UART0_TX_PIN_FUNC);
#endif
#if UART1_EN
        HAL_gpio_func_set(UART1_CTS_PIN_NUM, UART1_CTS_PIN_FUNC);
        HAL_gpio_func_set(UART1_RTS_PIN_NUM, UART1_RTS_PIN_FUNC);
        HAL_gpio_func_set(UART1_RX_PIN_NUM, UART1_RX_PIN_FUNC);
        HAL_gpio_func_set(UART1_TX_PIN_NUM, UART1_TX_PIN_FUNC);
#endif
#if UART2_EN
        HAL_gpio_func_set(UART2_CTS_PIN_NUM, UART2_CTS_PIN_FUNC);
        HAL_gpio_func_set(UART2_RTS_PIN_NUM, UART2_RTS_PIN_FUNC);
        HAL_gpio_func_set(UART2_RX_PIN_NUM, UART2_RX_PIN_FUNC);
        HAL_gpio_func_set(UART2_TX_PIN_NUM, UART2_TX_PIN_FUNC);
#endif
    }
}

void uart_uart1_dma_callback(uint8_t *rx_data, uint8_t len)
{
#if UART1_EN
#if UART1_DMA_EN
    printf_array(rx_data,len);
#endif
#endif
}

void uart_uart2_dma_callback(uint8_t *rx_data, uint8_t len)
{
    // 在这里处理接收到的数据
    for (int i = 0; i < len; i++) {
        // 例如，将接收到的数据打印出来
        // uart2_printf("Received data on UART2: %c\n", rx_data[i]);
    }
#if UART2_EN
#if UART2_DMA_EN
    printf_array(rx_data,len);
#endif
#endif
}

void uart_dma_mode_init(void)
{
#if (UART1_EN && UART1_DMA_EN)
    uart_module_dma_rx_init(UART_ID_01,DMA_DATA_LEN);
    uart_register_dma_rx_callback(UART_ID_01,uart_uart1_dma_callback);
    uart_module_dma_tx_init(UART_ID_01,DMA_DATA_LEN);
#endif

#if (UART2_EN && UART2_DMA_EN)
    uart_module_dma_rx_init(UART_ID_02,DMA_DATA_LEN);
    uart_register_dma_rx_callback(UART_ID_02,uart_uart2_dma_callback);
    uart_module_dma_tx_init(UART_ID_02,DMA_DATA_LEN);
#endif
}

#if KEY_UART_EN
AT_SRAM(.uart)
void uart_send_event(uint32_t event)
{
    event_put(event, NULL, 0);
}

AT_SRAM(.uart)
void uart_rx_key(uint32_t data)
{
#if EQ_AFX_API_TEST
    if(cmd_cache_state == 0) {
#endif
    switch (data) {
    case 'p':
        uart_send_event(KEY_EVENT_SU | KEY_PREV);
        break;

    case 'n':
        uart_send_event(KEY_EVENT_SU | KEY_NEXT);
        break;

    case '+':
        uart_send_event(KEY_EVENT_SU | KEY_VOL_UP);
        break;

    case '-':
        uart_send_event(KEY_EVENT_SU | KEY_VOL_DOWN);
        break;

    case ' ':
        uart_send_event(KEY_EVENT_SU | KEY_PLAY);
        break;

    case 'r':
        uart_send_event(KEY_EVENT_L | KEY_PLAY);
        break;

    case 'm':
        mode_update(1);
        uart_send_event(KEY_EVENT_SU | KEY_MODE);
        break;

    case 'e':
        uart_send_event(KEY_EVENT_SU | KEY_EQ);
        break;

	case 0x0D:
		logi(" ");
		break;

	case '1':
		logi(" p1 ");
        *(uint32_t *)0x40037ffc ^= 1<<1;
		break;

    case '2':
        logi(" p2 ");
        *(uint32_t *)0x40037ffc ^= 1<<2;
        break;

    case '3':
        logi(" p3 ");
        *(uint32_t *)0x40037ffc ^= 1<<3;
        break;

    case '4':
        logi(" p4 ");
        *(uint32_t *)0x40037ffc ^= 1<<4;
        break;

    case '5':
        logi(" p5 ");
        *(uint32_t *)0x40037ffc ^= 1<<5;
        break;

	case '6':
		logi(" p6 ");
        *(uint32_t *)0x40037ffc ^= 1<<6;
		break;

    case '7':
        logi(" p7 ");
        *(uint32_t *)0x40037ffc ^= 1<<7;
        break;

    case '8':
        logi(" p8 ");
        *(uint32_t *)0x40037ffc ^= 1<<8;
        break;

    case '9':
        logi(" p9 ");
        *(uint32_t *)0x40037ffc ^= 1<<9;
        break;

    case '0':
        logi(" p10 ");
        *(uint32_t *)0x40037ffc ^= 1<<10;
        break;
#if EQ_AFX_API_TEST
        case '#':
            cmd_cache_state = 1;
            cmd_cache_it    = 0;
            break;
        case '@':
            reset_afx_test();
            break;
#endif
        }
#if EQ_AFX_API_TEST
    }
    else {
        if(data == '#') {
            cmd_cache_state = 0;
            uart_send_event(EQ_EVENT_AFX_TEST);
        }
        else {
            cmd_cache[cmd_cache_it++] = data & 0xff;
        }
    }
#endif
}
#endif

void uart_test(void)
{
    for (uint8_t i=0;i<10;i++) {
        HAL_uart_tx(UART_ID_00, 0xc0+i);
    }
    while(1);
}

/**
 * 通过UART发送格式化字符串，类似于printf。
 * 
 * @param uart_id 指定的UART接口ID。
 * @param format 格式化字符串。
 * @param ... 可变参数列表。
 */
void uart2_printf(const char *format, ...) {
    char buffer[UART2_BUFFER_SIZE];  // 创建一个缓冲区来存储格式化后的字符串
    va_list args;  // 定义可变参数列表

    // 初始化可变参数列表
    va_start(args, format);
    
    // 使用vsnprintf格式化字符串
    vsnprintf(buffer, UART2_BUFFER_SIZE, format, args);
    
    // 结束可变参数列表的使用
    va_end(args);

    // 逐个字符发送格式化后的字符串
    for (char *ptr = buffer; *ptr != '\0'; ptr++) {
        HAL_uart_tx(UART_ID_02, (uint32_t)(*ptr));  // 发送当前字符
    }
}

#else
void uart_init(void)
{
}

void uart_dma_mode_init(void)
{
}

#endif

