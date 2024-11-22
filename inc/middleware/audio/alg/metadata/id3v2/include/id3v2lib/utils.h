/*
 * This file is part of the id3v2lib library
 *
 * Copyright (c) 2013, Lorenzo Ruiz
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifndef id3v2lib_utils_h
#define id3v2lib_utils_h

#include "sltk_ap.h"
#include "sys_types.h"
#include "sltk_mem.h"
#include "sltk_sys.h"
// this piece of code makes this header usable under MSVC
// without downloading msinttypes
typedef unsigned short uint16_t;

#include "id3v2lib/types.h"

unsigned int btoi(char* bytes, int size, int offset);
int syncint_decode(int value);


#endif
