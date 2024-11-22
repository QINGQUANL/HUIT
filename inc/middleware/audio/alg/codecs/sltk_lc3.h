
#ifndef _SLTK_LC3_H_
#define _SLTK_LC3_H_

#include "sltk_codecs_common.h"

/*! Construct version number from major/minor/micro values. */
#define LC3_VERSION_INT(major, minor, micro) (((major) << 16) | ((minor) << 8) | (micro))

/*! Version number to ensure header and binary are matching. */
#define LC3_VERSION LC3_VERSION_INT(1, 4, 2)

/*! Maximum number of supported channels. The actual binary might support
 *  less, use lc3_channels_supported() to check. */
#define LC3_MAX_CHANNELS 16

/*! Maximum number of samples per channel that can be stored in one LC3 frame.
 */
#define LC3_MAX_SAMPLES 960

/*! Maximum number of bytes of one LC3 frame. */
#define LC3_MAX_BYTES 6960

/*! Error codes returned by functions. */
typedef enum {
    LC3_OK                  = 0,  /*!< No error occurred */
    LC3_ERROR               = 1,  /*!< Function call failed */
    LC3_DECODE_ERROR        = 2,  /*!< Frame failed to decode and was concealed */
    LC3_NULL_ERROR          = 3,  /*!< Pointer argument is null */
    LC3_SAMPLERATE_ERROR    = 4,  /*!< Invalid samplerate value */
    LC3_CHANNELS_ERROR      = 5,  /*!< Invalid channels value */
    LC3_BITRATE_ERROR       = 6,  /*!< Invalid bitrate value */
    LC3_NUMBYTES_ERROR      = 7,  /*!< Invalid num_bytes value */
    LC3_EPMODE_ERROR        = 8,  /*!< Invalid ep_mode value */
    LC3_FRAMEMS_ERROR       = 9,  /*!< Invalid frame ms value */
    LC3_ALIGN_ERROR         = 10, /*!< Unaligned pointer */
    _SLTK_LC3_H_RMODE_ERROR        = 11, /*!< Invalid usage of hrmode, sampling rate and frame size */
    LC3_BITRATE_UNSET_ERROR = 12, /*!< Function called before bitrate has been set */
    LC3_BITRATE_SET_ERROR   = 13, /*!< Function called after bitrate has been set */
    _SLTK_LC3_H_RMODE_BW_ERROR     = 14, /*!< High quality mode and bandwidth switching must not be used together */
    LC3_PLCMODE_ERROR       = 15, /*!< Invalid plc_method value */

    /* START WARNING */
    LC3_WARNING             = 16,
    LC3_BW_WARNING          = 17  /*!< Invalid bandwidth cutoff frequency */
} LC3_Error;

/*! Decoder packet loss concealment mode */
typedef enum
{
    LC3_PLC_STANDARD = 0, /*!< Less complex than advanced method */
    LC3_PLC_ADVANCED = 1  /*!< Enhanced concealment method */
} LC3_PlcMode;

void* lc3_dec_open(int fs, int channels, int plc_mode, int hr_mode, float frame_ms);
void lc3_dec_close(void* handle);
int lc3_dec_framedecode(void* handle, uint8_t* bs, int num_bytes, int16_t** output_smaples);

void* lc3_enc_open(int sampleRate, int nChannels, int bitrate, float frame_ms, int hrmode);
void lc3_enc_close(void* handle);
int lc3_enc_frameenc(void* encoder, int16_t** input_samples, uint8_t* bsbuffer);
#endif /* LC3 */
