/*
*****************************************************************************
**

**  File        : LinkerScript.ld
**
**  Abstract    : Linker script for SLXXXX Device with
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : Smartlink Technology SLXXXX
**
**
**  Distribution: The file is distributed as is, without any warranty
**                of any kind.
**
**  (c)Copyright mName.
**  You may use this file as-is or modify it according to the needs of your
**  project. Distribution of this file (unmodified or modified) is not
**  permitted.
**
*****************************************************************************
*/
/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *   _estack
 */
/* Entry Point */
ENTRY(Reset_Handler)


/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x0000;    /* required amount of heap  */
_Min_Stack_Size = 0x0000;   /* required amount of stack */

BASE_LADDR          = 0x4030; /* 12K loader + 4K partition table + 0x30 firmware header */

VECTOR_LADDR        = 0x00000000;
VECTOR_ADDR         = 0x00800000;       /* keep it */
VECTOR_SIZE         = 0x400;

UPDATE_LADDR        = VECTOR_LADDR + VECTOR_SIZE;
UPDATE_ADDR         = 0x00804C00;       /* keep it */
UPDATE_SIZE         = 0x40;

STARTUP_LADDR       = UPDATE_LADDR + UPDATE_SIZE;
STARTUP_ADDR        = UPDATE_ADDR + UPDATE_SIZE;
STARTUP_SIZE        = 4K;

SRAM_LADDR          = STARTUP_LADDR + STARTUP_SIZE;
SRAM_ADDR           = STARTUP_ADDR + STARTUP_SIZE;
SRAM_SIZE           = 0x870000 - SRAM_ADDR;

SRAM7_ADDR          = 0x870000;
SRAM7_SIZE          = 64K;

/* Specify the memory areas */
MEMORY
{
    VECTOR(rxw):        ORIGIN = VECTOR_ADDR,           LENGTH = VECTOR_SIZE
    STARTUP(rxw):       ORIGIN = STARTUP_ADDR,          LENGTH = STARTUP_SIZE
    SRAM(rxw):          ORIGIN = SRAM_ADDR,             LENGTH = SRAM_SIZE
    SRAM7(rxw):         ORIGIN = SRAM7_ADDR,            LENGTH = SRAM7_SIZE
}

