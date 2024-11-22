#ifndef _RF_H_
#define _RF_H_

uint8_t freq_drift_cal(int32_t freq_drift);
uint8_t freq_drift_cal_xiaoniu(int32_t freq_drift);
uint8_t freq_drift_value_get(void);
void rfdss_freq_offset_set(uint8_t fre_value);

#endif
