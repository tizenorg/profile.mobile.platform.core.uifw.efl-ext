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

static void _eext_circle_surface_del_internal(Eext_Circle_Surface *surface);

static void
_eext_circle_surface_cairo_draw_arc(cairo_t *cairo,
                                    int line_width,
                                    double center_x, double center_y, double radius,
                                    double r, double g, double b, double a,
                                    double angle1, double angle2)
{
   cairo_set_line_width(cairo, line_width);
   cairo_set_source_rgba(cairo, r, g, b, a);

   cairo_arc(cairo, center_x, center_y, radius,
             angle1 * M_PI / 180, angle2 * M_PI / 180);
   cairo_stroke(cairo);
}

static void
_eext_circle_surface_cairo_draw_text(cairo_t *cairo,
                                     const char *text, const char *font, int font_size,
                                     cairo_font_slant_t slant, cairo_font_weight_t weight,
                                     double center_x, double center_y, double radius,
                                     double r, double g, double b, double a,
                                     double angle, double rotate_angle)
{
   int x, y;

   x = radius * cos(angle * M_PI / 180);
   y = radius * sin(angle * M_PI / 180);
   x += center_x;
   y += center_y;

   cairo_set_source_rgba(cairo, r, g, b, a);
   cairo_set_font_size(cairo, font_size);
   cairo_select_font_face(cairo, font, slant, weight);
   cairo_move_to(cairo, x, y);
   cairo_rotate(cairo, rotate_angle * M_PI / 180);
   cairo_show_text(cairo, text);
   cairo_stroke(cairo);
}

static void
_eext_circle_surface_clear(Eext_Circle_Surface *surface)
{
   Evas_Object *image_object;
   unsigned char *buffer;

   if (!surface || !surface->image_widget) return;

   image_object = elm_image_object_get(surface->image_widget);

   buffer = (unsigned char *)evas_object_image_data_get(image_object, 1);
   memset(buffer, 0, surface->w * surface->h * 4);
   cairo_set_source_rgba(surface->cairo, 0, 0, 0, 0);
   cairo_rectangle(surface->cairo, 0.0, 0.0, surface->w, surface->h);
   cairo_fill(surface->cairo);
}

static void
_eext_circle_bg_surface_memcpy(Eext_Circle_Surface *surface, Eext_Circle_Object *obj)
{
   Evas_Object *image_object, *bg_image;
   unsigned char *buffer, *bg_buffer;

   if (!surface || !surface->image_widget) return;

   image_object = elm_image_object_get(surface->image_widget);
   buffer = (unsigned char *)evas_object_image_data_get(image_object, 1);

   bg_image = eina_list_nth(obj->bg_image_objs, obj->bg_image_index);
   evas_object_image_load_size_set(bg_image, surface->w, surface->h);
   evas_object_image_fill_set(bg_image, 0, 0, surface->w, surface->h);
   bg_buffer = (unsigned char *)evas_object_image_data_get(bg_image, 1);

   if (bg_buffer)
     {
        memcpy(buffer, bg_buffer, surface->w * surface->h * 4);
     }
   else
     {
        LOGE("Background image buffer is NULL!");
        memset(buffer, 0, surface->w * surface->h * 4);
     }
}

