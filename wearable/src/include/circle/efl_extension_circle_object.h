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

#ifndef __EFL_EXTENSION_CIRCLE_OBJECT_H__
#define __EFL_EXTENSION_CIRCLE_OBJECT_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup CAPI_EFL_EXTENSION_CIRCLE_OBJECT_MODULE Efl Extension Circle Object
 * @ingroup CAPI_EFL_EXTENSION_CIRCLE_UI_MODULE
 * @brief Circle object is used for circular design of circular ui-controls.
 *        It can be combined with elementary widgets and be a part of design.
 *        Or it can make itself as a ui-control which is operated by rotary event.
 *
 * @details A Circle object extends elementary widgets in a form of circular design.
 *          It can be rendered either independently or by @ref Eext_Circle_Surface.
 *          When a circle object handle is added with surface parameter of @c NULL,
 *          it is rendered independently along with Evas_Object. On the other hand,
 *          if a circle object is created with surface parameter of @ref Eext_Circle_Surface,
 *          passed surface will render the circle object.
 *
 *          eext_circle_object functions are same with eext_circle_object_item functions
 *          with "default" for item_name parameter. You can check what is the meaning of "default" item
 *          in each circle object's documentation.
 *
 * @{
 */

/**
 * @WEARABLE_ONLY
 * @brief Set the line width of the circle object.
 *
 * @param[in] obj The circle object
 * @param[in] line_width The line width value of the circle object
 *
 * @see eext_circle_line_width_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_line_width_set(Evas_Object *obj, int line_width);

/**
 * @WEARABLE_ONLY
 * @brief Get the line width of a given circle object.
 *
 * @param[in] obj The circle object
 * @return The line width value of the circle object
 *
 * @see eext_circle_line_width_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI int eext_circle_object_line_width_get(const Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Set the angle in degree of a given circle object.
 *
 * @param[in] obj The circle object
 * @param[in] angle The angle in degree of the circle object
 *
 * @see eext_circle_angle_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_angle_set(Evas_Object *obj, double angle);

/**
 * @WEARABLE_ONLY
 * @brief Get the angle in degree of a given circle object.
 *
 * @param[in] obj The circle object
 * @return The angle of circle
 *
 * @see eext_circle_angle_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI double eext_circle_object_angle_get(const Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Set the angle offset of a given circle object.
 *
 * @param[in] obj The circle object
 * @param[in] angle_offset The angle offset value of the circle object
 *
 * @see eext_circle_angle_offset_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_angle_offset_set(Evas_Object *obj, double angle_offset);

/**
 * @WEARABLE_ONLY
 * @brief Get the angle offset of a given circle object.
 *
 * @param[in] obj The circle object
 * @return The angle offset of the circle object
 *
 * @see eext_circle_angle_offset_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI double eext_circle_object_angle_offset_get(const Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Set the minimum and maximum angle for the circle object.
 *
 * @param[in] obj The circle object
 * @param[in] min_angle The minimum angle in degree
 * @param[in] max_angle The maximum angle in degree
 *
 * @see eext_circle_angle_min_max_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_angle_min_max_set(Evas_Object *obj, double min_angle, double max_angle);

/**
 * @WEARABLE_ONLY
 * @brief Get the minimum and maximum angle of a given circle object.
 *
 * @param[in] obj The circle object
 * @param[out] min_angle The pointer to store the minimum angle in degree
 * @param[out] max_angle The pointer to store the minimum angle in degree
 *
 * @see eext_circle_angle_min_max_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_angle_min_max_get(const Evas_Object *obj, double *min_angle, double *max_angle);

/**
 * @WEARABLE_ONLY
 * @brief Set the minimum and maximum value for the circle object.
 *
 * @param[in] obj The circle object
 * @param[in] min_value The minimum value
 * @param[in] max_value The maximum value
 *
 * @see eext_circle_value_min_max_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_value_min_max_set(Evas_Object *obj, double min_value, double max_value);

/**
 * @WEARABLE_ONLY
 * @brief Get the minimum and maximum value of a given circle object.
 *
 * @param[in] obj The circle object
 * @param[out] min_value The pointer to store the minimum value
 * @param[out] max_value The pointer to store the minimum value
 *
 * @see eext_circle_value_min_max_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_value_min_max_get(const Evas_Object *obj, double *min_value, double *max_value);

/**
 * @WEARABLE_ONLY
 * @brief Set the value of a given circle object.
 *
 * @param[in] obj The circle object
 * @param[in] value The value of circle object
 *
 * @see eext_circle_object_value_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_value_set(Evas_Object *obj, double value);

/**
 * @WEARABLE_ONLY
 * @brief Get the value of a given circle object.
 *
 * @param[in] obj The circle object
 * @return The value of circle
 *
 * @see eext_circle_object_value_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI double eext_circle_object_value_get(const Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Set the color of circle line and font in a given circle object.
 *
 * @param[in] obj The circle object
 * @param[in] r The red component of the given color
 * @param[in] g The green component of the given color
 * @param[in] b The blue component of the given color
 * @param[in] a The alpha component of the given color
 *
 * @see eext_circle_color_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_color_set(Evas_Object *obj, int r, int g, int b, int a);

/**
 * @WEARABLE_ONLY
 * @brief Get color of circle line and font color in a given circle object.
 *
 * @param[in] obj The circle object
 * @param[out]   r    The pointer to an integer in which to store the red component
 *               of the color
 * @param[out]   g    The pointer to an integer in which to store the green
 *               component of the color
 * @param[out]   b    The pointer to an integer in which to store the blue component
 *               of the color
 * @param[out]   a    The pointer to an integer in which to store the alpha
 *               component of the color
 *
 * @see eext_circle_color_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_color_get(const Evas_Object *obj, int *r, int *g, int *b, int *a);

/**
 * @WEARABLE_ONLY
 * @brief Set circle radius of a given circle object.
 *
 * @param[in] obj The circle object
 * @param[in] radius The radius value of a given circle object
 *
 * @see eext_circle_radius_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_radius_set(Evas_Object *obj, double radius);

/**
 * @WEARABLE_ONLY
 * @brief Get circle radius of a given circle object.
 *
 * @param[in] obj The circle object
 * @return The radius of circle
 *
 * @see eext_circle_color_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI double eext_circle_object_radius_get(const Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Set the disabled state of the circle object.
 *
 * @details This sets the disabled state of the circle object.
 *          If @c EINA_TRUE, circle object will be disabled
 *          and will not recieve any event. If @c EINA_FALSE,
 *          circle object will be enabled.
 *
 * @param[in] obj The circle object
 * @param[in] disabled The disabled state
 *
 * @see eext_circle_object_disabled_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_disabled_set(Evas_Object *obj, Eina_Bool disabled);

/**
 * @WEARABLE_ONLY
 * @brief Get the disabled state of the circle object.
 *
 * @param[in] obj The circle object
 * @return Disabled state. If @c EINA_TRUE, circle object will be disabled
 *         and will not recieve any event. If @c EINA_FALSE, circle object
 *         will be enabled.
 *
 * @see eext_circle_object_disabled_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eina_Bool eext_circle_object_disabled_get(Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Add a new circle object.
 *
 * @param[in] parent The parent object
 * @param[in] surface The @ref Eext_Circle_Surface object to render the circle object.
 *                    If @c NULL, circle object will be rendered independently.
 *
 * @return A new circle object handle, otherwise
 *         @c NULL if it cannot be created
 *
 * @see @ref CAPI_EFL_EXTENSION_CIRCLE_SURFACE_MODULE
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Evas_Object *eext_circle_object_add(Evas_Object *parent, Eext_Circle_Surface *surface);

/**
 * @WEARABLE_ONLY
 * @brief Set the line width of a given item of circle object.
 *
 * @param[in] obj The circle object
 * @param[in] item_name The item name of circle object
 * @param[in] line_width The line width value of the circle object
 *
 * @see eext_circle_object_item_line_width_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_item_line_width_set(Evas_Object *obj, const char *item_name, int line_width);

/**
 * @WEARABLE_ONLY
 * @brief Get the line width of a given item of circle object.
 *
 * @param[in] obj The circle object
 * @param[in] item_name The item name of circle object
 * @return The line width value of the circle object if it succeeds to find the item,
 *         otherwise -1 if it fails to find the item.
 *
 * @see eext_circle_object_item_line_width_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI int eext_circle_object_item_line_width_get(const Evas_Object *obj, const char *item_name);

/**
 * @WEARABLE_ONLY
 * @brief Set the angle in degree of a given item of circle object.
 *
 * @param[in] obj The circle object
 * @param[in] item_name The item name of circle object
 * @param[in] angle The angle in degree of the circle object
 *
 * @see eext_circle_object_item_angle_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_item_angle_set(Evas_Object *obj, const char *item_name, double angle);

/**
 * @WEARABLE_ONLY
 * @brief Get the angle in degree of a given item of circle object.
 *
 * @param[in] obj The circle object
 * @param[in] item_name The item name of circle object
 * @return The angle of circle
 *
 * @see eext_circle_object_item_angle_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI double eext_circle_object_item_angle_get(const Evas_Object *obj, const char *item_name);

/**
 * @WEARABLE_ONLY
 * @brief Set the angle offset of a given item of circle object.
 *
 * @param[in] obj The circle object
 * @param[in] item_name The item name of circle object
 * @param[in] angle_offset The angle offset value of the circle object
 *
 * @see eext_circle_object_item_angle_offset_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_item_angle_offset_set(Evas_Object *obj, const char *item_name, double angle_offset);

/**
 * @WEARABLE_ONLY
 * @brief Get the angle offset of a given item of circle object.
 *
 * @param[in] obj The circle object
 * @param[in] item_name The item name of circle object
 * @return The angle offset of the circle object
 *
 * @see eext_circle_object_item_angle_offset_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI double eext_circle_object_item_angle_offset_get(const Evas_Object *obj, const char *item_name);

/**
 * @WEARABLE_ONLY
 * @brief Set the minimum and maximum angle for the item of circle object.
 *
 * @param[in] obj The circle object
 * @param[in] item_name The item name of circle object
 * @param[in] min_angle The minimum angle in degree
 * @param[in] max_angle The maximum angle in degree
 *
 * @see eext_circle_object_item_angle_min_max_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_item_angle_min_max_set(Evas_Object *obj, const char *item_name, double min_angle, double max_angle);

/**
 * @WEARABLE_ONLY
 * @brief Get the minimum and maximum angle of a given item of circle object.
 *
 * @param[in] obj The circle object
 * @param[in] item_name The item name of circle object
 * @param[out] min_angle The pointer to store the minimum angle in degree
 * @param[out] max_angle The pointer to store the minimum angle in degree
 *
 * @see eext_circle_object_item_angle_min_max_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_item_angle_min_max_get(const Evas_Object *obj, const char *item_name, double *min_angle, double *max_angle);

/**
 * @WEARABLE_ONLY
 * @brief Set the minimum and maximum value for the item of circle object.
 *
 * @param[in] obj The circle object
 * @param[in] item_name The item name of circle object
 * @param[in] min_value The minimum value
 * @param[in] max_value The maximum value
 *
 * @see eext_circle_object_item_value_min_max_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_item_value_min_max_set(Evas_Object *obj, const char *item_name, double min_value, double max_value);

/**
 * @WEARABLE_ONLY
 * @brief Get the minimum and maximum value of a given item of circle object.
 *
 * @param[in] obj The circle object
 * @param[in] item_name The item name of circle object
 * @param[out] min_value The pointer to store the minimum value
 * @param[out] max_value The pointer to store the minimum value
 *
 * @see eext_circle_object_item_value_min_max_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_item_value_min_max_get(const Evas_Object *obj, const char *item_name, double *min_value, double *max_value);

/**
 * @WEARABLE_ONLY
 * @brief Set the value of a given item of circle object.
 *
 * @param[in] obj The circle object
 * @param[in] item_name The item name of circle object
 * @param[in] value The value of circle object
 *
 * @see eext_circle_object_item_value_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_item_value_set(Evas_Object *obj, const char *item_name, double value);

/**
 * @WEARABLE_ONLY
 * @brief Get the value of a given item of circle object.
 *
 * @param[in] obj The circle object
 * @param[in] item_name The item name of circle object
 * @return The value of circle
 *
 * @see eext_circle_object_item_value_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI double eext_circle_object_item_value_get(const Evas_Object *obj, const char *item_name);

/**
 * @WEARABLE_ONLY
 * @brief Set the color of circle line and font in a given item of circle object.
 *
 * @param[in] obj The circle object
 * @param[in] item_name The item name of circle object
 * @param[in] r The red component of the given color
 * @param[in] g The green component of the given color
 * @param[in] b The blue component of the given color
 * @param[in] a The alpha component of the given color
 *
 * @see eext_circle_object_item_color_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_item_color_set(Evas_Object *obj, const char *item_name, int r, int g, int b, int a);

/**
 * @WEARABLE_ONLY
 * @brief Get color of circle line and font color in a given item of circle object.
 *
 * @param[in] obj The circle object
 * @param[in] item_name The item name of circle object
 * @param[out]   r    The pointer to an integer in which to store the red component
 *               of the color
 * @param[out]   g    The pointer to an integer in which to store the green
 *               component of the color
 * @param[out]   b    The pointer to an integer in which to store the blue component
 *               of the color
 * @param[out]   a    The pointer to an integer in which to store the alpha
 *               component of the color
 *
 * @see eext_circle_object_item_color_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_item_color_get(const Evas_Object *obj, const char *item_name, int *r, int *g, int *b, int *a);

/**
 * @WEARABLE_ONLY
 * @brief Set circle radius of a given item of circle object.
 *
 * @param[in] obj The circle object
 * @param[in] item_name The item name of circle object
 * @param[in] radius The radius value of a given circle object
 *
 * @see eext_circle_object_item_radius_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_item_radius_set(Evas_Object *obj, const char *item_name, double radius);

/**
 * @WEARABLE_ONLY
 * @brief Get circle radius of a given item of circle object.
 *
 * @param[in] obj The circle object
 * @param[in] item_name The item name of circle object
 * @return The radius of the circle object if it succeeds to find the item,
 *         otherwise -1.0 if it fails to find the item.
 *
 * @see eext_circle_object_item_radius_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI double eext_circle_object_item_radius_get(const Evas_Object *obj, const char *item_name);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __EFL_EXTENSION_CIRCLE_H__ */