/* Define output sections */
SECTIONS
{
    .vector_sec : AT( "VECTOR_LADDR" )
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
    } >VECTOR

    .startup_sec : AT( "STARTUP_LADDR" )
    {
        . = ALIGN(4);
        KEEP(*(.text.Reset_Handler))
        KEEP(*(.text.Exception_Handler))
        KEEP(*(.startup_seg))
    } >STARTUP

    .sram_code_sec : AT( "SRAM_LADDR" )
    {
        . = ALIGN(4);
        KEEP(*(.eid_seg))

        *(.*_sram_seg)

        /* oal */
        *rtos_event.o(.text*)
        *rtos_event.o(.rodata*)
        *rtos_mem.o(.text*)
        *rtos_mem.o(.rodata*)
        *rtos_queue.o(.text*)
        *rtos_queue.o(.rodata*)
        *rtos_thread.o(.text*)
        *rtos_thread.o(.rodata*)

        /* sysview */
        *SEGGER_RTT.o(.text*)
        *SEGGER_SYSVIEW.o(.text*)
        *SEGGER_SYSVIEW_Config_FreeRTOS.o(.text*)
        *SEGGER_SYSVIEW_FreeRTOS.o(.text*)
        *SEGGER_RTT.o(.rodata*)
        *SEGGER_SYSVIEW.o(.rodata*)
        *SEGGER_SYSVIEW_Config_FreeRTOS.o(.rodata*)
        *SEGGER_SYSVIEW_FreeRTOS.o(.rodata*)

        /* psm */
        *psm-nvds-test.o(.text*)
        *psm-nvds-test.o(.rodata*)
        *psm-crc.o(.text*)
        *psm-crc.o(.rodata*)
        *psm-v2.o(.text*)
        *psm-v2.o(.rodata*)
        *psm-flash.o(.text*)
        *psm-flash.o(.rodata*)

        /* bt */
        *sltbt.o(.rodata*)
        *sltip.o(.rodata*)
        *ld_init.o(.rodata*)
        *bt_host_hal_uart.o(.rodata*)
        *bt_host_rfcomm.o(.text*)
        *bt_host_reconnect.o(.rodata*)
        *bt_host_avdtp.o(.rodata*)
        *bt_host_hs_hf.o(.rodata*)

        /* ble */
        *llc_llcp.o(.rodata*)
        *llm_task.o(.rodata*)
        *llm.o(.rodata*)
        *llc_task.o(.rodata*)
        *ble_find_device_top.o(.rodata*)

        /* 其它 */
        *hal_gpio.o(.rodata.CSWTCH*)
        *hal_gpio_lib.o(.rodata.CSWTCH*)
        *usbd_comp_storage.o(.rodata*)
        *uart.o(.text*)
        *uart.o(.rodata*)
        *charge.o(.rodata*)
        *hal_power_lib.o(.rodata*)
        *update.o(.rodata*)
        *update_api.o(.rodata*)

        /* 音频 */

        *(.isr_seg)
        *(.ccu_seg)

        /* 算法 */
        *reservoir.o(.text*)
        *encoder.o(.text*)
        *quantize.o(.text*)
        *bitstream.o(.rodata*)
        *encoder.o(.rodata*)
        //*lame.o(.rodata*)
        *newmdct.o(.rodata*)
        //*presets.o(.rodata*)
        *quantize.o(.rodata*)
        *quantize_pvt.o(.rodata*)
        *reservoir.o(.rodata*)
        *takehiro.o(.rodata*)
        //*util.o(.rodata*)
        *(.dump_seg)

        *sltk_math.o(.text*)
        *sltk_math.o(.rodata*)

        /* ota */
        *(.bt_ota_seg)
        *bt_ota.o(.rodata*)
        *bt_ota_bottom.o(.rodata*)
        *bt_ota_irq.o(.rodata*)
        *bt_ota_middle.o(.rodata*)

        . = ALIGN(4);
    } >SRAM

    SRAM_CODE_END = .;

    .data_sec : AT( SRAM_LADDR + SIZEOF(.sram_code_sec) )
    {
        . = ALIGN(4);
        _sdata = .;        /* create a global symbol at data start */
        *(SORT_BY_ALIGNMENT(.data))           /* .data sections */
        *(SORT_BY_ALIGNMENT(.data*))          /* .data* sections */

        . = ALIGN(4);
        _edata = .;        /* define a global symbol at data end */

        /* used by the startup to initialize data */
        _sidata = _sdata;
    } >SRAM

    SRAM_LOAD_END = .;

    .bss_start (NOLOAD):
    {
        . = ALIGN(4);
        _bss = .;
        _sbss = .;         /* define a global symbol at bss start */
        __bss_start__ = _sbss;
    } >SRAM

    /* Uninitialized data section */
    . = ALIGN(4);
    .app_bss_sec (NOLOAD):
    {
        /* This is used by the startup in order to initialize the .bss secion */
        *(SORT_BY_ALIGNMENT(.bss))
        *(SORT_BY_ALIGNMENT(.bss*))
    } >SRAM

    .user_heap (NOLOAD):
    {
        . = ALIGN(4);
        _heap = .;
        *(.os_heap)
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        _eheap = .;
        . = ALIGN(4);
    } >SRAM

    .app_bss_end (NOLOAD):
    {
        . = ALIGN(4);
        *(.app_stack)
         . = ALIGN(4);
        __app_stack_end  = .;

        . = ALIGN(4);
        _ebss = .;         /* define a global symbol at bss end */
        __bss_end__ = _ebss;
    } >SRAM

    .sram7_sec (NOLOAD):
    {
        . = ALIGN(4);
        *(.audio_engine_heap)
        . = ALIGN(4);
    } >SRAM7

    BANK_ALIGN = 0x2000;
    BANK_LADDR = (( SRAM_LADDR + SRAM_LOAD_END - SRAM_ADDR + BANK_ALIGN ) / BANK_ALIGN) * BANK_ALIGN;
    BANK_ADDR  =  0xC00000 + BANK_LADDR + BASE_LADDR;

    .bank_sec "BANK_ADDR": AT( "BANK_LADDR" )
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */
        *(.glue_7)         /* glue arm to thumb code */
        *(.glue_7t)        /* glue thumb to arm code */

        . = ALIGN(4);
        _etext = .;        /* define a global symbols at end of code */
    }

    .ARM.extab : AT(BANK_LADDR + SIZEOF(.bank_sec))
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    }

    .ARM : AT(BANK_LADDR + SIZEOF(.bank_sec) + SIZEOF(.ARM.extab))
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    }

    /* Remove information from the standard libraries */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

    __etext = _etext;
    __init_data = _sidata;
    __data_start__ = _sdata;
    _data = _sdata;
    __data_end__ = _edata;


    .ARM.attributes 0 : { *(.ARM.attributes) }
}
