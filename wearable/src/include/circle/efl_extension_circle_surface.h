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

#ifndef __EFL_EXTENSION_CIRCLE_SURFACE_H__
#define __EFL_EXTENSION_CIRCLE_SURFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup CAPI_EFL_EXTENSION_CIRCLE_UI_MODULE Efl Extension Circle UI
 * @ingroup CAPI_EFL_EXTENSION_MODULE
 * @brief This module provides circular ui-controls which are for the devices with circular display.
 */

/**
 * @defgroup CAPI_EFL_EXTENSION_CIRCLE_SURFACE_MODULE Efl Extension Circle Surface
 * @ingroup CAPI_EFL_EXTENSION_CIRCLE_UI_MODULE
 * @brief Surface aims to show one of its connected circle objects.
 *
 * @details The circle surface manages connected circle objects.
 *          When a circle object is chosen to be shown,
 *          the surface shows the circle object and hides the others.
 *          A circle object can be shown and received the rotary event on the surface by
 *          @ref eext_rotary_object_event_activated_set() or @ref evas_object_show().
 *
 *          The circle surface can be created by @ref eext_circle_surface_conformant_add()
 *          and @ref eext_circle_surface_layout_add().
 * @{
 */

/**
 * @WEARABLE_ONLY
 * @typedef Eext_Circle_Surface
 * @brief The structure of circle surface to render a connected circle object.
 *
 * @details The circle surface manages and shows a connected circle object.
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
typedef struct _Eext_Circle_Surface Eext_Circle_Surface;

/**
 * @WEARABLE_ONLY
 * @brief Add a new circle surface to the given comformant.
 *
 * @details The circle surface needs the conformant to be seen in upper layer.
 *
 * @remarks The confom parameter is a conformant object pointer
 *          which is created by @ref elm_conformant_add()
 *          and the conformant's style should not be changed
 *          because in eext_circle_surface_conformant_add()
 *          it will be changed to circle style for circle surface.
 *
 * @param[in] conform The conformant
 * @return An @ref Eext_Circle_Surface handle, otherwise @c NULL in case of an error
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Circle_Surface *eext_circle_surface_conformant_add(Evas_Object *conform);

/**
 * @WEARABLE_ONLY
 * @brief Add a new circle surface to the given layout.
 *
 * @details The circle surface needs the layout to be set to the given layout.
 *
 * @remarks The layout parameter is a layout object pointer which is created by @ref elm_layout_add()
 *          and the style of it should not change because in @ref eext_circle_surface_layout_add()
 *          it will be changed to circle style for circle surface.
 *
 * @param[in] layout The layout
 * @return An @ref Eext_Circle_Surface handle, otherwise @c NULL in case of an error
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Circle_Surface *eext_circle_surface_layout_add(Evas_Object *layout);

/**
 * @WEARABLE_ONLY
 * @brief Add a new circle surface to the given naviframe.
 *
 * @details The circle surface needs the naviframe to be set to the pushed naviframe items.
 *
 * @remarks The naviframe parameter is a naviframe object pointer
 *          which is created by @ref elm_naviframe_add()
 *
 * @param[in] naviframe The naviframe
 * @return An @ref Eext_Circle_Surface handle, otherwise @c NULL in case of an error
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Circle_Surface *eext_circle_surface_naviframe_add(Evas_Object *naviframe);

/**
 * @WEARABLE_ONLY
 * @brief Delete the given circle surface.
 *
 * @param[in] surface The @ref Eext_Circle_Surface
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_surface_del(Eext_Circle_Surface *surface);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __EFL_EXTENSION_CIRCLE_H__ */