static Eina_Bool
_eext_circle_surface_render(void *data)
{
   Eext_Circle_Surface *surface = (Eext_Circle_Surface *)data;
   Eext_Circle_Object *render_obj;
   Eina_List *l;
   Eina_List *ll;
   Eina_Bool skip_surface_clear = EINA_FALSE;

   if (!surface)
     {
        LOGE("Surface is NULL... Going to remove surface render function.");
        return ECORE_CALLBACK_CANCEL;
     }

   if (!surface->changed)
     return ECORE_CALLBACK_RENEW;

   surface->changed = EINA_FALSE;

   EINA_LIST_FOREACH(surface->render_objs, l, render_obj)
     {
        if (!render_obj->visible)
          continue;

        if (render_obj->bg_image_objs)
          {
             _eext_circle_bg_surface_memcpy(surface, render_obj);
             skip_surface_clear = EINA_TRUE;
             break;
          }
     }

   if (!skip_surface_clear)
     _eext_circle_surface_clear(surface);

   EINA_LIST_FOREACH(surface->render_objs, l, render_obj)
     {
        Eext_Circle_Object_Item *item;

        if (!render_obj->visible)
          continue;

        if (render_obj->render_pre_cb)
          render_obj->render_pre_cb(render_obj);

        EINA_LIST_FOREACH(render_obj->items, ll, item)
          {
             // Skip invisible item.
             if (item->draw.color.a == 0) continue;

             // If there is no text, the item is arc item.
             if (!item->text)
               {
                  // Draw Arc
                  if (item->line_width)
                    {
                       cairo_save(surface->cairo);
                       _eext_circle_surface_cairo_draw_arc(surface->cairo, item->line_width,
                                                           surface->center_x, surface->center_y, item->radius,
                                                           (double)item->draw.color.r / 255,
                                                           (double)item->draw.color.g / 255,
                                                           (double)item->draw.color.b / 255,
                                                           (double)item->draw.color.a / 255,
                                                           item->draw.angle_offset + item->min_angle - 90.0,
                                                           item->draw.angle_offset + item->draw.angle - 90.0);
                       cairo_restore(surface->cairo);
                    }
               }
             else if (item->font_size)
               {
                  // Draw Text
                  cairo_save(surface->cairo);
                  _eext_circle_surface_cairo_draw_text(surface->cairo,
                                                       item->text, item->selected_font, item->font_size,
                                                       CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL,
                                                       surface->center_x, surface->center_y, item->radius,
                                                       (double)item->draw.color.r / 255,
                                                       (double)item->draw.color.g / 255,
                                                       (double)item->draw.color.b / 255,
                                                       (double)item->draw.color.a / 255,
                                                       item->draw.angle_offset + item->draw.angle - 90.0,
                                                       item->draw.angle_offset + item->draw.angle);
                  cairo_restore(surface->cairo);
               }
          }
     }

   evas_object_image_data_update_add(elm_image_object_get(surface->image_widget),
                                     0, 0, surface->w, surface->h);

   return ECORE_CALLBACK_RENEW;
}

static void
_eext_circle_surface_render_finish(Eext_Circle_Surface *surface)
{
   if (!surface) return;

   if (surface->render_animator) ecore_animator_del(surface->render_animator);
   surface->render_animator = NULL;
}

static void
_eext_circle_surface_render_init(Eext_Circle_Surface *surface)
{
   if (!surface) return;

   if (surface->render_animator) _eext_circle_surface_render_finish(surface);
   surface->render_animator = ecore_animator_add(_eext_circle_surface_render, surface);
   surface->changed = EINA_TRUE;
}

static void
_eext_circle_surface_cairo_finish(Eext_Circle_Surface *surface)
{
   if (surface->cairo) cairo_destroy(surface->cairo);
   if (surface->cairo_surface) cairo_surface_destroy(surface->cairo_surface);

   surface->cairo = NULL;
   surface->cairo_surface = NULL;
}

static void
_eext_circle_surface_cairo_init(Eext_Circle_Surface *surface)
{
   Evas_Object *image_object;
   cairo_surface_t *cairo_surface;
   cairo_t *cairo;
   unsigned char *buffer;
   int stride;

   if (surface->cairo_surface || surface->cairo)
     _eext_circle_surface_cairo_finish(surface);

   if (surface->image_widget == NULL)
     {
        LOGE("Fail to get buffer from image object!");
        return;
     }

   image_object = elm_image_object_get(surface->image_widget);

   buffer = (unsigned char *)evas_object_image_data_get(image_object, 1);
   stride = evas_object_image_stride_get(image_object);
   cairo_surface = cairo_image_surface_create_for_data(buffer, CAIRO_FORMAT_ARGB32,
                                                       surface->w, surface->h, stride);
   cairo = cairo_create(cairo_surface);
   cairo_set_antialias(cairo, CAIRO_ANTIALIAS_BEST);

   surface->cairo_surface = cairo_surface;
   surface->cairo = cairo;
}

static void
_eext_circle_surface_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eext_Circle_Surface *surface = (Eext_Circle_Surface *)data;
   Evas_Object *image_object = elm_image_object_get(obj);

   evas_object_geometry_get(obj, NULL, NULL, &surface->w, &surface->h);

   surface->center_x = surface->w / 2;
   surface->center_y = surface->h / 2;

   evas_object_image_size_set(image_object, surface->w, surface->h);
   evas_object_image_fill_set(image_object, 0, 0, surface->w, surface->h);

   if ((surface->w != 0) && (surface->h != 0))
     {
        LOGI("Surface will be initialized!");
        _eext_circle_surface_cairo_init(surface);
        _eext_circle_surface_render_init(surface);
     }
}

