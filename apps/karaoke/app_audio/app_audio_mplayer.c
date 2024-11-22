#include "sys_inc.h"
#include "audio_service.h"
#include "audio_player.h"
#include "local_play_service.h"
#include "local_play_manager.h"
#include "mode_manager.h"
#include "xfs.h"
#include "pms.h"
#include "hal_rtc.h"
#include "hal_timestamp.h"
#include "info_mem.h"
#include "music.h"
#include "user_config.h"
#if (DISP_EN && LED_DIODE_EN)
#include "disp.h"
#endif

#if (RECORD_MIC_FUNC || RECORD_LINEIN_FUNC || RECORD_FM_FUNC)
/* Play all dir, except "record" */
const dir_list_item_t dir_list0[] = {
    {.type = DIR_TYPE_BLACKLIST, .name.ascii = RECORD_FILE_DIR},  /* 搜索排除 RECORD_FILE_DIR 文件夹 */
    {.type = DIR_TYPE_NONE,} /* NOTE:最后一项必须是DIR_TYPE_NONE */
};
#endif

#if 0
//"儿歌"的unicode; '儿' unicode是0x513F， '歌'的unicode是0x6B4C, '\0'的unicode是0x0000
const uint16_t song_unicode[] = {0x513F, 0x6B4C, 0x0000};
// "故事"的unicode; '故' unicode是0x6545， '事'的unicode是0x4E8B, '\0'的unicode是0x0000
const uint16_t story_unicode[] = {0x6545, 0x4E8B, 0x0000};
/* only play song dir */
const dir_list_item_t dir_song[] = {
    {.type = DIR_TYPE_UNICODE_WHITELIST, .name.unicode = song_unicode}, /* 只搜索 "儿歌" 文件夹 */
    {.type = DIR_TYPE_NONE,} /* NOTE:最后一项必须是DIR_TYPE_NONE */
};

/* only play story dir */
const dir_list_item_t dir_story[] = {
    {.type = DIR_TYPE_UNICODE_WHITELIST, .name.unicode = story_unicode}, /* 只搜索 "故事" 文件夹 */
    {.type = DIR_TYPE_NONE,} /* NOTE:最后一项必须是DIR_TYPE_NONE */
};

/* only play dir0 dir */
const dir_list_item_t dir_list3[] = {
    {.type = DIR_TYPE_WHITELIST, .name.ascii = "dir0"}, /* 只搜索 "dir0" 文件夹 */
    {.type = DIR_TYPE_NONE,} /* NOTE:最后一项必须是DIR_TYPE_NONE */
};
#endif

/*
 * 文件后缀名白名单；只有白名单文件可以被检索
 */
const char *file_suffix_wl[] = {
#if MUSIC_WAV
    ".wav",
#endif

#if MUSIC_MP3
    ".mp3",
    ".mp2",
#endif

#if MUSIC_SBC
    ".sbc",
#endif

#if (MUSIC_OGG/* && (0 == AUDIO_PLAYBACK_VSS) */)
    ".ogg",
#endif
#if MUSIC_WMA
    ".wma",
#endif

#if MUSIC_APE
    ".ape",
#endif

#if (MUSIC_AAC/* && (0 == AUDIO_PLAYBACK_VSS) */)
    ".aac",
    ".m4a",
#endif

#if (MUSIC_FLAC/* && (0 == AUDIO_PLAYBACK_VSS) */)
    ".flac",
#endif
    NULL
};

/* 播放事件监听器
 * 此函数被audio库解码线程调用，
 * @param player: 播放器句柄
 * @param event: 播放事件， 参考@player_event_t
 */
