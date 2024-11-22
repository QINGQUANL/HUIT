#ifndef _LL_SDMMC_H_
#define _LL_SDMMC_H_

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
//#include "sys_hal_def.h"
#include "slxxxx_reg.h"


/** @addtogroup
  * @{
  */

/** @addtogroup SDMMC_LL
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup SDMMC_LL_Exported_Types SDMMC_LL Exported Types
  * @{
  */

/**
  * @brief  SDMMC Configuration Structure definition
  */
typedef struct
{
    unsigned int ClockPowerSave;       /*!< Specifies whether SDIO Clock output is enabled or disabled when the bus is idle.*/
    unsigned int BusWide;              /*!< Specifies the SDIO bus width. This parameter can be a value of @ref SDIO_Bus_Wide                   */
    unsigned int MulTxThre;
    unsigned int TxFIFOThre;
    unsigned int RxFIFOThre;
    unsigned int DebounceCnt;               // Card detect timeout
    unsigned int ProgramBurstLen;
    unsigned int DescSkipLen;
    unsigned int FixedBurstEn;
    unsigned int DescListBAddr;
    unsigned int ClockDiv;
    unsigned int IDMAIntEn;
    unsigned int DRespTimeout;
#if (SYS_CHIP_ID == CHIP_SL6800)
    uint32_t read_mode;
    uint32_t write_mode;
#endif
}SDIO_InitTypeDef;


/**
  * @brief  SDIO Command Control structure
  */
typedef struct
{
  uint32_t Argument;            /*!< Specifies the SDIO command argument which is sent
                                                     to a card as part of a command message. If a command
                                                     contains an argument, it must be loaded into this register
                                                     before writing the command to the command register.              */
    uint32_t CmdConfig;
}SDIO_CmdInitTypeDef;


/**
  * @brief  SDIO Data Control structure
  */
typedef struct
{



    uint32_t DataRespTimeout;
  uint32_t DataByteCount;       /*!< Specifies the number of data bytes to be transferred.         */

  uint32_t DataBlockSize;       /*!< Specifies the data block size for block transfer.
                                                    This parameter can be a value of @ref SDIO_Data_Block_Size    */

}SDIO_DataInitTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup SDMMC_LL_Exported_Constants SDMMC_LL Exported Constants
  * @{
  */

/** @defgroup SDIO_Clock_Edge Clock Edge
  * @{
  */

#define SDIO_CLOCK_EDGE_RISING              ((uint32_t)0x00000000)
#define SDIO_CLOCK_EDGE_FALLING             SDIO_CLKCR_NEGEDGE

#define IS_SDIO_CLOCK_EDGE(EDGE)            (((EDGE) == SDIO_CLOCK_EDGE_RISING) || \
                                            ((EDGE) == SDIO_CLOCK_EDGE_FALLING))
/**
  * @}
  */

/** @defgroup SDIO_Clock_Bypass Clock Bypass
  * @{
  */
#define SDIO_CLOCK_BYPASS_DISABLE           ((uint32_t)0x00000000)
#define SDIO_CLOCK_BYPASS_ENABLE            SDIO_CLKCR_BYPASS

#define IS_SDIO_CLOCK_BYPASS(BYPASS)        (((BYPASS) == SDIO_CLOCK_BYPASS_DISABLE) || \
                                            ((BYPASS) == SDIO_CLOCK_BYPASS_ENABLE))
/**
  * @}
  */

/** @defgroup SDIO_Clock_Power_Save Clock Power Saving
  * @{
  */
#ifndef SD_DBG
#define SDIO_CLOCK_POWER_SAVE_DISABLE       ((uint32_t)0x00000000)
#define SDIO_CLOCK_POWER_SAVE_ENABLE        SDIO_CR_CLK_LPOW

#define IS_SDIO_CLOCK_POWER_SAVE(SAVE)      (((SAVE) == SDIO_CLOCK_POWER_SAVE_DISABLE) || \
                                            ((SAVE) == SDIO_CLOCK_POWER_SAVE_ENABLE))

/**
  * @}
  */

/** @defgroup SDIO_Bus_Wide Bus Width
  * @{
  */
#define SDIO_BUS_WIDE_1B                    SDIO_CTR_BITMODE_1BIT
#define SDIO_BUS_WIDE_4B                    SDIO_CTR_BITMODE_4BIT

#define IS_SDIO_BUS_WIDE(WIDE)              (((WIDE) == SDIO_BUS_WIDE_1B) || \
                                            ((WIDE) == SDIO_BUS_WIDE_4B))

