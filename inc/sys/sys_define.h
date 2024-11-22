#ifndef _SYS_DEFINE_H_
#define _SYS_DEFINE_H_

#ifdef __cplusplus
 extern "C" {
#endif


/***************************************************************************
 *               type define
 ***************************************************************************/
#define SYS_TYPE_CHIP                       0x00010000
#define SYS_TYPE_CHIP_VER                   0x00020000
#define SYS_TYPE_BOARD                      0x00030000
#define SYS_TYPE_PLATFORM                   0x00040000

#define AUDIO_TYPE_DEVICE                   0x00001000

/***************************************************************************
 *               system define
 ***************************************************************************/

/* chip */
#define CHIP_SL6800                         (SYS_TYPE_CHIP + 0x0001)
#define CHIP_SL6801                         (SYS_TYPE_CHIP + 0x0002)

/* chip version */
#define CHIP_VER_A                          (SYS_TYPE_CHIP_VER + 0x0001)
#define CHIP_VER_B                          (SYS_TYPE_CHIP_VER + 0x0002)

/* board */
#define BOARD_SL6800_FPGA                   (SYS_TYPE_BOARD + 0x0011)   //用于SL6800 FPGA开发
#define BOARD_SL6800                        (SYS_TYPE_BOARD + 0x0012)   //用于SL6800
#define BOARD_SL6800_KARAOKE                (SYS_TYPE_BOARD + 0x0013)   //用于SL6800带karaoke项目
#define BOARD_SL6800_EARPHONE               (SYS_TYPE_BOARD + 0x0014)   //用于SL6800耳机项目
#define BOARD_SL6800_KARAOKE_TONE_SIZE_250K (SYS_TYPE_BOARD + 0x0015)   //用于SL6800带karaoke项目，flash整合了250K给TONE使用
#define BOARD_SL6800_TWS                    (SYS_TYPE_BOARD + 0x0015)   //用于SL6800 TWS项目


#define BOARD_SL6801_FPGA                   (SYS_TYPE_BOARD + 0x1000)
#define BOARD_SL6801_EVB                    (SYS_TYPE_BOARD + 0x1001)

/* platform */
#define PLATFORM_CHIP                       (SYS_TYPE_PLATFORM + 0x0001)
#define PLATFORM_FPGA                       (SYS_TYPE_PLATFORM + 0x0002)


/*
 * -chip-    -package-   -core-
 * WS100 - QFN48_6X6 - LDO
 * WS101 - QFN32_4X4 - LDO
 *
 * CA100 - QFN48_5X5 - LDO
 * CA101 - QFN32_4X4 - LDO
 * CA102 - QFN48_6X6 - LDO
 *
 * HS100 - QFN32_4X4 - DCDC
 */

#ifdef __cplusplus
}
#endif

#endif
