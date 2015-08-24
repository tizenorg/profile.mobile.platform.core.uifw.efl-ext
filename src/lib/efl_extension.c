/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#include "efl_extension.h"
#include "efl_extension_private.h"

/*===========================================================================*
 *                                 Local                                     *
 *===========================================================================*/

static const char *
_magic_string_get(eext_magic m)
{
	switch (m) {
	case EEXT_MAGIC_NONE:
		return "None (Freed Object)";

	default:
		return "<UNKNOWN>";
     }
}

__CONSTRUCTOR__ static void
eext_mod_init(void)
{
}

__DESTRUCTOR__ static void
eext_mod_shutdown(void)
{
}


/*===========================================================================*
 *                                Global                                     *
 *===========================================================================*/

void
_eext_magic_fail(const void *d, eext_magic m, eext_magic req_m, const char *fname)
{
	LOGE("\n*** MAGIC FAIL (%s) ***\n", fname);

	if (!d)
		LOGE("  Input handle pointer is NULL!");
	else if (m == EEXT_MAGIC_NONE)
		LOGE("  Input handle has already been freed!");
	else if (m != req_m)
		LOGE("  Input handle is wrong type\n"
		    "    Expected: %08x - %s\n"
		    "    Supplied: %08x - %s",
		    (unsigned int)req_m, _magic_string_get(req_m),
		    (unsigned int)m, _magic_string_get(m));

   if (getenv("EEXT_ERROR_ABORT")) abort();
}

/*===========================================================================*
 *                                  API                                      *
 *===========================================================================*/

EAPI Eina_Bool
eext_win_keygrab_set(Elm_Win *obj, const char *key)
{
   Eina_Bool ret = EINA_FALSE;
   ret = elm_win_keygrab_set(obj, key, 0, 0, 0, ELM_WIN_KEYGRAB_TOPMOST);
   if (!ret)
     LOGE("  Can't set keygrab of [%s].", key);
   return ret;
}

EAPI Eina_Bool
eext_win_keygrab_unset(Elm_Win *obj, const char *key)
{
   Eina_Bool ret = EINA_FALSE;
   ret = elm_win_keygrab_unset(obj, key, 0, 0);
   if (!ret)
     LOGE("  Can't unset keygrab of [%s].", key);
   return ret;
}