#endif

#define SDIO_IDMA_ENABLE                    ((uint32_t)0x00000001)
#define SDIO_IDMA_DISABLE                   ((uint32_t)0x00000000)

#define SDIO_MULTI_TX_BSIZE_1               SDIO_FTR_MULTX_BURSSIZE_1
#define SDIO_MULTI_TX_BSIZE_4               SDIO_FTR_MULTX_BURSSIZE_4
#define SDIO_MULTI_TX_BSIZE_8               SDIO_FTR_MULTX_BURSSIZE_8
#define SDIO_MULTI_TX_BSIZE_16              SDIO_FTR_MULTX_BURSSIZE_16
#define SDIO_MULTI_TX_BSIZE_32              SDIO_FTR_MULTX_BURSSIZE_32
#define SDIO_MULTI_TX_BSIZE_64              SDIO_FTR_MULTX_BURSSIZE_64
#define SDIO_MULTI_TX_BSIZE_128             SDIO_FTR_MULTX_BURSSIZE_128
#define SDIO_MULTI_TX_BSIZE_256             SDIO_FTR_MULTX_BURSSIZE_256
#define IS_SDIO_MULTI_TX_BSIZE(SIZE)        (((SIZE) == SDIO_MULTI_TX_BSIZE_1) || \
                                            ((SIZE) == SDIO_MULTI_TX_BSIZE_4) || \
                                            ((SIZE) == SDIO_MULTI_TX_BSIZE_8) || \
                                            ((SIZE) == SDIO_MULTI_TX_BSIZE_16) || \
                                            ((SIZE) == SDIO_MULTI_TX_BSIZE_32) || \
                                            ((SIZE) == SDIO_MULTI_TX_BSIZE_64) || \
                                            ((SIZE) == SDIO_MULTI_TX_BSIZE_128) || \
                                            ((SIZE) == SDIO_MULTI_TX_BSIZE_256))

#define IS_SDIO_RX_FIFO_THRESHOLD(THR)      (THR < SDIO_FTR_TX_FIFO_THR)

#define IS_SDIO_TX_FIFO_THRESHOLD(THR)      (THR < ( SDIO_FTR_RX_FIFO_THR >> POSITION_VAL(SDIO_FTR_RX_FIFO_THR)))

#define IS_SDIO_DEBOUNCE_COUNT(COUNT)       (COUNT < SDIO_DBCR)

#define SDIO_PROGRAM_BURST_LEN_1            SDIO_BMR_PROG_BURSLEN_1
#define SDIO_PROGRAM_BURST_LEN_4            SDIO_BMR_PROG_BURSLEN_4
#define SDIO_PROGRAM_BURST_LEN_8            SDIO_BMR_PROG_BURSLEN_8
#define SDIO_PROGRAM_BURST_LEN_16           SDIO_BMR_PROG_BURSLEN_16
#define SDIO_PROGRAM_BURST_LEN_32           SDIO_BMR_PROG_BURSLEN_32
#define SDIO_PROGRAM_BURST_LEN_64           SDIO_BMR_PROG_BURSLEN_64
#define SDIO_PROGRAM_BURST_LEN_128          SDIO_BMR_PROG_BURSLEN_128
#define SDIO_PROGRAM_BURST_LEN_256          SDIO_BMR_PROG_BURSLEN_256
#define IS_SDIO_PROGRAM_BURST_LEN(LENGTH)   (((LENGTH) == SDIO_PROGRAM_BURST_LEN_1) || \
                                            ((LENGTH) == SDIO_PROGRAM_BURST_LEN_4) || \
                                            ((LENGTH) == SDIO_PROGRAM_BURST_LEN_8) || \
                                            ((LENGTH) == SDIO_PROGRAM_BURST_LEN_16) || \
                                            ((LENGTH) == SDIO_PROGRAM_BURST_LEN_32) || \
                                            ((LENGTH) == SDIO_PROGRAM_BURST_LEN_64) || \
                                            ((LENGTH) == SDIO_PROGRAM_BURST_LEN_128) || \
                                            ((LENGTH) == SDIO_PROGRAM_BURST_LEN_256))

#define IS_SDIO_DESC_SKIP_LENGTH(LENGTH)    (LENGTH < (SDIO_BMR_DESC_SKIPLEN >> POSITION_VAL(SDIO_BMR_DESC_SKIPLEN)))


