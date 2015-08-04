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

#include "efl_extension.h"
#include "efl_extension_private.h"
#include "circle/efl_extension_circle_private.h"
#include "common/efl_extension_common_private.h"

#define EEXT_INDEX_LAYOUT_TYPE "Eext_Index_Layout"
#define EEXT_INDEX_LAYOUT_DATA_GET(circle_obj, data) \
   Eext_Index_Layout_Data *data = NULL; \
   if (circle_obj && circle_obj->widget_data && \
       circle_obj->widget_type && !strcmp(circle_obj->widget_type, EEXT_INDEX_LAYOUT_TYPE)) \
      data = (Eext_Index_Layout_Data *)circle_obj->widget_data; \
   if (!data)

static const char SIG_CHANGED[] = "changed";
static const char SIG_INDEX_SHOW[] = "index,show";
static const char SIG_INDEX_HIDE[] = "index,hide";

typedef enum {
   APPEND,
   PREPEND,
   BEFORE,
   AFTER,
} Index_Layout_Item_Add_Type;

static void _eext_index_layout_render_pre_cb(Eext_Circle_Object *obj);

static void
_eext_index_layout_del_cb(Eext_Circle_Object *obj)
{
   Eext_Index_Layout_Item *item;
   EEXT_INDEX_LAYOUT_DATA_GET(obj, data) return;

   if (data->item_list)
     EINA_LIST_FREE(data->item_list, item)
       eina_stringshare_del(item->letter);
   data->item_list = NULL;

   if (data->hide_timer)
     {
        ecore_timer_del(data->hide_timer);
        data->hide_timer = NULL;
     }

   free(data);
}

static void
_omit_calc(Eext_Circle_Object *obj, Eina_List *letter_list, int start_offset, int end_offset)
{
   int i, j, count;
   int max_group_num, num_of_extra_items, g, size, max_item;
   int *omit_info, *group_pos;
   Eext_Index_Layout_Item *index_item;
   Eext_Circle_Object_Item *item;

   EEXT_INDEX_LAYOUT_DATA_GET(obj, data) return;

   if (data->max_item > eina_list_count(data->current_list)) max_item = eina_list_count(data->current_list);
   else max_item = data->max_item;

   max_group_num = (max_item - start_offset - end_offset - 1) / 2;
   num_of_extra_items = eina_list_count(letter_list) - max_item + start_offset + end_offset;
   size = num_of_extra_items / max_group_num;

   if (data->omit_list)
     {
        eina_list_free(data->omit_list);
        data->omit_list = NULL;
     }

   data->item_index = (int *)calloc(sizeof(int), eina_list_count(data->current_list));

   omit_info = (int *)calloc(sizeof(int), max_item);
   group_pos = (int *)calloc(sizeof(int), max_group_num);

   if (num_of_extra_items > max_group_num)
     {
        g = start_offset + 1;
        for (i = 0;  i < max_group_num; i++)
          {
              group_pos[i] = g;
              g += 2;
          }
     }
   else
     {
         size = max_item / (num_of_extra_items + 1);
         g = start_offset + size;
        for (i = 0;  i < num_of_extra_items; i++)
          {
              group_pos[i] = g;
              g += size;
          }
     }

   for (i = 0; i < max_item ; i++)
     omit_info[i] = 1;
   for (i = 0; i < num_of_extra_items; i++)
     omit_info[group_pos[i % max_group_num]]++;

   count = 0;
   for (i = 0; i < max_item; i++)
     {
        if (omit_info[i] > 1)
          {
             Eina_List *internal_list;

             item = _eext_circle_object_item_new();
             item->radius = 158;
             _eext_circle_object_item_text_set(item, "*", NULL);

             for (j = 0 ; j < omit_info[i] ; j++)
               {
                  index_item = eina_list_nth(data->current_list, count);
                  index_item->item = item;

                  internal_list = eina_list_append(internal_list, index_item);

                  data->item_index[count] = i;
                  count++;

                  if (!item->data)
                    item->data = internal_list;
               }

             internal_list = NULL;
          }
        else
          {
             item = _eext_circle_object_item_new();
             item->radius = 162;
             index_item = eina_list_nth(data->current_list, count);
             _eext_circle_object_item_text_set(item, index_item->letter, NULL);
             item->data = index_item;

             data->item_index[count] = i;
             index_item = eina_list_nth(data->current_list, count);
             count++;
          }

        _eext_circle_object_item_name_set(item, "omit_list_item");

        _eext_circle_object_item_append(obj, item);
        data->omit_list = eina_list_append(data->omit_list, item);
     }

   data->max_index = eina_list_count(data->omit_list);

   free(omit_info);
   free(group_pos);
}

