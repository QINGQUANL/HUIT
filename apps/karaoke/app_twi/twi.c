#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "hal.h"
#include "twi.h"
#include "device.h"


void twi_init(void)
{
#if TWI1_EN
    module_ccu_enable(CCU_TWI0);
    HAL_clk_enable(CLK_TWI0);

    HAL_gpio_config(TWI1_SCL_PIN_CFG);
    HAL_gpio_config(TWI1_SDA_PIN_CFG);

    HAL_twi_module_init(TWI_ID_01, TWI1_CLK_FREQ);
#endif

#if TWI2_EN
    module_ccu_enable(CCU_TWI1);
    HAL_clk_enable(CLK_TWI1);

    HAL_gpio_config(TWI2_SCL_PIN_CFG);
    HAL_gpio_config(TWI2_SDA_PIN_CFG);

    HAL_twi_module_init(TWI_ID_02, TWI2_CLK_FREQ);
#endif
}

void twi_deinit(void)
{
#if TWI1_EN
    HAL_twi_module_deinit(TWI_ID_01);
    HAL_clk_disable(CLK_TWI0);
    module_ccu_disable(CCU_TWI0);
#endif

#if TWI2_EN
    HAL_twi_module_deinit(TWI_ID_02);
    HAL_clk_disable(CLK_TWI1);
    module_ccu_disable(CCU_TWI1);
#endif

#if (TWI1_SD_SHARE_EN || TWI2_SD_SHARE_EN)
    HAL_gpio_config(SD_D0_PIN_CFG);
    HAL_gpio_config(SD_CMD_PIN_CFG);
#endif
}

AT_SRAM(.twi_proc)
void twi_clk_set(uint32_t id, uint32_t speed_mode)
{
    uint32_t freq;
    uint32_t freq_fast;

    if (id == TWI_ID_01) {
        freq = TWI1_CLK_FREQ;
        freq_fast = TWI1_CLK_FREQ_FAST;
    } else {
        freq = TWI2_CLK_FREQ;
        freq_fast = TWI2_CLK_FREQ_FAST;
    }

    if (speed_mode) {
        HAL_twi_module_clk_set(id, freq_fast);
    } else {
        HAL_twi_module_clk_set(id, freq);
    }
}

AT_SRAM(.twi_proc)
int twi_write(uint32_t id, uint32_t addr, const uint8_t *data, uint32_t bytes)
{
    int ret;

#if TWI2_DM_SHARE_EN
    usb_detect_set(USB_DISABLE);
    HAL_gpio_pull_set(TWI2_SCL_PIN_NUM, TWI2_SCL_PIN_FUNC);
    HAL_gpio_pull_set(TWI2_SCL_PIN_NUM, TWI2_SCL_PIN_PULL);
#endif

    ret = HAL_twi_module_write(id, addr, data, bytes,1);

#if TWI2_DM_SHARE_EN
    usb_detect_set(USB_DETECT);
#endif

    return ret;
}


/*------------------------------------------------------------------------------
*函数功能:twi 写数据可控制是否有stop位
*-------------------------------------------------------------------------------*/

AT_SRAM(.twi_proc)
int twi_write_stop_ctl(uint32_t id, uint32_t addr, const uint8_t *data, uint32_t bytes,uint8_t need_stop)
{
    int ret;

#if TWI2_DM_SHARE_EN
    usb_detect_set(USB_DISABLE);
    HAL_gpio_pull_set(TWI2_SCL_PIN_NUM, TWI2_SCL_PIN_FUNC);
    HAL_gpio_pull_set(TWI2_SCL_PIN_NUM, TWI2_SCL_PIN_PULL);
#endif

    ret = HAL_twi_module_write(id, addr, data, bytes,need_stop);

#if TWI2_DM_SHARE_EN
    usb_detect_set(USB_DETECT);
#endif

    return ret;
}