#define SDIO_FIX_BURST_ENABLE               SDIO_BMR_FIX_BURST
#define SDIO_FIX_BURST_DISABLE              ((uint32_t)0x00000000)
#define IS_SDIO_FIXED_BURST_ENABLE(ENABLE)  (((ENABLE) == SDIO_FIX_BURST_ENABLE) || \
                                            ((ENABLE) == SDIO_FIX_BURST_DISABLE))

#define IS_SDIO_DESC_LIST_BASE_ADDR(ADDR)   (ADDR > 0x00)


/**
  * @}
  */

/** @defgroup SDIO_Hardware_Flow_Control Hardware Flow Control
  * @{
  */

#define SDIO_HARDWARE_FLOW_CONTROL_DISABLE      ((uint32_t)0x00000000)
#define SDIO_HARDWARE_FLOW_CONTROL_ENABLE       SDIO_CLKCR_HWFC_EN

#define IS_SDIO_HARDWARE_FLOW_CONTROL(CONTROL)  (((CONTROL) == SDIO_HARDWARE_FLOW_CONTROL_DISABLE) || \
                                                ((CONTROL) == SDIO_HARDWARE_FLOW_CONTROL_ENABLE))
/**
  * @}
  */

/** @defgroup SDIO_Clock_Division Clock Division
  * @{
  */
#define IS_SDIO_CLKDIV(DIV)   ((DIV) <= 0xFF)
/**
  * @}
  */

/** @defgroup SDIO_Command_Index Command Index
  * @{
  */
#define IS_SDIO_CMD_INDEX(INDEX)            ((INDEX) < 0x40)
/**
  * @}
  */

/** @defgroup SDIO_Response_Type Response Type
  * @{
  */
#define SDIO_RESPONSE_NO                    SDIO_CMDR_RESP_NONE
#define SDIO_RESPONSE_SHORT                 SDIO_CMDR_RESP_SHORT
#define SDIO_RESPONSE_LONG                  SDIO_CMDR_RESP_LONG

#define IS_SDIO_RESPONSE(RESPONSE)          (((RESPONSE) == SDIO_RESPONSE_NO) || \
                                            ((RESPONSE) == SDIO_RESPONSE_SHORT)|| \
                                            ((RESPONSE) == SDIO_RESPONSE_LONG))

#define SDIO_CMD_RESPCRC_CHECK_EN           SDIO_CMDR_CHECK_RESPCRC
#define SDIO_CMD_RESPCRC_CHECK_DIS          ((uint32_t)0x00000000)
#define IS_SDIO_RESP_CRC(PARAM)             (((PARAM) == SDIO_CMD_RESPCRC_CHECK_EN) || \
                                            ((PARAM) == SDIO_CMD_RESPCRC_CHECK_DIS))

#define SDIO_CMD_DATA_EXPECT                SDIO_CMDR_DATA_EXPECT
#define SDIO_CMD_DATA_UNEXPECT              ((uint32_t)0x00000000)
#define IS_SDIO_DATA_EXPECT(PARAM)          (((PARAM) == SDIO_CMD_DATA_EXPECT) || \
                                            ((PARAM) == SDIO_CMD_DATA_UNEXPECT))

#define SDIO_CMD_DATA_WRITE                 SDIO_CMDR_RW_DIR
#define SDIO_CMD_DATA_READ                  ((uint32_t)0x00000000)
#define IS_SDIO_DATA_DIRECTION(PARAM)       (((PARAM) == SDIO_CMD_DATA_WRITE) || \
                                            ((PARAM) == SDIO_CMD_DATA_READ))

#define SDIO_CMD_TRANS_MODE_STREAM          SDIO_CMDR_TXMODE
#define SDIO_CMD_TRANS_MODE_BLOCK           ((uint32_t)0x00000000)
#define IS_SDIO_TRANS_MODE(PARAM)           (((PARAM) == SDIO_CMD_TRANS_MODE_STREAM) || \
                                            ((PARAM) == SDIO_CMD_TRANS_MODE_BLOCK))

#define SDIO_CMD_AUTOSTOP_ENABLE            SDIO_CMDR_SEND_AUTOSTOP
#define SDIO_CMD_AUTOSTOP_DISABLE           ((uint32_t)0x00000000)
#define IS_SDIO_AUTOSTOP(PARAM)             (((PARAM) == SDIO_CMD_AUTOSTOP_ENABLE) || \
                                            ((PARAM) == SDIO_CMD_AUTOSTOP_DISABLE))

