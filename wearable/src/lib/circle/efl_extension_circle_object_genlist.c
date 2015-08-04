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

#define EEXT_CIRCLE_OBJECT_GENLIST_TYPE "Eext_Circle_Object_Genlist"

#define VERTICAL_BAR_ANGLE_START 45.0
#define VERTICAL_BAR_ANGLE_RANGE 90.0
#define VERTICAL_BAR_ANGLE_MAX_SIZE 90.0
#define VERTICAL_BAR_ANGLE_MIN_SIZE 6.0
#define VERTICAL_BAR_TRANSIT_TIME 0.2
#define VERTICAL_BAR_HIDE_WAITTING_TIME 0.2
#define VERTICAL_BAR_HIDE_WAITTING_TIME_ON_INITIALIZING 1.0
#define VERTICAL_BAR_WIDTH_SIZE 3
#define VERTICAL_BAR_RADIUS_SIZE 169 // (180 - 11)

#define EEXT_CIRCLE_OBJECT_GENLIST_DATA_GET(circle_obj, data) \
   Eext_Circle_Object_Genlist_Data *data = NULL; \
   if (circle_obj && circle_obj->widget_data && \
       circle_obj->widget_type && !strcmp(circle_obj->widget_type, EEXT_CIRCLE_OBJECT_GENLIST_TYPE)) \
      data = (Eext_Circle_Object_Genlist_Data *)circle_obj->widget_data; \
   if (!data)

#define GENLIST_ITEM_NAME "default"
#define GENLIST_BG_ITEM_NAME "vertical,scroll,bg"

static void
_eext_circle_object_genlist_region_show(void *data, Eina_Bool clockwise)
{
   Eext_Circle_Object *obj = (Eext_Circle_Object *)data;
   Evas_Coord vh; /* viewport h*/
   Elm_Object_Item *next_item;

   if (!obj) return;

   elm_scroller_region_get(obj->widget_object, NULL, NULL, NULL, &vh);

   next_item = elm_genlist_at_xy_item_get(obj->widget_object, 0, (vh / 2), 0);

   if (!next_item) return;

   if (clockwise)
     next_item = elm_genlist_item_next_get(next_item);
   else
     next_item = elm_genlist_item_prev_get(next_item);

   if (!next_item) return;

   elm_genlist_item_bring_in(next_item, ELM_GENLIST_ITEM_SCROLLTO_MIDDLE);
}

static Eina_Bool
_rotary_changed_cb(void *data, Evas_Object *obj, Eext_Rotary_Event_Info *info)
{
   if (info->direction == EEXT_ROTARY_DIRECTION_CLOCKWISE)
     _eext_circle_object_genlist_region_show(data, EINA_TRUE);
   else
     _eext_circle_object_genlist_region_show(data, EINA_FALSE);

   return EINA_TRUE;
}

static void
_eext_circle_object_genlist_scrollbar_update(Eext_Circle_Object *obj)
{
   Eext_Circle_Object_Item *item;
   Evas_Coord ch; /* content h */
   Evas_Coord py, vh; /* postion y, viewport h*/
   double ratio;
   double bar_pos_y = VERTICAL_BAR_ANGLE_START;
   double bar_size_y = VERTICAL_BAR_ANGLE_RANGE;
   double vertical_bar_angle_range = VERTICAL_BAR_ANGLE_RANGE;

   if (!obj) return;

   item = _eext_circle_object_item_get(obj, GENLIST_ITEM_NAME);

   elm_scroller_child_size_get(obj->widget_object, NULL, &ch);
   elm_scroller_region_get(obj->widget_object, NULL, &py, NULL, &vh);

   if (ch > 0)
     {
        ratio = (double)ch / (double)vh;

        if (ratio == 0) return;

        if (ch > vh)
          {
             bar_size_y = vertical_bar_angle_range / ratio;

             if (bar_size_y > VERTICAL_BAR_ANGLE_MAX_SIZE)
               bar_size_y = VERTICAL_BAR_ANGLE_MAX_SIZE;
             if (bar_size_y < VERTICAL_BAR_ANGLE_MIN_SIZE)
               bar_size_y = VERTICAL_BAR_ANGLE_MIN_SIZE;
          }
        bar_pos_y += (double)py / ratio / 360 * vertical_bar_angle_range;

        _eext_circle_object_item_angle_set(item, bar_size_y);
        _eext_circle_object_item_angle_offset_set(item, bar_pos_y);
     }
}

