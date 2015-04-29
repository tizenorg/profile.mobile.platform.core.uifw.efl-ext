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

#ifndef __EFL_EXTENSION_H__
#define __EFL_EXTENSION_H__

#include "eext_events.h"
typedef uintptr_t tzsh_native_window;

/**
 * @brief Get the native window handle of the Elm_Win object
 *
 * @details This API returns the handle of the tizen shell native window.
 *
 * @param[in] obj object to get the native window handle
 *            The widget type of this object should be elm_win
 *
 * @return    The native window handle
 *
 * @since_tizen 2.4
 */
EAPI tzsh_native_window eext_win_tzsh_native_window_get(const Elm_Win *obj);

/**
 * @brief Sets the alpha window's visual state to opaque state
 *
 * @details This API sets the window's visual state to opaque state even if the window is an alpha window.
 *          This is available only for an alpha window.
 *          If the alpha window sets the visual state to the opaque,
 *          then the window manager could handle it as the opaque window while calculating visibility.
 *
 * @param[in] obj object to get the native window handle
 *            The widget type of this object should be elm_win
 * @param[in] opaque value that indicates whether the window has set a visual state to opaque (@c true = on, @c false = off)
 *
 * @return    @c 1 on success,
              otherwise failure
 *
 * @since_tizen 2.4
 */
EAPI int eext_win_alpha_visual_opaque_set(const Elm_Win *obj, Eina_Bool opaque);

#endif /* __EFL_EXTENSION_H__ */