#define SDIO_CMD_WAIT_END_EN                SDIO_CMDR_WAIT_PREDATA
#define SDIO_CMD_WAIT_END_DIS               ((uint32_t)0x00000000)
#define IS_SDIO_WAIT_END(PARAM)             (((PARAM) == SDIO_CMD_WAIT_END_EN) || \
                                            ((PARAM) == SDIO_CMD_WAIT_END_DIS))

#define SDIO_CMD_STOP_ABORT_EN              SDIO_CMDR_STOP_ABORT
#define SDIO_CMD_STOP_ABORT_DIS             ((uint32_t)0x00000000)
#define IS_SDIO_STOP_ABORT(PARAM)           (((PARAM) == SDIO_CMD_STOP_ABORT_EN) || \
                                            ((PARAM) == SDIO_CMD_STOP_ABORT_DIS))

#define SDIO_CMD_SEND_INIT_EN               SDIO_CMDR_SEND_INIT
#define SDIO_CMD_SEND_INIT_DIS              ((uint32_t)0x00000000)
#define IS_SDIO_SEND_INIT(PARAM)            (((PARAM) == SDIO_CMD_SEND_INIT_EN) || \
                                            ((PARAM) == SDIO_CMD_SEND_INIT_DIS))

#define SDIO_CMD_UPDATE_CLK_EN              SDIO_CMDR_CLK_UPDATE
#define SDIO_CMD_UPDATE_CLK_DIS             ((uint32_t)0x00000000)
#define IS_SDIO_UPDATE_CLK(PARAM)           (((PARAM) == SDIO_CMD_UPDATE_CLK_EN) || \
                                            ((PARAM) == SDIO_CMD_UPDATE_CLK_DIS))


/**
  * @}
  */

/** @defgroup SDIO_Wait_Interrupt_State Wait Interrupt
  * @{
  */
#define SDIO_WAIT_NO                        ((uint32_t)0x00000000)
#define SDIO_WAIT_PEND                      SDIO_CMDR_WAIT_PREDATA

#define IS_SDIO_WAIT(WAIT) (((WAIT) == SDIO_WAIT_NO) || \
                            ((WAIT) == SDIO_WAIT_PEND))
/**
  * @}
  */

/** @defgroup SDIO_CPSM_State CPSM State
  * @{
  */
#define SDIO_CPSM_DISABLE                   ((uint32_t)0x00000000)
#define SDIO_CPSM_ENABLE                    ((uint32_t)0x00000000) //SDIO_CMD_CPSMEN

#define IS_SDIO_CPSM(CPSM) (((CPSM) == SDIO_CPSM_DISABLE) || \
                            ((CPSM) == SDIO_CPSM_ENABLE))
/**
  * @}
  */

/** @defgroup SDIO_Response_Registers Response Register
  * @{
  */
#define SDIO_RESP1                          ((uint32_t)0x00000000)
#define SDIO_RESP2                          ((uint32_t)0x00000004)
#define SDIO_RESP3                          ((uint32_t)0x00000008)
#define SDIO_RESP4                          ((uint32_t)0x0000000C)

#define IS_SDIO_RESP(RESP) (((RESP) == SDIO_RESP1) || \
                            ((RESP) == SDIO_RESP2) || \
                            ((RESP) == SDIO_RESP3) || \
                            ((RESP) == SDIO_RESP4))
/**
  * @}
  */

/** @defgroup SDIO_Data_Length Data Lenght
  * @{
  */
#define IS_SDIO_DATA_LENGTH(LENGTH) ((LENGTH) <= 0x01FFFFFF)
/**
  * @}
  */

/** @defgroup SDIO_Data_Block_Size Data Block Size
  * @{
  */
#define SDIO_DATABLOCK_SIZE_1B               SDIO_BSR_BIT_00
#define SDIO_DATABLOCK_SIZE_2B               SDIO_BSR_BIT_01
#define SDIO_DATABLOCK_SIZE_4B               SDIO_BSR_BIT_02
#define SDIO_DATABLOCK_SIZE_8B               SDIO_BSR_BIT_03
#define SDIO_DATABLOCK_SIZE_16B              SDIO_BSR_BIT_04
#define SDIO_DATABLOCK_SIZE_32B              SDIO_BSR_BIT_05
#define SDIO_DATABLOCK_SIZE_64B              SDIO_BSR_BIT_06
#define SDIO_DATABLOCK_SIZE_128B             SDIO_BSR_BIT_07
#define SDIO_DATABLOCK_SIZE_256B             SDIO_BSR_BIT_08
#define SDIO_DATABLOCK_SIZE_512B             SDIO_BSR_BIT_09
#define SDIO_DATABLOCK_SIZE_1024B            SDIO_BSR_BIT_10
#define SDIO_DATABLOCK_SIZE_2048B            SDIO_BSR_BIT_11
#define SDIO_DATABLOCK_SIZE_4096B            SDIO_BSR_BIT_12
#define SDIO_DATABLOCK_SIZE_8192B            SDIO_BSR_BIT_13
#define SDIO_DATABLOCK_SIZE_16384B           SDIO_BSR_BIT_14
#define SDIO_DATABLOCK_SIZE_32768B                   SDIO_BSR_BIT_15

