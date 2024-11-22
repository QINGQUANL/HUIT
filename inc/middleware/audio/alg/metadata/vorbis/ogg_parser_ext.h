/**
    Copyright (C) 2005  Michael Ahlberg, M?ns Rullg?rd

    Permission is hereby granted, sltk_free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
**/

#ifndef AVFORMAT_OGGDEC_EXT_H
#define AVFORMAT_OGGDEC_EXT_H

#ifdef _MSC_VER
#include "codecs_common.h"
#else
#include "sys_types.h"
#include "sltk_mem.h"
#include "sltk_sys.h"
#endif
#include "sltk_ap.h"

#define OS_ERROR_GENERAL -1
#define OS_ERROR_EOF -2
#define OS_ERROR_INVALIDDATA -3
#define OS_ERROR_OUTOFMEM -4
#define OS_ERROR_SYNC -5
struct ogg_codec {
    const int8_t *magic;
    uint8_t magicsize;
    /**
     * Number of expected headers
     */
    int nb_header;
    /**
     * 1 if granule is the start time of the associated packet.
     * 0 if granule is the end time of the associated packet.
     */
    int granule_is_start;

    /**
     * Attempt to process a packet as a header
     * @return 1 if the packet was a valid header,
     *         0 if the packet was not a header (was a data packet)
     *         -1 if an error occurred or for unsupported stream
     */
    int (*header) (void *, int);



    void (*cleanup) (void *s);
};

struct ogg_stream {
    //uint8_t *buf;
    //unsigned int bufsize;
    //unsigned int bufpos;
    //unsigned int pstart;
    unsigned int psize;         //packet size
    //unsigned int pflags;
    //unsigned int pduration;
    //uint32_t serial;
    uint64_t granule;
    //uint64_t start_granule;
    int64_t sync_pos;           ///< file offset of the first page needed to reconstruct the current packet
    int64_t page_pos;           ///< file offset of the current page
    int flags;
    const struct ogg_codec *codec;
    int header;
    int nsegs, segp;
    uint8_t segments[255];
    int incomplete;             ///< whether we're expecting a continuation in the next page
    int page_end;               ///< current packet is the last one completed in the page
    //int keyframe_seek;
    //int got_start;
    //int got_data;   ///< 1 if the stream got some data (non-initial packets), 0 otherwise
    int nb_header;              ///< set to the number of parsed headers
    //int end_trimming; ///< set the number of packets to drop from the end
    //uint8_t *new_metadata;
    //unsigned int new_metadata_size;
    void *private;
    int64_t duration;

};

struct ogg_state {
    uint64_t pos;
    int curidx;
    struct ogg_state *next;
    int nstreams;
    struct ogg_stream streams[1];
};

struct ogg {
    struct ogg_stream streams;
    int nstreams;
    int headers;
    /* if -1 then we need a new page, else the previous page has not beed completely parsed */
    int curidx;
    int64_t page_pos;           ///< file offset of the current page
    struct ogg_state *state;
    /* customrized fields */
    sltk_streamio_t *io;
    int valid_serial;
    int codec_found;
    uint8_t *bs_buf;
    int bs_bufsize;
    int64_t file_size;
    sltk_stream_info_t info;
    int32_t total_time;
    int32_t start_pos;
};

#define OGG_FLAG_CONT 1
#define OGG_FLAG_BOS  2
#define OGG_FLAG_EOS  4

#define OGG_NOGRANULE_VALUE (-1ull)
#define OGG_NOPTS_VALUE -1

#define AV_INPUT_BUFFER_PADDING_SIZE 32

#endif                          /* AVFORMAT_OGGDEC_H */
