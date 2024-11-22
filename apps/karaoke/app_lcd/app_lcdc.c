#define LOG_LEV 4

#include "user_config.h"
#include "sys_inc.h"

#if LCDC_EN
#include "hal_clk.h"
#include "hal_gpio.h"
#include "hal_lcdc.h"
#include "app_lcd_font.h"
#include "app_lcdc.h"
#include "hal_timestamp.h"
#include "oal_thread.h"
#include "hal_pwm.h"
#include "hal_power.h"
#include "amsg.h"
#include "system.h"
#include "mode_g24.h"
#include "event.h"
#include "amixer.h"
#include "hal_pmu.h"
#include "app_pmu.h"
#include "hal_uart.h"

lcd_dma_ctrl_t lcd_dma_ctrl;


#define LCD_DISP_TASK_NAME           "lcd_disp"
#define LCD_DISP_TASK_STACK_DEPTH    (768)
#define LCD_DISP_MSG_QUEUE_LENGTH    (10)
#define LCD_DISP_TASK_PRIO           (TRACK_TASK_PRIO)
#define LCD_DISP_MSG_QUEUE_LENGTH    (10)

#define LCD_ENG_LEVEL_WIDTH          (70)      //默认最大0db对应70
#define LCD_ENG_LEVEL_HEIGHT         (12)
#define LCD_DMA_BUF_SIZE             ((LCD_ENG_LEVEL_WIDTH* LCD_ENG_LEVEL_HEIGHT * sizeof(uint16_t)))     //适配eng显示，可根据最大dma长度配置

#define LCD_POWERON_LOGO_WIDTH       (64)
#define LCD_POWERON_LOGO_HEIGHT      (64)

#if EVB_LCD_DEMO

#if LCD_DIR_R90_CHANGE_SUPPORT
#define Image_smartlink_logo_16_16   gImage_smartlink_logo_R90_16_16
#define Image_smartlink_logo_32_32   gImage_smartlink_logo_R90_32_32
#define Image_smartlink_logo_64_64   gImage_smartlink_logo_R90_64_64
bool lcd_dir_change;

#else
#define Image_smartlink_logo_16_16   gImage_smartlink_logo_16_16
#define Image_smartlink_logo_32_32   gImage_smartlink_logo_32_32
#define Image_smartlink_logo_64_64   gImage_smartlink_logo_64_64
#endif

#endif



typedef struct _icon_param_t
{
    uint32_t val;
    uint32_t latest_val;
    uint8_t *color_buf;
    OAL_TIMER Timer;
} icon_param_t;

typedef struct _lcd_icon_t
{
    void * dma_buf;                 // DMA缓冲区指针，用于数据传输

    icon_param_t eng_level[2];     // 引擎状态图标参数数组，包含两个状态
    icon_param_t link_status[2];    // 连接状态图标参数数组，包含两个状态
    uint32_t update_flag;           // 更新标志位，指示图标是否需要更新
} lcd_icon_t;


typedef struct _lcd_disp_ctrl_t
{
    OAL_MUTEX cmd_lock;      // 命令锁，用于保护命令的互斥访问
    OAL_QUEUE cmd_queue;     // 命令队列，用于存储待处理的命令
    void*        amsg_comp;  // 消息组件指针，用于消息处理或传递
    OAL_THREAD task_handle;  // 任务句柄，表示LCD显示控制任务

    OAL_SEM disp_lock;       // 显示锁，确保显示操作的互斥访问

    lcd_icon_t icon;         // 当前显示的图标
} lcd_disp_ctrl_t;


lcd_disp_ctrl_t* lcd_disp_ctrl;

typedef struct _lcd_disp_t
{
    void *handler;
} lcd_t;

lcd_t lcd_disp;


#if (LCD_BACKLIGHT_USE_PWM == 1)
static int32_t g_cur_backlight_percent = 100;
#endif

#define MIN(a,b)    ((a) < (b) ? (a) : (b))

#if LCD_USE_TE
AT_SRAM(.gui_sram_seg)
void lcd_te_rasing_process(void)
{
    if(HAL_lcd_te_trigger_get() == true) {
        HAL_lcd_te_trigger_set(false);
    }
}

