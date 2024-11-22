#ifndef _TWI_H_
#define _TWI_H_


/* minimum clk: 24M/256=93.75K
 *
 * divider = 2^(n+1)
 * [0x7]: 24M / 256 = 93.75K
 * [0x6]: 24M / 128 = 187.5K
 * [0x5]: 24M / 64 = 375K
 */

#define TWI1_CLK_FREQ           100000
#define TWI2_CLK_FREQ           100000
#define TWI1_CLK_FREQ_FAST      1000000
#define TWI2_CLK_FREQ_FAST      1000000


#if (TWI1_SCL_PIN_SEL == 0)
//#define TWI1_SCL_PIN_CFG        (PIN_A4 | PIN_FUNC8 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SCL_PIN_SEL == 1)
#define TWI1_SCL_PIN_CFG        (PIN_B0 | PIN_FUNC3 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SCL_PIN_SEL == 2)
#define TWI1_SCL_PIN_CFG        (PIN_B13 | PIN_FUNC3 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SCL_PIN_SEL == 3)
#define TWI1_SCL_PIN_CFG        (PIN_B18 | PIN_FUNC8 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SCL_PIN_SEL == 4)
#define TWI1_SCL_PIN_CFG        (PIN_B22 | PIN_FUNC7 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SCL_PIN_SEL == 5)
#define TWI1_SCL_PIN_CFG        (PIN_B28 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SCL_PIN_SEL == 6)
#define TWI1_SCL_PIN_CFG        (PIN_B30 | PIN_FUNC7 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SCL_PIN_SEL == 7)
#define TWI1_SCL_PIN_CFG        (PIN_C2 | PIN_FUNC8 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SCL_PIN_SEL == 8)
#define TWI1_SCL_PIN_CFG        (PIN_D1 | PIN_FUNC6 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SCL_PIN_SEL == 9)
#define TWI1_SCL_PIN_CFG        (PIN_D3 | PIN_FUNC6 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SCL_PIN_SEL == 10)
#define TWI1_SCL_PIN_CFG        (PIN_D7 | PIN_FUNC9 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SCL_PIN_SEL == 11)
#define TWI1_SCL_PIN_CFG        (PIN_D11 | PIN_FUNC7 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SCL_PIN_SEL == 12)
#define TWI1_SCL_PIN_CFG        (PIN_D12 | PIN_FUNC3 | PIN_DRI1 | PIN_PULL_UP2K)
#endif

#if (TWI1_SDA_PIN_SEL == 0)
//#define TWI1_SDA_PIN_CFG        (PIN_A5 | PIN_FUNC8 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SDA_PIN_SEL == 1)
#define TWI1_SDA_PIN_CFG        (PIN_B2 | PIN_FUNC3 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SDA_PIN_SEL == 2)
#define TWI1_SDA_PIN_CFG        (PIN_B11 | PIN_FUNC3 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SDA_PIN_SEL == 3)
#define TWI1_SDA_PIN_CFG        (PIN_B12 | PIN_FUNC10 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SDA_PIN_SEL == 4)
#define TWI1_SDA_PIN_CFG        (PIN_B15 | PIN_FUNC10 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SDA_PIN_SEL == 5)
#define TWI1_SDA_PIN_CFG        (PIN_B19 | PIN_FUNC8 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SDA_PIN_SEL == 6)
#define TWI1_SDA_PIN_CFG        (PIN_B23 | PIN_FUNC7 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SDA_PIN_SEL == 7)
#define TWI1_SDA_PIN_CFG        (PIN_B29 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SDA_PIN_SEL == 8)
#define TWI1_SDA_PIN_CFG        (PIN_C2 | PIN_FUNC9 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SDA_PIN_SEL == 9)
#define TWI1_SDA_PIN_CFG        (PIN_D0 | PIN_FUNC11 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SDA_PIN_SEL == 10)
#define TWI1_SDA_PIN_CFG        (PIN_D2 | PIN_FUNC6 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SDA_PIN_SEL == 11)
#define TWI1_SDA_PIN_CFG        (PIN_D6 | PIN_FUNC6 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SDA_PIN_SEL == 12)
#define TWI1_SDA_PIN_CFG        (PIN_D10 | PIN_FUNC3 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SDA_PIN_SEL == 13)
#define TWI1_SDA_PIN_CFG        (PIN_D12 | PIN_FUNC7 | PIN_DRI1 | PIN_PULL_UP2K)
#endif


#if (TWI2_SCL_PIN_SEL == 0)
#define TWI2_SCL_PIN_CFG        (PIN_B4 | PIN_FUNC11 | PIN_DRI1 | PIN_PULL_UP2K)
#define TWI2_SCL_PIN_NUM        (PIN_B4)
#define TWI2_SCL_PIN_FUNC       (PIN_FUNC11)
#define TWI2_SCL_PIN_PULL       (PIN_PULL_UP2K)
#elif (TWI2_SCL_PIN_SEL == 1)
#define TWI2_SCL_PIN_CFG        (PIN_B7 | PIN_FUNC8 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SCL_PIN_SEL == 2)
#define TWI2_SCL_PIN_CFG        (PIN_B8 | PIN_FUNC6 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SCL_PIN_SEL == 3)
#define TWI2_SCL_PIN_CFG        (PIN_B9 | PIN_FUNC9 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SCL_PIN_SEL == 4)
#define TWI2_SCL_PIN_CFG        (PIN_B16 | PIN_FUNC8 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SCL_PIN_SEL == 5)
#define TWI2_SCL_PIN_CFG        (PIN_B20 | PIN_FUNC5 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SCL_PIN_SEL == 6)
#define TWI2_SCL_PIN_CFG        (PIN_B26 | PIN_FUNC10 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SCL_PIN_SEL == 7)
#define TWI2_SCL_PIN_CFG        (PIN_C2 | PIN_FUNC10 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SCL_PIN_SEL == 8)
#define TWI2_SCL_PIN_CFG        (PIN_D4 | PIN_FUNC6 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SCL_PIN_SEL == 9)
#define TWI2_SCL_PIN_CFG        (PIN_D8 | PIN_FUNC6 | PIN_DRI1 | PIN_PULL_UP2K)
#endif

