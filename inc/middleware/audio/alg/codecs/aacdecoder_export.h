#ifndef AACELD_EXPORT_H
#define AACELD_EXPORT_H

#define AACDECODER_LIB_VL0 3
#define AACDECODER_LIB_VL1 0
#define AACDECODER_LIB_VL2 0

/**
 * \brief  AAC decoder error codes.
 */
typedef enum {
  AAC_DEC_OK =
      0x0000, /*!< No error occurred. Output buffer is valid and error free. */
  AAC_DEC_OUT_OF_MEMORY =
      0x0002, /*!< Heap returned NULL pointer. Output buffer is invalid. */
  AAC_DEC_UNKNOWN =
      0x0005, /*!< Error condition is of unknown reason, or from a another
                 module. Output buffer is invalid. */

  /* Synchronization errors. Output buffer is invalid. */
  aac_dec_sync_error_start = 0x1000,
  AAC_DEC_TRANSPORT_SYNC_ERROR = 0x1001, /*!< The transport decoder had
                                            synchronization problems. Do not
                                            exit decoding. Just feed new
                                              bitstream data. */
  AAC_DEC_NOT_ENOUGH_BITS = 0x1002, /*!< The input buffer ran out of bits. */
  aac_dec_sync_error_end = 0x1FFF,

  /* Initialization errors. Output buffer is invalid. */
  aac_dec_init_error_start = 0x2000,
  AAC_DEC_INVALID_HANDLE =
      0x2001, /*!< The handle passed to the function call was invalid (NULL). */
  AAC_DEC_UNSUPPORTED_AOT =
      0x2002, /*!< The AOT found in the configuration is not supported. */
  AAC_DEC_UNSUPPORTED_FORMAT =
      0x2003, /*!< The bitstream format is not supported.  */
  AAC_DEC_UNSUPPORTED_ER_FORMAT =
      0x2004, /*!< The error resilience tool format is not supported. */
  AAC_DEC_UNSUPPORTED_EPCONFIG =
      0x2005, /*!< The error protection format is not supported. */
  AAC_DEC_UNSUPPORTED_MULTILAYER =
      0x2006, /*!< More than one layer for AAC scalable is not supported. */
  AAC_DEC_UNSUPPORTED_CHANNELCONFIG =
      0x2007, /*!< The channel configuration (either number or arrangement) is
                 not supported. */
  AAC_DEC_UNSUPPORTED_SAMPLINGRATE = 0x2008, /*!< The sample rate specified in
                                                the configuration is not
                                                supported. */
  AAC_DEC_INVALID_SBR_CONFIG =
      0x2009, /*!< The SBR configuration is not supported. */
  AAC_DEC_SET_PARAM_FAIL = 0x200A,  /*!< The parameter could not be set. Either
                                       the value was out of range or the
                                       parameter does  not exist. */
  AAC_DEC_NEED_TO_RESTART = 0x200B, /*!< The decoder needs to be restarted,
                                       since the required configuration change
                                       cannot be performed. */
  AAC_DEC_OUTPUT_BUFFER_TOO_SMALL =
      0x200C, /*!< The provided output buffer is too small. */
  aac_dec_init_error_end = 0x2FFF,

  /* Decode errors. Output buffer is valid but concealed. */
  aac_dec_decode_error_start = 0x4000,
  AAC_DEC_TRANSPORT_ERROR =
      0x4001, /*!< The transport decoder encountered an unexpected error. */
  AAC_DEC_PARSE_ERROR = 0x4002, /*!< Error while parsing the bitstream. Most
                                   probably it is corrupted, or the system
                                   crashed. */
  AAC_DEC_UNSUPPORTED_EXTENSION_PAYLOAD =
      0x4003, /*!< Error while parsing the extension payload of the bitstream.
                 The extension payload type found is not supported. */
  AAC_DEC_DECODE_FRAME_ERROR = 0x4004, /*!< The parsed bitstream value is out of
                                          range. Most probably the bitstream is
                                          corrupt, or the system crashed. */
  AAC_DEC_CRC_ERROR = 0x4005,          /*!< The embedded CRC did not match. */
  AAC_DEC_INVALID_CODE_BOOK = 0x4006,  /*!< An invalid codebook was signaled.
                                          Most probably the bitstream is corrupt,
                                          or the system  crashed. */
  AAC_DEC_UNSUPPORTED_PREDICTION =
      0x4007, /*!< Predictor found, but not supported in the AAC Low Complexity
                 profile. Most probably the bitstream is corrupt, or has a wrong
                 format. */
  AAC_DEC_UNSUPPORTED_CCE = 0x4008, /*!< A CCE element was found which is not
                                       supported. Most probably the bitstream is
                                       corrupt, or has a wrong format. */
  AAC_DEC_UNSUPPORTED_LFE = 0x4009, /*!< A LFE element was found which is not
                                       supported. Most probably the bitstream is
                                       corrupt, or has a wrong format. */
  AAC_DEC_UNSUPPORTED_GAIN_CONTROL_DATA =
      0x400A, /*!< Gain control data found but not supported. Most probably the
                 bitstream is corrupt, or has a wrong format. */
  AAC_DEC_UNSUPPORTED_SBA =
      0x400B, /*!< SBA found, but currently not supported in the BSAC profile.
               */
  AAC_DEC_TNS_READ_ERROR = 0x400C, /*!< Error while reading TNS data. Most
                                      probably the bitstream is corrupt or the
                                      system crashed. */
  AAC_DEC_RVLC_ERROR =
      0x400D, /*!< Error while decoding error resilient data. */
  aac_dec_decode_error_end = 0x4FFF,
  /* Ancillary data errors. Output buffer is valid. */
  aac_dec_anc_data_error_start = 0x8000,
  AAC_DEC_ANC_DATA_ERROR =
      0x8001, /*!< Non severe error concerning the ancillary data handling. */
  AAC_DEC_TOO_SMALL_ANC_BUFFER = 0x8002,  /*!< The registered ancillary data
                                             buffer is too small to receive the
                                             parsed data. */
  AAC_DEC_TOO_MANY_ANC_ELEMENTS = 0x8003, /*!< More than the allowed number of
                                             ancillary data elements should be
                                             written to buffer. */
  aac_dec_anc_data_error_end = 0x8FFF

} AAC_DECODER_ERROR;