void lcd_te_int_set(bool enable)
{
    static bool irq_en_bak = 0;
    if(irq_en_bak != enable) {
        if(enable) {
            HAL_gpio_irq_config(LCD_TE_INT_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
            HAL_lcd_te_en_set(ENABLE);
        } else {
            HAL_gpio_irq_config(LCD_TE_INT_PIN_NUM, PIN_IRQ_DIS);
            HAL_lcd_te_en_set(DISABLE);
        }
        irq_en_bak = enable;
    }
}
#endif

//#if (LCDC_EN)

void lcdc_init(lcdc_param_t *param)
{
    HAL_lcdc_module_init(param);
    HAL_lcdc_module_io_setting(&param->io_ctrl);
}

void lcdc_dma_mode_init(lcdc_dma_t *dma_param)
{
    if(dma_param->rx_en) {
        HAL_lcdc_module_dma_rx_init(dma_param->lcdc_dma_rx_callback);
    }
    if(dma_param->tx_en) {
        HAL_lcdc_module_dma_tx_init(dma_param->lcdc_dma_tx_callback);
    }
}


lcd_dev_type lcddev;

void lcd_controller_init(void)
{
    lcdc_param_t lcdc_param;
    logi("%s",__func__);
    //pin cfg
    LCDC_X0_INIT();
    LCDC_X1_INIT();
    LCDC_X2_INIT();
    LCDC_X3_INIT();
    LCDC_X4_INIT();
    LCDC_X5_INIT();
    LCDC_X6_INIT();
    LCDC_X7_INIT();
    LCDC_X8_INIT();
    LCDC_X9_INIT();
    LCDC_X10_INIT();
    LCDC_X11_INIT();
    LCDC_X12_INIT();
    LCDC_X13_INIT();
    LCDC_X14_INIT();
    LCDC_X15_INIT();
    LCD_RESET_INIT();

    //io mapping
    lcdc_param.io_ctrl.io_mapping[0] = LCDC_X0_MAP;
    lcdc_param.io_ctrl.io_mapping[1] = LCDC_X1_MAP;
    lcdc_param.io_ctrl.io_mapping[2] = LCDC_X2_MAP;
    lcdc_param.io_ctrl.io_mapping[3] = LCDC_X3_MAP;
    lcdc_param.io_ctrl.io_mapping[4] = LCDC_X4_MAP;
    lcdc_param.io_ctrl.io_mapping[5] = LCDC_X5_MAP;
    lcdc_param.io_ctrl.io_mapping[6] = LCDC_X6_MAP;
    lcdc_param.io_ctrl.io_mapping[7] = LCDC_X7_MAP;
    lcdc_param.io_ctrl.io_mapping[8] = LCDC_X8_MAP;
    lcdc_param.io_ctrl.io_mapping[9] = LCDC_X9_MAP;
    lcdc_param.io_ctrl.io_mapping[10] = LCDC_X10_MAP;
    lcdc_param.io_ctrl.io_mapping[11] = LCDC_X11_MAP;
    lcdc_param.io_ctrl.io_mapping[12] = LCDC_X12_MAP;
    lcdc_param.io_ctrl.io_mapping[13] = LCDC_X13_MAP;
    lcdc_param.io_ctrl.io_mapping[14] = LCDC_X14_MAP;
    lcdc_param.io_ctrl.io_mapping[15] = LCDC_X15_MAP;
    lcdc_param.io_ctrl.io_en[0] = !!LCDC_X0_SEL;
    lcdc_param.io_ctrl.io_en[1] = !!LCDC_X1_SEL;
    lcdc_param.io_ctrl.io_en[2] = !!LCDC_X2_SEL;
    lcdc_param.io_ctrl.io_en[3] = !!LCDC_X3_SEL;
    lcdc_param.io_ctrl.io_en[4] = !!LCDC_X4_SEL;
    lcdc_param.io_ctrl.io_en[5] = !!LCDC_X5_SEL;
    lcdc_param.io_ctrl.io_en[6] = !!LCDC_X6_SEL;
    lcdc_param.io_ctrl.io_en[7] = !!LCDC_X7_SEL;
    lcdc_param.io_ctrl.io_en[8] = !!LCDC_X8_SEL;
    lcdc_param.io_ctrl.io_en[9] = !!LCDC_X9_SEL;
    lcdc_param.io_ctrl.io_en[10] = !!LCDC_X10_SEL;
    lcdc_param.io_ctrl.io_en[11] = !!LCDC_X11_SEL;
    lcdc_param.io_ctrl.io_en[12] = !!LCDC_X12_SEL;
    lcdc_param.io_ctrl.io_en[13] = !!LCDC_X13_SEL;
    lcdc_param.io_ctrl.io_en[14] = !!LCDC_X14_SEL;
    lcdc_param.io_ctrl.io_en[15] = !!LCDC_X15_SEL;

#if LCDC_INTERFACE      //spi
    lcdc_param.clk_sorce = CLK_96M;
    lcdc_param.clkdiv = 2;
    lcdc_param.lcdc_interface = LCDC_INTERFACE_SPI;
#if LCDC_SPI_INTERFACE
    lcdc_param.spi_ctrl.spi_interface = LCDC_SPI_INTERFACE_II;
#else
    lcdc_param.spi_ctrl.spi_interface = LCDC_SPI_INTERFACE_I;
#endif
    lcdc_param.spi_ctrl.rx_phase = LCDC_SPI_RX_PHASE_0;
    lcdc_param.spi_ctrl.cpha = LCDC_SPI_PHASE_0;
    lcdc_param.spi_ctrl.cpol = LCDC_SPI_ACTIVE_POLARITY_HIGH;
    lcdc_param.spi_ctrl.data_lane = LCDC_SPI_1DATA_LANE;
    lcdc_param.spi_ctrl.dummy_cycle = 1;

#if LCDC_SPI_LINE_SEL
    lcdc_param.spi_ctrl.line = LCDC_SPI_3LINE;
    lcd_operation_mode_set(LCDC_OPERATING_3LINE_SPI);
#else
    lcdc_param.spi_ctrl.line = LCDC_SPI_4LINE;
    lcd_operation_mode_set(LCDC_OPERATING_4LINE_SPI);
#endif

#else                   //8080
    lcdc_param.clk_sorce = CLK_96M;
    lcdc_param.clkdiv = 3;
    lcdc_param.lcdc_interface = LCDC_INTERFACE_8080;
    lcdc_param.ctrl_8080.rdx_h_cycle = LCDC_8080_HL_CYCLE_1;
    lcdc_param.ctrl_8080.rdx_l_cycle = LCDC_8080_HL_CYCLE_1;
    lcdc_param.ctrl_8080.wrx_h_cycle = LCDC_8080_HL_CYCLE_1;
    lcdc_param.ctrl_8080.wrx_l_cycle = LCDC_8080_HL_CYCLE_1;
    lcd_operation_mode_set(LCDC_OPERATING_8BIT);
#endif
    lcdc_param.dma_param.rx_en = DISABLE;
    lcdc_param.dma_param.tx_en = DISABLE;
    lcdc_param.dma_param.lcdc_dma_rx_callback = NULL;
    lcdc_param.dma_param.lcdc_dma_tx_callback = NULL;
#if LCDC_DMA_RGB_CHAGE
    lcd_dma_ctrl.out_buf  = (uint8_t *)libc_malloc(DMA_RING_BUF_SIZE);
#endif

    lcdc_param.lcd_api.dma_disp_flush      = NULL;//lcd_dma_disp_flush;
    lcdc_param.lcd_api.disp_flush          = lcd_show_picture;
    lcdc_param.lcd_api.lcd_clear           = lcd_clear;
    lcdc_param.lcd_api.bootloader_disp     = lcd_bootloader_disp;

    lcdc_init(&lcdc_param);
    lcdc_dma_mode_init(&lcdc_param.dma_param);
    
    HAL_lcd_api_register(&lcdc_param.lcd_api);
#if LCD_USE_TE
    HAL_gpio_func_set(LCD_TE_INT_PIN_NUM, PIN_FUNCEINT);
    HAL_gpio_irq_clk_config(LCD_TE_INT_PIN_NUM, (0x00<<8)|(0x1<<7)|(0x00<<4));
    lcd_te_int_set(ENABLE);
#endif
#if EVB_LCD_DEMO
    lcdc_dma_callback_register();
#endif
}

void lcdc_dma_callback_register(void)
{
    lcdc_dma_t dma_param;
    dma_param.rx_en = ENABLE;
    dma_param.tx_en = ENABLE;
    dma_param.lcdc_dma_rx_callback = lcdc_dma_rx_callback;
    dma_param.lcdc_dma_tx_callback = lcdc_dma_tx_callback;
    lcdc_dma_mode_init(&dma_param);
    lcd_dma_ctrl.dma_register = true;
}

AT_SRAM(.lcdc)
void lcdc_dma_rx_callback(void)
{
    //logi("lcdc rx cb");
}

AT_SRAM(.lcdc)
void lcdc_dma_tx_callback(void)
{
    //logi("lcdc tx cb");
    uint8_t * wptr;
    lcd_disp_ctrl_t *disp = lcd_disp_ctrl;

    if(lcd_dma_ctrl.big_end) {
        lcd_dma_ctrl.dma_write_byte -= lcd_dma_ctrl.dma_trans_byte;
        lcd_dma_ctrl.draw_ptr += lcd_dma_ctrl.dma_trans_byte;
        if(lcd_dma_ctrl.dma_write_byte) {
            lcd_dma_ctrl.dma_trans_byte = MIN(lcd_dma_ctrl.dma_write_byte,lcd_dma_ctrl.dma_trans_max);
            wptr = lcd_dma_ctrl.draw_ptr;
            HAL_lcdc_dma_write(0x00,0,wptr,lcd_dma_ctrl.dma_trans_byte);
        } else {
            //lv_disp_flush_dma_ready();
			lcd_dma_ctrl.trans_flag = false;

            if(disp->disp_lock) {
                lcd_dir_change_set(0);
                OAL_sem_post(&disp->disp_lock);
            }
        }
    }
#if LCDC_DMA_RGB_CHAGE
    else {
        uint32_t avail;
        ring_buffer_update_rpos(&lcd_dma_ctrl.ringbuf,lcd_dma_ctrl.dma_trans_byte);
        ring_buffer_get_r_block(&lcd_dma_ctrl.ringbuf,&avail,&wptr);
        if(avail>=4) {
            avail = MIN(avail,lcd_dma_ctrl.dma_trans_max);
            lcd_dma_ctrl.dma_trans_byte = avail;
            HAL_lcdc_dma_write(0x00,0,wptr,lcd_dma_ctrl.dma_trans_byte);
        } else {
            if(lcd_dma_ctrl.dma_write_byte) {
                lcd_dma_ctrl.dma_kick = 1;
            } else {
                //lv_disp_flush_dma_ready();
				lcd_dma_ctrl.trans_flag = false;

                if(disp->disp_lock) {
                    lcd_dir_change_set(0);
                    OAL_sem_post(&disp->disp_lock);
                }
            }
        }
    }
#endif
}


void lcd_operation_mode_set(uint8_t mode)
{
#if (LCD_CHIP_SEL == LCD_CHIP_ILI9488)
    uint8_t operating_mode = 0;
    switch(mode) {                                  //IM0~2 setting
    case LCDC_OPERATING_8BIT:
        operating_mode = ILI9488_OPERATING_8BIT;
        break;
    case LCDC_OPERATING_3LINE_SPI:
        operating_mode = ILI9488_OPERATING_3LINE_SPI;
        break;
    case LCDC_OPERATING_4LINE_SPI:
        operating_mode = ILI9488_OPERATING_4LINE_SPI;
        break;
    }
    HAL_gpio_config(LCDC_IM0_PIN_NUM|PIN_FUNCOUTPUT);
    HAL_gpio_config(LCDC_IM1_PIN_NUM|PIN_FUNCOUTPUT);
    HAL_gpio_config(LCDC_IM2_PIN_NUM|PIN_FUNCOUTPUT);
    HAL_gpio_value_set(LCDC_IM0_PIN_NUM,(operating_mode>>0)&0x1);
    HAL_gpio_value_set(LCDC_IM1_PIN_NUM,(operating_mode>>1)&0x1);
    HAL_gpio_value_set(LCDC_IM2_PIN_NUM,(operating_mode>>2)&0x1);
#endif
}

//屏相关初始化
void lcd_screen_init(uint8_t if_fill)
{
    lcd_reset();
    lcd_chip_init();
    
    lcd_clear(BLACK);

#if !EVB_LCD_DEMO
    LCD_BACKLIGHT_ON();
#endif

#if LCDC_TEST_EN
    lcdc_test();
#endif
}

void lcd_chip_init(void)
{
#if (LCD_CHIP_SEL == LCD_CHIP_ILI9488)
    ili9488_lcd_init();
#elif (LCD_CHIP_SEL == LCD_CHIP_GC9A01)
    gc9a01_lcd_init();
#elif (LCD_CHIP_SEL == LCD_CHIP_ST7789V2_IG03)
    st7789v2_lcd_init();
#elif (LCD_CHIP_SEL == LCD_CHIP_ST7789S)
    st7789s_lcd_init();
#elif (LCD_CHIP_SEL == LCD_CHIP_ST7735S)
#if LCDC_INTERFACE
    st7735s_spi_lcd_init();
#else
    st7735s_lcd_init();
#endif
#endif
    lcd_display_setting(LCD_DFT_DIR);
    lcd_clear(BLACK);
    lcd_exit_sleep();
}

uint32_t lcd_read_id(void)
{
    uint32_t id = 0;
#if (LCD_CHIP_SEL == LCD_CHIP_ILI9488)
    id = ili9488_read_id();
#elif (LCD_CHIP_SEL == LCD_CHIP_GC9A01)
    id = gc9a01_read_id();
#elif (LCD_CHIP_SEL == LCD_CHIP_ST7789S)
    id = st7789s_read_id();
#elif (LCD_CHIP_SEL == LCD_CHIP_ST7735S)
    //id = st7735s_read_id();
#endif
    return id;
}
uint32_t lcd_read_power_mode(void)
{
    uint32_t mode = 0;
#if (LCD_CHIP_SEL == LCD_CHIP_ILI9488)
    //id = ili9488_read_id();
#elif (LCD_CHIP_SEL == LCD_CHIP_GC9A01)
    //id = gc9a01_read_id();
#elif (LCD_CHIP_SEL == LCD_CHIP_ST7789S)
    mode = st7789s_read_power_mode();
#elif (LCD_CHIP_SEL == LCD_CHIP_ST7735S)
    mode = st7735s_read_power_mode();
#endif
    return mode;
}

uint32_t lcd_sleep(void)
{
    uint32_t id = 0;
#if (LCD_CHIP_SEL == LCD_CHIP_ILI9488)

#elif (LCD_CHIP_SEL == LCD_CHIP_GC9A01)

#elif (LCD_CHIP_SEL == LCD_CHIP_ST7789S)
    id = st7789s_tosleep();
#elif (LCD_CHIP_SEL == LCD_CHIP_ST7735S)
    id = st7735s_tosleep();
#endif
    return id;
}

uint32_t lcd_weakup(void)
{
    uint32_t id = 0;
#if (LCD_CHIP_SEL == LCD_CHIP_ILI9488)

#elif (LCD_CHIP_SEL == LCD_CHIP_GC9A01)

#elif (LCD_CHIP_SEL == LCD_CHIP_ST7789S)
    id = st7789s_weakup();
#elif (LCD_CHIP_SEL == LCD_CHIP_ST7735S)
    id = st7735s_weakup();
#endif
    return id;
}

void lcd_reset(void)
{
    LCD_RESET_REL();
    HAL_mdelay(50);
    LCD_RESET();
    HAL_mdelay(50);
    LCD_RESET_REL();
    HAL_mdelay(120);
}

void lcd_scan_dir(uint8_t dir)
{
    uint16_t regval = 0;
    uint16_t dirreg = 0;
    uint16_t temp;
    if(lcddev.dir == LCD_DIR_HORIZONTAL) {
        switch(dir) {
        case L2R_U2D: dir = D2U_L2R; break;
        case L2R_D2U: dir = D2U_R2L; break;
        case R2L_U2D: dir = U2D_L2R; break;
        case R2L_D2U: dir = U2D_R2L; break;
        case U2D_L2R: dir = L2R_D2U; break;
        case U2D_R2L: dir = L2R_U2D; break;
        case D2U_L2R: dir = R2L_D2U; break;
        case D2U_R2L: dir = R2L_U2D; break;
        }
    }

    switch(dir) {
    case L2R_U2D:
        regval|=(0<<7)|(0<<6)|(0<<5);
        break;
    case L2R_D2U:
        regval|=(1<<7)|(0<<6)|(0<<5);
        break;
    case R2L_U2D:
        regval|=(0<<7)|(1<<6)|(0<<5);
        break;
    case R2L_D2U:
        regval|=(1<<7)|(1<<6)|(0<<5);
        break;
    case U2D_L2R:
        regval|=(0<<7)|(0<<6)|(1<<5);
        break;
    case U2D_R2L:
        regval|=(0<<7)|(1<<6)|(1<<5);
        break;
    case D2U_L2R:
        regval|=(1<<7)|(0<<6)|(1<<5);
        break;
    case D2U_R2L:
        regval|=(1<<7)|(1<<6)|(1<<5);
        break;
    }
#if (LCD_CHIP_SEL != LCD_CHIP_ST7789V2_IG03) && (LCD_CHIP_SEL != LCD_CHIP_ST7789S)
    regval|=0X08;                               //BGR color filter panel
#endif
    dirreg = 0x36;                              //Memory Access Control
    HAL_lcdc_module_write_reg(dirreg);
    HAL_lcdc_module_write_data(regval,1);
    //logi("##0x36 = %#x", regval);

#if LCD_DIR_R90_CHANGE_SUPPORT
    lcd_dir_change = 0;
#endif

    if(regval & 0x20) {                         //Row/Column Exchange
        if(lcddev.width<lcddev.height) {
            temp=lcddev.width;
            lcddev.width=lcddev.height;
            lcddev.height=temp;
        }
    } else {
        if(lcddev.width>lcddev.height) {
            temp=lcddev.width;
            lcddev.width=lcddev.height;
            lcddev.height=temp;
        }
    }
}

AT_SRAM(.lcdc)
void lcd_ground_color_set(uint32_t forecolor, uint32_t backcolor)
{
    lcddev.forecolor = forecolor;
    lcddev.backcolor = backcolor;
}

void lcd_display_setting(uint8_t dir)
{
    lcddev.dir = dir;
    if(lcddev.dir == LCD_DIR_VERTICAL) {
        lcddev.width = LCD_WIDTH;
        lcddev.height = LCD_HEIGHT;
    } else {
        lcddev.width = LCD_HEIGHT;
        lcddev.height = LCD_WIDTH;
    }
    lcddev.wrramcmd = LCD_WR_RAM_CMD;
    lcddev.rdramcmd = LCD_RD_RAM_CMD;
    lcddev.setxcmd = LCD_SET_X_CMD;
    lcddev.setycmd = LCD_SET_Y_CMD;
    lcddev.backcolor = WHITE;
    lcddev.forecolor = BLACK;
    lcd_scan_dir(LCD_DFT_SCAN_DIR);         //set the default scan dir
}

AT_SRAM(.lcdc)
void lcd_write_ram_prepare(void)
{
    HAL_lcdc_module_write_reg(lcddev.wrramcmd);
}

AT_SRAM(.lcdc)
void lcd_set_cursor(uint16_t sx, uint16_t ex, uint16_t sy, uint16_t ey)
{
    uint16_t sx_enlarge = 0, ex_enlarge = 0, sy_enlarge = 0, ey_enlarge = 0;

#if ((LCD_CHIP_SEL == LCD_CHIP_ST7735S) && LCDC_INTERFACE)

#if LCD_DFT_DIR
    sy_enlarge = 24;
    ey_enlarge = 24;
#else
    sx_enlarge = 24;
    ex_enlarge = 24;
#endif

#elif (LCD_CHIP_SEL == LCD_CHIP_ST7789V2_IG03)
    sy_enlarge = 20;
    ey_enlarge = 20;
#endif


#if LCD_DIR_R90_CHANGE_SUPPORT
    if(lcd_dir_change) {
        //这里只支持了竖屏向右旋转90度的坐标对应关系，其他组合需重新折算
        uint16_t sx_temp = sx;
        uint16_t ex_temp = ex;
        sx = lcddev.height - 1 - ey;
        ex = lcddev.height - 1 - sy;
        sy = sx_temp;
        ey = ex_temp;

#if ((LCD_CHIP_SEL == LCD_CHIP_ST7735S) && LCDC_INTERFACE && LCD_DFT_DIR) 
        //按照竖屏时的配置
        sx_enlarge = 24;
        ex_enlarge = 24;
        sy_enlarge = 0;
        ey_enlarge = 0;
#endif
    }
#endif

    HAL_lcdc_module_write_reg(lcddev.setxcmd);
    HAL_lcdc_module_write_data(sx>>8,1);
    HAL_lcdc_module_write_data((sx&0XFF)+sx_enlarge,1);
    HAL_lcdc_module_write_data(ex>>8,1);
    HAL_lcdc_module_write_data((ex&0XFF)+ex_enlarge,1);

    HAL_lcdc_module_write_reg(lcddev.setycmd);
    HAL_lcdc_module_write_data(sy>>8,1);
    HAL_lcdc_module_write_data((sy&0XFF)+sy_enlarge,1);          //纵坐标需要整体+24
    HAL_lcdc_module_write_data(ey>>8,1);
    HAL_lcdc_module_write_data((ey&0XFF)+ey_enlarge,1);          //纵坐标需要整体+24

    

}

/**
 * @brief:  填充整个屏幕
 * @param:  color  要填充的颜色
 */
AT_SRAM(.lcdc)
void lcd_clear(uint32_t color)
{
    uint32_t totalpoint=lcddev.width;
    totalpoint*=lcddev.height;
    lcd_set_cursor(0,lcddev.width-1,0,lcddev.height-1);
    lcd_write_ram_prepare();
    color = RGB_BEND(color);
    for(uint32_t index=0;index<totalpoint;index++) {
#if (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB565)
        HAL_lcdc_module_write_data(color,2);
#elif (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB666)
        HAL_lcdc_module_write_data(color,3);
#endif
    }
}

/**
 * @brief:  在指定区域内填充指定颜色块
 * @param:  (sx,sy),(ex,ey): 填充矩形对角线，区域大小为:(ex-sx+1)*(ey-sy+1)
 * @param:  color 要填充的颜色
 */
AT_SRAM(.lcdc)
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex,uint16_t ey,uint32_t color)
{
    uint32_t totalpoint=(ex-sx+1);
    totalpoint*=(ey-sy+1);
    lcd_set_cursor(sx,ex,sy,ey);
    lcd_write_ram_prepare();
    color = RGB_BEND(color);
    for(uint32_t index=0;index<totalpoint;index++) {
#if (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB565)
        HAL_lcdc_module_write_data(color,2);
#elif (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB666)
        HAL_lcdc_module_write_data(color,3);
#endif
    }
}

