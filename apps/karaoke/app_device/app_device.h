#ifndef _APP_DEVICE_H_
#define _APP_DEVICE_H_

#if SD_EN
#if (SD_D0_PIN_SEL == 0)
#define SD_D0_PIN_CFG       (PIN_B0 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (SD_D0_PIN_SEL == 1)
#define SD_D0_PIN_CFG       (PIN_B3 | PIN_FUNC6 | PIN_DRI2 | PIN_PULL_UP2K)
#elif (SD_D0_PIN_SEL == 2)
#define SD_D0_PIN_CFG       (PIN_A17 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (SD_D0_PIN_SEL == 3)
#define SD_D0_PIN_CFG       (PIN_B15 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (SD_D0_PIN_SEL == 4)
#define SD_D0_PIN_CFG       (PIN_B25 | PIN_FUNC4 | PIN_DRI1 | PIN_PULL_UP2K)
#endif

#if (SD_D1_PIN_SEL == 0)
#define SD_D1_PIN_CFG       //FIXME
#elif (SD_D1_PIN_SEL == 1)
#define SD_D1_PIN_CFG       //FIXME
#elif (SD_D1_PIN_SEL == 2)
#define SD_D1_PIN_CFG       //FIXME
#elif (SD_D1_PIN_SEL == 3)
#define SD_D1_PIN_CFG       //FIXME
#elif (SD_D1_PIN_SEL == 4)
#define SD_D1_PIN_CFG       (PIN_B28 | PIN_FUNC4 | PIN_DRI1 | PIN_PULL_UP2K)
#endif

#if (SD_D2_PIN_SEL == 0)
#define SD_D2_PIN_CFG       //FIXME
#elif (SD_D2_PIN_SEL == 1)
#define SD_D2_PIN_CFG       //FIXME
#elif (SD_D2_PIN_SEL == 2)
#define SD_D2_PIN_CFG       //FIXME
#elif (SD_D2_PIN_SEL == 3)
#define SD_D2_PIN_CFG       //FIXME
#elif (SD_D2_PIN_SEL == 4)
#define SD_D2_PIN_CFG       (PIN_B29 | PIN_FUNC4 | PIN_DRI1 | PIN_PULL_UP2K)
#endif

#if (SD_D3_PIN_SEL == 0)
#define SD_D3_PIN_CFG       //FIXME
#elif (SD_D3_PIN_SEL == 1)
#define SD_D3_PIN_CFG       //FIXME
#elif (SD_D3_PIN_SEL == 2)
#define SD_D3_PIN_CFG       //FIXME
#elif (SD_D3_PIN_SEL == 3)
#define SD_D3_PIN_CFG       //FIXME
#elif (SD_D3_PIN_SEL == 4)
#define SD_D3_PIN_CFG       (PIN_B24 | PIN_FUNC4 | PIN_DRI1 | PIN_PULL_UP2K)
#endif

#if (SD_CLK_PIN_SEL == 0)
#define SD_CLK_PIN_CFG      (PIN_B1 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP100K)
#define SD_CLK_PIN_NUM      (PIN_B1)
#define SD_CLK_FUNC         (PIN_FUNC2)
#elif (SD_CLK_PIN_SEL == 1)
#define SD_CLK_PIN_CFG      (PIN_B6 | PIN_FUNC4 | PIN_DRI1 | PIN_PULL_UP100K)
#define SD_CLK_PIN_NUM      (PIN_B6)
#define SD_CLK_FUNC         (PIN_FUNC4)
#elif (SD_CLK_PIN_SEL == 2)
#define SD_CLK_PIN_CFG      (PIN_A15 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP100K)
#define SD_CLK_PIN_NUM      (PIN_A15)
#define SD_CLK_FUNC         (PIN_FUNC2)
#elif (SD_CLK_PIN_SEL == 3)
#define SD_CLK_PIN_CFG      (PIN_B13 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP100K)
#define SD_CLK_PIN_NUM      (PIN_B13)
#define SD_CLK_FUNC         (PIN_FUNC2)
#elif (SD_CLK_PIN_SEL == 4)
#define SD_CLK_PIN_CFG      (PIN_B26 | PIN_FUNC4 | PIN_DRI1 | PIN_PULL_UP100K)
#define SD_CLK_PIN_NUM      (PIN_B26)
#define SD_CLK_FUNC         (PIN_FUNC4)
#endif

#define SD_DETECT_PIN_NUM   (SD_CLK_PIN_NUM)

#if (SD_CMD_PIN_SEL == 0)
#define SD_CMD_PIN_CFG      (PIN_B2 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (SD_CMD_PIN_SEL == 1)
#define SD_CMD_PIN_CFG      (PIN_B4 | PIN_FUNC6 | PIN_DRI2 | PIN_PULL_UP2K)
#elif (SD_CMD_PIN_SEL == 2)
#define SD_CMD_PIN_CFG      (PIN_A16 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (SD_CMD_PIN_SEL == 3)
#define SD_CMD_PIN_CFG      (PIN_B14 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (SD_CMD_PIN_SEL == 4)
#define SD_CMD_PIN_CFG      (PIN_B27 | PIN_FUNC4 | PIN_DRI1 | PIN_PULL_UP2K)
#endif

#endif

void app_device_init(void);
#endif