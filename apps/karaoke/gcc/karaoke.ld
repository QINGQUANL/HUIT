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

OVERLAY_SRAM_LADDR  = STARTUP_LADDR + STARTUP_SIZE;
OVERLAY_SRAM_ADDR   = STARTUP_ADDR + STARTUP_SIZE;

_G_SRAM_ADDR           = OVERLAY_SRAM_ADDR;
_G_SRAM_SIZE           = 0x87D800 - _G_SRAM_ADDR;

SRAM7_ADDR          = 0x87D800;
SRAM7_SIZE          = 10K;

/* Specify the memory areas */
MEMORY
{
    VECTOR(rxw):        ORIGIN = VECTOR_ADDR,           LENGTH = VECTOR_SIZE
    STARTUP(rxw):       ORIGIN = STARTUP_ADDR,          LENGTH = STARTUP_SIZE
    SRAM(rxw):          ORIGIN = _G_SRAM_ADDR,          LENGTH = _G_SRAM_SIZE
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
	
	OVERLAY : AT( "OVERLAY_SRAM_LADDR" )
	{
	
		/**/
		g24_sec
		{
			. = ALIGN(4);
			*(.*_sram_g24_seg) 
			
			*(.usbd_seg)
			*(.bt_24g_isr_comm_seg_*)
			*(.sbt_seg_*)
			*(.bt_24g_task_seg)
			*(.sd)
			
			*usb_device(.text*)
			*usb_device(.bss*)
			*usbd_audio(.text*)
			*usbd_audio(.bss*)
			*(.usb_seg_sram_seg)
			*(.usbd_*_sram_seg)
			
			*usbd_audio_mono(.text*)
			*usbd_audio_mono(.bss*)
			*usbd_audio_stereo(.text*)
			*usbd_audio_stereo(.bss*)
			
			*(.usbplay_track_notify_sram_seg)
			*(.usbplay_track_write_sram_seg)
			*(.usbrec_store_data_sram_seg)
			*(.usbrec_data_read_sram_seg)
			*(.put_g24_bs_to_wrx_track_sram_seg)
			*(.bt_24g_isr_comm_seg_sram_seg)
			
			*(.usbplay_write_sram_seg)
			*(.usbplay_sram_seg)
			*(.wrec_*_sram_seg)
			*(.wrx_*_sram_seg)
			
			*(.sbt_sram_seg)
			*(.sbt_*_sram_seg)
			*g24_sbt.o(.text*)
			*g24_sbt.o(.bss*)
		
			*xfs_diskio_sram_seg(.text*)
			*xfs_diskio_sram_seg(.bss*)
			
			
			*lc3_enc.o(.text*)
			*lc3_enc.o(.rodata*)
			*lc3_enc.o(.bss*)

			*lc3_mdct.o(.text*)
			*lc3_mdct.o(.rodata*)
			*lc3_mdct.o(.bss*)
			
			*(.lc3plus_*_sram_seg)
			
			/* LC3 common */
			*lc3plus_constants.o(.text*)
			*lc3plus_constants.o(.rodata*)
			*lc3plus_constants.o(.bss*)

			*lc3plus_dct4.o(.text*)
			*lc3plus_dct4.o(.rodata*)
			*lc3plus_dct4.o(.bss*)

			*lc3plus_fft.o(.text*)
			*lc3plus_fft.o(.rodata*)
			*lc3plus_fft.o(.bss*)

			*mdct_shaping.o(.text*)
			*mdct_shaping.o(.rodata*)
			*mdct_shaping.o(.bss*)

			*sns_interpolate_scf.o(.text*)
			*sns_interpolate_scf.o(.rodata*)
			*sns_interpolate_scf.o(.bss*)
			
			/* LC3 encoder */
			*adjust_global_gain.o(.text*)
			*adjust_global_gain.o(.rodata*)
			*adjust_global_gain.o(.bss*)

			*ari_enc.o(.text*)
			*ari_enc.o(.rodata*)
			*ari_enc.o(.bss*)

			*attack_detector.o(.text*)
			*attack_detector.o(.rodata*)
			*attack_detector.o(.bss*)

			*cutoff_bandwidth.o(.text*)
			*cutoff_bandwidth.o(.rodata*)
			*cutoff_bandwidth.o(.bss*)

			*detect_cutoff_warped.o(.text*)
			*detect_cutoff_warped.o(.rodata*)
			*detect_cutoff_warped.o(.bss*)

			*enc_entropy.o(.text*)
			*enc_entropy.o(.rodata*)
			*enc_entropy.o(.bss*)

			*enc_lc3_fl.o(.text*)
			*enc_lc3_fl.o(.rodata*)
			*enc_lc3_fl.o(.bss*)

			*estimate_global_gain.o(.text*)
			*estimate_global_gain.o(.rodata*)
			*estimate_global_gain.o(.bss*)

			*ltpf_coder.o(.text*)
			*ltpf_coder.o(.rodata*)
			*ltpf_coder.o(.bss*)

			*noise_factor.o(.text*)
			*noise_factor.o(.rodata*)
			*noise_factor.o(.bss*)

			*olpa.o(.text*)
			*olpa.o(.rodata*)
			*olpa.o(.bss*)

			*per_band_energy.o(.text*)
			*per_band_energy.o(.rodata*)
			*per_band_energy.o(.bss*)

			*quantize_spec.o(.text*)
			*quantize_spec.o(.rodata*)
			*quantize_spec.o(.bss*)

			*resamp12k8.o(.text*)
			*resamp12k8.o(.rodata*)
			*resamp12k8.o(.bss*)

			*residual_coding.o(.text*)
			*residual_coding.o(.rodata*)
			*residual_coding.o(.bss*)

			*sns_compute_scf.o(.text*)
			*sns_compute_scf.o(.rodata*)
			*sns_compute_scf.o(.bss*)

			*sns_quantize_scf.o(.text*)
			*sns_quantize_scf.o(.rodata*)
			*sns_quantize_scf.o(.bss*)

			*tns_coder.o(.text*)
			*tns_coder.o(.rodata*)
			
			*usbd_audio_app.o(.bss*)
			*usb_hid_app.o(.bss*)
			*usbd_app_common.o(.bss*)
			*usbd_audio.o(.bss*)
			*usbd_*.o(.bss*)
			
			*bt_top.o(.bss*)
			*g24_api.o(.bss*)
			*sbt_isr.o(.bss*)
			
			*bt_audio.o(.bss*)
			*bt_lm.o(.bss*)
			*bt_data_level.o(.bss*)
			
			*mode_g24.o(.bss*)
			*wrec.o(.bss*)
			*bt_afh.o(.bss*)
			
			*bt_middle_level.o(.bss*)
			*bt_irq_handle.o(.bss*)
			*bt_multiplex.o(.bss*)
			*sbt_ld.o(.bss*)
			
			/*libg24.a: hci_uart.o(.bss*)*/
		}
		
		/*蓝牙模式特有的放到这个段*/
		bt_sec
		{
			. = ALIGN(4);
			*(.*_sram_bt_seg)
			*btc_sram_seg.o(.text*)
			*btc_sram_seg.o(.rodata*)
			*btc_sram_seg.o(.bss*)
			
			*bt_host_hal_uart.o(.text*)
			*bt_host_hal_uart.o(.bss*)
			*bt_host_hal_uart.o(.rodata*)
			
			*bt_multilink.o(.text*)
			*bt_multilink.o(.bss*)

			*(.latmaac_sram_seg)
			*(.aacdec_sram_seg)
			*(.aac_dec_asm_sram_seg)
			*(.*kws_sram_seg)
			*opus_encoder.o(.text*)
			*opus_encoder.o(.rodata*)
			*(.*audio_call_cpux_dsp*)
			*(.bth_sram_seg)
			*(.bth_*sram_seg)

			/* bt bss */
			*a2dp_low_latency_asm.o(.bss*)

			*bt_host_a2dp.o(.bss*)
			*a2dp_space_audio.o(.bss*)
			*a2dp_space_audio_plugin.o(.bss*)
			
			*bt_slot_sched.o(.bss*)
			/**sltbt.o(.bss*)*/
			*s_monitor.o(.bss*)
			*bt_api.o(.bss*)
			*ble_find_device_top.o(.bss*)
			*bt_host_api.o(.bss*)
			*bt_host_avdtp.o(.bss*)
			
			*bt_host_Hci.o(.bss*)
			*bt_host_reconnect.o(.bss*)
			*bt_host_rfcomm.o(.bss*)
			*bt_host_sdp.o(.bss*)
			*bt_host_*.o(.bss*)
			
			*bt_seamless_initiator.o(.bss*)
			*bt_seamless_stack(.bss*)
			*bt_host_hs_hf.o(.bss*)
			*bt_seamless_stm.o(.bss*)
			
			*tws_service.o(.bss*)
			
			libnet.a:hci_uart.o(.bss*)
			libnet.a:hci_uart.o(.rodata*)
			libnet.a:ld_init.o(.bss*)
			libnet.a:ld_init*.o(.bss*)
			libnet.a:hci_eif.o(.bss*)
			libnet.a:sltip.o(.bss*)
			libnet.a:tws_profile.o(.bss*)
		}
	}>SRAM
	
    SRAM_LADDR = ((LOADADDR(bt_sec) + SIZEOF(bt_sec) + 3) / 4) * 4;
	
    .sram_code_sec : AT( "SRAM_LADDR" )
    {
        . = ALIGN(4);
		SRAM_ADDR = .;
		
        KEEP(*(.eid_seg))

        *(EXCLUDE_FILE(
            *libnet.a
            *sco_data_io.o
            *bt_audio_service.o
            *a2dp_data_io.o
            *tws_a2dp_ctrl_process.o
            *tws_a2dp_data_process.o
            *tws_common_process.o
            *tws_sco_ctrl_process.o
            *tws_sco_ctrl_process.o
            *tws_sco_data_process.o
            *eq_loader_common.o
            *anc_service.o
            *pms.o
            *audiosrate_conventor.o
            *eq_controller_eq14.o
            *eq_controller_common.o
        ).*_sram_seg)
			
        /* oal */
        *rtos_event.o(.text*)
        *rtos_event.o(.rodata*)
        //*rtos_mem.o(.text*)
        //*rtos_mem.o(.rodata*)
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



        /* 其它 */
        *hal_gpio.o(.rodata.CSWTCH*)
        *hal_gpio_lib.o(.rodata.CSWTCH*)
        *usbd_comp_storage.o(.rodata*)
        /**uart.o(.text*) */
        *uart.o(.rodata*)
        *charge.o(.rodata*)
        *hal_power.o(.rodata*)
        *hal_clk.o(.rodata*)


        /* 音频 */
        /* *audio_source_common.o(.text*) */
        *audio_source_common.o(.rodata*)
        /* *audio_sink_common.o(.text*) */
        *audio_sink_common.o(.rodata*)

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

        //*sltk_math.o(.text*)
        //*sltk_math.o(.rodata*)

        /* ota */
        *(.bt_ota_seg)
        *bt_ota.o(.rodata*)
        *bt_ota_bottom.o(.rodata*)
        *bt_ota_irq.o(.rodata*)
        *bt_ota_middle.o(.rodata*)

        *Sensor_fusion.o(.rodata*)
        *Sensor_fusion.o(.text*)

        *sensor_motion.o(.rodata*)
        *sensor_motion.o(.text*)

        *asinf.o(.rodata*)
        *asinf.o(.text*)

        *atan2f.o(.rodata*)
        *atan2f.o(.text*)

        *atanf.o(.rodata*)
        *atanf.o(.text*)

        *net.o(.text*)
        *net.o(.rodata*)
        *tabels.o(.text*)
        *tabels.o(.rodata*)
        *feature.o(.text*)
        *feature.o(.rodata*)
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


        *(.*_sram_seg)    /* EXCLUDE_FILE的.*_sram_seg段 */
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