/**
 * @brief:  画点
 * @param:  (x,y):点坐标
 * @param:  color 点颜色
 */
AT_SRAM(.lcdc)
void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color)
{
    lcd_set_cursor(x, x, y, y);
    lcd_write_ram_prepare();
    color = RGB_BEND(color);
#if (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB565)
    HAL_lcdc_module_write_data(color,2);
#elif (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB666)
    HAL_lcdc_module_write_data(color,3);
#endif
}

/**
 * @brief:  读点
 * @param:  (x,y):点坐标
 */
AT_SRAM(.lcdc)
uint32_t lcd_read_point(uint16_t x, uint16_t y)
{
    uint8_t r,g,b;
    uint32_t color;
    if(x>=lcddev.width||y>=lcddev.height)return 0;
    lcd_set_cursor(x, x, y, y);
#if (LCD_CHIP_SEL == LCD_CHIP_ILI9488)&&(LCDC_INTERFACE && !LCDC_SPI_LINE_SEL)                   //spi 4line
    HAL_lcdc_module_spi_dummy_cycle_set(0);
#endif

#if (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB565)
    HAL_lcdc_module_read(lcddev.rdramcmd,1,(uint8_t *)&color,4);        //the first byte is dummy
    logi("RGB565 lcdc_read color:0x%x",color);
    r = (color&0xf800)>>11;
    g = (color&0xfc0000)>>18;
    b = (color&0xf8000000)>>27;
    color = (r<<11)|(g<<5)|(b);                              //RGB565
#elif (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB666)
    HAL_lcdc_module_read(lcddev.rdramcmd,1,(uint8_t *)&color,4);        //the first byte is dummy
    logi("RGB666 lcdc_read color:0x%x",color);
    r = (color&0xff00)>>8;
    g = (color&0xff0000)>>16;
    b = (color&0xff000000)>>24;
    color = (r<<16)|(g<<8)|(b);                              //RGB666
#endif
#if (LCD_CHIP_SEL == LCD_CHIP_ILI9488)&&(LCDC_INTERFACE && !LCDC_SPI_LINE_SEL)                   //spi 4line
    HAL_lcdc_module_spi_dummy_cycle_set(1);
#endif
    return color;
}


