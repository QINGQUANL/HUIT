#ifndef _APP_LCDC_H_
#define _APP_LCDC_H_
#include "sys_inc.h"
#include "hal_lcdc.h"
#include "user_config.h"
#include "app_lcd_ili9488.h"
#include "app_lcd_gc9a01.h"
#include "app_lcd_st7789v2.h"
#include "app_lcd_st7789s.h"
#include "app_lcd_st7735s.h"
#include "ring_buffer.h"

#if (LCD_BACKLIGHT_USE_PWM == 1)
void lcd_backlight_percent_on(void);
void lcd_backlight_percent_off(void);
void lcd_backlight_percent_init(void);
#endif

#define L2R_U2D                     0                           //Left to Right, Up to Down
#define L2R_D2U                     1                           //Left to Right, Down to Up
#define R2L_U2D                     2                           //Right to Left, Up to Down
#define R2L_D2U                     3                           //Right to Left, Down to Up
#define U2D_L2R                     4                           //Up to Down, Left to Right
#define U2D_R2L                     5                           //Up to Down, Right to Left
#define D2U_L2R                     6                           //Down to Up, Left to Right
#define D2U_R2L                     7                           //Down to Up, Right to Left

#define LCD_DIR_VERTICAL            1//0                           //竖屏
#define LCD_DIR_HORIZONTAL          0//1                           //横屏

#define LCD_COLOR_FORMAT_RGB666     0
#define LCD_COLOR_FORMAT_RGB565     1

//一般屏RGB写入顺序为R->G->B,R在高地址字节区,8080和SPI连续写入时先写低地址字节,因此需要转换一下
#define RGB565_BEND(color)       (((color)>>8)&0xff)|(((color)&0xff)<<8)
#define RGB666_BEND(color)       (((color)>>16)&0xff)|((color)&0xff00)|(((color)&0xff)<<16)
#define DMA_RING_BUF_SIZE           (4096*2)

#define LCD_CHIP_ILI9488            0
#define LCD_CHIP_GC9A01             1       //IG01 SPI/IG02 8080
#define LCD_CHIP_ST7789V2_IG03      2       //IG03 8080, no RDX pin
#define LCD_CHIP_ST7789S            3       //8080
#define LCD_CHIP_ST7735S            4       //8080  

/********************  user setting for LCD *******************/
#define LCD_CHIP_SEL                LCD_CHIP_GC9A01//LCD_CHIP_ST7735S-----11/10

#define LCDC_TEST_EN                0

#define LCD_WIDTH                   240//320                         //竖屏时宽度
#define LCD_HEIGHT                  240//480                         //竖屏时高度
#define LCD_DFT_SCAN_DIR            R2L_U2D//L2R_U2D                     //Left to Right, Up to Down
#define LCD_DFT_DIR                 LCD_DIR_VERTICAL//LCD_DIR_HORIZONTAL//            //竖屏
#define LCD_WR_RAM_CMD              0x2C                        //写gram命令
#define LCD_RD_RAM_CMD              0x2E                        //读gram命令
#define LCD_SET_X_CMD               0x2A                        //设置X坐标命令
#define LCD_SET_Y_CMD               0x2B                        //设置Y坐标命令
#define LCD_COLOR_FORMAT            LCD_COLOR_FORMAT_RGB565

#if (LCD_CHIP_SEL == LCD_CHIP_ILI9488)
#undef LCD_WIDTH
#undef LCD_HEIGHT
#undef LCD_DFT_SCAN_DIR
#define LCD_DFT_SCAN_DIR            R2L_U2D
#define LCD_WIDTH                   320
#define LCD_HEIGHT                  480
#if LCDC_INTERFACE                                              //spi模式ili9488不支持RGB565
#undef LCD_COLOR_FORMAT
#define LCD_COLOR_FORMAT            LCD_COLOR_FORMAT_RGB666
#endif

#elif (LCD_CHIP_SEL == LCD_CHIP_GC9A01)
#undef LCD_WIDTH
#undef LCD_HEIGHT
#undef LCD_DFT_SCAN_DIR
#undef LCD_COLOR_FORMAT
#undef LCDC_SPI_INTERFACE
#define LCD_DFT_SCAN_DIR            R2L_U2D//L2R_U2D
#define LCD_WIDTH                   240
#define LCD_HEIGHT                  240
#define LCD_COLOR_FORMAT            LCD_COLOR_FORMAT_RGB565     //GC9A01 RGB666写一个点时会写不出来
#define LCDC_SPI_INTERFACE          0                           //0:Interface I(SDA和SDO复用)

