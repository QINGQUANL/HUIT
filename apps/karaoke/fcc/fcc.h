#ifndef _FCC_H_
#define _FCC_H_


#ifdef __cplusplus
extern "C" {
#endif


enum fcc_rx_e {
   FCC_UART_IDLE = 0,
   FCC_UART_RX,
};

typedef struct
{
	uint8_t   test_control;	 // 0: stop; 1: start
	uint8_t   freq_type;     // 0: hop;  1: hop
    uint8_t   channel_idx;   // 0~79
    uint8_t   tx_power;      // 0~32
    uint8_t   rx_gain;       // 0~32
    uint8_t   rx_gain_auto;
	uint8_t   role_idx;      // 0:slave 1:master -> node_idx
	uint8_t   tx_type;       // packe type
	uint16_t  tx_pkt_len;    // 0~1024
	uint8_t   rx_type;       // packe type
	uint16_t  rx_pkt_len;    // 0~1024
	uint8_t   payload_type;  // 0: 0x0;  1:0xff  2:0x55  3:0xf0:  4:prbs9
	uint8_t   tx_edr;
    uint8_t   rx_edr;
}fcc_control_t;

void bt_fcc_test(void);
void fcc_test_init(void);
int pattern_parse(void);
void fcc_bt_addr_set(void);
void btss_close(void);

void fcc_test_cntl(fcc_control_t fcc_control_n);


#ifdef __cplusplus
}
#endif

#endif

