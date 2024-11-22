#ifndef METADATA_H
#define METADATA_H

#include "sltk_math.h"
#include "sltk_sys.h"

typedef enum{
    METADATA_TYPE_ID3V2 = 0,
    METADATA_TYPE_ID3V1,
    METADATA_TYPE_OGG_CONTENT,
    METADATA_TYPE_FLAC_CONTENT,
    METADATA_TYPE_APE_CONTENT,
    METADATA_TYPE_ASF_CONTENT,
    METADATA_TYPE_MP4_CONTENT
}MetaData;

typedef struct _metadata_list metadata_list;
typedef union _frame_internal frame_internal;
typedef struct _metadata_frame metadata_frame;

#include "types.h"
#include "vorbis_ctx.h"
#include "ape_tag.h"
#include "asf_tag.h"
#include "mp4_tag.h"

union _frame_internal
{
    ID3v2_frame* id3v2;
    ID3v1_frame* id3v1;
    VorbisCtxFrame* vorbis_tag;
    ApeTagV2* ape_tag;
    AsfTag*   asf_tag;
    Mp4Tag*   mp4_tag;
    int       ptr_val;
};

struct _metadata_frame
{
    MetaData ctype;
    frame_internal fi;
};

struct _metadata_list
{
    metadata_frame* frame;
    metadata_list* start;
    metadata_list* last;
    metadata_list* next;
};

metadata_frame* new_frame(MetaData ctype);

int delete_frame(metadata_frame* mf);

metadata_list* new_frame_list();

void add_to_list(metadata_list* main, metadata_frame* frame);

void combine_list(metadata_list* front, metadata_list* rear);

void delete_list(metadata_list* main);

#define LOG_TRACE(fmt, arg...) sltk_printf("func(%s)-line(%d): "fmt"\n", __FUNCTION__, __LINE__, ##arg)

static __inline char sltk_toupper(char c)
{
    if (c >= 'a' && c <= 'z')
        c ^= 0x20;
    return c;
}

#define GET_UTF16(val, GET_16BIT, ERROR)\
    val = GET_16BIT;\
    {\
        unsigned int hi = val - 0xD800;\
        if (hi < 0x800) {\
            val = GET_16BIT - 0xDC00;\
            if (val > 0x3FFU || hi > 0x3FFU)\
                ERROR\
            val += (hi<<10) + 0x10000;\
        }\
    }\

#define PUT_UTF8(val, tmp, PUT_BYTE)\
    {\
        int bytes, shift;\
        uint32_t in = val;\
        if (in < 0x80) {\
            tmp = in;\
            PUT_BYTE\
        } else {\
            bytes = ((uint32_t)(sltk_log2((float)in)) + 4) / 5;\
            shift = (bytes - 1) * 6;\
            tmp = (256 - (256 >> bytes)) | (in >> shift);\
            PUT_BYTE\
            while (shift >= 6) {\
                shift -= 6;\
                tmp = 0x80 | ((in >> shift) & 0x3f);\
                PUT_BYTE\
            }\
        }\
    }

#define METADATA_ABANDON_FOR_TAG_NAME (1<<0)
#define METADATA_ABANDON_FOR_TAG_SIZE (1<<1)
#define METADATA_ABANDON_FOR_ALL      (METADATA_ABANDON_FOR_TAG_NAME|METADATA_ABANDON_FOR_TAG_SIZE)

int should_abandon(unsigned char for_reason, char *tag_name, int tag_size);

#endif