#define IS_SDIO_BLOCK_SIZE(SIZE) (((SIZE) == SDIO_DATABLOCK_SIZE_1B)    || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_2B)    || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_4B)    || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_8B)    || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_16B)   || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_32B)   || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_64B)   || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_128B)  || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_256B)  || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_512B)  || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_1024B) || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_2048B) || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_4096B) || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_8192B) || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_16384B) || \
                                  ((SIZE) == SDIO_DATABLOCK_SIZE_32768B))
/**
  * @}
  */

/** @defgroup SDIO_Transfer_Direction Transfer Direction
  * @{
  */
#define SDIO_TRANSFER_DIR_TO_CARD            SDIO_CMDR_RW_DIR
#define SDIO_TRANSFER_DIR_TO_SDIO            ((uint32_t)0x00000000)

#define IS_SDIO_TRANSFER_DIR(DIR) (((DIR) == SDIO_TRANSFER_DIR_TO_CARD) || \
                                   ((DIR) == SDIO_TRANSFER_DIR_TO_SDIO))
/**
  * @}
  */

/** @defgroup SDIO_Transfer_Type Transfer Type
  * @{
  */
#define SDIO_TRANSFER_MODE_BLOCK             ((uint32_t)0x00000000)
#define SDIO_TRANSFER_MODE_STREAM            SDIO_CMDR_TXMODE

#define IS_SDIO_TRANSFER_MODE(MODE) (((MODE) == SDIO_TRANSFER_MODE_BLOCK) || \
                                     ((MODE) == SDIO_TRANSFER_MODE_STREAM))
/**
  * @}
  */

/** @defgroup SDIO_DPSM_State DPSM State
  * @{
  */
#define SDIO_DPSM_DISABLE                    ((uint32_t)0x00000000)
#define SDIO_DPSM_ENABLE                     ((uint32_t)0x00000000) //SDIO_DCTRL_DTEN

#define IS_SDIO_DPSM(DPSM) (((DPSM) == SDIO_DPSM_DISABLE) ||\
                            ((DPSM) == SDIO_DPSM_ENABLE))
/**
  * @}
  */

/** @defgroup SDIO_Read_Wait_Mode Read Wait Mode
  * @{
  */
#define SDIO_READ_WAIT_MODE_DATA2             ((uint32_t)0x00000000)
#define SDIO_READ_WAIT_MODE_CLK               ((uint32_t)0x00000001)

#define IS_SDIO_READWAIT_MODE(MODE) (((MODE) == SDIO_READ_WAIT_MODE_CLK) || \
                                     ((MODE) == SDIO_READ_WAIT_MODE_DATA2))
/**
  * @}
  */