static void
_eext_index_layout_update_item_list(Eext_Circle_Object *obj)
{
   int exception_count;
   Eina_List *l, *letter0_list = NULL;
   Eext_Index_Layout_Item *item;

   EEXT_INDEX_LAYOUT_DATA_GET(obj, data) return;

   exception_count = 0;
   if (data->current_list)
     {
        eina_list_free(data->current_list);
        data->current_list = NULL;
     }

   if (data->index_priority == 0)
     {
        EINA_LIST_FOREACH(data->item_list, l, item)
          {
             if (item->priority == -1)
               {
                  data->current_list = eina_list_append(data->current_list, item);
                  exception_count++;
               }

             data->default_item_count = exception_count;
          }

        EINA_LIST_FOREACH(data->item_list, l, item)
           if (item->priority == 0)
             {
                letter0_list = eina_list_append(letter0_list, item->letter);
                data->current_list = eina_list_append(data->current_list, item);
             }

        if (data->second_list_exist)
          {
             EINA_LIST_REVERSE_FOREACH(data->item_list, l, item)
               {
                  if (item->priority == 1)
                    {
                       data->current_list = eina_list_append(data->current_list, item);
                       break;
                    }
               }
             //This is not need in current UX.
             //_omit_calc(obj, letter0_list, letter1_list, exception_count);
          }
        else
          //The count depands on UX.
          if (eina_list_count(data->current_list) >= 72)
            _omit_calc(obj, letter0_list, exception_count, 0);
     }
   else
     {
        EINA_LIST_FOREACH(data->item_list, l, item)
          {
             if (item->priority == -1)
               {
                  data->current_list = eina_list_append(data->current_list, item);
                  exception_count++;
               }
          }
        EINA_LIST_FOREACH(data->item_list, l, item)
           if (item->priority == 0)
             {
                data->current_list = eina_list_append(data->current_list, item);
                exception_count++;
                break;
             }

        EINA_LIST_REVERSE_FOREACH(data->item_list, l, item)
           if (item->priority == 1)
             data->current_list = eina_list_append(data->current_list, item);

        //This is not need in current UX.
        //_omit_calc(obj, letter1_list, NULL, exception_count);
     }

   if (!data->omit_list)
     data->max_index = eina_list_count(data->current_list);
}

static void
_eext_index_layout_priority_change(Eext_Circle_Object *obj)
{
   const char *letter;
   int index = 0;
   Eina_List *l;
   Eext_Index_Layout_Item *item;

   EEXT_INDEX_LAYOUT_DATA_GET(obj, data) return;

   item = eina_list_nth(data->current_list, data->highlighted_index);
   letter = item->letter;

   _eext_index_layout_update_item_list(obj);
   EINA_LIST_FOREACH(data->current_list, l, item)
     {
        if (!strcmp(letter, item->letter))
          {
             data->highlighted_index = index;
             obj->render_pre_cb = _eext_index_layout_render_pre_cb;
             return;
          }
        index++;
     }

   obj->render_pre_cb = _eext_index_layout_render_pre_cb;
}

