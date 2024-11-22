#ifndef _APP_SPDIF_H_
#define _APP_SPDIF_H_
#include "user_config.h"

#if SPDIF_TX_PIN_SEL == 0
#define SPDIF_TX_PIN_CFG     (PIN_B3 | PIN_FUNC10 | PIN_DRI1)
#elif SPDIF_TX_PIN_SEL == 1
#define SPDIF_TX_PIN_CFG     (PIN_B5 | PIN_FUNC10 | PIN_DRI1 )
#elif SPDIF_TX_PIN_SEL == 2
#define SPDIF_TX_PIN_CFG     (PIN_B7 | PIN_FUNC4 | PIN_DRI1)
#elif SPDIF_TX_PIN_SEL == 3
#define SPDIF_TX_PIN_CFG     (PIN_B8 | PIN_FUNC5 | PIN_DRI1)
#elif SPDIF_TX_PIN_SEL == 4
#define SPDIF_TX_PIN_CFG     (PIN_B14 | PIN_FUNC8 | PIN_DRI1)
#elif SPDIF_TX_PIN_SEL == 5
#define SPDIF_TX_PIN_CFG     (PIN_B24 | PIN_FUNC10 | PIN_DRI1)
#elif SPDIF_TX_PIN_SEL == 6
#define SPDIF_TX_PIN_CFG     (PIN_B28 | PIN_FUNC9 | PIN_DRI1)

#endif

#if SPDIF_RX_PIN_SEL == 0
#define SPDIF_OPT_PIN_CFG     (PIN_B4 | PIN_FUNC10 | PIN_DRI1)
#define SPDIF_OPT_DISABLE     (PIN_B4 | PIN_FUNCDISABLE)
#elif SPDIF_RX_PIN_SEL == 1
#define SPDIF_OPT_PIN_CFG     (PIN_B6 | PIN_FUNC10 | PIN_DRI1)
#define SPDIF_OPT_DISABLE     (PIN_B6 | PIN_FUNCDISABLE)
#elif SPDIF_RX_PIN_SEL == 2
#define SPDIF_OPT_PIN_CFG     (PIN_B7 | PIN_FUNC5 | PIN_DRI1)
#define SPDIF_OPT_DISABLE     (PIN_B7 | PIN_FUNCDISABLE)
#elif SPDIF_RX_PIN_SEL == 3
#define SPDIF_OPT_PIN_CFG     (PIN_B10 | PIN_FUNC8 | PIN_DRI1)
#define SPDIF_OPT_DISABLE     (PIN_B10 | PIN_FUNCDISABLE)
#elif SPDIF_RX_PIN_SEL == 4
#define SPDIF_OPT_PIN_CFG     (PIN_B15 | PIN_FUNC7 | PIN_DRI1)
#define SPDIF_OPT_DISABLE     (PIN_B15 | PIN_FUNCDISABLE)
#elif SPDIF_RX_PIN_SEL == 5
#define SPDIF_OPT_PIN_CFG     (PIN_B24 | PIN_FUNC9 | PIN_DRI1)
#define SPDIF_OPT_DISABLE     (PIN_B24 | PIN_FUNCDISABLE)
#elif SPDIF_RX_PIN_SEL == 6
#define SPDIF_OPT_PIN_CFG     (PIN_B29 | PIN_FUNC9 | PIN_DRI1)
#define SPDIF_OPT_DISABLE     (PIN_B29 | PIN_FUNCDISABLE)
#endif

#if SPDIF_COA_RX_PIN_SEL == 1  //光纤
#define SPDIF_COA_PIN_CFG     (PIN_D1 | PIN_FUNC7 | PIN_DRI1)
#define SPDIF_COA_DISABLE     (PIN_D1 | PIN_FUNCDISABLE)
#endif

enum {
    SEL_OPT = 0,    //光纤
    SEL_COA = 1,    //同轴
};

#define M_SPDIF_TX  0   //TX模式
#define M_SPDIF_RX  1   //RX模式

typedef struct {
    uint8_t* rec_buf;//[REC_BUF_LEN/4];

    //struct ring_buffer spdif_tx_buf_ctrl;
    //struct ring_buffer spdif_rx_buf_ctrl;
    //uint8_t* spdif_tx_tmpbuf;
} spdif_stream_info_t;
void spdif_input_sel(uint8_t input_sel);

#endif
