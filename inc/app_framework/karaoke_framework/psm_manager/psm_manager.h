#ifndef _PSM_MANAGER_H_
#define _PSM_MANAGER_H_

#define BT_LINK_MAX         5
#define BT_VOLUME_MAX       4
#define BT_ADDR_SIZE        6
#define BT_LINKKEY_SIZE     16

#define G24_LINK_MAX        2


#define FILE_NUM_DEFAULT        1
#define FILE_MS_DEFAULT         0

#define FM_FREQ_DEFAULT         0
#define FM_TOTAL_DEFAULT        1
#define FM_CH_CUR_DEFAULT       1

//info mem addr define (rtc memory，地址范围：0x00 ~ 0x0F，共64 byte)
#define INFO_MUSIC_FNUM         0x00    //0-sd fnum; 1-u fnum;
#define INFO_MUSIC_PTIME        0x02    //2-sd ptime; 3-u ptime;
#define INFO_FILE_CHECKSUM      0x04    //4-sd checksum; 5-u checksum;
#define INFO_SYS_STARTUP        0x06    //6-system startup first flag

//info mem addr define (norflash，地址范围：0x00 ~ 0xFF，共256 byte)
#define INFO_SDCARD_NUM         0x00    //music sdcard num          4
#define INFO_UDISK_NUM          0x04    //music udisk num           4
#define INFO_SDCARD_CHECKSUM    0x08    //music sdcard checksum     4
#define INFO_UDISK_CHECKSUM     0x0C    //music udisk checksum      4
#define INFO_SDCARD_TIME        0x10    //music sdcard time         4
#define INFO_UDISK_TIME         0x14    //music udisk time          4
#define INFO_MUSIC_DEVICE       0x18    //music device              1
#define INFO_BTMUSIC_VOLUME     0x19    //bt music volume           1
#define INFO_BTCALL_VOLUME      0x1A    //bt call volume            1
#define INFO_REPEAT_SEL         0x1B    //music repeat sel          1
#define INFO_EQ_SEL             0x1C    //music eq sel              1

#define INFO_ALARM_HOUR         0x20    //alarm hour                1
#define INFO_ALARM_MIN          0x21    //alarm min                 1
#define INFO_ALARM_EN           0x22    //alarm en                  1
#define INFO_ALARM_MODE         0x23    //alarm mode                1

#define INFO_FM_FREQ            0x30    //fm freq                   1
#define INFO_FM_TOTAL           0x31    //fm ch total               1
#define INFO_FM_CH_CUR          0x32    //fm ch cur                 1
#define INFO_FM_CH              0x33    //fm ch                     1*50 (0x33~0x64)
#define INFO_BT_VOLUME_OFFSET   0x30    //bt volume                 72(0x30~0x78)

#define INFO_BT_OFFSET          0x90    //bt                        96(0x90~0xEF)
#define INFO_BLE_RAND_NUM       0xF4    //ble rand num              4
#define INFO_RAND_NUM           0xF8    //rand num                  4

#define INFO_CRC                0xFC    //crc                       2
#define INFO_SIGN               0xFE    //sign(0x53,0x4C)           2


#define EWS_LEFT_FLAG                   'L'
#define EWS_RIGHT_FLAG                  'R'
#define EWS_BOOTUP_CODE                 'C'
#define EWS_PAIRED_FLAG                 'P'
#define EWS_UNPAIRED_FLAG               'U'
#define EWS_ADDR_BACKUPED               'B'
#define EWS_ADDR_UNBACKUPED             'D'
/*------------------------------------------------------------------------------
*PSM:存储信息lable
*-------------------------------------------------------------------------------*/
#define ANC_INFO_NAME           "anc_info"
#define LINK_INFO               "link_info"
#define BT_VOL_INFO             "bt_vol"
#define FREQ_DRIFT_INFO         "freq_drift"
#define RF_PARAM_INFO           "rfparam"
#define BT_ADDR_INFO            "bt_addr"
#define BT_NAME_INFO            "bt_name"
#define BT_ADDR_BACK_INFO       "bt_addr_backup"
#define EAR_TYPE                "ear_type"
#define START_UP_FLAG           "startup_flag"
#define PAIRED_FLAG             "paired_flag"
#define ADDR_BKUP_FLAG          "addr_backup_flag"
#define ANC_MODE_SEL            "anc_mode_sel"
#define BAT_LEVEL               "bat_level"
#define EN_INFO                 "en_info"
#define SN_INFO                 "sn_info"
#define KWS_INFO                "kws_info"
#define G24_ADDR_INFO           "g24_addr"
#define G24_LINK_INFO           "g24_link"
#define AM_VOL_INFO             "am_vol"
#define BLE_ADDR_INFO            "ble_addr"
#define BLE_NAME_INFO            "ble_name"


