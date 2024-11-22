#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "sd.h"

#if 0//TURNING_SPARK_TEMP
#include "usb.h"
#endif

enum device_id_e {
    DEV_SDCARD = 0,
    DEV_UDISK,
    DEV_PC,
    DEV_NONE,
};

enum device_sta_e {
    DEV_OFFLINE         = 0,    //offline
    DEV_UNINITIALIZED,          //have not initialized
    DEV_FAIL,                   //have initalized, but fail
    DEV_FS_FAIL,                //have initialized, and ok, but fat system is fail
    DEV_OK,                     //have initialized, and ok, have valid files
};

typedef struct {
    uint8_t cur_dev;
    uint8_t mounted;
    uint8_t startup;

    enum device_sta_e sdcard_sta;
    enum device_sta_e udisk_sta;
    enum device_sta_e pc_sta;
    bool line_sta;
} device_t;



void device_manager_init(device_t *dev_t, bool detect_start, void (*detect_callback)(void));
void device_detect(void* xTimer);
int device_initialize(uint32_t dev_num);
bool device_is_initialized(uint32_t dev_num);
bool device_is_online(uint32_t dev_num);
uint32_t device_get_sta(uint32_t dev_num);
void device_set_sta(uint32_t dev_num, uint32_t dev_sta);
uint32_t device_get_num(void);
uint32_t device_get_next_num(void);
void device_set_num(uint32_t dev_num);
int device_try(uint32_t dev_num);

void device_register_try_callback(  int (*device_try_cb_t)(uint32_t dev_num));

void device_sdcard_sta_set(uint32_t sta);
uint32_t device_sdcard_sta_get(void);
void device_udisk_sta_set(uint32_t sta);
uint32_t device_udisk_sta_get(void);
uint32_t device_cur_dev_get(void);
void device_cur_dev_set(uint8_t cur_dev);

void device_startup_set(uint8_t startup);
uint8_t device_startup_get(void);

uint8_t device_mouted_get(void);
void device_mouted_set(uint8_t mounted);

void device_pc_sta_set(uint32_t sta);
uint32_t device_pc_sta_get(void);

void device_linein_sta_set(uint32_t sta);
uint32_t device_linein_sta_get(void);

void device_detect_timer_start();
void device_detect_timer_stop();

#endif