/** Macro to identify initialization errors. Output buffer is invalid. */
#define IS_INIT_ERROR(err)                                                    \
  ((((err) >= aac_dec_init_error_start) && ((err) <= aac_dec_init_error_end)) \
       ? 1                                                                    \
       : 0)
/** Macro to identify decode errors. Output buffer is valid but concealed. */
#define IS_DECODE_ERROR(err)                 \
  ((((err) >= aac_dec_decode_error_start) && \
    ((err) <= aac_dec_decode_error_end))     \
       ? 1                                   \
       : 0)
/**
 * Macro to identify if the audio output buffer contains valid samples after
 * calling aacDecoder_DecodeFrame(). Output buffer is valid but can be
 * concealed.
 */
#define IS_OUTPUT_VALID(err) (((err) == AAC_DEC_OK) || IS_DECODE_ERROR(err))

/**
 * \brief AAC decoder setting parameters
 */
typedef enum {
  AAC_PCM_DUAL_CHANNEL_OUTPUT_MODE =
      0x0002, /*!< Defines how the decoder processes two channel signals: \n
                   0: Leave both signals as they are (default). \n
                   1: Create a dual mono output signal from channel 1. \n
                   2: Create a dual mono output signal from channel 2. \n
                   3: Create a dual mono output signal by mixing both channels
                 (L' = R' = 0.5*Ch1 + 0.5*Ch2). */
  AAC_PCM_OUTPUT_CHANNEL_MAPPING =
      0x0003, /*!< Output buffer channel ordering. 0: MPEG PCE style order, 1:
                 WAV file channel order (default). */
  AAC_PCM_LIMITER_ENABLE =
      0x0004,                           /*!< Enable signal level limiting. \n
                                             -1: Auto-config. Enable limiter for all
                                           non-lowdelay configurations by default. \n
                                              0: Disable limiter in general. \n
                                              1: Enable limiter always.
                                             It is recommended to call the decoder
                                           with a AACDEC_CLRHIST flag to reset all
                                           states when      the limiter switch is changed
                                           explicitly. */
  AAC_PCM_LIMITER_ATTACK_TIME = 0x0005, /*!< Signal level limiting attack time
                                           in ms. Default configuration is 15
                                           ms. Adjustable range from 1 ms to 15
                                           ms. */
  AAC_PCM_LIMITER_RELEAS_TIME = 0x0006, /*!< Signal level limiting release time
                                           in ms. Default configuration is 50
                                           ms. Adjustable time must be larger
                                           than 0 ms. */
  AAC_PCM_MIN_OUTPUT_CHANNELS =
      0x0011, /*!< Minimum number of PCM output channels. If higher than the
                 number of encoded audio channels, a simple channel extension is
                 applied (see note 4 for exceptions). \n -1, 0: Disable channel
                 extension feature. The decoder output contains the same number
                 of channels as the encoded bitstream. \n 1:    This value is
                 currently needed only together with the mix-down feature. See
                          ::AAC_PCM_MAX_OUTPUT_CHANNELS and note 2 below. \n
                    2:    Encoded mono signals will be duplicated to achieve a
                 2/0/0.0 channel output configuration. \n 6:    The decoder
                 tries to reorder encoded signals with less than six channels to
                 achieve a 3/0/2.1 channel output signal. Missing channels will
                 be filled with a zero signal. If reordering is not possible the
                 empty channels will simply be appended. Only available if
                 instance is configured to support multichannel output. \n 8:
                 The decoder tries to reorder encoded signals with less than
                 eight channels to achieve a 3/0/4.1 channel output signal.
                 Missing channels will be filled with a zero signal. If
                 reordering is not possible the empty channels will simply be
                          appended. Only available if instance is configured to
                 support multichannel output.\n NOTE: \n
                     1. The channel signaling (CStreamInfo::pChannelType and
                 CStreamInfo::pChannelIndices) will not be modified. Added empty
                 channels will be signaled with channel type
                        AUDIO_CHANNEL_TYPE::ACT_NONE. \n
                     2. If the parameter value is greater than that of
                 ::AAC_PCM_MAX_OUTPUT_CHANNELS both will be set to the same
                 value. \n
                     3. This parameter does not affect MPEG Surround processing.
                 \n
                     4. This parameter will be ignored if the number of encoded
                 audio channels is greater than 8. */
  AAC_PCM_MAX_OUTPUT_CHANNELS =
      0x0012, /*!< Maximum number of PCM output channels. If lower than the
                 number of encoded audio channels, downmixing is applied
                 accordingly (see note 5 for exceptions). If dedicated metadata
                 is available in the stream it will be used to achieve better
                 mixing results. \n -1, 0: Disable downmixing feature. The
                 decoder output contains the same number of channels as the
                 encoded bitstream. \n 1:    All encoded audio configurations
                 with more than one channel will be mixed down to one mono
                 output signal. \n 2:    The decoder performs a stereo mix-down
                 if the number encoded audio channels is greater than two. \n 6:
                 If the number of encoded audio channels is greater than six the
                 decoder performs a mix-down to meet the target output
                 configuration of 3/0/2.1 channels. Only available if instance
                 is configured to support multichannel output. \n 8:    This
                 value is currently needed only together with the channel
                 extension feature. See ::AAC_PCM_MIN_OUTPUT_CHANNELS and note 2
                 below. Only available if instance is configured to support
                 multichannel output. \n NOTE: \n
                     1. Down-mixing of any seven or eight channel configuration
                 not defined in ISO/IEC 14496-3 PDAM 4 is not supported by this
                 software version. \n
                     2. If the parameter value is greater than zero but smaller
                 than ::AAC_PCM_MIN_OUTPUT_CHANNELS both will be set to same
                 value. \n
                     3. The operating mode of the MPEG Surround module will be
                 set accordingly. \n
                     4. Setting this parameter with any value will disable the
                 binaural processing of the MPEG Surround module
                     5. This parameter will be ignored if the number of encoded
                 audio channels is greater than 8. */
  AAC_METADATA_PROFILE =
      0x0020, /*!< See ::AAC_MD_PROFILE for all available values. */
  AAC_METADATA_EXPIRY_TIME = 0x0021, /*!< Defines the time in ms after which all
                                        the bitstream associated meta-data (DRC,
                                        downmix coefficients, ...) will be reset
                                        to default if no update has been
                                        received. Negative values disable the
                                        feature. */

  AAC_CONCEAL_METHOD = 0x0100, /*!< Error concealment: Processing method. \n
                                    0: Spectral muting. \n
                                    1: Noise substitution (see ::CONCEAL_NOISE).
                                  \n 2: Energy interpolation (adds additional
                                  signal delay of one frame, see
                                  ::CONCEAL_INTER. only some AOTs are
                                  supported). \n */
  AAC_DRC_BOOST_FACTOR =
      0x0200, /*!< Dynamic Range Control: Scaling factor for boosting gain
                 values. Defines how the boosting DRC factors (conveyed in the
                 bitstream) will be applied to the decoded signal. The valid
                 values range from 0 (don't apply boost factors) to 127 (fully
                   apply boost factors). Default value is 0. */
  AAC_DRC_ATTENUATION_FACTOR =
      0x0201, /*!< Dynamic Range Control: Scaling factor for attenuating gain
                 values. Same as
                   ::AAC_DRC_BOOST_FACTOR but for attenuating DRC factors. */
  AAC_DRC_REFERENCE_LEVEL =
      0x0202, /*!< Dynamic Range Control (DRC): Target reference level. Defines
                 the level below full-scale (quantized in steps of 0.25dB) to
                 which the output audio signal will be normalized to by the DRC
                 module. The parameter controls loudness normalization for both
                 MPEG-4 DRC and MPEG-D DRC. The valid values range from 40 (-10
                 dBFS) to 127 (-31.75 dBFS). Any value smaller than 0 switches
                 off loudness normalization and MPEG-4 DRC. By default, loudness
                 normalization and MPEG-4 DRC is switched off. */
  AAC_DRC_HEAVY_COMPRESSION =
      0x0203, /*!< Dynamic Range Control: En-/Disable DVB specific heavy
                 compression (aka RF mode). If set to 1, the decoder will apply
                 the compression values from the DVB specific ancillary data
                 field. At the same time the MPEG-4 Dynamic Range Control tool
                 will be disabled. By default, heavy compression is disabled. */
  AAC_DRC_DEFAULT_PRESENTATION_MODE =
      0x0204, /*!< Dynamic Range Control: Default presentation mode (DRC
                 parameter handling). \n Defines the handling of the DRC
                 parameters boost factor, attenuation factor and heavy
                 compression, if no presentation mode is indicated in the
                 bitstream.\n For options, see
                 ::AAC_DRC_DEFAULT_PRESENTATION_MODE_OPTIONS.\n Default:
                 ::AAC_DRC_PARAMETER_HANDLING_DISABLED */
  AAC_DRC_ENC_TARGET_LEVEL =
      0x0205, /*!< Dynamic Range Control: Encoder target level for light (i.e.
                 not heavy) compression.\n If known, this declares the target
                 reference level that was assumed at the encoder for calculation
                   of limiting gains. The valid values range from 0 (full-scale)
                 to 127 (31.75 dB below full-scale). This parameter is used only
                 with ::AAC_DRC_PARAMETER_HANDLING_ENABLED and ignored
                 otherwise.\n Default: 127 (worst-case assumption).\n */
  AAC_QMF_LOWPOWER = 0x0300, /*!< Quadrature Mirror Filter (QMF) Bank processing
                                mode. \n -1: Use internal default. Implies MPEG
                                Surround partially complex accordingly. \n 0:
                                Use complex QMF data mode. \n 1: Use real (low
                                power) QMF data mode. \n */
  AAC_TPDEC_CLEAR_BUFFER =
      0x0603, /*!< Clear internal bit stream buffer of transport layers. The
                 decoder will start decoding at new data passed after this event
                 and any previous data is discarded. */
  AAC_UNIDRC_SET_EFFECT = 0x0903 /*!<  MPEG-D DRC: Request a DRC effect type for
                                    selection of a DRC set.\n Supported indices
                                    are:\n -1: DRC off. Completely disables
                                    MPEG-D DRC.\n 0: None (default). Disables
                                    MPEG-D DRC, but automatically enables DRC if
                                    necessary to prevent clipping.\n 1: Late
                                    night\n 2: Noisy environment\n 3: Limited
                                    playback range\n 4: Low playback level\n 5:
                                    Dialog enhancement\n 6: General compression.
                                    Used for generally enabling MPEG-D DRC
                                    without particular request.\n */
} AACDEC_PARAM;

