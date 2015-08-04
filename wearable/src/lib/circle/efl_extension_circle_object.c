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

static void
_eext_circle_object_del_cb(void *data,
                           Evas *e,
                           Evas_Object *obj,
                           void *event_info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;
   Eext_Circle_Object_Item *item;
   Eina_List *l, *l_next;

   if (!circle_obj)
     {
        LOGE("Eext_Circle_Object is NULL");
        return;
     }

   if (circle_obj->del_func)
     circle_obj->del_func(circle_obj);

   EINA_LIST_FOREACH_SAFE(circle_obj->items, l, l_next, item)
     {
        _eext_circle_object_item_free(item);
     }

   if (circle_obj->surface)
     {
        _eext_circle_surface_object_remove(circle_obj->surface, circle_obj);
        circle_obj->surface = NULL;
     }

   circle_obj->visible = EINA_FALSE;

   if (obj)
     evas_object_data_del(obj, EEXT_CIRCLE_OBJECT_TYPE);

   free(circle_obj);
}

static void
_eext_circle_object_show_cb(void *data,
                            Evas *e,
                            Evas_Object *obj,
                            void *event_info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;

   if (!circle_obj) return;
   if (circle_obj->visible) return;

   if (circle_obj->widget_object)
     {
        if (evas_object_visible_get(circle_obj->widget_object))
          {
             circle_obj->visible = EINA_TRUE;
             _eext_circle_object_changed(circle_obj);
          }
     }
   else
     {
        circle_obj->visible = EINA_TRUE;
        _eext_circle_object_changed(circle_obj);
     }
}

static void
_eext_circle_object_hide_cb(void *data,
                            Evas *e,
                            Evas_Object *obj,
                            void *event_info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;

   if (!circle_obj) return;
   if (!circle_obj->visible) return;

   circle_obj->visible = EINA_FALSE;
   _eext_circle_object_changed(circle_obj);
}

static void
_eext_circle_object_widget_object_show_cb(void *data,
                                          Evas *e,
                                          Evas_Object *obj,
                                          void *event_info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;

   if (!circle_obj) return;
   if (circle_obj->visible) return;

   if (evas_object_visible_get(circle_obj->image_widget))
     {
        circle_obj->visible = EINA_TRUE;
        _eext_circle_object_changed(circle_obj);
     }
}

static void
_eext_circle_object_widget_object_hide_cb(void *data,
                                          Evas *e,
                                          Evas_Object *obj,
                                          void *event_info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;

   evas_object_hide(circle_obj->image_widget);
}

static void
_eext_circle_object_widget_object_disabled_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;
   Evas_Object *main_obj = circle_obj->image_widget;

   if (eext_circle_object_disabled_get(main_obj) != elm_object_disabled_get(obj))
     eext_circle_object_disabled_set(main_obj, EINA_TRUE);
}

static void
_eext_circle_object_widget_object_enabled_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;
   Evas_Object *main_obj = circle_obj->image_widget;

   if (eext_circle_object_disabled_get(main_obj) != elm_object_disabled_get(obj))
     eext_circle_object_disabled_set(main_obj, EINA_FALSE);
}

//////////////////////////////
// Internal API //////////////
//////////////////////////////
void
_eext_circle_object_init(Evas_Object *obj, Evas_Object *widget_obj, Eext_Circle_Surface *surface)
{
   Eext_Circle_Object *circle_obj;

   circle_obj = (Eext_Circle_Object *)calloc(1, sizeof(Eext_Circle_Object));
   // Obj is expected to be elm_image object.
   circle_obj->image_widget = obj;
   circle_obj->widget_object = widget_obj;
   circle_obj->widget_data = NULL;
   circle_obj->widget_type = EEXT_CIRCLE_OBJECT_TYPE;
   circle_obj->del_func = NULL;
   circle_obj->disabled_func = NULL;

   evas_object_data_set(obj, EEXT_CIRCLE_OBJECT_TYPE, circle_obj);

   evas_object_event_callback_add(obj, EVAS_CALLBACK_DEL, _eext_circle_object_del_cb, circle_obj);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_SHOW, _eext_circle_object_show_cb, circle_obj);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_HIDE, _eext_circle_object_hide_cb, circle_obj);

   if (widget_obj)
     {
        evas_object_event_callback_add(widget_obj, EVAS_CALLBACK_SHOW,
                                       _eext_circle_object_widget_object_show_cb, circle_obj);
        evas_object_event_callback_add(widget_obj, EVAS_CALLBACK_HIDE,
                                       _eext_circle_object_widget_object_hide_cb, circle_obj);
        evas_object_smart_callback_add(widget_obj, "disabled", _eext_circle_object_widget_object_disabled_cb, circle_obj);
        evas_object_smart_callback_add(widget_obj, "enabled", _eext_circle_object_widget_object_enabled_cb, circle_obj);
     }

   if (surface)
     _eext_circle_surface_object_append(surface, circle_obj);
   else
     {
        Eext_Circle_Surface *private_surface =
           _eext_circle_surface_init(circle_obj->image_widget, NULL, EEXT_CIRCLE_SURFACE_TYPE_PRIVATE);
        _eext_circle_surface_object_append(private_surface, circle_obj);
     }
}