#define CUSTOMER_NAME_INFO      "customer_name"
#define CHIP_SERIAL_ID          "chip_serial_id"
#define BOARD_NAME_INFO         "board_name"
#define BOOTUP_FLAG_INFO        "bootup_flag"
#define BASEBOARD_VERSION_INFO  "baseboard_ver_info"
#define CUSTOMER_VERSION_INFO   "customer_ver_info"

#define MP_DUT_FLAG             "dut"
#define SENSOR_RESET_INFO       "sensor_reset_info"
#define SENSOR_LTK_CAL_INFO     "sensor_ltk1181_cal_info"
#define MIC_AFC_PARAMS_INFO     "mic_afc_params_info"

#define G24_AFC_PARAM           "g24_afc_param"
#define WORK_MODE               "WORK_MODE"
#define USB_AUDIO_WORK_MODE      "MODE_USBAUDIO"
#define LPC_WAKEUP              "lpcwakeup"
#define RTC_TIME_FLAG           "rtc_set"
#define MASTER_MIC_ID           "mmic_id"

typedef struct {
    uint8_t order;                      // 1,2,3,4, the latest is 1
    uint8_t addr[BT_ADDR_SIZE];         //x6
    uint8_t linkkey[BT_LINKKEY_SIZE];   //x16
} bt_link_t;

typedef struct {
    uint8_t sign;
    uint8_t second_latest_num;          //1,2,3,4,5
    uint8_t latest_num;                 //1,2,3,4,5
    bt_link_t link[BT_LINK_MAX];        //x5
} bt_link_info_t;

typedef struct {
    uint8_t order;                      // 1,2,3,4,5,6,7,8 the latest is 1
    uint8_t addr[BT_ADDR_SIZE];
    uint8_t volume;
    uint8_t call_volume;
} bt_volume_t;

typedef struct {
    bt_volume_t volume[BT_VOLUME_MAX];  //x4
} bt_volume_info_t;

typedef struct {
    uint32_t poweron_time;
    bt_link_info_t bt_info;
    bt_volume_info_t bt_volume_info;
} bt_exchange_info_t;

typedef struct {
    uint8_t addr[BT_ADDR_SIZE];         //x6
    //uint8_t linkkey[BT_LINKKEY_SIZE];   //x16
} g24_link_t;

typedef struct {
    g24_link_t g24_link[G24_LINK_MAX];
    g24_link_t earphone_addr[G24_LINK_MAX];  //save for slaver the same with Master
} g24_link_info_t;

void nor_rfpara_set(uint32_t *flash_btss_para,uint8_t len);
void nor_rfpara_get(uint32_t *flash_btss_para,uint8_t len);


void nor_bt_exchange_info_get(bt_exchange_info_t *exchange_info);

uint16_t nor_freq_drift_get(void);


void bt_volume_all_info_get(uint8_t default_volume);
void bt_volume_all_info_default_set(uint8_t default_volume);
void bt_volume_info_set(uint8_t *addr, uint8_t volume, bool sel_call);
uint8_t bt_volume_info_get(uint8_t *addr, bool sel_call);
void bt_volume_info_save(void);

uint8_t nor_tws_ear_type_get(void);
void nor_tws_ear_type_set(uint8_t left);
bool nor_tws_startup_flag_get(void);
uint8_t nor_tws_paired_get(void);
void nor_tws_paired_set(uint8_t paired);
void nor_tws_addr_backup(uint8_t *addr);
void nor_tws_addr_restore(void);
bool nor_tws_pair(bool is_ews_left, uint8_t* bd_addr);
bool nor_tws_unpair(bool is_ews_left);
uint8_t nor_tws_name_set(uint8_t *name, uint8_t len);
void nor_tws_addr_set(uint8_t *addr);
void nor_tws_addr_get(uint8_t *addr);

uint8_t link_addr_get(uint8_t *addr);
void link_info_init(void);
void link_info_update(void);
void bt_info_init(void);
void link_info_save(void);
void bt_exchange_info_do(void *info);

bool is_nor_mp_dut_flag_set();
void nor_mp_dut_flag_set(BOOL set);
bool is_nor_bootup_flag_set();
void nor_mp_bootup_flag_set(BOOL bSet);
void nor_freq_drift_set(uint16_t value);
void link_info_clean(void);
void bt_volume_info_clean(uint8_t default_volume);

void bt_name_info_save(void);
void low_power_wakup_en();
void low_power_wakup_clear();
bool is_low_power_wakup_flag_set();

#endif

