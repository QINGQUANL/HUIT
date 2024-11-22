#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "iodm_test.h"
#include "hal.h"
#include "system.h"
#include "bt_host_api.h"
#include "event.h"
#include "psm_manager.h"
#include "info_mem.h"
#include "rf.h"
#include "tws_profile.h"
#include "mode_manager.h"

#if BT_DUT_EN && BT_IODM_TEST_MODE_EN //|| BT_TWS_MP_MODE_EN

#if (BT_DUT_UART_COM_NUM == 0)
#define TEST_UART	UART_ID_00
#elif (BT_DUT_UART_COM_NUM == 1)
#define TEST_UART	UART_ID_01
#elif (BT_DUT_UART_COM_NUM == 2)
#define TEST_UART	UART_ID_02
#endif

void mac_heart_break_timer_stop();
void nor_tws_startup_flag_set(BOOL bSet);


UrCmd g_urCmd;
uint8_t* g_pUrData;
uint8_t  g_rxCount ;
uint8_t g_len =0;
uint32_t g_timestamp;

extern void event_put(uint32_t event, const void *params, uint32_t params_len);
extern void HAL_uart_tx(uint32_t uart_id, uint32_t data);

uint32_t g_frequntOffset;
extern void rfdss_tone(unsigned int fre);

#if 0
/*定时器发送heart_break信息有可能引起线程不同步的问题
比如定时器发送了一半数据，MAC地址就发下来触发了串口中断，
此时PC应该会受到一个错误的包，引起错误，不过PC有1秒的反应时间，
应该大概率避开，  先这样，后续测试到真有问题再改这个机制
*/

AT(.iodm_test_seg)
void mac_heart_break_timer(xTimerHandle xTimer)
{
    respond_ack(UR_HEART_BREAK);
    heart_break_count ++;
    if(heart_break_count > 5){
        mac_heart_break_timer_stop();
    }
}

AT(.iodm_test_seg)
void mac_heart_break_timer_stop()
{
    xTimerStopFromISR(autoMacTimerHandler, 0);
    //xTimerDelete(autoMacTimerHandler, 0);
    //autoMacTimerHandler = NULL;
}
#endif

BOOL is_mp_enter_udt_mode()
{
    BOOL res = is_nor_mp_dut_flag_set();
    if(res) {
       logi("mp enter dut");
    } else {
        logi("mp is not enter dut");
    }
    return res;
}

void iodm_test_init()
{
    logi("iodm_test_init ++++++++++++++++++++++++++");
    // uart_dma_rx_get_data(UART_ID_00,sizeof(UrCmd));
    HAL_uart_register_rx_callback(TEST_UART, uart2_rx_data);
    g_pUrData = (uint8_t*)&g_urCmd;

#if 0
    if (autoMacTimerHandler == NULL) {
        autoMacTimerHandler = xTimerCreate("autoMac", pdMS_TO_TICKS(1000), pdTRUE, (void *)1, mac_heart_break_timer);
        xTimerStart(autoMacTimerHandler, 0);
    }
    UrCmdAck cmdAck;
    cmdAck.start = ACK_START;
    cmdAck.type = 'M';
    cmdAck.channel = 0x02;
    cmdAck.checksum = 0;
    uint8_t* p = (uint8_t*)&cmdAck;
    for(int i =0;i< sizeof(cmdAck) - 1;i++) {
        cmdAck.checksum += p[i];
    }

    for(int i=0;i< sizeof(cmdAck);i++){
        uart_tx(UART_ID_00, p[0]);}
    }
    #endif
}

//AT_SRAM(.iodm_test_seg)
void respond_get_bt_addr()
{
    memset(g_urCmd.data,0x00,6);
    sys_info_read(BT_ADDR_INFO, g_urCmd.data, 6);

    if (((g_urCmd.data[0]==0x00)&&(g_urCmd.data[1]==0x00)&&(g_urCmd.data[2]==0x00)&&(g_urCmd.data[3]==0x00)&&(g_urCmd.data[4]==0x00)&&(g_urCmd.data[5]==0x00)) || \
        ((g_urCmd.data[0]==0xFF)&&(g_urCmd.data[1]==0xFF)&&(g_urCmd.data[2]==0xFF)&&(g_urCmd.data[3]==0xFF)&&(g_urCmd.data[4]==0xFF)&&(g_urCmd.data[5]==0xFF))) {

            extern uint8_t bt_addr_default[6];

            g_urCmd.data[0] = bt_addr_default[5];
            g_urCmd.data[1] = bt_addr_default[4];
            g_urCmd.data[2] = bt_addr_default[3];
            g_urCmd.data[3] = bt_addr_default[2];
            g_urCmd.data[4] = bt_addr_default[1];
            g_urCmd.data[5] = bt_addr_default[0];

            //libc_memcpy(g_urCmd.data, bt_addr_default, 6);
    }


    uint8_t* p = (uint8_t*)&g_urCmd;
    g_urCmd.len = 0x0D;
    g_urCmd.data[6] = ~g_urCmd.data[0];
    g_urCmd.data[7] = ~g_urCmd.data[1];
    g_urCmd.data[8] = ~g_urCmd.data[2];
    g_urCmd.data[9] = ~g_urCmd.data[3];
    g_urCmd.data[10] = ~g_urCmd.data[4];
    g_urCmd.data[11] = ~g_urCmd.data[5];
    for(int i=0;i< 17 ;i++){
        HAL_uart_tx(TEST_UART, p[i]);
    }
}

