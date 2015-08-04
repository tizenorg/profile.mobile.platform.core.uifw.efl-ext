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

#define EEXT_CIRCLE_OBJECT_SCROLLER_TYPE "Eext_Circle_Object_Scroller"

#define VERTICAL_BAR_NAME "default"
#define VERTICAL_BAR_BG_NAME "vertical,scroll,bg"
#define VERTICAL_BAR_ANGLE_START 45.0
#define VERTICAL_BAR_ANGLE_RANGE 90.0
#define VERTICAL_BAR_ANGLE_MAX_SIZE 90.0
#define VERTICAL_BAR_ANGLE_MIN_SIZE 6.0

#define HORIZONTAL_BAR_NAME "horizontal,scroll,bar"
#define HORIZONTAL_BAR_BG_NAME "horizontal,scroll,bg"
#define HORIZONTAL_BAR_ANGLE_START -45.0
#define HORIZONTAL_BAR_ANGLE_RANGE 90.0
#define HORIZONTAL_BAR_ANGLE_MAX_SIZE 90.0
#define HORIZONTAL_BAR_ANGLE_MIN_SIZE 6.0

#define SCROLL_BAR_TRANSIT_TIME 0.2
#define SCROLL_BAR_HIDE_WAITTING_TIME 0.2
#define SCROLL_BAR_HIDE_WAITTING_TIME_ON_INITIALIZING 1.0

#define EEXT_CIRCLE_OBJECT_SCROLLER_DATA_GET(circle_obj, data) \
   Eext_Circle_Object_Scroller_Data *data = NULL; \
   if (circle_obj && circle_obj->widget_data && \
       circle_obj->widget_type && !strcmp(circle_obj->widget_type, EEXT_CIRCLE_OBJECT_SCROLLER_TYPE)) \
      data = (Eext_Circle_Object_Scroller_Data *)circle_obj->widget_data; \
   if (!data)

static void
_eext_circle_object_scroller_color_changed_cb(Eext_Circle_Object *obj, const char *item_name)
{
   Eext_Circle_Object_Item *item;
   int r, g, b, a;

   EEXT_CIRCLE_OBJECT_SCROLLER_DATA_GET(obj, widget_data) return;

   item = _eext_circle_object_item_get(obj, item_name);
   if (!item) return;

   _eext_circle_object_item_color_get(item, &r, &g, &b, &a);

   if (!strcmp(item_name, VERTICAL_BAR_NAME))
     {
        widget_data->vertical_color.r = r;
        widget_data->vertical_color.g = g;
        widget_data->vertical_color.b = b;
        widget_data->vertical_color.a = a;

        return;
     }

   if (!strcmp(item_name, VERTICAL_BAR_BG_NAME))
     {
        widget_data->vertical_bg_color.r = r;
        widget_data->vertical_bg_color.g = g;
        widget_data->vertical_bg_color.b = b;
        widget_data->vertical_bg_color.a = a;

        return;
     }

   if (!strcmp(item_name, HORIZONTAL_BAR_NAME))
     {
        widget_data->horizontal_color.r = r;
        widget_data->horizontal_color.g = g;
        widget_data->horizontal_color.b = b;
        widget_data->horizontal_color.a = a;

        return;
     }

   if (!strcmp(item_name, HORIZONTAL_BAR_BG_NAME))
     {
        widget_data->horizontal_bg_color.r = r;
        widget_data->horizontal_bg_color.g = g;
        widget_data->horizontal_bg_color.b = b;
        widget_data->horizontal_bg_color.a = a;

        return;
     }
}