/**
 * \brief This structure gives information about the currently decoded audio
 * data. All fields are read-only.
 */
typedef struct {
  /* These five members are the only really relevant ones for the user. */
  int32_t sampleRate; /*!< The sample rate in Hz of the decoded PCM audio signal. */
  int32_t frameSize;  /*!< The frame size of the decoded PCM audio signal. \n
                       Typically this is: \n
                       1024 or 960 for AAC-LC \n
                       2048 or 1920 for HE-AAC (v2) \n
                       512 or 480 for AAC-LD and AAC-ELD \n
                       768, 1024, 2048 or 4096 for USAC  */
  int32_t numChannels; /*!< The number of output audio channels before the rendering
                      module, i.e. the original channel configuration. */
  int32_t
  *pChannelType; /*!< Audio channel type of each output audio channel. */
  uint8_t *pChannelIndices; /*!< Audio channel index for each output audio
                             channel. See ISO/IEC 13818-7:2005(E), 8.5.3.2
                             Explicit channel mapping using a
                             program_config_element() */
  /* Decoder internal members. */
  int32_t aacSampleRate; /*!< Sampling rate in Hz without SBR (from configuration
                        info) divided by a (ELD) downscale factor if present. */
  int32_t profile; /*!< MPEG-2 profile (from file header) (-1: not applicable (e. g.
                  MPEG-4)).               */
  int32_t
  aot; /*!< Audio Object Type (from ASC): is set to the appropriate value
          for MPEG-2 bitstreams (e. g. 2 for AAC-LC). */
  int32_t channelConfig; /*!< Channel configuration (0: PCE defined, 1: mono, 2:
                        stereo, ...                       */
  int32_t bitRate;       /*!< Instantaneous bit rate.                   */
  int32_t aacSamplesPerFrame;   /*!< Samples per frame for the AAC core (from ASC)
                               divided by a (ELD) downscale factor if present. \n
                                 Typically this is (with a downscale factor of 1):
                               \n   1024 or 960 for AAC-LC \n   512 or 480 for
                               AAC-LD   and AAC-ELD         */
  int32_t aacNumChannels;       /*!< The number of audio channels after AAC core
                               processing (before PS or MPS processing).       CAUTION: This
                               are not the final number of output channels! */
  int32_t extAot; /*!< Extension Audio Object Type (from ASC)   */
  int32_t extSamplingRate; /*!< Extension sampling rate in Hz (from ASC) divided by
                          a (ELD) downscale factor if present. */

  uint32_t outputDelay; /*!< The number of samples the output is additionally
                       delayed by.the decoder. */
  uint32_t flags; /*!< Copy of internal flags. Only to be written by the decoder,
                 and only to be read externally. */

  int8_t epConfig; /*!< epConfig level (from ASC): only level 0 supported, -1
                     means no ER (e. g. AOT=2, MPEG-2 AAC, etc.)  */
  /* Statistics */
  int32_t numLostAccessUnits; /*!< This integer will reflect the estimated amount of
                             lost access units in case aacDecoder_DecodeFrame()
                               returns AAC_DEC_TRANSPORT_SYNC_ERROR. It will be
                             < 0 if the estimation failed. */

  int64_t numTotalBytes; /*!< This is the number of total bytes that have passed
                          through the decoder. */
  int64_t
  numBadBytes; /*!< This is the number of total bytes that were considered
                  with errors from numTotalBytes. */
  int64_t
  numTotalAccessUnits;     /*!< This is the number of total access units that
                              have passed through the decoder. */
  int64_t numBadAccessUnits; /*!< This is the number of total access units that
                              were considered with errors from numTotalBytes. */

  /* Metadata */
  int8_t drcProgRefLev; /*!< DRC program reference level. Defines the reference
                          level below full-scale. It is quantized in steps of
                          0.25dB. The valid values range from 0 (0 dBFS) to 127
                          (-31.75 dBFS). It is used to reflect the average
                          loudness of the audio in LKFS according to ITU-R BS
                          1770. If no level has been found in the bitstream the
                          value is -1. */
  int8_t
  drcPresMode; /*!< DRC presentation mode. According to ETSI TS 101 154,
                  this field indicates whether   light (MPEG-4 Dynamic Range
                  Control tool) or heavy compression (DVB heavy
                  compression)   dynamic range control shall take priority
                  on the outputs.   For details, see ETSI TS 101 154, table
                  C.33. Possible values are: \n   -1: No corresponding
                  metadata found in the bitstream \n   0: DRC presentation
                  mode not indicated \n   1: DRC presentation mode 1 \n   2:
                  DRC presentation mode 2 \n   3: Reserved */

} CStreamInfo;