static void
_eext_circle_surface_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eext_Circle_Surface *surface = (Eext_Circle_Surface *)data;

   LOGI("Surface is going to free in delete callback for image widget.");
   _eext_circle_surface_del_internal(surface);
}

static void
_eext_circle_surface_object_rotary_activated_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;

   evas_object_show(circle_obj->image_widget);

   // Previous view's circle object should not be disappeared while naviframe effect.
   // At here, show the latest deactivated object if the conditions are satisfied.
   if (circle_obj->surface)
     {
        if (circle_obj->surface->parent_surface &&
            (circle_obj->surface->parent_surface->type == EEXT_CIRCLE_SURFACE_TYPE_NAVIFRAME))
          {
             Eext_Circle_Object *deact_obj = circle_obj->surface->parent_surface->deactivated_obj;

             if (deact_obj && (deact_obj->surface->parent != circle_obj->surface->parent))
               {
                  evas_object_show(deact_obj->image_widget);
               }
          }
        else if (circle_obj->surface->type == EEXT_CIRCLE_SURFACE_TYPE_NAVIFRAME)
          {
             if (circle_obj->surface->deactivated_obj)
               evas_object_show(circle_obj->surface->deactivated_obj->image_widget);
          }
     }
}

static void
_deactivated_obj_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;

   if (circle_obj && circle_obj->surface && circle_obj->surface->parent_surface &&
       (circle_obj->surface->parent_surface->type == EEXT_CIRCLE_SURFACE_TYPE_NAVIFRAME))
     {
        if (circle_obj->surface->parent_surface->deactivated_obj == circle_obj)
          circle_obj->surface->parent_surface->deactivated_obj = NULL;
     }
}

static void
_eext_circle_surface_object_rotary_deactivated_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;

   evas_object_hide(circle_obj->image_widget);

   // Previous view's circle object should not be disappeared while naviframe effect.
   // At here, just keep the current deactivated object.
   if (circle_obj->surface && circle_obj->surface->parent_surface &&
       (circle_obj->surface->parent_surface->type == EEXT_CIRCLE_SURFACE_TYPE_NAVIFRAME))
     {
        Eext_Circle_Object *deactivated_obj = circle_obj->surface->parent_surface->deactivated_obj;

        if (deactivated_obj == circle_obj)
          return;

        if (deactivated_obj)
          evas_object_event_callback_del_full(deactivated_obj->image_widget, EVAS_CALLBACK_DEL, _deactivated_obj_del_cb, deactivated_obj);
        circle_obj->surface->parent_surface->deactivated_obj = circle_obj;
        evas_object_event_callback_add(circle_obj->image_widget, EVAS_CALLBACK_DEL, _deactivated_obj_del_cb, circle_obj);
     }
}

static Eext_Circle_Surface *
_eext_circle_surface_naviframe_item_view_add(Evas_Object *layout)
{
   Eext_Circle_Surface *surface;

   if (!layout)
     {
        LOGE("The given Evas_Object is NULL! It returns NULL.");
        return NULL;
     }

   if (edje_object_part_exists(elm_layout_edje_get(layout), "elm.swallow.circle"))
     {
        surface = _eext_circle_surface_add(layout, EEXT_CIRCLE_SURFACE_TYPE_DEFAULT);

        if (!surface->image_widget)
          {
             LOGE("Surface doesn't have image widget for drawing. It returns NULL.");
             return NULL;
          }

        LOGI("Put the surface[%p]'s widget[%p] to layout widget[%p]", surface, surface->image_widget, layout);
        elm_object_part_content_set(layout, "elm.swallow.circle", surface->image_widget);

        return surface;
     }
   else
     {
        LOGE("There is no \"elm.swallow.circle\" part in elm_layout widget! It returns NULL.");
        return NULL;
     }
}

static void _naviframe_item_push_cb(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *view = (Evas_Object *)event_info;
   Eext_Circle_Surface *surface = (Eext_Circle_Surface *)data;
   Eext_Circle_Surface *view_surface;
   Eext_Circle_Object *circle_obj;
   Eina_List *l, *l_next;

   if (!surface)
     {
        LOGE("No matched surface exists for the naviframe:%p", obj);
        return;
     }

   view_surface = _eext_circle_surface_naviframe_item_view_add(view);
   view_surface->parent_surface = surface;
   surface->sub_surfaces = eina_list_append(surface->sub_surfaces, view_surface);

   EINA_LIST_FOREACH_SAFE(surface->render_objs, l, l_next, circle_obj)
     {
        Evas_Object *parent;
        parent = elm_object_parent_widget_get(circle_obj->image_widget);

        while (parent)
          {
             if (parent == view)
               {
                  _eext_circle_surface_object_remove(surface, circle_obj);
                  _eext_circle_surface_object_append(view_surface, circle_obj);
                  circle_obj->surface = view_surface;
                  break;
               }

             parent = elm_object_parent_widget_get(parent);
             if (parent == obj) break;
          }
     }
}