#define SDIO_IT_CCRCFAIL                    ((uint32_t)0x00000000)
#define SDIO_IT_DCRCFAIL                    ((uint32_t)0x00000000)
#define SDIO_IT_CTIMEOUT                    ((uint32_t)0x00000000)
#define SDIO_IT_DTIMEOUT                    ((uint32_t)0x00000000)
#define SDIO_IT_TXUNDERR                    ((uint32_t)0x00000000)
#define SDIO_IT_RXOVERR                     ((uint32_t)0x00000000)
#define SDIO_IT_CMDREND                     ((uint32_t)0x00000000)
#define SDIO_IT_CMDSENT                     ((uint32_t)0x00000000)
#define SDIO_IT_DATAEND                     ((uint32_t)0x00000000)
#define SDIO_IT_STBITERR                    ((uint32_t)0x00000000)
#define SDIO_IT_DBCKEND                     ((uint32_t)0x00000000)
#define SDIO_IT_CMDACT                      ((uint32_t)0x00000000)
#define SDIO_IT_TXACT                       ((uint32_t)0x00000000)
#define SDIO_IT_RXACT                       ((uint32_t)0x00000000)
#define SDIO_IT_TXFIFOHE                    ((uint32_t)0x00000000)
#define SDIO_IT_RXFIFOHF                    ((uint32_t)0x00000000)
#define SDIO_IT_TXFIFOF                     ((uint32_t)0x00000000)
#define SDIO_IT_RXFIFOF                     ((uint32_t)0x00000000)
#define SDIO_IT_TXFIFOE                     ((uint32_t)0x00000000)
#define SDIO_IT_RXFIFOE                     ((uint32_t)0x00000000)
#define SDIO_IT_TXDAVL                      ((uint32_t)0x00000000)
#define SDIO_IT_RXDAVL                      ((uint32_t)0x00000000)
#define SDIO_IT_SDIOIT                      ((uint32_t)0x00000000)
#define SDIO_IT_CEATAEND                    ((uint32_t)0x00000000)
/**
  * @}
  */

/** @defgroup SDIO_Flags Flags
  * @{
  */
#define SDIO_FLAG_CCRCFAIL                  ((uint32_t)0x00000000)
#define SDIO_FLAG_DCRCFAIL                  ((uint32_t)0x00000000)
#define SDIO_FLAG_CTIMEOUT                  ((uint32_t)0x00000000)
#define SDIO_FLAG_DTIMEOUT                  ((uint32_t)0x00000000)
#define SDIO_FLAG_TXUNDERR                  ((uint32_t)0x00000000)
#define SDIO_FLAG_RXOVERR                   ((uint32_t)0x00000000)
#define SDIO_FLAG_CMDREND                   ((uint32_t)0x00000000)
#define SDIO_FLAG_CMDSENT                   ((uint32_t)0x00000000)
#define SDIO_FLAG_DATAEND                   ((uint32_t)0x00000000)
#define SDIO_FLAG_STBITERR                  ((uint32_t)0x00000000)
#define SDIO_FLAG_DBCKEND                   ((uint32_t)0x00000000)
#define SDIO_FLAG_CMDACT                    ((uint32_t)0x00000000)
#define SDIO_FLAG_TXACT                     ((uint32_t)0x00000000)
#define SDIO_FLAG_RXACT                     ((uint32_t)0x00000000)
#define SDIO_FLAG_TXFIFOHE                  ((uint32_t)0x00000000)
#define SDIO_FLAG_RXFIFOHF                  ((uint32_t)0x00000000)
#define SDIO_FLAG_TXFIFOF                   ((uint32_t)0x00000000)
#define SDIO_FLAG_RXFIFOF                   ((uint32_t)0x00000000)
#define SDIO_FLAG_TXFIFOE                   ((uint32_t)0x00000000)
#define SDIO_FLAG_RXFIFOE                   ((uint32_t)0x00000000)
#define SDIO_FLAG_TXDAVL                    ((uint32_t)0x00000000)
#define SDIO_FLAG_RXDAVL                    ((uint32_t)0x00000000)
#define SDIO_FLAG_SDIOIT                    ((uint32_t)0x00000000)
#define SDIO_FLAG_CEATAEND                  ((uint32_t)0x00000000)


#define SDIO_IT_CRDDETECED                  ((uint32_t)0x00000001)
#define SDIO_IT_RESPERR                         ((uint32_t)0x00000002)
#define SDIO_IT_CMDDONE                         ((uint32_t)0x00000004)
#define SDIO_IT_DTXRXOVER                   ((uint32_t)0x00000008)
#define SDIO_IT_TXFIFODREQ                  ((uint32_t)0x00000010)
#define SDIO_IT_RXFIFODREQ                  ((uint32_t)0x00000020)
#define SDIO_IT_RESPCRCERR                  ((uint32_t)0x00000040)
#define SDIO_IT_DATACRCERR                  ((uint32_t)0x00000080)
#define SDIO_IT_RESPTMOUT                   ((uint32_t)0x00000100)
#define SDIO_IT_DREADTMOUT                  ((uint32_t)0x00000200)
#define SDIO_IT_DSTARVBYHOST                ((uint32_t)0x00000400)
#define SDIO_IT_FIFOUORUNERR                ((uint32_t)0x00000800)
#define SDIO_IT_HWLOCKWRERR                 ((uint32_t)0x00001000)
#define SDIO_IT_SBITERR                     ((uint32_t)0x00002000)
#define SDIO_IT_AUTOCMDDONE                 ((uint32_t)0x00004000)
#define SDIO_IT_EBITERR                         ((uint32_t)0x00008000)