void
_eext_circle_object_changed(Eext_Circle_Object *obj)
{
   if (!obj) return;

   if (obj->surface)
     _eext_circle_surface_changed(obj->surface);
}

Eext_Circle_Object_Item *
_eext_circle_object_item_get(const Eext_Circle_Object *obj, const char *name)
{
   Eina_List *l;
   Eext_Circle_Object_Item *item;

   if (!obj)
     {
        LOGE("Eext_Circle_Object is NULL! It returns NULL!");
        return NULL;
     }

   if (!name)
     {
        LOGE("The given item name is NULL! It returns NULL!");
        return NULL;
     }

   EINA_LIST_FOREACH(obj->items, l, item)
     {
        const char *item_name = _eext_circle_object_item_name_get(item);
        if (item_name &&
            (name && !strcmp(_eext_circle_object_item_name_get(item), name)))
          return item;
     }

   LOGE("There is no [%s] item in the given object[%p:%s]! It returns NULL!", name, obj->image_widget, obj->widget_type);
   return NULL;
}

void
_eext_circle_object_hide(Eext_Circle_Object *obj)
{
   if (!obj) return;
   if (!obj->visible) return;

   obj->visible = EINA_FALSE;

   _eext_circle_object_changed(obj);
}

void
_eext_circle_object_show(Eext_Circle_Object *obj)
{
   if (!obj) return;
   if (obj->visible) return;

   obj->visible = EINA_TRUE;

   if (obj->surface)
     _eext_circle_surface_object_exclusive_show(obj->surface, obj);

   _eext_circle_object_changed(obj);
}

/////////////////////////////////////////////////
// EAPI /////////////////////////////////////////
/////////////////////////////////////////////////
EAPI void
eext_circle_object_line_width_set(Evas_Object *obj, int line_width)
{
   eext_circle_object_item_line_width_set(obj, "default", line_width);
}

EAPI int
eext_circle_object_line_width_get(const Evas_Object *obj)
{
   return eext_circle_object_item_line_width_get(obj, "default");
}

EAPI void
eext_circle_object_angle_set(Evas_Object *obj, double angle)
{
   eext_circle_object_item_angle_set(obj, "default", angle);
}

EAPI double
eext_circle_object_angle_get(const Evas_Object *obj)
{
   return eext_circle_object_item_angle_get(obj, "default");
}

EAPI void
eext_circle_object_angle_offset_set(Evas_Object *obj, double angle_offset)
{
   eext_circle_object_item_angle_offset_set(obj, "default", angle_offset);
}

EAPI double
eext_circle_object_angle_offset_get(const Evas_Object *obj)
{
   return eext_circle_object_item_angle_offset_get(obj, "default");
}

EAPI void
eext_circle_object_angle_min_max_set(Evas_Object *obj, double min_angle, double max_angle)
{
   eext_circle_object_item_angle_min_max_set(obj, "default", min_angle, max_angle);
}

EAPI void
eext_circle_object_angle_min_max_get(const Evas_Object *obj, double *min_angle, double *max_angle)
{
   eext_circle_object_item_angle_min_max_get(obj, "default", min_angle, max_angle);
}

EAPI void
eext_circle_object_value_min_max_set(Evas_Object *obj, double min_value, double max_value)
{
   eext_circle_object_item_value_min_max_set(obj, "default", min_value, max_value);
}