static void
_eext_circle_object_genlist_realized_cb(void *data,
                                        Evas_Object *obj,
                                        void *event_info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;

   EEXT_CIRCLE_OBJECT_GENLIST_DATA_GET(circle_obj, widget_data) return;

   if (widget_data->policy_v == ELM_SCROLLER_POLICY_OFF) return;

   _eext_circle_object_genlist_scrollbar_update(circle_obj);
}

static void
_eext_circle_object_genlist_color_changed_cb(Eext_Circle_Object *obj, const char *item_name)
{
   Eext_Circle_Object_Item *item;
   int r, g, b, a;

   EEXT_CIRCLE_OBJECT_GENLIST_DATA_GET(obj, widget_data) return;

   item = _eext_circle_object_item_get(obj, item_name);
   if (!item) return;

   _eext_circle_object_item_color_get(item, &r, &g, &b, &a);

   if (!strcmp(item_name, GENLIST_ITEM_NAME))
     {
        widget_data->color.r = r;
        widget_data->color.g = g;
        widget_data->color.b = b;
        widget_data->color.a = a;

        return;
     }

   if (!strcmp(item_name, GENLIST_BG_ITEM_NAME))
     {
        widget_data->bg_color.r = r;
        widget_data->bg_color.g = g;
        widget_data->bg_color.b = b;
        widget_data->bg_color.a = a;

        return;
     }
}

static Eina_Bool
_eext_circle_object_genlist_scrollbar_policy_hide_cb(void *data)
{
   Eext_Circle_Object *obj = (Eext_Circle_Object *)data;
   Eext_Circle_Object_Item *item;
   Eina_List *l;

   EEXT_CIRCLE_OBJECT_GENLIST_DATA_GET(obj, widget_data) return ECORE_CALLBACK_CANCEL;

   if (widget_data->realized_callback_added)
     {
        evas_object_smart_callback_del_full(obj->widget_object, "realized", _eext_circle_object_genlist_realized_cb, obj);
        widget_data->realized_callback_added = EINA_FALSE;
     }

   EINA_LIST_FOREACH(obj->items, l, item)
     {
        _eext_circle_object_item_color_transit_set(item, VERTICAL_BAR_TRANSIT_TIME,
                                                   0, 0, 0, 0,
                                                   EINA_TRUE,
                                                   0.25, 0.46, 0.45, 1.0);
     }

   if (widget_data->bar_hide_timer)
     {
         ecore_timer_del(widget_data->bar_hide_timer);
         widget_data->bar_hide_timer = NULL;
     }

   return ECORE_CALLBACK_CANCEL;
}