static void
_eext_circle_surface_del_internal(Eext_Circle_Surface *surface)
{
   Eext_Circle_Object *obj;
   Eina_List *l, *l_next;

   if (surface->type == EEXT_CIRCLE_SURFACE_TYPE_NAVIFRAME)
     {
        Eext_Circle_Surface *cur_surface;

        evas_object_smart_callback_del_full(surface->parent, "item,pushed,internal", _naviframe_item_push_cb, surface);
        evas_object_event_callback_del_full(surface->parent, EVAS_CALLBACK_DEL, _eext_circle_surface_del_cb, surface);

        EINA_LIST_FOREACH_SAFE(surface->sub_surfaces, l, l_next, cur_surface)
          {
             _eext_circle_surface_del_internal(cur_surface);
          }

     }
   else
     {
        _eext_circle_surface_cairo_finish(surface);
        _eext_circle_surface_render_finish(surface);
        evas_object_event_callback_del(surface->image_widget, EVAS_CALLBACK_RESIZE, _eext_circle_surface_resize_cb);
        evas_object_event_callback_del(surface->image_widget, EVAS_CALLBACK_DEL, _eext_circle_surface_del_cb);

        if (surface->parent_surface)
          surface->parent_surface->sub_surfaces = eina_list_remove(surface->parent_surface->sub_surfaces, surface);
     }

   // If public surface is deleted, prepare a private surface.
   if (surface->type != EEXT_CIRCLE_SURFACE_TYPE_PRIVATE)
     {
        EINA_LIST_FOREACH_SAFE(surface->render_objs, l, l_next, obj)
          {
             Eext_Circle_Surface *private_surface;
             LOGI("Private Surface will be initialized for widget[%p], circle_obj[%p]!",
                  obj->image_widget, obj);

             surface->render_objs = eina_list_remove_list(surface->render_objs, l);

             private_surface =
                _eext_circle_surface_init(obj->image_widget, NULL, EEXT_CIRCLE_SURFACE_TYPE_PRIVATE);
             _eext_circle_surface_object_append(private_surface, obj);

             evas_object_smart_callback_del(obj->image_widget, "rotary,activated",
                                            _eext_circle_surface_object_rotary_activated_cb);
             if (obj->widget_object)
               {
                  evas_object_smart_callback_del(obj->widget_object, "rotary,activated",
                                                 _eext_circle_surface_object_rotary_activated_cb);
               }
          }
     }

   free(surface);
}

//////////////////////////
// Internal API //////////
//////////////////////////
Eext_Circle_Surface *
_eext_circle_surface_init(Evas_Object *image_widget, Evas_Object *parent, Eext_Circle_Surface_Type type)
{
   Eext_Circle_Surface *surface;

   surface = (Eext_Circle_Surface *)calloc(1, sizeof(Eext_Circle_Surface));
   if (!surface)
     {
        LOGE("Fail to calloc for (Eext_Circle_Surface)");
        return NULL;
     }

   surface->type = type;
   surface->parent = parent;

   if (type == EEXT_CIRCLE_SURFACE_TYPE_NAVIFRAME)
     {
        // If the surface is created with elm_naviframe.
        evas_object_smart_callback_add(surface->parent, "item,pushed,internal", _naviframe_item_push_cb, surface);
        evas_object_event_callback_add(surface->parent, EVAS_CALLBACK_DEL, _eext_circle_surface_del_cb, surface);
     }
   else
     {
        surface->image_widget = image_widget;
        evas_object_event_callback_add(surface->image_widget, EVAS_CALLBACK_RESIZE, _eext_circle_surface_resize_cb, surface);
        evas_object_event_callback_add(surface->image_widget, EVAS_CALLBACK_DEL, _eext_circle_surface_del_cb, surface);
        evas_object_image_alpha_set(elm_image_object_get(surface->image_widget), EINA_TRUE);
     }

   return surface;
}

void
_eext_circle_surface_finish(Eext_Circle_Surface *surface)
{
   _eext_circle_surface_del_internal(surface);
}

