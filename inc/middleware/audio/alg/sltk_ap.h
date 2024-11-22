#ifndef SLTK_AP_H_
#define SLTK_AP_H_

#ifdef _MSC_VER
#else
#include "sys_inc.h"
#include "oal.h"
#endif
#include "sltk_audio.h"
#include "stream_io.h"
#include "sltk_codecs_common.h"
#include "fade.h"


#ifdef __cplusplus
extern "C" {
#endif

const parser_plugin_t* parser_plugin_get(ctype_t ctype);
const decoder_plugin_t* decoder_plugin_get(atype_t atype);

#ifdef __cplusplus
}
#endif

#endif /*_SLTK_AP_H_ */
