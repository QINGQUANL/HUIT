#define LOG_LEV 4
#include "sys_inc.h" // libsys.a
#include "oal.h" // liboal.a
#include "hal.h" //libhal.a
#include "system.h" //libmuddleware.a
#include "user_config.h"
#include "device.h"
#include "event.h"
#include "usb.h"
#include "hal.h"
#include "event.h"
#include "record.h"
#include "key.h"
#include "xfs.h"
#include "device.h"
#include "sd.h"
#include "info_mem.h"
#include "diskio.h"
#include "device.h"
#include "audio_recorder.h"
#include "audio_player.h"
#include "app_record.h"
#include "local_play_manager.h"
#include "pms.h"
#include "sleep.h"
#include "ff.h"
#include "disp.h"

bool is_usb_cardreader_device_attach();

extern int linein_play_vol;
extern int fm_play_vol;
extern scene_mode_t scene_mode;
#if SD_EN

extern struct native_player np;


#if (RECORD_MIC_FUNC || RECORD_LINEIN_FUNC || RECORD_FM_FUNC || RECORD_G24_FUNC)

record_context_t *rec_context;



/* 录音事件监听器；
 * 此函数被audio库编码线程调用；
 * @param recorder: 录音器句柄；
 * @param event: 录音事件， 参考enum ar_event_e；
 */
void recorder_event_listener(void *recorder, uint32_t event)
{
    event_force_put(event,0 , 0); /* 给APP线程发送事件 */
}

/* 判断文件是否是录音文件；
 * @param file_name: 文件名
 * @return: true:是录音文件； false：不是录音文件
 */
bool is_record_file(char *file_name)
{
    char *p, *fix;

    /* check prefix */
    p = file_name;
    fix = RECORD_FILE_PREFIX;
    for (; *fix != '\0'; fix++, p++) {
        if (*p != *fix) {
            return false;
        }
    }

    /* check file number */
    for (int i = 0; i < RECORD_FILE_COUNT_BITS; i++) {
        if (*p > '9' || *p < '0') {
            return false;
        }
        ++p;
    }

    /* check suffix */
    fix = RECORD_FILE_SUFFIX;
    for (; *fix != '\0'; fix++, p++) {
        if (*p != *fix) {
            return false;
        }
    }

    if (*p != '\0') {
        return false;
    }

    return true;
}

/* 开始录音；
 */
void start_record(void)
{
    logi("start_record %x", rec_context);
    bool is_have_device = false;
    if (SLTK_STYPE_FATFS == rec_context->streamtype) {
#if SD_EN
#if SD_DETECT_EN
    is_have_device = device_is_online(DEV_SDCARD);
#else
    is_have_device = true;
#endif
#endif

#if UDISK_EN
    is_have_device |= device_is_online(DEV_UDISK);
#endif
        if (!is_have_device) {
            loge("no rec dev");
            return;
        }
        prepare_record_device();
    }
    logi("stop_play_record_file ");
    stop_play_record_file();

    recorder_stream_params_t params;
    params.fs = RECORD_FS;
    params.ch = RECORD_CHANNELS;
    params.fmt = RECORD_FORMAT;
    params.truncate_time = RECORD_TRUNCATE_TIME_MS;
    logi("recorder_set_stream_params");
    recorder_set_stream_params(&params);
    rec_context->cur_num++;
    rec_context->cur_play_num = rec_context->cur_num;
    rec_context->max_play_num = rec_context->cur_num;
    rec_context->total_file++;
    logi("index_to_filename");
    index_to_filename(rec_context->cur_num, rec_context->file_name);
    recorder_open(NULL, rec_context->file_name);
    recorder_start();
    logd("start record: %s", rec_context->file_name);
    logi("total file:%d", rec_context->total_file);

    rec_context->status = REC_STATUS_RECORDING;
    np.paused = false;
}

/* 选择录音设备；
 * @param dev_num: 设备号
 */
