#ifndef _SHIT_H_
#define _SHIT_H_

#include "sys_types.h"

#ifdef __cplusplus
 extern "C" {
#endif



//USB_OTG_Core
typedef struct
{
    volatile uint32_t GOTGCTL;                  /*!< USB_OTG Control and Status Register          000h */
    volatile uint32_t GOTGINT;                  /*!< USB_OTG Interrupt Register                   004h */
    volatile uint32_t GAHBCFG;                  /*!< Core AHB Configuration Register              008h */
    volatile uint32_t GUSBCFG;                  /*!< Core USB Configuration Register              00Ch */
    volatile uint32_t GRSTCTL;                  /*!< Core Reset Register                          010h */
    volatile uint32_t GINTSTS;                  /*!< Core Interrupt Register                      014h */
    volatile uint32_t GINTMSK;                  /*!< Core Interrupt Mask Register                 018h */
    volatile uint32_t GRXSTSR;                  /*!< Receive Sts Q Read Register                  01Ch */
    volatile uint32_t GRXSTSP;                  /*!< Receive Sts Q Read & POP Register            020h */
    volatile uint32_t GRXFSIZ;                  /*!< Receive FIFO Size Register                   024h */
    volatile uint32_t DIEPTXF0_HNPTXFSIZ;       /*!< EP0 / Non Periodic Tx FIFO Size Register     028h */
    volatile uint32_t HNPTXSTS;                 /*!< Non Periodic Tx FIFO/Queue Sts reg           02Ch */
    uint32_t Reserved30[2];                     /*!< Reserved                                     030h */
    volatile uint32_t GCCFG;                    /*!< General Purpose IO Register                  038h */
    volatile uint32_t CID;                      /*!< User ID Register                             03Ch */
    uint32_t  Reserved5[3];                     /*!< Reserved                                040h-048h */
    volatile uint32_t GHWCFG3;                  /*!< User HW config3                              04Ch */
    uint32_t  Reserved6;                        /*!< Reserved                                     050h */
    volatile uint32_t GLPMCFG;                  /*!< LPM Register                                 054h */
    uint32_t  Reserved;                         /*!< Reserved                                     058h */
    volatile uint32_t GDFIFOCFG;                /*!< DFIFO Software Config Register               05Ch */
    uint32_t  Reserved43[40];                   /*!< Reserved                                058h-0FFh */
    volatile uint32_t HPTXFSIZ;                 /*!< Host Periodic Tx FIFO Size Reg               100h */
    volatile uint32_t DIEPTXF[0x0F];            /*!< dev Periodic Transmit FIFO */
} USB_OTG_GlobalTypeDef;



#ifdef __cplusplus
  }
#endif

#endif
