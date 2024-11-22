#ifndef _HAL_LCDC_LIB_H_
#define _HAL_LCDC_LIB_H_

typedef struct {
    uint8_t line;              // 3线模式/4线模式
    uint8_t spi_interface;     // 串行接口 I/串行接口 II
    uint8_t rx_phase;          // 接收相位 0/接收相位 1
    uint8_t cpha;              // 相位 0/1
    uint8_t cpol;              // 极性高/低
    uint8_t data_lane;         // 1数据通道/2数据通道
    uint8_t dummy_cycle;       // N个周期，N最大值为15
} lcdc_spi_ctrl_type;

typedef struct {
    uint8_t rdx_h_cycle;        // N+1 8080-RDX脉冲“高”持续时间，N最大值为15
    uint8_t rdx_l_cycle;        // N+1 8080-RDX脉冲“低”持续时间，N最大值为15
    uint8_t wrx_h_cycle;        // N+1 8080-WRX脉冲“高”持续时间，N最大值为15
    uint8_t wrx_l_cycle;        // N+1 8080-WRX脉冲“低”持续时间，N最大值为15
} lcdc_8080_ctrl_type;

typedef struct {
    uint8_t io_mapping[16];     // IO映射数组
    bool io_en[16];             // IO使能标志数组
} lcdc_io_ctrl_type;

typedef struct {
    bool rx_en;                 // 接收使能标志
    bool tx_en;                 // 发送使能标志
    void (*lcdc_dma_rx_callback)(); // DMA接收回调函数
    void (*lcdc_dma_tx_callback)(); // DMA发送回调函数
} lcdc_dma_t;

typedef struct {
    void (*dma_disp_flush)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *buf, bool big_end); // DMA显示刷新函数
    void (*disp_flush)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *buf, bool big_end);       // 显示刷新函数
    void (*lcd_clear)(uint32_t color);                          // 清屏函数
    void (*bootloader_disp)(void);                              // 引导加载程序显示函数
} lcd_api_t;



typedef struct {
    uint8_t clk_sorce;             // 时钟源：HOSC/PLL_DIV_96M
    uint8_t clkdiv;                // 时钟分频器
    uint8_t lcdc_interface;        // LCD接口模式：8080模式/SPI模式
    lcdc_8080_ctrl_type ctrl_8080; // 8080模式控制参数
    lcdc_spi_ctrl_type spi_ctrl;   // SPI模式控制参数
    lcdc_io_ctrl_type io_ctrl;     // IO控制参数
    lcdc_dma_t dma_param;          // DMA参数
    lcd_api_t lcd_api;             // LCD API接口
} lcdc_param_t;


typedef struct {
    bool te_trigger_flag;  // TE触发标志
    bool en;               // 启用标志
} lcd_te_ctrl_t;

enum lcdc_interface_e {
    LCDC_INTERFACE_8080 = 0x0, // 8080接口
    LCDC_INTERFACE_SPI,         // SPI接口
};

enum lcdc_spi_line_e {
    LCDC_SPI_3LINE = 0x0, // 3线SPI
    LCDC_SPI_4LINE,       // 4线SPI
};

enum lcdc_spi_interface_e {
    LCDC_SPI_INTERFACE_I = 0x0, // SPI接口类型I
    LCDC_SPI_INTERFACE_II,      // SPI接口类型II
};

enum lcdc_spi_rx_phase_e {
    LCDC_SPI_RX_PHASE_0 = 0x0, // 接收相位0
    LCDC_SPI_RX_PHASE_1,       // 接收相位1
};

enum lcdc_spi_polarity_e {
    LCDC_SPI_ACTIVE_POLARITY_HIGH = 0x0, // 活动极性高 (0 = 空闲)
    LCDC_SPI_ACTIVE_POLARITY_LOW,        // 活动极性低 (1 = 空闲)
};

enum lcdc_spi_phase_e {
    LCDC_SPI_PHASE_0 = 0x0, // SPI相位0
    LCDC_SPI_PHASE_1,       // SPI相位1
};

enum lcdc_spi_data_lane_e {
    LCDC_SPI_1DATA_LANE = 0x0, // 1数据通道
    LCDC_SPI_2DATA_LANE,       // 2数据通道
};