typedef enum {
  TT_UNKNOWN = -1, /**< Unknown format.            */
  TT_MP4_RAW = 0,  /**< "as is" access units (packet based since there is
                      obviously no sync layer) */
  TT_MP4_ADIF = 1, /**< ADIF bitstream format.     */
  TT_MP4_ADTS = 2, /**< ADTS bitstream format.     */

  TT_MP4_LATM_MCP1 = 6, /**< Audio Mux Elements with muxConfigPresent = 1 */
  TT_MP4_LATM_MCP0 = 7, /**< Audio Mux Elements with muxConfigPresent = 0, out
                           of band StreamMuxConfig */

  TT_MP4_LOAS = 10, /**< Audio Sync Stream.         */

  TT_DRM = 12 /**< Digital Radio Mondial (DRM30/DRM+) bitstream format. */

} TRANSPORT_TYPE;

typedef enum {
    ConcealMethodNone = -1,
    ConcealMethodMute = 0,
    ConcealMethodNoise = 1,
    ConcealMethodInter = 2,
    ConcealMethodTonal = 3
} CConcealmentMethod;

typedef void* HANDLE_AACDECODER;
//typedef AAC_DECODER_INSTANCE *HANDLE_AACDECODER; /*!< Pointer to a AAC decoder instance. */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Set one single decoder parameter.
 *
 * \param self   AAC decoder handle.
 * \param param  Parameter to be set.
 * \param value  Parameter value.
 * \return       Error code.
 */
 AAC_DECODER_ERROR aacDecoder_SetParam(const HANDLE_AACDECODER self,
                                                 const AACDEC_PARAM param,
                                                 const int32_t value);