#define SDIO_FLAG_CRDDETECED                  ((uint32_t)0x00000001)
#define SDIO_FLAG_RESPERR                       ((uint32_t)0x00000002)
#define SDIO_FLAG_CMDDONE                       ((uint32_t)0x00000004)
#define SDIO_FLAG_DTXRXOVER                  ((uint32_t)0x00000008)
#define SDIO_FLAG_TXFIFODREQ                  ((uint32_t)0x00000010)
#define SDIO_FLAG_RXFIFODREQ                  ((uint32_t)0x00000020)
#define SDIO_FLAG_RESPCRCERR                  ((uint32_t)0x00000040)
#define SDIO_FLAG_DATACRCERR                  ((uint32_t)0x00000080)
#define SDIO_FLAG_RESPTMOUT                   ((uint32_t)0x00000100)
#define SDIO_FLAG_DREADTMOUT                  ((uint32_t)0x00000200)
#define SDIO_FLAG_DSTARVBYHOST                ((uint32_t)0x00000400)
#define SDIO_FLAG_FIFOUORUNERR                ((uint32_t)0x00000800)
#define SDIO_FLAG_HWLOCKWRERR                 ((uint32_t)0x00001000)
#define SDIO_FLAG_SBIT_BCLR_ERR               ((uint32_t)0x00002000)
#define SDIO_FLAG_AUTOCMDDONE                 ((uint32_t)0x00004000)
#define SDIO_FLAG_EBITERR                       ((uint32_t)0x00008000)


/**
  * @}
  */

/**
  * @}
  */
/* Exported macro ------------------------------------------------------------*/
/** @defgroup SDMMC_LL_Exported_macros SDMMC_LL Exported Macros
  * @{
  */


/* --- CMD Register ---*/
/* CMD Register clear mask */
#define CMD_CLEAR_MASK           ((uint32_t)SDIO_CMDR)


/* SDIO Initialization Frequency (400KHz max) */
//#define SDIO_INIT_CLK_DIV ((uint8_t)0x76)
#define SDIO_INIT_CLK_DIV       ((uint32_t)CCU_SDCLKCFR_PREDIV_8 | CCU_SDCLKCFR_DIV_8)

/* SDIO Data Transfer Frequency (25MHz max) */
//#define SDIO_TRANSFER_CLK_DIV ((uint8_t)0x0)
//#define SDIO_TRANSFER_CLK_DIV ((uint8_t)CCU_SDCLKCFR_PREDIV_1 | CCU_SDCLKCFR_DIV_1)
#define SDIO_TRANSFER_CLK_DIV2 ((uint8_t)CCU_SDCLKCFR_PREDIV_2 | CCU_SDCLKCFR_DIV_1)

/**
  * @}
  */

/** @defgroup SDMMC_LL_Interrupt_Clock Interrupt And Clock Configuration
  * @brief macros to handle interrupts and specific clock configurations
  * @{
  */



#define __SDIO_ENABLE(__INSTANCE__)   //((__INSTANCE__)->CR |= (SDIO_CR_CLK_EN))

#define __SDIO_DISABLE(__INSTANCE__)   //((__INSTANCE__)->CR &= ~(SDIO_CR_CLK_EN))

#define __SDIO_DMA_ENABLE(__INSTANCE__)  ((__INSTANCE__)->CR |= (SDIO_CR_IDMA_EN))

#define __SDIO_DMA_DISABLE(__INSTANCE__)   ((__INSTANCE__)->CR &= ~(SDIO_CR_IDMA_EN))

#define __SDIO_ENABLE_IT(__INSTANCE__, __INTERRUPT__)  //((__INSTANCE__)->MASK |= (__INTERRUPT__))

#define __SDIO_DISABLE_IT(__INSTANCE__, __INTERRUPT__) // ((__INSTANCE__)->MASK &= ~(__INTERRUPT__))

#define __SDIO_GET_FLAG(__INSTANCE__, __FLAG__)   (((__INSTANCE__)->RISR &(__FLAG__)) != RESET)

#define __SDIO_CLEAR_FLAG(__INSTANCE__, __FLAG__)  ((__INSTANCE__)->RISR = (__FLAG__))

#define __SDIO_GET_DMA_FLAG(__INSTANCE__, __FLAG__)   (((__INSTANCE__)->DMASR &(__FLAG__)) != RESET)