static void
_eext_index_layout_text_update(Eext_Circle_Object *circle_obj)
{
   Eina_List *l;
   char buf[PATH_MAX] = {0,};
   Eext_Index_Layout_Item *item;
   int index = 0;

   EEXT_INDEX_LAYOUT_DATA_GET(circle_obj, data) return;

   if ((data->index_priority == 0) &&
       (data->highlighted_index == eina_list_count(data->current_list) - 1) &&
       (data->second_list_exist))
     {
        data->index_priority = 1;
        _eext_index_layout_priority_change(circle_obj);
     }

   if ((data->index_priority == 1) &&
       (data->highlighted_index == data->default_item_count))
     {
        data->index_priority = 0;
        _eext_index_layout_priority_change(circle_obj);
     }

   if (data->current_list)
     {
        //Update center layout text and button text.
        EINA_LIST_FOREACH(data->current_list, l, item)
          {
             if (item->selected)
               {
                  snprintf(buf, sizeof(buf), "%s", item->letter);
                  data->highlighted_index = index;
                  break;
               }
             index++;
          }

        if (!strlen(buf))
          {
             item = eina_list_nth(data->current_list, data->highlighted_index);
             snprintf(buf, sizeof(buf), "%s", item->letter);
          }

        elm_object_part_text_set(data->index, "elm.text", buf);
        elm_object_text_set(data->cue, buf);

        //For re-render.
        _eext_circle_object_changed(circle_obj);
     }
   else
     {
        snprintf(buf, sizeof(buf), " ");
        elm_object_part_text_set(data->index, "elm.text", buf);
        elm_object_text_set(data->cue, buf);
     }
}

static void
_eext_index_layout_render_pre_cb(Eext_Circle_Object *obj)
{
   Eina_List *l, *item_l;
   Eext_Circle_Object_Item *item;
   Eext_Index_Layout_Item *index_item;
   int index = 0;
   Eina_Bool bg_exist = EINA_FALSE;

   EEXT_INDEX_LAYOUT_DATA_GET(obj, data) return;
   if (data->panel_hide)
     _eext_circle_object_hide(obj);

   _eext_index_layout_update_item_list(obj);
   _eext_index_layout_text_update(obj);
   eina_list_free(obj->text_items);
   obj->text_items = NULL;

   //Text item initialization.
   EINA_LIST_FOREACH(obj->items, l, item)
     {
        if (item->text)
          _eext_circle_object_item_font_size_set(item, 0);
     }

   if (data->omit_list)
     EINA_LIST_FOREACH(data->omit_list, l, item)
       {
          if (item->text)
            {
               obj->text_items = eina_list_append(obj->text_items, item);
               _eext_circle_object_item_font_size_set(item, 22);

               if (data->item_index[data->highlighted_index] == index)
                 {
                    _eext_circle_object_item_color_set(item, 54, 154, 237, 255);

                    index_item = eina_list_nth(data->current_list, data->highlighted_index);
                    evas_object_smart_callback_call(data->main_ly, SIG_CHANGED, index_item);

                    if (index_item->func)
                      index_item->func((void *)index_item->base.data,
                                       index_item->base.obj, index_item);
                 }
               else
                 {
                    _eext_circle_object_item_color_set(item, 94, 94, 94, 255);
                 }

               _eext_circle_object_item_angle_set(item, 360 / data->max_index * index);
               index++;
            }
       }

   else
     EINA_LIST_FOREACH(data->current_list, l, index_item)
       {
          EINA_LIST_FOREACH(obj->items, item_l, item)
            {
               if (!strcmp(_eext_circle_object_item_name_get(item), "index_bg"))
                 bg_exist = EINA_TRUE;

               if (item->text)
                 {
                    if (index == data->max_index) break;
                    if (!strcmp(item->text, index_item->letter))
                      {
                         obj->text_items = eina_list_append(obj->text_items, item);
                         _eext_circle_object_item_font_size_set(item, 22);
                         if (index == data->highlighted_index)
                           {
                              _eext_circle_object_item_color_set(item, 54, 154, 237, 255);

                              evas_object_smart_callback_call(data->main_ly, SIG_CHANGED, item->data);

                              if (index_item->func)
                                index_item->func((void *)index_item->base.data,
                                                 index_item->base.obj, index_item);
                           }
                         else
                           {
                              _eext_circle_object_item_color_set(item, 94, 94, 94, 255);
                           }

                         _eext_circle_object_item_angle_set(item, 360 / data->max_index * index);
                         index++;
                      }
                 }
            }
       }

   if (!bg_exist)
     {
        item = _eext_circle_object_item_new();
        _eext_circle_object_item_radius_set(item, 169);
        _eext_circle_object_item_line_width_set(item, 18);
        _eext_circle_object_item_color_set(item, 38, 38, 38, 255);
        _eext_circle_object_item_angle_set(item, 360.0);
        _eext_circle_object_item_name_set(item, "index_bg");
        _eext_circle_object_item_prepend(obj, item);
     }

   obj->render_pre_cb = NULL;
}