AT_SRAM(.twi_proc)
int twi_read(uint32_t id, uint32_t addr, uint8_t *data, uint32_t bytes)
{
    int ret;

#if TWI2_DM_SHARE_EN
    usb_detect_set(USB_DISABLE);
    HAL_gpio_pull_set(TWI2_SCL_PIN_NUM, TWI2_SCL_PIN_FUNC);
    HAL_gpio_pull_set(TWI2_SCL_PIN_NUM, TWI2_SCL_PIN_PULL);
#endif

    ret = HAL_twi_module_read(id, addr, data, bytes);

#if TWI2_DM_SHARE_EN
    usb_detect_set(USB_DETECT);
#endif

    return ret;
}


/***************************************************************************
                 use gpio software mode
***************************************************************************/
#if TWI_IO_EN

AT_SRAM(.sensor)
void twi_delay(void)
{
    HAL_udelay(500);
}

AT_SRAM(.sensor)
void twi_io_init(void)
{
   // TWI_SCL_INIT();
    HAL_gpio_driving_set(TWI_IO_SCL_PIN,PIN_DRI3);
    HAL_gpio_pull_set(TWI_IO_SCL_PIN,PIN_PULL_UP2K);
   // TWI_SDA_INIT();
    HAL_gpio_driving_set(TWI_IO_SDA_PIN,PIN_DRI3);
    HAL_gpio_pull_set(TWI_IO_SDA_PIN,PIN_PULL_UP2K);

    TWI_SCL_VAL_HIGH();
    TWI_SDA_VAL_HIGH();
    TWI_SCL_FUNC_OUT();
    TWI_SDA_FUNC_OUT();
}

AT_SRAM(.sensor)
void twi_io_deinit(void)
{
  //  TWI_SCL_DEINIT();
  HAL_gpio_func_set(TWI_IO_SCL_PIN, PIN_FUNCDISABLE);
  HAL_gpio_driving_set(TWI_IO_SCL_PIN,PIN_DRI1);
  HAL_gpio_pull_set(TWI_IO_SCL_PIN,PIN_PULL_UP2K);
  //  TWI_SDA_DEINIT();
    HAL_gpio_func_set(TWI_IO_SDA_PIN, PIN_FUNCDISABLE); 
    HAL_gpio_driving_set(TWI_IO_SDA_PIN,PIN_DRI1);
    HAL_gpio_pull_set(TWI_IO_SDA_PIN,PIN_PULL_UP2K);
}

AT_SRAM(.sensor)
void twi_start(void)
{
    twi_io_init();
    twi_delay();

    TWI_SDA_VAL_LOW();
    twi_delay();

    TWI_SCL_VAL_LOW();
    twi_delay();
}

AT_SRAM(.sensor)
void twi_stop(void)
{
    TWI_SDA_VAL_LOW();
    TWI_SDA_FUNC_OUT();
    twi_delay();

    TWI_SCL_VAL_HIGH();
    twi_delay();

    TWI_SDA_VAL_HIGH();
    twi_delay();
    twi_io_deinit();
}

AT_SRAM(.sensor)
void twi_write_byte(uint8_t data)
{
    TWI_SDA_FUNC_OUT();
    twi_delay();
    for(uint32_t i=0;i<8;i++) {
        if (data & 0x80) {
            TWI_SDA_VAL_HIGH();
        } else {
            TWI_SDA_VAL_LOW();
        }
        twi_delay();
        TWI_SCL_VAL_HIGH();
        twi_delay();
        data <<= 1;
        TWI_SCL_VAL_LOW();
        HAL_udelay(300);
    }
}

uint8_t twi_read_byte(void)
{
    uint8_t val = 0;

    TWI_SDA_FUNC_IN();
    twi_delay();
    for(uint32_t i=0;i<8;i++) {
        TWI_SCL_VAL_HIGH();
        twi_delay();
        val <<= 1;
        if (TWI_SDA_VAL_GET()) {
            val |= 0x01;
        }
        TWI_SCL_VAL_LOW();
        twi_delay();
    }
    return val;
}

void twi_send_ack(void)
{
    TWI_SDA_VAL_LOW();
    TWI_SDA_FUNC_OUT();
    TWI_SCL_VAL_LOW();
    twi_delay();

    TWI_SCL_VAL_HIGH();
    twi_delay();

    TWI_SCL_VAL_LOW();
}

