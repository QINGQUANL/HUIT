#ifndef _SENSOR_PROXIMITY_H_
#define _SENSOR_PROXIMITY_H_


#define SENSOR_PROXIMITY_TWI_ID             TWI_ID_01


#if (SENSOR_PROXIMITY_DEVICE_SEL == 0)
#define SENSOR_PROXIMITY_INT_PIN_PULL       PIN_PULL_UP100K

/* PA22409001 */
#define SENSOR_PROXIMITY_TWI_ADDR           (0x3C>>1)
#define SENSOR_PROXIMITY_CHIP_ID            0x11
#define SENSOR_PROXIMITY_PIN_ACTIVE_STATE   0       //0-低电平
//register
#define SENSOR_PROXIMITY_REG_PS_EN          0x00
#define SENSOR_PROXIMITY_REG_CHIP_ID        0x7F
#define SENSOR_PROXIMITY_REG_PS_INTERRUPT   0x02

#define SENSOR_PROXIMITY_PS_INTF            (0x01<<1)

#elif (SENSOR_PROXIMITY_DEVICE_SEL == 1)
/* HY2751 */
#define SENSOR_PROXIMITY_PIN_ACTIVE_STATE   1       //1-高电平
#define SENSOR_PROXIMITY_INT_PIN_PULL       PIN_PULL_DISABLE

#elif (SENSOR_PROXIMITY_DEVICE_SEL == 2)
#define SENSOR_PROXIMITY_INT_PIN_PULL       PIN_PULL_UP100K
#define SENSOR_PROXIMITY_PIN_ACTIVE_STATE   0       //0-低电平
#endif


#define    LTK1181                          1
#define    TMD2635                          2

void sensor_proximity_init(void);
void sensor_proximity_deinit(void);
bool sensor_proximity_online(void);
bool sensor_proximity_online_sta_get(void);
void sensor_proximity_ps_sta_set(uint8_t sta);
uint8_t sensor_proximity_ps_sta_get(void);
void sensor_proximity_ps_sta_ews_set(uint8_t sta);
uint8_t sensor_proximity_ps_sta_ews_get(void);
void sensor_proximity_interrupt(void);

void sensor_proximity_lt_timer_create(void);
void sensor_proximity_lt_timer_start(void);
void sensor_proximity_lt_timer_stop(void);
void sensor_proximity_lt_timeout(void* xTimer);
void sensor_proximity_lt_timeout_sta_set(bool sta);
bool sensor_proximity_lt_timeout_sta_get(void);

void sensor_proximity_ht_need_play_set(bool sta);
bool sensor_proximity_ht_need_play_get(void);

void sensor_proximity_write(uint8_t adr, uint8_t value);
uint8_t sensor_proximity_read(uint8_t adr);

bool sensor_proximity_is_enable(void);

bool sensor_get_proximity_sta(void);
void sensor_proximity_timer_start(bool start);

void sensor_proximity_lt_timer(void);
void sensor_proximity_lt_sta_set(bool sta);
bool sensor_proximity_lt_sta_get(void);
void sensor_proximity_lt_timer_stop_and_sync(void);
void sensor_proximity_lt_timer_start_and_sync(void);
void sensor_proximity_lt_timeout_sta_set_and_sync(bool sta);
void sensor_proximity_ht_need_play_set_and_sync(bool sta);
uint8_t sensor_proximity_peer_sta_get(void);
void sensor_proximity_pre_init(void);
#if 1 
uint8_t sensor_proximity_reset_flag_get(void);
uint8_t sensor_proximity_reset_cnt_get(void);
void sensor_proximity_info_get(void);
void sensor_proximity_info_set(uint8_t reset_flag, uint8_t reset_cnt);
#endif
void sensor_proximity_mode_init(void);
void sensor_proximit_id_check(void);
uint8_t sensor_proximity_get_select(void);


#endif