static void
_eext_index_layout_highlighted_item_changed(Evas_Object *obj, int unhighlighted_index)
{
   Eext_Circle_Object_Item *item;
   Eext_Index_Layout_Item *index_item;
   int omit_unhigh_index = 0, omit_high_index = 0;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return;
   EEXT_INDEX_LAYOUT_DATA_GET(circle_obj, data) return;

   if (data->omit_list)
     {
        omit_unhigh_index = data->item_index[unhighlighted_index];
        omit_high_index = data->item_index[data->highlighted_index];

        item = eina_list_nth(circle_obj->text_items, omit_unhigh_index);
        _eext_circle_object_item_color_set(item, 94, 94, 94, 255);
        index_item = eina_list_nth(data->current_list, unhighlighted_index);
     }
   else
     {
        item = eina_list_nth(circle_obj->text_items, unhighlighted_index);
        _eext_circle_object_item_color_set(item, 94, 94, 94, 255);
        index_item = item->data;
     }

   index_item->selected = EINA_FALSE;

   if (data->omit_list)
     {
        item = eina_list_nth(circle_obj->text_items, omit_high_index);
        _eext_circle_object_item_color_set(item, 54, 154, 237, 255);
        index_item = eina_list_nth(data->current_list, data->highlighted_index);
     }
   else
     {
        item = eina_list_nth(circle_obj->text_items, data->highlighted_index);
        _eext_circle_object_item_color_set(item, 54, 154, 237, 255);
        index_item = item->data;
     }

   index_item->selected = EINA_TRUE;

   evas_object_smart_callback_call(data->main_ly, SIG_CHANGED, index_item);

   if (index_item->func)
     index_item->func((void *)index_item->base.data, index_item->base.obj, index_item);

   _eext_index_layout_text_update(circle_obj);
}

static void
_index_move(Eext_Circle_Object *circle_obj, Eext_Circle_Object_Item *item, Eina_Bool inc)
{
   int move_value;
   EEXT_INDEX_LAYOUT_DATA_GET(circle_obj, data) return;

   if (inc)
     {
        data->highlighted_index++;
        move_value = -1;
     }
   else
     {
        data->highlighted_index--;
        move_value = 1;
     }

   _eext_index_layout_highlighted_item_changed(data->main_ly, data->highlighted_index + move_value);
}

static Eina_Bool
_eext_index_timer_cb(void *data)
{
   Eext_Index_Layout_Data *widget_data = data;
   if (!widget_data) return ECORE_CALLBACK_CANCEL;

   if (widget_data->hide_timer)
     {
        ecore_timer_del(widget_data->hide_timer);
        widget_data->hide_timer = NULL;
     }

   elm_panel_toggle(widget_data->panel);
   return ECORE_CALLBACK_CANCEL;
}

static void _eext_index_back_cb(void *data, Evas_Object *obj, void *event_info)
{
   elm_panel_toggle(obj);
}


static Eina_Bool
_rotary_changed_cb(void *data, Evas_Object *obj, Eext_Rotary_Event_Info* info)
{
   Eext_Circle_Object_Item *item;
   Eext_Circle_Object *circle_obj = data;

   if (!circle_obj->items) return EINA_FALSE;
   EEXT_INDEX_LAYOUT_DATA_GET(circle_obj, widget_data) return EINA_FALSE;

   if (info->direction == EEXT_ROTARY_DIRECTION_CLOCKWISE)
     {
        if (widget_data->max_index != widget_data->highlighted_index + 1)
          _index_move(circle_obj, item, EINA_TRUE);
     }
   else
     {
        if (widget_data->highlighted_index != 0)
          _index_move(circle_obj, item, EINA_FALSE);
     }

   if (widget_data->hide_timer)
     {
        ecore_timer_del(widget_data->hide_timer);
        widget_data->hide_timer = NULL;
     }

   //Panel will be hide after gets last rotary event.
   widget_data->hide_timer = ecore_timer_add(2.0, _eext_index_timer_cb, widget_data);

   return EINA_TRUE;
}