/**
 * @brief:  lcd show char
 * @param:  x  X 坐标
 * @param:  y  Y 坐标
 * @param:  num  字符:" "--->"~"
 * @param:  size 字体大小:12/16/24/32/60
 * @param:  mode  show mode:0:非叠加显示, 1:叠加显示
 */
AT_SRAM(.lcdc)
void lcd_show_char(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode)
{
    uint8_t temp,t1,t;
    uint16_t y0=y;
    uint8_t csize=(size/8+((size%8)?1:0))*(size/2);
    num=num-' ';
    for(t=0;t<csize;t++) {
        if(size==12)temp=asc2_1206[num][t];
        else if(size==16)temp=asc2_1608[num][t];
        else if(size==24)temp=asc2_2412[num][t];
        else if(size==32)temp=asc2_4824[num][t];
        else if(size==60)temp=asc2_6848[num][t];
        else return;
        for(t1=0;t1<8;t1++) {
            if(temp&0x80)lcd_draw_point(x,y,lcddev.forecolor);
            else if(mode==0)lcd_draw_point(x,y,lcddev.backcolor);
            temp<<=1;
            y++;
            if(y>=lcddev.height)return;
            if((y-y0)==size) {
                y=y0;
                x++;
                if(x>=lcddev.width)return;
                break;
            }
        }
     }
}

/**
 * @brief:  显示字符串
 * @param:  (x,y):起始坐标
 * @param:  width:字符串区域宽度
 * @param:  height:字符串区域高度
 * @param:  size:字体大小:12/16/24/32/60
 * @param:  p:字符串
 * @param:  mode  show mode:0:非叠加显示, 1:叠加显示
 */
AT_SRAM(.lcdc)
void lcd_show_string(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p,uint8_t mode)
{
    uint16_t x0 = x;
    width += x;
    height += y;
    while((*p<='~')&&(*p>=' ')) {
        if(x >= width){x = x0;y += size;}
        if(y >= height)break;
        lcd_show_char(x,y,*p,size,mode);
        x += size/2;
        p++;
    }
}

#if 0

/*********just for test ********/
AT_SRAM(.lcdc)
void  run_rest(uint32_t data, uint8_t len)
{
     ;
}

#endif
extern void  run_rest(uint32_t data, uint8_t len);
/**
 * @brief:  显示图片/填充区域
 * @param:  (x,y):起始坐标
 * @param:  width:图片宽度
 * @param:  height:图片高度
 * @param:  buf:图片数据
 * @param:  big_end:输入buf是否是大端模式
 */
AT_SRAM(.lcdc)
void lcd_show_picture(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t *buf,bool big_end)
{
    uint8_t data_size = 0;
    uint32_t rgb_big_end = 0;
    uint32_t *draw_ptr = (uint32_t *)buf;
    uint32_t picture_size = width*height;
    if((width>lcddev.width)||(0==lcddev.width)||(height>lcddev.height)||(0==lcddev.height)) {
        return;
    }

    //xprintf("cursor  indriver x=%d y=%d width=%d height=%d \n",x,y,width,height);

    width = x+width-1;
    height = y+height-1;

    lcd_set_cursor(x,width,y,height);

    lcd_write_ram_prepare();
#if (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB565)
    data_size = 2;
#elif (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB666)
    data_size = 3;
#endif
    for(uint32_t index=0;index<picture_size;index++) {
        draw_ptr = (uint32_t *)&buf[data_size*index];
        if(big_end) {                                   //当buf数据已经是大端模式时不需要转换
            rgb_big_end = *draw_ptr;
        } else {
            rgb_big_end = RGB_BEND(*draw_ptr);
        }
        HAL_lcdc_module_write_data(rgb_big_end,data_size);
    }
}

AT_SRAM(.lcdc)
void lcdc_rgb_change(uint8_t *in_buf, uint8_t *out_buf, uint32_t bytes)
{
    if(bytes == 0) {
        return;
    }
    uint8_t data_size = 0;
    uint32_t color_change = 0;
    uint32_t pixel = 0;
    uint32_t *draw_ptr = (uint32_t *)in_buf;
#if (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB565)
    data_size = 2;
#elif (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB666)
    data_size = 3;
#endif
    pixel = bytes/data_size;
    for(uint32_t index=0;index<pixel;index++) {
        draw_ptr = (uint32_t *)&in_buf[data_size*index];
        color_change = RGB_BEND(*draw_ptr);
        memcpy((uint8_t*)&out_buf[data_size*index],(uint8_t*)&color_change,data_size);
    }
}

AT_SRAM(.lcdc)
bool lcd_dma_disp_flush(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t *buf,bool big_end)		//ret 0:传输完成，1:DMA工作
{
//    if((width>lcddev.width)||(0==lcddev.width)||(height>lcddev.height)||(0==lcddev.height)) {
//        logi("#RT0:%d, %d, %d, %d", width, lcddev.width, height, lcddev.height);
//        return;
//    }

    if(lcd_dma_ctrl.dma_register == false) {
        return 0;
    }

    while(lcd_dma_ctrl.trans_flag);

#if (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB565)
    uint32_t write_byte = width*height*2;
    uint8_t write_remain = 4;
    uint8_t data_size = 2;
    lcd_dma_ctrl.dma_trans_max = 4096;
#elif (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB666)
    uint32_t write_byte = width*height*3;
    uint8_t write_remain = 12;
    uint8_t data_size = 3;
    lcd_dma_ctrl.dma_trans_max = 4092;
#endif
    uint32_t byte_index = 0;
    lcd_dma_ctrl.draw_ptr = &buf[byte_index];
    lcd_dma_ctrl.trans_flag = true;
    uint32_t rgb_big_end = 0;
    uint8_t *wptr;

    width = x+width-1;
    height = y+height-1;

    lcd_set_cursor(x,width,y,height);
    lcd_write_ram_prepare();


    while(write_byte%write_remain) {
        if(big_end == 0) {
            lcdc_rgb_change(lcd_dma_ctrl.draw_ptr,(uint8_t*)&rgb_big_end,data_size);
        } else {
            memcpy((uint8_t*)&rgb_big_end,lcd_dma_ctrl.draw_ptr,data_size);
        }
        HAL_lcdc_module_write_data(rgb_big_end,data_size);

        byte_index += data_size;
        write_byte -= data_size;
        lcd_dma_ctrl.draw_ptr = &buf[byte_index];
    }

    lcd_dma_ctrl.dma_write_byte = write_byte;
    lcd_dma_ctrl.big_end = big_end;

    if(lcd_dma_ctrl.dma_write_byte==0) {
        //lv_disp_flush_dma_ready();
		lcd_dma_ctrl.trans_flag = false;
        return 0;
    }

    if(lcd_dma_ctrl.big_end) {
        if(lcd_dma_ctrl.dma_write_byte) {
            lcd_dma_ctrl.dma_trans_byte = MIN(lcd_dma_ctrl.dma_write_byte,lcd_dma_ctrl.dma_trans_max);
            wptr = lcd_dma_ctrl.draw_ptr;
            HAL_lcdc_dma_write(0x00,0,wptr,lcd_dma_ctrl.dma_trans_byte);
        }
    }
#if LCDC_DMA_RGB_CHAGE
    else {
        uint32_t avail;
        lcd_dma_ctrl.dma_kick = true;

        ring_buffer_init(&lcd_dma_ctrl.ringbuf,lcd_dma_ctrl.out_buf,DMA_RING_BUF_SIZE-DMA_RING_BUF_SIZE%write_remain);

        while(lcd_dma_ctrl.dma_write_byte) {
            ring_buffer_get_w_block(&lcd_dma_ctrl.ringbuf,&avail,&wptr);
            if(avail>=write_remain) {
                avail = MIN(avail,lcd_dma_ctrl.dma_trans_max);
                avail = MIN(avail,lcd_dma_ctrl.dma_write_byte);
                avail = avail - avail%write_remain;
                lcdc_rgb_change(lcd_dma_ctrl.draw_ptr,wptr,avail);
                ring_buffer_update_wpos(&lcd_dma_ctrl.ringbuf,avail);
                lcd_dma_ctrl.dma_write_byte -= avail;
                lcd_dma_ctrl.draw_ptr += avail;
            }
            if(lcd_dma_ctrl.dma_kick) {
                ring_buffer_get_r_block(&lcd_dma_ctrl.ringbuf,&avail,&wptr);
                if(avail>=4) {
                    avail = MIN(avail,lcd_dma_ctrl.dma_trans_max);
                    lcd_dma_ctrl.dma_kick = false;
                    lcd_dma_ctrl.dma_trans_byte = avail;
                    HAL_lcdc_dma_write(0x00,0,wptr,lcd_dma_ctrl.dma_trans_byte);
                }
            }
        }
    }
#endif
    return 1;
}