int32_t select_record_device(uint32_t dev_num)
{
    int32_t ret;

    ret = device_try(dev_num);
    if (ret < 0) {
        if (DEV_SDCARD == dev_num) {
            sd_detect_set(1);
        }
        return -1;
    }

    if (device_mouted_get() == 0) {
        f_unmount("0:");
        xfs_fscache_sd_init();
        xfs_cmd(XFS_CMD_FSCACHE_ENABLE, 0);
        xfs_cmd(XFS_CMD_TIMEOUT_SET, 5000);     //uint: ms

        memset(&rec_context->fs, 0, sizeof(rec_context->fs));
        ret = f_mount(&rec_context->fs, "0:", 1);
        if (FR_OK != ret) {
            loge("f_mount err(%d)", ret);
            sd_detect_set(1);
            return -1;
        }
        device_mouted_set(1);

        rec_context->mount_dev = dev_num;
        rec_context->cur_num = 0;
        rec_context->status = REC_STATUS_IDLE;
        rec_context->file_scaned = false;

        sleep_delay_set();
        sleep_prevent_set(PS_APP_PROCESS);

        scan_record_file();

        if (rec_context->total_file == 0) {
            sd_detect_set(1);
        }

        event_clear(PLAYER_EVENT_ERR);

        sleep_delay_set();
        sleep_prevent_clear(PS_APP_PROCESS);
    }

    return 0;
}

//----------------------
/* 文件编号转换成文件名；
 * @param index: 文件编号
 * @return name: 文件名
 */
void index_to_filename(int32_t index, char *name)
{
    int32_t len;
    uint32_t num;
    char *p;

    strcpy(name, RECORD_FILE_DIR "/" RECORD_FILE_PREFIX);

    len = strlen(name);
    num = index;
    p = name + len + RECORD_FILE_COUNT_BITS;
    *p-- = '\0';
    for (int i = 0; i < RECORD_FILE_COUNT_BITS; i++) {
        *p-- = (num % 10) + '0';
        num = num / 10;
    }
    strcat(name, RECORD_FILE_SUFFIX);
}


/*
 * 扫描录音文件夹，获取录音文件数量和最大文件编号；
 */
void scan_record_file(void)
{
    FRESULT res;
    DIR dir;

    if (!rec_context->file_scaned) {
try_opendir:
        res = f_opendir(&dir, RECORD_FILE_DIR); /* 打开录音文件夹 */
        logv("opendir res:%d", res);

        /* make dir if no path */
        if (res != FR_OK) {
            if (FR_NO_PATH == res) {
                res = f_mkdir(RECORD_FILE_DIR); /* 创建录音文件夹 */
                logv("mkdir res:%d", res);
                if (res == FR_OK) {
                    goto try_opendir;
                }
            } else {
                //TODO
            }
        }

        FILINFO *pfno = libc_calloc(sizeof(FILINFO));
        if (NULL == pfno) {
            loge("calloc pfno fail");
        }

        int32_t num = 0;
        int32_t num_max = -1;
        uint32_t offset = strlen(RECORD_FILE_PREFIX);

        rec_context->total_file = 0;

        /* 遍历文件夹，获取录音文件数量和最大文件编号 */
        while (1) {
            res = f_readdir(&dir, pfno);
            if (res != FR_OK || pfno->fname[0] == 0) {
                break;
            }

            if (!(pfno->fattrib & AM_DIR)) {
                if (is_record_file(pfno->fname)) {
                    logv("fname:%s", pfno->fname);
                    int n = 1;
                    num = 0;
                    for (int i = 1; i <= RECORD_FILE_COUNT_BITS; i++) {
                        int index = offset + RECORD_FILE_COUNT_BITS - i;
                        num += (pfno->fname[index] - '0') * n;
                        n *= 10;
                    }

                    num_max = num_max > num ? num_max : num;

                    rec_context->total_file++;
                }
            }
        }

        logv("num_max:%d", num_max);
        rec_context->max_play_num = num_max;
        rec_context->cur_play_num = num_max;
        rec_context->cur_num = num_max;
        rec_context->file_fail_count = 0;
        logd("total file:%d", rec_context->total_file);

        libc_free(pfno);
        f_closedir(&dir);
        rec_context->file_scaned = true;
    }
}


/* 停止录音文件的播放；
 * @param rec_context: 录音上下文
 */
void stop_play_record_file(void)
{
    if (REC_STATUS_PLAYING == rec_context->status) {
        logv("stop play.");
        player_loop_play(0);
        player_close(0); //close player immediately

        rec_context->status = REC_STATUS_IDLE;
#if DISP_EN
        disp_menu(MENU_SCENE_MAIN, scene_mode.sta, 0);
#endif
    }
}

void record_mode_scene_save(void)
{
    if (REC_STATUS_PLAYING == rec_context->status) {
        rec_context->breakpoint_time = player_get_current_play_time();
        stop_play_record_file();
    } else if (REC_STATUS_RECORDING == rec_context->status) {
        stop_record();
    }
}

void record_mode_scene_recovery(void)
{
    start_play_record_file();
}

/* 录音文件播放/暂停；
 */