static void
_eext_circle_object_genlist_scrollbar_show(Eext_Circle_Object *obj,
                                           double hide_waiting_time)
{
   Eext_Circle_Object_Item *item;

   EEXT_CIRCLE_OBJECT_GENLIST_DATA_GET(obj, widget_data) return;

   if (widget_data->realized_callback_added)
     {
        evas_object_smart_callback_del_full(obj->widget_object, "realized", _eext_circle_object_genlist_realized_cb, obj);
        widget_data->realized_callback_added = EINA_FALSE;
     }

   if (widget_data->bar_hide_timer)
     {
         ecore_timer_del(widget_data->bar_hide_timer);
         widget_data->bar_hide_timer = NULL;
     }

   item = _eext_circle_object_item_get(obj, GENLIST_BG_ITEM_NAME);
   if (obj->disabled)
     _eext_circle_object_item_color_set(item,
                                        widget_data->disabled_bg_color.r,
                                        widget_data->disabled_bg_color.g,
                                        widget_data->disabled_bg_color.b,
                                        widget_data->disabled_bg_color.a);
   else
     _eext_circle_object_item_color_set(item,
                                        widget_data->bg_color.r,
                                        widget_data->bg_color.g,
                                        widget_data->bg_color.b,
                                        widget_data->bg_color.a);

   item = _eext_circle_object_item_get(obj, GENLIST_ITEM_NAME);
   if (obj->disabled)
     _eext_circle_object_item_color_set(item,
                                        widget_data->disabled_color.r,
                                        widget_data->disabled_color.g,
                                        widget_data->disabled_color.b,
                                        widget_data->disabled_color.a);
   else
     _eext_circle_object_item_color_set(item,
                                        widget_data->color.r,
                                        widget_data->color.g,
                                        widget_data->color.b,
                                        widget_data->color.a);

   if (widget_data->policy_v == ELM_SCROLLER_POLICY_AUTO)
     {
        widget_data->bar_hide_timer = ecore_timer_add(hide_waiting_time,
                                                      _eext_circle_object_genlist_scrollbar_policy_hide_cb,
                                                      obj);
     }
}

static void
_eext_circle_object_genlist_show_cb(void *data,
                                    Evas *e,
                                    Evas_Object *obj,
                                    void *event_info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;

   EEXT_CIRCLE_OBJECT_GENLIST_DATA_GET(circle_obj, widget_data) return;

   if (widget_data->policy_v == ELM_SCROLLER_POLICY_OFF) return;

   _eext_circle_object_genlist_scrollbar_update(circle_obj);
   _eext_circle_object_genlist_scrollbar_show(circle_obj,
                                              VERTICAL_BAR_HIDE_WAITTING_TIME_ON_INITIALIZING);
   if (!widget_data->realized_callback_added)
     {
        evas_object_smart_callback_add(circle_obj->widget_object, "realized", _eext_circle_object_genlist_realized_cb, circle_obj);
        widget_data->realized_callback_added = EINA_TRUE;
     }
}

static void
_eext_circle_object_genlist_scroll_cb(void *data,
                                      Evas_Object *obj,
                                      void *event_info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;

   EEXT_CIRCLE_OBJECT_GENLIST_DATA_GET(circle_obj, widget_data) return;

   if (widget_data->policy_v == ELM_SCROLLER_POLICY_OFF) return;

   _eext_circle_object_genlist_scrollbar_update(circle_obj);
   _eext_circle_object_genlist_scrollbar_show(circle_obj,
                                              VERTICAL_BAR_HIDE_WAITTING_TIME);
}

static void
_eext_circle_object_genlist_del_cb(Eext_Circle_Object *obj)
{
   EEXT_CIRCLE_OBJECT_GENLIST_DATA_GET(obj, widget_data) return;

   evas_object_event_callback_del_full(obj->widget_object, EVAS_CALLBACK_SHOW, _eext_circle_object_genlist_show_cb, obj);
   evas_object_smart_callback_del_full(obj->widget_object, "scroll", _eext_circle_object_genlist_scroll_cb, obj);

   if (widget_data->realized_callback_added)
     {
        evas_object_smart_callback_del_full(obj->widget_object, "realized", _eext_circle_object_genlist_realized_cb, obj);
        widget_data->realized_callback_added = EINA_FALSE;
     }

   if (widget_data->bar_hide_timer)
     {
        ecore_timer_del(widget_data->bar_hide_timer);
        widget_data->bar_hide_timer = NULL;
     }

   free(obj->widget_data);
   obj->widget_data = NULL;

   eext_rotary_object_event_callback_del(obj->widget_object, _rotary_changed_cb);
}

