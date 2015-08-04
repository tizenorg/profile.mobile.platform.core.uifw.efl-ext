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

#ifndef __EFL_EXTENSION_INDEX_H__
#define __EFL_EXTENSION_INDEX_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup CAPI_EFL_EXTENSION_INDEX_LAYOUT_MODULE Efl Extension Index Layout
 * @ingroup CAPI_EFL_EXTENSION_COMMON_UI_MODULE
 * @brief Index layout is based on elm_layout with draggable index view, and an item of index
 *        can be selected by rotary event.
 *
 * @details This widget emits the following signals.
 *
 *          @li "changed": The index layout item is changed.
 *          @li "index,show": The index layout item is shown.
 *          @li "index,hide": The index layout item is hidden.
 *
 * @{
 */

/**
 * @WEARABLE_ONLY
 * @brief Add a new index layout to the given parent
 *        Elementary object.
 *
 * @param[in] parent The parent object
 * @return A new index layout handle,
 *         otherwise @c NULL in case of an error
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Evas_Object* eext_index_layout_add(Evas_Object *parent);

/**
 * @WEARABLE_ONLY
 * @brief Append a new item on a given index layout.
 *
 * @param[in] obj The index layout
 * @param[in] text The text under which the item should be indexed
 * @param[in] func The function to call when the item is clicked
 * @param[in] data The item data to set for the index's item
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Object_Item* eext_index_layout_item_append(Evas_Object *obj, const char *text, Evas_Smart_Cb func, void *data);

/**
 * @WEARABLE_ONLY
 * @brief Prepend a new item on a given index layout.
 *
 * @param[in] obj The index layout object
 * @param[in] text The text under which the item should be indexed
 * @param[in] func The function to call when the item is clicked
 * @param[in] data The item data to set for the index's item
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Object_Item* eext_index_layout_item_prepend(Evas_Object *obj, const char *text, Evas_Smart_Cb func, void *data);

/**
 * @WEARABLE_ONLY
 * @brief Insert a new item into the index object before the input index layout item.
 *
 * @param[in] obj The index layout object
 * @param[in] before The index layout item to insert before
 * @param[in] text The text under which the item should be indexed
 * @param[in] priority The priority of index layout item
 * @param[in] func The function to call when the item is clicked
 * @param[in] data The item data to set for the index's item
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Object_Item* eext_index_layout_item_insert_before(Evas_Object *obj, Eext_Object_Item *before, const char *text, Evas_Smart_Cb func, void *data);

/**
 * @WEARABLE_ONLY
 * @brief Insert a new item into the index object after the input index layout item.
 *
 * @param[in] obj The index layout object
 * @param[in] after The index layout item to insert after
 * @param[in] text The text under which the item should be indexed
 * @param[in] priority The priority of index layout item
 * @param[in] func The function to call when the item is clicked
 * @param[in] data The item data to set for the index's item
 * @return A handle to the item added, otherwise @c NULL in case of an error
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Object_Item* eext_index_layout_item_insert_after(Evas_Object *obj, Eext_Object_Item *after, const char *text, Evas_Smart_Cb func, void *data);

/**
 * @WEARABLE_ONLY
 * @brief Remove all items from a given index layout object.
 *
 * @param[in] obj The index layout
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_index_layout_items_clear(Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Delete an item which is the given index layout item.
 *
 * @param[in] item The index layout item
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_index_layout_item_del(Eext_Object_Item *item);

/**
 * @WEARABLE_ONLY
 * @brief Find a given index widget's item using item data.
 *
 * @param[in] obj The index layout
 * @param[in] data The item data pointed by the desired index layout item
 * @return The index layout item handle if found, otherwise @c NULL
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Object_Item* eext_index_layout_item_find(Evas_Object *obj, const void *data);

/**
 * @WEARABLE_ONLY
 * @brief Get the last selected item, for a given index layout object.
 *
 * @param[in] obj The index layout object
 * @return The index layout item handle if selected, otherwise @c NULL
 *
 * @see eext_index_layout_item_selected_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI Eext_Object_Item* eext_index_layout_selected_item_get(Evas_Object *obj);

/**
 * @WEARABLE_ONLY
 * @brief Set the selected state of an item.
 *
 * @remarks Selected item will be highlighted.
 *
 * @param[in] item The index layout item
 * @param[in] selected The selected state
 *
 * @see eext_index_layout_selected_item_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_index_layout_selected_item_set(Eext_Object_Item *item);

/**
 * @WEARABLE_ONLY
 * @brief Set the text to the given index layout item.
 *
 * @param[in] item The index layout item
 * @param[in] text The text under which the item should be indexed
 *
 * @see eext_index_layout_item_text_get()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI void eext_index_layout_item_text_set(Eext_Object_Item *item, const char *text);

/**
 * @WEARABLE_ONLY
 * @brief Get the text (string) set on a given index layout item.
 *
 * @param[in] item The index layout item
 * @return The text string set on @a item
 *
 * @see eext_index_layout_item_text_set()
 *
 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
EAPI const char* eext_index_layout_item_text_get(Eext_Object_Item *item);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __EFL_EXTENSION_INDEX_H__ */
