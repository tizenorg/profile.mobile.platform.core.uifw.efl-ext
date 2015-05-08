/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *		  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#define ELM_INTERNAL_API_ARGESFSDFEFC 1

#include <Eo.h>

#include "efl_extension.h"
#include "efl_extension_private.h"

#include <elm_widget.h>

#define MY_CLASS EEXT_FLOATINGBUTTON_CLASS

#define MY_CLASS_NAME "Eext_Floatingbutton"
#define MY_CLASS_NAME_LEGACY "eext_floatingbutton"

static const char *BTN1_PART = "button1";
static const char *BTN2_PART = "button2";

typedef struct _Eext_Floatingbutton_Data {

   Evas_Object *scroller;
   Evas_Object *main_layout;
   Evas_Object *bg;
   Evas_Object *bg1;
   Evas_Object *event;

   Evas_Object *parent;

   Eext_Floatingbutton_Pos pos;
   Evas_Coord base_w;
   Evas_Coord down_x, down_y;
   Evas_Coord pos_table[EEXT_FLOATINGBUTTON_LAST];

   Eina_Bool pos_fixed : 1;

} Eext_Floatingbutton_Data;


static void
_scroller_anim_start_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eext_Floatingbutton_Data *fbd = (Eext_Floatingbutton_Data *)data;
   if (eo_do(fbd->main_layout, elm_obj_container_content_get(BTN1_PART)) &&
       eo_do(fbd->main_layout, elm_obj_container_content_get(BTN2_PART)))
     {
        char buf[200];
        snprintf(buf, sizeof(buf), "elm,floatingbutton,state,%d", fbd->pos);
        elm_object_signal_emit(fbd->main_layout, buf, "elm");
     }
}

static void
_update_pos(Eo *obj, Eext_Floatingbutton_Data *fbd, Eina_Bool scroll)
{
   const char *str;
   int button_height = 0;
   Evas_Coord base_w;

   evas_object_geometry_get(elm_object_top_widget_get(fbd->parent), NULL, NULL, &base_w, NULL);

   if (!fbd->pos_table[0])
     {
        int handler_size = 0, base_diff = 0;
        Evas_Coord left_w, right_w;

        str = edje_object_data_get(elm_layout_edje_get(fbd->main_layout), "handler_size");
        if (str) handler_size = (int)(atoi(str)
                                      / edje_object_base_scale_get(elm_layout_edje_get(fbd->main_layout))
                                      * elm_config_scale_get()
                                      * elm_object_scale_get(obj));

        if (base_w < fbd->base_w) base_diff = fbd->base_w - base_w;

        edje_object_part_geometry_get(elm_layout_edje_get(fbd->main_layout), BTN1_PART, NULL, NULL, &left_w, NULL);
        edje_object_part_geometry_get(elm_layout_edje_get(fbd->main_layout), BTN2_PART, NULL, NULL, &right_w, NULL);

        fbd->pos_table[0] = base_diff + base_w + left_w + right_w - handler_size;
        fbd->pos_table[1] = base_diff + base_w + left_w;
        fbd->pos_table[2] = base_diff + base_w + handler_size;
        fbd->pos_table[3] = base_diff + left_w + (base_w / 2);
        fbd->pos_table[4] = base_diff + left_w + right_w - handler_size;
        fbd->pos_table[5] = base_diff + left_w;
        fbd->pos_table[6] = base_diff + handler_size;
     }

   str = edje_object_data_get(elm_layout_edje_get(fbd->main_layout), "button_height");
   if (str) button_height = (int)(atoi(str)
                                  / edje_object_base_scale_get(elm_layout_edje_get(fbd->main_layout))
                                  * elm_config_scale_get()
                                  * elm_object_scale_get(obj));

   if (scroll)
   {
     elm_scroller_region_bring_in(fbd->scroller, fbd->pos_table[fbd->pos], 0, base_w, button_height);

   }
   else
     elm_scroller_region_show(fbd->scroller, fbd->pos_table[fbd->pos], 0, base_w, button_height);
}