uint32_t app_audio_mplayer_event_listener(void* cookie, uint32_t who, uint32_t cmd, uint32_t val, uint32_t rev)
{
    uint32_t type;

    if(LOCAL_PLAY_MANAGER_CMD_REQ_APP_HANDLE_DEV_OFFLINE == cmd) {
#if MUSIC_DEV_OFFLINE_SWITCH_MODE
        mode_switch_auto();
#endif
    }
    else if(LOCAL_PLAY_MANAGER_CMD_REQ_APP_FILE_SCAN == cmd) {
        *((int32_t*)rev) = file_scan(val);
    }
    else if(LOCAL_PLAY_MANAGER_CMD_REQ_APP_PLAY_INFO_WRITE_WHEN_PWROFF == cmd) {
#if (MUSIC_INFO_SAVE_MODE == 2)
        uint32_t file_num = ((uint32_t*)val)[0];
        uint32_t pts_ms   = ((uint32_t*)val)[1];
        info_set(INFO_SDCARD_NUM, (void *)&file_num, 4);
        info_set(INFO_SDCARD_TIME, (void *)&pts_ms, 4);
        /*这里只更新buf，在mode_poweroff中统一写flash*/
#endif
    }
    else if(LOCAL_PLAY_MANAGER_CMD_REQ_APP_PLAY_TIME_READ == cmd) {
#if (MUSIC_INFO_SAVE_MODE == 0)
        __maybe_unused uint32_t dev_num = device_get_num();
   //     *((uint32_t*)rev) = HAL_rtc_memory_read(INFO_MUSIC_PTIME + dev_num);FIX ME LCJ
#endif
    }
    else if(LOCAL_PLAY_MANAGER_CMD_REQ_APP_PLAY_TIME_WRITE == cmd) {
#if (MUSIC_INFO_SAVE_MODE == 0)
       __maybe_unused uint32_t dev_num;
        dev_num = device_get_num();
    //    HAL_rtc_memory_write(INFO_MUSIC_PTIME + dev_num, val);FIX ME LCJ
#endif
    }
    else if(LOCAL_PLAY_MANAGER_CMD_REQ_APP_PLAY_NUM_WRITE == cmd) {
#if (MUSIC_INFO_SAVE_MODE == 0 || MUSIC_INFO_SAVE_MODE == 1)

        //uint32_t dev_num;
        //dev_num = device_get_num();
        //ddlog("p num write:%d,%d", dev_num, val);

#if (MUSIC_INFO_SAVE_MODE == 0)
   //     HAL_rtc_memory_write(INFO_MUSIC_FNUM + dev_num, val);FIX ME LCJ
#elif (MUSIC_INFO_SAVE_MODE == 1)
        info_set(INFO_SDCARD_NUM + dev_num*4, (void *)&val, 4);
        info_write_page();
#endif
#endif
    }
    else if(LOCAL_PLAY_MANAGER_CMD_REQ_APP_PLAY_NUM_READ == cmd) {
#if (MUSIC_INFO_SAVE_MODE == 0 || MUSIC_INFO_SAVE_MODE == 1)
#if 0//LCJ
       __maybe_unused uint32_t dev_num;
        uint32_t checksum_cur;
        uint32_t checksum_last;
        xfs_cmd(XFS_CMD_GET_FILES_CHECKSUM, (uint32_t)&checksum_cur);
        dev_num = device_get_num();
#if (MUSIC_INFO_SAVE_MODE == 0)
        *((uint32_t*)rev) = HAL_rtc_memory_read(INFO_MUSIC_FNUM + dev_num);FIX ME LCJ
        checksum_last = HAL_rtc_memory_read(INFO_FILE_CHECKSUM + dev_num);FIX ME LCJ
#elif (MUSIC_INFO_SAVE_MODE == 1)
//        info_get(INFO_SDCARD_NUM + dev_num*4, (void *)rev, 4);FIX ME LCJ
//        info_get(INFO_SDCARD_CHECKSUM + dev_num*4, (void *)&checksum_last, 4);FIX ME LCJ
#endif
        logi("files checksum:cur%#x,last%#x", checksum_cur, checksum_last);
        if (checksum_cur != checksum_last) {
            local_play_manager_breakpoint_info_invalid();
#if (MUSIC_INFO_SAVE_MODE == 0)
      //      HAL_rtc_memory_write(INFO_FILE_CHECKSUM + dev_num, checksum_cur);FIX ME LCJ
#elif (MUSIC_INFO_SAVE_MODE == 1)
            info_set(INFO_SDCARD_CHECKSUM + dev_num*4, (void *)&checksum_cur, 4);FIX ME LCJ
#endif
        } else {
            local_play_manager_breakpoint_info_standby();
        }
#endif
#endif
    }
    else
#if USB_DISK_EN
    if(LOCAL_PLAY_MANAGER_CMD_REQ_APP_RESET_AND_SET_USB_STU == cmd) {
        usb_host_deinit();
        usb_detect_set(USB_DETECT);
    }
    else
#endif
#if SD_EN
    if(LOCAL_PLAY_MANAGER_CMD_REQ_APP_SET_SD_STU == cmd) {
        sd_detect_set(val);
    }
    else if(LOCAL_PLAY_MANAGER_CMD_REQ_APP_STOP_SD == cmd) {
        HAL_sd_stop();
    }
    else
#endif
#if (EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
    if(LOCAL_PLAY_MANAGER_CMD_REQ_APP_CHECKING_PC_CONNECT_STU == cmd) {
        if((val == DEV_UDISK) && (device_get_sta(DEV_PC) != DEV_OFFLINE)) {
            *((int32_t*)rev) = 1;
        }
    }
    else
#endif
#if MUSIC_DEVICE_SWITCH
    if(LOCAL_PLAY_MANAGER_CMD_REQ_APP_CHECKING_NEXT_FILE == cmd) {
        *((int*)rev) = local_play_manager_checkout_next_file();
    }
    else
#endif
#if (DISP_EN && LED_DIODE_EN)
    if(LOCAL_PLAY_MANAGER_CMD_REQ_APP_DISP_STU == cmd ||
            LOCAL_PLAY_MANAGER_CMD_REQ_APP_DISP_MEMU == cmd ||
            LOCAL_PLAY_MANAGER_CMD_REQ_APP_DISP_MEMU_SET == cmd) {
        if(LOCAL_PLAY_MANAGER_CMD_REQ_APP_DISP_MEMU_SET == cmd) {
            disp_menu_set(val);
        }
        else if(LOCAL_PLAY_MANAGER_CMD_REQ_APP_DISP_STU == cmd) {
        }
        else
        {
            local_play_manager_disp_menu_ack *ack = (local_play_manager_disp_menu_ack*)val;
            disp_menu(ack->menu_type, ack->val0, ack->va11);
        }
    }
    else
#endif
    if(LOCAL_PLAY_MANAGER_CMD_REQ_APP_HANDLE_EVENT == cmd) {
        uint32_t event = val;
        if(AUDIO_NOTIFY_MUSIC_MODE_EVENT_PROCESSOR == who) {
#if MODE_MUSIC_EN
            switch(event) {
                case PLAYER_EVENT_END_OF_FILE:
                case PLAYER_EVENT_ERR:
                    //自动播放上(下）一首
                    if (local_play_manager_auto_play_file(0) == 0) {

                    }
                    break;
            }
#endif
        }
        else if(AUDIO_NOTIFY_AUDIO_PLAYER == who) {
            if (PLAYER_EVENT_NONE == event) {
                return -1;
            }

            switch (event) {
            case PLAYER_EVENT_ERR:
                type = player_get_error_type();
                logi("play error (type=%d)", type);
                switch (type) {
                case APERR_FORMAT_PROBE:
                case APERR_CREATE_PARSER:
                case APERR_OPEN_INVALID_PARAMS:
                case APERR_CREATE_DEC:
                case APERR_OPEN_DEC:
                case APERR_OPEN_TRACK:
#if MODE_MUSIC_EN
                    local_play_manager_init_err_handling();
#endif
                    break;

                case APERR_FRAME_DEC:
#if MODE_MUSIC_EN
                    local_play_manager_dec_err_handling();
#endif
                    break;

                default:
                    break;
                }
                event_force_put(event, NULL, 0); /* 发送事件给app线程 */
                break;

            case PLAYER_EVENT_OPEN_COMPLETED:
#if MODE_MUSIC_EN
                local_play_manager_musicmode_open_finish_handling();
#elif RECORD_FUNC_EN
                local_play_manager_recordmode_open_finish_handling();
#endif
                break;

            case PLAYER_EVENT_END_OF_FILE:
                event_force_put(event, NULL, 0); /* 发送事件给app线程 */
                break;

            case PLAYER_EVENT_PAUSE_COMPLETED:
#if SD_EN
                if (DEV_SDCARD == device_get_num()) {
                    while(disk_is_lock()) {
                        if (RES_OK != sd_get_status()) {
                            break;
                        }
                        HAL_mdelay(1);
                    }
                    sd_detect_set(SD_DETACH_ENABLE);
                }
                /* 由于在DEC_TASK线程中有使用sd_detect_set，以上内容不可以在app线程中处理 */
#endif
                event_force_put(event, NULL, 0); /* 发送事件给app线程 */
                break;

            case PLAYER_EVENT_STOP_COMPLETED:
#if MODE_MUSIC_EN
                local_play_manager_stop_finish_handling();
#endif
                break;
            default:
                break;
            }
        }
    }
    return 0;
}