AT_SRAM(.lcdc)
void lcd_enter_sleep(void)
{
    logi("lcd_enter_sleep");
    HAL_lcdc_module_write_reg(0x28);
    HAL_mdelay(120);
    HAL_lcdc_module_write_reg(0x10);
    HAL_mdelay(50);
}

AT_SRAM(.lcdc)
void lcd_exit_sleep(void)
{
    logi("lcd_exit_sleep");
    // HAL_lcdc_module_write_reg(0x11);
    HAL_mdelay(120);
    // HAL_lcdc_module_write_reg(0x29);
}

AT_SRAM(.lcdc)
uint16_t lcddev_height_get(void)
{
    return lcddev.height;
}

AT_SRAM(.lcdc)
uint16_t lcddev_width_get(void)
{
    return lcddev.width;
}

AT_SRAM(.lcdc)
void lcd_bootloader_disp(void)
{
#if 0
    // LCD init
    HAL_gpio_config(PIN_D0 | PIN_FUNCDISABLE);
    lcd_controller_init();
    lcd_screen_init(1);
    Debug_printf(disp_K, "file open pass!\r\n");
    // lcd_show_string(30,40,204,24,24,(uint8_t *)"Please wait...",0);
#endif
}

#if (LCD_BACKLIGHT_USE_PWM == 1)
AT_SRAM(.lcdc)
void lcd_backlight_percent_set(int32_t percent)
{
	uint16_t active_cycle;
	YP3_NV_VALUE value;

	if(percent > 100)
		percent = 100;

	if(percent < 0)
		percent = 0;

	active_cycle = 48000 / 100 * percent;


	HAL_pwm_config(PWM_CH3, 48000, active_cycle);

	g_cur_backlight_percent = percent;
	value.type = YP3_NV_BACKLIGHT;
	value.v.value_int = percent;
	value.length = 0;
	yp3_nv_set(YP3_NV_BACKLIGHT, value);
}

AT_SRAM(.lcdc)
void lcd_backlight_percent_on(void)
{
	uint16_t active_cycle;

	active_cycle = (uint16_t)g_cur_backlight_percent * 480;
	HAL_pwm_config(PWM_CH3, 48000, active_cycle);
}

AT_SRAM(.lcdc)
void lcd_backlight_percent_off(void)
{
	HAL_pwm_config(PWM_CH3, 48000, 0);
}

AT_SRAM(.lcdc)
void lcd_backlight_percent_init(void)
{
	YP3_NV_VALUE value;

	yp3_nv_get(YP3_NV_BACKLIGHT, &value);
	g_cur_backlight_percent = value.v.value_int;
	lcd_backlight_percent_on();
}
#endif

void lcd_mic_eng_flush(uint8_t index) 
{
    if(index) {
        lcd_disp_xet_ctrl(LCD_DISP_CMD_LINK1_ENG, mic_eng_lvl_get(index), 0);
    } else {
        lcd_disp_xet_ctrl(LCD_DISP_CMD_LINK0_ENG, mic_eng_lvl_get(index), 0);
    }
}

AT_SRAM(.lcdc)
void lcd_eng_level_timer_cb(void* xTimer)
{
    g24_mode_t *g24_mode = g24_mode_param_get();
    if(g24_mode->connect[0]) {
        event_put(G24_EVENT_ENG0_UPDATE, NULL, 0);
    }
#if G24_DUAL_CHANNEL
    if(g24_mode->connect[1]) {
        event_put(G24_EVENT_ENG1_UPDATE, NULL, 0);
    }
#endif
}

void lcd_link_status_flush(uint8_t index) 
{
    g24_mode_t *g24_mode = g24_mode_param_get();
    if(index) {
#if G24_DUAL_CHANNEL
        lcd_disp_xet_ctrl(LCD_DISP_CMD_LINK1_STA, g24_mode->connect[1], 0);
#endif
    } else {
        lcd_disp_xet_ctrl(LCD_DISP_CMD_LINK0_STA, g24_mode->connect[0], 0);
    }
}

void lcd_pmu_percent_status_flush(uint8_t index)
{
    lcd_disp_xet_ctrl(LCD_DISP_CMD_PMU_PERCENT,0,0);
}

void lcd_add_sub_status_flush(uint8_t index)
{
    lcd_disp_xet_ctrl(LCD_DISP_ADD_SUB,index,0);
}

void lcd_signal_status_flush(void)
{
    lcd_disp_xet_ctrl(LCD_DISP_SIGNAL,0,0);
}

void lcd_volume_status_flush(void)
{
    lcd_disp_xet_ctrl(LCD_DISP_CMD_VOLUME,0,0);   
}

AT_SRAM(.lcdc)
void lcd_link_status_timer_cb(void* xTimer)
{
    g24_mode_t *g24_mode = g24_mode_param_get();
    if(g24_mode->connect[0] == 0) {
        event_put(G24_EVENT_LINK0_STA_UPDATE, NULL, 0);
    }
#if G24_DUAL_CHANNEL
    if(g24_mode->connect[1] == 0) {
        event_put(G24_EVENT_LINK1_STA_UPDATE, NULL, 0);
    }
#endif
}

AT_SRAM(.lcdc)
int lcd_disp_cmd_process(lcd_disp_ctrl_t *disp, uint32_t wait_ticks)
{
    amsg_t msg;
    logi("OAL_queue_recv");
    if (OAL_SUCCESS != OAL_queue_recv(&disp->cmd_queue, &msg, wait_ticks)) {
        logi("OAL_queue_recv");
        return -1;
    }
    switch (msg.cmd) {
#if EVB_LCD_DEMO  
        case LCD_DISP_CMD_LINK0_ENG:
        {
            amsg_t* rply = (amsg_t*)msg.user_data;
            uint32_t val = rply->req;
            disp->icon.eng_level[0].val = val;
            disp->icon.update_flag = ICON_UPDATE_ENG0_LEVEL;
            OAL_sem_post(&rply->_lock);
            break;
        }

        case LCD_DISP_CMD_LINK1_ENG:
        {
            amsg_t* rply = (amsg_t*)msg.user_data;
            uint32_t val = rply->req;
            disp->icon.eng_level[1].val = val;
            disp->icon.update_flag = ICON_UPDATE_ENG1_LEVEL;
            OAL_sem_post(&rply->_lock);
            break;
        }

        case LCD_DISP_CMD_LINK0_STA:
        {
            amsg_t* rply = (amsg_t*)msg.user_data;
            uint32_t val = rply->req;
            disp->icon.link_status[0].val = val;
            disp->icon.update_flag = ICON_UPDATE_LINK0_STA;
            OAL_sem_post(&rply->_lock);
            break;
        }

        case LCD_DISP_CMD_LINK1_STA:
        {
            amsg_t* rply = (amsg_t*)msg.user_data;
            uint32_t val = rply->req;
            disp->icon.link_status[1].val = val;
            disp->icon.update_flag = ICON_UPDATE_LINK1_STA;
            OAL_sem_post(&rply->_lock);
            break;
        }

        case LCD_DISP_CMD_PMU_PERCENT:
        {
            amsg_t* rply = (amsg_t*)msg.user_data;
            uint32_t val = rply->req;
            disp->icon.link_status[1].val = val;
            disp->icon.update_flag = ICON_UPDATE_PMU_PERCENT;
            OAL_sem_post(&rply->_lock);
            break;
        }
        case LCD_DISP_CMD_VOLUME:
        {
            amsg_t* rply = (amsg_t*)msg.user_data;
            uint32_t val = rply->req;
            disp->icon.link_status[1].val = val;
            disp->icon.update_flag = ICON_UPDATE_VOLUME;
            OAL_sem_post(&rply->_lock);
        }
            break;
        case LCD_DISP_ADD_SUB:
        {
            amsg_t* rply = (amsg_t*)msg.user_data;
            uint32_t val = rply->req;
            disp->icon.link_status[1].val = val;
            // logi("disp->icon.link_status[1].val == %d\n",val);
            disp->icon.update_flag = ICON_UPDATE_ADD_SUB;
            OAL_sem_post(&rply->_lock);
        }
            break;
        case LCD_DISP_SIGNAL:
        {
            amsg_t* rply = (amsg_t*)msg.user_data;
            uint32_t val = rply->req;
            disp->icon.link_status[1].val = val;
            // logi("disp->icon.link_status[1].val == %d\n",val);
            disp->icon.update_flag = ICON_UPDATE_SIGNAL;
            OAL_sem_post(&rply->_lock);
        }
            break;
#endif
        default:
            break;
    }

    return 0;
}