#if LCDC_INTERFACE
#undef LCDC_X11_SEL
#undef LCDC_X11_MAP
#define LCDC_X11_SEL                0                           //0:DIS;1:PD11(fpga);2:PB3;3:PD6
#define LCDC_X11_MAP                0xF                         //Disable
#endif

#elif (LCD_CHIP_SEL == LCD_CHIP_ST7789V2_IG03)
#undef LCD_WIDTH
#undef LCD_HEIGHT
#undef LCD_DFT_SCAN_DIR
#define LCD_DFT_SCAN_DIR            L2R_U2D
#define LCD_WIDTH                   240
#define LCD_HEIGHT                  280

#elif (LCD_CHIP_SEL == LCD_CHIP_ST7789S)
#undef LCD_WIDTH
#undef LCD_HEIGHT
#undef LCD_DFT_SCAN_DIR
#define LCD_DFT_SCAN_DIR            L2R_U2D//L2R_U2D
#define LCD_WIDTH                 240
#define LCD_HEIGHT                320
// #define LCD_WIDTH                LV_HOR_RES_MAX
// #define LCD_HEIGHT               LV_VOR_RES_MAX
#elif (LCD_CHIP_SEL == LCD_CHIP_ST7735S)
#undef LCD_WIDTH
#undef LCD_HEIGHT
#undef LCD_DFT_SCAN_DIR

#if (LCD_DFT_DIR == LCD_DIR_HORIZONTAL)     //横屏
#define LCD_DFT_SCAN_DIR            R2L_D2U         
#define LCD_DIR_R90_CHANGE_SUPPORT  1               //部分元素显示时使用L2R_U2D扫描，图像需要向右旋转90度
#else
#define LCD_DFT_SCAN_DIR            L2R_U2D
#endif

#if LCDC_INTERFACE
#define LCD_WIDTH                   240///160
#define LCD_HEIGHT                  240//80
#else
#define LCD_WIDTH                   240
#define LCD_HEIGHT                  320
#endif
// #define LCD_WIDTH                LV_HOR_RES_MAX
// #define LCD_HEIGHT               LV_VOR_RES_MAX
#endif


//SPI:LOW Byte fist and HIGH bit first
#if (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB666)
#define RED    0xFC0000                      //RGB666
#define GREEN  0x00FC00
#define BLUE   0x0000FC
#define YELLOW 0xFCFC00
#define WHITE  0xFFFFFF
#define BLACK  0x0000

#define RGB_BEND(color)             RGB666_BEND(color)

#elif (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB565)
#define RED    0xF800                        //RGB565
#define GREEN  0x07E0
#define BLUE   0x001F
#define YELLOW 0xFFE0
#define WHITE  0xFFFF
#define BLACK  0x0000

#define RGB_BEND(color)             RGB565_BEND(color)
#endif

/*******************************PIN CFG********************************************/

#if (LCDC_X0_SEL==0)
#elif (LCDC_X0_SEL==1)
#define LCDC_X0_PIN_CFG             (PIN_D0 | PIN_FUNC5)        //for fpga
#elif (LCDC_X0_SEL==2)
#define LCDC_X0_PIN_CFG             (PIN_A4 | PIN_FUNC3)
#elif (LCDC_X0_SEL==3)
#define LCDC_X0_PIN_CFG             (PIN_B14 | PIN_FUNC12)
#elif (LCDC_X0_SEL==4)
#define LCDC_X0_PIN_CFG             (PIN_D11 | PIN_FUNC12)
#endif

#if (LCDC_X1_SEL==0)
#elif (LCDC_X1_SEL==1)
#define LCDC_X1_PIN_CFG             (PIN_D1 | PIN_FUNC5)        //for fpga
#elif (LCDC_X1_SEL==2)
#define LCDC_X1_PIN_CFG             (PIN_A5 | PIN_FUNC3)
#elif (LCDC_X1_SEL==3)
#define LCDC_X1_PIN_CFG             (PIN_B15 | PIN_FUNC12)
#endif

#if (LCDC_X2_SEL==0)
#elif (LCDC_X2_SEL==1)
#define LCDC_X2_PIN_CFG             (PIN_D2 | PIN_FUNC5)        //for fpga
#elif (LCDC_X2_SEL==2)
#define LCDC_X2_PIN_CFG             (PIN_B16 | PIN_FUNC12)
#endif