int play_or_pause_record(void)
{
    //if (COMMON_TONE_STATUS_PLAYING == np.common_tone_status) {
    //    return PLAYER_RET_SUCCESS;
    //}

    if (!np.paused) {
        if (AP_STATUS_PLAYING == player_get_status()) {
            logd("play -> pause");
            player_pause(0);
            np.paused = true;
        }
    } else {
        logd("pause -> play");
        player_resume(0);
        np.paused = false;
    }

    return PLAYER_RET_SUCCESS;
}

/* 切换录音设备；
 * @param auto_play: true:自动播放录音文件； flase:不自动播放录音文件
 */
void switch_record_device(bool auto_play)
{
#if 0
    int ret;
    uint32_t dev;
    uint32_t device_1st;
    uint32_t device_2nd;

#if RECORD_NEW_DEVICE_FIRST
    /* new device first */
    dev = rec_context->new_dev;
    if (DEV_UDISK == dev) {
        device_1st = DEV_UDISK;
        device_2nd = DEV_SDCARD;
    } else if (DEV_SDCARD == dev) {
        device_1st = DEV_SDCARD;
        device_2nd = DEV_UDISK;
    } else {
        device_1st = DEV_UDISK;
        device_2nd = DEV_SDCARD;
    }
#else
    /* old device first */
    dev = rec_context->new_dev;
    if (DEV_UDISK == dev) {
        if (device_is_online(DEV_SDCARD)) {
            device_1st = DEV_SDCARD;
            device_2nd = DEV_UDISK;
        } else {
            device_1st = DEV_UDISK;
            device_2nd = DEV_SDCARD;
        }
    } else if (DEV_SDCARD == dev) {
        if (device_is_online(DEV_UDISK)) {
            device_1st = DEV_UDISK;
            device_2nd = DEV_SDCARD;
        } else {
            device_1st = DEV_SDCARD;
            device_2nd = DEV_UDISK;
        }
    } else {
        device_1st = DEV_UDISK;
        device_2nd = DEV_SDCARD;
    }
#endif

    if (DEV_OK != device_get_sta(device_1st) || rec_context->mount_dev != device_1st) {
        stop_record();
        stop_play_record_file();
        ret = select_record_device(device_1st);
        if (ret < 0) {
            ret = select_record_device(device_2nd);
            if (ret < 0) {
                device_set_num(DEV_NONE);
            } else {
                if (auto_play) {
                    start_play_record_file();
                }
            }
        } else {
            if (auto_play) {
                start_play_record_file();
            }
        }
    }
#endif
}
#endif

#if 0//(RECORD_LINEIN_FUNC || RECORD_FM_FUNC)

//linein 录音初始化
void linein_record_init(uint32_t rec_func)
{
    int32_t ret;
    if (NULL == rec_context) {
        rec_context = libc_calloc(sizeof(*rec_context));
        if (NULL == rec_context) {
            loge("calloc rec_context fail");
            return;
        }
    }

    recorder_init_params_t params;
#if (0 == RECORD_STREAM_TYPE)
    rec_context->streamtype = SLTK_STYPE_FATFS;
#elif (1 == RECORD_STREAM_TYPE)
    rec_context->streamtype = SLTK_STYPE_FLASH_RECORD;
#endif

    params.streamtype = rec_context->streamtype;
    switch (rec_func) {
    case 1: //录linein
        params.mode_num = 1;
        break;

    case 2: //录mic
        params.mode_num = 0;
        break;

    case 3: //录mic+linein
    default:
        params.mode_num = 2;
        break;
    }
    create_recorder(&params);
    recorder_set_event_listener(recorder_event_listener);

    if (SLTK_STYPE_FATFS == rec_context->streamtype) {
        //录到文件系统
        ret = select_record_device(DEV_UDISK);
        if (ret < 0) {
        #if SD_EN
            ret = select_record_device(DEV_SDCARD);
            if (ret < 0) {
                device_set_num(DEV_NONE);
                logi("have no dev");
            } else {
                rec_context->new_dev = DEV_SDCARD;
                if (device_get_num() == DEV_SDCARD) {
                    sd_detect_set(1);
                }
            }
        #endif
        } else {
        #if SD_EN
            if (device_get_num() == DEV_SDCARD) {
                sd_detect_set(1);
            }
        #endif
        }
    } else if (SLTK_STYPE_FLASH_RECORD == rec_context->streamtype) {
        //录到flash
    }
}

