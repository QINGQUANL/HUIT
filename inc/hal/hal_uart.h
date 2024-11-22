#ifndef _HAL_UART_LIB_H_
#define _HAL_UART_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif

#define UART_NUM                3
#define UART_RX_CALLBACK_MAXNUM 2


enum uart_id_e {
    UART_ID_00 = 0,
    UART_ID_01,
    UART_ID_02,
};

/* UART Hardware Flow Control */
enum uart_hw_flow_ctl_e {
    UART_HWCONTROL_NONE       = (0x0 << 0),
    UART_HWCONTROL_AUTO       = (0x3 << 0),
    UART_HWCONTROL_ONLY_RTS   = (0x1 << 0),
    UART_HWCONTROL_ONLY_CTS   = (0x2 << 0),
};

typedef struct {
    uint32_t txbaudrate;
    uint32_t rxbaudrate;
    uint32_t clksrc;
    uint32_t flowctl;
    uint32_t fifo;
    uint32_t dma;
    uint32_t txinvert;
    uint32_t rxinvert;
} uart_param_t;

typedef void (*rx_callback)(uint32_t rx_data);
typedef struct{
    rx_callback rx_callback;
    uint8_t registered;
} uart_rx_callback;

typedef void (*rx_dma_callback)(uint8_t *rx_data, uint8_t len);
typedef struct{
    rx_dma_callback rx_dma_callback;
    uint8_t registered;
} uart_rx_dma_callback;

#define UART2_BUFFER_SIZE 256  // 根据需要调整缓冲区大小
#define PACKAGE_HEADER "HT"
#define PACKAGE_TAIL "END"

typedef struct {
    char buffer[UART2_BUFFER_SIZE];
    volatile uint32_t head;
    volatile uint32_t tail;
} uart2_buffer_t;

extern uart2_buffer_t uart2_rx_buffer;

void HAL_uart0_module_init(uart_param_t *params); // 初始化UART0模块
void HAL_uart1_module_init(uart_param_t *params); // 初始化UART1模块
void HAL_uart2_module_init(uart_param_t *params); // 初始化UART2模块
void HAL_uart_register_rx_callback(uint8_t uart_id, void (*rx_callback)(uint32_t rx_data)); // 注册UART接收回调函数
void HAL_uart_dbg_init(uint32_t uart_id); // 初始化UART调试接口
void HAL_uart_tx(uint32_t uart_id, uint32_t data); // 通过指定的UART发送数据
void uart2_printf(const char *format, ...);
char* uart2_read();  // 声明uart2_read函数

//int HAL_uart1_reinit(void); // 重新初始化UART1（此函数在您的列表中被注释掉了）
void uart1_dma_rx_callback(void); // UART1 DMA接收回调函数
void uart1_dma_tx_callback(void); // UART1 DMA发送回调函数
void uart2_dma_rx_callback(void); // UART2 DMA接收回调函数
void uart2_dma_tx_callback(void); // UART2 DMA发送回调函数
void uart_module_dma_rx_init(uint8_t uart_id,uint32_t buffer_len); // 初始化UART DMA接收
void uart_module_dma_tx_init(uint8_t uart_id,uint32_t buffer_len); // 初始化UART DMA发送
void uart_dma_tx_send_data(uint8_t uart_id,uint8_t *buffer,uint8_t len); // 通过DMA发送UART数据
void uart_dma_rx_get_data(uint8_t uart_id,uint8_t buffer_len); // 获取UART DMA接收到的数据
void uart_register_dma_rx_callback(uint8_t uart_id, void (*rx_callback)(uint8_t *rx_data, uint8_t len)); // 注册UART DMA接收回调函数

#if DEBUG_DUMP_TASK_EN
void dump_ch_data(uint32_t ch, uint8_t *buf, uint32_t bytes); // 转储通道数据
void task_dump(void *pvParameters); // 任务转储函数
void task_dump_init(void); // 初始化任务转储
#endif


#ifdef __cplusplus
}
#endif

#endif


