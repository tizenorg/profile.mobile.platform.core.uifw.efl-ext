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

#ifndef __EFL_EXTENSION_CIRCLE_OBJECT_DATETIME_H__
#define __EFL_EXTENSION_CIRCLE_OBJECT_DATETIME_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup CAPI_EFL_EXTENSION_CIRCLE_DATETIME_MODULE Efl Extension Circle Datetime
 * @ingroup CAPI_EFL_EXTENSION_CIRCLE_UI_MODULE
 * @brief Circle datetime is based on elm_datetime and is operated by rotary event.
 *
 * @details Circle datetime is to utilize selected field in elm_datetime.
 *          If rotary event is activated by eext_rotary_object_event_activated_set(),
 *          circle datetime increases/decreases value of selected field in elm_datetime
 *          by the clockwise/counter clockwise rotary event.
 *
 *          The available circle object items are as follows:
 *          @li @c "default": Default circle item. It draws a marker.
 *
 * @{
 */

/**
 * @WEARABLE_ONLY
 * @brief Add a new circle datetime object.
 *
 * @param[in] datetime The datetime object
 * @param[in] surface The @ref Eext_Circle_Surface object to render the circle object.
 *                    If @c NULL, circle object will be rendered independently.
 * @return A new circle object handle, otherwise @c NULL if it cannot be created
 *
 * @see @ref CAPI_EFL_EXTENSION_CIRCLE_OBJECT_MODULE
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Evas_Object *eext_circle_object_datetime_add(Evas_Object *datetime, Eext_Circle_Surface *surface);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __EFL_EXTENSION_CIRCLE_OBJECT_DATETIME_H__ */