//linein 录音退出
void linein_record_deinit(void)
{
    if (rec_context) {
        if (REC_STATUS_RECORDING == rec_context->status ||
            REC_STATUS_IDLE == rec_context->status) {
            stop_record();
        } else if (REC_STATUS_PLAYING == rec_context->status) {
            stop_play_record_file();
            audio_service_set_playback_volume(np.volume);
        }
    }

    destroy_recorder();

    if (SLTK_STYPE_FATFS == rec_context->streamtype) {
        f_unmount("0:");
        xfs_cmd(XFS_CMD_FSCACHE_DEINIT, 1);
    }

    if (rec_context) {
        libc_free(rec_context);
        rec_context = NULL;
    }
}

//更新linein录音(或者录音播放)的显示时间
void linein_record_update_disp_time(void)
{
    if (REC_STATUS_RECORDING == rec_context->status) {
        get_record_time();
        disp_menu(MENU_RECORD_ING, rec_context->rtime.min, rec_context->rtime.sec);
    } else if (REC_STATUS_PLAYING == rec_context->status) {
        local_play_manager_get_play_time();
        disp_menu(MENU_RECORD_PLAY, np.ptime.min, np.ptime.sec);
    }
}

//linein录音处理SD卡事件
void linein_record_proc_sd_event(uint8_t mode, uint32_t event)
{
#if SD_EN
    if (SLTK_STYPE_FATFS != rec_context->streamtype) {
        return;
    }

    switch (event) {
    case DEVICE_EVENT_SD_IN:
        logi("sd in");
        if (REC_STATUS_PLAYING == rec_context->status) {
            stop_play_record_file();     //stop play record file
            audio_service_set_playback_volume(np.volume);
            adda_init(mode);
        } else if (REC_STATUS_RECORDING == rec_context->status) {
            logd("stop rec");
            stop_record();
        }
        rec_context->new_dev = DEV_SDCARD;
        if (device_get_num() != DEV_SDCARD) {
            if (RECORD_NEW_DEVICE_FIRST) {
                switch_record_device(0);
                sd_detect_set(1);
            }
        }
        break;

    case DEVICE_EVENT_SD_OUT:
        if (REC_STATUS_PLAYING == rec_context->status && device_get_num() == DEV_SDCARD) {
            stop_play_record_file();     //stop play record file
            audio_service_set_playback_volume(np.volume);
            adda_init(mode);
        } else if (REC_STATUS_RECORDING == rec_context->status && device_get_num() == DEV_SDCARD) {
            logd("stop rec");
            stop_record();
        }

        rec_context->new_dev = DEV_UDISK;
        sd_detect_set(1);
        if (device_get_num() == DEV_SDCARD) {
            switch_record_device(0);
        }
        break;

    default:
        break;
    }
#endif //SD_EN
}

//linein录音处理U盘事件
void linein_record_proc_udisk_event(uint8_t mode, uint32_t event)
{
#if USB_DISK_EN
    if (SLTK_STYPE_FATFS != rec_context->streamtype) {
        return;
    }

    switch (event) {
    case DEVICE_EVENT_UDISK_IN:
        logi("udisk in");
        if (REC_STATUS_PLAYING == rec_context->status) {
            stop_play_record_file();     //stop play record file
            audio_service_set_playback_volume(np.volume);
            adda_init(mode);
        } else if (REC_STATUS_RECORDING == rec_context->status) {
            logd("stop rec");
            stop_record();
        }

        rec_context->new_dev = DEV_UDISK;
        if (device_get_num() != DEV_UDISK) {
            if (RECORD_NEW_DEVICE_FIRST) {
                switch_record_device(0);
            }
            sd_detect_set(1);
        }
        break;

    case DEVICE_EVENT_UDISK_OUT:
        logi("udisk out");
        if (REC_STATUS_PLAYING == rec_context->status  && device_get_num() == DEV_UDISK) {
            stop_play_record_file();     //stop play record file
            audio_service_set_playback_volume(np.volume);
            adda_init(mode);
        } else if (REC_STATUS_RECORDING == rec_context->status && device_get_num() == DEV_UDISK) {
            logd("stop rec");
            stop_record();
        }

        rec_context->new_dev = DEV_SDCARD;
        usb_host_deinit();
        usb_detect_set(USB_DETECT);
        if (device_get_num() == DEV_UDISK) {
            switch_record_device(0);
            sd_detect_set(1);
        }
        break;

    default:
        break;
    }
#endif //USB_DISK_EN
}

