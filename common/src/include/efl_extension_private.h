/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __EFL_EXTENSION_PRIVATE_H__
#define __EFL_EXTENSION_PRIVATE_H__

#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <dlog.h>
#include <unistd.h>
#include <fontconfig/fontconfig.h>
#include <fontconfig/fcfreetype.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @internal
 *
 * @ {
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "efl-extension"
#define __CONSTRUCTOR__ __attribute__ ((constructor))
#define __DESTRUCTOR__ __attribute__ ((destructor))

/* eina magic types */
#define EEXT_MAGIC_NONE 0x87657890

typedef unsigned int eext_magic;
#define EEXT_MAGIC                eext_magic __magic

#define EEXT_MAGIC_SET(d, m)      (d)->__magic = (m)
#define EEXT_MAGIC_CHECK(d, m)    ((d) && ((d)->__magic == (m)))
#define EEXT_MAGIC_FAIL(d, m, fn) \
		_eext_magic_fail((d), (d) ? (d)->__magic : 0, (m), (fn));

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __EFL_EXTENSION_PRIVATE_H__ */
