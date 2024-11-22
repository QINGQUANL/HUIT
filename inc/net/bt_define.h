#ifndef _BT_DEFINE_H_
#define _BT_DEFINE_H_


#define MASTER_DISCONNECTED			            0x00
#define MASTER_CONNECTING				        0x01
#define MASTER_CONNECTIND				        0x02
#define MASTER_CONNECTCFM				        0x03
#define MASTER_CONNECTED				        0x04

#define MASTER_CONNECTREQ				        0x10
#define MASTER_WAITACL					        0x11
#define MASTER_WAIT_ROLE_SWITCH			        0x12
#define MASTER_ACL_FINISH                       0x13
#define MASTER_WAIT_ENCRYPTED			        0x14
#define MASTER_CONNECT                          0x15
#define MASTER_WAITCONNECTED			        0x16
#define MASTER_CONFIGREQ				        0x17
#define MASTER_WAITCINFIGCFM			        0x18
#define MASTER_DISCONNECTREQ			        0x19
#define MASTER_WAITDISCONNECTED			        0x1a
#define MASTER_DISCONNECTING                    0x1b

#define MASTER_TX_SDP_SERVICE_SEARCH_ATT_REQ    0x21
#define MASTER_WAIT_SDP_SERVICE_SEARCH_ATT_RSP	0x22
#define MASTER_RX_SDP_SERVICE_SEARCH_ATT_RSP	0x23

#define MASTER_RFCOMM_TX_SABM0                  0x30
#define MASTER_RFCOMM_WAIT_SABM0                0x31
#define MASTER_RFCOMM_CREATE_DLCI               0x32
#define MASTER_RFCOMM_WAIT_DLCI                 0x33
#define MASTER_RFCOMM_FINISH                    0x34


#define MAX_NR_AVCTP_CHANNELS		            1
#define MAX_NR_AVDTP_CHANNELS		            2

#define COMPILER_LITTLE_ENDIAN

typedef struct{
	uint8_t flag;
	uint8_t service_type;
	uint32_t token_rate;
	uint32_t bucket_size;
	uint32_t peak_bandwidth;
	uint32_t latency;
	uint32_t delay_variation;
} __attribute__((packed)) quality_service;

typedef struct {
	uint8_t sig_id_rcv;
	uint8_t sig_id_snk;
	uint8_t l2cap_state;
	uint8_t link;	//bit0,1:config request rx tx
			//bit4,5: config response rx tx
	uint8_t bdaddr[6];
	uint16_t hci_handle;
	uint16_t local_cid;
	uint16_t remote_cid;
	uint16_t psm;
	uint16_t local_mtu;
	uint16_t remote_mtu;
	uint16_t flush_timeout;
	uint16_t flag;

	uint8_t GOTMTUFlag;
	quality_service qos;
} __attribute__((packed)) l2cap_connection;

typedef struct {
	uint8_t valid;
    uint16_t connect_handle;
	uint16_t cid;
	uint16_t len;
    uint8_t *payload;
} l2cap_receive_data_t;

typedef struct {
	union {
		uint8_t param;
		struct {

#ifdef COMPILER_LITTLE_ENDIAN
			uint8_t	rfa:4;
			uint8_t	type:4;
#else
			uint8_t	type:4;
			uint8_t	rfa:4;
#endif
		}__attribute__((packed))bit_field;
	} __attribute__((packed)) media_type_byte0;

	volatile uint8_t media_codec_type;

	union {
		uint8_t param;
		struct {

#ifdef COMPILER_LITTLE_ENDIAN
			uint8_t	channel_mode:4; //MONO, DUAL CHANNEL, STEREO, JOINT STEREO
			uint8_t	sample_frequency:4;
#else
			uint8_t	sample_frequency:4;
			uint8_t	channel_mode:4;
#endif
		}__attribute__((packed)) bit_field;

	} __attribute__((packed)) media_codec_information;

	union {
		uint8_t param;
		struct {

#ifdef COMPILER_LITTLE_ENDIAN
			uint8_t	allocation_method:2;
			uint8_t	subbands:2;
			uint8_t	block_len:4;
#else
			uint8_t	block_len:4;
			uint8_t	subbands:2;
			uint8_t	allocation_method:2;
#endif
		}__attribute__((packed)) bit_field;
	} __attribute__((packed)) media_codec_info_third_byte;

	uint8_t codec_info_minimum_bitpool;
	uint8_t codec_info_maximum_bitpool;
}__attribute__((packed)) avdtp_media_codec_t;

typedef struct {
	uint8_t state;                   //must corresponding l2cap_upper_t
    uint8_t ticks;             //must corresponding l2cap_upper_t
	uint8_t channel_type;
	uint16_t l2cap_cid;              //must corresponding with (to) l2cap_upper_t
    struct a2dp_connection *a2dp_connection;
} __attribute__((packed)) avdtp_con_t;