/**
 * \brief              Get free bytes inside decoder internal buffer.
 * \param self         Handle of AAC decoder instance.
 * \param pFreeBytes   Pointer to variable receiving amount of free bytes inside
 * decoder internal buffer.
 * \return             Error code.
 */
 AAC_DECODER_ERROR
aacDecoder_GetFreeBytes(const HANDLE_AACDECODER self, uint32_t *pFreeBytes);

/**
 * \brief               Open an AAC decoder instance.
 * \param transportFmt  The transport type to be used.
 * \param nrOfLayers    Number of transport layers.
 * \return              AAC decoder handle.
 */
 HANDLE_AACDECODER aacDecoder_Open(int32_t transportFmt,
                                             uint32_t nrOfLayers);

/**
 * \brief Explicitly configure the decoder by passing a raw AudioSpecificConfig
 * (ASC) or a StreamMuxConfig (SMC), contained in a binary buffer. This is
 * required for MPEG-4 and Raw Packets file format bitstreams as well as for
 * LATM bitstreams with no in-band SMC. If the transport format is LATM with or
 * without LOAS, configuration is assumed to be an SMC, for all other file
 * formats an ASC.
 *
 * \param self    AAC decoder handle.
 * \param conf    Pointer to an unsigned char buffer containing the binary
 * configuration buffer (either ASC or SMC).
 * \param length  Length of the configuration buffer in bytes.
 * \return        Error code.
 */
 AAC_DECODER_ERROR aacDecoder_ConfigRaw(HANDLE_AACDECODER self,
                                                  uint8_t *conf[],
                                                  const uint32_t length[]);