enum lcdc_8080_hl_cycle_e {
    LCDC_8080_HL_CYCLE_1 = 0x0,   // HL周期1
    LCDC_8080_HL_CYCLE_2,         // HL周期2
    LCDC_8080_HL_CYCLE_3,         // HL周期3
    LCDC_8080_HL_CYCLE_4,         // HL周期4
    LCDC_8080_HL_CYCLE_5,         // HL周期5
    LCDC_8080_HL_CYCLE_6,         // HL周期6
    LCDC_8080_HL_CYCLE_7,         // HL周期7
    LCDC_8080_HL_CYCLE_8,         // HL周期8
    LCDC_8080_HL_CYCLE_9,         // HL周期9
    LCDC_8080_HL_CYCLE_10,        // HL周期10
    LCDC_8080_HL_CYCLE_11,        // HL周期11
    LCDC_8080_HL_CYCLE_12,        // HL周期12
    LCDC_8080_HL_CYCLE_13,        // HL周期13
    LCDC_8080_HL_CYCLE_14,        // HL周期14
    LCDC_8080_HL_CYCLE_15,        // HL周期15
    LCDC_8080_HL_CYCLE_16,        // HL周期16
};



void HAL_lcdc_module_init(lcdc_param_t *param); // 初始化LCD控制模块，传入参数指针
void HAL_lcdc_module_io_setting(lcdc_io_ctrl_type *io_ctrl); // 设置LCD控制模块的IO配置
void HAL_lcdc_module_write_reg(uint8_t cmd); // 写入寄存器命令
void HAL_lcdc_module_write_data(uint32_t data, uint8_t len); // 写入数据，len最大为4字节
uint32_t HAL_lcdc_module_read_data(uint8_t len); // 读取数据，len最大为4字节
void HAL_lcdc_module_read(unsigned int cmd, uint8_t cmd_len, uint8_t * data_buf, uint8_t data_len); // 读取数据，cmd_len最大为4字节，data_len最大为16字节
void HAL_lcdc_module_write(unsigned int cmd, uint8_t cmd_len, uint8_t * data_buf, uint8_t data_len); // 写入数据，cmd_len最大为4字节，data_len最大为16字节
void HAL_lcdc_module_spi_cs_hold(void); // 保持SPI片选信号
void HAL_lcdc_module_spi_cs_de_select(void); // 取消选择SPI片选信号
void HAL_lcdc_module_spi_dummy_cycle_set(uint8_t dummy_cycle); // 设置SPI的空操作周期
bool HAL_is_lcdc_tx_dma_transing(void); // 检查LCD控制器的TX DMA是否正在传输
bool HAL_is_lcdc_rx_dma_transing(void); // 检查LCD控制器的RX DMA是否正在传输
void HAL_lcdc_module_dma_rx_init(void (*full_callback)(void)); // 初始化DMA接收，传入回调函数
void HAL_lcdc_module_dma_tx_init(void (*full_callback)(void)); // 初始化DMA发送，传入回调函数
void HAL_lcdc_dma_read(uint32_t cmd, uint8_t cmd_len, uint8_t *data_buf, uint16_t data_len); // DMA读取数据，data_len需为4字节对齐
void HAL_lcdc_dma_write(uint32_t cmd, uint8_t cmd_len, uint8_t *data_buf, uint16_t data_len); // DMA写入数据，data_len需为4字节对齐
void lcdc_dma_rx_callback(void); // DMA接收回调函数
void lcdc_dma_tx_callback(void); // DMA发送回调函数

void HAL_lcd_te_trigger_set(bool flag); // 设置LCD TE触发标志
bool HAL_lcd_te_trigger_get(void); // 获取LCD TE触发标志
void HAL_lcd_te_en_set(bool en); // 设置LCD TE使能
bool HAL_lcd_te_en_get(void); // 获取LCD TE使能状态
void HAL_lcd_api_register(lcd_api_t *api); // 注册LCD API
void HAL_lcd_dma_disp_flush(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t *buf,bool big_end); // DMA刷新显示，指定区域和缓冲区
void HAL_lcd_disp_flush(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t *buf,bool big_end); // 刷新显示，指定区域和缓冲区
void HAL_lcd_clear(uint32_t color); // 清屏，设置背景色
void HAL_lcd_bootloader_disp(void); // 显示引导加载程序界面


#endif

