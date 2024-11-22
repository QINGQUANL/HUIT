#define LOG_LEV 5
#include "sys_inc.h"
#include "user_config.h"
#include "sensor.h"
#include "bt.h"
//#include "bt_tws.h"
#include "tws_profile.h"
#include "bt_host_api.h"
#include "audio_service.h"



void sensor_init(void)
{
#if SENSOR_EN

#if SENSOR_G_EN
    sensor_g_init();
#endif

#if SENSOR_PROXIMITY_EN
    sensor_proximity_init();
#endif

#if SENSOR_FORCE_EN
    sensor_force_init();
#endif

#if SENSOR_MOTION_EN
    sensor_motion_init();
#endif
#endif
}

void sensor_pre_init(void)
{
#if SENSOR_EN
#if (TWI1_EN || TWI2_EN)
     logi("init twi");
     twi_init();
#endif

#if (SENSOR_PROXIMITY_EN && (SENSOR_PROXIMITY_DEVICE_SEL == 2 || SENSOR_PROXIMITY_DEVICE_SEL == 4))
    sensor_proximity_pre_init();
#endif
#endif
}

void sensor_deinit(void)
{
#if SENSOR_EN
#if SENSOR_G_EN
    sensor_g_deinit();
#endif

#if SENSOR_PROXIMITY_EN
    sensor_proximity_deinit();
#endif

#if SENSOR_FORCE_EN
    sensor_force_deinit();
#endif

#if SENSOR_MOTION_EN
    sensor_motion_deinit();
#endif

#if (TWI1_EN || TWI2_EN)
    twi_deinit();
#endif
#endif
}



#if SENSOR_TEST_EN
AT_SRAM(.sensor)
void sensor_proximity_test(void)
{
#if SENSOR_PROXIMITY_EN
    static uint32_t sensor_p_sta = 0;

    if (sensor_p_sta) {
        sensor_p_sta = 0;
        event_put(SENSOR_EVENT_P_LT, NULL, 0);
    } else {
        sensor_p_sta = 1;
        event_put(SENSOR_EVENT_P_HT, NULL, 0);
    }
#endif
}

AT_SRAM(.sensor)
void sensor_g_test(void)
{
#if SENSOR_G_EN
    event_put(SENSOR_EVENT_G_DOUBLE_TAP, NULL, 0);
#endif
}
#endif
