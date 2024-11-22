#ifndef _SBT_API_H_
#define _SBT_API_H_

void sbt_init(void);

void sbt_master_create(uint8_t link_id, uint8_t free_chan_param_en, uint16_t start_slot, uint16_t start_us);
void sbt_master_destroy(uint8_t link_id);
void sbt_master_master_disconnect(uint8_t link_id, uint8_t user_id);
void sbt_slave_create(uint8_t link_id, uint8_t user_id, uint8_t user_id_map, uint8_t *master_addr, uint16_t clk_offset, uint16_t start_slot, uint16_t start_us);
void sbt_slave_start_scan(void);
void sbt_slave_stop_scan(void);
void sbt_slave_connect(uint8_t link_id, uint8_t master_link_id, uint8_t *master_addr);
void sbt_slave_disconnect(uint8_t link_id);

void sbt_data_tx_en(uint8_t link_id, uint8_t tx_en);
void sbt_data_rx_en(uint8_t link_id, uint8_t rx_en);
void sbt_data_tx(uint8_t *data, uint8_t link_id, uint16_t payload_len);
uint8_t sbt_data_rx(uint8_t *data, uint8_t link_id, uint8_t *chan_id, uint16_t *payload_len);

void sbt_clock_handle(uint16_t clk);


#endif // _SBT_API_H_