/**
 * \brief Fill AAC decoder's internal input buffer with bitstream data from the
 * external input buffer. The function only copies such data as long as the
 * decoder-internal input buffer is not full. So it grabs whatever it can from
 * pBuffer and returns information (bytesValid) so that at a subsequent call of
 * %aacDecoder_Fill(), the right position in pBuffer can be determined to grab
 * the next data.
 *
 * \param self        AAC decoder handle.
 * \param pBuffer     Pointer to external input buffer.
 * \param bufferSize  Size of external input buffer. This argument is required
 * because decoder-internally we need the information to calculate the offset to
 * pBuffer, where the next available data is, which is then
 * fed into the decoder-internal buffer (as much as
 * possible). Our example framework implementation fills the
 * buffer at pBuffer again, once it contains no available valid bytes anymore
 * (meaning bytesValid equal 0).
 * \param bytesValid  Number of bitstream bytes in the external bitstream buffer
 * that have not yet been copied into the decoder's internal bitstream buffer by
 * calling this function. The value is updated according to
 * the amount of newly copied bytes.
 * \return            Error code.
 */
 AAC_DECODER_ERROR aacDecoder_Fill(HANDLE_AACDECODER self,
                                             uint8_t *pBuffer[],
                                             const uint32_t bufferSize[],
                                             uint32_t *bytesValid);

