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

#ifndef __EFL_EXTENSION_MORE_OPTION_LAYOUT_H__
#define __EFL_EXTENSION_MORE_OPTION_LAYOUT_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup CAPI_EFL_EXTENSION_MORE_OPTION_LAYOUT_MODULE Efl Extension More Option Layout
 * @ingroup CAPI_EFL_EXTENSION_COMMON_UI_MODULE
 * @brief More option layout contains items whose visibility can be toggled.
 *
 * @details More option layout is based on elm_panel. It is visible in opened state, and
 *          partially hidden in other than opened state. To change the opened state, scroll
 *          visible part of the more option layout object when its scrollable state is
 *          @c EINA_TRUE, which is default, or use eext_more_option_layout_opened_set().
 *          can be selected by rotary event.
 *
 *          This widget emits the following signals:
 *          @li "item,selected": When the user selected an item.
 *          @li "more,option,opened": When the user set opened state as @c EINA_TRUE.
 *          @li "more,option,closed": When the user set opened state as @c EINA_FALSE.
 *          @li "more,option,hold": When the more option layout is not draggable.
 *          @li "more,option,unhold": When the more option layout is draggable.
 *
 * @{
 */

/**
 * @brief Enumeration of More Option Layout Diection type
 */
typedef enum
{
   EEXT_MORE_OPTION_LAYOUT_DIRECTION_TOP,  /**< More option layout (dis)appears from the top */
   EEXT_MORE_OPTION_LAYOUT_DIRECTION_BOTTOM,  /**< More option layout (dis)appears from the bottom */
   EEXT_MORE_OPTION_LAYOUT_DIRECTION_LEFT,  /**< More option layout (dis)appears from the left */
   EEXT_MORE_OPTION_LAYOUT_DIRECTION_RIGHT  /**< More option layout (dis)appears from the right */
} Eext_More_Option_Layout_Direction;

/**
 * @WEARABLE_ONLY
 * @brief Add a new more option layout.
 *
 * @param[in] parent The parent object
 * @return A new more option layout handle, otherwise @c NULL if it cannot be created
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Evas_Object * eext_more_option_layout_add(Evas_Object *parent);

/**
 * @WEARABLE_ONLY
 * @brief Set the direction of more option layout.
 *
 * @param[in] obj The more option layout
 * @param[in] direction The direction of more option layout
 *
 * @see eext_more_option_layout_direction_get()
 *      @ref Eext_More_Option_Layout_Direction
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_more_option_layout_direction_set(Evas_Object *obj, Eext_More_Option_Layout_Direction direction);

/**
 * @WEARABLE_ONLY
 * @brief Get @ref Eext_More_Option_Layout_Direction.
 *
 * @param[in] obj The more option layout
 * @return A direction of more option layout
 *
 * @see eext_more_option_layout_direction_set()
 *      @ref Eext_More_Option_Layout_Direction
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_More_Option_Layout_Direction eext_more_option_layout_direction_get(const Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Append a new item on a more option layout.
 *
 * @param[in] obj The more option layout
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @see @ref Eext_Object_Item
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Object_Item * eext_more_option_layout_item_append(Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief prepend a new item on a more option layout.
 *
 * @param[in] obj The more option layout
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @see @ref Eext_Object_Item
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Object_Item * eext_more_option_layout_item_prepend(Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Insert a new item on a more option layout after more option layout item @a after.
 *
 * @param[in] obj The more option layout
 * @param[in] after The more option layout item to insert after
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @see @ref Eext_Object_Item
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Object_Item * eext_more_option_layout_item_insert_after(Evas_Object *obj, Eext_Object_Item *after);

/**
 * @WEARABLE_ONLY
 * @brief Insert a new item on a more option layout before more option layout item @a before.
 *
 * @param[in] obj The more option layout
 * @param[in] before The more option layout item to insert before
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @see @ref Eext_Object_Item
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Object_Item * eext_more_option_layout_item_insert_before(Evas_Object *obj, Eext_Object_Item *before);

/**
 * @WEARABLE_ONLY
 * @brief Delete an item which is the given more option layout item.
 *
 * @param[in] item The more option layout item
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_more_option_layout_item_del(Eext_Object_Item *item);

/**
 * @WEARABLE_ONLY
 * @brief Remove all items from a given more option layout object.
 *
 * @param[in] obj The more option layout
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_more_option_layout_items_clear(Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Set the text of a more option layout object.
 *
 * @param[in] item The more option layout item
 * @param[in] part_name The text part name to set (@c NULL for the default part)
 * @param[in] text The new text of the part
 *
 * @see eext_more_option_layout_item_part_text_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_more_option_layout_item_part_text_set(Eext_Object_Item *item, const char *part_name, const char *text);

/**
 * @WEARABLE_ONLY
 * @brief Set the text of a more option layout object.
 *
 * @param[in] item The more option layout item
 * @param[in] part_name The text part name to Get (@c NULL for the default part)
 * @return The text of the part, otherwise @c NULL for any error
 *
 * @see eext_more_option_layout_item_part_text_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI const char * eext_more_option_layout_item_part_text_get(const Eext_Object_Item *item, const char *part_name);

/**
 * @WEARABLE_ONLY
 * @brief Set the translatable text of a more option layout object.
 *
 * @param[in] item The more option layout item
 * @param[in] part_name The text part name to set (@c NULL for the default part)
 * @param[in] domain The name of the domain which contains translatable text
 * @param[in] text ID of the new translatable text
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_more_option_layout_item_domain_translatable_part_text_set(Eext_Object_Item *item, const char *part_name, const char *domain, const char *text);

/**
 * @WEARABLE_ONLY
 * @def eext_more_option_layout_item_translatable_part_text_set
 * @brief Convenient macro for eext_more_option_layout_item_domain_translatable_part_text_set
 *
 * @see eext_more_option_layout_item_domain_translatable_part_text_set
 *
 * @param[in] item The more option layout item
 * @param[in] part_name The text part name to set (@c NULL for the default part)
 * @param[in] text ID of the new translatable text
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
#define eext_more_option_layout_item_translatable_part_text_set(item, part_name, text)   eext_more_option_layout_item_domain_translatable_part_text_set((item), (part_name), NULL, (text))

/**
 * @WEARABLE_ONLY
 * @brief Set the content at a part of a given container widget.
 *
 * @remarks The more option layout item may hold child objects as content at given parts.
 *          This sets new content to a given part. If any object is already set as a content
 *          object in the same part, the previous object is deleted automatically
 *          with this call.
 *
 * @param[in] item The more option layout item
 * @param[in] part_name The more option layout item's part name to set
 * @param[in] content The new content for that part
 *
 * @see eext_more_option_layout_item_part_content_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_more_option_layout_item_part_content_set(Eext_Object_Item *item, const char *part_name, Evas_Object *content);

/**
 * @WEARABLE_ONLY
 * @brief Get the content at a part of a given container widget.
 *
 * @param[in] item The more option layout item
 * @param[in] part_name The more option layout item's part name to set
 * @return The content of the object at the given part, otherwise @c NULL in case of an error
 *
 * @see eext_more_option_layout_item_part_content_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Evas_Object * eext_more_option_layout_item_part_content_get(const Eext_Object_Item *item, const char *part_name);

/**
 * @WEARABLE_ONLY
 * @brief Set the opened state of the more option layout.
 *
 * @param[in] obj The more option layout object
 * @param[in] opened If @c EINA_TRUE, the more option layout runs the animation to appear,
 *                   If @c EINA_FALSE, the more option layout runs the animation to disappear
 *
 * @see eext_more_option_layout_opened_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_more_option_layout_opened_set(Evas_Object *obj, Eina_Bool opened);

/**
 * @WEARABLE_ONLY
 * @brief Get the opened state of the more option layout object.
 *
 * @param[in] obj The more option layout object
 * @return The opened state of more option layout.
 *
 * @see eext_more_option_layout_opened_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eina_Bool eext_more_option_layout_opened_get(Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Enables or disables scrolling in the more option layout object.
 *
 * @remarks Scrollable is EINA_TRUE by default.
 *
 * @param[in] obj The more option layout object
 * @param[in] scrollable The scrollable state. If @c EINA_TRUE it is scrollable,
                         otherwise @c EINA_FALSE
 *
 * @see eext_more_option_layout_scrollable_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_more_option_layout_scrollable_set(Evas_Object *obj, Eina_Bool scrollable);

/**
 * @WEARABLE_ONLY
 * @brief Get scrollable state of the more option layout object.
 *
 * @param[in] obj The more option layout object
 * @return If @c EINA_TRUE it is scrollable, otherwise @c EINA_FALSE
 *
 * @see eext_more_option_layout_scrollable_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eina_Bool eext_more_option_layout_scrollable_get(const Evas_Object *obj);

/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif /* __EFL_EXTENSION_MORE_OPTION_LAYOUT_H__ */
