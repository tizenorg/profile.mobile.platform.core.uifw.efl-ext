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

#ifndef __EFL_EXTENSION_ROTARY_H__
#define __EFL_EXTENSION_ROTARY_H__

#include "Evas.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup CAPI_EFL_EXTENSION_ROTARY_EVENT_MODULE Efl Extension Rotary Event
 * @ingroup CAPI_EFL_EXTENSION_MODULE
 * @brief This module provides functionalities to handle rotary events.
 *
 * @details This rotary object emits the following signals.
 *
 *          @li "rotary,activated": A circle object is activated.
 *          @li "rotary,deactivated": A circle object is deactivated.
 *
 * @{
 *
 */

/**
 * @WEARABLE_ONLY
 * @brief Enumeration of rotary direction
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
typedef enum _Eext_Rotary_Event_Direction
{
   EEXT_ROTARY_DIRECTION_CLOCKWISE, /**< Rotary is rotated clockwise direction */
   EEXT_ROTARY_DIRECTION_COUNTER_CLOCKWISE /**< Rotary is rotated counter clockwise direction */
} Eext_Rotary_Event_Direction;

/**
 * @WEARABLE_ONLY
 * @brief Rotary event information structure
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
typedef struct _Eext_Rotary_Event_Info
{
   Eext_Rotary_Event_Direction direction; /**< Direction of rotary event */
   unsigned int                time_stamp; /**< Timestamp of rotary event */
} Eext_Rotary_Event_Info;