#if (LCDC_X3_SEL==0)
#elif (LCDC_X3_SEL==1)
#define LCDC_X3_PIN_CFG             (PIN_D3 | PIN_FUNC5)        //for fpga
#elif (LCDC_X3_SEL==2)
#define LCDC_X3_PIN_CFG             (PIN_B17 | PIN_FUNC12)
#endif

#if (LCDC_X4_SEL==0)
#elif (LCDC_X4_SEL==1)
#define LCDC_X4_PIN_CFG             (PIN_D4 | PIN_FUNC5)        //for fpga
#elif (LCDC_X4_SEL==2)
#define LCDC_X4_PIN_CFG             (PIN_B18 | PIN_FUNC12)
#endif

#if (LCDC_X5_SEL==0)
#elif (LCDC_X5_SEL==1)
#define LCDC_X5_PIN_CFG             (PIN_D5 | PIN_FUNC5)        //for fpga
#elif (LCDC_X5_SEL==2)
#define LCDC_X5_PIN_CFG             (PIN_B19 | PIN_FUNC12)
#endif

#if (LCDC_X6_SEL==0)
#elif (LCDC_X6_SEL==1)
#define LCDC_X6_PIN_CFG             (PIN_D6 | PIN_FUNC5)        //for fpga
#elif (LCDC_X6_SEL==2)
#define LCDC_X6_PIN_CFG             (PIN_B20 | PIN_FUNC12)
#elif (LCDC_X6_SEL==3)
#define LCDC_X6_PIN_CFG             (PIN_D1 | PIN_FUNC12)
#endif

#if (LCDC_X7_SEL==0)
#elif (LCDC_X7_SEL==1)
#define LCDC_X7_PIN_CFG             (PIN_D7 | PIN_FUNC5)        //for fpga
#elif (LCDC_X7_SEL==2)
#define LCDC_X7_PIN_CFG             (PIN_B21 | PIN_FUNC12)
#elif (LCDC_X7_SEL==3)
#define LCDC_X7_PIN_CFG             (PIN_D2 | PIN_FUNC12)
#endif

#if (LCDC_X8_SEL==0)
#elif (LCDC_X8_SEL==1)
#define LCDC_X8_PIN_CFG             (PIN_D8 | PIN_FUNC5)        //for fpga
#elif (LCDC_X8_SEL==2)
#define LCDC_X8_PIN_CFG             (PIN_B6 | PIN_FUNC12)
#elif (LCDC_X8_SEL==3)
#define LCDC_X8_PIN_CFG             (PIN_D3 | PIN_FUNC12)
#endif

#if (LCDC_X9_SEL==0)
#elif (LCDC_X9_SEL==1)
#define LCDC_X9_PIN_CFG             (PIN_D9 | PIN_FUNC5)        //for fpga
#elif (LCDC_X9_SEL==2)
#define LCDC_X9_PIN_CFG             (PIN_B5 | PIN_FUNC12)
#elif (LCDC_X9_SEL==3)
#define LCDC_X9_PIN_CFG             (PIN_D4 | PIN_FUNC12)
#endif

#if (LCDC_X10_SEL==0)
#elif (LCDC_X10_SEL==1)
#define LCDC_X10_PIN_CFG             (PIN_D10 | PIN_FUNC5)        //for fpga
#elif (LCDC_X10_SEL==2)
#define LCDC_X10_PIN_CFG             (PIN_B4 | PIN_FUNC12)
#elif (LCDC_X10_SEL==3)
#define LCDC_X10_PIN_CFG             (PIN_D5 | PIN_FUNC12)
#endif

#if (LCDC_X11_SEL==0)
#elif (LCDC_X11_SEL==1)
#define LCDC_X11_PIN_CFG             (PIN_D11 | PIN_FUNC5)        //for fpga
#elif (LCDC_X11_SEL==2)
#define LCDC_X11_PIN_CFG             (PIN_B3 | PIN_FUNC12)
#elif (LCDC_X11_SEL==3)
#define LCDC_X11_PIN_CFG             (PIN_D6 | PIN_FUNC12)
#endif