void linein_record_play_rec_file(uint32_t mode_id)
{
    stop_record();               //stop record
    adda_deinit(mode_id);
    /* 如果录linein,由于录的是音量处理前的数据，因此播放时保持原音量可听到相同音量大小的声音
     * 如果录的是linein+mic，由于录的是音量处理后的数据，因此播放需要设到100音量，否则声音大小会有衰减
     */
    if (MODE_LINEIN == mode_id) {
        if (RECORD_LINEIN_FUNC == 3) {
            linein_play_vol = 100;
            audio_service_set_playback_volume(linein_play_vol);
        }
    } else if (MODE_FM == mode_id) {
        if (RECORD_FM_FUNC == 3) {
            fm_play_vol = 100;
            audio_service_set_playback_volume(fm_play_vol);
        }
    }

    start_play_record_file();    //start play record file
}

void linein_stop_play_rec_file(uint8_t mode)
{
    stop_play_record_file();     //stop play record file
    audio_service_set_playback_volume(np.volume);
    adda_init(mode);
    sd_detect_set(1);
}

#endif


#if RECORD_FUNC_EN

void record_event_process(uint32_t event)
{
    switch (event) {
        case DEVICE_EVENT_SD_IN:
    		logi("sd in +++++ ");
    		rec_context->new_dev = DEV_SDCARD;
            //notify :  do not change the SD io state here when usb cardreader is running
		break;

        case DEVICE_EVENT_SD_OUT:
		logi("sd out");
		rec_context->new_dev = DEV_UDISK;
		sd_detect_set(1);
		if (device_get_num() == DEV_SDCARD) {
			switch_record_device(RECORD_AUTO_PLAY);
		}
		break;

	case DEVICE_EVENT_SD_FAIL:
		logi("record,sd fail.");
		rec_context->new_dev = DEV_UDISK;
		sd_detect_set(1);

		if (device_get_num() == DEV_SDCARD) {
			switch_record_device(RECORD_AUTO_PLAY);
		}
		break;

	case AR_EVENT_CLOSE:
		logd("close file");
		break;

	case AR_EVENT_ERROR:
		logd("record error event");
		stop_record();
		if (device_get_num() == DEV_SDCARD) {
			sd_detect_set(1);
		}
		break;

#if (0 == RECORD_PLAY_LOOP)
	case PLAYER_EVENT_END_OF_FILE:
		if (REC_RET_SUCCESS == play_next_record_file()) {
			disp_menu(MENU_NUM, rec_context->cur_play_num, 0);
		}
		else {
			sd_detect_set(1);
		}
		break;
#endif
    case PLAYER_EVENT_ERR:              //play err
		{
    		uint32_t type = player_get_error_type();
    		logw("play error (type=%d)", type);
    		if (APERR_FORMAT_PROBE == type ||
    			APERR_CREATE_PARSER == type ||
    			APERR_OPEN_INVALID_PARAMS == type ||
    			APERR_CREATE_DEC == type ||
    			APERR_OPEN_DEC == type ||
    			APERR_OPEN_TRACK == type) {
    			if (device_get_num() == DEV_SDCARD) {
    				if (DEV_OK == device_get_sta(DEV_SDCARD)) {
    					++(rec_context->file_fail_count);
    				}
    				else {
    					rec_context->file_fail_count = 0;
    				}
    			}

    			if (PLAY_NEXT == rec_context->play_direction) {
    				if (REC_RET_SUCCESS == play_next_record_file()) {
    					//disp_menu(MENU_NUM, rec_context->cur_play_num, 0);
    				}
    				else {
    					if (device_get_num() == DEV_SDCARD) {
    						sd_detect_set(1);
    					}
    				}
    			}
    			else {
    				if (REC_RET_SUCCESS == play_prev_record_file()) {
    					//disp_menu(MENU_NUM, rec_context->cur_play_num, 0);
    				}
    				else {
    					sd_detect_set(1);
    				}
                 }
		    }
        }
		break;

    case PLAYER_EVENT_PAUSE_COMPLETED:            //pause
		logd("pause");
		if (!np.paused) {
			break;
		}
		local_play_manager_get_play_time(NULL);
		//disp_menu(MENU_RECORD_PLAY, np.ptime.min, np.ptime.sec);
		break;

	case KEY_EVENT_SU | KEY_PREV:       //previous file
		logd("prev");
		rec_context->file_fail_count = 0;
		if (REC_RET_SUCCESS == play_prev_record_file()) {
			//disp_menu(MENU_NUM, rec_context->cur_play_num, 0);
		}
		else {
			if (device_get_num() == DEV_SDCARD) {
				sd_detect_set(1);
			}
		}
		break;

	case KEY_EVENT_SU | KEY_NEXT:       //next file
		logd("next");
		rec_context->file_fail_count = 0;
		if (REC_RET_SUCCESS == play_next_record_file()) {
			//disp_menu(MENU_NUM, rec_context->cur_play_num, 0);
		}
		else {
			if (device_get_num() == DEV_SDCARD) {
				sd_detect_set(1);
			}
		}
		break;

	case KEY_EVENT_L | KEY_PLAY:        //start rec
	case KEY_EVENT_L | KEY_PLAY_MODE:
#if USB_DEVICE_TYPE == USB_DEVICE_TYPE_CARDREADER   //读卡器正在工作，不响应录音事件
        if(is_usb_cardreader_device_attach()) {
            break;
        }
#endif
    
#if RECORD_MIC_FUNC
		logd("start rec");
		start_record();
#endif
		break;

	case KEY_EVENT_SU | KEY_PLAY:       //play rec
	case KEY_EVENT_SU | KEY_PLAY_MODE:
#if USB_DEVICE_TYPE == USB_DEVICE_TYPE_CARDREADER   //读卡器正在工作，不响应录音事件
        if(is_usb_cardreader_device_attach()) {
            break;
        }
#endif
#if RECORD_MIC_FUNC
		if (REC_STATUS_RECORDING == rec_context->status ||
			REC_STATUS_IDLE == rec_context->status) {
			stop_record();
            logi("stop end");
			//
		}
		
#endif
		break;
    case KEY_EVENT_SU | KEY_M:
        
        if (REC_STATUS_PLAYING == rec_context->status) {
#if 0                                   //play / stop
			logd("stop play");
			stop_play_record_file();
			if (DEV_SDCARD == device_get_num()) {
				sd_detect_set(1);
			}
#else                                   //play / pause
            logi("pause/ play record file");
			play_or_pause_record();
#endif
		} else {
		    logi("play record file");
		    start_play_record_file();
        } 
        break;
    case KEY_SW | KEY_EVENT_SU:
        logi("stop play");
        stop_play_record_file();
        break;

	case KEY_EVENT_L | KEY_PREV:        //delete rec
		logd("del rec");
		delete_record_file_by_num();
		if (device_get_num() == DEV_SDCARD) {
			sd_detect_set(1);
		}
		break;

	default:
		break;
    }
}