EAPI void
eext_circle_object_value_min_max_get(const Evas_Object *obj, double *min_value, double *max_value)
{
   eext_circle_object_item_value_min_max_get(obj, "default", min_value, max_value);
}

EAPI void
eext_circle_object_value_set(Evas_Object *obj, double value)
{
   eext_circle_object_item_value_set(obj, "default", value);
}

EAPI double
eext_circle_object_value_get(const Evas_Object *obj)
{
   return eext_circle_object_item_value_get(obj, "default");
}

EAPI void
eext_circle_object_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
   eext_circle_object_item_color_set(obj, "default", r, g, b, a);
}

EAPI void
eext_circle_object_color_get(const Evas_Object *obj, int *r, int *g, int *b, int *a)
{
   eext_circle_object_item_color_get(obj, "default", r, g, b, a);
}

EAPI void
eext_circle_object_radius_set(Evas_Object *obj, double radius)
{
   eext_circle_object_item_radius_set(obj, "default", radius);
}

EAPI double
eext_circle_object_radius_get(const Evas_Object *obj)
{
   return eext_circle_object_item_radius_get(obj, "default");
}

EAPI void
eext_circle_object_disabled_set(Evas_Object *obj, Eina_Bool disabled)
{
   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return;

   if (circle_obj->disabled == disabled) return;

   circle_obj->disabled = disabled;

   if (circle_obj->disabled_func)
     circle_obj->disabled_func(circle_obj);

   if (circle_obj->widget_object &&
       (elm_object_disabled_get(circle_obj->widget_object) != disabled))
     elm_object_disabled_set(circle_obj->widget_object, disabled);
}

EAPI Eina_Bool
eext_circle_object_disabled_get(Evas_Object *obj)
{
   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return EINA_FALSE;

   return circle_obj->disabled;
}

EAPI Evas_Object *
eext_circle_object_add(Evas_Object *parent, Eext_Circle_Surface *surface)
{
   Evas_Object *obj;
   Eext_Circle_Object_Item *item;

   obj = elm_image_add(parent);
   _eext_circle_object_init(obj, NULL, surface);

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return NULL;

   item = _eext_circle_object_item_new();
   _eext_circle_object_item_append(circle_obj, item);

   return obj;
}

EAPI void
eext_circle_object_item_line_width_set(Evas_Object *obj, const char *item_name, int line_width)
{
   Eext_Circle_Object_Item *item = NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return;

   item = _eext_circle_object_item_get(circle_obj, item_name);

   if (item)
     _eext_circle_object_item_line_width_set(item, line_width);
}

EAPI int
eext_circle_object_item_line_width_get(const Evas_Object *obj, const char *item_name)
{
   Eext_Circle_Object_Item *item = NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return -1;

   item = _eext_circle_object_item_get(circle_obj, item_name);

   if (item)
     return _eext_circle_object_item_line_width_get(item);

   return -1;
}

EAPI void
eext_circle_object_item_angle_set(Evas_Object *obj, const char *item_name, double angle)
{
   Eext_Circle_Object_Item *item = NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return;

   item = _eext_circle_object_item_get(circle_obj, item_name);

   if (item)
     _eext_circle_object_item_angle_set(item, angle);
}

EAPI double
eext_circle_object_item_angle_get(const Evas_Object *obj, const char *item_name)
{
   Eext_Circle_Object_Item *item = NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return -1.0;

   item = _eext_circle_object_item_get(circle_obj, item_name);

   if (item)
     return _eext_circle_object_item_angle_get(item);

   return -1.0;
}

EAPI void
eext_circle_object_item_angle_offset_set(Evas_Object *obj, const char *item_name, double angle_offset)
{
   Eext_Circle_Object_Item *item = NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return;

   item = _eext_circle_object_item_get(circle_obj, item_name);

   if (item)
     _eext_circle_object_item_angle_offset_set(item, angle_offset);
}

EAPI double
eext_circle_object_item_angle_offset_get(const Evas_Object *obj, const char *item_name)
{
   Eext_Circle_Object_Item *item = NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return -1.0;

   item = _eext_circle_object_item_get(circle_obj, item_name);

   if (item)
     return _eext_circle_object_item_angle_offset_get(item);

   return -1.0;
}