#if (LCDC_X12_SEL==0)
#elif (LCDC_X12_SEL==1)
#define LCDC_X12_PIN_CFG             (PIN_D12 | PIN_FUNC5)        //for fpga
#elif (LCDC_X12_SEL==2)
#define LCDC_X12_PIN_CFG             (PIN_B2 | PIN_FUNC12)
#elif (LCDC_X12_SEL==3)
#define LCDC_X12_PIN_CFG             (PIN_D7 | PIN_FUNC12)
#endif

#if (LCDC_X13_SEL==0)
#elif (LCDC_X13_SEL==1)
#define LCDC_X13_PIN_CFG             (PIN_D13 | PIN_FUNC5)        //for fpga
#elif (LCDC_X13_SEL==2)
#define LCDC_X13_PIN_CFG             (PIN_B1 | PIN_FUNC12)
#elif (LCDC_X13_SEL==3)
#define LCDC_X13_PIN_CFG             (PIN_D8 | PIN_FUNC12)
#endif

#if (LCDC_X14_SEL==0)
#elif (LCDC_X14_SEL==1)
#define LCDC_X14_PIN_CFG             (PIN_D14 | PIN_FUNC5)        //for fpga
#elif (LCDC_X14_SEL==2)
#define LCDC_X14_PIN_CFG             (PIN_B0 | PIN_FUNC12)
#elif (LCDC_X14_SEL==3)
#define LCDC_X14_PIN_CFG             (PIN_D9 | PIN_FUNC12)
#endif

#if (LCDC_X15_SEL==0)
#elif (LCDC_X15_SEL==1)
#define LCDC_X15_PIN_CFG             (PIN_D15 | PIN_FUNC5)        //for fpga
#elif (LCDC_X15_SEL==2)
#define LCDC_X15_PIN_CFG             (PIN_D10 | PIN_FUNC12)
#endif


#if (LCDC_X0_SEL==0)
#define LCDC_X0_INIT()
#else
#define LCDC_X0_INIT()              HAL_gpio_config(LCDC_X0_PIN_CFG);
#endif

#if (LCDC_X1_SEL==0)
#define LCDC_X1_INIT()
#else
#define LCDC_X1_INIT()              HAL_gpio_config(LCDC_X1_PIN_CFG);
#endif

#if (LCDC_X2_SEL==0)
#define LCDC_X2_INIT()
#else
#define LCDC_X2_INIT()              HAL_gpio_config(LCDC_X2_PIN_CFG);
#endif

#if (LCDC_X3_SEL==0)
#define LCDC_X3_INIT()
#else
#define LCDC_X3_INIT()              HAL_gpio_config(LCDC_X3_PIN_CFG);
#endif

#if (LCDC_X4_SEL==0)
#define LCDC_X4_INIT()
#else
#define LCDC_X4_INIT()              HAL_gpio_config(LCDC_X4_PIN_CFG);
#endif

#if (LCDC_X5_SEL==0)
#define LCDC_X5_INIT()
#else
#define LCDC_X5_INIT()              HAL_gpio_config(LCDC_X5_PIN_CFG);
#endif

#if (LCDC_X6_SEL==0)
#define LCDC_X6_INIT()
#else
#define LCDC_X6_INIT()              HAL_gpio_config(LCDC_X6_PIN_CFG);
#endif

#if (LCDC_X7_SEL==0)
#define LCDC_X7_INIT()
#else
#define LCDC_X7_INIT()              HAL_gpio_config(LCDC_X7_PIN_CFG);
#endif

#if (LCDC_X8_SEL==0)
#define LCDC_X8_INIT()
#else
#define LCDC_X8_INIT()              HAL_gpio_config(LCDC_X8_PIN_CFG);
#endif

#if (LCDC_X9_SEL==0)
#define LCDC_X9_INIT()
#else
#define LCDC_X9_INIT()              HAL_gpio_config(LCDC_X9_PIN_CFG);
#endif

#if (LCDC_X10_SEL==0)
#define LCDC_X10_INIT()
#else
#define LCDC_X10_INIT()             HAL_gpio_config(LCDC_X10_PIN_CFG);
#endif

#if (LCDC_X11_SEL==0)
#define LCDC_X11_INIT()
#else
#define LCDC_X11_INIT()             HAL_gpio_config(LCDC_X11_PIN_CFG);
#endif

#if (LCDC_X12_SEL==0)
#define LCDC_X12_INIT()
#else
#define LCDC_X12_INIT()             HAL_gpio_config(LCDC_X12_PIN_CFG);
#endif

