#ifndef _MODE_PMU_OBSERVER_H_
#define _MODE_PMU_OBSERVER_H_



typedef enum _app_pmu_cmd_id {
    APP_PMU_CMD_START = 0,
    APP_PMU_CMD_TASK_DEINIT,
    APP_PMU_CMD_VBUS_IS_IN,
    APP_PMU_CMD_BAT_LEVEL_GET,
    APP_PMU_CMD_MAX,
}app_pmu_cmd;

typedef uint32_t (*app_pmu_cb)(app_pmu_cmd cmd, uint32_t value);
void app_pmu_resiger_callback(app_pmu_cb cb);
uint32_t app_pmu_call_by_id(app_pmu_cmd cmd_id, uint32_t val);

#define APP_PMU_CMD_VAILD(id) ( (id) >= APP_PMU_CMD_START && (id) <= APP_PMU_CMD_MAX )
#endif