int lcd_disp_event_listener(void* handle, uint32_t who, uint32_t cmd, uint32_t val, uint32_t rev)
{
    lcd_disp_ctrl_t * disp = (lcd_disp_ctrl_t*)handle;
    if(disp) {
        if(AUDIO_NOTIFY_AMSG == who) {
            switch(cmd) {
                case AMSG_MSG_SENT:
                {
                    break;
                }
                case AMSG_MSG_SENDING:
                {
                    amsg_t   *msg = (amsg_t*)val;
                    uint32_t *msg_rev = (uint32_t*)rev;
                    if(msg && msg_rev) {
                        *msg_rev = OAL_queue_send(&disp->cmd_queue, msg, 2000);
                    }
                    break;
                }
            }
        }
        else if(AUDIO_NOTIFY_AEFFECT_COMPONENT == who)
        {

        }
    }
    return 0;
}

int lcd_disp_xet_ctrl(uint32_t cmd, uint32_t val, uint32_t rev)
{
    lcd_disp_ctrl_t *disp = lcd_disp_ctrl;
    if(!disp) {
        return -1;
    }
    int ret = 0;
    switch(cmd) {
    case LCD_DISP_CMD_LINK0_ENG:
    case LCD_DISP_CMD_LINK1_ENG:
    case LCD_DISP_CMD_LINK0_STA:
    case LCD_DISP_CMD_LINK1_STA:
    case LCD_DISP_CMD_PMU_PERCENT:
    case LCD_DISP_CMD_VOLUME:
    case LCD_DISP_ADD_SUB:
    case LCD_DISP_SIGNAL:
        {
            OAL_mutex_lock(&disp->cmd_lock);
            amsg_block_msg_send(disp->amsg_comp, cmd, val, rev);
            OAL_mutex_unlock(&disp->cmd_lock);
            break;
        }
    }
    
    return ret;
}

void lcd_charging_show(void)
{
    lcd_ground_color_set(WHITE,BLACK);
    lcd_show_string(5,32,192,16,32,(uint8_t *)"charging...",1);
    HAL_mdelay(30);
    LCD_BACKLIGHT_ON();
}


void lcd_poweron_show(uint8_t step)
{
#if EVB_LCD_DEMO

    lcd_disp_ctrl_t * disp = lcd_disp_ctrl;
    bool ret = 0;

    if((NULL == disp) && (step!=0)) {
        return;
    }
    
    switch(step) {
    case 0:
        lcd_dir_change_set(1);
        lcd_show_picture(32, 32, 16, 16,(uint8_t *)Image_smartlink_logo_16_16, 1);
        lcd_dir_change_set(0);
        break;
    case 1:
        OAL_sem_wait(&disp->disp_lock);
        lcd_dir_change_set(1);
        ret = lcd_dma_disp_flush(24, 24, 32, 32,(uint8_t *)Image_smartlink_logo_32_32, 1);
        if(ret == 0) {
            lcd_dir_change_set(0);
            OAL_sem_post(&disp->disp_lock);
        }
        break;
    case 2:
        OAL_sem_wait(&disp->disp_lock);
        
        lcd_dir_change_set(1);
        ret = lcd_dma_disp_flush(8, 8, LCD_POWERON_LOGO_WIDTH, LCD_POWERON_LOGO_HEIGHT,(uint8_t *)Image_smartlink_logo_64_64, 1);
        if(ret == 0) {
            lcd_dir_change_set(0);
            OAL_sem_post(&disp->disp_lock);
        }
        break;

    case 3:
        OAL_sem_wait(&disp->disp_lock);
        
        lcd_dir_change_set(1);
        ret = lcd_dma_disp_flush(8, 8, LCD_POWERON_LOGO_WIDTH, LCD_POWERON_LOGO_HEIGHT,(uint8_t *)gImage_smartlink_logo_black_64_64, 1);
        if(ret == 0) {
            lcd_dir_change_set(0);
            OAL_sem_post(&disp->disp_lock);
        }
        break;
    }
#endif
}

AT_SRAM(.lcdc)
void lcd_dir_change_set(uint8_t dir_change)
{
#if LCD_DIR_R90_CHANGE_SUPPORT
    if((lcd_dir_change == dir_change) || 
        (lcddev.dir != LCD_DIR_HORIZONTAL)) {           //暂只支持竖屏向右转90°作为横屏使用
        return;
    }

    uint16_t regval = 0;
    uint16_t dirreg = 0;
    uint8_t dir = L2R_U2D;
    if(!dir_change) {
        dir = LCD_DFT_SCAN_DIR;
        switch(dir) {
        case L2R_U2D: dir = D2U_L2R; break;
        case L2R_D2U: dir = D2U_R2L; break;
        case R2L_U2D: dir = U2D_L2R; break;
        case R2L_D2U: dir = U2D_R2L; break;
        case U2D_L2R: dir = L2R_D2U; break;
        case U2D_R2L: dir = L2R_U2D; break;
        case D2U_L2R: dir = R2L_D2U; break;
        case D2U_R2L: dir = R2L_U2D; break;
        }
    }

    switch(dir) {
    case L2R_U2D:
        regval|=(0<<7)|(0<<6)|(0<<5);
        break;
    case L2R_D2U:
        regval|=(1<<7)|(0<<6)|(0<<5);
        break;
    case R2L_U2D:
        regval|=(0<<7)|(1<<6)|(0<<5);
        break;
    case R2L_D2U:
        regval|=(1<<7)|(1<<6)|(0<<5);
        break;
    case U2D_L2R:
        regval|=(0<<7)|(0<<6)|(1<<5);
        break;
    case U2D_R2L:
        regval|=(0<<7)|(1<<6)|(1<<5);
        break;
    case D2U_L2R:
        regval|=(1<<7)|(0<<6)|(1<<5);
        break;
    case D2U_R2L:
        regval|=(1<<7)|(1<<6)|(1<<5);
        break;
    }
#if (LCD_CHIP_SEL != LCD_CHIP_ST7789V2_IG03) && (LCD_CHIP_SEL != LCD_CHIP_ST7789S)
    regval|=0X08;                               //BGR color filter panel
#endif
    dirreg = 0x36;                              //Memory Access Control
    HAL_lcdc_module_write_reg(dirreg);
    HAL_lcdc_module_write_data(regval,1);

    if(dir_change) {
        lcd_dir_change = 1;
    } else {
        lcd_dir_change = 0;
    }

    //logi("**dir %d, 0x36 = %#x", lcd_dir_change, regval);
    
#endif
}



void lcd_ble_select_display(int list)
{


}

void all_lcd_disp(void)
{
    // if()

}


AT_SRAM(.lcdc)
uint8_t lcd_dir_change_get(void)
{
#if LCD_DIR_R90_CHANGE_SUPPORT
    return lcd_dir_change;
#else
    return 0;
#endif
}

extern struct pmu_battery_info bat_info;