#if (LCDC_X13_SEL==0)
#define LCDC_X13_INIT()
#else
#define LCDC_X13_INIT()             HAL_gpio_config(LCDC_X13_PIN_CFG);
#endif

#if (LCDC_X14_SEL==0)
#define LCDC_X14_INIT()
#else
#define LCDC_X14_INIT()             HAL_gpio_config(LCDC_X14_PIN_CFG);
#endif

#if (LCDC_X15_SEL==0)
#define LCDC_X15_INIT()
#else
#define LCDC_X15_INIT()             HAL_gpio_config(LCDC_X15_PIN_CFG);
#endif

#define LCD_RESET_INIT()            HAL_gpio_config(LCD_RESET_PIN_NUM | PIN_FUNCOUTPUT| PIN_PULL_UP100K |PIN_HIGH)//HAL_gpio_func_set(LCD_RESET_PIN_NUM, PIN_FUNCOUTPUT)
#define LCD_RESET_REL()             HAL_gpio_value_set(LCD_RESET_PIN_NUM, 1)
#define LCD_RESET()                 HAL_gpio_value_set(LCD_RESET_PIN_NUM, 0)

#if EVB_LCD_DEMO
#define LCD_BACKLIGHT_ON()          HAL_gpio_config(LCD_BACKLIGHT_PIN_NUM|PIN_FUNCOUTPUT|PIN_HIGH)
#define LCD_BACKLIGHT_OFF()         HAL_gpio_config(LCD_BACKLIGHT_PIN_NUM|PIN_FUNCOUTPUT|PIN_LOW)
#elif (LCD_BACKLIGHT_USE_PWM == 1)
#define LCD_BACKLIGHT_ON()          lcd_backlight_percent_on();
#define LCD_BACKLIGHT_OFF()         lcd_backlight_percent_off();
#else
#define LCD_BACKLIGHT_ON()          //HAL_gpio_config(LCD_BACKLIGHT_PIN_NUM|PIN_FUNCOUTPUT|PIN_HIGH)
#define LCD_BACKLIGHT_OFF()         //HAL_gpio_config(LCD_BACKLIGHT_PIN_NUM|PIN_FUNCOUTPUT|PIN_LOW)
#endif


typedef struct {
    uint16_t width;             //LCD width                 //液晶宽度
    uint16_t height;            //LCD height                //液晶高度
    uint8_t dir;                //vertical or horizontal    //垂直或水平
    uint8_t wrramcmd;           //start write gram CMD      //启动写gram命令
    uint8_t rdramcmd;           //start read gram CMD       //开始读取gram CMD
    uint8_t setxcmd;            //set X coordinate CMD      //设置X坐标CMD
    uint8_t setycmd;            //set Y coordinate CMD      //设置Y坐标CMD
    uint32_t forecolor;         //foreground color          //前景颜色
    uint32_t backcolor;         //background color          //背景颜色
} lcd_dev_type;

typedef struct {
    volatile uint32_t dma_write_byte;
    volatile uint32_t dma_trans_byte;
    volatile uint32_t dma_trans_max;
    uint8_t *draw_ptr;
    uint8_t *out_buf;
    bool big_end;
    bool dma_register;
	volatile bool trans_flag;
    volatile bool dma_kick;
    struct ring_buffer ringbuf;
} lcd_dma_ctrl_t;


enum lcdc_operating_mode_e {
    LCDC_OPERATING_8BIT,
    LCDC_OPERATING_3LINE_SPI,
    LCDC_OPERATING_4LINE_SPI,
};

typedef enum _lcd_disp_cmd{
    LCD_DISP_CMD_NONE = 0,
   
    LCD_DISP_CMD_LINK0_ENG,
    LCD_DISP_CMD_LINK1_ENG,
    LCD_DISP_CMD_LINK0_STA,
    LCD_DISP_CMD_LINK1_STA,

    LCD_DISP_CMD_PMU_PERCENT,
    LCD_DISP_CMD_VOLUME,
    LCD_DISP_ADD_SUB,
    LCD_DISP_SIGNAL,
}lcd_disp_cmd;
enum icon_update {
    ICON_UPDATE_NONE      = 0, // 无更新
    ICON_UPDATE_ENG0_LEVEL,  // 引擎0音量等级更新
    ICON_UPDATE_ENG1_LEVEL,  // 引擎1音量等级更新
    ICON_UPDATE_LINK0_STA,   // 连接0状态更新
    ICON_UPDATE_LINK1_STA,   // 连接1状态更新