#define __SDIO_CLEAR_DMA_FLAG(__INSTANCE__, __FLAG__)  ((__INSTANCE__)->DMASR = (__FLAG__))

#define __SDIO_GET_FLAG_BITS(__INSTANCE__, __FLAG__)   ((__INSTANCE__)->RISR &(__FLAG__))

#define __SDIO_GET_IT(__INSTANCE__, __INTERRUPT__)   //(((__INSTANCE__)->STA &(__INTERRUPT__)) == (__INTERRUPT__))

#define __SDIO_CLEAR_IT(__INSTANCE__, __INTERRUPT__)   //((__INSTANCE__)->ICR = (__INTERRUPT__))

#define __SDIO_START_READWAIT_ENABLE()   //(*(__IO uint32_t *) DCTRL_RWSTART_BB = ENABLE)

#define __SDIO_START_READWAIT_DISABLE()   //(*(__IO uint32_t *) DCTRL_RWSTART_BB = DISABLE)

#define __SDIO_STOP_READWAIT_ENABLE()  // (*(__IO uint32_t *) DCTRL_RWSTOP_BB = ENABLE)

#define __SDIO_STOP_READWAIT_DISABLE()  // (*(__IO uint32_t *) DCTRL_RWSTOP_BB = DISABLE)

#define __SDIO_OPERATION_ENABLE()   //(*(__IO uint32_t *) DCTRL_SDIOEN_BB = ENABLE)

#define __SDIO_OPERATION_DISABLE()   //(*(__IO uint32_t *) DCTRL_SDIOEN_BB = DISABLE)

#define __SDIO_SUSPEND_CMD_ENABLE()   //(*(__IO uint32_t *) CMD_SDIOSUSPEND_BB = ENABLE)

#define __SDIO_SUSPEND_CMD_DISABLE()  // (*(__IO uint32_t *) CMD_SDIOSUSPEND_BB = DISABLE)



/* Exported functions --------------------------------------------------------*/
/** @addtogroup SDMMC_LL_Exported_Functions
  * @{
  */

/* Initialization/de-initialization functions  **********************************/
/** @addtogroup HAL_SDMMC_LL_Group1
  * @{
  */
HAL_StatusTypeDef SDIO_Init(SDIO_TypeDef *SDIOx, SDIO_InitTypeDef Init);
/**
  * @}
  */

/* I/O operation functions  *****************************************************/
/** @addtogroup HAL_SDMMC_LL_Group2
  * @{
  */
/* Blocking mode: Polling */
uint32_t          SDIO_ReadFIFO(SDIO_TypeDef *SDIOx);
HAL_StatusTypeDef SDIO_WriteFIFO(SDIO_TypeDef *SDIOx, uint32_t *pWriteData);
/**
  * @}
  */

/* Peripheral Control functions  ************************************************/
/** @addtogroup HAL_SDMMC_LL_Group3
  * @{
  */
HAL_StatusTypeDef SDIO_PowerState_ON(SDIO_TypeDef *SDIOx);
HAL_StatusTypeDef SDIO_PowerState_OFF(SDIO_TypeDef *SDIOx);
uint32_t          SDIO_GetPowerState(SDIO_TypeDef *SDIOx);

/* Command path state machine (CPSM) management functions */
HAL_StatusTypeDef SDIO_SendCommand(SDIO_TypeDef *SDIOx, SDIO_CmdInitTypeDef *SDIO_CmdInitStruct);
uint8_t           SDIO_GetCommandResponse(SDIO_TypeDef *SDIOx);
uint32_t          SDIO_GetResponse(uint32_t SDIO_RESP);

/* Data path state machine (DPSM) management functions */
HAL_StatusTypeDef SDIO_DataConfig(SDIO_TypeDef *SDIOx, SDIO_DataInitTypeDef* SDIO_DataInitStruct);
uint32_t          SDIO_GetFIFOCount(SDIO_TypeDef *SDIOx);

uint32_t SDIO_SetBusWide(SDIO_TypeDef *SDIOx, uint32_t busWide);

#ifdef COMPATIBLE_FUNC
uint32_t          SDIO_GetDataCounter(SDIO_TypeDef *SDIOx);
/* SDIO IO Cards mode management functions */
HAL_StatusTypeDef SDIO_SetSDIOReadWaitMode(uint32_t SDIO_ReadWaitMode);


#endif


#ifdef __cplusplus
}
#endif

#endif

