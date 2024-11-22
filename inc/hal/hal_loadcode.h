#ifndef _HAL_LOADCODE_LIB_H_
#define _HAL_LOADCODE_LIB_H_
#define ENTRY                   extern char
#define ENTRY_TO_VALUE(entry)   ((int)(&entry))

ENTRY BASE_LADDR;

ENTRY VECTOR_SIZE;
ENTRY VECTOR_ADDR;
ENTRY VECTOR_LADDR;

/*
ENTRY DATA_SIZE;
ENTRY DATA_ADDR;
ENTRY DATA_LADDR;
*/

ENTRY LOAD_SIZE;

ENTRY BANK_LADDR;
ENTRY BANK_ADDR;
ENTRY BANK_SIZE;

ENTRY SRAM_SIZE;
ENTRY SRAM_ADDR;
ENTRY SRAM_LADDR;

ENTRY SRAM_CODE_END;
ENTRY SRAM_LOAD_END;

#define BASE_FLASH_ADDR     ENTRY_TO_VALUE(BASE_LADDR)

#define VECTOR_SIZE         ENTRY_TO_VALUE(VECTOR_SIZE)
#define VECTOR_RAM_ADDR     ENTRY_TO_VALUE(VECTOR_ADDR)
#define VECTOR_FLASH_ADDR   (ENTRY_TO_VALUE(VECTOR_LADDR) + BASE_FLASH_ADDR)

#define SRAM_SIZE           ENTRY_TO_VALUE(SRAM_SIZE)
#define SRAM_ADDR           ENTRY_TO_VALUE(SRAM_ADDR)
#define SRAM_FLASH_ADDR     (ENTRY_TO_VALUE(SRAM_LADDR) + BASE_FLASH_ADDR)
#define SRAM_CODE_SIZE      (ENTRY_TO_VALUE(SRAM_CODE_END) - SRAM_ADDR)
#define SRAM_LOAD_SIZE      (ENTRY_TO_VALUE(SRAM_LOAD_END) - SRAM_ADDR)

/*
#define DATA_SIZE           ENTRY_TO_VALUE(DATA_SIZE)
#define DATA_RAM_ADDR       ENTRY_TO_VALUE(DATA_ADDR)
#define DATA_FLASH_ADDR     (ENTRY_TO_VALUE(DATA_LADDR) + BASE_FLASH_ADDR)
*/

#define FLASH_RAM_ADDR      ENTRY_TO_VALUE(BANK_ADDR)
#define FLASH_RAM_SIZE      ENTRY_TO_VALUE(BANK_SIZE)

#define BANK_OFFSET         0x00

void load_sram_code(void);

#endif
