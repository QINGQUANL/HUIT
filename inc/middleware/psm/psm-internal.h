#ifndef __PSM_INTERNAL_H__
#define __PSM_INTERNAL_H__

/* #define CONFIG_PSM_INIT_SCAN_DEBUG */
/* #define CONFIG_PSM_EXTRA_DEBUG */
/* #define CONFIG_PSM_FUNC_DEBUG */
/* #define CONFIG_PSM_INDEXING_DEBUG */
/* #define CONFIG_PSM_LOWLEVEL_FLASH_OPR_DEBUG */
/* #define CONFIG_PSM_LOCKS_DEBUG */

#ifdef CONFIG_PSM_INIT_SCAN_DEBUG
#define psm_isc(...)				\
	psmlog("psm-isc", ##__VA_ARGS__)
#else
#define psm_isc(...)
#endif

#define psm_e(...)				\
	psmlog_e("psm", ##__VA_ARGS__)
#define psm_w(...)				\
	psmlog_w("psm", ##__VA_ARGS__)
//#define CONFIG_PSM_DEBUG
#ifdef CONFIG_PSM_DEBUG
#define psm_d(...)				\
	psmlog("psm", ##__VA_ARGS__)
#else
#define psm_d(...)
#endif /* ! CONFIG_PSM_DEBUG */
#ifdef CONFIG_PSM_EXTRA_DEBUG
#define psm_ed(...)				\
	psmlog("psm-ed", ##__VA_ARGS__)
#else
#define psm_ed(...)
#endif /* ! CONFIG_PSM_EXTRA_DEBUG */
#ifdef CONFIG_PSM_INDEXING_DEBUG
#define psm_i(...)				\
	psmlog("psm-i", ##__VA_ARGS__)
#else
#define psm_i(...)
#endif /* ! CONFIG_PSM_INDEXING_DEBUG */
#ifdef CONFIG_PSM_LOWLEVEL_FLASH_OPR_DEBUG
#define psm_ll(...)				\
	psmlog("psm-ll", ##__VA_ARGS__)
#else
#define psm_ll(...)
#endif /* ! CONFIG_PSM_LOWLEVEL_FLASH_OPR_DEBUG */

//#define CONFIG_PSM_FUNC_DEBUG
#ifdef CONFIG_PSM_FUNC_DEBUG
#define psm_entry(_fmt_, ...)						\
	logi("######## > %s ("_fmt_") ####\n\r", __func__, ##__VA_ARGS__)
#define psm_entry_i(...)			\
	logi(__VA_ARGS__)
#else
#define psm_entry(...)
#define psm_entry_i(...)
#endif /* ! CONFIG_PSM_DEBUG */


typedef enum {
	PSM_EVENT_READ,
	PSM_EVENT_WRITE,
	PSM_EVENT_ERASE,
	PSM_EVENT_COMPACTION,
} psm_event_t;

typedef enum {
	PSM_COMPACTION_1,
	PSM_COMPACTION_2,
	PSM_COMPACTION_3,
	PSM_COMPACTION_4,
	PSM_COMPACTION_5,
	PSM_COMPACTION_6,
	PSM_COMPACTION_7,
	PSM_COMPACTION_8,
	PSM_COMPACTION_9,
	PSM_COMPACTION_10,
	PSM_COMPACTION_MAX,
} psm_compaction_evt_t;

typedef void (*psm_event_callback)(psm_event_t event, void *data, void *data1);

int psm_register_event_callback(psm_event_callback cb);

#endif /* __PSM_INTERNAL_H__ */