static void
_eext_circle_object_genlist_disabled_cb(Eext_Circle_Object *obj)
{
   Eext_Circle_Object_Item *item;

   EEXT_CIRCLE_OBJECT_GENLIST_DATA_GET(obj, data) return;

   if (obj->disabled)
     {
        item = _eext_circle_object_item_get(obj, GENLIST_ITEM_NAME);
        _eext_circle_object_item_color_set(item,
                                           data->disabled_color.r,
                                           data->disabled_color.g,
                                           data->disabled_color.b,
                                           data->disabled_color.a);

        item = _eext_circle_object_item_get(obj, GENLIST_BG_ITEM_NAME);
        _eext_circle_object_item_color_set(item,
                                           data->disabled_bg_color.r,
                                           data->disabled_bg_color.g,
                                           data->disabled_bg_color.b,
                                           data->disabled_bg_color.a);

        eext_rotary_object_event_callback_del(obj->widget_object, _rotary_changed_cb);
        eext_rotary_object_event_callback_del(obj->image_widget, _rotary_changed_cb);
     }
   else
     {
        item = _eext_circle_object_item_get(obj, GENLIST_ITEM_NAME);
        _eext_circle_object_item_color_set(item,
                                           data->color.r,
                                           data->color.g,
                                           data->color.b,
                                           data->color.a);

        item = _eext_circle_object_item_get(obj, GENLIST_BG_ITEM_NAME);
        _eext_circle_object_item_color_set(item,
                                           data->bg_color.r,
                                           data->bg_color.g,
                                           data->bg_color.b,
                                           data->bg_color.a);

        eext_rotary_object_event_callback_add(obj->widget_object, _rotary_changed_cb, obj);
        eext_rotary_object_event_callback_add(obj->image_widget, _rotary_changed_cb, obj);
     }
}

