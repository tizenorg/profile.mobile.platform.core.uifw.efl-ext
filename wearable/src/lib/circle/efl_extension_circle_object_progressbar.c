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

#define EEXT_CIRCLE_OBJECT_PROGRESSBAR_TYPE "Eext_Circle_Progressbar"

#define EEXT_CIRCLE_OBJECT_PROGRESSBAR_DATA_GET(circle_obj, data) \
      Eext_Circle_Object_Progressbar_Data *data = NULL; \
   if (circle_obj && circle_obj->widget_data && \
              circle_obj->widget_type && !strcmp(circle_obj->widget_type, EEXT_CIRCLE_OBJECT_PROGRESSBAR_TYPE)) \
      data = (Eext_Circle_Object_Progressbar_Data *)circle_obj->widget_data; \
   if (!data)

#define PROGRESSBAR_ITEM_NAME "default"
#define PROGRESSBAR_BG_ITEM_NAME "bg"

static void
_eext_circle_object_progressbar_del_cb(Eext_Circle_Object *obj)
{
   if (obj->widget_data)
     {
        free(obj->widget_data);
        obj->widget_data = NULL;
     }
}

static void
_eext_circle_object_progressbar_color_changed_cb(Eext_Circle_Object *obj, const char *item_name)
{
   Eext_Circle_Object_Item *item;
   int r, g, b, a;

   EEXT_CIRCLE_OBJECT_PROGRESSBAR_DATA_GET(obj, widget_data) return;

   item = _eext_circle_object_item_get(obj, item_name);
   if (!item) return;

   _eext_circle_object_item_color_get(item, &r, &g, &b, &a);

   if (!strcmp(item_name, PROGRESSBAR_ITEM_NAME))
     {
        widget_data->color.r = r;
        widget_data->color.g = g;
        widget_data->color.b = b;
        widget_data->color.a = a;

        return;
     }

   if (!strcmp(item_name, PROGRESSBAR_BG_ITEM_NAME))
     {
        widget_data->bg_color.r = r;
        widget_data->bg_color.g = g;
        widget_data->bg_color.b = b;
        widget_data->bg_color.a = a;

        return;
     }
}

static void
_eext_circle_object_progressbar_disabled_cb(Eext_Circle_Object *obj)
{
   Eext_Circle_Object_Item *item, *bg_item;

   EEXT_CIRCLE_OBJECT_PROGRESSBAR_DATA_GET(obj, data) return;

   item = _eext_circle_object_item_get(obj, PROGRESSBAR_ITEM_NAME);
   bg_item = _eext_circle_object_item_get(obj, PROGRESSBAR_BG_ITEM_NAME);

   if (obj->disabled)
     {
        if (item)
          _eext_circle_object_item_color_set(item,
                                             data->disabled_color.r,
                                             data->disabled_color.g,
                                             data->disabled_color.b,
                                             data->disabled_color.a);
        if (bg_item)
          _eext_circle_object_item_color_set(bg_item,
                                             data->disabled_bg_color.r,
                                             data->disabled_bg_color.g,
                                             data->disabled_bg_color.b,
                                             data->disabled_bg_color.a);
     }
   else
     {
        if (item)
          _eext_circle_object_item_color_set(item,
                                             data->color.r,
                                             data->color.g,
                                             data->color.b,
                                             data->color.a);
        if (bg_item)
          _eext_circle_object_item_color_set(bg_item,
                                             data->bg_color.r,
                                             data->bg_color.g,
                                             data->bg_color.b,
                                             data->bg_color.a);
     }
}

static void
_eext_circle_object_progressbar_init(Eext_Circle_Object *obj)
{
   Eext_Circle_Object_Item *item;
   Eext_Circle_Object_Progressbar_Data *data;

   obj->widget_object = NULL;
   obj->widget_type = EEXT_CIRCLE_OBJECT_PROGRESSBAR_TYPE;
   obj->del_func = _eext_circle_object_progressbar_del_cb;
   obj->disabled_func = _eext_circle_object_progressbar_disabled_cb;
   obj->color_changed_func = _eext_circle_object_progressbar_color_changed_cb;

   data = (Eext_Circle_Object_Progressbar_Data *)calloc(1, sizeof(Eext_Circle_Object_Progressbar_Data));
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
   _eext_circle_object_item_name_set(item, PROGRESSBAR_BG_ITEM_NAME);
   _eext_circle_object_item_color_set(item, data->bg_color.r, data->bg_color.g, data->bg_color.b, data->bg_color.a);
   _eext_circle_object_item_radius_set(item, 176);
   _eext_circle_object_item_line_width_set(item, 8);
   _eext_circle_object_item_angle_set(item, 360);
   _eext_circle_object_item_append(obj, item);

   item = _eext_circle_object_item_new();
   _eext_circle_object_item_name_set(item, PROGRESSBAR_ITEM_NAME);
   _eext_circle_object_item_color_set(item, data->color.r, data->color.g, data->color.b, data->color.a);
   _eext_circle_object_item_radius_set(item, 176);
   _eext_circle_object_item_line_width_set(item, 8);
   _eext_circle_object_item_append(obj, item);
}

EAPI Evas_Object *
eext_circle_object_progressbar_add(Evas_Object *parent, Eext_Circle_Surface *surface)
{
   Evas_Object *obj;

   if (!parent) return NULL;

   obj = elm_image_add(parent);
   _eext_circle_object_init(obj, NULL, surface);

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return NULL;

   _eext_circle_object_progressbar_init(circle_obj);

   return obj;
}
