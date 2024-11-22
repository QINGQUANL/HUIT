#ifndef __PSM_TYPES_H__
#define __PSM_TYPES_H__

#ifdef __linux__
#include <stdint.h>
#else
#include "sys_inc.h"
#endif

#ifndef PACK_END
#ifdef __linux__
#define PACK_END
#else
#define PACK_END __attribute__((packed))
#endif
#endif

#ifdef __linux__
#define bool _Bool
#endif
#define true    1
#define false   0

#ifndef NULL
#define NULL (void*)0
#endif

/* Globally unique success code */
#define PSM_SUCCESS 0

#define MOD_ERROR_START(x)  (x << 12 | 0)
#define MOD_WARN_START(x)   (x << 12 | 1)
#define MOD_INFO_START(x)   (x << 12 | 2)

/* Create Module index */
#define MOD_GENERIC    0
/** Unused */
#define MOD_UNUSED_3   2
/** HTTPD module index */
#define MOD_HTTPD      3
/** Application framework module index */
#define MOD_AF         4
/** FTFS module index */
#define MOD_FTFS       5
/** RFGET module index */
#define MOD_RFGET      6
/** JSON module index  */
#define MOD_JSON       7
/** TELNETD module index */
#define MOD_TELNETD    8
/** SIMPLE MDNS module index */
#define MOD_SMDNS      9
/** EXML module index */
#define MOD_EXML       10
/** DHCPD module index */
#define MOD_DHCPD      11
/** MDNS module index */
#define MOD_MDNS       12
/** SYSINFO module index */
#define MOD_SYSINFO   13
/** Unused module index */
#define MOD_UNUSED_1     14
/** CRYPTO module index */
#define MOD_CRYPTO     15
/** HTTP-CLIENT module index */
#define MOD_HTTPC      16
/** PROVISIONING module index */
#define MOD_PROV       17
/** SPI module index */
#define MOD_SPI        18
/** PSM module index */
#define MOD_PSM        19
/** TTCP module index */
#define MOD_TTCP       20
/** DIAGNOSTICS module index */
#define MOD_DIAG       21
/** Unused module index */
#define MOD_UNUSED_2    22
/** WPS module index */
#define MOD_WPS        23
/** WLAN module index */
#define MOD_WLAN        24
/** USB module index */
#define MOD_USB        25
/** WIFI driver module index */
#define MOD_WIFI        26
/** Critical error module index */
#define MOD_CRIT_ERR    27
/** Last module index .Applications can define their own modules beyond this */
#define MOD_ERR_LAST	50

enum psm_errno {
	/* First Generic Error codes */
	PSM_GEN_E_BASE = MOD_ERROR_START(MOD_GENERIC),
	PSM_FAIL,     /* 1 */
	PSM_E_PERM,   /* 2: Operation not permitted */
	PSM_E_NOENT,  /* 3: No such file or directory */
	PSM_E_SRCH,   /* 4: No such process */
	PSM_E_INTR,   /* 5: Interrupted system call */
	PSM_E_IO,     /* 6: I/O error */
	PSM_E_NXIO,   /* 7: No such device or address */
	PSM_E_2BIG,   /* 8: Argument list too long */
	PSM_E_NOEXEC, /* 9: Exec format error */
	PSM_E_BADF,   /* 10: Bad file number */
	PSM_E_CHILD,  /* 11: No child processes */
	PSM_E_AGAIN,  /* 12: Try again */
	PSM_E_NOMEM,  /* 13: Out of memory */
	PSM_E_ACCES,  /* 14: Permission denied */
	PSM_E_FAULT,  /* 15: Bad address */
	PSM_E_NOTBLK, /* 16: Block device required */
	PSM_E_BUSY,   /* 17: Device or resource busy */
	PSM_E_EXIST,  /* 18: File exists */
	PSM_E_XDEV,   /* 19: Cross-device link */
	PSM_E_NODEV,  /* 20: No such device */
	PSM_E_NOTDIR, /* 21: Not a directory */
	PSM_E_ISDIR,  /* 22: Is a directory */
	PSM_E_INVAL,  /* 23: Invalid argument */
	PSM_E_NFILE,  /* 24: File table overflow */
	PSM_E_MFILE,  /* 25: Too many open files */
	PSM_E_NOTTY,  /* 26: Not a typewriter */
	PSM_E_TXTBSY, /* 27: Text file busy */
	PSM_E_FBIG,   /* 28: File too large */
	PSM_E_NOSPC,  /* 29: No space left on device */
	PSM_E_SPIPE,  /* 30: Illegal seek */
	PSM_E_ROFS,   /* 31: Read-only file system */
	PSM_E_MLINK,  /* 32: Too many links */
	PSM_E_PIPE,   /* 33: Broken pipe */
	PSM_E_DOM,    /* 34: Math argument out of domain of func */
	PSM_E_RANGE,  /* 35: Math result not representable */

	/* PSMSDK generic error codes */
	PSM_E_CRC,    /* 36: Error in CRC check */
	PSM_E_UNINIT,  /* 37: Module is not yet initialized */
	PSM_E_TIMEOUT, /* 38: Timeout occurred during operation */
};

#define psmprintf logi
#undef CONFIG_DEBUG_BUILD

#ifdef CONFIG_ENABLE_ERROR_LOGS
#define psmlog_e(_mod_name_, _fmt_, ...)                                        \
	psmprintf("[%s]%s" _fmt_ "\n\r", _mod_name_, " Error: ", ##__VA_ARGS__)
#else
#define psmlog_e(...)
#endif /* CONFIG_ENABLE_ERROR_LOGS */

#ifdef CONFIG_ENABLE_WARNING_LOGS
#define psmlog_w(_mod_name_, _fmt_, ...)                                        \
	psmprintf("[%s]%s" _fmt_ "\n\r", _mod_name_, " Warn: ", ##__VA_ARGS__)
#else
#define psmlog_w(...)
#endif /* CONFIG_ENABLE_WARNING_LOGS */

/* General debug function. User can map his own debug functions to this
ne */
#ifdef CONFIG_DEBUG_BUILD
#define psmlog(_mod_name_, _fmt_, ...)                          \
	psmprintf("[%s] " _fmt_ "\n\r", _mod_name_, ##__VA_ARGS__)
#else
#define psmlog(...)
#endif /* CONFIG_DEBUG_BUILD */

#ifdef CONFIG_DEBUG_BUILD
/* Function entry */
#define psmlog_entry(_fmt_, ...)                                        \
	psmprintf("> %s (" _fmt_ ")\n\r", __func__, ##__VA_ARGS__)

/* function exit */
#define psmlog_exit(_fmt_, ...)                                        \
	psmprintf("< %s" _fmt_ "\n\r", __func__, ##__VA_ARGS__)
#else
#define psmlog_entry(...)
#define psmlog_exit(...)
#endif /* CONFIG_DEBUG_BUILD */

#ifdef CONFIG_LL_DEBUG
#define ll_log(_fmt_, ...)						\
	ll_printf(_fmt_, ##__VA_ARGS__)
#else
#define ll_log(...)
#endif /* CONFIG_LL_DEBUG */

#endif