/**
 * @WEARABLE_ONLY
 * @brief Rotary object event callback function signature
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
typedef Eina_Bool (*Eext_Rotary_Event_Cb)(void *data, Evas_Object *obj, Eext_Rotary_Event_Info *info);

/**
 * @WEARABLE_ONLY
 * @brief Rotary event handler function signature
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
typedef Eina_Bool (*Eext_Rotary_Handler_Cb)(void *data, Eext_Rotary_Event_Info *info);

/**
 * @WEARABLE_ONLY
 * @def EEXT_CALLBACK_PRIORITY_BEFORE
 * Slightly more prioritized than default.
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
#define EEXT_CALLBACK_PRIORITY_BEFORE  -100

/**
 * @WEARABLE_ONLY
 * @def EEXT_CALLBACK_PRIORITY_DEFAULT
 * Default callback priority level
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
#define EEXT_CALLBACK_PRIORITY_DEFAULT 0

/**
 * @WEARABLE_ONLY
 * @def EEXT_CALLBACK_PRIORITY_AFTER
 * Slightly less prioritized than default.
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
#define EEXT_CALLBACK_PRIORITY_AFTER   100

/**
 * @WEARABLE_ONLY
 * @typedef Eext_Callback_Priority
 *
 * Callback priority value. Range is -32k - 32k. The lower the number, the
 * higher the priority.
 *
 * @see EEXT_CALLBACK_PRIORITY_AFTER
 * @see EEXT_CALLBACK_PRIORITY_BEFORE
 * @see EEXT_CALLBACK_PRIORITY_DEFAULT
 *
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
typedef short Eext_Callback_Priority;

/**
 * @WEARABLE_ONLY
 * @brief Add (register) a rotary event callback for evas object @a obj.
 *
 * @details This function adds a callback function to object with default
 * priority level, so that when rotary event happens,
 * the callback can be called for that object.
 * A callback function must have the ::Eext_Rotary_Event_Cb prototype
 * definition.
 * @note Be carefull not to add the same callback multiple times, if
 * that is not what you want, because Eext won't check if a callback
 * existed before exactly as the one being registered (and thus, call
 * it more than once on the event, in this case).
 *
 * @param[in] obj The object that will receive rotary event.
 * @param[in] func Callback function.
 * @param[in] data User data to pass to callback function @a func.
 *
 * @return @c EINA_TRUE if the callback is successfully registered.
 *         @c EINA_FALSE otherwise.
 *
 * @see eext_rotary_object_event_callback_priority_add
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eina_Bool eext_rotary_object_event_callback_add(Evas_Object *obj, Eext_Rotary_Event_Cb func, void *data);

/**
 * @WEARABLE_ONLY
 * @brief Add (register) a rotary event callback for evas object @a obj
 * with priority @a priority.
 *
 * @details This function adds a callback function to object with
 * priority level @a priority, so that when rotary event happens,
 * the callback can be called for that object. Except for the priority field,
 * it is exactly the same as @ref eext_rotary_object_event_callback_add.
 *
 * @param[in] obj The object that will receive rotary event.
 * @param[in] priority The priority of the callback, lower values called first.
 * @param[in] func Callback function.
 * @param[in] data User data to pass to callback function @a func.
 *
 * @return @c EINA_TRUE if the callback is successfully registered.
 *         @c EINA_FALSE otherwise.
 *
 * @see eext_rotary_object_event_callback_add
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eina_Bool eext_rotary_object_event_callback_priority_add(Evas_Object *obj, Eext_Callback_Priority priority, Eext_Rotary_Event_Cb func, void *data);

/**
 * @WEARABLE_ONLY
 * @brief Remove registered rotary event callback from evas object @a obj.
 *
 * @param[in] obj The object from which the callback function is removed.
 * @param[in] func Callback function.
 *
 * @return The data which is passed when registers callback function.
 *
 * @see eext_rotary_object_event_callback_add.
 * @see eext_rotary_object_event_callback_priority_add.
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void *eext_rotary_object_event_callback_del(Evas_Object *obj, Eext_Rotary_Event_Cb func);

/**
 * @WEARABLE_ONLY
 * @brief Add (register) a handler for rotary event.
 *
 * @details This function adds a handler function for the rotary event,
 * so that when rotary event happens, the handler can be called.
 * A handler function must have the ::Eext_Rotary_Handler_Cb prototype
 * definition.
 * @note Be carefull not to add the same handler multiple times, if
 * that is not what you want, because Eext won't check if a handler
 * existed before exactly as the one being registered (and thus, call
 * it more than once on the event, in this case).
 *
 * @param[in] func Handler function.
 * @param[in] data User data to pass to handler function @a func.
 *
 * @return @c EINA_TRUE if the handler is successfully registered.
 *         @c EINA_FALSE otherwise.
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eina_Bool eext_rotary_event_handler_add(Eext_Rotary_Handler_Cb func, void *data);

/**
 * @WEARABLE_ONLY
 * @brief Remove registered rotary event handler.
 *
 * @param[in] func Handler function.
 *
 * @return The data which is passed when registers callback function.
 *
 * @see eext_rotary_event_handler_add.
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void *eext_rotary_event_handler_del(Eext_Rotary_Handler_Cb func);

/**
 * @WEARABLE_ONLY
 * @brief Set an object @a obj as activated object.
 *
 * @details This function sets object as an activated object.
 * The activated object and its parent are only objects to receive rotary event.
 * Only one object is registered as activated object. If the new object is
 * registered as activated object, the previous activated object will be
 * de-activated. Set EINA_FALSE to activated parameter will set current activated
 * object as non-activated object also.
 * A signal named "rotary,activated" is emitted to the object when it is set as
 * activated object and similar for "rotary,deactivated" signal.
 * To receive the rotary event, the object must be visible and a callback function
 * must be registered to the object with
 * @ref eext_rotary_object_event_callback_add or
 * @ref eext_rotary_object_event_callback_priority_add.
 *
 * @param[in] obj The object to set as activated object.
 * @param[in] activated Set EINA_TRUE to activate or EINA_FALSE to de-activate object.
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_rotary_object_event_activated_set(Evas_Object *obj, Eina_Bool activated);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // __EFL_EXTENSION_ROTARY_H__
