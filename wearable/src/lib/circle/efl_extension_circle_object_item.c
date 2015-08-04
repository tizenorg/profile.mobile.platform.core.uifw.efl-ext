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

static void _eext_circle_object_item_color_transit_animator_del(Eext_Circle_Object_Item *item);
static void _eext_circle_object_item_angle_transit_animator_del(Eext_Circle_Object_Item *item);

static Eina_Bool
_eext_circle_object_item_color_transit_animator_cb(void *data, double pos)
{
   Eext_Circle_Object_Item *item = (Eext_Circle_Object_Item *)data;
   int diff_r, diff_g, diff_b, diff_a;
   double mapped_pos;

   diff_r = item->color_transit.from_color.r - item->color.r;
   diff_g = item->color_transit.from_color.g - item->color.g;
   diff_b = item->color_transit.from_color.b - item->color.b;
   diff_a = item->color_transit.from_color.a - item->color.a;

   item->color_transit.current_animator_pos = pos;

   if (item->color_transit.cubic_bezier_enabled)
     mapped_pos = ecore_animator_pos_map_n(pos,
                                           ECORE_POS_MAP_CUBIC_BEZIER,
                                           4,
                                           item->color_transit.cubic_bezier_v);
   else
     mapped_pos = pos;

   item->draw.color.r = item->color_transit.from_color.r - diff_r * mapped_pos;
   item->draw.color.g = item->color_transit.from_color.g - diff_g * mapped_pos;
   item->draw.color.b = item->color_transit.from_color.b - diff_b * mapped_pos;
   item->draw.color.a = item->color_transit.from_color.a - diff_a * mapped_pos;

   _eext_circle_object_changed(item->obj);

   if (pos >= 1.0)
     {
        _eext_circle_object_item_color_transit_animator_del(item);
        return ECORE_CALLBACK_CANCEL;
     }

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_eext_circle_object_item_angle_transit_animator_cb(void *data, double pos)
{
   Eext_Circle_Object_Item *item = (Eext_Circle_Object_Item *)data;
   double diff_angle, diff_angle_offset;
   double mapped_pos;

   diff_angle = item->angle_transit.from_angle - item->angle;
   diff_angle_offset =
      item->angle_transit.from_angle_offset - item->angle_offset;

   item->angle_transit.current_animator_pos = pos;

   if (item->angle_transit.cubic_bezier_enabled)
     mapped_pos = ecore_animator_pos_map_n(pos,
                                           ECORE_POS_MAP_CUBIC_BEZIER,
                                           4,
                                           item->angle_transit.cubic_bezier_v);
   else
     mapped_pos = pos;

   item->draw.angle = item->angle_transit.from_angle - diff_angle * mapped_pos;
   item->draw.angle_offset = item->angle_transit.from_angle_offset - diff_angle_offset * mapped_pos;

   _eext_circle_object_changed(item->obj);

   if (pos >= 1.0)
     {
        _eext_circle_object_item_angle_transit_animator_del(item);
        return ECORE_CALLBACK_CANCEL;
     }

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_eext_circle_object_item_angle_set_internal(Eext_Circle_Object_Item *item, double angle)
{
   double temp_angle;

   if (item->angle == angle) return EINA_FALSE;

   temp_angle = angle;

   if (angle < item->min_angle) temp_angle = item->min_angle;
   if (angle > item->max_angle) temp_angle = item->max_angle;

   if (item->angle == temp_angle) return EINA_FALSE;

   item->angle = temp_angle;
   item->value = _eext_circle_object_item_angle_value_get(item, item->angle);

   return EINA_TRUE;
}

static Eina_Bool
_eext_circle_object_item_angle_offset_set_internal(Eext_Circle_Object_Item *item, double angle_offset)
{
   if (item->angle_offset == angle_offset) return EINA_FALSE;

   item->angle_offset = angle_offset;

   return EINA_TRUE;
}

static Eina_Bool
_eext_circle_object_item_value_set_internal(Eext_Circle_Object_Item *item, double value)
{
   double angle;

   if (item->value == value) return EINA_FALSE;

   item->value = value;

   if (item->value < item->min_value) item->value = item->min_value;
   if (item->value > item->max_value) item->value = item->max_value;

   angle = _eext_circle_object_item_value_angle_get(item, item->value);

   if (item->angle == angle) return EINA_FALSE;

   item->angle = angle;

   return EINA_TRUE;
}

static Eina_Bool
_eext_circle_object_item_color_set_internal(Eext_Circle_Object_Item *item,
                                            int r, int g, int b, int a)
{
   if (r > 255) r = 255; if (r < 0) r = 0;
   if (g > 255) g = 255; if (g < 0) g = 0;
   if (b > 255) b = 255; if (b < 0) b = 0;
   if (a > 255) a = 255; if (a < 0) a = 0;

   if ((item->color.r == r) && (item->color.g == g) && (item->color.b == b) && (item->color.a == a))
     return EINA_FALSE;

   item->color.r = r;
   item->color.g = g;
   item->color.b = b;
   item->color.a = a;

   return EINA_TRUE;
}

static void
_eext_circle_object_item_color_transit_animator_del(Eext_Circle_Object_Item *item)
{
   if (!item) return;

   if (item->color_transit.animator_cb)
     ecore_animator_del(item->color_transit.animator_cb);

   item->color_transit.animator_cb = NULL;
}

static void
_eext_circle_object_item_angle_transit_animator_del(Eext_Circle_Object_Item *item)
{
   if (!item) return;

   if (item->angle_transit.animator_cb)
     ecore_animator_del(item->angle_transit.animator_cb);

   item->angle_transit.animator_cb = NULL;
}

static Eina_Bool
_eext_circle_object_item_angle_transit_set_internal(Eext_Circle_Object_Item *item, double duration,
                                                    double to_angle, double to_angle_offset,
                                                    Eina_Bool cubic_bezier_enabled,
                                                    double cubic_bezier_p1_x, double cubic_bezier_p1_y,
                                                    double cubic_bezier_p2_x, double cubic_bezier_p2_y)
{
   if (duration == 0.0) return EINA_FALSE;
   if ((item->angle_transit.duration == duration) &&
       (item->angle == to_angle) &&
       (item->angle_offset == to_angle_offset))
     return EINA_FALSE;

   if (item->angle_transit.animator_cb)
     {
        _eext_circle_object_item_angle_transit_animator_del(item);

        // If previous animator is never excuted, run animator callback manually once.
        if (item->angle_transit.current_animator_pos == 0.0)
          {
             double current_time = ecore_time_unix_get();

             if (item->angle_transit.start_time != current_time)
               {
                  double pos = (current_time - item->angle_transit.start_time) /
                     item->angle_transit.duration;

                  _eext_circle_object_item_angle_transit_animator_cb(item, pos);
               }
          }
     }

   _eext_circle_object_item_angle_set_internal(item, to_angle);
   _eext_circle_object_item_angle_offset_set_internal(item, to_angle_offset);

   item->angle_transit.start_time = ecore_time_unix_get();
   item->angle_transit.current_animator_pos = 0.0;
   item->angle_transit.duration = duration;
   item->angle_transit.from_angle = item->draw.angle;
   item->angle_transit.from_angle_offset = item->draw.angle_offset;
   item->angle_transit.cubic_bezier_enabled = cubic_bezier_enabled;

   if (cubic_bezier_enabled)
     {
        item->angle_transit.cubic_bezier_v[0] = cubic_bezier_p1_x;
        item->angle_transit.cubic_bezier_v[1] = cubic_bezier_p1_y;
        item->angle_transit.cubic_bezier_v[2] = cubic_bezier_p2_x;
        item->angle_transit.cubic_bezier_v[3] = cubic_bezier_p2_y;
     }

   item->angle_transit.animator_cb = ecore_animator_timeline_add(duration,
                                                                 _eext_circle_object_item_angle_transit_animator_cb,
                                                                 item);

   return EINA_TRUE;
}

//////////////////////////////
// Internal API //////////////
//////////////////////////////
double
_eext_circle_object_item_value_angle_get(const Eext_Circle_Object_Item *item, double value)
{
   double angle;

   angle = item->min_angle;
   if (item->max_value != item->min_value)
     angle += (value - item->min_value) * (item->max_angle - item->min_angle) / (item->max_value - item->min_value);

   if (angle < item->min_angle) angle = item->min_angle;
   if (angle > item->max_angle) angle = item->max_angle;

   return angle;
}

double
_eext_circle_object_item_angle_value_get(const Eext_Circle_Object_Item *item, double angle)
{
   double value;

   value = item->min_value;
   if (item->max_angle != item->min_angle)
     value += (angle - item->min_angle) * (item->max_value - item->min_value) / (item->max_angle - item->min_angle);

   if (value < item->min_value) value = item->min_value;
   if (value > item->max_value) value = item->max_value;

   return value;
}

const char *
_eext_circle_text_font_get(const char *text, const char *font_family)
{
   FcFontSet *set = NULL;
   FcPattern *p_nm = NULL;
   FcCharSet *c_set = NULL;
   FcValue family;
   FcResult result;
   Eina_Unicode u_str;
   int u_str_index = 0;

   if (!text || !font_family)
     return NULL;

   // Going to use first character.
   u_str = eina_unicode_utf8_get_next(text, &u_str_index);

   // Query font using FontConfig.
   c_set = FcCharSetCreate();
   p_nm = FcPatternBuild (NULL,
                          FC_WEIGHT, FcTypeInteger, FC_WEIGHT_NORMAL,
                          FC_SLANT,  FcTypeInteger, FC_SLANT_ROMAN,
                          FC_WIDTH,  FcTypeInteger, FC_WIDTH_NORMAL,
                          NULL);

   FcCharSetAddChar(c_set, (FcChar32)u_str);
   FcPatternAddCharSet(p_nm, FC_CHARSET, c_set);
   FcPatternAddString(p_nm, FC_FAMILY, (FcChar8 *)font_family);
   FcConfigSubstitute(NULL, p_nm, FcMatchPattern);
   FcDefaultSubstitute(p_nm);
   set = FcFontSort(NULL, p_nm, FcTrue, NULL, &result);

   FcCharSetDestroy(c_set);
   FcPatternDestroy(p_nm);

   result = FcPatternGet(set->fonts[0], FC_FAMILY, 0, &family);
   FcFontSetSortDestroy(set);

   if (result == FcResultNoMatch)
     {
        LOGE("Fail to Load Family Name to draw %s [%X]", text, u_str);
        return NULL;
     }

   return (const char *)family.u.s;
}

Eext_Circle_Object_Item *
_eext_circle_object_item_new(void)
{
   Eext_Circle_Object_Item *item;

   item = (Eext_Circle_Object_Item *)calloc(1, sizeof(Eext_Circle_Object_Item));

   // Initialize...
   item->name = eina_stringshare_add("default");
   item->angle = item->draw.angle = 0.0;
   item->angle_offset = item->draw.angle_offset = 0.0;
   item->min_angle = 0;
   item->max_angle = 360;
   item->min_value = 0;
   item->max_value = 360;
   item->radius = 176;
   item->line_width = 8;
   item->color.r = item->draw.color.r = 102;
   item->color.g = item->draw.color.g = 179;
   item->color.b = item->draw.color.b = 230;
   item->color.a = item->draw.color.a = 255;

   return item;
}

void
_eext_circle_object_item_free(Eext_Circle_Object_Item *item)
{
   if (!item) return;

   eina_stringshare_del(item->name);

   if (item->color_transit.animator_cb)
     _eext_circle_object_item_color_transit_animator_del(item);

   if (item->angle_transit.animator_cb)
     _eext_circle_object_item_angle_transit_animator_del(item);

   if (item->obj)
     _eext_circle_object_item_remove(item);

   free(item);
}

void
_eext_circle_object_item_append(Eext_Circle_Object *obj, Eext_Circle_Object_Item *item)
{
   if (!obj || !item) return;

   if (obj == item->obj)
     {
        LOGE("You are trying to append the item that is already appended");
        return;
     }

   if (item->obj)
     {
        LOGE("You are trying to append the item that is already appended to the other object!");
        return;
     }

   obj->items = eina_list_append(obj->items, item);
   item->obj = obj;

   _eext_circle_object_changed(item->obj);
}

void
_eext_circle_object_item_prepend(Eext_Circle_Object *obj, Eext_Circle_Object_Item *item)
{
   if (!obj || !item) return;

   if (obj == item->obj)
     {
        LOGE("You are trying to append the item that is already appended");
        return;
     }

   if (item->obj)
     {
        LOGE("You are trying to append the item that is already appended to the other object!");
        return;
     }

   obj->items = eina_list_prepend(obj->items, item);
   item->obj = obj;

   _eext_circle_object_changed(item->obj);
}

void
_eext_circle_object_item_insert_before(Eext_Circle_Object *obj, Eext_Circle_Object *before, Eext_Circle_Object_Item *item)
{
   if (!obj || !item) return;

   if (obj == item->obj)
     {
        LOGE("You are trying to append the item that is already appended");
        return;
     }

   if (item->obj)
     {
        LOGE("You are trying to append the item that is already appended to the other object!");
        return;
     }

   if (!before) _eext_circle_object_item_prepend(obj, item);

   obj->items = eina_list_prepend_relative(obj->items, item, before);
   item->obj = obj;

   _eext_circle_object_changed(item->obj);
}

void
_eext_circle_object_item_insert_after(Eext_Circle_Object *obj, Eext_Circle_Object *after, Eext_Circle_Object_Item *item)
{
   if (!obj || !item) return;

   if (obj == item->obj)
     {
        LOGE("You are trying to append the item that is already appended");
        return;
     }

   if (item->obj)
     {
        LOGE("You are trying to append the item that is already appended to the other object!");
        return;
     }

   if (!after) _eext_circle_object_item_append(obj, item);

   obj->items = eina_list_append_relative(obj->items, item, after);
   item->obj = obj;

   _eext_circle_object_changed(item->obj);
}

void
_eext_circle_object_item_remove(Eext_Circle_Object_Item *item)
{
   if (!item || !item->obj) return;

   // You must have to free this item.
   item->obj->items = eina_list_remove(item->obj->items, item);

   _eext_circle_object_changed(item->obj);
   item->obj = NULL;
}

void
_eext_circle_object_item_name_set(Eext_Circle_Object_Item *item, const char *name)
{
   if (!item) return;

   if (name)
     eina_stringshare_replace(&item->name, name);
   else
     eina_stringshare_replace(&item->name, "default");
}

const char *
_eext_circle_object_item_name_get(const Eext_Circle_Object_Item *item)
{
   if (!item) return NULL;

   return item->name;
}

void
_eext_circle_object_item_line_width_set(Eext_Circle_Object_Item *item, int line_width)
{
   if (!item) return;

   if ((line_width <= 0) || (item->line_width == line_width)) return;

   item->line_width = line_width;

   _eext_circle_object_changed(item->obj);
}

int
_eext_circle_object_item_line_width_get(const Eext_Circle_Object_Item *item)
{
   if (!item) return -1;

   return item->line_width;
}

void
_eext_circle_object_item_angle_set(Eext_Circle_Object_Item *item, double angle)
{
   if (!item) return;

   if (item->angle_transit.animator_cb)
     {
        _eext_circle_object_item_angle_transit_animator_del(item);
        item->draw.angle = item->angle;
        _eext_circle_object_changed(item->obj);
     }

   if (_eext_circle_object_item_angle_set_internal(item, angle))
     {
        item->draw.angle = item->angle;
        _eext_circle_object_changed(item->obj);
     }
}

double
_eext_circle_object_item_angle_get(const Eext_Circle_Object_Item *item)
{
   if (!item) return -1.0;

   return item->angle;
}

void
_eext_circle_object_item_angle_offset_set(Eext_Circle_Object_Item *item, double angle_offset)
{
   if (!item) return;

   if (_eext_circle_object_item_angle_offset_set_internal(item, angle_offset))
     {
        item->draw.angle_offset = item->angle_offset;
        _eext_circle_object_changed(item->obj);
     }
}

double
_eext_circle_object_item_angle_offset_get(const Eext_Circle_Object_Item *item)
{
   if (!item) return -1.0;

   return item->angle_offset;
}

void
_eext_circle_object_item_angle_min_max_set(Eext_Circle_Object_Item *item, double min_angle, double max_angle)
{
   double angle = 0.0;

   if (!item) return;

   if (item->angle_transit.animator_cb)
     {
        _eext_circle_object_item_angle_transit_animator_del(item);
        item->draw.angle = item->angle;
        _eext_circle_object_changed(item->obj);
     }

   if ((item->min_angle == min_angle) && (item->max_angle == max_angle)) return;

   item->min_angle = min_angle;
   item->max_angle = max_angle;

   if (item->angle < item->min_angle) angle = item->min_angle;
   if (item->angle > item->max_angle) angle = item->max_angle;

   if (item->angle == angle) return;

   item->angle = angle;
   item->value = _eext_circle_object_item_angle_value_get(item, item->angle);
   item->draw.angle = item->angle;

   _eext_circle_object_changed(item->obj);
}

void
_eext_circle_object_item_angle_min_max_get(const Eext_Circle_Object_Item *item, double *min_angle, double *max_angle)
{
   if (!item)
     {
        if (min_angle) *min_angle = 0.0;
        if (max_angle) *max_angle = 0.0;

        return;
     }

   if (min_angle) *min_angle = item->min_angle;
   if (max_angle) *max_angle = item->max_angle;
}

void
_eext_circle_object_item_value_min_max_set(Eext_Circle_Object_Item *item, double min_value, double max_value)
{
   double angle;

   if (!item) return;

   if (item->angle_transit.animator_cb)
     {
        _eext_circle_object_item_angle_transit_animator_del(item);
        item->draw.angle = item->angle;
        _eext_circle_object_changed(item->obj);
     }

   if ((item->min_value == min_value) && (item->max_value == max_value)) return;

   item->min_value = min_value;
   item->max_value = max_value;

   if (item->value < item->min_value) item->value = item->min_value;
   if (item->value > item->max_value) item->value = item->max_value;

   angle = _eext_circle_object_item_value_angle_get(item, item->value);

   if (item->angle == angle) return;

   item->angle = angle;
   item->draw.angle = item->angle;

   _eext_circle_object_changed(item->obj);
}

void
_eext_circle_object_item_value_min_max_get(const Eext_Circle_Object_Item *item, double *min_value, double *max_value)
{
   if (!item)
     {
        if (min_value) *min_value = 0.0;
        if (max_value) *max_value = 0.0;

        return;
     }

   if (min_value) *min_value = item->min_value;
   if (max_value) *max_value = item->max_value;
}

void
_eext_circle_object_item_value_set(Eext_Circle_Object_Item *item, double value)
{
   if (!item) return;

   if (item->angle_transit.animator_cb)
     {
        _eext_circle_object_item_angle_transit_animator_del(item);
        item->draw.angle = item->angle;
        _eext_circle_object_changed(item->obj);
     }

   if (_eext_circle_object_item_value_set_internal(item, value))
     {
        item->draw.angle = item->angle;
        _eext_circle_object_changed(item->obj);
     }
}

double
_eext_circle_object_item_value_get(const Eext_Circle_Object_Item *item)
{
   if (!item) return -1.0;

   return item->value;
}

void
_eext_circle_object_item_color_set(Eext_Circle_Object_Item *item,
                                   int r, int g, int b, int a)
{
   if (!item) return;

   if (item->color_transit.animator_cb)
     {
        _eext_circle_object_item_color_transit_animator_del(item);
        item->draw.color.r = item->color.r;
        item->draw.color.g = item->color.g;
        item->draw.color.b = item->color.b;
        item->draw.color.a = item->color.a;

        _eext_circle_object_changed(item->obj);
     }

   if (_eext_circle_object_item_color_set_internal(item, r, g, b, a))
     {
        item->draw.color.r = item->color.r;
        item->draw.color.g = item->color.g;
        item->draw.color.b = item->color.b;
        item->draw.color.a = item->color.a;

        _eext_circle_object_changed(item->obj);
     }
}

void
_eext_circle_object_item_color_get(const Eext_Circle_Object_Item *item,
                                   int *r, int *g, int *b, int *a)
{
   if (!item)
     {
        if (r) *r = 0;
        if (g) *g = 0;
        if (b) *b = 0;
        if (a) *a = 0;

        return;
     }

   if (r) *r = item->color.r;
   if (g) *g = item->color.g;
   if (b) *b = item->color.b;
   if (a) *a = item->color.a;
}

void
_eext_circle_object_item_radius_set(Eext_Circle_Object_Item *item, double radius)
{
   if (!item) return;

   if (radius < 0.0) radius = 0.0; if (radius > 180.0) radius = 180.0;

   if (item->radius == radius) return;

   item->radius = radius;

   _eext_circle_object_changed(item->obj);
}

double
_eext_circle_object_item_radius_get(const Eext_Circle_Object_Item *item)
{
   if (!item) return -1.0;

   return item->radius;
}

void
_eext_circle_object_item_text_set(Eext_Circle_Object_Item *item,
                                  const char *text,
                                  const char *font)
{
   const char *selected_font;

   if (!item) return;

   if (font)
     eina_stringshare_replace(&item->font, font);
   else
     eina_stringshare_replace(&item->font, "Tizen");

   eina_stringshare_replace(&item->text, text);

   selected_font = _eext_circle_text_font_get(item->text, item->font);

   if (!selected_font)
     {
        LOGE("There is no font for the text: %s", text);
     }

   eina_stringshare_replace(&item->selected_font, selected_font);

   _eext_circle_object_changed(item->obj);
}

void
_eext_circle_object_item_text_get(const Eext_Circle_Object_Item *item,
                                  const char **text,
                                  const char **font)
{
   if (!item)
     {
        if (text) *text = NULL;
        if (font) *font = NULL;

        return;
     }

   if (text) *text = item->text;
   if (font) *font = item->font;
}

void
_eext_circle_object_item_font_size_set(Eext_Circle_Object_Item *item,
                                       int font_size)
{
   if (!item) return;

   if (item->font_size == font_size) return;

   item->font_size = font_size;

   _eext_circle_object_changed(item->obj);
}

int
_eext_circle_object_item_font_size_get(const Eext_Circle_Object_Item *item)
{
   if (!item) return -1;

   return item->font_size;
}

void
_eext_circle_object_item_color_transit_set(Eext_Circle_Object_Item *item, double duration,
                                           int to_r, int to_g, int to_b, int to_a,
                                           Eina_Bool cubic_bezier_enabled,
                                           double cubic_bezier_p1_x, double cubic_bezier_p1_y,
                                           double cubic_bezier_p2_x, double cubic_bezier_p2_y)
{
   if (!item) return;

   if (duration == 0.0) return;
   if ((item->color_transit.duration == duration) &&
       (item->color.r == to_r) && (item->color.g == to_g) && (item->color.b == to_b) && (item->color.a == to_a))
     return;

   if (item->color_transit.animator_cb)
     {
        _eext_circle_object_item_color_transit_animator_del(item);

        if (item->color_transit.current_animator_pos == 0.0)
          {
             double current_time = ecore_time_unix_get();

             if (item->color_transit.start_time != current_time)
               {
                  double pos = (current_time - item->color_transit.start_time) /
                     item->color_transit.duration;

                  _eext_circle_object_item_color_transit_animator_cb(item, pos);
               }
          }
     }

   _eext_circle_object_item_color_set_internal(item, to_r, to_g, to_b, to_a);

   item->color_transit.start_time = ecore_time_unix_get();
   item->color_transit.current_animator_pos = 0.0;
   item->color_transit.duration = duration;
   item->color_transit.from_color.r = item->draw.color.r;
   item->color_transit.from_color.g = item->draw.color.g;
   item->color_transit.from_color.b = item->draw.color.b;
   item->color_transit.from_color.a = item->draw.color.a;
   item->color_transit.cubic_bezier_enabled = cubic_bezier_enabled;

   if (cubic_bezier_enabled)
     {
        item->color_transit.cubic_bezier_v[0] = cubic_bezier_p1_x;
        item->color_transit.cubic_bezier_v[1] = cubic_bezier_p1_y;
        item->color_transit.cubic_bezier_v[2] = cubic_bezier_p2_x;
        item->color_transit.cubic_bezier_v[3] = cubic_bezier_p2_y;
     }

   item->color_transit.animator_cb = ecore_animator_timeline_add(duration,
                                                                 _eext_circle_object_item_color_transit_animator_cb,
                                                                 item);
}


void
_eext_circle_object_item_angle_transit_set(Eext_Circle_Object_Item *item, double duration,
                                           double to_angle, double to_angle_offset,
                                           Eina_Bool cubic_bezier_enabled,
                                           double cubic_bezier_p1_x, double cubic_bezier_p1_y,
                                           double cubic_bezier_p2_x, double cubic_bezier_p2_y)
{
   Eina_Bool ret;

   if (!item) return;

   ret = _eext_circle_object_item_angle_transit_set_internal(item, duration,
                                                             to_angle, to_angle_offset,
                                                             cubic_bezier_enabled,
                                                             cubic_bezier_p1_x, cubic_bezier_p1_y,
                                                             cubic_bezier_p2_x, cubic_bezier_p2_y);

   if (ret)
     item->value = _eext_circle_object_item_angle_value_get(item, item->angle);
}

void
_eext_circle_object_item_value_transit_set(Eext_Circle_Object_Item *item, double duration,
                                           double to_value,
                                           Eina_Bool cubic_bezier_enabled,
                                           double cubic_bezier_p1_x, double cubic_bezier_p1_y,
                                           double cubic_bezier_p2_x, double cubic_bezier_p2_y)
{
   Eina_Bool ret;
   double to_angle, to_angle_offset;

   if (!item) return;

   to_angle = _eext_circle_object_item_value_angle_get(item, to_value);
   to_angle_offset = item->angle_offset;

   ret = _eext_circle_object_item_angle_transit_set_internal(item, duration,
                                                             to_angle, to_angle_offset,
                                                             cubic_bezier_enabled,
                                                             cubic_bezier_p1_x, cubic_bezier_p1_y,
                                                             cubic_bezier_p2_x, cubic_bezier_p2_y);

   if (ret)
     item->value = to_value;
}