AT_SRAM(.lcdc)
static void *lcd_disp_loop(void *handle)
{
    // uint8_t tx_data[] = "Hello UART DMA!"; // 要发送的数据
    lcd_disp_ctrl_t *disp = (lcd_disp_ctrl_t *)handle;

    OAL_sem_post(&disp->disp_lock);     //post first
    logi("lcd_disp_loop\r\n");
#if EVB_LCD_DEMO
    g24_mode_t *g24_mode = g24_mode_param_get(); 
#endif

    while(disp) {
        lcd_disp_cmd_process(disp, OAL_WAIT_FOREVER);
        switch(disp->icon.update_flag) {
#if EVB_LCD_DEMO
        case ICON_UPDATE_ENG0_LEVEL:
            {
                if(!g24_mode->connect[0]) {
                    disp->icon.eng_level[0].val = 0;
                }
                
                if(disp->icon.eng_level[0].latest_val == disp->icon.eng_level[0].val) {
                    break;
                }
                OAL_sem_wait(&disp->disp_lock);

                lcd_dir_change_set(1);

                disp->icon.eng_level[0].latest_val = disp->icon.eng_level[0].val;
                uint16_t *dst_w = (uint16_t *)disp->icon.dma_buf;
                uint16_t *dst = (uint16_t *)disp->icon.dma_buf;
                if(lcd_dir_change_get()) {
                    uint32_t level_picel = LCD_ENG_LEVEL_HEIGHT * disp->icon.eng_level[0].val;
                    uint32_t remain_picel = LCD_ENG_LEVEL_HEIGHT * (LCD_ENG_LEVEL_WIDTH - disp->icon.eng_level[0].val);
                    memcpy(dst, disp->icon.eng_level[0].color_buf, level_picel* sizeof(uint16_t));                     //填充能量条颜色
                    memset(dst+level_picel, 0x00, remain_picel * sizeof(uint16_t));                                     //填充黑色
                } else {
                    for(uint32_t j=0; j<LCD_ENG_LEVEL_HEIGHT; j++) {
                        memcpy(dst, disp->icon.eng_level[0].color_buf, disp->icon.eng_level[0].val * sizeof(uint16_t));                     //填充能量条颜色
                        memset(dst+disp->icon.eng_level[0].val, 0x00, (LCD_ENG_LEVEL_WIDTH-disp->icon.eng_level[0].val) * sizeof(uint16_t));//填充黑色
                        dst += LCD_ENG_LEVEL_WIDTH;
                    }
                }
                
                
                bool ret = lcd_dma_disp_flush(0, 0, LCD_ENG_LEVEL_WIDTH, LCD_ENG_LEVEL_HEIGHT, (uint8_t *)&dst_w[0], 1);
                if(ret == 0) {
                    lcd_dir_change_set(0);
                    OAL_sem_post(&disp->disp_lock);
                }
            }
            break;
        
        case ICON_UPDATE_ENG1_LEVEL:
#if G24_DUAL_CHANNEL
            {
                
                if(!g24_mode->connect[1]) {
                    disp->icon.eng_level[1].val = 0;
                }
                
                if(disp->icon.eng_level[1].latest_val == disp->icon.eng_level[1].val) {
                    break;
                }
                OAL_sem_wait(&disp->disp_lock);

                lcd_dir_change_set(1);

                disp->icon.eng_level[1].latest_val = disp->icon.eng_level[1].val;
                uint16_t *dst_w = (uint16_t *)disp->icon.dma_buf;
                uint16_t *dst = (uint16_t *)disp->icon.dma_buf;
                if(lcd_dir_change_get()) {
                    uint32_t level_picel = LCD_ENG_LEVEL_HEIGHT * disp->icon.eng_level[1].val ;
                    uint32_t remain_picel = LCD_ENG_LEVEL_HEIGHT * (LCD_ENG_LEVEL_WIDTH - disp->icon.eng_level[1].val);
                    memcpy(dst, disp->icon.eng_level[1].color_buf, level_picel* sizeof(uint16_t));                     //填充能量条颜色
                    memset(dst+level_picel, 0x00, remain_picel * sizeof(uint16_t));                                     //填充黑色
                } else {
                    for(uint32_t j=0; j<LCD_ENG_LEVEL_HEIGHT; j++) {
                        memcpy(dst, disp->icon.eng_level[1].color_buf, disp->icon.eng_level[1].val * sizeof(uint16_t));                      //填充能量条颜色
                        memset(dst+disp->icon.eng_level[1].val, 0x00, (LCD_ENG_LEVEL_WIDTH-disp->icon.eng_level[1].val) * sizeof(uint16_t)); //填充黑色
                        dst += LCD_ENG_LEVEL_WIDTH;
                    }
                }
                
                bool ret = lcd_dma_disp_flush(0, 14, LCD_ENG_LEVEL_WIDTH, LCD_ENG_LEVEL_HEIGHT, (uint8_t *)&dst_w[0], 1);
                if(ret == 0) {
                    lcd_dir_change_set(0);
                    OAL_sem_post(&disp->disp_lock);
                }
            }
#endif
            break;

       case ICON_UPDATE_LINK0_STA:
            OAL_sem_wait(&disp->disp_lock);

            if(disp->icon.link_status[0].val) {     //connect
                // lcd_fill(30,32,30+5-1,32+5-1, YELLOW);
                disp->icon.link_status[0].latest_val = 1;
            } else {                                //disconnect
                if(disp->icon.link_status[0].latest_val) {
                    // lcd_fill(30,32,30+5-1,32+5-1, BLACK);
                    disp->icon.link_status[0].latest_val = 0;
                } else {
                    // lcd_fill(30,32,30+5-1,32+5-1, YELLOW);
                    disp->icon.link_status[0].latest_val = 1;
                }
            }

            OAL_sem_post(&disp->disp_lock);
            break;
            
        case ICON_UPDATE_LINK1_STA:
            OAL_sem_wait(&disp->disp_lock);

            if(disp->icon.link_status[1].val) {     //connect
                // lcd_fill(30,58,30+5-1,58+5-1, YELLOW);
                disp->icon.link_status[1].latest_val = 1;
            } else {                                //disconnect
                if(disp->icon.link_status[1].latest_val) {
                    // lcd_fill(30,58,30+5-1,58+5-1, BLACK);
                    disp->icon.link_status[1].latest_val = 0;
                } else {
                    // lcd_fill(30,58,30+5-1,58+5-1, YELLOW);
                    disp->icon.link_status[1].latest_val = 1;
                }
            }

            OAL_sem_post(&disp->disp_lock);
            break;
#endif
        }
 
        disp->icon.update_flag = ICON_UPDATE_NONE;
    }
    while(1)
    {
        HAL_mdelay(50);

    }
    return NULL;
}


void destroy_lcd_disp(void* handle)
{
    lcd_disp_ctrl_t * disp = lcd_disp_ctrl;

    if(disp) {
        if(disp->task_handle){
            OAL_thread_join(disp->task_handle);
            disp->task_handle = NULL;
            wait_idle_running_a_period(10000);
        }
        if (disp->cmd_lock) {
            OAL_mutex_destroy(&disp->cmd_lock);
            disp->cmd_lock = NULL;
        }
        if (disp->cmd_queue) {
            OAL_queue_delete(&disp->cmd_queue);
            disp->cmd_queue = NULL;
        }
        if (disp->amsg_comp) {
            destroy_amsg_component(disp->amsg_comp);
            disp->amsg_comp = NULL;
        }

        if (disp->disp_lock) {
            OAL_sem_destroy(&disp->disp_lock);
            disp->disp_lock = NULL;
        }

        if(disp->icon.dma_buf) {
            general_free(disp->icon.dma_buf);
            disp->icon.dma_buf = NULL;
        }
        if(disp->icon.eng_level[0].color_buf) {
            general_free(disp->icon.eng_level[0].color_buf);
            disp->icon.eng_level[0].color_buf = NULL;
        }
        if(disp->icon.eng_level[0].Timer){
            OAL_timer_delete(&disp->icon.eng_level[0].Timer);
            disp->icon.eng_level[0].Timer = NULL;
        }

        if(disp->icon.link_status[0].Timer){
            OAL_timer_delete(&disp->icon.link_status[0].Timer);
            disp->icon.link_status[0].Timer = NULL;
        }
        
        general_free(disp);
    }
    lcd_disp_ctrl = 0;
}

void *create_lcd_disp(void)
{
    int ret;
    OAL_QUEUE_POOL pool;
    lcd_disp_ctrl_t * disp = (lcd_disp_ctrl_t*)general_calloc(sizeof(lcd_disp_ctrl_t));

    if (!disp) {
        logd("lcd disp create fail!");
        return NULL;
    }

    amsg_init_t ai;
    ai.cctrl = (notify_clbk)lcd_disp_event_listener;
    ai.user_data = disp;
    disp->amsg_comp = create_amsg_component(&ai);
    if (!disp->amsg_comp) {
        goto create_lcd_disp_task_fail;
    }

    disp->cmd_lock = OAL_mutex_create();
    if (NULL == disp->cmd_lock) {
        goto create_lcd_disp_task_fail;
    }

    pool.size = LCD_DISP_MSG_QUEUE_LENGTH * sizeof(amsg_t);
    ret = OAL_queue_create(&disp->cmd_queue, NULL, sizeof(amsg_t), &pool);
    if (OAL_SUCCESS != ret) {
        goto create_lcd_disp_task_fail;
    }

    disp->disp_lock = OAL_sem_create_binary();
    if(NULL == disp->disp_lock) {
        goto create_lcd_disp_task_fail;
    }

#if EVB_LCD_DEMO
    disp->icon.dma_buf = general_calloc(LCD_DMA_BUF_SIZE);
    if (NULL == disp->icon.dma_buf) {
        goto create_lcd_disp_task_fail;
    }

    //eng level
    uint16_t fill_height = 1;
#if LCD_DIR_R90_CHANGE_SUPPORT
    fill_height = LCD_ENG_LEVEL_HEIGHT;
    disp->icon.eng_level[0].color_buf = general_calloc(LCD_ENG_LEVEL_HEIGHT * LCD_ENG_LEVEL_WIDTH * sizeof(uint16_t));//用于填充能量条，减少读flash操作
#else
    disp->icon.eng_level[0].color_buf = general_calloc(LCD_ENG_LEVEL_WIDTH * sizeof(uint16_t));//用于填充能量条一行，减少读flash操作
#endif

    if(NULL == disp->icon.eng_level[0].color_buf) {
        goto create_lcd_disp_task_fail;
    }
    disp->icon.eng_level[1].color_buf = disp->icon.eng_level[0].color_buf;      //公用
    uint16_t *color_buf_ptr = (uint16_t *)disp->icon.eng_level[0].color_buf;
    uint16_t green = RGB565_BEND(GREEN);
    for(uint32_t i=0; i<fill_height *LCD_ENG_LEVEL_WIDTH; i++) {
        color_buf_ptr[i] = green;
    }

    if (disp->icon.eng_level[0].Timer == 0) {           //定时更新能量条
        int t_ret = OAL_timer_create(&disp->icon.eng_level[0].Timer, "eng_level", OAL_ms_to_ticks(100),
            lcd_eng_level_timer_cb, (void *)1, 1, OAL_TIMER_AUTO_ACTIVATE);
        if(t_ret == OAL_FAILED) {
            goto create_lcd_disp_task_fail;
        }
    }
    
    //link_status
    if (disp->icon.link_status[0].Timer == 0) {         //用于断开连接时闪烁
        int t_ret = OAL_timer_create(&disp->icon.link_status[0].Timer, "link_status", OAL_ms_to_ticks(500),
            lcd_link_status_timer_cb, (void *)1, 1, OAL_TIMER_AUTO_ACTIVATE);
        if(t_ret == OAL_FAILED) {
            goto create_lcd_disp_task_fail;
        }
    }
#endif

    disp->task_handle = OAL_thread_create(lcd_disp_loop, disp,
                                LCD_DISP_TASK_NAME, LCD_DISP_TASK_PRIO, LCD_DISP_TASK_STACK_DEPTH);
    if(NULL == disp->task_handle) {
        goto create_lcd_disp_task_fail;
    }

    logd("lcd_disp create ok!");
    lcd_disp_ctrl = disp;
    
    return disp;

create_lcd_disp_task_fail:
    destroy_lcd_disp(disp);
    disp= NULL;
    return NULL;
}