static void
_panel_scroll_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eext_Circle_Object *circle_obj = data;
   EEXT_INDEX_LAYOUT_DATA_GET(circle_obj, widget_data) return;
   Elm_Panel_Scroll_Info *ev = event_info;

   if (ev->rel_x == 0) elm_layout_signal_emit(widget_data->main_ly, "index,state,content,show", "elm");
   else elm_layout_signal_emit(widget_data->main_ly, "index,state,content,hide,without,transition", "elm");

   if (widget_data->hide_timer)
     {
        ecore_timer_del(widget_data->hide_timer);
        widget_data->hide_timer = NULL;
     }

   if (ev->rel_x == 1.0)
     {
        _eext_circle_object_show(circle_obj);
        elm_layout_signal_emit(widget_data->index, "index,state,circle,show", "elm");
        widget_data->hide_timer = ecore_timer_add(2.0, _eext_index_timer_cb, widget_data);
        widget_data->panel_hide = EINA_FALSE;
        eext_object_event_callback_add(widget_data->panel, EEXT_CALLBACK_BACK, _eext_index_back_cb, NULL);
        evas_object_smart_callback_call(widget_data->main_ly, SIG_INDEX_SHOW, NULL);
     }
   else
     {
        if (!widget_data->panel_hide)
          {
             _eext_circle_object_hide(circle_obj);
             elm_layout_signal_emit(widget_data->index, "index,state,circle,hide", "elm");
             widget_data->panel_hide = EINA_TRUE;
             eext_object_event_callback_del(widget_data->panel, EEXT_CALLBACK_BACK, _eext_index_back_cb);
             evas_object_smart_callback_call(widget_data->main_ly, SIG_INDEX_HIDE, NULL);
          }
     }
}

static Eext_Index_Layout_Item*
_eext_index_layout_item_new(Evas_Object *obj,
                            const char *letter,
                            Eext_Index_Layout_Item *relative_item,
                            Index_Layout_Item_Add_Type type,
                            Evas_Smart_Cb func,
                            void *data)
{
   Eext_Index_Layout_Item *item;
   Eext_Circle_Object_Item *circle_item;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj);
   EEXT_INDEX_LAYOUT_DATA_GET(circle_obj, widget_data);

   item = (Eext_Index_Layout_Item *)calloc(1, sizeof(Eext_Index_Layout_Item));
   if (!item) return NULL;

   circle_item = _eext_circle_object_item_new();
   circle_item->radius = 162;
   _eext_circle_object_item_text_set(circle_item, letter, NULL);

   item->item = circle_item;

   //Priority feature does not need in current UX.
   int priority = 0;
   item->priority = priority;

   if (priority == 1)
     widget_data->second_list_exist = EINA_TRUE;

   item->letter = eina_stringshare_add(letter);
   if (func)
     item->func = func;
   if (data)
     item->base.data = data;
   item->base.obj = obj;

   if (eina_list_count(widget_data->item_list) == 0)
     item->selected = EINA_TRUE;

   circle_item->data = item;

   if (type == APPEND)
     {
        _eext_circle_object_item_append(circle_obj, circle_item);
        widget_data->item_list = eina_list_append(widget_data->item_list, item);
     }
   else if (type == PREPEND)
     {
        _eext_circle_object_item_prepend(circle_obj, circle_item);
        widget_data->item_list = eina_list_prepend(widget_data->item_list, item);
     }
   else if (type == BEFORE)
     {
        _eext_circle_object_item_insert_before(circle_obj,
                                               (Eext_Circle_Object *)relative_item->item, circle_item);
        widget_data->item_list = eina_list_prepend_relative(widget_data->item_list, relative_item, item);
     }
   else
     {
        _eext_circle_object_item_insert_after(circle_obj,
                                              (Eext_Circle_Object *)relative_item->item, circle_item);
        widget_data->item_list = eina_list_append_relative(widget_data->item_list, relative_item, item);
     }

   return item;
}