void record_share_buff_init();
void record_share_buff_deinit();

void record_enter(void)
{
#if (TONE_EN && TONE_RECORDING_MODE_EN)
    am_tone_play(TONE_RECORDING_MODE_EN, 1, 0, TONE_KEY_VOL, 0, 0);
#endif

#if RECORD_MIC_FUNC
    int32_t ret;

    if (NULL == rec_context) {
        rec_context = libc_calloc(sizeof(*rec_context));
        if (NULL == rec_context) {
            loge("calloc rec_context fail");
        }
    }

    recorder_init_params_t params;
#if (0 == RECORD_STREAM_TYPE)
    rec_context->streamtype = SLTK_STYPE_FATFS;
#elif (1 == RECORD_STREAM_TYPE)
    rec_context->streamtype = SLTK_STYPE_FLASH_RECORD;
#endif
    params.streamtype = rec_context->streamtype;
    params.mode_num = 0;
    params.record_data_get = record_data_get;
    create_recorder(&params);
    recorder_set_event_listener(recorder_event_listener);

    if (SLTK_STYPE_FATFS == rec_context->streamtype) {
        //play sdcard
        rec_context->mount_dev = DEV_NONE;
        rec_context->new_dev = DEV_SDCARD;
        ret = select_record_device(DEV_SDCARD);
        if (ret < 0) {
            device_set_num(DEV_NONE);
            logi("no dev");
            //mode_switch_auto();
        } else {
            #if RECORD_AUTO_PLAY
            if (REC_RET_ERR == start_play_record_file()) {
                if (device_get_num() == DEV_SDCARD) {
                    sd_detect_set(1);
                }
            }
            #else
            if (device_get_num() == DEV_SDCARD) {
                sd_detect_set(1);
            }
            #endif
        }
    } else {
        #if RECORD_AUTO_PLAY
        start_play_record_file();
        #endif
    }

    record_share_buff_init();
#endif
}