EOLIAN static void
_eext_floatingbutton_evas_object_smart_resize(Eo *obj, Eext_Floatingbutton_Data *sd, Evas_Coord w, Evas_Coord h)
{
   const char *str;
   int button_height = 0;
   Evas_Coord base_w, base_h, left_w, right_w;

   eo_do_super(obj, MY_CLASS, evas_obj_smart_resize(w, h));

   evas_object_geometry_get(elm_object_top_widget_get(sd->parent), NULL, NULL, &base_w, &base_h);
   if (base_w >= base_h) sd->base_w = base_w;
   else sd->base_w = base_h;

   edje_object_part_geometry_get(elm_layout_edje_get(sd->main_layout), BTN1_PART, NULL, NULL, &left_w, NULL);
   edje_object_part_geometry_get(elm_layout_edje_get(sd->main_layout), BTN2_PART, NULL, NULL, &right_w, NULL);

   str = edje_object_data_get(elm_layout_edje_get(sd->main_layout), "button_height");
   if (str) button_height = (int)(atoi(str)
                                  / edje_object_base_scale_get(elm_layout_edje_get(sd->main_layout))
                                  * elm_config_scale_get()
                                  * elm_object_scale_get(obj));

   evas_object_size_hint_min_set(sd->bg, (sd->base_w * 2 + left_w + right_w), button_height);

   sd->pos_table[0] = 0;

   _update_pos(obj, sd, EINA_FALSE);
}

EOLIAN static void
_eext_floatingbutton_evas_object_smart_del(Eo *obj, Eext_Floatingbutton_Data *pd EINA_UNUSED)
{
   eo_do_super(obj, MY_CLASS, evas_obj_smart_del());
}

static void
_on_mouse_down(void *data, Evas *e, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   Eext_Floatingbutton_Data *fbd = data;
   Evas_Event_Mouse_Down *ev = event_info;

   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;

   fbd->down_x = ev->canvas.x;
   fbd->down_y = ev->canvas.y;
}

static void
_on_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eext_Floatingbutton_Data *fbd = data;
   Evas_Event_Mouse_Up *ev = event_info;
   Evas_Coord diff_x;
   int finger_size;

   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;

   diff_x = fbd->down_x - ev->canvas.x;

   finger_size = elm_config_finger_size_get();

   if (diff_x > finger_size && fbd->pos > EEXT_FLOATINGBUTTON_LEFT_OUT)
     fbd->pos--;
   else if (diff_x < -finger_size && fbd->pos < EEXT_FLOATINGBUTTON_RIGHT_OUT)
     fbd->pos++;

   _update_pos(obj, fbd, EINA_TRUE);
}

EOLIAN static void
_eext_floatingbutton_pos_fixed_set(Eo *obj EINA_UNUSED, Eext_Floatingbutton_Data *sd, Eina_Bool pos_fixed)
{
   pos_fixed = !!pos_fixed;

   sd->pos_fixed = pos_fixed;
}

EOLIAN static Eina_Bool
_eext_floatingbutton_pos_fixed_get(Eo *obj EINA_UNUSED, Eext_Floatingbutton_Data *sd)
{
   return sd->pos_fixed;
}

EOLIAN static void
_eext_floatingbutton_pos_set(Eo *obj, Eext_Floatingbutton_Data *sd, Eext_Floatingbutton_Pos pos)
{
   if (sd->pos_fixed) return;

   if (pos < EEXT_FLOATINGBUTTON_LEFT_OUT || pos > EEXT_FLOATINGBUTTON_RIGHT_OUT) return;
   sd->pos = pos;

   _update_pos(obj, sd, EINA_FALSE);
}

EOLIAN static Eext_Floatingbutton_Pos
_eext_floatingbutton_pos_get(Eo *obj EINA_UNUSED, Eext_Floatingbutton_Data *sd)
{
   return sd->pos;
}

EOLIAN static void
_eext_floatingbutton_elm_widget_parent_set(Eo *obj EINA_UNUSED, Eext_Floatingbutton_Data *sd, Evas_Object *parent)
{
   sd->parent = parent;
}

EAPI Evas_Object *
eext_floatingbutton_add(Evas_Object *parent)
{
   if (!parent) return NULL;
   Evas_Object *obj = eo_add(MY_CLASS, parent);

   return obj;
}

EOLIAN static void
_eext_floatingbutton_eo_base_constructor(Eo *obj, Eext_Floatingbutton_Data *sd EINA_UNUSED)
{
   eo_do_super(obj, MY_CLASS, eo_constructor());
   eo_do(obj,
         evas_obj_type_set(MY_CLASS_NAME_LEGACY));
}