static Evas_Object*
_eext_index_main_layout_add(Evas_Object *parent)
{
   Evas_Object *main_ly;
   main_ly = elm_layout_add(parent);
   elm_layout_theme_set(main_ly, "layout", "application", "circle/fastscroll");
   evas_object_show(main_ly);

   return main_ly;
}

static Evas_Object*
_eext_index_panel_add(Evas_Object *parent)
{
   Evas_Object *panel;
   panel = elm_panel_add(parent);
   elm_panel_scrollable_set(panel, EINA_TRUE);
   elm_panel_scrollable_content_size_set(panel, 1.0);
   elm_object_style_set(panel, "fastscroll");
   elm_panel_hidden_set(panel, EINA_TRUE);
   elm_panel_orient_set(panel, ELM_PANEL_ORIENT_LEFT);
   evas_object_show(panel);

   return panel;
}

static void
_eext_index_layout_init(Eext_Circle_Object *obj)
{
   Eext_Index_Layout_Data *data;

   obj->widget_object = NULL;
   obj->widget_type = EEXT_INDEX_LAYOUT_TYPE;
   obj->del_func = _eext_index_layout_del_cb;

   data = (Eext_Index_Layout_Data *)calloc(1, sizeof(Eext_Index_Layout_Data));
   data->index_priority = 0;
   data->highlighted_index = 0;
   data->max_item = 360 / 5; //The Minimum Item size in GUI Guide

   obj->widget_data = (void *)data;
}
static Evas_Object *
_eext_index_add(Evas_Object *parent)
{
   Evas_Object *obj;

   if (!parent) return NULL;

   obj = elm_image_add(parent);
   _eext_circle_object_init(obj, NULL, NULL);

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return NULL;

   _eext_index_layout_init(circle_obj);

   return obj;
}

//This index scroller layout including mobile UX such as indicator button, panel scroll.
EAPI Evas_Object *
eext_index_layout_add(Evas_Object *parent)
{
   Evas_Object *obj, *circle_obj_index;

   if (!parent) return NULL;

   obj = _eext_index_main_layout_add(parent);
   if (!obj) return NULL;

   circle_obj_index = _eext_index_add(parent);
   if (!circle_obj_index) return NULL;

   EEXT_CIRCLE_OBJECT_GET(circle_obj_index, circle_obj) return NULL;
   EEXT_INDEX_LAYOUT_DATA_GET(circle_obj, data) return NULL;

   data->main_ly = obj;
   data->panel = _eext_index_panel_add(data->main_ly);
   data->panel_hide = EINA_TRUE;

   data->index = elm_layout_add(parent);
   elm_layout_theme_set(data->index, "layout", "circle", "fastscroll");
   elm_object_part_content_set(data->index, "elm.swallow.circle", circle_obj_index);

   data->cue = elm_button_add(data->main_ly);
   elm_object_style_set(data->cue, "fastscroll");
   elm_object_part_content_set(data->main_ly, "elm.swallow.cue", data->cue);
   evas_object_show(data->cue);

   eext_rotary_object_event_callback_add(data->main_ly, _rotary_changed_cb, circle_obj);
   evas_object_smart_callback_add(data->panel, "scroll", _panel_scroll_cb, circle_obj);

   elm_object_part_content_set(data->main_ly, "elm.swallow.left", data->panel);
   elm_object_content_set(data->panel, data->index);

   evas_object_data_set(data->main_ly, EEXT_CIRCLE_OBJECT_TYPE, circle_obj);

   return obj;
}