/* 切换模式时，播放器处理函数；
 * 此函数完成切换模式时播放器的清除或者初始化工作；
 * @param mode: 当前模式；
 * @param enter: true:表示进入该模式； false:表示退出该模式；
 */
void app_audio_mplayer_mode_switch_proc(uint8_t mode, bool enter)
{
    switch (mode) {
#if MODE_MUSIC_EN
    case MODE_MUSIC:
        local_play_manager_musicmode_swicth_proc(enter);
        break;
#endif

#if MODE_BT_EN || BT_DUT_EN
    case MODE_BT:
        local_play_manager_btmode_swicth_proc(enter);
        break;
#endif

#if MODE_USBDEV_EN && USB_AUDIO_EN
    //case MODE_USBDEV: removed by seven
    //    local_play_manager_usbamode_swicth_proc(enter);
    //    break;
#endif

#if MODE_LINEIN_EN
    case MODE_LINEIN:
        local_play_manager_lineinmode_swicth_proc(enter);
        break;
#endif

#if MODE_SPDIF_EN
    case MODE_SPDIF:
        local_play_manager_spdifmode_switch_proc(enter);
        break;
#endif

    default:
        break;
    }
}

void app_audio_mplayer_scene_save(void)
{
    switch (mode_sta_get()) {
#if MODE_MUSIC_EN
    case MODE_MUSIC:
        local_play_manager_musicmode_scene_snapshot();
        break;
#endif

    default:
        break;
    }
}