EAPI void
eext_circle_object_item_angle_min_max_set(Evas_Object *obj, const char *item_name, double min_angle, double max_angle)
{
   Eext_Circle_Object_Item *item = NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return;

   item = _eext_circle_object_item_get(circle_obj, item_name);

   if (item)
     _eext_circle_object_item_angle_min_max_set(item, min_angle, max_angle);
}

EAPI void
eext_circle_object_item_angle_min_max_get(const Evas_Object *obj, const char *item_name, double *min_angle, double *max_angle)
{
   Eext_Circle_Object_Item *item = NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj)
     {
        if (min_angle) *min_angle = 0;
        if (max_angle) *max_angle = 0;

        return;
     }

   item = _eext_circle_object_item_get(circle_obj, item_name);

   if (item)
     {
        _eext_circle_object_item_angle_min_max_get(item, min_angle, max_angle);
     }
   else
     {
        if (min_angle) *min_angle = 0;
        if (max_angle) *max_angle = 0;
     }
}

EAPI void
eext_circle_object_item_value_min_max_set(Evas_Object *obj, const char *item_name, double min_value, double max_value)
{
   Eext_Circle_Object_Item *item = NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return;

   item = _eext_circle_object_item_get(circle_obj, item_name);

   if (item)
     _eext_circle_object_item_value_min_max_set(item, min_value, max_value);
}

EAPI void
eext_circle_object_item_value_min_max_get(const Evas_Object *obj, const char *item_name, double *min_value, double *max_value)
{
   Eext_Circle_Object_Item *item = NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj)
     {
        if (min_value) *min_value = 0;
        if (max_value) *max_value = 0;

        return;
     }

   item = _eext_circle_object_item_get(circle_obj, item_name);

   if (item)
     {
        _eext_circle_object_item_value_min_max_get(item, min_value, max_value);
     }
   else
     {
        if (min_value) *min_value = 0;
        if (max_value) *max_value = 0;
     }
}

EAPI void
eext_circle_object_item_value_set(Evas_Object *obj, const char *item_name, double value)
{
   Eext_Circle_Object_Item *item = NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return;

   item = _eext_circle_object_item_get(circle_obj, item_name);

   if (item)
     _eext_circle_object_item_value_set(item, value);
}

EAPI double
eext_circle_object_item_value_get(const Evas_Object *obj, const char *item_name)
{
   Eext_Circle_Object_Item *item = NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return -1.0;

   item = _eext_circle_object_item_get(circle_obj, item_name);

   if (item)
     return _eext_circle_object_item_value_get(item);

   return -1.0;
}

EAPI void
eext_circle_object_item_color_set(Evas_Object *obj, const char *item_name, int r, int g, int b, int a)
{
   Eext_Circle_Object_Item *item = NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return;

   item = _eext_circle_object_item_get(circle_obj, item_name);

   if (item)
     {
        _eext_circle_object_item_color_set(item, r, g, b, a);

        if (circle_obj->color_changed_func)
          {
             if (item_name)
               circle_obj->color_changed_func(circle_obj, item_name);
             else
               circle_obj->color_changed_func(circle_obj, "default");
          }
     }
}

EAPI void
eext_circle_object_item_color_get(const Evas_Object *obj, const char *item_name, int *r, int *g, int *b, int *a)
{
   Eext_Circle_Object_Item *item = NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj)
     {
        if (r) *r = 0;
        if (g) *g = 0;
        if (b) *b = 0;
        if (a) *a = 0;

        return;
     }

   item = _eext_circle_object_item_get(circle_obj, item_name);

   if (item)
     {
        _eext_circle_object_item_color_get(item, r, g, b, a);
     }
   else
     {
        if (r) *r = 0;
        if (g) *g = 0;
        if (b) *b = 0;
        if (a) *a = 0;
     }
}

EAPI void
eext_circle_object_item_radius_set(Evas_Object *obj, const char *item_name, double radius)
{
   Eext_Circle_Object_Item *item = NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return;

   item = _eext_circle_object_item_get(circle_obj, item_name);

   if (item)
     _eext_circle_object_item_radius_set(item, radius);
}

EAPI double
eext_circle_object_item_radius_get(const Evas_Object *obj, const char *item_name)
{
   Eext_Circle_Object_Item *item = NULL;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return -1.0;

   item = _eext_circle_object_item_get(circle_obj, item_name);

   if (item)
     return _eext_circle_object_item_radius_get(item);

   return -1.0;
}