void lcd_disp_init(void)
{
    lcd_disp.handler = create_lcd_disp();
}


#if LCDC_TEST_EN
__attribute__((aligned(4)))
//uint8_t lcd_buf[1024];

uint8_t lcd_buf[4096];

/*
#define LINE_CNT   40
uint16_t  buf_a[240*LINE_CNT]={0x0};
uint16_t  buf_b[240*LINE_CNT]={0xffff};
bool lcd_flush_test_flag;

AT_SRAM(.lcdc)
bool lcd_flush_test_flag_get(void)
{
    return lcd_flush_test_flag;
}


AT_SRAM(.lcdc)
void lcd_flush_test(void)
{
static  uint32_t   inc = 0,cnt = 0;
lcd_flush_test_flag = true;
for(cnt=0;cnt<240*LINE_CNT;cnt++)
{
    buf_a[cnt] = 0;
    buf_b[cnt] = 0xffff;
}

inc = OAL_get_tick_count();
for(cnt=0;cnt< 10000;cnt++)
{
    if(0 == (cnt/(320/LINE_CNT))%2)
    lcd_dma_disp_flush(0,LINE_CNT*(cnt%(320/LINE_CNT)),240,LINE_CNT,(uint8_t *)buf_a,1);
    else
    lcd_dma_disp_flush(0,LINE_CNT*(cnt%(320/LINE_CNT)),240,LINE_CNT,(uint8_t *)buf_b,1);

    if( 0 != cnt && 0 == cnt%800 )
    {
    xprintf("  %d ms per flush,interval ticks=%d now times=%d \n", (OAL_get_tick_count()-inc)/100,OAL_get_tick_count()-inc,cnt);
    inc = OAL_get_tick_count();
    }
}
lcd_flush_test_flag = false;
}
*/

AT_SRAM(.lcdc)
void lcdc_test(void)
{
    logi("lcdc_test");
    __maybe_unused uint32_t id,color;
    __maybe_unused uint32_t *lcd_buf_ptr = (void *)lcd_buf;
    lcdc_dma_t dma_param;
    LCD_BACKLIGHT_ON();
    dma_param.rx_en = ENABLE;
    dma_param.tx_en = ENABLE;
    dma_param.lcdc_dma_rx_callback = NULL;
    dma_param.lcdc_dma_tx_callback = NULL;
    lcdc_dma_mode_init(&dma_param);

#if (LCD_CHIP_SEL != LCD_CHIP_ST7789V2_IG03)        //ST7789V2 IG03没有RDX
    //read ID test
    id = lcd_read_id();
    logi("id = 0x%x",id);
    //burst命令,Read display identification
    HAL_lcdc_module_read(0x04,1,lcd_buf,4);
    logi("display ID:");
    printf_array(lcd_buf, 4);
#endif

    //show color test
    lcd_clear(RED);

    //show char test
    lcd_ground_color_set(BLUE,YELLOW);
    lcd_show_string(30,40,204,24,24,(uint8_t *)"Smartlinktech",0);

    //gram write and read test
    color = RGB_BEND(BLUE);
    for(uint16_t i=0;i<85;i++) {                //BLUE   0xFC0000
#if (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB565)
        lcd_buf_ptr[3*i] = (color&0xffff)|((color&0xffff)<<16);
        lcd_buf_ptr[3*i+1] = (color&0xffff)|((color&0xffff)<<16);
        lcd_buf_ptr[3*i+2] = (color&0xffff)|((color&0xffff)<<16);
#elif (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB666)
        lcd_buf_ptr[3*i] = (color&0xffffff)|((color&0xff)<<24);
        lcd_buf_ptr[3*i+1] = ((color&0xffffff)>>8)|((color&0xffff)<<16);
        lcd_buf_ptr[3*i+2] = ((color&0xffffff)>>16)|(color<<8);
#endif
    }
    lcd_set_cursor(100,100+12-1,20,20);
    HAL_lcdc_module_write(lcddev.wrramcmd,1,lcd_buf,12);                            //cmd_len max 4byte,data_len max 16byte

#if (LCD_CHIP_SEL == LCD_CHIP_ILI9488) || (LCD_CHIP_SEL == LCD_CHIP_ST7789S)
    color = lcd_read_point(100,20);
    logi("color=0x%08x",color);
#endif

    //show picture test
#if (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB565)
    logi("RGB565 image");
    lcd_show_picture(20,70,100,100,(uint8_t *)gImage_lcd_rgb565_100_100_big_end,1);
#elif (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB666)
    logi("RGB666 image");
    lcd_show_picture(20,70,100,100,(uint8_t *)gImage_lcd_rgb666_100_100_big_end,1);
#endif

    //dma write and read test
    lcd_set_cursor(60,76-1,180,196-1);
#if (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB565)
    HAL_lcdc_dma_write(lcddev.wrramcmd,1,lcd_buf,512);
#elif (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB666)
    HAL_lcdc_dma_write(lcddev.wrramcmd,1,lcd_buf,768);
#endif
    while(HAL_is_lcdc_tx_dma_transing());

    lcd_set_cursor(125,125+100-1,70,70+100-1);
    lcd_write_ram_prepare();
    for(uint16_t i=0;i<10;i++) {
#if (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB565)
        HAL_lcdc_dma_write(0x00,0,(uint8_t *)&gImage_lcd_rgb565_100_100_big_end[1000*i],2000);
#elif (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB666)
        HAL_lcdc_dma_write(0x00,0,(uint8_t *)&gImage_lcd_rgb666_100_100_big_end[3000*i],3000);
#endif
        while(HAL_is_lcdc_tx_dma_transing());
    }

    //测试 4.支持单次最大传输长度4KByte
    lcd_set_cursor(80,180-1,180,180+41-1);
#if (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB565)
    HAL_lcdc_dma_write(lcddev.wrramcmd,1,(uint8_t *)&gImage_lcd_rgb565_100_100_big_end[0],4096);
#elif (LCD_COLOR_FORMAT==LCD_COLOR_FORMAT_RGB666)
    HAL_lcdc_dma_write(lcddev.wrramcmd,1,(uint8_t *)&gImage_lcd_rgb666_100_100_big_end[0],4096);
#endif
    while(HAL_is_lcdc_tx_dma_transing());
    logi("finish dma write");

#if (LCD_CHIP_SEL == LCD_CHIP_ILI9488) || (LCD_CHIP_SEL == LCD_CHIP_ST7789S)                       //GX9A01 datasheet没有读gram指令，ST7789V2 IG03没有RDX
    memset(lcd_buf,0,1024);
#if LCDC_INTERFACE && !LCDC_SPI_LINE_SEL                     //spi 4line
    HAL_lcdc_module_spi_dummy_cycle_set(0);
#endif
    lcd_set_cursor(60,76-1,180,196-1);
    HAL_lcdc_dma_read(lcddev.rdramcmd,1,lcd_buf,768);        //the first byte is dummy,ili9488 3byte存一个像素
    while(HAL_is_lcdc_rx_dma_transing());
#if LCDC_INTERFACE && !LCDC_SPI_LINE_SEL                     //spi 4line
    HAL_lcdc_module_spi_dummy_cycle_set(1);
#endif
    printf_array(lcd_buf,768);
    logi("finish dma read");
#endif

    //sleep test
    HAL_mdelay(1000);
    lcd_enter_sleep();
    HAL_mdelay(1000);
    lcd_exit_sleep();

    while(1);
}
#endif

#else
void lcd_controller_init(void)
{
}

void lcd_screen_init(uint8_t if_fill)
{

}

AT_SRAM(.lcdc)
bool lcd_flush_test_flag_get(void)
{
    return false;
}

uint8_t LCD_g24_get_channel_ID(void)
{
    return 10;
}



#endif

