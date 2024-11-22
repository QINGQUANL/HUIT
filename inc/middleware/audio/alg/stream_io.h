/* The Alps Open Source Project
 * Copyright (c) 2016 - 2017 Smartlink Technology Co. Ltd. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _STREAM_IO_H_
#define _STREAM_IO_H_

#ifdef _MSC_VER
#include "al_types.h"
#else
#include "sys_types.h"
#endif
#include "sltk_audio.h"

#ifdef __cplusplus
extern "C" {
#endif

enum streamio_flag_e {
    STREAMIO_FLAG_NONE,
    STREAMIO_FLAG_WRITE,
    STREAMIO_FLAG_READ,
};

typedef struct streamio_params_s {
    uint32_t num;
    char *name;
    uint8_t flags;
} streamio_params_t;

typedef struct sltk_streamio_s streamio_t;
typedef streamio_t sltk_streamio_t;

struct sltk_streamio_s {
    int32_t (*read)(void *buf, uint32_t size,
        int32_t count, struct sltk_streamio_s *io);
    int32_t (*write)(void *buf, uint32_t size,
        int32_t count, struct sltk_streamio_s *io);
    int32_t (*seek)(struct sltk_streamio_s *io, int32_t offset, int32_t whence);
    int32_t (*tell)(struct sltk_streamio_s *io);
    int32_t (*open)(sltk_streamio_t *io, streamio_params_t *params);
    int32_t (*close)(sltk_streamio_t *io, void *input);

    int32_t (*flush)(struct sltk_streamio_s *io);
    int32_t (*truncate)(struct sltk_streamio_s *io, int32_t len);
    int32_t (*frame_limit)(struct sltk_streamio_s *io, bool limit, int32_t max_frame_len);
};

typedef streamio_t* (*create_stream_t)(streamtype_t type);
typedef void (*destroy_stream_t)(streamio_t *io);

#ifdef __cplusplus
}
#endif

#endif /* _STREAM_IO_H_ */