#define AACDEC_CONCEAL                                                        \
  1 /*!< Flag for aacDecoder_DecodeFrame(): Trigger the built-in error        \
       concealment module to generate a substitute signal for one lost frame. \
       New input data will not be considered. */
#define AACDEC_FLUSH                                                         \
  2 /*!< Flag for aacDecoder_DecodeFrame(): Flush all filterbanks to get all \
       delayed audio without having new input data. Thus new input data will \
       not be considered.*/
#define AACDEC_INTR                                                         \
  4 /*!< Flag for aacDecoder_DecodeFrame(): Signal an input bit stream data \
       discontinuity. Resync any internals as necessary. */
#define AACDEC_CLRHIST                                                        \
  8 /*!< Flag for aacDecoder_DecodeFrame(): Clear all signal delay lines and  \
       history buffers. CAUTION: This can cause discontinuities in the output \
       signal. */

/**
 * \brief               Decode one audio frame
 *
 * \param self          AAC decoder handle.
 * \param pTimeData     Pointer to external output buffer where the decoded PCM
 * samples will be stored into.
 * \param timeDataSize  Size of external output buffer.
 * \param flags         Bit field with flags for the decoder: \n
 *                      (flags & AACDEC_CONCEAL) == 1: Do concealment. \n
 *                      (flags & AACDEC_FLUSH) == 2: Discard input data. Flush
 * filter banks (output delayed audio). \n (flags & AACDEC_INTR) == 4: Input
 * data is discontinuous. Resynchronize any internals as
 * necessary. \n (flags & AACDEC_CLRHIST) == 8: Clear all signal delay lines and
 * history buffers.
 * \return              Error code.
 */
#if 0
AAC_DECODER_ERROR
aacDecoder_DecodeFrame(HANDLE_AACDECODER self, INT_PCM *pTimeData_extern,
                       const INT timeDataSize_extern, const UINT flags);
#else
AAC_DECODER_ERROR
aacDecoder_DecodeFrame(HANDLE_AACDECODER self, int16_t **left, int16_t **right,
                       const INT timeDataSize_extern, const UINT flags);
#endif

/**
 * \brief       De-allocate all resources of an AAC decoder instance.
 *
 * \param self  AAC decoder handle.
 * \return      void.
 */
 void aacDecoder_Close(HANDLE_AACDECODER self);

/**
 * \brief       Get CStreamInfo handle from decoder.
 *
 * \param self  AAC decoder handle.
 * \return      Reference to requested CStreamInfo.
 */
 CStreamInfo *aacDecoder_GetStreamInfo(HANDLE_AACDECODER self);

#endif /* AACDECODER_LIB_H */