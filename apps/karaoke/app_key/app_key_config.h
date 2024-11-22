#ifndef _APP_KEY_CONFIG_H_
#define _APP_KEY_CONFIG_H_

#include "user_config.h"
#include "key.h"
#include "hal_keyadc.h"
#include "hal_keydig.h"


#if KEY_IR_EN  //ir key config
#if (IRRX_PIN_SEL == 0)
#define IRRX_PIN_CFG    (PIN_B30 | PIN_FUNC11)
#elif (IRRX_PIN_SEL == 1)
#define IRRX_PIN_CFG    (PIN_D12 | PIN_FUNC4)
#elif (IRRX_PIN_SEL == 2)
#define IRRX_PIN_CFG    (PIN_C2 | PIN_FUNC2)
#endif

#define IR_DATA_SHIFT       1

#define IR_KEY_NUM          21

#if IR_DATA_SHIFT
enum ir_ff00_e {
    IR_KEY00 = 0x45,
    IR_KEY01 = 0x46,
    IR_KEY02 = 0x47,

    IR_KEY10 = 0x44,
    IR_KEY11 = 0x40,
    IR_KEY12 = 0x43,

    IR_KEY20 = 0x07,
    IR_KEY21 = 0x15,
    IR_KEY22 = 0x09,

    IR_KEY30 = 0x16,
    IR_KEY31 = 0x19,
    IR_KEY32 = 0x0D,

    IR_KEY40 = 0x0C,
    IR_KEY41 = 0x18,
    IR_KEY42 = 0x5E,

    IR_KEY50 = 0x08,
    IR_KEY51 = 0x1C,
    IR_KEY52 = 0x5A,

    IR_KEY60 = 0x42,
    IR_KEY61 = 0x52,
    IR_KEY62 = 0x4A,
};

#else
enum ir_ff00_e {
    IR_KEY00 = 0xA2,
    IR_KEY01 = 0x62,
    IR_KEY02 = 0xE2,

    IR_KEY10 = 0x22,
    IR_KEY11 = 0x02,
    IR_KEY12 = 0xC2,

    IR_KEY20 = 0xE0,
    IR_KEY21 = 0xA8,
    IR_KEY22 = 0x90,

    IR_KEY30 = 0x68,
    IR_KEY31 = 0x98,
    IR_KEY32 = 0xB0,

    IR_KEY40 = 0x30,
    IR_KEY41 = 0x18,
    IR_KEY42 = 0x7A,

    IR_KEY50 = 0x10,
    IR_KEY51 = 0x38,
    IR_KEY52 = 0x5A,

    IR_KEY60 = 0x42,
    IR_KEY61 = 0x4A,
    IR_KEY62 = 0x52,
};
#endif
#endif

#define KEY_IO_TBLE         8

#if KEY_CODING_EN  //key coding config

#define KEY_CODING_A_PIN_CFG    (PIN_B12 | PIN_FUNC14 | PIN_PULL_UP100K)
#define KEY_CODING_A_PIN_NUM    (PIN_B12)

#define KEY_CODING_B_PIN_CFG    (PIN_B13 | PIN_FUNCINPUT | PIN_PULL_UP100K)
#define KEY_CODING_B_PIN_NUM    (PIN_B13)

#endif



#if KEY_AD_EN// adc key config

#define KADC0_PIN_NUM   (PIN_B30)
#define KADC1_PIN_NUM   (PIN_D0)
#define KADC2_PIN_NUM   (PIN_D3)
#define KADC3_PIN_NUM   (PIN_D4)
#define KADC4_PIN_NUM   (PIN_D5)
#define KADC5_PIN_NUM   (PIN_D6)
#define KADC6_PIN_NUM   (PIN_D9)
#define KADC7_PIN_NUM   (PIN_B3)
#define KADC8_PIN_NUM   (PIN_B5)
#define KADC9_PIN_NUM   (PIN_B6)

 enum kadc_ch_e {
     KADC_CH0 = 0,
     KADC_CH1,
     KADC_CH2,
     KADC_CH3,
     KADC_CH4,
     KADC_CH5
 };


#if !KADC0_IRQ && SLEEP_EN
#define KADC0_FUNC      PIN_FUNCEINT
#else
#define KADC0_FUNC      PIN_FUNCDISABLE
#endif

#if !KADC1_IRQ && SLEEP_EN
#define KADC1_FUNC      PIN_FUNCEINT
#else
#define KADC1_FUNC      PIN_FUNCDISABLE
#endif

#if !KADC2_IRQ && SLEEP_EN
#define KADC2_FUNC      PIN_FUNCEINT
#else
#define KADC2_FUNC      PIN_FUNCDISABLE
#endif

#if !KADC3_IRQ && SLEEP_EN
#define KADC3_FUNC      PIN_FUNCEINT
#else
#define KADC3_FUNC      PIN_FUNCDISABLE
#endif

#if !KADC4_IRQ && SLEEP_EN
#define KADC4_FUNC      PIN_FUNCEINT
#else
#define KADC4_FUNC      PIN_FUNCDISABLE
#endif

