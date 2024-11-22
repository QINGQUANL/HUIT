#define LOG_LEV 4

#define SLTK_LOG_TAG "musice"
#include "sys_inc.h"
#include "user_config.h"
#include "hal.h"
#include "music.h"
#include "mode_manager.h"

#include "middleware.h"
#include "karaoke_framework.h"
#include "audio_service.h"
#include "music.h"
#include "disp.h"
#include "bt.h"
#include "app_pmu.h"
#include "sltk_audio.h"
#include "audio_player.h"
void playback_vol_set(float vol_db);

FATFS musicFs;

void play_music()
{
    f_unmount("0:");
    xfs_fscache_sd_init();
    xfs_cmd(XFS_CMD_FSCACHE_ENABLE, 0);
    xfs_cmd(XFS_CMD_TIMEOUT_SET, 5000);     //uint: ms
    f_mount(&musicFs, "0:", 1);

    playback_vol_set(-10);

    source_info_t info;
    memset(&info, 0, sizeof(info));

    info.type = SLTK_STYPE_FATFS;
    info.dev_type = PLAYER_DEV_SDCARD;//(DEV_UDISK == device_get_num()) ? PLAYER_DEV_UDISK : PLAYER_DEV_SDCARD;
    info.num = 0;
    info.dir = 0;
    strcpy(info.name, "chengdu.mp3");
    
    info.seek_time = 0;//rec_context->breakpoint_time;
    rec_context->breakpoint_time = 0;
    player_loop_play(RECORD_PLAY_LOOP);
    player_open(&info);
}

void mode_music_app_process(uint32_t event)
{
#if (WATCHDOG_EN && RTC_ALARM0_EN)
    HAL_watchdog_refresh(NULL);
#endif

    switch(event) {

#if MODE_KEY_ONOFF
    case KEY_EVENT_SU | KEY_MODE:
        logi("-->>mode key");
        event_put(PM_CODE_EVENT | PM_POWERON, NULL, 0);
        break;
#endif
#if SD_EN
    case DEVICE_EVENT_SD_IN:
		logi("sd in +++++ ");
		//rec_context->new_dev = DEV_SDCARD;

        if (device_initialize(DEV_SDCARD) < 0) {
            logd("init sd (%d) fail", DEV_SDCARD);
            device_set_sta(DEV_SDCARD, DEV_FAIL);
            break;
        }

        play_music();
        //notify :  do not change the SD io state here when usb cardreader is running
		break;
   case DEVICE_EVENT_SD_OUT:
    
        break;
#endif
    default:
        break;
    }
}


void mode_music_enter(void)
{
    if (NULL == rec_context) {
        rec_context = libc_calloc(sizeof(*rec_context));
        if (NULL == rec_context) {
            loge("calloc rec_context fail");
        }
    }
    
    logi("mode_music_enter");

   // bt_autotest_chargemode_do();
    music_register_event_handler(mode_music_app_process);
}


void mode_music_exit(void)
{
    logi("mode charge exit");
}

void mode_music(void)
{
    mode_music_enter();
    mode_music_loop();
    mode_music_exit();
}
