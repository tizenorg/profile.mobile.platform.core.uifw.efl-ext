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

#ifndef __EFL_EXTENSION_CIRCLE_OBJECT_GENLIST_H__
#define __EFL_EXTENSION_CIRCLE_OBJECT_GENLIST_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup CAPI_EFL_EXTENSION_CIRCLE_GENLIST_MODULE Efl Extension Circle Genlist
 * @ingroup CAPI_EFL_EXTENSION_CIRCLE_UI_MODULE
 * @brief Circle genlist is based on elm_genlist and is scrolled by rotary event.
 *
 * @details Circle genlist is to visualize and utilize scroll effect for elm_genlist.
 *          While elm_genlist provides scrollbar with straight horizontal/vertical
 *          movement, circle genlist provides scrollbar with circular movement.
 *          Rotary event can be activated by eext_rotary_object_event_activated_set(),
 *          circle genlist scrolls genlist items upward/leftward responding to
 *          counter clockwise rotary event, and scrolls downward/rightward responding
 *          to clockwise rotary event.
 *
 *          The available circle object items are as follows:
 *          @li @c "default": Default circle item. It draws vertical scroll bar.
 *          @li @c "vertical,scroll,bg": Vertical scroll background circle item.
 *
 * @{
 */

/**
 * @WEARABLE_ONLY
 * @brief Add a new circle genlist object.
 *
 * @param[in] genlist The genlist object
 * @param[in] surface The @ref Eext_Circle_Surface object to render the circle object.
 *                    If @c NULL, circle object will be rendered independently.
 * @return A new circle object handle, otherwise @c NULL if it cannot be created
 *
 * @see @ref CAPI_EFL_EXTENSION_CIRCLE_OBJECT_MODULE
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Evas_Object *eext_circle_object_genlist_add(Evas_Object *genlist, Eext_Circle_Surface *surface);

/**
 * @WEARABLE_ONLY
 * @brief Set the scrollbar visibility policy.
 *
 * @details This sets the scrollbar visibility policy for a given scroller.
 *          @c ELM_SCROLLER_POLICY_AUTO means the scrollbar is made visible if it is
 *          needed, and otherwise kept hidden. @c ELM_SCROLLER_POLICY_ON turns it on all
 *          the time, and @c ELM_SCROLLER_POLICY_OFF always keeps it off. This applies
 *          respectively for the vertical scrollbars.
 *
 * @param[in] obj The circle genlist object
 * @param[in] policy_h The horizontal scrollbar policy
 * @param[in] policy_v The vertical scrollbar policy
 *
 * @see eext_circle_object_genlist_scroller_policy_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_genlist_scroller_policy_set(Evas_Object *obj, Elm_Scroller_Policy policy_h, Elm_Scroller_Policy policy_v);

/**
 * @WEARABLE_ONLY
 * @brief Get the scrollbar visibility policy.
 *
 * @param[in] obj The circle genlist object
 * @param[out] policy_h The horizontal scrollbar policy
 * @param[out] policy_v The vertical scrollbar policy
 *
 * @see eext_circle_object_genlist_scroller_policy_set();
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_circle_object_genlist_scroller_policy_get(const Evas_Object *obj, Elm_Scroller_Policy *policy_h, Elm_Scroller_Policy *policy_v);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __EFL_EXTENSION_CIRCLE_OBJECT_GENLIST_H__ */