void record_exit(void)
{
    player_loop_play(0);
#if RECORD_MIC_FUNC
    if (rec_context) {
        if (REC_STATUS_RECORDING == rec_context->status ||
            REC_STATUS_IDLE == rec_context->status) {
            stop_record();
        } else if (REC_STATUS_PLAYING == rec_context->status) {
            stop_play_record_file();
        }
    }

    if (DEV_SDCARD == device_get_num()) {
        HAL_sd_stop();
    }

    destroy_recorder();
    f_unmount("0:");
    xfs_cmd(XFS_CMD_FSCACHE_DEINIT, 1);

    if (rec_context) {
        libc_free(rec_context);
        rec_context = NULL;
    }
#endif
    record_share_buff_deinit();

    logi("record exit.");
}

#endif


/*
 * 录音设备准备；
 */
void prepare_record_device(void)
{
    if (DEV_OK != device_get_sta(DEV_SDCARD)) {
        if (select_record_device(DEV_SDCARD) < 0) {
            device_set_num(DEV_NONE);
        }
    }
}

/* 停止录音；
 * @param rec_context: 录音上下文
 */
void stop_record(void)
{
    recorder_stop();
    recorder_close();

    if (AR_ERR_NO_STORE_FILE == recorder_get_error_type()) {
        rec_context->cur_num--;
        rec_context->cur_play_num = rec_context->cur_num;
        rec_context->max_play_num = rec_context->cur_num;
        rec_context->total_file--;
        recorder_clear_error_type();
    }

    if (REC_STATUS_RECORDING == rec_context->status) {
        rec_context->status = REC_STATUS_IDLE;
    }
#if SD_EN
    sd_detect_set(1);
#endif
}


/* 开始播放录音文件；
 * @param rec_context: 录音上下文
 */
int start_play_record_file(void)
{
    if (SLTK_STYPE_FATFS == rec_context->streamtype) {
        if (rec_context->total_file <= 0) {
            logi("err no file");
            return REC_RET_ERR;
        }

        if (rec_context->cur_play_num < 0) {
            logi("err cur play, %d", rec_context->cur_play_num);
            return REC_RET_ERR;
        }

        if (rec_context->file_fail_count >= rec_context->total_file) {
            logi("err file fail count,%d, %d", rec_context->file_fail_count,  rec_context->total_file);
            stop_play_record_file();
            return REC_RET_ERR;
        }

        if (SLTK_STYPE_FATFS == rec_context->streamtype) {
            prepare_record_device();
        }
    }

    np.paused = false;
    rec_context->play_direction = PLAY_NEXT;
    rec_context->status = REC_STATUS_PLAYING;
    stop_play_record_file();
    rec_context->status = REC_STATUS_PLAYING;
    index_to_filename(rec_context->cur_play_num, rec_context->play_file_name);
    event_clear(PLAYER_EVENT_ERR);

    source_info_t info;
    memset(&info, 0, sizeof(info));

    if (SLTK_STYPE_FATFS == rec_context->streamtype) {
        info.type = SLTK_STYPE_FATFS;
        info.dev_type = (DEV_UDISK == device_get_num()) ? PLAYER_DEV_UDISK : PLAYER_DEV_SDCARD;
        info.num = 0;
        strcpy(info.name, rec_context->play_file_name);
    } else {
        info.type = SLTK_STYPE_FLASH_RECORD;
        info.dev_type = 0;
        info.num = rec_context->cur_play_num;
    }
    info.seek_time = rec_context->breakpoint_time;
    rec_context->breakpoint_time = 0;
    player_loop_play(RECORD_PLAY_LOOP);
    player_open(&info);
    logd("play record file:%s", rec_context->play_file_name);

#if 0//DISP_EN
    disp.menu = MENU_SCENE_MAIN;
    local_play_manager_get_play_time(NULL);
    disp_menu(MENU_RECORD_PLAY, np.ptime.min, np.ptime.sec);
#endif

    return REC_RET_SUCCESS;
}



/* 播放下一个录音文件；
 * @param rec_context: 录音上下文
 */