static Eina_Bool
_eext_circle_object_scroller_scrollbar_policy_hide_cb(void *data)
{
   Eext_Circle_Object *obj = (Eext_Circle_Object *)data;
   Eext_Circle_Object_Item *item, *bg_item;

   EEXT_CIRCLE_OBJECT_SCROLLER_DATA_GET(obj, widget_data) return ECORE_CALLBACK_CANCEL;

   if (widget_data->policy_v == ELM_SCROLLER_POLICY_AUTO)
     {
        item = _eext_circle_object_item_get(obj, VERTICAL_BAR_NAME);
        bg_item = _eext_circle_object_item_get(obj, VERTICAL_BAR_BG_NAME);

        _eext_circle_object_item_color_transit_set(item, SCROLL_BAR_TRANSIT_TIME,
                                                   0, 0, 0, 0,
                                                   EINA_TRUE,
                                                   0.25, 0.46, 0.45, 1.0);
        _eext_circle_object_item_color_transit_set(bg_item, SCROLL_BAR_TRANSIT_TIME,
                                                   0, 0, 0, 0,
                                                   EINA_TRUE,
                                                   0.25, 0.46, 0.45, 1.0);
     }

   if (widget_data->policy_h == ELM_SCROLLER_POLICY_AUTO)
     {
        item = _eext_circle_object_item_get(obj, HORIZONTAL_BAR_NAME);
        bg_item = _eext_circle_object_item_get(obj, HORIZONTAL_BAR_BG_NAME);

        _eext_circle_object_item_color_transit_set(item, SCROLL_BAR_TRANSIT_TIME,
                                                   0, 0, 0, 0,
                                                   EINA_TRUE,
                                                   0.25, 0.46, 0.45, 1.0);
        _eext_circle_object_item_color_transit_set(bg_item, SCROLL_BAR_TRANSIT_TIME,
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
_eext_circle_object_scroller_scrollbar_update(Eext_Circle_Object *obj,
                                              double hide_waiting_time)
{
   Eext_Circle_Object_Item *item, *bg_item;
   Evas_Object *scroller = obj->widget_object;
   Evas_Coord cont_v = 0, cont_h = 0; /* content v/h */
   Evas_Coord pos_x = 0, pos_y = 0;   /* postion x/y */
   Evas_Coord vp_v = 0, vp_h = 0;     /* viewport v/h */
   double bar_pos_v = 0.0, bar_pos_h = 0.0;   /* bar position v/h */
   double bar_size_v = 0.0, bar_size_h = 0.0; /* bar size v/h */

   EEXT_CIRCLE_OBJECT_SCROLLER_DATA_GET(obj, data) return;

   elm_scroller_child_size_get(scroller, &cont_h, &cont_v);
   elm_scroller_region_get(scroller, &pos_y, &pos_x, &vp_h, &vp_v);

   item = _eext_circle_object_item_get(obj, VERTICAL_BAR_NAME);
   bg_item = _eext_circle_object_item_get(obj, VERTICAL_BAR_BG_NAME);

   if ((cont_v > 0.0) && (vp_v > 0.0) &&
       (data->policy_v != ELM_SCROLLER_POLICY_OFF))
     {
        if (cont_v > vp_v)
          {
             bar_size_v = VERTICAL_BAR_ANGLE_RANGE / ((double)cont_v / (double)vp_v);

             if (bar_size_v > VERTICAL_BAR_ANGLE_MAX_SIZE)
                bar_size_v = VERTICAL_BAR_ANGLE_MAX_SIZE;
             if (bar_size_v < VERTICAL_BAR_ANGLE_MIN_SIZE)
                bar_size_v = VERTICAL_BAR_ANGLE_MIN_SIZE;
          }
        else
           bar_size_v = VERTICAL_BAR_ANGLE_MAX_SIZE;

        bar_pos_v = (VERTICAL_BAR_ANGLE_RANGE / (double)cont_v) * (double)pos_x;
        bar_pos_v += VERTICAL_BAR_ANGLE_START;

        _eext_circle_object_item_angle_offset_set(item, bar_pos_v);
        _eext_circle_object_item_angle_set(item, bar_size_v);

        if (obj->disabled)
          {
             _eext_circle_object_item_color_transit_set(item, SCROLL_BAR_TRANSIT_TIME,
                                                        data->disabled_color.r,
                                                        data->disabled_color.g,
                                                        data->disabled_color.b,
                                                        data->disabled_color.a,
                                                        EINA_TRUE,
                                                        0.25, 0.46, 0.45, 1.0);
             _eext_circle_object_item_color_transit_set(bg_item, SCROLL_BAR_TRANSIT_TIME,
                                                        data->disabled_bg_color.r,
                                                        data->disabled_bg_color.g,
                                                        data->disabled_bg_color.b,
                                                        data->disabled_bg_color.a,
                                                        EINA_TRUE,
                                                        0.25, 0.46, 0.45, 1.0);
          }
        else
          {
             _eext_circle_object_item_color_transit_set(item, SCROLL_BAR_TRANSIT_TIME,
                                                        data->vertical_color.r,
                                                        data->vertical_color.g,
                                                        data->vertical_color.b,
                                                        data->vertical_color.a,
                                                        EINA_TRUE,
                                                        0.25, 0.46, 0.45, 1.0);
             _eext_circle_object_item_color_transit_set(bg_item, SCROLL_BAR_TRANSIT_TIME,
                                                        data->vertical_bg_color.r,
                                                        data->vertical_bg_color.g,
                                                        data->vertical_bg_color.b,
                                                        data->vertical_bg_color.a,
                                                        EINA_TRUE,
                                                        0.25, 0.46, 0.45, 1.0);
          }
     }
   else
     {
        _eext_circle_object_item_color_set(item, 0, 0, 0, 0);
        _eext_circle_object_item_color_set(bg_item, 0, 0, 0, 0);
     }

   item = _eext_circle_object_item_get(obj, HORIZONTAL_BAR_NAME);
   bg_item = _eext_circle_object_item_get(obj, HORIZONTAL_BAR_BG_NAME);

   if ((cont_h > 0.0) && (vp_h > 0.0) &&
       (data->policy_h != ELM_SCROLLER_POLICY_OFF))
     {
        if (cont_h > vp_h)
          {
             bar_size_h = HORIZONTAL_BAR_ANGLE_RANGE / ((double)cont_h / (double)vp_h);

             if (bar_size_h > HORIZONTAL_BAR_ANGLE_MAX_SIZE)
                bar_size_h = HORIZONTAL_BAR_ANGLE_MAX_SIZE;
             if (bar_size_h < HORIZONTAL_BAR_ANGLE_MIN_SIZE)
                bar_size_h = HORIZONTAL_BAR_ANGLE_MIN_SIZE;
          }
        else
           bar_size_h = HORIZONTAL_BAR_ANGLE_MAX_SIZE;

        bar_pos_h = (HORIZONTAL_BAR_ANGLE_RANGE / (double)cont_h) * (double)pos_y;
        bar_pos_h += HORIZONTAL_BAR_ANGLE_START;

        _eext_circle_object_item_angle_offset_set(item, bar_pos_h);
        _eext_circle_object_item_angle_set(item, bar_size_h);

        if (obj->disabled)
          {
             _eext_circle_object_item_color_transit_set(item, SCROLL_BAR_TRANSIT_TIME,
                                                        data->disabled_color.r,
                                                        data->disabled_color.g,
                                                        data->disabled_color.b,
                                                        data->disabled_color.a,
                                                        EINA_TRUE,
                                                        0.25, 0.46, 0.45, 1.0);
             _eext_circle_object_item_color_transit_set(bg_item, SCROLL_BAR_TRANSIT_TIME,
                                                        data->disabled_bg_color.r,
                                                        data->disabled_bg_color.g,
                                                        data->disabled_bg_color.b,
                                                        data->disabled_bg_color.a,
                                                        EINA_TRUE,
                                                        0.25, 0.46, 0.45, 1.0);
          }
        else
          {
             _eext_circle_object_item_color_transit_set(item, SCROLL_BAR_TRANSIT_TIME,
                                                        data->horizontal_color.r,
                                                        data->horizontal_color.g,
                                                        data->horizontal_color.b,
                                                        data->horizontal_color.a,
                                                        EINA_TRUE,
                                                        0.25, 0.46, 0.45, 1.0);
             _eext_circle_object_item_color_transit_set(bg_item, SCROLL_BAR_TRANSIT_TIME,
                                                        data->horizontal_bg_color.r,
                                                        data->horizontal_bg_color.g,
                                                        data->horizontal_bg_color.b,
                                                        data->horizontal_bg_color.a,
                                                        EINA_TRUE,
                                                        0.25, 0.46, 0.45, 1.0);
          }
     }
   else
     {
        _eext_circle_object_item_color_set(item, 0, 0, 0, 0);
        _eext_circle_object_item_color_set(bg_item, 0, 0, 0, 0);
     }

   if ((hide_waiting_time > 0.0) &&
       ((data->policy_v == ELM_SCROLLER_POLICY_AUTO) ||
        (data->policy_h == ELM_SCROLLER_POLICY_AUTO)))
     {
        if (data->bar_hide_timer)
          ecore_timer_del(data->bar_hide_timer);

        data->bar_hide_timer = ecore_timer_add(hide_waiting_time,
                                               _eext_circle_object_scroller_scrollbar_policy_hide_cb,
                                               obj);
     }
}

static void
_eext_circle_object_scroller_scroll_cb(void *data,
                                                 Evas_Object *obj,
                                                 void *event_info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;

   EEXT_CIRCLE_OBJECT_SCROLLER_DATA_GET(circle_obj, widget_data) return;

   if ((widget_data->policy_v == ELM_SCROLLER_POLICY_OFF) &&
       (widget_data->policy_h == ELM_SCROLLER_POLICY_OFF))
     return;

   _eext_circle_object_scroller_scrollbar_update(circle_obj,
                                                 SCROLL_BAR_HIDE_WAITTING_TIME);
}

static void
_eext_circle_object_scroller_show_cb(void *data,
                                     Evas *e,
                                     Evas_Object *obj,
                                     void *event_info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;

   EEXT_CIRCLE_OBJECT_SCROLLER_DATA_GET(circle_obj, widget_data) return;

   if ((widget_data->policy_v == ELM_SCROLLER_POLICY_OFF) &&
       (widget_data->policy_h == ELM_SCROLLER_POLICY_OFF))
     return;

   _eext_circle_object_scroller_scrollbar_update(circle_obj,
                                                 SCROLL_BAR_HIDE_WAITTING_TIME_ON_INITIALIZING);
}

static void
_eext_circle_object_scroller_content_resize_cb(void *data,
                                               Evas_Object *obj,
                                               void *event_info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;

   EEXT_CIRCLE_OBJECT_SCROLLER_DATA_GET(circle_obj, widget_data) return;

   if ((widget_data->policy_v == ELM_SCROLLER_POLICY_OFF) &&
       (widget_data->policy_h == ELM_SCROLLER_POLICY_OFF))
     return;

   _eext_circle_object_scroller_scrollbar_update(circle_obj,
                                                 SCROLL_BAR_HIDE_WAITTING_TIME_ON_INITIALIZING);
}

static Eina_Bool
_eext_circle_object_scroller_rotary_changed_cb(void *data,
                                               Evas_Object *obj,
                                               Eext_Rotary_Event_Info *info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;
   Evas_Object *scroller = circle_obj->widget_object;
   int h_pagesize, v_pagesize;
   int cw, ch;
   int x, y, w, h;
   int d = (info->direction == EEXT_ROTARY_DIRECTION_CLOCKWISE) ? 1 : -1;
   Elm_Scroller_Movement_Block scroller_block;
   Eina_Bool scroll_locked_x, scroll_locked_y;

   EEXT_CIRCLE_OBJECT_SCROLLER_DATA_GET(circle_obj, widget_data) return EINA_TRUE;

   if (elm_object_scroll_freeze_get(scroller))
     return EINA_TRUE;

   elm_scroller_page_size_get(scroller, &h_pagesize, &v_pagesize);
   elm_scroller_child_size_get(scroller, &cw, &ch);
   elm_scroller_region_get(scroller, &x, &y, &w, &h);

   scroller_block = elm_scroller_movement_block_get(scroller);
   scroll_locked_x = elm_object_scroll_lock_x_get(scroller);
   scroll_locked_y = elm_object_scroll_lock_y_get(scroller);

   if ((h_pagesize > 0) || (v_pagesize > 0))
     {
        int h_current_page, v_current_page;

        // Handle scroll per page
        elm_scroller_current_page_get(scroller, &h_current_page, &v_current_page);

        if (!scroll_locked_y &&
            !(scroller_block & ELM_SCROLLER_MOVEMENT_BLOCK_VERTICAL) &&
            (ch > h) && (v_pagesize > 0))
          {
             v_current_page += d;
          }
        else if (!scroll_locked_x &&
                 !(scroller_block & ELM_SCROLLER_MOVEMENT_BLOCK_HORIZONTAL) &&
                 (cw > w) && (h_pagesize > 0))
          {
             h_current_page += d;
          }

        if (v_current_page < 0) v_current_page = 0;
        if (h_current_page < 0) h_current_page = 0;

        elm_scroller_page_bring_in(scroller, h_current_page,v_current_page);
     }
   else
     {
        // Handle scroll
        if (!scroll_locked_y &&
            !(scroller_block & ELM_SCROLLER_MOVEMENT_BLOCK_VERTICAL) &&
            (ch > h))
          {
             y += d * widget_data->step_y;
          }
        else if (!scroll_locked_x &&
                 !(scroller_block & ELM_SCROLLER_MOVEMENT_BLOCK_HORIZONTAL) &&
                 (cw > w))
          {
             x += d * widget_data->step_x;
          }

        if (y < 0) y = 0;
        if (x < 0) x = 0;

        elm_scroller_region_bring_in(scroller, x, y, w, h);
     }

   return EINA_TRUE;
}

static void
_eext_circle_object_scroller_del_cb(Eext_Circle_Object *obj)
{
   EEXT_CIRCLE_OBJECT_SCROLLER_DATA_GET(obj, data) return;

   evas_object_event_callback_del_full(obj->widget_object, EVAS_CALLBACK_SHOW, _eext_circle_object_scroller_show_cb, obj);
   evas_object_smart_callback_del_full(obj->widget_object, "scroll", _eext_circle_object_scroller_scroll_cb, obj);
   evas_object_smart_callback_del_full(obj->widget_object, "content,resize", _eext_circle_object_scroller_content_resize_cb, obj);
   eext_rotary_object_event_callback_del(obj->widget_object, _eext_circle_object_scroller_rotary_changed_cb);
   eext_rotary_object_event_callback_del(obj->image_widget, _eext_circle_object_scroller_rotary_changed_cb);

   if (data->bar_hide_timer)
     {
        ecore_timer_del(data->bar_hide_timer);
        data->bar_hide_timer = NULL;
     }

   free(obj->widget_data);
   obj->widget_data = NULL;
}

static void
_eext_circle_object_scroller_disabled_cb(Eext_Circle_Object *obj)
{
   Eext_Circle_Object_Item *item;

   EEXT_CIRCLE_OBJECT_SCROLLER_DATA_GET(obj, data) return;

   if (obj->disabled)
     {
        item = _eext_circle_object_item_get(obj, VERTICAL_BAR_NAME);
        _eext_circle_object_item_color_set(item,
                                           data->disabled_color.r,
                                           data->disabled_color.g,
                                           data->disabled_color.b,
                                           data->disabled_color.a);

        item = _eext_circle_object_item_get(obj, VERTICAL_BAR_BG_NAME);
        _eext_circle_object_item_color_set(item,
                                           data->disabled_bg_color.r,
                                           data->disabled_bg_color.g,
                                           data->disabled_bg_color.b,
                                           data->disabled_bg_color.a);

        item = _eext_circle_object_item_get(obj, HORIZONTAL_BAR_NAME);
        _eext_circle_object_item_color_set(item,
                                           data->disabled_color.r,
                                           data->disabled_color.g,
                                           data->disabled_color.b,
                                           data->disabled_color.a);

        item = _eext_circle_object_item_get(obj, HORIZONTAL_BAR_BG_NAME);
        _eext_circle_object_item_color_set(item,
                                           data->disabled_bg_color.r,
                                           data->disabled_bg_color.g,
                                           data->disabled_bg_color.b,
                                           data->disabled_bg_color.a);

        eext_rotary_object_event_callback_del(obj->widget_object, _eext_circle_object_scroller_rotary_changed_cb);
        eext_rotary_object_event_callback_del(obj->image_widget, _eext_circle_object_scroller_rotary_changed_cb);
     }
   else
     {
        item = _eext_circle_object_item_get(obj, VERTICAL_BAR_NAME);
        _eext_circle_object_item_color_set(item,
                                           data->vertical_color.r,
                                           data->vertical_color.g,
                                           data->vertical_color.b,
                                           data->vertical_color.a);

        item = _eext_circle_object_item_get(obj, VERTICAL_BAR_BG_NAME);
        _eext_circle_object_item_color_set(item,
                                           data->vertical_bg_color.r,
                                           data->vertical_bg_color.g,
                                           data->vertical_bg_color.b,
                                           data->vertical_bg_color.a);

        item = _eext_circle_object_item_get(obj, HORIZONTAL_BAR_NAME);
        _eext_circle_object_item_color_set(item,
                                           data->horizontal_color.r,
                                           data->horizontal_color.g,
                                           data->horizontal_color.b,
                                           data->horizontal_color.a);

        item = _eext_circle_object_item_get(obj, HORIZONTAL_BAR_BG_NAME);
        _eext_circle_object_item_color_set(item,
                                           data->horizontal_bg_color.r,
                                           data->horizontal_bg_color.g,
                                           data->horizontal_bg_color.b,
                                           data->horizontal_bg_color.a);

        eext_rotary_object_event_callback_add(obj->widget_object, _eext_circle_object_scroller_rotary_changed_cb, obj);
        eext_rotary_object_event_callback_add(obj->image_widget, _eext_circle_object_scroller_rotary_changed_cb, obj);
     }
}

static void
_eext_circle_object_scroller_init(Eext_Circle_Object *obj, Evas_Object *scroller)
{
   Eext_Circle_Object_Scroller_Data *data;
   Eext_Circle_Object_Item *item;

   obj->widget_type = EEXT_CIRCLE_OBJECT_SCROLLER_TYPE;
   obj->del_func = _eext_circle_object_scroller_del_cb;
   obj->disabled_func = _eext_circle_object_scroller_disabled_cb;
   obj->color_changed_func = _eext_circle_object_scroller_color_changed_cb;

   data = (Eext_Circle_Object_Scroller_Data *)calloc(1, sizeof(Eext_Circle_Object_Scroller_Data));
   data->policy_v = ELM_SCROLLER_POLICY_AUTO;
   data->policy_h = ELM_SCROLLER_POLICY_AUTO;
   data->line_width = 3;
   data->radius = 169;
   data->step_x = data->step_y = 32;
   data->vertical_color.r = data->horizontal_color.r = 0;
   data->vertical_color.g = data->horizontal_color.g = 148;
   data->vertical_color.b = data->horizontal_color.b = 255;
   data->vertical_color.a = data->horizontal_color.a = 255;
   data->vertical_bg_color.r = data->horizontal_bg_color.r = 0;
   data->vertical_bg_color.g = data->horizontal_bg_color.g = 148;
   data->vertical_bg_color.b = data->horizontal_bg_color.b = 255;
   data->vertical_bg_color.a = data->horizontal_bg_color.a = 63;
   data->disabled_color.r = 45;
   data->disabled_color.g = 45;
   data->disabled_color.b = 45;
   data->disabled_color.a = 255;
   data->disabled_bg_color.r = 45;
   data->disabled_bg_color.g = 45;
   data->disabled_bg_color.b = 45;
   data->disabled_bg_color.a = 63;
   obj->widget_data = (void *)data;

   // We don't want to allow to change properties of these items.
   // There is no item with "default" as its name.
   // So, eext_circle_object_* APIs is meaningless in here.
   item = _eext_circle_object_item_new();
   _eext_circle_object_item_name_set(item, VERTICAL_BAR_BG_NAME);
   _eext_circle_object_item_color_set(item,
                                      data->vertical_bg_color.r,
                                      data->vertical_bg_color.g,
                                      data->vertical_bg_color.b,
                                      0);
   _eext_circle_object_item_line_width_set(item, data->line_width);
   _eext_circle_object_item_radius_set(item, data->radius);
   _eext_circle_object_item_angle_offset_set(item, VERTICAL_BAR_ANGLE_START);
   _eext_circle_object_item_angle_set(item, VERTICAL_BAR_ANGLE_RANGE);
   _eext_circle_object_item_append(obj, item);

   item = _eext_circle_object_item_new();
   _eext_circle_object_item_name_set(item, HORIZONTAL_BAR_BG_NAME);
   _eext_circle_object_item_color_set(item,
                                      data->horizontal_bg_color.r,
                                      data->horizontal_bg_color.g,
                                      data->horizontal_bg_color.b,
                                      0);
   _eext_circle_object_item_line_width_set(item, data->line_width);
   _eext_circle_object_item_radius_set(item, data->radius);
   _eext_circle_object_item_angle_offset_set(item, HORIZONTAL_BAR_ANGLE_START);
   _eext_circle_object_item_angle_set(item, HORIZONTAL_BAR_ANGLE_RANGE);
   _eext_circle_object_item_append(obj, item);

   item = _eext_circle_object_item_new();
   _eext_circle_object_item_name_set(item, VERTICAL_BAR_NAME);
   _eext_circle_object_item_color_set(item,
                                      data->vertical_color.r,
                                      data->vertical_color.g,
                                      data->vertical_color.b,
                                      data->vertical_color.a);
   _eext_circle_object_item_line_width_set(item, data->line_width);
   _eext_circle_object_item_radius_set(item, data->radius);
   _eext_circle_object_item_append(obj, item);

   item = _eext_circle_object_item_new();
   _eext_circle_object_item_name_set(item, HORIZONTAL_BAR_NAME);
   _eext_circle_object_item_color_set(item,
                                      data->horizontal_color.r,
                                      data->horizontal_color.g,
                                      data->horizontal_color.b,
                                      data->horizontal_color.a);
   _eext_circle_object_item_line_width_set(item, data->line_width);
   _eext_circle_object_item_radius_set(item, data->radius);
   _eext_circle_object_item_append(obj, item);

   elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   evas_object_event_callback_add(scroller, EVAS_CALLBACK_SHOW, _eext_circle_object_scroller_show_cb, obj);
   evas_object_smart_callback_add(scroller, "scroll", _eext_circle_object_scroller_scroll_cb, obj);
   evas_object_smart_callback_add(scroller, "content,resize", _eext_circle_object_scroller_content_resize_cb, obj);

   eext_rotary_object_event_callback_add(scroller, _eext_circle_object_scroller_rotary_changed_cb, obj);
   eext_rotary_object_event_callback_add(obj->image_widget, _eext_circle_object_scroller_rotary_changed_cb, obj);
}

EAPI Evas_Object *
eext_circle_object_scroller_add(Evas_Object *scroller, Eext_Circle_Surface *surface)
{
   Evas_Object *obj;

   if (!scroller) return NULL;

   obj = elm_image_add(scroller);
   _eext_circle_object_init(obj, scroller, surface);

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return NULL;

   _eext_circle_object_scroller_init(circle_obj, scroller);

   return obj;
}

EAPI void
eext_circle_object_scroller_policy_set(Evas_Object *obj,
                                       Elm_Scroller_Policy policy_h,
                                       Elm_Scroller_Policy policy_v)
{
   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return;
   EEXT_CIRCLE_OBJECT_SCROLLER_DATA_GET(circle_obj, data) return;

   if ((data->policy_v == policy_v) && (data->policy_h == policy_h))
     return;

   if (policy_v < ELM_SCROLLER_POLICY_LAST)
     data->policy_v = policy_v;

   if (policy_h < ELM_SCROLLER_POLICY_LAST)
     data->policy_h = policy_h;

   _eext_circle_object_scroller_scrollbar_update(circle_obj,
                                                 SCROLL_BAR_HIDE_WAITTING_TIME_ON_INITIALIZING);
}

EAPI void
eext_circle_object_scroller_policy_get(const Evas_Object *obj,
                                       Elm_Scroller_Policy *policy_h,
                                       Elm_Scroller_Policy *policy_v)
{
   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return;
   EEXT_CIRCLE_OBJECT_SCROLLER_DATA_GET(circle_obj, data) return;

   if (policy_v) *policy_v = data->policy_v;
   if (policy_h) *policy_h = data->policy_h;
}
