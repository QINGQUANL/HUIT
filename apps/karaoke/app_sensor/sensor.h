#ifndef _SENSOR_H_
#define _SENSOR_H_


#include "twi.h"
#include "gpio.h"
#include "event.h"
#include "app_pmu.h"
#include "sensor_g.h"
#include "sensor_proximity.h"
#include "sensor_bone_vibrate.h"
#include "sensor_force.h"
#include "sensor_motion.h"

#define SENSOR_TEST_EN      0


enum sensor_p_sta_e {
    SENSOR_P_SINGLE_OUT = 0x0,
    SENSOR_P_SINGLE_IN,

    SENSOR_P_DOUBLE_2OUT,
    SENSOR_P_DOUBLE_1IN_1OUT,
    SENSOR_P_DOUBLE_2IN,
};

void sensor_init(void);
void sensor_deinit(void);
void sensor_proximity_test(void);
void sensor_g_test(void);
void sensor_pre_init(void);


#endif