void app_audio_mplayer_recovery(void)
{
    switch (mode_sta_get()) {
#if MODE_MUSIC_EN
    case MODE_MUSIC:
        local_play_manager_musicmode_scene_recovery();
        break;
#endif
    default:
        break;
    }
}

int app_audio_mplayer_init()
{
    as_localplay_service_init(NULL);
    /* 设置文件后缀名白名单,仅白名单的文件被检索 */
     xfs_cmd(XFS_CMD_CONFIG_FILE_SUFFIX, (uint32_t)file_suffix_wl);
    /* 设置文件夹黑白名单
     * NOTE：如果设置了白名单，则仅白名单文件夹被检索；
     *       如果设置了黑名单， 则除黑名单外的所有文件夹都被检索；
     *       如果同时设置白名单和黑名单，则黑名单设置无效。
     */
#if 0
    xfs_cmd(XFS_CMD_CONFIG_DIR_LIST, (uint32_t)dir_song);  /* 只搜索"儿歌"文件夹 */
    xfs_cmd(XFS_CMD_CONFIG_DIR_LIST, (uint32_t)dir_story); /* 只搜索"故事"文件夹 */
    xfs_cmd(XFS_CMD_CONFIG_DIR_LIST, (uint32_t)dir_list3); /* 只搜索"dir0"文件夹 */
#endif
#if (RECORD_MIC_FUNC || RECORD_LINEIN_FUNC || RECORD_FM_FUNC)
#if MUSIC_REC_FOLDER_IGNORE
    xfs_cmd(XFS_CMD_CONFIG_DIR_LIST, (uint32_t)dir_list0);/* 不播放录音文件夹 */
#endif
#endif
    local_play_manager_init_para lpm_init = {
        .root = "",
        .listener = app_audio_mplayer_event_listener,
    };

    local_play_manager_init(&lpm_init);
#if MUSIC_MUTE_FOR_FAST
    local_play_manager_mute_for_fastskip();
#endif
    local_play_manager_set_play_direction(PLAY_NEXT); /* 配置播放完成或者播放出错后自动播放下一首 */
    return 0;
}