EOLIAN static void
_eext_floatingbutton_evas_object_smart_add(Eo *obj, Eext_Floatingbutton_Data *priv)
{
   char buf[1024];

   eo_do_super(obj, MY_CLASS, evas_obj_smart_add());
   elm_widget_sub_object_parent_add(obj);

   snprintf(buf, sizeof(buf), "elm/floatingbutton/base/%s", elm_widget_style_get(obj));
   elm_layout_file_set(obj, EFL_EXTENSION_EDJ, buf);

   priv->scroller = elm_scroller_add(obj);
   elm_widget_sub_object_add(obj, priv->scroller);
   elm_scroller_policy_set(priv->scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_scroller_movement_block_set(priv->scroller,
                                   ELM_SCROLLER_MOVEMENT_BLOCK_HORIZONTAL |
                                   ELM_SCROLLER_MOVEMENT_BLOCK_VERTICAL);


   priv->main_layout = elm_layout_add(obj);
   snprintf(buf, sizeof(buf), "elm/floatingbutton/main_layout/%s", elm_widget_style_get(obj));
   elm_layout_file_set(priv->main_layout, EFL_EXTENSION_EDJ, buf);
   elm_object_content_set(priv->scroller, priv->main_layout);

   priv->bg = evas_object_rectangle_add(evas_object_evas_get(obj));
   elm_widget_sub_object_add(obj, priv->bg);
   evas_object_color_set(priv->bg, 0, 0, 0, 0);
   elm_object_part_content_set(priv->main_layout, "elm.swallow.bg", priv->bg);

   priv->bg1 = elm_button_add(evas_object_evas_get(obj));
   elm_widget_sub_object_add(obj, priv->bg1);
   elm_object_style_set(priv->bg1, "fb_bg");
   elm_object_part_content_set(priv->main_layout, "elm.swallow.bg1", priv->bg1);

   priv->event = evas_object_rectangle_add(evas_object_evas_get(obj));
   elm_widget_sub_object_add(obj, priv->event);
   evas_object_color_set(priv->event, 0, 0, 0, 0);
   elm_object_part_content_set(priv->main_layout, "elm.swallow.button_event", priv->event);
   evas_object_event_callback_add(priv->event, EVAS_CALLBACK_MOUSE_DOWN, _on_mouse_down, priv);
   evas_object_event_callback_add(priv->event, EVAS_CALLBACK_MOUSE_UP, _on_mouse_up, priv);


   priv->pos = EEXT_FLOATINGBUTTON_RIGHT_OUT;
   priv->pos_fixed = EINA_FALSE;

   evas_object_smart_callback_add(priv->scroller, "scroll,anim,stop", _scroller_anim_start_cb, priv);

   elm_object_part_content_set(obj, "elm.swallow.content", priv->scroller);
}

EOLIAN static void
_eext_floatingbutton_class_constructor(Eo_Class *klass)
{
   evas_smart_legacy_type_register(MY_CLASS_NAME_LEGACY, klass);
}

EOLIAN static Eina_Bool
_eext_floatingbutton_elm_container_content_set(Eo *obj, Eext_Floatingbutton_Data *sd, const char *part, Evas_Object *content)
{
   Eina_Bool int_ret = EINA_FALSE;

   if (!strcmp(part, BTN1_PART) || !strcmp(part, BTN2_PART))
     {
        int_ret = elm_layout_content_set(sd->main_layout, part, content);
        elm_object_style_set(content, "fb_button");
        char buf[200];
        snprintf(buf, sizeof(buf), "elm,state,%s,visible", part);
        elm_object_signal_emit(sd->main_layout, buf, "elm");
     }
   else
     eo_do_super(obj, MY_CLASS, int_ret = elm_obj_container_content_set(part, content));
   return EINA_TRUE;
}

EOLIAN static Evas_Object *
_eext_floatingbutton_elm_container_content_get(Eo *obj, Eext_Floatingbutton_Data *sd, const char *part)
{
   Evas_Object *ret;

   if (!strcmp(part, BTN1_PART) || !strcmp(part, BTN2_PART))
     ret = elm_layout_content_get(sd->main_layout, part);
   else 
     eo_do_super(obj, MY_CLASS, ret = elm_obj_container_content_get(part));

   return ret;
}

EOLIAN static Evas_Object *
_eext_floatingbutton_elm_container_content_unset(Eo *obj, Eext_Floatingbutton_Data *sd, const char *part)
{
   Evas_Object *ret;

   if (!strcmp(part, BTN1_PART) || !strcmp(part, BTN2_PART))
     {
        ret = elm_layout_content_unset(sd->main_layout, part);
        char buf[200];
        snprintf(buf, sizeof(buf), "elm,state,%s,hidden", part);
        elm_object_signal_emit(sd->main_layout, buf, "elm");
     }
   else
      eo_do_super(obj, MY_CLASS, ret = elm_obj_container_content_unset(part));
   return ret;
}

#include "eext_floatingbutton.eo.c"
