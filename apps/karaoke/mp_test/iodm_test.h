#ifndef _MAC_H_
#define _MAC_H_

#include "sys_inc.h"
#include "user_config.h"

enum UART_CMD_
{
    CMD_TEST_INIT   = 0x05,
    CMD_UPDATE_MAC  = 0x09,
    CMD_UPDATE_STARTUP = 0x0A,
    CMD_UPDATE_TEST_POWEROFF_CURRENT = 0x0B,
    CMD_UPDATE_TEST_CHARGE_MODE = 0x0C,

    CMD_GET_BT_ADDR = 0x62,
    CMD_GET_BT_NAME = 0x65,
    CMD_SET_BT_NAME = 0x67,


    CMD_BT_SIGLE_WAVE = 0x80,
    CMD_ADJUST_FREQUENT = 0x81,
    CMD_RESTART      = 0x82,
    CMD_SAVE_FREQUENT = 0x83,
    CMD_BT_SIGLE_WAVE_FREQ_ENTER = 0x84,
    CMD_BT_SIGLE_WAVE_FREQ_SET = 0x85,
    CMD_PREPARE_ENTER_DUT= 0xAF,
    CMD_READ_FLASH  = 0xBC,
};

#define CMD_START0 0x01
#define CMD_START1 0xE0
#define CMD_START2 0xFC

#define ACK_START 0xA5

typedef enum _UR_STATUE_
{
    UR_OK = 0,
    UR_RX_CHECKSUM_ERROR,
    UR_CHECK_ERROR,
    UR_HEART_BREAK = 0xCA,
    UR_UNKOWN_CMD = 0xF0,
}E_UrStatus;

typedef struct _UR_CMD_
{
	uint8_t start0; //always 0xCC
	uint8_t start1; //0x55
	uint8_t start2; //0x55
	uint8_t len;
	uint8_t cmd;
	uint8_t data[19];
}UrCmd;


typedef struct _UR_CMD_ACK_
{
	uint8_t start; //always 0xA5
	uint8_t type;   //
	uint8_t channel;    //
	uint8_t status;	//0 : OK
	int32_t frequent_offset;
	uint32_t reserved0;
	uint8_t reseved1[3];
	uint8_t checksum;
}UrCmdAck;


void iodm_test_init();
void respond_ack(uint8_t status);
void uart2_rx_data(uint32_t data);
BOOL is_mp_enter_udt_mode();
void mp_enter_dut_process(void);
void repond_dut_cmd();

#endif

