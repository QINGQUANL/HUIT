/*
 * This file is part of the id3v2lib library
 *
 * Copyright (c) 2013, Lorenzo Ruiz
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifndef id3v2lib_id3v2lib_h
#define id3v2lib_id3v2lib_h

#include "sltk_ap.h"
#include "sltk_mem.h"
#include "sltk_sys.h"
#include "stream_io.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "id3v2lib/types.h"
#include "id3v2lib/constants.h"
#include "id3v2lib/header.h"
#include "id3v2lib/frame.h"
#include "id3v2lib/utils.h"

metadata_list* load_tag(sltk_streamio_t * io);

#ifdef __cplusplus
} // end of extern C
#endif

#endif
