#ifndef _SYSTEM_RETCODE_H_
#define _SYSTEM_RETCODE_H_

/* include header files */

#ifdef __cplusplus
 extern "C" {
#endif

/* Return value of function calls */
/* Section Success, Fail */
#define RET_SUCCESS         0       /* Success return */
#define RET_ERR_FAILURE			-1			/* Common error, operation not success */

/* Section Common*/
#define RET_ERR_BUSY				-2			/* Device status busy */
#define RET_ERR_TIMEOUT			-3			/* Waiting time out */


/* Section HLD, HAL 6~14*/
#define RET_ERR_DEV_ERROR		-10			/* Device work status error */
#define RET_ERR_DEV_CLASH		-11			/* Device clash for same device in queue */
#define RET_ERR_NO_DEV			-12			/* Device not exist on BUS */
#define RET_ERR_ID_FULL     -13     /* No more ID available */
#define RET_ERR_ID_FREE     -14     /* Specified ID isn't allocated yet */



/* Section Buffer */
#define RET_ERR_NO_MEM				-20			/* Not enough memory error */
#define RET_ERR_BUF_FULL			-21
#define RET_ERR_BUF_EMPTY			-22
#define RET_ERR_QUEUE_FULL		-23		/* Queue node count reached the max. val*/

/* Section Package */
#define RET_ERR_LONG_PACK			-30			/* Package too long */
#define RET_ERR_RUNT_PACK			-31			/* Package too short */



/* Section Transfer, Receive */
#define RET_ERR_TX_BUSY				-40			/* TX descriptor full */
#define RET_ERR_RX_BUSY				-41			/* RX descriptor full */
#define RET_ERR_NO_PACK     	-42     /* No packet received */


/* Section parameters */
#define RET_ERR_ERR_PARA      -50     /* Parameter is invalid */


/* Section OSD */
#define RET_ERR_OFF_SCRN    	-60     /* Block is out off the screen */
#define RET_ERR_V_OVRLAP    	-61     /* Block is overlaped in vertical */
#define RET_ERR_BAD_CLR     			-62     /* Invalid Color Mode code */
#define RET_ERR_OFF_BLOCK   			-63     /* Bitmap is out off the block */

/* Section Bluetooth 70~90 */


/* Section Flash */
#define RET_ERR_FLASH_OFFSET_ERROR  -100
#define RET_ERR_FLASH_NO_BUFFER     -101
#define RET_ERR_FLASH_NEED_ERASE    -102
#define RET_ERR_FLASH_WRITE_FAIL    -103
#define RET_ERR_FLASH_READ_FAIL    	-104

/* Section Temp Info */
#define RET_RET_ERR_TEMPINFO_ID_USED    -110
#define RET_ERR_TEMPINFO_NO_SPACE   		-111
#define RET_ERR_TEMPINFO_NO_MEMORY  		-112
#define RET_ERR_TEMPINFO_NOT_OPEN   		-113
#define RET_ERR_TEMPINFO_WRITE_FAIL 		-114
#define RET_ERR_TEMPINFO_NEED_ERASE 		-115

/* Section TWI */
#define RET_ERR_TWI_SCL_LOCK				-120		 	/* TWI SCL be locked */
#define RET_ERR_TWI_SDA_LOCK				-121			/* TWI SDA be locked */
#define RET_ERR_TWI_NO_ACK					-122			/* TWI slave no ack */

#ifdef __cplusplus
}
#endif

#endif // __SYSTEM_CONFIG_H__

