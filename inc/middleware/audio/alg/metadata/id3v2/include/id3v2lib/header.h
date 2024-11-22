/*
 * This file is part of the id3v2lib library
 *
 * Copyright (c) 2013, Lorenzo Ruiz
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifndef id3v2lib_header_h
#define id3v2lib_header_h

#include "sltk_ap.h"
#include "stream_io.h"
#include "id3v2lib/types.h"
#include "constants.h"
#include "id3v2lib/utils.h"

int has_id3v2tag(ID3v2_header* tag_header);
int _has_id3v2tag(char* raw_header);
int get_tag_version(ID3v2_header* tag_header);


#endif