void
_eext_circle_surface_object_remove(Eext_Circle_Surface *surface, Eext_Circle_Object *obj)
{
   if (!surface) return;
   if (!obj) return;

   surface->render_objs = eina_list_remove(surface->render_objs, obj);
   surface->changed = EINA_TRUE;

   if (surface->type != EEXT_CIRCLE_SURFACE_TYPE_PRIVATE)
     {
        evas_object_smart_callback_del(obj->image_widget, "rotary,activated",
                                       _eext_circle_surface_object_rotary_activated_cb);
        evas_object_smart_callback_del(obj->image_widget, "rotary,deactivated",
                                       _eext_circle_surface_object_rotary_deactivated_cb);

        if (obj->widget_object)
          {
             evas_object_smart_callback_del(obj->widget_object, "rotary,activated",
                                            _eext_circle_surface_object_rotary_activated_cb);
             evas_object_smart_callback_del(obj->widget_object, "rotary,deactivated",
                                            _eext_circle_surface_object_rotary_deactivated_cb);
          }
     }
}

void
_eext_circle_surface_object_append(Eext_Circle_Surface *surface, Eext_Circle_Object *obj)
{
   if (!surface) return;
   if (!obj) return;

   if (obj->surface)
     _eext_circle_surface_object_remove(obj->surface, obj);

   // Search whether proper sub-surface exists or not for the circle object.
   if ((surface->type == EEXT_CIRCLE_SURFACE_TYPE_NAVIFRAME) && surface->sub_surfaces)
     {
        Eina_List *l, *l_next;
        Eext_Circle_Surface *cur_surface;

        EINA_LIST_FOREACH_SAFE(surface->sub_surfaces, l, l_next, cur_surface)
          {
             Evas_Object *parent;
             parent = elm_object_parent_widget_get(obj->image_widget);

             while (parent)
               {
                  if (parent == cur_surface->parent)
                    {
                       _eext_circle_surface_object_append(cur_surface, obj);
                       obj->surface = cur_surface;
                       return;
                    }

                  parent = elm_object_parent_widget_get(parent);
                  if (parent == surface->parent) break;
               }

          }
     }

   surface->render_objs = eina_list_append(surface->render_objs, obj);
   surface->changed = EINA_TRUE;
   obj->surface = surface;

   if (obj->surface->type != EEXT_CIRCLE_SURFACE_TYPE_PRIVATE)
     {
        evas_object_smart_callback_add(obj->image_widget, "rotary,activated",
                                       _eext_circle_surface_object_rotary_activated_cb, obj);
        evas_object_smart_callback_add(obj->image_widget, "rotary,deactivated",
                                       _eext_circle_surface_object_rotary_deactivated_cb, obj);

        if (obj->widget_object)
          {
             evas_object_smart_callback_add(obj->widget_object, "rotary,activated",
                                            _eext_circle_surface_object_rotary_activated_cb, obj);
             evas_object_smart_callback_add(obj->widget_object, "rotary,deactivated",
                                            _eext_circle_surface_object_rotary_deactivated_cb, obj);
          }
     }
}

void
_eext_circle_surface_object_exclusive_show(Eext_Circle_Surface *surface, Eext_Circle_Object *obj)
{
   Eext_Circle_Object *render_obj;
   Eina_List *l;

   if (!surface) return;
   if (!obj) return;

   if (!evas_object_visible_get(obj->image_widget) &&
       (obj->widget_object && evas_object_visible_get(obj->widget_object)))
     evas_object_show(obj->image_widget);

   EINA_LIST_FOREACH(surface->render_objs, l, render_obj)
     {
        if (render_obj != obj)
          evas_object_hide(render_obj->image_widget);
     }
}

void
_eext_circle_surface_changed(Eext_Circle_Surface *surface)
{
   if (!surface) return;

   surface->changed = EINA_TRUE;
}

Eext_Circle_Surface *
_eext_circle_surface_add(Evas_Object *parent, Eext_Circle_Surface_Type type)
{
   Eext_Circle_Surface *surface;
   Evas_Object *image_widget = NULL;

   if (!parent) return NULL;

   if (type != EEXT_CIRCLE_SURFACE_TYPE_NAVIFRAME)
     image_widget = elm_image_add(parent);

   surface = _eext_circle_surface_init(image_widget, parent, type);

   return surface;
}