#if !KADC5_IRQ && SLEEP_EN
#define KADC5_FUNC      PIN_FUNCEINT
#else
#define KADC5_FUNC      PIN_FUNCDISABLE
#endif

#if !KADC6_IRQ && SLEEP_EN
#define KADC6_FUNC      PIN_FUNCEINT
#else
#define KADC6_FUNC      PIN_FUNCDISABLE
#endif

#if !KADC7_IRQ && SLEEP_EN
#define KADC7_FUNC      PIN_FUNCEINT
#else
#define KADC7_FUNC      PIN_FUNCDISABLE
#endif

#if !KADC8_IRQ && SLEEP_EN
#define KADC8_FUNC      PIN_FUNCEINT
#else
#define KADC8_FUNC      PIN_FUNCDISABLE
#endif

#if !KADC9_IRQ && SLEEP_EN
#define KADC9_FUNC      PIN_FUNCEINT
#else
#define KADC9_FUNC      PIN_FUNCDISABLE
#endif


#define KADC_IRQ_EN     ((KADC0_EN && KADC0_IRQ) || \
     (KADC1_EN && KADC1_IRQ) || \
     (KADC2_EN && KADC2_IRQ) || \
     (KADC3_EN && KADC3_IRQ) || \
     (KADC4_EN && KADC4_IRQ) || \
     (KADC5_EN && KADC5_IRQ) || \
     (KADC6_EN && KADC6_IRQ) || \
     (KADC7_EN && KADC7_IRQ) || \
     (KADC8_EN && KADC8_IRQ) || \
     (KADC9_EN && KADC9_IRQ))

#define KADC_SCAN_EN    ((KADC0_EN && !KADC0_IRQ) || \
    (KADC1_EN && !KADC1_IRQ) || \
    (KADC2_EN && !KADC2_IRQ) || \
    (KADC3_EN && !KADC3_IRQ) || \
    (KADC4_EN && !KADC4_IRQ) || \
    (KADC5_EN && !KADC5_IRQ) || \
    (KADC6_EN && !KADC6_IRQ) || \
    (KADC7_EN && !KADC7_IRQ) || \
    (KADC8_EN && !KADC8_IRQ) || \
    (KADC9_EN && !KADC9_IRQ))

#define KADC_KNOB_EN    (KADC0_KNOB || KADC1_KNOB || KADC2_KNOB || \
     KADC3_KNOB || KADC4_KNOB || KADC5_KNOB || KADC6_KNOB || KADC7_KNOB || KADC8_KNOB || KADC9_KNOB)


#define KNOB0_THRESHOLD     0xF00
#define KNOB1_THRESHOLD     0xF00
#define KNOB2_THRESHOLD     0xF00
#define KNOB3_THRESHOLD     0xF00
#define KNOB4_THRESHOLD     0xF00
#define KNOB5_THRESHOLD     0xF00
#define KNOB6_THRESHOLD     0xF00
#define KNOB7_THRESHOLD     0xF00
#define KNOB8_THRESHOLD     0xF00
#define KNOB9_THRESHOLD     0xF00

#define KEY_EVENT_TAB_MAX    16

#if MYME_M4_BOARD
#undef KEY_EVENT_TAB_MAX
#define KEY_EVENT_TAB_MAX    4
#endif

#endif

#if KEY_DIG_EN
#define KEY_DIG_TBLE         3


#if (KDIG0_SEL == 0)
#define KDIG0_PIN_CFG    (PIN_C2 | PIN_FUNC7)
#elif (KDIG0_SEL == 1)
#define KDIG0_PIN_CFG    (PIN_D0 | PIN_FUNC9)
#elif (KDIG0_SEL == 2)
#define KDIG0_PIN_CFG    (PIN_D12 | PIN_FUNC5 | PIN_PULL_UP100K)
#endif

#if (KDIG1_SEL == 0)
#define KDIG1_PIN_CFG    (PIN_B3 | PIN_FUNC3)
#elif (KDIG1_SEL == 1)
#define KDIG1_PIN_CFG    (PIN_B30 | PIN_FUNC2)
#elif (KDIG1_SEL == 2)
#define KDIG1_PIN_CFG    (PIN_C2 | PIN_FUNC6)
#elif (KDIG1_SEL == 3)
#define KDIG1_PIN_CFG    (PIN_D10 | PIN_FUNC4 | PIN_PULL_UP100K)
#endif

#if (KDIG2_SEL == 0)
#define KDIG2_PIN_CFG    (PIN_B4 | PIN_FUNC3)
#elif (KDIG2_SEL == 1)
#define KDIG2_PIN_CFG    (PIN_B5 | PIN_FUNC8)
#elif (KDIG2_SEL == 2)
#define KDIG2_PIN_CFG    (PIN_C2 | PIN_FUNC5)
#elif (KDIG2_SEL == 3)
#define KDIG2_PIN_CFG    (PIN_D8 | PIN_FUNC4 | PIN_PULL_UP100K)
#endif

#endif

#endif