#if (TWI2_SDA_PIN_SEL == 0)
#define TWI2_SDA_PIN_CFG        (PIN_B6 | PIN_FUNC8 | PIN_DRI1 | PIN_PULL_UP2K)
#define TWI2_SDA_PIN_NUM        (PIN_B6)
#define TWI2_SDA_PIN_FUNC       (PIN_FUNC8)
#define TWI2_SDA_PIN_PULL       (PIN_PULL_UP2K)
#define TWI2_SDA_PIN_PULL_100K  (PIN_PULL_UP100K)
#elif (TWI2_SDA_PIN_SEL == 1)
#define TWI2_SDA_PIN_CFG        (PIN_B8 | PIN_FUNC9 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SDA_PIN_SEL == 2)
#define TWI2_SDA_PIN_CFG        (PIN_B9 | PIN_FUNC6 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SDA_PIN_SEL == 3)
#define TWI2_SDA_PIN_CFG        (PIN_B10 | PIN_FUNC9 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SDA_PIN_SEL == 4)
#define TWI2_SDA_PIN_CFG        (PIN_B17 | PIN_FUNC8 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SDA_PIN_SEL == 5)
#define TWI2_SDA_PIN_CFG        (PIN_B21 | PIN_FUNC5 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SDA_PIN_SEL == 6)
#define TWI2_SDA_PIN_CFG        (PIN_B27 | PIN_FUNC10 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SDA_PIN_SEL == 7)
#define TWI2_SDA_PIN_CFG        (PIN_C2 | PIN_FUNC11 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SDA_PIN_SEL == 8)
#define TWI2_SDA_PIN_CFG        (PIN_D3 | PIN_FUNC7 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SDA_PIN_SEL == 9)
#define TWI2_SDA_PIN_CFG        (PIN_D5 | PIN_FUNC6 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SDA_PIN_SEL == 10)
#define TWI2_SDA_PIN_CFG        (PIN_D7 | PIN_FUNC6 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SDA_PIN_SEL == 11)
#define TWI2_SDA_PIN_CFG        (PIN_D10 | PIN_FUNC11 | PIN_DRI1 | PIN_PULL_UP2K)
#endif


/***************************************************************************
                 use gpio software mode
***************************************************************************/
#define TWI_SCL_INIT()          HAL_gpio_config(TWI_IO_SCL_PIN | PIN_FUNCOUTPUT | PIN_DRI3 | PIN_PULL_UP2K)
#define TWI_SCL_DEINIT()        HAL_gpio_config(TWI_IO_SCL_PIN | PIN_FUNCDISABLE | PIN_DRI1| PIN_PULL_UP2K)
#define TWI_SCL_FUNC_OUT()      HAL_gpio_func_set(TWI_IO_SCL_PIN, PIN_FUNCOUTPUT)
#define TWI_SCL_VAL_HIGH()      HAL_gpio_value_set(TWI_IO_SCL_PIN, 1)
#define TWI_SCL_VAL_LOW()       HAL_gpio_value_set(TWI_IO_SCL_PIN, 0)

#define TWI_SDA_INIT()          HAL_gpio_config(TWI_IO_SDA_PIN | PIN_FUNCOUTPUT | PIN_DRI3 | PIN_PULL_UP2K)
#define TWI_SDA_DEINIT()        HAL_gpio_config(TWI_IO_SDA_PIN | PIN_FUNCDISABLE | PIN_DRI1 | PIN_PULL_UP2K)
#define TWI_SDA_FUNC_OUT()      HAL_gpio_func_set(TWI_IO_SDA_PIN, PIN_FUNCOUTPUT)
#define TWI_SDA_FUNC_IN()       HAL_gpio_func_set(TWI_IO_SDA_PIN, PIN_FUNCINPUT)
#define TWI_SDA_VAL_HIGH()      HAL_gpio_value_set(TWI_IO_SDA_PIN, 1)
#define TWI_SDA_VAL_LOW()       HAL_gpio_value_set(TWI_IO_SDA_PIN, 0)
#define TWI_SDA_VAL_GET()       HAL_gpio_value_get(TWI_IO_SDA_PIN)


void twi_init(void);
void twi_deinit(void);
void twi_clk_set(uint32_t id, uint32_t speed_mode);
int twi_write(uint32_t id, uint32_t addr, const uint8_t *data, uint32_t bytes);
int twi_read(uint32_t id, uint32_t addr, uint8_t *data, uint32_t bytes);
void twi_io_read(uint8_t addr, uint8_t *data, uint8_t size);
void twi_io_write(uint8_t addr, uint8_t *data, uint8_t size);

void twi_delay(void);
void twi_io_init(void);
void twi_io_deinit(void);
void twi_start(void);
void twi_stop(void);
void twi_write_byte(uint8_t data);
uint8_t twi_read_byte(void);
void twi_send_ack(void);
void twi_send_nack(void);
uint8_t twi_get_ack(void);
int twi_write_stop_ctl(uint32_t id, uint32_t addr, const uint8_t *data, uint32_t bytes,uint8_t need_stop);

#endif

