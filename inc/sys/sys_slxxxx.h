#ifndef _SLxxxx_H_
#define _SLxxxx_H_

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/**
  * @brief Configuration of the RISC32 Processor and Core Peripherals
  */
#define __RISC32_REV              0x0001U  /*!< Core revision r0p1                            */
#define __MPU_PRESENT             1U       /*!< SL28XX provides an MPU                     */
#define __INTC_PRIO_BITS          4U       /*!< SL28XX uses 4 Bits for the Priority Levels */
#define __Vendor_SysTickConfig    0U       /*!< Set to 1 if different SysTick Config is used  */
#ifndef __FPU_PRESENT
#define __FPU_PRESENT             1U       /*!< FPU present                                   */
#endif /* __FPU_PRESENT */

/**
 * @brief slxxxx Interrupt Number Definition, according to the selected device
 *        in @ref Library_configuration_section
 */
typedef enum {
/******  Cortex-M4 Processor Exceptions Numbers ****************************************************************/
  NonMaskableInt_IRQn           = -14,      /*!< 2 Non Maskable Interrupt                                           */
  MemoryManagement_IRQn         = -12,      /*!< 4 Cortex-M4 Memory Management Interrupt                   */
  BusFault_IRQn                 = -11,      /*!< 5 Cortex-M4 Bus Fault Interrupt                                    */
  UsageFault_IRQn               = -10,      /*!< 6 Cortex-M4 Usage Fault Interrupt                                 */
  SVCall_IRQn                   = -5,       /*!< 11 Cortex-M4 SV Call Interrupt                                     */
  DebugMonitor_IRQn             = -4,       /*!< 12 Cortex-M4 Debug Monitor Interrupt                           */
  PendSV_IRQn                   = -2,       /*!< 14 Cortex-M4 Pend SV Interrupt                                    */
  SysTick_IRQn                  = -1,       /*!< 15 Cortex-M4 System Tick Interrupt                              */
/******  SL68XX specific Interrupt Numbers **********************************************************************/
  IRQn_WDG                      = 0,        /*!< Window WatchDog Interrupt                                          */
  IRQn_GPIOA                    = 1,        /*!< GPIO A global Interrupt                                    */
  IRQn_GPIOB                    = 2,        /*!< GPIO B global Interrupt                                    */
  IRQn_GPIOC                    = 3,        /*!< GPIO C global Interrupt                                        */
  IRQn_GPIOD                    = 4,        /*!< GPIO D global Interrupt                                                */
  IRQn_GPIOE                    = 5,        /*!< GPIO E global Interrupt                                                */
  IRQn_GPIOF                    = 6,        /*!< GPIO F global Interrupt                                                */
  IRQn_GPIOG                    = 7,        /*!< GPIO G global Interrupt                                                */
  IRQn_TIMER0                   = 8,        /*!< TIMER0 global Interrupt                                                */
  IRQn_TIMER1                   = 9,        /*!< TIMER1 global  Interrupt                                               */
  IRQn_TIMER2                   = 10,        /*!< TIMER1 global  Interrupt                                               */
  IRQn_TIMER3                   = 11,        /*!< TIMER1 global  Interrupt                                               */
  IRQn_TIMER4                   = 12,        /*!< TIMER1 global  Interrupt                                               */
  IRQn_TIMER5                   = 13,        /*!< TIMER0 global Interrupt                                                */
  IRQn_TIMER6                   = 14,        /*!< TIMER1 global  Interrupt                                               */
  IRQn_TIMER7                   = 15,        /*!< TIMER1 global  Interrupt                                               */
  IRQn_UART0                    = 16,        /*!< UART0 global  Interrupt                                                */
  IRQn_UART1                    = 17,        /*!< UART1 global  Interrupt                                                */
  IRQn_UART2                    = 18,        /*!< UART0 global  Interrupt                                                */
  IRQn_UART3                    = 19,        /*!< UART1 global  Interrupt                                                */
  IRQn_KADC                     = 20,        /*!< KADC global Interrupt                              */
  IRQn_SPI                      = 21,        /*!< SPI global  Interrupt                                                  */
  IRQn_TWI0                     = 22,       /*!< TWI1 global  Interrupt                                             */
  IRQn_TWI1                     = 23,       /*!< TWI1 global  Interrupt                                             */
  IRQn_STWI                     = 24,       /*!< TWI1 global  Interrupt                                             */
  IRQn_TWI2                     = 25,       /*!< TWI1 global  Interrupt                                             */
  IRQn_BT                       = 26,       /*!< BT Controller global  Interrupt                                    */
  IRQn_BLE                      = 27,       /*!< BLE global  Interrupt                                              */
  IRQn_MDM                      = 28,       /*!< BTSS MDM                                                          */
  IRQn_LE52                     = 29,       /*!< BTSS MDM                                                          */
  IRQn_AUSS                     = 30,       /*!< AUSS                                                           */
  IRQn_VAD                      = 31,       /*!< VAD                                                           */
  IRQn_SBT                      = 32,       /*!< RESERVED                                                           */
  IRQn_RES2                     = 33,       /*!< RESERVED                                                           */
  IRQn_I2S                      = 34,       /*!< I2S global  Interrupt                                                  */
  IRQn_SPDIF                    = 35,       /*!< I2S global  Interrupt                                                  */
  IRQn_ASRC                     = 36,       /*!< ASRC                                       */
  IRQn_HCEC                     = 37,       /*!< ASRC                                       */
  IRQn_DRC                      = 38,       /*!< ASRC                                       */
  IRQn_ANC                      = 39,       /*!< ASRC                                       */
  IRQn_EQ                       = 40,       /*!< ASRC                                       */
  IRQn_USB                      = 41,       /*!< USB global  Interrupt                                                  */
  IRQn_USB_IDMA                 = 42,       /*!< USB Internal DMA                                                 */
  IRQn_USB_DET                  = 43,       /*!< USB Internal DMA                                                 */
  IRQn_SDMMC                    = 44,       /*!< SDMMC global  Interrupt                                            */
  IRQn_RES3                     = 45,       /*!< RESERVED                                                           */
  IRQn_RES4                     = 46,       /*!< RESERVED                                                           */
  RTC_TIMER                     = 47,       /*!< RTC_TIMER global  Interrupt                                            */
  RTC_PRCM                      = 48,       /*!< RTC_PRCM global  Interrupt                                           */
  IRQn_TRNG                     = 49,       /*!< TRNG global  Interrupt                                     */
  IRQn_NORF                     = 50,       /*!< NORF global  Interrupt                                     */
  IRQn_SIC                      = 51,       /*!< NORF global  Interrupt                                     */
  IRQn_R_TWI                    = 52,       /*!< R_TWI global  Interrupt                                                  */
  IRQn_EID                      = 53,       /*!< EID global  Interrupt                                                  */
  IRQn_MSGBOXC0                 = 54,       /*!< EID global  Interrupt                                                  */
  IRQn_MSGBOXC1                 = 55,       /*!< EID global  Interrupt                                                  */
  IRQn_BROM_WR                  = 56,       /*!< RESERVED                                                              */
  IRQn_RES5                     = 57,       /*!< RESERVED                                                           */
  IRQn_RES6                     = 58,       /*!< RESERVED                                                           */
  IRQn_RES7                     = 59,       /*!< RESERVED                                                           */
  IRQn_DMA0                     = 60,       /*!< RESERVED                                                       */
  IRQn_DMA1                     = 61,       /*!< RESERVED                                                       */
  IRQn_DMA2                     = 62,       /*!< RESERVED                                                           */
  IRQn_DMA3                     = 63,       /*!< RESERVED                                                           */
  IRQn_DMA4                     = 64,       /*!< RESERVED                                                       */
  IRQn_DMA5                     = 65,       /*!< RESERVED                                                       */
  IRQn_DMA6                     = 66,       /*!< RESERVED                                                           */
  IRQn_DMA7                     = 67,       /*!< RESERVED                                                           */
  IRQn_TKC                      = 68,       /*!< TKC  global  Interrupt                                                      */
  IRQn_TKC_SLD                  = 69,       /*!< TKC_SLD                                           */
  IRQn_DKC                      = 70,       /*!< DKC                                                           */
  IRQn_IR                       = 71,       /*!< IR                                                */
  IRQn_LEDC                     = 72,       /*!< LEDC                                                                       */
  IRQn_PWM                      = 73,       /*!< PWM global  Interrupt                                                  */
} IRQn_Type;



typedef enum
{
  RESET = 0U,
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum
{
  DISABLE = 0U,
  ENABLE = !DISABLE
} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef enum
{
  ERROR = 0U,
  SUCCESS = !ERROR
} ErrorStatus;

/**
  * @}
  */


/** @addtogroup Exported_macro
  * @{
  */
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))

#define GET_U32(p)			(*(uint32_t *)(p))
#define GET_U16(p)			(*(uint16_t *)(p))
#define GET_U8(p)			(*(uint8_t *)(p))

#define readb(reg)              (*(volatile uint8_t  *)(reg))
#define readw(reg)              (*(volatile uint16_t *)(reg))
#define readl(reg)              (*(volatile uint32_t *)(reg))

#define writeb(val, reg)        (*(volatile uint8_t *)(reg) = (val))
#define writew(val, reg)        (*(volatile uint16_t *)(reg) = (val))
#define writel(val, reg)        (*(volatile uint32_t *)(reg) = (val))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SLxxxx_H */