EAPI Eext_Object_Item*
eext_index_layout_item_append(Evas_Object *obj, const char *text, Evas_Smart_Cb func,void *data)
{
   Eext_Index_Layout_Item *item;

   if (!obj) return NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return NULL;
   EEXT_INDEX_LAYOUT_DATA_GET(circle_obj, widget_data) return NULL;

   item = _eext_index_layout_item_new(obj, text, NULL, APPEND, func, data);

   //This callback will be called before cairo render start.
   if (!circle_obj->render_pre_cb)
     circle_obj->render_pre_cb = _eext_index_layout_render_pre_cb;

   return (Eext_Object_Item *)item;
}

EAPI Eext_Object_Item*
eext_index_layout_item_prepend(Evas_Object *obj, const char *text, Evas_Smart_Cb func, void *data)
{
   Eext_Index_Layout_Item *item;

   if (!obj) return NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return NULL;
   EEXT_INDEX_LAYOUT_DATA_GET(circle_obj, widget_data) return NULL;

   item = _eext_index_layout_item_new(obj, text, NULL, PREPEND, func ,data);

   //This callback will be called before cairo render start.
   if (!circle_obj->render_pre_cb)
     circle_obj->render_pre_cb = _eext_index_layout_render_pre_cb;

   return (Eext_Object_Item *)item;
}

EAPI Eext_Object_Item*
eext_index_layout_item_insert_before(Evas_Object *obj,
                                     Eext_Object_Item *before,
                                     const char *text,
                                     Evas_Smart_Cb func,
                                     void *data)
{
   Eext_Index_Layout_Item *item, *b_item;
   b_item = (Eext_Index_Layout_Item *)before;
   if (!obj) return NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return NULL;
   EEXT_INDEX_LAYOUT_DATA_GET(circle_obj, widget_data) return NULL;

   if (!before)
     return eext_index_layout_item_prepend(obj, text, func, data);

   item = _eext_index_layout_item_new(obj, text, b_item, BEFORE, func, data);

   //This callback will be called before cairo render start.
   if (!circle_obj->render_pre_cb)
     circle_obj->render_pre_cb = _eext_index_layout_render_pre_cb;

   return (Eext_Object_Item *)item;
}

EAPI Eext_Object_Item*
eext_index_layout_item_insert_after(Evas_Object *obj,
                                    Eext_Object_Item *after,
                                    const char *text,
                                    Evas_Smart_Cb func,
                                    void *data)
{
   Eext_Index_Layout_Item *item, *a_item;
   a_item = (Eext_Index_Layout_Item *)after;
   if (!obj) return NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return NULL;
   EEXT_INDEX_LAYOUT_DATA_GET(circle_obj, widget_data) return NULL;

   if (!after)
     return eext_index_layout_item_append(obj, text, func, data);

   item = _eext_index_layout_item_new(obj, text, a_item, AFTER, func, data);

   //This callback will be called before cairo render start.
   if (!circle_obj->render_pre_cb)
     circle_obj->render_pre_cb = _eext_index_layout_render_pre_cb;

   return (Eext_Object_Item *)item;
}

EAPI void
eext_index_layout_items_clear(Evas_Object *obj)
{
   char buf[PATH_MAX];
   Eext_Circle_Object_Item *item;
   Eext_Index_Layout_Item *index_item;

   if (!obj) return;
   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return;
   EEXT_INDEX_LAYOUT_DATA_GET(circle_obj, data) return;

   if (data->item_list)
     EINA_LIST_FREE(data->item_list, index_item)
       {
          eina_stringshare_del(index_item->letter);
          index_item->letter = NULL;
       }
   data->item_list = NULL;

   data->current_list = NULL;
   data->omit_list = NULL;

   if (circle_obj->items)
     EINA_LIST_FREE(circle_obj->items, item)
       {
          if (item->data)
            {
               free(item->data);
               item->data = NULL;
            }
          _eext_circle_object_item_free(item);
       }
   circle_obj->items = NULL;

   snprintf(buf, sizeof(buf), " ");
   elm_object_part_text_set(data->index, "elm.text", buf);
}

