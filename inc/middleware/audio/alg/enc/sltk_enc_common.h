#ifndef _SLTK_ENC_COMMON_H_
#define _SLTK_ENC_COMMON_H_

#include "sys_types.h"
#include "stream_io.h"

/*
 * get header size
 */
#define SLTK_ENC_CMD_GETHEADSIZE    0
/*
 * get header body
 */
#define SLTK_ENC_CMD_GETHEADBODY    1
/*
 * flush bitstream at the end of process
 */
#define SLTK_ENC_CMD_FLUSHBS        2

typedef int (*output_func) (void *handle, uint8_t * buff, int32_t size);
typedef int (*input_func) (void *handle, int16_t ** buff, int32_t chnum, int32_t size);

typedef struct {
    output_func func_out;
    void *handle_out;
    input_func func_in;
    void *handle_in;
    void *init_prm;
} encoder_init_t;

typedef enum {
    SLTK_CMD_ENC_RESET,
} encoder_cmd_t;

typedef enum {
    SLTK_RET_ENC_ERR = -1,
    SLTK_RET_ENC_NORMAL,
    SLTK_RET_ENC_EOF,
} encoder_ret_t;

typedef int (*pcm_read_t)(void* handle, void *buf, uint32_t bytes);
typedef int (*bs_write_t)(void* handle, void *buf, uint32_t bytes);

typedef struct {
    void *init_data;
    pcm_read_t pcm_r;
    void *pcm_r_handle;
    bs_write_t bs_w;
    void *bs_w_handle;
} enc_init_t;

typedef struct {
    /* open encoder handle */
    void* (*encoder_open)(enc_init_t *enc_init);
    /* frame encode */
    int32_t (*encoder_frame_encode)(void *encoder);
    /* commands control */
    int32_t(*encoder_cmd)(void *encoder, uint32_t cmd, uint32_t arg);
    /* close encoder handle */
    void (*encoder_close)(void* encoder);
} encoder_plugin_t;

typedef enum {
    SLTK_CMD_PACKER_RESET,
} packer_cmd_t;

typedef enum {
    SLTK_RET_PACKER_READ_ERR=-1,
    SLTK_RET_PACKER_NORMAL,
    SLTK_RET_PACKER_ENDOFPACKET,
    SLTK_RET_PACKER_ENDOFFILE,
} packer_ret_t;

typedef struct {
    void* (*packer_open)(streamio_t *stream, stream_info_t *info);
    void (*packer_close)(void *packer);
    int32_t (*packer_putframe)(void *packer, uint8_t *bsbuf, int32_t bytes);
    int32_t (*packer_cmd)(void *packer, uint32_t cmd, uint32_t arg);
} packer_plugin_t;

#endif
