#ifndef SLTK_CODECS_COMMON_H
#define SLTK_CODECS_COMMON_H

#include "sltk_audio.h"
#include "stream_io.h"
//#include "oal.h"
#include "fade.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_CHANNEL 2

typedef struct {
    void *pcm[MAX_CHANNEL];
    /* channels number*/
    int channels;
    /* frames(samples) of the output */
    int samples;
    /* bits of per sample,16/24/32 */
    int sample_bits;
    /* frac bits */
    int frac_bits;
} aoutinfo_t;
typedef aoutinfo_t sltk_aoutinfo_t;
typedef aoutinfo_t abuffer_info_t;

typedef enum {
    SLTK_CMD_DEC_RESET,
    SLTK_CMD_DEC_PLC,
    SLTK_CMD_DEC_SETFUNC,
    SLTK_CMD_DEC_SET_CRC,
} decoder_cmd_t;
typedef decoder_cmd_t sltk_decoder_cmd_t;

typedef enum {
    SLTK_RET_DEC_ERR = -1,
    SLTK_RET_DEC_NORMAL,
    SLTK_RET_DEC_EOF,
} decoder_ret_t;
typedef decoder_ret_t sltk_decoder_ret_t;

typedef int (*aout_callback_func)(void* aout, sltk_aoutinfo_t* info);
typedef aout_callback_func sltk_aout_callback_func;

typedef int (*bsinput_callback_func)(void* parser,
    unsigned char* bsbuff, int32_t *length);
typedef bsinput_callback_func sltk_bsinput_callback_func;

typedef struct {
    void *init_data;
    aout_callback_func aout_callback;
    void *aout_handle;
    bsinput_callback_func bs_callback;
    void *bs_handle;
} codec_init_t;
typedef codec_init_t sltk_codec_init_t;

typedef void* (*decoder_open_func)(codec_init_t *init_prms);
typedef int32_t (*decoder_frame_decode_func)(void *decoder);
typedef int32_t (*decoder_cmd_func)(void *decoder, uint32_t cmd, uint32_t arg);
typedef void (*decoder_close_func)(void* decoder);

typedef struct {
    /* open decoder handle */
    decoder_open_func decoder_open;

    /* frame decode */
    decoder_frame_decode_func decoder_frame_decode;

    /* commands control */
    decoder_cmd_func decoder_cmd;

    /* close decoder handle */
    decoder_close_func decoder_close;
} decoder_plugin_t;

typedef decoder_plugin_t sltk_decoder_plugin_t;
typedef decoder_plugin_t* (*decoder_plugin_get_t)(atype_t atype);
typedef void (*destroy_decoder_plugin_t)(decoder_plugin_t *plugin);

#define MAX_SEEKITEMS 100

typedef struct _unify_seek_table
{
    uint32_t toc[MAX_SEEKITEMS];
    /* coresponding time stamp */
    uint32_t toc_time[MAX_SEEKITEMS];
    /* seek step of the seek table */
    uint32_t toc_step;
    /* already read items number */
    uint8_t toc_itemnum;
}unify_seek_table;

typedef enum {
    SLTK_CMD_PARSER_RESET,
    SLTK_CMD_PARSER_SETFUNC,
    SLTK_CMD_PARSER_DUMP_SEEK_TABLE,
    SLTK_CMD_PARSER_SET_SEEK_TABLE,
    SLTK_CMD_PARSER_REDIRECT_CMD_HOOK,
} parser_cmd_t;
typedef parser_cmd_t sltk_parser_cmd_t;

typedef enum {
    SLTK_RET_PARSER_READ_ERR=-1,
    SLTK_RET_PARSER_NORMAL,
    SLTK_RET_PARSER_ENDOFPACKET,
    SLTK_RET_PARSER_ENDOFFILE,
} parser_ret_t;
typedef parser_ret_t sltk_parser_ret_t;

typedef void* (*parser_open_func_t)(streamio_t *stream, stream_info_t *info);
typedef int32_t (*parser_getframe_func_t)(void *handle, uint8_t *bsbuf, int32_t *bytes_needed);
typedef int32_t (*parser_seek_func_t)(void *parser, int32_t *time, int32_t whence);
typedef int32_t (*parser_cmd_func_t)(void *parser, uint32_t cmd, uint32_t arg);
typedef void (*parser_close_func_t)(void *parser);

typedef struct {
    void* (*parser_open)(streamio_t *stream, stream_info_t *info);
    int32_t (*parser_getframe)(void *handle,
        uint8_t *bsbuf, int32_t *bytes_needed);
    int32_t (*parser_seek)(void *parser, int32_t *time, int32_t whence);
    int32_t (*parser_cmd)(void *parser, uint32_t cmd, uint32_t arg);
    void (*parser_close)(void *parser);
} parser_plugin_t;

typedef parser_plugin_t sltk_parser_plugin_t;
typedef parser_plugin_t* (*parser_plugin_get_t)(ctype_t ctype);

#ifdef __cplusplus
}
#endif

#endif