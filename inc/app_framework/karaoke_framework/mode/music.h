#ifndef _MODE_MUSIC_H_
#define _MODE_MUSIC_H_

#include "oal.h"
#include "event.h"

extern OAL_THREAD track_handler;
extern OAL_THREAD karaoke_handler;
extern OAL_THREAD dec_handler;
extern OAL_THREAD bt_isr_bh_handler;
extern OAL_THREAD bta_handler;
extern OAL_THREAD btc_handler;
extern OAL_THREAD bth_handler;
extern OAL_THREAD main_task_handler;
extern OAL_THREAD linein_handler;

int file_scan(bool scan);
void music_event_process(sys_event_info_t *info);
void mode_music(void);
void mode_music_enter(void);
void mode_music_loop(void);
void mode_music_exit(void);
void music_register_event_handler(void (*handler)(uint32_t event));


#endif
