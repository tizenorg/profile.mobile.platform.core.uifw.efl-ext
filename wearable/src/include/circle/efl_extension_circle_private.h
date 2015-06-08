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

#ifndef __EFL_EXTENSION_CIRCLE_PRIVATE_H__
#define __EFL_EXTENSION_CIRCLE_PRIVATE_H__

#include <cairo.h>

#ifdef __cplusplus
extern "C" {
#endif


#define EEXT_CIRCLE_OBJECT_TYPE "Eext_Circle_Object"

#define EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) \
     Eext_Circle_Object *circle_obj = NULL; \
     circle_obj = evas_object_data_get(obj, EEXT_CIRCLE_OBJECT_TYPE); \
     if (!circle_obj)
/**
 * Put Document
 */
typedef struct _Eext_Circle_Object Eext_Circle_Object;

typedef enum
{
   EEXT_CIRCLE_SURFACE_TYPE_DEFAULT,
   EEXT_CIRCLE_SURFACE_TYPE_PRIVATE,
   EEXT_CIRCLE_SURFACE_TYPE_NAVIFRAME,
   EEXT_CIRCLE_SURFACE_TYPE_LAST
} Eext_Circle_Surface_Type;

struct _Eext_Circle_Surface {
   Evas_Object         *parent;
   Evas_Object         *image_widget;
   cairo_surface_t     *cairo_surface;
   cairo_t             *cairo;
   unsigned char       *buffer;
   Evas_Coord           w, h;
   Evas_Coord           center_x, center_y;
   Eext_Circle_Surface_Type type;

   Eina_List           *render_objs;

   Ecore_Animator      *render_animator;

   Eext_Circle_Surface *parent_surface;
   Eina_List           *sub_surfaces;
   Eext_Circle_Object  *deactivated_obj;

   Eina_Bool            changed : 1;
};

typedef struct _Eext_Circle_Color Eext_Circle_Color;
struct _Eext_Circle_Color {
   int r, g, b, a;
};

struct _Eext_Circle_Object {
   Evas_Object          *image_widget;
   Evas_Object          *widget_object;
   void                 *widget_data;
   const char           *widget_type;

   Eina_List           *bg_image_objs;
   int                  bg_image_index;

   Eext_Circle_Surface  *surface;
   Eina_List            *items;
   Eina_List            *text_items;

   // del_func: Do only for widget_data.
   // Other things are handled by Eext_Circle_Object.
   void                (*del_func)(Eext_Circle_Object *obj);
   // disabled_func: When disabled state is changed, it will be called.
   void                (*disabled_func)(Eext_Circle_Object *obj);
   // color_changed_func: When color of an item is changed, it will be called.
   void                (*color_changed_func)(Eext_Circle_Object *obj, const char *item_name);
   //Do something before cairo render.
   void                (*render_pre_cb)(Eext_Circle_Object *obj);

   Eina_Bool             visible : 1;
   Eina_Bool             disabled : 1;
};

typedef struct _Eext_Circle_Object_Item Eext_Circle_Object_Item;
struct _Eext_Circle_Object_Item {
   Eext_Circle_Object *obj;
   const char *name;
   void *data;

   // For text draw
   const char *text;
   const char *font;
   const char *selected_font;
   int font_size;

   double radius;
   int line_width;

   int center_x;
   int center_y;

   double min_angle;
   double max_angle;

   double value;
   double min_value;
   double max_value;

   double angle;
   double angle_offset;

   Eext_Circle_Color color;

   struct {
      double angle;
      double angle_offset;
      Eext_Circle_Color color;
   } draw;

   struct {
      Ecore_Animator *animator_cb;
      double          duration;
      double          start_time;
      double          current_animator_pos;
      Eext_Circle_Color from_color;
      double          cubic_bezier_v[4];
      Eina_Bool       cubic_bezier_enabled : 1;
   } color_transit;

   struct {
      Ecore_Animator *animator_cb;
      double          duration;
      double          start_time;
      double          current_animator_pos;
      double          from_angle, from_angle_offset;
      double          cubic_bezier_v[4];
      Eina_Bool       cubic_bezier_enabled : 1;
   } angle_transit;
};

/* Datetime widget data */
typedef struct _Eext_Circle_Object_Datetime_Data Eext_Circle_Object_Datetime_Data;
struct _Eext_Circle_Object_Datetime_Data {
   Elm_Datetime_Field_Type current_type;
   double rotary_angle;
   double type_current_angle;

   Eina_Bool circle_changed : 1;
};

/* Progressbar widget data */
typedef struct _Eext_Circle_Object_Progressbar_Data Eext_Circle_Object_Progressbar_Data;
struct _Eext_Circle_Object_Progressbar_Data {
   Eext_Circle_Color color;
   Eext_Circle_Color bg_color;
   Eext_Circle_Color disabled_color;
   Eext_Circle_Color disabled_bg_color;
};

/* Slider widget data */
typedef struct _Eext_Circle_Object_Slider_Data Eext_Circle_Object_Slider_Data;
struct _Eext_Circle_Object_Slider_Data {
   double value_step;

   Eext_Circle_Color color;
   Eext_Circle_Color bg_color;
   Eext_Circle_Color disabled_color;
   Eext_Circle_Color disabled_bg_color;
};

/* Genlist widget data */
typedef struct _Eext_Circle_Object_Genlist_Data Eext_Circle_Object_Genlist_Data;
struct _Eext_Circle_Object_Genlist_Data {
   Ecore_Timer *bar_hide_timer;
   Elm_Scroller_Policy policy_h, policy_v;

   Eext_Circle_Color color;
   Eext_Circle_Color bg_color;
   Eext_Circle_Color disabled_color;
   Eext_Circle_Color disabled_bg_color;

   Eina_Bool realized_callback_added : 1;
};

/* Scroller widget data */
typedef struct _Eext_Circle_Object_Scroller_Data Eext_Circle_Object_Scroller_Data;
struct _Eext_Circle_Object_Scroller_Data {
   Ecore_Timer *bar_hide_timer;
   Elm_Scroller_Policy policy_v, policy_h;

   Eext_Circle_Color vertical_color;
   Eext_Circle_Color vertical_bg_color;
   Eext_Circle_Color horizontal_color;
   Eext_Circle_Color horizontal_bg_color;
   Eext_Circle_Color disabled_color;
   Eext_Circle_Color disabled_bg_color;

   int step_x, step_y;
   int line_width;
   double radius;
};

///////////////////////////////
// Internal APIs //////////////
///////////////////////////////

// Surface
Eext_Circle_Surface *_eext_circle_surface_init(Evas_Object *image_widget, Evas_Object *parent, Eext_Circle_Surface_Type type);

void _eext_circle_surface_finish(Eext_Circle_Surface *surface);

void _eext_circle_surface_object_remove(Eext_Circle_Surface *surface, Eext_Circle_Object *obj);

void _eext_circle_surface_object_append(Eext_Circle_Surface *surface, Eext_Circle_Object *obj);

void _eext_circle_surface_changed(Eext_Circle_Surface *surface);

Eext_Circle_Surface *_eext_circle_surface_add(Evas_Object *parent, Eext_Circle_Surface_Type type);

void _eext_circle_surface_object_exclusive_show(Eext_Circle_Surface *surface, Eext_Circle_Object *obj);

// Item
double _eext_circle_object_item_value_angle_get(const Eext_Circle_Object_Item *item, double value);

double _eext_circle_object_item_angle_value_get(const Eext_Circle_Object_Item *item, double angle);

const char *_eext_circle_text_font_get(const char *text, const char *font_family);

void _eext_circle_object_item_name_set(Eext_Circle_Object_Item *item, const char *name);

const char *_eext_circle_object_item_name_get(const Eext_Circle_Object_Item *item);

Eext_Circle_Object_Item *_eext_circle_object_item_new(void);

void _eext_circle_object_item_free(Eext_Circle_Object_Item *item);

void _eext_circle_object_item_prepend(Eext_Circle_Object *obj, Eext_Circle_Object_Item *item);

void _eext_circle_object_item_append(Eext_Circle_Object *obj, Eext_Circle_Object_Item *item);

void _eext_circle_object_item_insert_before(Eext_Circle_Object *obj, Eext_Circle_Object *before, Eext_Circle_Object_Item *item);

void _eext_circle_object_item_insert_after(Eext_Circle_Object *obj, Eext_Circle_Object *after, Eext_Circle_Object_Item *item);

void _eext_circle_object_item_remove(Eext_Circle_Object_Item *item);

void _eext_circle_object_item_line_width_set(Eext_Circle_Object_Item *item, int line_width);

int _eext_circle_object_item_line_width_get(const Eext_Circle_Object_Item *item);

void _eext_circle_object_item_angle_set(Eext_Circle_Object_Item *item, double angle);

double _eext_circle_object_item_angle_get(const Eext_Circle_Object_Item *item);

void _eext_circle_object_item_angle_offset_set(Eext_Circle_Object_Item *item, double angle_offset);

double _eext_circle_object_item_angle_offset_get(const Eext_Circle_Object_Item *item);

void _eext_circle_object_item_angle_min_max_set(Eext_Circle_Object_Item *item,
                                                double min_angle, double max_angle);

void _eext_circle_object_item_angle_min_max_get(const Eext_Circle_Object_Item *item,
                                                double *min_angle, double *max_angle);

void _eext_circle_object_item_value_min_max_set(Eext_Circle_Object_Item *item,
                                                double min_value, double max_value);

void _eext_circle_object_item_value_min_max_get(const Eext_Circle_Object_Item *item,
                                                double *min_value, double *max_value);

void _eext_circle_object_item_value_set(Eext_Circle_Object_Item *item, double value);

double _eext_circle_object_item_value_get(const Eext_Circle_Object_Item *item);

void _eext_circle_object_item_color_set(Eext_Circle_Object_Item *item,
                                        int r, int g, int b, int a);

void _eext_circle_object_item_color_get(const Eext_Circle_Object_Item *item,
                                        int *r, int *g, int *b, int *a);

void _eext_circle_object_item_radius_set(Eext_Circle_Object_Item *item, double radius);

double _eext_circle_object_item_radius_get(const Eext_Circle_Object_Item *item);

void _eext_circle_object_item_text_set(Eext_Circle_Object_Item *item,
                                       const char *text,
                                       const char *font);

void _eext_circle_object_item_text_get(const Eext_Circle_Object_Item *item,
                                       const char **text,
                                       const char **font);

void _eext_circle_object_item_font_size_set(Eext_Circle_Object_Item *item,
                                            int font_size);

int _eext_circle_object_item_font_size_get(const Eext_Circle_Object_Item *item);

void _eext_circle_object_item_color_transit_set(Eext_Circle_Object_Item *item, double duration,
                                                int to_r, int to_g, int to_b, int to_a,
                                                Eina_Bool cubic_bezier_enabled,
                                                double cubic_bezier_p1_x, double cubic_bezier_p1_y,
                                                double cubic_bezier_p2_x, double cubic_bezier_p2_y
                                               );

void _eext_circle_object_item_angle_transit_set(Eext_Circle_Object_Item *item, double duration,
                                                double to_angle, double to_angle_offset,
                                                Eina_Bool cubic_bezier_enabled,
                                                double cubic_bezier_p1_x, double cubic_bezier_p1_y,
                                                double cubic_bezier_p2_x, double cubic_bezier_p2_y);

void _eext_circle_object_item_value_transit_set(Eext_Circle_Object_Item *item, double duration,
                                                double to_value,
                                                Eina_Bool cubic_bezier_enabled,
                                                double cubic_bezier_p1_x, double cubic_bezier_p1_y,
                                                double cubic_bezier_p2_x, double cubic_bezier_p2_y);

// Circle Common Private
void _eext_circle_object_init(Evas_Object *obj, Evas_Object *widget_object, Eext_Circle_Surface *surface);

void _eext_circle_object_finish(Evas_Object *obj, Eext_Circle_Object *circle_obj);

void _eext_circle_object_changed(Eext_Circle_Object *obj);

Eext_Circle_Object_Item *_eext_circle_object_item_get(const Eext_Circle_Object *obj, const char *name);

void _eext_circle_object_hide(Eext_Circle_Object *obj);

void _eext_circle_object_show(Eext_Circle_Object *obj);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif


#endif /* __EFL_EXTENSION_CIRCLE_PRIVATE_H__ */
