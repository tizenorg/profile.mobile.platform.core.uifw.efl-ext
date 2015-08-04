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

#ifndef __EFL_EXTENSION_CIRCLE_OBJECT_PROGRESSBAR_H__
#define __EFL_EXTENSION_CIRCLE_OBJECT_PROGRESSBAR_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup CAPI_EFL_EXTENSION_CIRCLE_PROGRESSBAR_MODULE Efl Extension Circle Progressbar
 * @ingroup CAPI_EFL_EXTENSION_CIRCLE_UI_MODULE
 * @brief Circle progressbar aims to show the progress status of a given task with circular design.
 *        It visualizes progress status within a range set by eext_circle_object_angle_min_max_set()
 *        as an arc of a circle.
 *
 * @details The available circle object items are as follows:
 *          @li @c "default": Default circle item. It draws progress bar.
 *          @li @c "bg": Progress bar background circle item.
 *
 * @{
 */

/**
 * @WEARABLE_ONLY
 * @brief Add a new circle progressbar to a given surface.
 *
 * @param[in] parent The parent object
 * @param[in] surface The @ref Eext_Circle_Surface object to render the circle object.
 *                    If @c NULL, circle object will be rendered independently.
 * @return A new circle object handle, otherwise @c NULL in case of an error
 *
 * @see @ref CAPI_EFL_EXTENSION_CIRCLE_OBJECT_MODULE
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Evas_Object *eext_circle_object_progressbar_add(Evas_Object *parent, Eext_Circle_Surface *surface);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __EFL_EXTENSION_CIRCLE_OBJECT_PROGRESSBAR_H__ */
