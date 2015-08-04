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

#ifndef __EFL_EXTENSION_CIRCLE_OBJECT_SLIDER_H__
#define __EFL_EXTENSION_CIRCLE_OBJECT_SLIDER_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup CAPI_EFL_EXTENSION_CIRCLE_SLIDER_MODULE Efl Extension Circle Slider
 * @ingroup CAPI_EFL_EXTENSION_CIRCLE_UI_MODULE
 * @brief Circle slider is circular designed widget to select a value in a range
 *        by rotary event.
 *
 * @details Circle slider visualizes and utilizess slider as an arc of a circle.
 *          Length of an arc indicates a value in a range set by
 *          eext_circle_object_angle_min_max_set(). If rotary event is activated
 *          by eext_rotary_object_event_activated_set(), Circle slider increases
 *          its value by step value from eext_circle_object_slider_step_set()
 *          when clockwise rotary event is received, vice versa.
 *
 *          This widget emits the following signals:
 *          @li "value,changed": Whenever the slider value is changed by the user.
 *
 *          The available circle object items are as follows:
 *          @li @c "default": Default circle item. It draws slider bar.
 *          @li @c "bg": Background circle item.
 *
 * @{
 */

/**
 * @WEARABLE_ONLY
 * @brief Add a new circle slider object.
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
EAPI Evas_Object *eext_circle_object_slider_add(Evas_Object *parent, Eext_Circle_Surface *surface);

/**
 * @WEARABLE_ONLY
 * @brief Set the step value of the circle slider object.
 *
 * @details This value is used when circle slider object's value is changed
 *          by an event. The value of eext_circle_object_slider is increased/decreased
 *          by the step value.
 *
 * @param[in] obj The circle slider object
 * @param[in] step The step value of the eext_circle_slider object
 *
 * @see eext_circle_object_slider_step_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_slider_step_set(Evas_Object *obj, double step);

/**
 * @WEARABLE_ONLY
 * @brief Get the step value of the circle slider object.
 *
 * @param[in] obj The circle slider object
 * @return The step value of the circle slider object
 *
 * @see eext_circle_object_slider_step_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI double eext_circle_object_slider_step_get(const Evas_Object *obj);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __EFL_EXTENSION_CIRCLE_OBJECT_SLIDER_H__ */
