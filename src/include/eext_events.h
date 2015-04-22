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

#ifndef __EFL_EXTENSION_EVENTS_H__
#define __EFL_EXTENSION_EVENTS_H__

#include <Elementary.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup EFL_EXTENSION_EVENTS_GROUP Efl Extension Event
 * @ingroup EFL_EXTENSION_GROUP
 * @brief This group provides functionalities to handle back/send key events.
 * @since_tizen 2.3
 *
 * @{
 */

/**
 * @brief Convenient macro function that sends back key events to the popup
 *        to be removed.
 *
 * @details Popup will be removed when it has the back key event with
 *          eext_object_event_callback_add()
 *
 * @param[in] data user data
 * @param[in] obj target object
 * @param[in] event_info event information
 *
 * @see   eext_object_event_callback_add()
 * @since_tizen 2.3
 */
static inline void
eext_popup_back_cb(void *data, Evas_Object *obj, void *event_info)
{
   evas_object_del(obj);
}

/**
 * @brief Convenient macro function that sends back key events to the ctxpopup
 *        to be dismissed.
 *
 * @details Ctxpopup will be dismissed when it has the back key event with
 *          eext_object_event_callback_add()
 *
 * @param[in] data user data
 * @param[in] obj target object
 * @param[in] event_info event information
 *
 * @see   eext_object_event_callback_add()
 * @since_tizen 2.3
 */
static inline void
eext_ctxpopup_back_cb(void *data, Evas_Object *obj, void *event_info)
{
   elm_ctxpopup_dismiss(obj);
}

/**
 * @brief Convenient macro function that sends more key events to the naviframe
 *        top item.
 *
 * @details Ctxpopup will be shown/hidden when naviframe has the more key event
 *          with eext_object_event_callback_add()
 *
 * @param[in] data user data
 * @param[in] obj target object
 * @param[in] event_info event information
 *
 * @see   eext_object_event_callback_add()
 * @since_tizen 2.3
 */
static inline void
eext_naviframe_more_cb(void *data, Evas_Object *obj, void *event_info)
{
   Elm_Object_Item *top = elm_naviframe_top_item_get(obj);
   if (!top) return;
   Evas_Object *more_btn = elm_object_item_part_content_get(top,
                                                            "toolbar_more_btn");
   if (more_btn)
     evas_object_smart_callback_call(more_btn, "clicked", NULL);
   else
     elm_object_item_signal_emit(top, "elm,action,more_event", "");
}

/**
 * @brief Convenient macro function that pop the naviframe item.
 *
 * @details Naviframe will be popped when naviframe has the back key event
 *          with eext_object_event_callback_add()
 *
 * @param[in] data user data
 * @param[in] obj target object
 * @param[in] event_info event information
 *
 * @see   eext_object_event_callback_add()
 * @since_tizen 2.3
 */
static inline void
eext_naviframe_back_cb(void *data, Evas_Object *obj, void *event_info)
{
   elm_naviframe_item_pop(obj);
}

/**
 * Identifier of callbacks to be set for Ea events.
 *
 * @see eext_object_event_callback_add()
 * @see eext_object_event_callback_del()
 * @since_tizen 2.3
 */
typedef enum _Eext_Callback_Type
{
   EEXT_CALLBACK_BACK,    /**< H/W Back Key Event */
   EEXT_CALLBACK_MORE,    /**< H/W More Key Event */
   EEXT_CALLBACK_LAST     /**< Reserved value. Acutally This is not matched to
                            any meaningful behavior. */
} Eext_Callback_Type;

/**
 * @brief Ea event callback function signature.
 * @param[in] data user data
 * @param[in] obj target object
 * @param[in] event_info event information (if the event passes an additional in
              formation.)
 * @see eext_object_event_callback_add()
 * @since_tizen 2.3
 */
typedef void (*Eext_Event_Cb)(void *data, Evas_Object *obj, void *event_info);

/**
 * @brief Delete a callback function from an object.
 *
 * @details This function removes the most recently added callback from the
 *          object @p obj which was triggered by the type @p type and
 *          was calling the function @p func when triggered. If the removal is
 *          successful it will also return the data pointer that was passed to
 *          eext_object_event_callback_add() when the callback was added to
 *          the object. If not successful @c NULl will be returned.
 *
 * @param[in] obj Object to remove a callback from.
 * @param[in] type The type of event that was triggering the callback.
 * @param[in] func The function that was to be called when the event was
 *            triggered
 * @return    data The data pointer that was to be passed to the callback.
 *
 * @see eext_object_event_callback_add()
 * @since_tizen 2.3
 */
EAPI void *eext_object_event_callback_del(Evas_Object *obj, Eext_Callback_Type type, Eext_Event_Cb func);

/**
 * @brief Add (register) a callback function to a given evas object.
 *
 * @details This function adds a function callback to an object when the key
 *          event occurs on object @p obj. The key event on the object is only
 *          triggered when the object is the most top in objects stack and
 *          visible. This means, like the naviframe widget, if your application
 *          needs to have the events based on the view but not focus, you can
 *          use this callback. A callback function must have the Eext_Event_Cb
 *          prototype definition. The first parameter (@p data) in this
 *          definition will have the same value passed to
 *          eext_object_event_callback_add() as the @p data parameter, at
 *          runtime. The second parameter @p obj is the evas object on which
 *          event occurred. Finally, the third parameter @p event_info is a
 *          pointer to a data structure that may or may not be passed to the
 *          callback, depending on the event type that triggered the callback.
 *          This is so because some events don't carry extra context with them,
 *          but others do.
 *
 * @param[in] obj evas object.
 * @param[in] type The type of event that will trigger the callback.
 * @param[in] func The function to be called when the key event is triggered.
 * @param[in] data The data pointer to be passed to @p func.
 *
 * @see eext_object_event_callback_del()
 * @since_tizen 2.3
 */
EAPI void eext_object_event_callback_add(Evas_Object *obj, Eext_Callback_Type type, Eext_Event_Cb func, void *data);

/**
 * @brief Convenient macro function that cancels the selection of the entry.
 *
 * @param[in] data user data
 * @param[in] obj target object
 * @param[in] event_info event information
 *
 * @see   eext_entry_selection_back_event_allow_set()
 * @since_tizen 2.3
 */
static inline void
eext_entry_back_cb(void *data, Evas_Object *obj, void *event_info)
{
   if (elm_entry_selection_get(obj))
     elm_entry_select_none(obj);

   eext_object_event_callback_del(obj, EEXT_CALLBACK_BACK, eext_entry_back_cb);
   evas_object_data_set(obj, "eext_entry", NULL);
}

/**
 * @brief Convenient macro function that registers back key callback for entry.
 *
 * @details If the selection handler should be canceled when back key event is
 *          triggered, then use this API.
 *
 * @param[in] data user data
 * @param[in] obj target object
 * @param[in] event_info event information
 *
 * @see   eext_entry_selection_back_event_allow_set()
 * @since_tizen 2.3
 */
static inline void
eext_entry_selection_start_cb(void *data, Evas_Object *obj, void *event_info)
{
   if (!evas_object_data_get(obj, "eext_entry"))
     {
        evas_object_data_set(obj, "eext_entry", (void *) 1);
        eext_object_event_callback_add(obj, EEXT_CALLBACK_BACK, eext_entry_back_cb,
                                     NULL);
     }
}

/**
 * @brief Convenient macro function that clears back key callback for entry.
 *
 * @param[in] data user data
 * @param[in] obj target object
 * @param[in] event_info event information
 *
 * @see   eext_entry_selection_back_event_allow_set()
 * @see   eext_object_event_callback_add()
 * @since_tizen 2.3
 */
static inline void
eext_entry_selection_cleared_cb(void *data, Evas_Object *obj, void *event_info)
{
   if (evas_object_data_get(obj, "eext_entry"))
     {
        eext_object_event_callback_del(obj, EEXT_CALLBACK_BACK, eext_entry_back_cb);
        evas_object_data_set(obj, "eext_entry", NULL);
     }
}

/**
 * @brief Convenient macro function that handle the back event to cancel the
 *        selection handler of the entry.
 * @details If the selection handler should be canceled (or not) when back key
 *          event is triggered, then use this API.
 * @param[in] obj Entry object.
 * @param[in] allow @c EINA_TRUE allows the back event handling, @c EINA_FALSE
 *              otherwise.
 * @see   eext_object_event_callback_add()
 * @see   eext_object_event_callback_del()
 * @since_tizen 2.3
 */
static inline void
eext_entry_selection_back_event_allow_set(Evas_Object *obj, Eina_Bool allow)
{
   if (allow)
     {
        evas_object_smart_callback_add(obj, "selection,start",
                                       eext_entry_selection_start_cb, NULL);
        evas_object_smart_callback_add(obj, "selection,cleared",
                                       eext_entry_selection_cleared_cb, NULL);
     }
   else
     {
        evas_object_smart_callback_del(obj, "selection,start",
                                       eext_entry_selection_start_cb);
        evas_object_smart_callback_del(obj, "selection,cleared",
                                       eext_entry_selection_cleared_cb);
     }
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __EFL_EXTENSION_EVENTS_H__ */