EAPI void
eext_index_layout_item_del(Eext_Object_Item *item)
{
   Eina_List *l;
   Eext_Index_Layout_Item *index_item, *temp_item;
   Eina_Bool deleted;

   if (!item) return;

   index_item = (Eext_Index_Layout_Item *)item;

   if (!index_item->base.obj) return;
   EEXT_CIRCLE_OBJECT_GET(index_item->base.obj, circle_obj) return;
   EEXT_INDEX_LAYOUT_DATA_GET(circle_obj, data) return;

   //Letter delete in target lists.
   EINA_LIST_FOREACH(data->item_list, l, temp_item)
     if (!strcmp(index_item->letter, temp_item->letter))
       {
          data->item_list = eina_list_remove(data->item_list, temp_item);
          deleted = EINA_TRUE;
       }

   //The item is not exists in lists.
   if (!deleted) return;

   //Eext_Circle_Object_Item deleted and re-render.
   if (index_item->letter)
     {
        eina_stringshare_del(index_item->letter);
        index_item->letter = NULL;
     }

   _eext_circle_object_item_remove(index_item->item);
   circle_obj->render_pre_cb = _eext_index_layout_render_pre_cb;

   //Free Eext_Index_Layout_Item data.
   free(index_item);
   index_item = NULL;
}

EAPI Eext_Object_Item*
eext_index_layout_item_find(Evas_Object *obj, const void *data)
{
   Eina_List *l;
   Eext_Index_Layout_Item *index_item;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return NULL;
   EEXT_INDEX_LAYOUT_DATA_GET(circle_obj, widget_data) return NULL;

   EINA_LIST_FOREACH(widget_data->item_list, l, index_item)
     if (index_item->base.data == data)
       return (Eext_Object_Item *)index_item;

   return NULL;
}

EAPI Eext_Object_Item*
eext_index_layout_selected_item_get(Evas_Object *obj)
{
   Eina_List *l;
   Eext_Index_Layout_Item *index_item;

   if (!obj) return NULL;
   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return NULL;
   EEXT_INDEX_LAYOUT_DATA_GET(circle_obj, data) return NULL;

   if (data->item_list)
     EINA_LIST_FOREACH(data->item_list, l, index_item)
       if (index_item->selected)
         return (Eext_Object_Item *)index_item;

   return NULL;
}

EAPI void
eext_index_layout_selected_item_set(Eext_Object_Item *item)
{
   Eina_List *l;
   Eext_Index_Layout_Item *index_item, *temp_item;

   if (!item || !item->obj) return;

   index_item = (Eext_Index_Layout_Item *)item;

   EEXT_CIRCLE_OBJECT_GET(index_item->base.obj, circle_obj) return;
   EEXT_INDEX_LAYOUT_DATA_GET(circle_obj, data) return;

   if (!data) return;

   EINA_LIST_FOREACH(data->item_list, l, temp_item)
      if (temp_item->selected)
        {
           if (temp_item == index_item)
             return;

           temp_item->selected = EINA_FALSE;
           break;
        }

   index_item->selected = EINA_TRUE;

   circle_obj->render_pre_cb = _eext_index_layout_render_pre_cb;

   return;
}

EAPI void
eext_index_layout_item_text_set(Eext_Object_Item *item, const char *text)
{
   Eext_Index_Layout_Item *index_item;

   if (!item) return;

   index_item = (Eext_Index_Layout_Item *)item;

   if (!index_item->base.obj) return;
   EEXT_CIRCLE_OBJECT_GET(index_item->base.obj, circle_obj) return;

   if (index_item->letter)
     if (!strcmp(index_item->letter, text))
       return;

   eina_stringshare_del(index_item->letter);
   index_item->letter = NULL;

   if (text)
     index_item->letter = eina_stringshare_add(text);

   _eext_circle_object_item_text_set(index_item->item, text, NULL);

   circle_obj->render_pre_cb = _eext_index_layout_render_pre_cb;
}

EAPI const char*
eext_index_layout_item_text_get(Eext_Object_Item *item)
{
   Eext_Index_Layout_Item *index_item;

   if (!item) return NULL;

   index_item = (Eext_Index_Layout_Item *)item;

   if (index_item->letter)
     return index_item->letter;
   else
     return NULL;
}
