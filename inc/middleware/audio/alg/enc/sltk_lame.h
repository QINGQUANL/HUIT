/*
 *	Interface to MP3 LAME encoding engine
 *
 *	Copyright (c) 1999 Mark Taylor
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/* $Id: lame.h,v 1.170.2.1 2008/09/14 11:51:49 robert Exp $ */

#ifndef SLTK_LAME_H
#define SLTK_LAME_H

#include "sys_types.h"
#include "sltk_enc_common.h"

typedef struct {
    /* bit rate */
    int brate;
    /* sampling rate in Hz */
    int sample_rate;
    /* channel number */
    int chnum;
} mp3enc_prm_t;

void *mp3enc_open(encoder_init_t * init);
int mp3enc_proc(void *handle);
int mp3enc_cmd(void *handle, uint32_t cmd, uint32_t arg);
void mp3enc_close(void *handle);

#endif                          /* LAME_LAME_H */
