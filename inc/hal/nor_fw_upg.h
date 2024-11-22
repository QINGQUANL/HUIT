#ifndef _NOR_FW_UPG_H_
#define _NOR_FW_UPG_H_

//#include "nor_controller.h"



#define CMP_UPG_FMT_FLAG            0xC0
#define CMP_UPG_FMT_CID_FLAG    0x0F
#define CMP_NOR_PAGE_SIZE          0x100

enum NorUpgradeCmd {
    NOR_CMDF_ID_READ=0,
    NOR_CMDF_WRITE_BYTE,
    NOR_CMDF_WRITE_PAGE,
    NOR_CMDF_ERASE_SECTOR,
    NOR_CMDF_ERASE_32K,
    NOR_CMDF_ERASE_64K,
    NOR_CMDF_ERASE_CHIP,
    NOR_CMDF_READ,
    NOR_CMDF_READ_FAST,
    NOR_CMDS_WRITE_RAM,
    NOR_CMDS_RUN_RAM,


    NOR_CMDD_DRV_NONE=0x80,
    NOR_CMDD_INIT,
    NOR_CMDD_GET_CRC,
    NOR_CMDD_SYS_RST,
    NOR_CMDD_GET_CHIP_VER,
    NOR_CMDD_ENCRY_PROC,
};


typedef struct{
    uint8_t uFlag;
    uint8_t uCmd;
    uint8_t uClkDiv;
    uint8_t uLineMode;
    uint8_t uChipSelInDir;
    uint8_t uChipSelDir;
    uint8_t uChipTypeId;
} uUPDrvInitType;


typedef struct {
   uint8_t uFlag;
    uint8_t uCmd;
    uint8_t uDLen[4];
   uint8_t uAddr[4];
}uUPDrvCmdType;




typedef enum
{
  CMP_OK       = 0x00U,
  CMP_ERR_UPG_FMT    = 0x01U,
  CMP_ERR_UPG_PARA  = 0x02U,
} CMP_StatusTypeDef;


int32_t CMP_UPG_Nor_Init(uUPDrvInitType *uFmtData);
int32_t CMP_UPG_Nor_GetCRC(uint32_t *pDBuf,uint8_t *params);
int32_t CMP_UPG_Nor_SysReset();
int32_t CMP_UPG_Nor_GetChipVer();
int32_t CMP_UPG_Nor_Encry_Proc(uint16_t *eKey);
int32_t CMP_UPG_Norf_Read(uint8_t *buf, uint32_t blk_addr, uint16_t blk_len, uint8_t *pFmtData);
int32_t CMP_UPG_Norf_Write(uint8_t *buf, uint32_t blk_addr, uint16_t blk_len, uint8_t *pFmtData);
int32_t CMP_UPG_Norf_Comm(uint8_t* pDBuf ,uint8_t* pFmtData);


#endif

