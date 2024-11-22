#define LOG_LEV 4
#include "sys_inc.h"
#include "hal.h"
#include "karaoke_framework.h"
#include "app_key_config.h"
#include "sensor_force.h"
#include "key_ir.h"
#include "lcd_seg.h"
#include "soundcard.h"

key_scan_t key;

#if KEY_IR_EN
kir_t kir;
#endif

#if KADC3_EN
    OAL_THREAD key_audio_task_handler;
    OAL_TIMER key_audio_auto;
#endif

#if KEY_AD_EN

uint32_t kadc_val_get[KADC_CH_MAX];

/*------------------------------------------------------------------------------
*函数功能: ADC按键配置表
*参数1：使能AD通道
*参数2：中断脚配置
*参数3：AD通道配置
*-------------------------------------------------------------------------------*/

const uint32_t kadc_init_config[KADC_CH_MAX][3] = {
    //en
    //irq sel
    //threshold(0x000 ~ 0xFFF), t_cnt(0x0 ~ 0xF), t_dbc(0x0 ~ 0xF), mode

    //adc0:
    {KADC0_EN,
#if KADC0_IRQ
    KADC_IRQ_DATA | KADC_IRQ_DOWN | KADC_IRQ_UP,
    (0xF00 << KADC_THRESHOLD_BIT) | (0x03 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_CONTINUE,
    },
#else
    0x0,
    (0xFFF << KADC_THRESHOLD_BIT) | (0x0 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_NORMAL,},
#endif

    //adc1:
    {KADC1_EN,
#if KADC1_IRQ
    KADC_IRQ_DATA | KADC_IRQ_DOWN | KADC_IRQ_UP,
    (0xF00 << KADC_THRESHOLD_BIT) | (0x03 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_CONTINUE,},
#else
    0x0,
    (0x7FF << KADC_THRESHOLD_BIT) | (0x0 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_NORMAL,},
#endif

    //adc2:
    {KADC2_EN,
#if KADC2_IRQ
    KADC_IRQ_DATA | KADC_IRQ_DOWN | KADC_IRQ_UP,
    (0xF00 << KADC_THRESHOLD_BIT) | (0x03 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_CONTINUE,},
#else
    0x0,
    (0xFFF << KADC_THRESHOLD_BIT) | (0x0 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_NORMAL,},
#endif

    //adc3:
    {KADC3_EN,
#if KADC3_IRQ
    KADC_IRQ_DATA | KADC_IRQ_DOWN | KADC_IRQ_UP,
    (0xF00 << KADC_THRESHOLD_BIT) | (0x03 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_CONTINUE,},
#else
    0x0,
    (0xFFF << KADC_THRESHOLD_BIT) | (0x0 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_NORMAL,},
#endif

    //adc4:
    {KADC4_EN,
#if KADC4_IRQ
    KADC_IRQ_DATA | KADC_IRQ_DOWN | KADC_IRQ_UP,
    (0xF00 << KADC_THRESHOLD_BIT) | (0x03 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_CONTINUE,},
#else
    0x0,
    (0xFFF << KADC_THRESHOLD_BIT) | (0x0 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_NORMAL,},
#endif

    //adc5:
    {KADC5_EN,
#if KADC5_IRQ
    KADC_IRQ_DATA | KADC_IRQ_DOWN | KADC_IRQ_UP,
    (0xF00 << KADC_THRESHOLD_BIT) | (0x03 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_CONTINUE,},
#else
    0x0,
    (0xFFF << KADC_THRESHOLD_BIT) | (0x0 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_NORMAL,},
#endif

    //adc6:
    {KADC6_EN,
#if KADC6_IRQ
    KADC_IRQ_DATA | KADC_IRQ_DOWN | KADC_IRQ_UP,
    (0xF00 << KADC_THRESHOLD_BIT) | (0x03 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_CONTINUE,},
#else
    0x0,
    (0xFFF << KADC_THRESHOLD_BIT) | (0x0 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_NORMAL,},
#endif

    //adc7:
    {KADC7_EN,
#if KADC7_IRQ
    KADC_IRQ_DATA | KADC_IRQ_DOWN | KADC_IRQ_UP,
    (0xF00 << KADC_THRESHOLD_BIT) | (0x03 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_CONTINUE,},
#else
    0x0,
    (0xFFF << KADC_THRESHOLD_BIT) | (0x0 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_NORMAL,},
#endif

    //adc8:
    {KADC8_EN,
#if KADC8_IRQ
    KADC_IRQ_DATA | KADC_IRQ_DOWN | KADC_IRQ_UP,
    (0xF00 << KADC_THRESHOLD_BIT) | (0x03 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_CONTINUE,},
#else
    0x0,
    (0xFFF << KADC_THRESHOLD_BIT) | (0x0 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_NORMAL,},
#endif

    //adc9:
    {KADC9_EN,
#if KADC9_IRQ
    KADC_IRQ_DATA | KADC_IRQ_DOWN | KADC_IRQ_UP,
    (0xF00 << KADC_THRESHOLD_BIT) | (0x03 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_CONTINUE,},
#else
    0x0,
    (0xFFF << KADC_THRESHOLD_BIT) | (0x0 << KADC_TCNT_BIT) | (0x3 << KADC_TDBC_BIT) | KADC_MODE_NORMAL,},
#endif
};


/*------------------------------------------------------------------------------
*函数功能: ADC旋钮配置表
*参数1：使能AD通道
*参数2：AD中断工作方式使能
*参数3：AD通道旋钮使能
*-------------------------------------------------------------------------------*/

kadc_ctl_t kadc_irq_knob_config[KADC_CH_MAX] = {
    {KADC0_EN, KADC0_IRQ, KADC0_KNOB},
    {KADC1_EN, KADC1_IRQ, KADC1_KNOB},
    {KADC2_EN, KADC2_IRQ, KADC2_KNOB},
    {KADC3_EN, KADC3_IRQ, KADC3_KNOB},
    {KADC4_EN, KADC4_IRQ, KADC4_KNOB},
    {KADC5_EN, KADC5_IRQ, KADC5_KNOB},
    {KADC6_EN, KADC6_IRQ, KADC6_KNOB},
    {KADC7_EN, KADC7_IRQ, KADC7_KNOB},
    {KADC8_EN, KADC8_IRQ, KADC8_KNOB},
    {KADC9_EN, KADC9_IRQ, KADC9_KNOB},
};

uint32_t app_knob_key_threshod[KADC_CH_MAX] = {
    KNOB0_THRESHOLD,
    KNOB1_THRESHOLD,
    KNOB2_THRESHOLD,
    KNOB3_THRESHOLD,
    KNOB4_THRESHOLD,
    KNOB5_THRESHOLD,
    KNOB6_THRESHOLD,
    KNOB7_THRESHOLD,
    KNOB8_THRESHOLD,
    KNOB9_THRESHOLD,
};

/*------------------------------------------------------------------------------
*函数功能: ADC按键事件配置表
*参数1：AD值
*参数2：事件
*-------------------------------------------------------------------------------*/
#if KADC0_EN
/*key adc0 键值表*/
kadc_event_tab key_event_table_00[KEY_EVENT_TAB_MAX] = {
#if (BASE_BOARD_CONFIG == 1)
    {0x30, KEY_MODE},
    {0x100, KEY_PLAY},
#else
    {0x50, KEY_PREV},//sw3
    {0x150, KEY_PLAY},//sw4
#endif
    {0x250, KEY_NEXT}, //sw5
    {0x390, KEY_SW},//sw6
    {0x490, KEY_M},//sw7
    {0x590, KEY_MODE},//sw8
    {0x690, KEY_NONE},//sw9
    {0x790, KEY_NONE},//sw10
    {0x890, KEY_NONE}, //sw11
    {0x990, KEY_NONE},//sw12
    {0xa50, KEY_NONE},//sw13
    {0xb90, KEY_NONE},//sw14
    {0xc90, KEY_NONE},//sw15
    {0xd00, KEY_NONE},//sw16
    {0xe00, KEY_NONE},//sw17
    {0xf00, KEY_NONE},//sw18
};
#endif

#if KADC1_EN
/*key adc1 键值表*/
kadc_event_tab key_event_table_01[KEY_EVENT_TAB_MAX] = {
    {0x300, KEY_PREV_VOLDOWN},	//sw3 
    {0x600, KEY_NEXT_VOLUP},	//sw4 
    {0x900, KEY_NEXT},	//sw5 
    {0xC00, KEY_PREV},  //sw6 
};
#endif

#if KADC2_EN
/*key adc2 键值表*/
kadc_event_tab key_event_table_02[KEY_EVENT_TAB_MAX] = {
#if (BASE_BOARD_CONFIG == 1)
    {0x30, KEY_MODE},
    {0x100, KEY_PLAY},
#else
    {0x50, KEY_PREV},//sw3
    {0x150, KEY_PLAY},//sw4
#endif
    {0x250, KEY_NEXT}, //sw5
    {0x390, KEY_SW},//sw6
    {0x490, KEY_M},//sw7
    {0x590, KEY_MODE},//sw8
    {0x690, KEY_NONE},//sw9
    {0x790, KEY_NONE},//sw10
    {0x890, KEY_NONE}, //sw11
    {0x990, KEY_NONE},//sw12
    {0xa50, KEY_NONE},//sw13
    {0xb90, KEY_NONE},//sw14
    {0xc90, KEY_NONE},//sw15
    {0xd00, KEY_NONE},//sw16
    {0xe00, KEY_NONE},//sw17
    {0xf00, KEY_NONE},//sw18
};
#endif

#if KADC3_EN
/*key adc3 键值表*/
kadc_event_tab key_event_table_03[KEY_EVENT_TAB_MAX] = {
    {0x199, KEY_VOL0}, //0
    {0x332, KEY_VOL1},//1
    {0x4CC, KEY_VOL2},//2
    {0x666, KEY_VOL3},//3
    {0x7E0, KEY_VOL4},//4
    {0x97A, KEY_VOL5},//5
    {0xA92, KEY_VOL6},//6
    {0xC0C, KEY_VOL7},//7
    {0xD86, KEY_VOL8},//8
    {0xFFF, KEY_VOL9},//9
};
#endif

#if KADC4_EN
/*key adc4 键值表*/
kadc_event_tab key_event_table_04[KEY_EVENT_TAB_MAX] = {
#if (BASE_BOARD_CONFIG == 1)
    {0x30, KEY_MODE},
    {0x100, KEY_PLAY},
#else
    {0x50, KEY_PREV},//sw3
    {0x150, KEY_PLAY},//sw4
#endif
    {0x250, KEY_NEXT}, //sw5
    {0x390, KEY_SW},//sw6
    {0x490, KEY_M},//sw7
    {0x590, KEY_MODE},//sw8
    {0x690, KEY_NONE},//sw9
    {0x790, KEY_NONE},//sw10
    {0x890, KEY_NONE}, //sw11
    {0x990, KEY_NONE},//sw12
    {0xa50, KEY_NONE},//sw13
    {0xb90, KEY_NONE},//sw14
    {0xc90, KEY_NONE},//sw15
    {0xd00, KEY_NONE},//sw16
    {0xe00, KEY_NONE},//sw17
    {0xf00, KEY_NONE},//sw18
};
#endif

#if KADC5_EN
/*key adc5 键值表*/
kadc_event_tab key_event_table_05[KEY_EVENT_TAB_MAX] = {
#if (BASE_BOARD_CONFIG == 1)
    {0x30, KEY_MODE},
    {0x100, KEY_PLAY},
#else
    {0x50, KEY_PREV},//sw3
    {0x150, KEY_PLAY},//sw4
#endif
    {0x250, KEY_NEXT}, //sw5
    {0x390, KEY_SW},//sw6
    {0x490, KEY_M},//sw7
    {0x590, KEY_MODE},//sw8
    {0x690, KEY_NONE},//sw9
    {0x790, KEY_NONE},//sw10
    {0x890, KEY_NONE}, //sw11
    {0x990, KEY_NONE},//sw12
    {0xa50, KEY_NONE},//sw13
    {0xb90, KEY_NONE},//sw14
    {0xc90, KEY_NONE},//sw15
    {0xd00, KEY_NONE},//sw16
    {0xe00, KEY_NONE},//sw17
    {0xf00, KEY_NONE},//sw18
};
#endif

#if KADC6_EN
/*key adc6 键值表*/
kadc_event_tab key_event_table_06[KEY_EVENT_TAB_MAX] = {
#if (BASE_BOARD_CONFIG == 1)
    {0x30, KEY_MODE},
    {0x100, KEY_PLAY},
#else
    {0x50, KEY_PREV},//sw3
    {0x150, KEY_PLAY},//sw4
#endif
    {0x250, KEY_NEXT}, //sw5
    {0x390, KEY_SW},//sw6
    {0x490, KEY_M},//sw7
    {0x590, KEY_MODE},//sw8
    {0x690, KEY_NONE},//sw9
    {0x790, KEY_NONE},//sw10
    {0x890, KEY_NONE}, //sw11
    {0x990, KEY_NONE},//sw12
    {0xa50, KEY_NONE},//sw13
    {0xb90, KEY_NONE},//sw14
    {0xc90, KEY_NONE},//sw15
    {0xd00, KEY_NONE},//sw16
    {0xe00, KEY_NONE},//sw17
    {0xf00, KEY_NONE},//sw18
};
#endif

#if KADC7_EN
/*key adc7 键值表*/
kadc_event_tab key_event_table_07[KEY_EVENT_TAB_MAX] = {
#if (BASE_BOARD_CONFIG == 1)
    {0x30, KEY_MODE},
    {0x100, KEY_PLAY},
#else
    {0x50, KEY_PREV},//sw3
    {0x150, KEY_PLAY},//sw4
#endif
    {0x250, KEY_NEXT}, //sw5
    {0x390, KEY_SW},//sw6
    {0x490, KEY_M},//sw7
    {0x590, KEY_MODE},//sw8
    {0x690, KEY_NONE},//sw9
    {0x790, KEY_NONE},//sw10
    {0x890, KEY_NONE}, //sw11
    {0x990, KEY_NONE},//sw12
    {0xa50, KEY_NONE},//sw13
    {0xb90, KEY_NONE},//sw14
    {0xc90, KEY_NONE},//sw15
    {0xd00, KEY_NONE},//sw16
    {0xe00, KEY_NONE},//sw17
    {0xf00, KEY_NONE},//sw18
};
#endif

#if KADC8_EN
/*key adc8 键值表*/
kadc_event_tab key_event_table_08[KEY_EVENT_TAB_MAX] = {
#if (BASE_BOARD_CONFIG == 1)
    {0x30, KEY_MODE},
    {0x100, KEY_PLAY},
#else
    {0x50, KEY_PREV},//sw3
    {0x150, KEY_PLAY},//sw4
#endif
    {0x250, KEY_NEXT}, //sw5
    {0x390, KEY_SW},//sw6
    {0x490, KEY_M},//sw7
    {0x590, KEY_MODE},//sw8
    {0x690, KEY_NONE},//sw9
    {0x790, KEY_NONE},//sw10
    {0x890, KEY_NONE}, //sw11
    {0x990, KEY_NONE},//sw12
    {0xa50, KEY_NONE},//sw13
    {0xb90, KEY_NONE},//sw14
    {0xc90, KEY_NONE},//sw15
    {0xd00, KEY_NONE},//sw16
    {0xe00, KEY_NONE},//sw17
    {0xf00, KEY_NONE},//sw18
};
#endif

#if KADC9_EN
/*key adc9 键值表*/
kadc_event_tab key_event_table_09[KEY_EVENT_TAB_MAX] = {
#if (BASE_BOARD_CONFIG == 1)
    {0x30, KEY_MODE},
    {0x100, KEY_PLAY},
#else
    {0x50, KEY_PREV},//sw3
    {0x150, KEY_PLAY},//sw4
#endif
    {0x250, KEY_NEXT}, //sw5
    {0x390, KEY_SW},//sw6
    {0x490, KEY_M},//sw7
    {0x590, KEY_MODE},//sw8
    {0x690, KEY_NONE},//sw9
    {0x790, KEY_NONE},//sw10
    {0x890, KEY_NONE}, //sw11
    {0x990, KEY_NONE},//sw12
    {0xa50, KEY_NONE},//sw13
    {0xb90, KEY_NONE},//sw14
    {0xc90, KEY_NONE},//sw15
    {0xd00, KEY_NONE},//sw16
    {0xe00, KEY_NONE},//sw17
    {0xf00, KEY_NONE},//sw18
};
#endif

#if KADC_KNOB_EN

#define KNOB0_DIFF          0x80
#define KNOB1_DIFF          0x80
#define KNOB2_DIFF          0x80
#define KNOB3_DIFF          0x80
#define KNOB4_DIFF          0x40
#define KNOB5_DIFF          0x80
#define KNOB6_DIFF          0x80
#define KNOB7_DIFF          0x80
#define KNOB8_DIFF          0x80
#define KNOB9_DIFF          0x80

#define KNOB0_VOL_MAX       12
#define KNOB1_VOL_MAX       12
#define KNOB2_VOL_MAX       12
#define KNOB3_VOL_MAX       12
#define KNOB4_VOL_MAX       5
#define KNOB5_VOL_MAX       12
#define KNOB6_VOL_MAX       12
#define KNOB7_VOL_MAX       12
#define KNOB8_VOL_MAX       12
#define KNOB9_VOL_MAX       12

#define KNOB_INIT           0xFFFF



/*------------------------------------------------------------------------------
*函数功能: 旋钮控制配置表
*参数1：旋钮AD初始值
*参数2：旋钮步进
*参数3：旋钮最大AD值
*参数4：旋钮间距
*参数5：旋钮AD值列表
*-------------------------------------------------------------------------------*/

knob_ctl_t knob_ctl_table[KADC_CH_MAX]  = {


    { KNOB_INIT, KNOB0_VOL_MAX, KNOB0_VOL_MAX, KNOB0_DIFF,
        {
            0x880,              // 0
            0x900,              // 1
            0x980,              // 2
            0xA00,              // 3
            0xA80,              // 4
            0xB00,              // 5
            0xB80,              // 6
            0xC00,              // 7
            0xC80,              // 8
            0xD00,              // 9
            0xE00,              // 10
            0xF00,              // 11
        },
    },

    { KNOB_INIT, KNOB1_VOL_MAX, KNOB1_VOL_MAX, KNOB1_DIFF,
        {
            0x080,              // 0
            0x100,              // 1
            0x1A0,              // 2
            0x240,              // 3
            0x2E0,              // 4
            0x380,              // 5
            0x420,              // 6
            0x4C0,              // 7
            0x560,              // 8
            0x600,              // 9
            0x700,              // 10
            0xF00,              // 11
        },
    },

    { KNOB_INIT, KNOB2_VOL_MAX, KNOB2_VOL_MAX, KNOB2_DIFF,
        {
            0x080,              // 0
            0x100,              // 1
            0x1A0,              // 2
            0x240,              // 3
            0x2E0,              // 4
            0x380,              // 5
            0x420,              // 6
            0x4C0,              // 7
            0x560,              // 8
            0x600,              // 9
            0x700,              // 10
            0xF00,              // 11
        },
    },
    
    { KNOB_INIT, KNOB3_VOL_MAX, KNOB3_VOL_MAX, KNOB3_DIFF,
        {
            0x080,              // 0
            0x100,              // 1
            0x1A0,              // 2
            0x240,              // 3
            0x2E0,              // 4
            0x380,              // 5
            0x420,              // 6
            0x4C0,              // 7
            0x560,              // 8
            0x600,              // 9
            0x700,              // 10
            0xF00,              // 11
        },
    },

    { KNOB_INIT, KNOB4_VOL_MAX, KNOB4_VOL_MAX, KNOB4_DIFF,
        {
            0x080,              // 0
            0x100,              // 1
            0x1A0,              // 2
            0x240,              // 3
            0x2E0,              // 4
            0x380,              // 5
            0x420,              // 6
            0x4C0,              // 7
            0x560,              // 8
            0x600,              // 9
            0x700,              // 10
            0xF00,              // 11
        },
    },

    { KNOB_INIT, KNOB5_VOL_MAX, KNOB5_VOL_MAX, KNOB5_DIFF,
        {
            0x080,              // 0
            0x100,              // 1
            0x1A0,              // 2
            0x240,              // 3
            0x2E0,              // 4
            0x380,              // 5
            0x420,              // 6
            0x4C0,              // 7
            0x560,              // 8
            0x600,              // 9
            0x700,              // 10
            0xF00,              // 11
        },
    },

    { KNOB_INIT, KNOB6_VOL_MAX, KNOB6_VOL_MAX, KNOB6_DIFF,
        {
            0x080,              // 0
            0x100,              // 1
            0x1A0,              // 2
            0x240,              // 3
            0x2E0,              // 4
            0x380,              // 5
            0x420,              // 6
            0x4C0,              // 7
            0x560,              // 8
            0x600,              // 9
            0x700,              // 10
            0xF00,              // 11
        },
    },

    { KNOB_INIT, KNOB7_VOL_MAX, KNOB7_VOL_MAX, KNOB7_DIFF,
        {
            0x080,              // 0
            0x100,              // 1
            0x1A0,              // 2
            0x240,              // 3
            0x2E0,              // 4
            0x380,              // 5
            0x420,              // 6
            0x4C0,              // 7
            0x560,              // 8
            0x600,              // 9
            0x700,              // 10
            0xF00,              // 11
        },
    },

    { KNOB_INIT, KNOB8_VOL_MAX, KNOB8_VOL_MAX, KNOB8_DIFF,
        {
            0x080,              // 0
            0x100,              // 1
            0x1A0,              // 2
            0x240,              // 3
            0x2E0,              // 4
            0x380,              // 5
            0x420,              // 6
            0x4C0,              // 7
            0x560,              // 8
            0x600,              // 9
            0x700,              // 10
            0xF00,              // 11
        },
    },

    { KNOB_INIT, KNOB9_VOL_MAX, KNOB9_VOL_MAX, KNOB9_DIFF,
        {
            0x080,              // 0
            0x100,              // 1
            0x1A0,              // 2
            0x240,              // 3
            0x2E0,              // 4
            0x380,              // 5
            0x420,              // 6
            0x4C0,              // 7
            0x560,              // 8
            0x600,              // 9
            0x700,              // 10
            0xF00,              // 11
        },
    },
};
#endif


#endif



#if KEY_IR_EN
AT_SRAM(.irrx_rodata)
const uint8_t kir_tbl[IR_KEY_NUM][2] = {
    {IR_KEY00, KEY_CH_DOWN},    {IR_KEY01, KEY_MODE},       {IR_KEY02, KEY_CH_UP},
    {IR_KEY10, KEY_PREV},       {IR_KEY11, KEY_NEXT},       {IR_KEY12, KEY_PLAY},
    {IR_KEY20, KEY_VOL_DOWN},   {IR_KEY21, KEY_VOL_UP},     {IR_KEY22, KEY_EQ},
    {IR_KEY30, KEY_NUM0},       {IR_KEY31, KEY_N100_PLUS},  {IR_KEY32, KEY_REPEAT},
    {IR_KEY40, KEY_NUM1},       {IR_KEY41, KEY_NUM2},       {IR_KEY42, KEY_NUM3},
    {IR_KEY50, KEY_NUM4},       {IR_KEY51, KEY_NUM5},       {IR_KEY52, KEY_NUM6},
    {IR_KEY60, KEY_NUM7},       {IR_KEY61, KEY_NUM8},       {IR_KEY62, KEY_NUM9},
};
#endif


#if KEY_IO_EN
/*------------------------------------------------------------------------------
*函数功能: io 按键配置表
*参数1：使能按键
*参数2：按键控制pin
*参数3：按键值
*-------------------------------------------------------------------------------*/
key_io_info_t key_io_info_table[KEY_IO_TBLE] = {

#ifdef PIN_KIO0
    {true, PIN_KIO0, KEY_PREV},
#else
    {false, 0, 0},
#endif

#ifdef PIN_KIO1
    {true, PIN_KIO1, KEY_PLAY},
#else
    {false, 0, 0},
#endif

#ifdef PIN_KIO2
    {true, PIN_KIO2, KEY_NEXT},
#else
    {false, 0, 0},
#endif

#ifdef PIN_KIO3
    {true, PIN_KIO3, KEY_VOL_DOWN},
#else
    {false, 0, 0},
#endif
#ifdef PIN_KIO4
    {true, PIN_KIO4, KEY_VOL_UP},
#else
    {false, 0, 0},
#endif
#ifdef PIN_KIO5
    {true, PIN_KIO5, KEY_MODE},
#else
    {false, 0, 0},
#endif
#ifdef PIN_KIO6
    {true, PIN_KIO6, KEY_REPEAT},
#else
    {false, 0, 0},
#endif

#ifdef PIN_KIO7
    {true, PIN_KIO7, KEY_EQ},
#else
    {false, 0, 0},
#endif
};

#endif

#if KEY_DIG_EN

/*------------------------------------------------------------------------------
*函数功能: 数字按键控制表
*参数1：通道使能
*参数2：多击使能
*参数3：
*参数4：
*参数5：中断配置
*-------------------------------------------------------------------------------*/

const kdig_ch_param_t kdig_init_config[KDIG_CH_MAX] = {
    //press_en, multi_click_en, down_level, multi_click_mode,
    //irq

    //dkc0:
    {KDIG0_EN,ENABLE, KDIG_DOWN_LVL_LOW, KDIG_MC_MODE_NORMAL,
    KDIG_DOWN_IRQ_EN|KDIG_UP_IRQ_EN|KDIG_HOLD_IRQ_EN|KDIG_LONG_IRQ_EN|KDIG_MC_IRQ_EN},

    //dkc1:
    {KDIG1_EN,ENABLE, KDIG_DOWN_LVL_LOW, KDIG_MC_MODE_NORMAL,
    KDIG_DOWN_IRQ_EN|KDIG_UP_IRQ_EN|KDIG_HOLD_IRQ_EN|KDIG_LONG_IRQ_EN|KDIG_MC_IRQ_EN},

    //dkc2:
    {KDIG2_EN,ENABLE, KDIG_DOWN_LVL_LOW, KDIG_MC_MODE_NORMAL,
    KDIG_DOWN_IRQ_EN|KDIG_UP_IRQ_EN|KDIG_HOLD_IRQ_EN|KDIG_LONG_IRQ_EN|KDIG_MC_IRQ_EN},
};


/*------------------------------------------------------------------------------
*函数功能: 数字按键配置表
*参数1：使能按键
*参数3：按键值
*-------------------------------------------------------------------------------*/

key_dig_info_t kdig_info_table[KEY_DIG_TBLE] = {

#if KDIG0_EN
    {true, KEY_PREV},
#else
    {false, 0},
#endif

#if KDIG1_EN
    {true, KEY_PLAY},
#else
    {false, 0},
#endif

#if KDIG2_EN
    {true, KEY_NEXT},
#else
    {false, 0},
#endif
};

#endif


/*------------------------------------------------------------------------------
*函数功能: 按键功能初始化：短按，长按，多击
*-------------------------------------------------------------------------------*/

void key_func_init(void)
{
   key_func_ctl key_func_t;
   key_func_t.key_func_s_en  = KEY_FUNC_S_EN;
   key_func_t.key_func_u_en  = KEY_FUNC_U_EN;
   key_func_t.key_func_su_en = KEY_FUNC_SU_EN;
   key_func_t.key_func_l_en  = KEY_FUNC_L_EN;
   key_func_t.key_func_lu_en = KEY_FUNC_LU_EN;
   key_func_t.key_func_ll_en = KEY_FUNC_LL_EN;
   key_func_t.key_func_h_en  = KEY_FUNC_H_EN;
   key_func_t.key_func_m_en  = KEY_FUNC_M_EN;
   key_func_t.key_onoff_mode = MODE_KEY_ONOFF;
   key_func_t.key_short_time = KEY_TIME_SHORT;
   key_func_t.key_up_time    = KEY_TIME_UP;
   key_func_t.key_hold_time  = KEY_TIME_HOLD;
   key_func_t.key_long_time  = KEY_TIME_LONG;
   key_set_ctl_func(&key_func_t);
}


#if KEY_AD_EN

/*------------------------------------------------------------------------------
*函数功能: AD按键 GPIO口初始化
*-------------------------------------------------------------------------------*/

void kadc_gpio_init(void)
{

#if KADC0_EN
    HAL_gpio_func_set(KADC0_PIN_NUM, KADC0_FUNC);
    //HAL_gpio_pull_set(KADC0_PIN_NUM, PIN_PULL_UP2K);
#endif
#if KADC1_EN
    HAL_gpio_func_set(KADC1_PIN_NUM, KADC1_FUNC);
    HAL_gpio_pull_set(KADC1_PIN_NUM, PIN_PULL_UP2K);  //PD0默认开启2k上拉
#endif

#if KADC2_EN
    HAL_gpio_func_set(KADC2_PIN_NUM, KADC2_FUNC);
    //HAL_gpio_pull_set(KADC2_PIN_NUM, PIN_PULL_UP100K);
#endif
#if KADC3_EN
    HAL_gpio_func_set(KADC3_PIN_NUM, KADC3_FUNC);
    HAL_gpio_pull_set(KADC3_PIN_NUM, PIN_PULL_DISABLE);
#endif
#if KADC4_EN
    HAL_gpio_func_set(KADC4_PIN_NUM, KADC4_FUNC);
    //HAL_gpio_pull_set(KADC4_PIN_NUM, PIN_PULL_UP100K);
#endif
#if KADC5_EN
    HAL_gpio_func_set(KADC5_PIN_NUM, KADC5_FUNC);
    //HAL_gpio_pull_set(KADC5_PIN_NUM, PIN_PULL_UP100K);
#endif
#if KADC6_EN
    HAL_gpio_func_set(KADC6_PIN_NUM, KADC5_FUNC);
    //HAL_gpio_pull_set(KADC6_PIN_NUM, PIN_PULL_UP100K);
#endif
#if KADC7_EN
    HAL_gpio_func_set(KADC7_PIN_NUM, KADC5_FUNC);
    //HAL_gpio_pull_set(KADC7_PIN_NUM, PIN_PULL_UP100K);
#endif
#if KADC8_EN
    HAL_gpio_func_set(KADC8_PIN_NUM, KADC5_FUNC);
    //HAL_gpio_pull_set(KADC8_PIN_NUM, PIN_PULL_UP100K);
#endif
#if KADC9_EN
    HAL_gpio_func_set(KADC9_PIN_NUM, KADC5_FUNC);
    //HAL_gpio_pull_set(KADC9_PIN_NUM, PIN_PULL_UP100K);
#endif
}

/*------------------------------------------------------------------------------
*函数功能: AD按键 唤醒初始化
*-------------------------------------------------------------------------------*/

void kadc_wakeup_init(void)
{
#if SLEEP_EN
#if KADC0_EN && !KADC0_IRQ
    HAL_gpio_irq_config(KADC0_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
#endif
#if KADC1_EN && !KADC1_IRQ
    HAL_gpio_irq_config(KADC1_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
#endif
#if KADC2_EN && !KADC2_IRQ
    HAL_gpio_irq_config(KADC2_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
#endif
#if KADC3_EN && !KADC3_IRQ
    HAL_gpio_irq_config(KADC3_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
#endif
#if KADC4_EN && !KADC4_IRQ
    HAL_gpio_irq_config(KADC4_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
#endif
#if KADC5_EN && !KADC5_IRQ
    HAL_gpio_irq_config(KADC5_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
#endif
#if KADC6_EN && !KADC6_IRQ
    HAL_gpio_irq_config(KADC6_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
#endif
#if KADC7_EN && !KADC7_IRQ
    HAL_gpio_irq_config(KADC7_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
#endif
#if KADC8_EN && !KADC8_IRQ
    HAL_gpio_irq_config(KADC8_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
#endif
#if KADC9_EN && !KADC9_IRQ
    HAL_gpio_irq_config(KADC9_PIN_NUM, (0xFF<<8)|(GPIO_INT_CLK_SEL<<7)|(0x07<<4));
#endif

#endif
}

#endif

#if KEY_DIG_EN
void kdig_gpio_init(void)
{
#if KDIG0_EN
    HAL_gpio_config(KDIG0_PIN_CFG);
#endif

#if KDIG1_EN
    HAL_gpio_config(KDIG1_PIN_CFG);
#endif

#if KDIG2_EN
    HAL_gpio_config(KDIG2_PIN_CFG);
#endif
}
#endif



/*------------------------------------------------------------------------------
*函数功能: 计算并确认多击按键事件
*-------------------------------------------------------------------------------*/

#if KEY_FUNC_M_EN
AT_SRAM(.mutil_key)
uint32_t key_multiple_click_process(uint32_t key_value)
{
    static uint8_t key_time = 0;    //倒计时
    static uint8_t key_cnt = 0;     //按键次数
    uint32_t kret;                  //返回值

    if (key_time > 0) {
        key_time--;
    }

    if (key_time == 1) {
        if (key_cnt == 1) {
            //单击
            kret = KEY_EVENT_SU | KEY_M_CLICK_NAME;
        } else {
            //多击
            kret = KEY_EVENT_M | KEY_M_CLICK_NAME | (key_cnt<<8);
        }
        key_cnt = 0;
        return kret;
    }

    if (key_value == KEY_M_CLICK_NAME) {
        key_time = KEY_M_CLICK_INTERVAL;
        key_cnt++;
        return KEY_EVENT_NONE;
    }

    return key_value;
}
#endif



/*------------------------------------------------------------------------------
*函数功能: 计算并确认按键事件
*-------------------------------------------------------------------------------*/

AT_SRAM(.key)
uint32_t key_get_value(void)
{
    uint32_t kret;
    kret = KEY_EVENT_NONE;

    if((key.cur_val == KEY_NONE) || (key.cur_val != key.last_val)) {//松手 或者有按键按下
        if(key.up_cnt * KEY_SCAN_CYCLE < KEY_TIME_UP) {
            key.up_cnt++;                           //松手计数5ms 加1 ，开机这个地方会先跑
        } else {                                                    //松手25ms 开始判断松手事件
            if(key.cur_cnt * KEY_SCAN_CYCLE >= KEY_TIME_LONG) {     //松手前 按下了750ms以上
#if KEY_FUNC_U_EN
                event_put(key.last_val | KEY_EVENT_U, NULL, 0);     //触发松手事件
#endif
#if KEY_FUNC_LU_EN
                kret = key.last_val | KEY_EVENT_LU;                 //触发长按松手事件
#endif
            } else if (key.cur_cnt * KEY_SCAN_CYCLE >= KEY_TIME_SHORT) { ////松手前 按下了25ms以上
#if KEY_FUNC_U_EN
                event_put(key.last_val | KEY_EVENT_U, NULL, 0);         //触发松手事件
#endif
#if KEY_FUNC_SU_EN
                kret = key.last_val | KEY_EVENT_SU;                     //触发短按松手事件
#endif
            }
            key.last_val = key.cur_val;       //保存上次事件，开机第一次按键会先跑这里
            key.cur_cnt = 0;                  //按下次数清0
        }
        key.long_cnt = 0;
    } else {
        key.cur_cnt++;                                       //按下计数5ms 加1
        if(key.cur_cnt * KEY_SCAN_CYCLE == KEY_TIME_SHORT) { //按下25ms 触发短按事件
            key.up_cnt = 0;                                  //松手计数清0
#if KEY_FUNC_S_EN
            kret = key.last_val | KEY_EVENT_S;
#endif
#if (SENSOR_FORCE_EN && (SENSOR_FORCE_DEVICE_SEL == 1))
            sensor_force_reset_timer_start();
#endif
        } else if(key.cur_cnt * KEY_SCAN_CYCLE == KEY_TIME_LONG) {//按下750ms 触发长按事件
#if KEY_FUNC_L_EN
            kret = key.last_val | KEY_EVENT_L;
#endif
        } else if(key.cur_cnt * KEY_SCAN_CYCLE == (KEY_TIME_LONG + KEY_TIME_HOLD)) {//按下900ms 触发hold事件
#if KEY_FUNC_H_EN
            kret = key.last_val | KEY_EVENT_H;
#endif
            key.cur_cnt = KEY_TIME_LONG / KEY_SCAN_CYCLE;//按下计数 750/5
        }

        key.long_cnt++; //长按计数5ms 加1

#if KEY_FUNC_LL_EN
        uint32_t ktime;
        uint32_t kll;
        uint8_t n;
        ktime = key.long_cnt * KEY_SCAN_CYCLE; //统计一共按下多少ms
        if (ktime % KEY_CNT_1000MS == 0) {
            n = ktime / KEY_CNT_1000MS; //统计按下多少s
            if ((n > 0) && (n < 10)) {
                kll = key.last_val | KEY_EVENT_L | (n<<8);
                event_put(kll, NULL, 0); //触发按下长按多少s事件
                //logi("k ll:%x", kll);
            }
        }
#endif
    }

    return kret;

}

#if KEY_IR_EN

AT_SRAM(.irrx)
void kir_get_data(void)
{

    sleep_delay_set();

    kir.cnt = 0;

    kir.user_code = HAL_irrx_get_usercode();
    kir.data_code = HAL_irrx_get_datacode();

    if (kir.is_data_shift) {
        kir.user_code = kir_shift_16bit(kir.user_code);
        kir.data_code = kir_shift_8bit(kir.data_code);
    }
    logi("ir[%04x,%02x]", kir.user_code,kir.data_code);
}


AT_SRAM(.irrx)
uint8_t kir_shift_8bit(uint8_t data)
{
    data = (data<<4) | (data>>4);
    data = ((data<<2)&0xCC) | ((data>>2)&0x33);
    data = ((data<<1)&0xAA) | ((data>>1)&0x55);
    return data;
}

AT_SRAM(.irrx)
uint16_t kir_shift_16bit(uint16_t data)
{
    data = (data<<8) | (data>>8);
    data = ((data<<4)&0xF0FF) | ((data>>4)&0xFF0F);
    data = ((data<<2)&0xCCCC) | ((data>>2)&0x3333);
    data = ((data<<1)&0xAAAA) | ((data>>1)&0x5555);
    return data;
}

void irrx_init(void)
{
    module_ccu_enable(CCU_IR);
#if 1
    HAL_clk_set_src(CLK_IR, CLK_32K_HOSC);
#else
    HAL_clk_set_src(CLK_IR, CLK_LOSC);      //the actual configuration is CLK_32K_HOSC
#endif
    HAL_clk_enable(CLK_IR);
    
    IRRX_InitTypeDef irrx_param;
    irrx_param.logic_threshold = 0x50;
    irrx_param.noise_threshold = 0x4;
    irrx_param.rppi = 1;
    irrx_param.wk_t9ms_max = 0xFF;
    irrx_param.wk_t9ms_min = 0x10;
    irrx_param.t9ms_max = 0x5A;
    irrx_param.t9ms_min = 0x3C;
    irrx_param.t2d25ms_max = 0x58;
    irrx_param.t2d25ms_min = 0x40;
    irrx_param.t4d5ms_max = 0x5A;
    irrx_param.t4d5ms_min = 0x3C;
    irrx_param.t1d69ms_max = 0x3D;
    irrx_param.t1d69ms_min = 0x28;
    irrx_param.t0d56ms_max = 0x20;
    irrx_param.t0d56ms_min = 0x08;

    HAL_irrx_module_init(&irrx_param,kir_get_data);
}


void kir_init(bool ir_data_shift, uint8_t key_max,const uint8_t (*key_table)[2])
{
    kir.user_code = KEY_IR_USER_NONE;
    kir.data_code = KEY_NONE;
    kir.cnt = 0xFF;
    kir.is_data_shift = ir_data_shift;
    kir.key_num_max = key_max;
    kir.key_table = key_table;
    irrx_init();
}

AT_SRAM(.irrx)
uint8_t kir_get_key(void)
{
    uint32_t n;
    if(kir.cnt < 23) {   //23*5ms > 110ms, time out
        kir.cnt++;
    } else {
        kir.user_code = KEY_IR_USER_NONE;
        kir.data_code = KEY_NONE;
        kir.cnt = 0xFF;
    }

    if (((kir.user_code != 0xFF00) && kir.is_data_shift) || ((kir.user_code != 0x00FF) && !kir.is_data_shift)) {
        return KEY_NONE;
    }

    n = kir.key_num_max;
    while(n--) {
        if(kir.data_code == kir.key_table[n][0]) {
            return kir.key_table[n][1];
        }
    }

    return KEY_NONE;
}
#endif

/*------------------------------------------------------------------------------
*函数功能: 按键扫描
*-------------------------------------------------------------------------------*/

AT_SRAM(.key)
void key_scan(void)
{
    //logi("key.cur_val = %#x\n",key.cur_val);
#if (KEY_IO_EN || KEY_IR_EN || (KEY_AD_EN&&KADC_SCAN_EN) || KEY_CODING_EN)
    uint32_t kret;
    kret = KEY_NONE;
    key.cur_val = KEY_NONE;

#if KEY_IO_EN
    key.cur_val = kio_get_key(KIO_M_EN);
#endif

#if KEY_IR_EN
    if (KEY_NONE == key.cur_val) {
        key.cur_val = kir_get_key(); 
        if(kir.user_code != 0 && kir.data_code != 0) {
            logi("IR get key %x %x", kir.user_code, kir.data_code);
        }

        if(key.cur_val != 0) {
            logi("key.cur_val = %04x", key.cur_val);
        }
    }
#endif

#if KEY_AD_EN
    if (KEY_NONE == key.cur_val) {
        key.cur_val = kadc_scan_get_key();
        // logi("K[1]:%04x\n",kadc_val_get[1]);
        //logi("K[3]:%04x\n",kadc_val_get[3]);
    }
#endif

#if KEY_CODING_EN
    if (KEY_NONE == key.cur_val) {
        key.cur_val = kcd_get_key();
    }
#endif

    if (KEY_NONE != key.cur_val) {
        sleep_delay_set();
    }

    kret = key_get_value();

#if KEY_FUNC_M_EN
    kret = key_multiple_click_process(kret);
#endif

    if (kret != KEY_EVENT_NONE) {
        if ((KEY_EVENT_SU | KEY_MODE) == kret) {
#if !MODE_KEY_ONOFF
            mode_update(1);
            key_mode_set(true);
#endif
        }
        event_put(kret, NULL, 0);
        logi("k:%x", kret);
    }
#endif

#if KEY_ONOFF_EN
    key_onoff_scan();
#endif
}

AT_SRAM(.key)
void app_knob_adc_cb(uint8_t ch, uint32_t kval, uint32_t kstep)
{
    logi("#ch:%d", ch);
    logi("#kval:%#x", kval);
    logi("#kstep:%d", kstep);
}


#if KADC3_EN
// void get_key_val_adc3_for_audio(void* xTimer)
void* get_key_val_adc3_for_audio(void *para)
// int get_key_val_adc3_for_audio(void)
{
    uint32_t value;
    uint8_t volume_value = 90;
    while(1)
    {
        value =  kadc_val_get[3];
        if (value <= 0x199) // 音量0
        {
            volume_value = 0;
        }
        else if (value > (uint32_t)0x199 && value <= (uint32_t)0x332) // 音量1
        {
            volume_value = 10;
        }
        else if (value > (uint32_t)0x332 && value <= (uint32_t)0x4CC) // 音量2
        {
            volume_value = 20;
        }
        else if (value > (uint32_t)0x4CC && value <= (uint32_t)0x666) // 音量3
        {
            volume_value = 30;
        }
        else if (value > (uint32_t)0x666 && value <= (uint32_t)0x7E0) // 音量4
        {
            volume_value = 40;
        }
        else if (value > (uint32_t)0x7E0 && value <= (uint32_t)0x97A) // 音量5
        {
            volume_value = 50;
        }
        else if (value > (uint32_t)0x97A && value <= (uint32_t)0xA92) // 音量6
        {
            volume_value = 60;
        }
        else if (value > (uint32_t)0xA92 && value <= (uint32_t)0xC0C) // 音量7
        {
            volume_value = 70;
        }
        else if (value > (uint32_t)0xC0C && value <= (uint32_t)0xD86) // 音量8
        {
            volume_value = 80;
        }
        else // 音量9
        {
            volume_value = 90;
        }
        am_playback_volume_set(volume_value);
        // am_record_volume_set(volume_value);
        #if LCD_SEG_EN
        disp_lcd_volume_update(volume_value / 10);//音量
        #endif
        OAL_sleep(500);
    }
    return NULL;
}

#endif


/*------------------------------------------------------------------------------
*函数功能: 按键模块初始化
*-------------------------------------------------------------------------------*/

void app_key_init(void)
{

    key_func_init();

#if KEY_IR_EN
    HAL_gpio_config(IRRX_PIN_CFG);
    kir_init(IR_DATA_SHIFT,IR_KEY_NUM,kir_tbl);
#endif

#if KEY_AD_EN
    kadc_gpio_init();
    kadc_wakeup_init();
#if KADC_KNOB_EN
    knob_cb_t app_knob_cb;
    app_knob_cb.knob_adc_cb = app_knob_adc_cb;
    knob_vol_init(knob_ctl_table, app_knob_cb);
#endif
    kadc_init(kadc_init_config, kadc_irq_knob_config,
              KADC_IRQ_EN,app_knob_key_threshod);                    //ad key 初始化
#if KADC0_EN
    key_event_table_init(0x0, key_event_table_00, KEY_EVENT_TAB_MAX,&kadc_val_get[0]); //ad key 事件初始化
#endif
#if KADC1_EN
    key_event_table_init(0x1, key_event_table_01, KEY_EVENT_TAB_MAX,&kadc_val_get[1]); //ad key 事件初始化
#endif
#if KADC2_EN
    key_event_table_init(0x2, key_event_table_02, KEY_EVENT_TAB_MAX,&kadc_val_get[2]); //ad key 事件初始化
#endif
#if KADC3_EN
    key_event_table_init(0x3, key_event_table_03, KEY_EVENT_TAB_MAX,&kadc_val_get[3]); //ad key 事件初始化
#endif
#if KADC4_EN
    key_event_table_init(0x4, key_event_table_04, KEY_EVENT_TAB_MAX,&kadc_val_get[4]); //ad key 事件初始化
#endif
#if KADC5_EN
    key_event_table_init(0x5, key_event_table_05, KEY_EVENT_TAB_MAX,&kadc_val_get[5]); //ad key 事件初始化
#endif
#if KADC6_EN
    key_event_table_init(0x6, key_event_table_06, KEY_EVENT_TAB_MAX,&kadc_val_get[6]); //ad key 事件初始化
#endif
#if KADC7_EN
    key_event_table_init(0x7, key_event_table_07, KEY_EVENT_TAB_MAX,&kadc_val_get[7]); //ad key 事件初始化
#endif
#if KADC8_EN
    key_event_table_init(0x8, key_event_table_08, KEY_EVENT_TAB_MAX,&kadc_val_get[8]); //ad key 事件初始化
#endif
#if KADC9_EN
    key_event_table_init(0x9, key_event_table_09, KEY_EVENT_TAB_MAX,&kadc_val_get[9]); //ad key 事件初始化
#endif
#endif

#if KADC3_EN
    // if(key_audio_auto == NULL) {
    //     OAL_timer_create(&key_audio_auto, "auto key aduio", OAL_ms_to_ticks(500), get_key_val_adc3_for_audio,
    //         (void *)1, 1,OAL_TIMER_AUTO_ACTIVATE);
    //         logi("##key_audio_task_handler Creat##\r\n");
    // }
    // logi("##key_audio_task_handler Creat##\r\n");

    // if(key_audio_task_handler == NULL)
    // {
    //     key_audio_task_handler = OAL_thread_create(get_key_val_adc3_for_audio, NULL, "auto_key_aduio", KEY_AUDIO_TASK_PRIO, KEY_AUDIO_TASK_SIZE);
    //     logi("##key_audio_task_handler Creat##\r\n");
    // }
#endif

#if KEY_IO_EN
    kio_init(key_io_info_table, KEY_IO_TBLE);
#endif

#if KEY_CODING_EN
    kcd_init(KEY_CODING_A_PIN_NUM, KEY_CODING_A_PIN_CFG,
             KEY_CODING_B_PIN_NUM, KEY_CODING_B_PIN_CFG);
#endif

#if KEY_ONOFF_EN
    key_onoff_init();
#endif

#if KEY_DIG_EN
    kdig_gpio_init();
    kdig_init(KDIG_CLOCKDIV_128, kdig_init_config, kdig_info_table, KEY_DIG_TBLE);
#endif


}