void twi_send_nack(void)
{
    TWI_SDA_VAL_HIGH();
    TWI_SDA_FUNC_OUT();
    TWI_SCL_VAL_LOW();
    twi_delay();

    TWI_SCL_VAL_HIGH();
    twi_delay();

    TWI_SCL_VAL_LOW();
}

AT_SRAM(.sensor)
uint8_t twi_get_ack(void)
{
    uint8_t val;

    TWI_SDA_FUNC_IN();
    twi_delay();

    TWI_SCL_VAL_HIGH();
    twi_delay();

    val = TWI_SDA_VAL_GET();

    TWI_SCL_VAL_LOW();
    twi_delay();

    return val;
}

void twi_io_read(uint8_t addr, uint8_t *data, uint8_t size)
{
    uint32_t i = 0;

    twi_start();
    twi_write_byte((addr << 1) | 0x1);
    twi_get_ack();

    for (i=0;i<size-1;i++) {
        data[i] = twi_read_byte();
        twi_send_ack();
    }

    data[i] = twi_read_byte();
    twi_send_nack();

    twi_stop();
}

AT_SRAM(.sensor)
void twi_io_write(uint8_t addr, uint8_t *data, uint8_t size)
{
    twi_start();

    twi_write_byte(addr << 1);
    twi_get_ack();

    for (uint32_t i=0;i<size;i++) {
        twi_write_byte(data[i]);
        twi_get_ack();
    }

    twi_stop();
}
#endif //TWI_IO_EN

#if EEPROM_TEST
#define EEPROM_TWI_ID           TWI_ID_02
#define EEPROM_TWI_ADDR         (0xA0>>1)

void eeprom_write(uint8_t adr, uint8_t *pdata, uint8_t bytes)
{
#if 1
    //硬件模块方式
    uint8_t data[bytes+1];  //bytes <= 8

    data[0] = adr;
    for (uint8_t i=0;i<bytes;i++) {
        data[1+i] = *(pdata+i);
    }

    twi_write(EEPROM_TWI_ID, EEPROM_TWI_ADDR, data, bytes+1);
#else
    //gpio软件模拟方式
    twi_start();

    twi_write_byte(0xa0);
    twi_get_ack();

    twi_write_byte(adr);
    twi_get_ack();

    for (uint32_t i=0;i<bytes;i++) {
        twi_write_byte(*pdata++);
        twi_get_ack();
    }

    twi_stop();
#endif
}

uint8_t eeprom_read(uint8_t adr)
{
#if 1
    //硬件模块方式
    uint8_t value;

    twi_write(EEPROM_TWI_ID, EEPROM_TWI_ADDR, &adr, 1);
    twi_read(EEPROM_TWI_ID, EEPROM_TWI_ADDR, &value, 1);

    return value;
#else
    //gpio软件模拟方式
    uint8_t value;

    twi_start();

    twi_write_byte(0xa0);
    twi_get_ack();

    twi_write_byte(adr);
    twi_get_ack();

    twi_start();
    twi_write_byte(0xa1);
    twi_get_ack();

    value = twi_read_byte();
    twi_send_nack();
    twi_stop();

    return value;
#endif
}

void eeprom_test(void)
{
    logi("%s", __func__);

    uint8_t data[3];

    data[0] = 0xa0;
    data[1] = 0xa1;
    data[2] = 0xa2;
    eeprom_write(0x00, data, 3);
    mdelay(10);
    logi("eeprom: %x,%x,%x", eeprom_read(0x00),eeprom_read(0x01),eeprom_read(0x02));
}
#endif

#if 0
//example:

void rda5807_read(uint8_t *data, uint8_t size)
{
    uint32_t i = 0;

    twi_start();
    twi_write_byte(0x21);
    twi_get_ack();

    for (i=0;i<size-1;i++) {
        data[i] = twi_read_byte();
        twi_send_ack();
    }

    data[i] = twi_read_byte();
    twi_send_nack();

    twi_stop();
}

void rda5807_write(const uint8_t *data, uint8_t size)
{
    twi_start();

    twi_write_byte(0x20);
    twi_get_ack();

    for (uint32_t i=0;i<size;i++) {
        twi_write_byte(data[i]);
        twi_get_ack();
    }

    twi_stop();
}
#endif
