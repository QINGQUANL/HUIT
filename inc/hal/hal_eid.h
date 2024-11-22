#ifndef _HAL_EID_LIB_H_
#define _HAL_EID_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif


enum eid_key_num_e {
    EID_KEY0 = 0,
    EID_KEY1,
    EID_KEY2,
    EID_KEY3,
};

enum eid_id_e {
    EID_CHIP_ID = 0,
    EID_ENCRYPT_KEY,
    EID_DCXO_TRIM,
    EID_RFTX_PAPOWER_CTUNE,
    EID_RFRX_LNA_CW,
    EID_RF_BIAS_RES_TRIM,
    EID_RFRX_LDO_TRIM,
    EID_RFTX_MAX_POWER,
    EID_RFTX_POWER_OFFSET,
    EID_RFRX_POWER_OFFSET,

    EID_FT_FLAG,
    EID_CP_FLAG,
    EID_CORE_LDO_TRIM,
};


uint32_t HAL_eid_key_get(uint32_t key_num);
void HAL_eid_read_protect(uint32_t key_num);
uint32_t HAL_eid_get_encryptkey(void);
uint32_t HAL_eid_get_pmu_vref_fine_tune(void);
uint32_t HAL_eid_get_pmu_iref_fine_tune(void);
bool HAL_eid_get_pmu_core_regu(void);
uint32_t HAL_eid_get_hp_select(void);
uint32_t HAL_eid_get_dac_cali(uint8_t ch_idx);
uint32_t HAL_eid_get_chipid(void);
uint32_t HAL_eid_get(uint32_t eid_id);
void HAL_eid_module_init(void);
void HAL_audio_hw_control_init(void);


#ifdef __cplusplus
}
#endif

#endif

