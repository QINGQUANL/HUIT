/*
 * This file is part of the id3v2lib library
 *
 * Copyright (c) 2013, Lorenzo Ruiz
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifndef id3v2lib_frame_h
#define id3v2lib_frame_h

#include "id3v2lib/types.h"
#include "constants.h"
#include "sltk_ap.h"
#include "sys_types.h"
#include "sltk_mem.h"
#include "sltk_sys.h"
#include "stream_io.h"
#include "sltk_codecs_common.h"
#include "sltk_metadata.h"

metadata_frame* id3v2_parse_frame(sltk_streamio_t * io, int version);
int get_frame_type(char* frame_id);

#endif