////////////////////////
// EAPI ////////////////
////////////////////////
EAPI Eext_Circle_Surface *
eext_circle_surface_conformant_add(Evas_Object *conform)
{
   Eext_Circle_Surface *surface;
   const char *type = NULL;

   if (!conform)
     {
        LOGE("The given Evas_Object is NULL! It returns NULL.");
        return NULL;
     }

   type = evas_object_type_get(conform);

   if (type && (!strcmp(type, "Elm_Conformant") || !strcmp(type, "elm_conformant")))
     {
        if (elm_object_style_set(conform, "circle"))
          {
             if (!edje_object_part_exists(elm_layout_edje_get(conform), "elm.swallow.circle"))
               {
                  LOGE("There is no \"elm.swallow.circle\" part in elm_conformant widget! It returns NULL.");
                  return NULL;
               }

             surface = _eext_circle_surface_add(conform, EEXT_CIRCLE_SURFACE_TYPE_DEFAULT);

             if (!surface->image_widget)
               {
                  LOGE("Surface doesn't have image widget for drawing. It returns NULL.");
                  return NULL;
               }

             LOGI("Put the surface[%p]'s widget[%p] to elm_conformant widget[%p]", surface, surface->image_widget, conform);
             elm_object_part_content_set(conform, "elm.swallow.circle", surface->image_widget);

             return surface;
          }
        else
          {
             LOGE("elm_object_style_set is failed! The theme doesn't have \"elm/conformant/base/circle\" group. It returns NULL.");
             return NULL;
          }
     }

   LOGE("The given Evas_Object[%p] is not elm_conformant widget! It returns NULL.", conform);
   return NULL;
}

EAPI Eext_Circle_Surface *
eext_circle_surface_layout_add(Evas_Object *layout)
{
   Eext_Circle_Surface *surface;
   const char *type = NULL;

   if (!layout)
     {
        LOGE("The given Evas_Object is NULL! It returns NULL.");
        return NULL;
     }

   type = evas_object_type_get(layout);

   if (type && (!strcmp(type, "Elm_Layout") || !strcmp(type, "elm_layout")))
     {
        if (elm_layout_theme_set(layout, "layout", "circle", "surface"))
          {
             if (!edje_object_part_exists(elm_layout_edje_get(layout), "elm.swallow.circle"))
               {
                  LOGE("There is no \"elm.swallow.circle\" part in elm_layout widget! It returns NULL.");
                  return NULL;
               }

             surface = _eext_circle_surface_add(layout, EEXT_CIRCLE_SURFACE_TYPE_DEFAULT);

             if (!surface->image_widget)
               {
                  LOGE("Surface doesn't have image widget for drawing. It returns NULL.");
                  return NULL;
               }

             LOGI("Put the surface[%p]'s widget[%p] to layout widget[%p]", surface, surface->image_widget, layout);
             elm_object_part_content_set(layout, "elm.swallow.circle", surface->image_widget);

             return surface;
          }
        else
          {
             LOGE("elm_layout_theme_set is failed! The theme doesn't have \"elm/layout/circle/surface\" group. It returns NULL.");
             return NULL;
          }
     }

   LOGE("The given Evas_Object[%p] is not elm_layout widget! It returns NULL.", layout);
   return NULL;
}

EAPI Eext_Circle_Surface *
eext_circle_surface_naviframe_add(Evas_Object *naviframe)
{
   Eext_Circle_Surface *surface;
   const char *type = NULL;

   if (!naviframe)
     {
        LOGE("The given Evas_Object is NULL! It returns NULL.");
        return NULL;
     }

   type = evas_object_type_get(naviframe);

   if (type && (!strcmp(type, "Elm_Naviframe") || !strcmp(type, "elm_naviframe")))
     {
        surface = _eext_circle_surface_add(naviframe, EEXT_CIRCLE_SURFACE_TYPE_NAVIFRAME);
        return surface;
     }

   LOGE("The given Evas_Object[%p] is not elm_naviframe widget! It returns NULL.", naviframe);
   return NULL;
}

EAPI void
eext_circle_surface_del(Eext_Circle_Surface *surface)
{
   if (!surface)
     {
        LOGE("Surface is already NULL");
        return;
     }

   if (surface->parent)
     {
        Evas_Object *image_widget;

        image_widget = elm_object_part_content_unset(surface->parent, "elm.swallow.circle");

        if (image_widget && (surface->image_widget != image_widget))
          {
             LOGE("There are different image widgets in parent[%p][%s] and surface[%p]! But, going to call evas_object_del to the image widgets!");
             evas_object_del(image_widget);
          }

        evas_object_del(surface->image_widget);
     }
   else
     {
        LOGI("Surface[%p] is going to free in eext_circle_surface_del API.", surface);
        _eext_circle_surface_del_internal(surface);
     }
}