int play_next_record_file(void)
{
    FRESULT res;
    XFILE *file;
    uint32_t try_count = rec_context->max_play_num;

    if (false == device_is_online(DEV_SDCARD)
        && false == device_is_online(DEV_UDISK)) {
        loge("no rec dev");
        return REC_RET_ERR;
    }

    if (rec_context->total_file <= 0) {
        return REC_RET_ERR;
    }

    if (rec_context->file_fail_count >= rec_context->total_file) {
        stop_play_record_file();
        return REC_RET_ERR;
    }

    sleep_delay_set();
    sleep_prevent_set(PS_APP_PROCESS);

    prepare_record_device();

    if (REC_STATUS_RECORDING != rec_context->status) {
        stop_play_record_file();
        file = libc_calloc(sizeof(*file));
next:
        if (rec_context->cur_play_num < rec_context->max_play_num) {
            rec_context->cur_play_num++;
        } else {
            rec_context->cur_play_num = 0;
        }
        index_to_filename(rec_context->cur_play_num, rec_context->play_file_name);
        res = f_open(file, rec_context->play_file_name, FA_READ | FA_OPEN_EXISTING);
        if (FR_NO_FILE == res || FR_NO_PATH == res) {
            f_close(file);
            if (try_count) {
                --try_count;
                goto next;
            }
            libc_free(file);
        } else {
            f_close(file);
            libc_free(file);
            if (FR_OK == res) {
                start_play_record_file();
                rec_context->play_direction = PLAY_NEXT;
            }
        }
    }

    sleep_delay_set();
    sleep_prevent_clear(PS_APP_PROCESS);

    return REC_RET_SUCCESS;
}



/* 播放上一个录音文件；
 * @param rec_context: 录音上下文
 */
int play_prev_record_file(void)
{
    FRESULT res;
    XFILE *file;
    uint32_t try_count = rec_context->max_play_num;

    if (false == device_is_online(DEV_SDCARD)
        && false == device_is_online(DEV_UDISK)) {
        loge("no rec dev");
        return REC_RET_ERR;
    }

    if (rec_context->total_file <= 0) {
        return REC_RET_ERR;
    }

    if (rec_context->file_fail_count >= rec_context->total_file) {
        stop_play_record_file();
        return REC_RET_ERR;
    }

    sleep_delay_set();
    sleep_prevent_set(PS_APP_PROCESS);

    prepare_record_device();

    if (REC_STATUS_RECORDING != rec_context->status) {
        stop_play_record_file();
        file = libc_calloc(sizeof(*file));
prev:
        if (rec_context->cur_play_num > 0) {
            rec_context->cur_play_num--;
        } else {
            rec_context->cur_play_num = rec_context->max_play_num;
        }
        index_to_filename(rec_context->cur_play_num, rec_context->play_file_name);
        res = f_open(file, rec_context->play_file_name, FA_READ | FA_OPEN_EXISTING);
        logv("try_count:%d, fopen res:%d", try_count, res);
        if (FR_NO_FILE == res || FR_NO_PATH == res) {
            f_close(file);
            if (try_count) {
                --try_count;
                goto prev;
            }
            libc_free(file);
        } else {
            f_close(file);
            libc_free(file);
            if (FR_OK == res) {
                start_play_record_file();
                rec_context->play_direction = PLAY_PREV;
            }
        }
    }

    sleep_delay_set();
    sleep_prevent_clear(PS_APP_PROCESS);

    return REC_RET_SUCCESS;
}


/* 删除录音文件；
 * @param rec_context: 录音上下文
 */
void delete_record_file_by_num(void)
{
    FRESULT res;
    XFILE *file;
    uint32_t try_count = rec_context->max_play_num;

    if (rec_context->cur_play_num < 0 || rec_context->total_file == 0) {
        return;
    }

    if (REC_STATUS_RECORDING == rec_context->status ){
        logd("stop record");
        stop_record();
    } else if (REC_STATUS_PLAYING == rec_context->status) {
        logd("stop play");
        stop_play_record_file();
    } else {
        /* check if the file exists.*/
        file = libc_calloc(sizeof(*file));
next:
        index_to_filename(rec_context->cur_play_num, rec_context->play_file_name);
        res = f_open(file, rec_context->play_file_name, FA_READ | FA_OPEN_EXISTING);
        if (FR_NO_FILE == res || FR_NO_PATH == res) {
            f_close(file);
            if (try_count) {
                --try_count;
                /* 跳过不存在的文件 */
                if (rec_context->cur_play_num < rec_context->max_play_num) {
                    rec_context->cur_play_num++;
                } else {
                    rec_context->cur_play_num = 0;
                }
                goto next;
            }
        } else {
            f_close(file);
        }
        libc_free(file);
    }

	index_to_filename(rec_context->cur_play_num, rec_context->play_file_name);
    logd("delete record file:%s", rec_context->play_file_name);
    res = f_unlink(rec_context->play_file_name);//delete the record file
    if (FR_OK == res) {
        //logd("result=%d",res);
        rec_context->total_file--;
        rec_context->cur_play_num++;
        logd("cur_play_num:%d, total_file:%d", rec_context->cur_play_num,rec_context->total_file);
    } else{
        logd("error = %d",res);
    }
}

#endif