    ICON_UPDATE_PMU_PERCENT,  // 电源管理单元电量百分比更新
    ICON_UPDATE_VOLUME,        // 音量更新
    ICON_UPDATE_ADD_SUB,       // 加减符号更新
    ICON_UPDATE_SIGNAL,        // 信号更新
};



uint8_t LCD_g24_get_channel_ID(void);

void lcd_backlight_percent_set(int32_t percent); // 设置LCD背光百分比
void lcd_controller_init(void); // LCD控制器初始化
void lcd_screen_init(uint8_t if_fill); // 液晶屏初始化，if_fill参数决定是否填充屏幕
void lcd_cmd_test(void); // LCD命令测试
void lcd_operation_mode_set(uint8_t mode); // 设置LCD操作模式
void lcd_chip_init(void); // LCD芯片初始化
uint32_t lcd_read_id(void); // 读取LCD的ID
uint32_t lcd_read_power_mode(void); // 读取LCD的电源模式
void lcd_reset(void); // 重置LCD
uint32_t lcd_sleep(void); // 使LCD进入睡眠模式
uint32_t lcd_weakup(void); // 唤醒LCD
void lcd_scan_dir(uint8_t dir); // 设置LCD的扫描方向
void lcd_display_setting(uint8_t dir); // 设置LCD的显示方向
void lcd_write_ram_prepare(void); // 准备写入LCD的RAM
void lcd_set_cursor(uint16_t sx, uint16_t ex, uint16_t sy, uint16_t ey); // 设置LCD的光标位置
void lcd_clear(uint32_t color); // 清除LCD显示，设置为指定颜色
bool lcd_dma_disp_flush(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t *buf,bool big_end); // 使用DMA刷新LCD显示
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex,uint16_t ey,uint32_t color); // 填充LCD区域
void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color); // 在LCD上绘制一个点
uint32_t lcd_read_point(uint16_t x, uint16_t y); // 读取LCD上某点的颜色
void lcd_show_char(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode); // 在LCD上显示一个字符
void lcd_show_string(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p,uint8_t mode); // 在LCD上显示一个字符串
void lcd_show_picture(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t *buf,bool big_end); // 在LCD上显示图片
void lcd_enter_sleep(void); // 进入睡眠模式
void lcd_exit_sleep(void); // 退出睡眠模式
uint16_t lcddev_height_get(void); // 获取LCD的高度
uint16_t lcddev_width_get(void); // 获取LCD的宽度
void lcdc_test(void); // LCDC测试
void lcd_flush_test(void); // LCD刷新测试
bool lcd_flush_test_flag_get(void); // 获取LCD刷新测试标志
void lcdc_init(lcdc_param_t *param); // 初始化LCDC，param为参数结构体指针
void lcdc_dma_mode_init(lcdc_dma_t *dma_param); // 初始化LCDC的DMA模式，dma_param为DMA参数结构体指针
void lcdc_rgb_change(uint8_t *in_buf, uint8_t *out_buf, uint32_t bytes); // RGB颜色转换
void lcdc_dma_callback_register(void); // 注册LCDC的DMA回调函数
void lcd_bootloader_disp(void); // 显示LCD引导程序

void lcd_te_rasing_process(void); // LCDTErase处理
void lcd_te_int_set(bool enable); // 设置LCDTE中断使能
void lcd_ground_color_set(uint32_t forecolor, uint32_t backcolor); // 设置LCD的前景色和背景色

void lcd_charging_show(void); // 显示充电状态
void lcd_poweron_show(uint8_t step); // 显示开机步骤
void lcd_disp_init(void); // 初始化LCD显示
int lcd_disp_xet_ctrl(uint32_t cmd, uint32_t val, uint32_t rev); // 控制LCD显示的额外命令
void lcd_mic_eng_flush(uint8_t index); // 刷新麦克风引擎状态
void lcd_link_status_flush(uint8_t index); // 刷新连接状态
void lcd_dir_change_set(uint8_t dir); // 设置LCD的方向

void lcd_pmu_percent_status_flush(uint8_t index); // 刷新电源管理单元百分比状态
void lcd_volume_status_flush(void); // 刷新音量状态
void lcd_add_sub_status_flush(uint8_t index); // 刷新附加/替代状态
void lcd_signal_status_flush(void); // 刷新信号状态


#endif