static void
_eext_circle_object_genlist_init(Eext_Circle_Object *obj, Evas_Object *genlist)
{
   Eext_Circle_Object_Genlist_Data *data;
   Eext_Circle_Object_Item *item;

   obj->widget_type = EEXT_CIRCLE_OBJECT_GENLIST_TYPE;
   obj->del_func = _eext_circle_object_genlist_del_cb;
   obj->disabled_func = _eext_circle_object_genlist_disabled_cb;
   obj->color_changed_func = _eext_circle_object_genlist_color_changed_cb;

   data = (Eext_Circle_Object_Genlist_Data *)calloc(1, sizeof(Eext_Circle_Object_Genlist_Data));
   data->policy_v = ELM_SCROLLER_POLICY_AUTO;
   data->color.r = 0;
   data->color.g = 148;
   data->color.b = 255;
   data->color.a = 255;
   data->bg_color.r = 0;
   data->bg_color.g = 148;
   data->bg_color.b = 255;
   data->bg_color.a = 63;
   data->disabled_color.r = 45;
   data->disabled_color.g = 45;
   data->disabled_color.b = 45;
   data->disabled_color.a = 255;
   data->disabled_bg_color.r = 45;
   data->disabled_bg_color.g = 45;
   data->disabled_bg_color.b = 45;
   data->disabled_bg_color.a = 63;
   obj->widget_data = (void *)data;

   item = _eext_circle_object_item_new();
   _eext_circle_object_item_name_set(item, GENLIST_BG_ITEM_NAME);
   _eext_circle_object_item_color_set(item,
                                      data->bg_color.r,
                                      data->bg_color.g,
                                      data->bg_color.b,
                                      data->bg_color.a);
   _eext_circle_object_item_line_width_set(item, VERTICAL_BAR_WIDTH_SIZE);
   _eext_circle_object_item_radius_set(item, VERTICAL_BAR_RADIUS_SIZE);
   _eext_circle_object_item_angle_set(item, VERTICAL_BAR_ANGLE_RANGE);
   _eext_circle_object_item_angle_offset_set(item, VERTICAL_BAR_ANGLE_START);
   _eext_circle_object_item_append(obj, item);

   item = _eext_circle_object_item_new();
   _eext_circle_object_item_name_set(item, GENLIST_ITEM_NAME);
   _eext_circle_object_item_color_set(item,
                                      data->color.r,
                                      data->color.g,
                                      data->color.b,
                                      data->color.a);
   _eext_circle_object_item_line_width_set(item, VERTICAL_BAR_WIDTH_SIZE);
   _eext_circle_object_item_radius_set(item, VERTICAL_BAR_RADIUS_SIZE);
   _eext_circle_object_item_angle_offset_set(item, VERTICAL_BAR_ANGLE_START);
   _eext_circle_object_item_append(obj, item);

   elm_scroller_policy_set(obj->widget_object, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   evas_object_event_callback_add(obj->widget_object, EVAS_CALLBACK_SHOW, _eext_circle_object_genlist_show_cb, obj);
   evas_object_smart_callback_add(obj->widget_object, "scroll", _eext_circle_object_genlist_scroll_cb, obj);
   eext_rotary_object_event_callback_add(obj->widget_object, _rotary_changed_cb, obj);
   eext_rotary_object_event_callback_add(obj->image_widget, _rotary_changed_cb, obj);
}

EAPI Evas_Object *
eext_circle_object_genlist_add(Evas_Object *genlist, Eext_Circle_Surface *surface)
{
   Evas_Object *obj;

   if (!genlist) return NULL;

   obj = elm_image_add(genlist);
   _eext_circle_object_init(obj, genlist, surface);

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return NULL;

   _eext_circle_object_genlist_init(circle_obj, genlist);

   return obj;
}


EAPI void
eext_circle_object_genlist_scroller_policy_set(Evas_Object *obj,
                                               Elm_Scroller_Policy policy_h,
                                               Elm_Scroller_Policy policy_v)
{
   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return;
   EEXT_CIRCLE_OBJECT_GENLIST_DATA_GET(circle_obj, data)
     {
        LOGE("Eext_Circle_Object type mismatch: expected type [%s], given type [%s]",
             EEXT_CIRCLE_OBJECT_GENLIST_TYPE, circle_obj->widget_type);
        return;
     }

   if ((policy_h >= ELM_SCROLLER_POLICY_LAST) ||
       (policy_v >= ELM_SCROLLER_POLICY_LAST))
     return;

   data->policy_h = policy_h;
   data->policy_v = policy_v;

   if (data->policy_v == ELM_SCROLLER_POLICY_ON)
     {
        _eext_circle_object_genlist_scrollbar_update(circle_obj);
        _eext_circle_object_genlist_scrollbar_show(circle_obj,
                                                   VERTICAL_BAR_HIDE_WAITTING_TIME_ON_INITIALIZING);
     }
}

EAPI void
eext_circle_object_genlist_scroller_policy_get(const Evas_Object *obj,
                                               Elm_Scroller_Policy *policy_h,
                                               Elm_Scroller_Policy *policy_v)
{
   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj)
     {
        if (policy_h) *policy_h = ELM_SCROLLER_POLICY_AUTO;
        if (policy_v) *policy_v = ELM_SCROLLER_POLICY_AUTO;
        return;
     }

   EEXT_CIRCLE_OBJECT_GENLIST_DATA_GET(circle_obj, data)
     {
        LOGE("Eext_Circle_Object type mismatch: expected type [%s], given type [%s]",
             EEXT_CIRCLE_OBJECT_GENLIST_TYPE, circle_obj->widget_type);
        if (policy_h) *policy_h = ELM_SCROLLER_POLICY_AUTO;
        if (policy_v) *policy_v = ELM_SCROLLER_POLICY_AUTO;
        return;
     }

   if (policy_h) *policy_h = data->policy_h;
   if (policy_v) *policy_v = data->policy_v;
}