typedef struct{
	uint8_t state;                   //corresponding  l2cap_upper_t
    uint8_t ticks;                   //corresponding l2cap_upper_t
    uint8_t avrcp_capability;
    uint8_t avrcp_id3_status;
    uint16_t avrcp_id3_delay;
	//uint16_t psm;                  		//must corresponding l2cap_upper_t
	uint16_t l2cap_cid;              //must corresponding l2cap_upper_t
    struct a2dp_connection *a2dp_connection;
    uint8_t tx_label;
    uint8_t rx_label;
    uint8_t need_play;
    uint32_t play_delay;
    struct {
        uint8_t state;
        uint8_t txcmd;
        uint8_t rxcmd;
        uint32_t ticks;
    } button;
}  __attribute__((packed)) avctp_con_t;

typedef struct {
    uint8_t seid_number;
    uint8_t seid_index;
    uint8_t seid_buf[10]; //samsung note8 seid not continuation 02 01 04 00 08 00 0c 00 14 00 18 00 1c 00
    uint8_t int_seid;
	uint8_t sep_inuse;
    uint8_t tx_label;
    uint8_t rx_label;
#if 1// ifdef AVDTP_SCMS_T_SUPPORT
    uint8_t scms_t_support;
#endif
    avdtp_media_codec_t m_codec;
    uint8_t sbc_seid_index;
    uint8_t sbc_capabilities;
    uint8_t aac_capabilities;
    uint8_t aac_codec[6];
    uint8_t ldac_capabilities;
    uint8_t ldac_seid;
    uint8_t acc_seid;
}__attribute__((packed)) avdtp_ctl_t;

typedef struct a2dp_connection{
	uint16_t hci_handle;
	uint8_t bdaddr[6];

    uint8_t state_a2dp_recheck;
    uint8_t state_a2dp_check;

    uint8_t index;
    uint8_t a2dp_play_state;
    uint8_t avdtp_play_state;        //0=idle,1=play,2=pause
    uint8_t avctp_play_state;        //current state
    uint8_t avctp_expect_state;      //expect state

    uint8_t state;
    uint8_t a2dp_feature;
    uint8_t avdtp_nego_state;
    uint8_t avdtp_flag;
    uint32_t state_a2dp_ticks;  //timeout 100ms
    uint32_t avdtp_nego_ticks;

	avdtp_con_t avdtp_connect[MAX_NR_AVDTP_CHANNELS]; //0:signaling , 1:media, 2:reporting, 3:recovery channel
	avctp_con_t avctp_connect[MAX_NR_AVCTP_CHANNELS]; //control channel
    avdtp_ctl_t avdtp_ctl;
	uint8_t reset_btmusic_vol;
}  a2dp_connection_t;

typedef enum _bt_a_service_observer_cmd {
    BT_A_SERVICE_OBSERVER_CONNECTED,
    BT_A_SERVICE_OBSERVER_CONNECT_IND,
    BT_A_SERVICE_OBSERVER_CONNECT_CONFIRM,
    BT_A_SERVICE_OBSERVER_DISCONNECT_IND,
    BT_A_SERVICE_OBSERVER_DISCONNECT_CONFIRM,
    BT_A_SERVICE_OBSERVER_DISCONNECT_PHONE,
    BT_A_SERVICE_OBSERVER_CONNECT_COMPLETE,
    BT_A_SERVICE_OBSERVER_PHONE_CONNECT,
    BT_A_SERVICE_OBSERVER_PHONE_DISCONNECT,
    BT_A_SERVICE_OBSERVER_RECEIVE_DATA,
    BT_A_SERVICE_OBSERVER_RECEIVE_DATA1,
    BT_A_SERVICE_OBSERVER_PARAM_GET,
    BT_A_SERVICE_OBSERVER_PARAM_SET,
    BT_A_SERVICE_OBSERVER_AVDTP_DISCOVER_RESPONSE,
    BT_A_SERVICE_OBSERVER_AVDTP_SIGNALING_GET_ALL_CAPABILITIES,
    BT_A_SERVICE_OBSERVER_AVDTP_SIGNALING_GET_CAPABILITIES,
    BT_A_SERVICE_OBSERVER_AVDTP_SIGNALING_SET_CONFIGURATION,
    BT_A_SERVICE_OBSERVER_AVDTP_START,
    BT_A_SERVICE_OBSERVER_AVDTP_START_RESPONSE,
    BT_A_SERVICE_OBSERVER_AVDTP_SUSPEND,
    BT_A_SERVICE_OBSERVER_AVDTP_SUSPEND_RESPONSE,
    BT_A_SERVICE_OBSERVER_MASTER_BSBUF_EMPTY,
    BT_A_SERVICE_OBSERVER_MASTER_ACL_FINISH,
    BT_A_SERVICE_OBSERVER_MASTER_LINK_KEY_GET,
    BT_A_SERVICE_OBSERVER_PAIR_STATUS_GET,
    BT_A_SERVICE_OBSERVER_KEY_UPDATE,
    BT_A_SERVICE_OBSERVER_SEAMLESS,
    BT_A_SERVICE_OBSERVER_AUTH_FAIL,
} bt_a_service_observer_cmd;


#endif