void respond_set_bt_addr()
{
    uint8_t* p = (uint8_t*)&g_urCmd;
    for(int i=0;i< 13 ;i++){
        HAL_uart_tx(TEST_UART, p[i]);
    }
}

void respond_frequent_cmd(BOOL fail, uint8_t drift_save)
{
    uint8_t* p = (uint8_t*)&g_urCmd;
    g_urCmd.len = 0x04;
    g_urCmd.data[0] = 0x01;
    g_urCmd.data[1] = fail;
    g_urCmd.data[2] = drift_save;

    for(int i=0;i< 8 ;i++){
        HAL_uart_tx(TEST_UART, p[i]);
    }
}

void respond_received_cmd()
{
    uint8_t* p = (uint8_t*)&g_urCmd;
    g_urCmd.len = 0x02;
    g_urCmd.data[0] = 0x01;
    for(int i=0;i< 6 ;i++){
        HAL_uart_tx(TEST_UART, p[i]);
    }
}

void respond_get_bt_name()
{
    uint8_t* p = (uint8_t*)&g_urCmd;
    g_urCmd.len = BT_NAME_MAX + 1;
    for(uint8_t i=0;i< 5;i++){
        HAL_uart_tx(TEST_UART, p[i]);
    }
    uint8_t btName[BT_NAME_MAX] ;
    uint8_t len;
    bt_name_get(btName,&len);

    for(uint8_t i=0; i< BT_NAME_MAX; i++) {
        HAL_uart_tx(TEST_UART, btName[i]);
    }
}

void repond_dut_cmd()
{
    HAL_uart_tx(TEST_UART, 0x01);
    HAL_uart_tx(TEST_UART, 0xE0);
    HAL_uart_tx(TEST_UART, 0xFC);
    HAL_uart_tx(TEST_UART, 0x02);
    HAL_uart_tx(TEST_UART, 0xAF);
    HAL_uart_tx(TEST_UART, 0x01);
}

extern void mode_bt_exit(void);

uint8_t* get_g_rx_data(void)
{
    return &g_urCmd.data[0];
}

bool is_iodm_test;
uint8_t is_iodm_test_get(void)
{
    return is_iodm_test;
}


