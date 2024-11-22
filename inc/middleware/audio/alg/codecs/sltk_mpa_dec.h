#ifndef SLTK_MPA_DEC_H
#define SLTK_MPA_DEC_H

#include "sltk_codecs_common.h"

typedef struct MPADecodeHeader {
    int frame_size;
    int error_protection;
    int layer;
    int sample_rate;
    int sample_rate_index;      /* between 0 and 8 */
    int bit_rate;
    int nb_channels;
    int mode;
    int mode_ext;
    int lsf;
} MPADecodeHeader;

/* header decoding. MUST check the header before because no
   onsistency check is done there. Return 1 if free format found and
   that the frame size must be computed externally */
int avpriv_mpegaudio_decode_header(MPADecodeHeader * s, uint32_t header);

/* fast header check for resync */
static __inline int ff_mpa_check_header(uint32_t header)
{
    /* header */
    if ((header & 0xffe00000) != 0xffe00000)
        return -1;
    /* layer check */
    if ((header & (3 << 17)) == 0)
        return -1;
    /* bit rate */
    if ((header & (0xf << 12)) == 0xf << 12)
        return -1;
    /* frequency */
    if ((header & (3 << 10)) == 3 << 10)
        return -1;
    return 0;
}

#endif