//AT_SRAM(.iodm_test_seg)
void uart2_rx_data(uint32_t data)
{
    logi("uart2_rx_data %x ,g_rxCount %d",data,g_rxCount);
    uint32_t timestamp = HAL_timestamp_to_ms(HAL_get_timestamp());
    if(timestamp  - g_timestamp > 400){
        g_rxCount = 0;
    }

    g_timestamp = timestamp;

    g_pUrData[g_rxCount] = data;
    if(g_rxCount == 0) {
        if(data == CMD_START0){
            g_rxCount++;
        }
    } else if(g_rxCount == 1){
        if(data != CMD_START1){
            g_rxCount = 0;
        } else {
            if(data == CMD_START0){
                g_pUrData[0] = CMD_START0;
                g_rxCount= 0;
            }
            g_rxCount++;
        }
    } else if(g_rxCount == 2){
        if(data != CMD_START2){
            g_rxCount = 0;
        }else {
            if(data == CMD_START0){
                g_pUrData[0] = CMD_START0;
                g_rxCount= 0;
            }
            g_rxCount++;
        }
    } else if(g_rxCount == 3){
        g_len = data;
        g_rxCount++;
    } else {
        g_rxCount++;
        g_len--;
        if(g_len == 0){// darta receive cpl
            g_rxCount = 0;
            switch(g_urCmd.cmd){
                case CMD_TEST_INIT:
                    nor_tws_startup_flag_set(0);
                    respond_received_cmd();
                    HAL_system_restart(0);
                    break;
                case CMD_GET_BT_ADDR:
                    respond_get_bt_addr();
                    break;
                case CMD_UPDATE_MAC:
                    logi("update mac+++++");
                    printf_array(g_urCmd.data,6);
                    {
                        uint8_t mac[6];
                        mac[0] = g_urCmd.data[5];
                        mac[1] = g_urCmd.data[4];
                        mac[2] = g_urCmd.data[3];
                        mac[3] = g_urCmd.data[2];
                        mac[4] = g_urCmd.data[1];
                        mac[5] = g_urCmd.data[0];
                        //update_mac(mac,g_urCmd.data[6]=='L');
                        sys_info_write(BT_ADDR_INFO, mac, 6);
                        nor_tws_ear_type_set(g_urCmd.data[6]=='L');
                        respond_set_bt_addr();
                    }
                    break;
                case CMD_UPDATE_STARTUP:
                    //write_flash_startup_flag();
                    respond_received_cmd();
                    break;
                case CMD_GET_BT_NAME:
                    logi("bt name +++++");
                    respond_get_bt_name();
                    break;
                case CMD_SET_BT_NAME:
                    break;
                case CMD_BT_SIGLE_WAVE:  //2.4G
                    logi("sigle wave++++");
                    ///mode_bt_exit();
                    //--------
                    //extern void rfdss_tone(unsigned int fre);
                    //rfdss_tone(2441);
                    //respond_frequent_cmd();

                           // mode_bt_exit();
                   // extern void rfdss_tone(unsigned int fre);
                   // rfdss_tone(2441);

                   // extern void respond_received_cmd();
                   respond_received_cmd();
                   event_put(BT_EVENT_SIGLE_WAVE, NULL,0);
                    break;
                case CMD_BT_SIGLE_WAVE_FREQ_ENTER:
                     //if (MODE_CLEANLINK!= mode_sta_get()) {
					 //	respond_received_cmd();
                     //   mode_set_force(MODE_CLEANLINK);
                     //} else {
					 //	respond_frequent_cmd(0, CMD_BT_SIGLE_WAVE_FREQ_ENTER);
                     //   mode_set_force(MODE_BT);
                     //}
                    break;
                 case CMD_BT_SIGLE_WAVE_FREQ_SET:
                    logi("bt sigle wave offset++++");
                   // event_put(APP_SYNC_EVENT|BT_EVENT_SIGLE_WAVE, &g_urCmd.data[0],1);
                    rfdss_tone(2402+g_urCmd.data[0]);
					respond_frequent_cmd(1, g_urCmd.data[0]);
                    break;
                case CMD_UPDATE_TEST_POWEROFF_CURRENT:
                    event_put(SYSTEM_EVENT_POWEROFF, NULL,0);
                    respond_received_cmd();
                    break;
                case CMD_UPDATE_TEST_CHARGE_MODE:
                    event_put(SYSTEM_EVENT_FORCE_CHARGE_MODE, NULL,0);
                    respond_received_cmd();
                    break;
                case CMD_ADJUST_FREQUENT:  //set frequent offset
                    {
                    logi("adjust fre++++");
                    #if 1
                        g_frequntOffset = g_urCmd.data[0];
                        g_frequntOffset <<= 8;
                        g_frequntOffset += g_urCmd.data[1];
                        g_frequntOffset <<= 8;
                        g_frequntOffset += g_urCmd.data[2];
                        g_frequntOffset <<= 8;
                        g_frequntOffset += g_urCmd.data[3];

                        uint8_t fail = freq_drift_cal_xiaoniu(g_frequntOffset);
                        uint8_t freq_to_save = freq_drift_value_get();
                        //freq_drift_save(freq_to_save);
                        nor_freq_drift_set(freq_to_save);
                        respond_frequent_cmd(fail,freq_to_save);
                    #endif    
                    }
                    break;
                case CMD_RESTART:
                    logi("restart---");
                    respond_received_cmd();
                    HAL_restart_pmu();
                    break;

                case CMD_PREPARE_ENTER_DUT:
                    {
                    	logi("===uart enter dut===");
                        nor_tws_startup_flag_set(1);
                        nor_mp_dut_flag_set(true);
                    }
                    repond_dut_cmd();
                    HAL_restart_pmu();
                    //event_put(BT_EVENT_UR_ENTER_DUT);
                    break;
                //内部命令
                case CMD_READ_FLASH:    //read data
                    //norflash_read((uint32_t)g_urCmd.mac,*(uint32_t*)g_urCmd.reserved,6);
                    //logi("read data %x",*(uint32_t*)g_urCmd.reserved);
                    //printf_array(g_urCmd.mac, 6);
                    //respond_ack(UR_OK);
                    break;
            }
        }
    }

}
#else
void respond_received_cmd()
{

}
#endif
