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

#define ELM_SCALE_SIZE(x) (((x) / elm_app_base_scale_get()) * elm_config_scale_get())

#define FLOATINGBUTTON_MID_PADDING_WIDTH 20

static const char *BTN1_PART = "button1";
static const char *BTN2_PART = "button2";
static const char *DRAGABLE_PART = "elm.dragable.fb";
static const char *TRACK_PART = "elm.track.fb";

typedef struct _Eext_Floatingbutton_Data {

   Evas_Object *box;
   Evas_Object *vg;
   Efl_VG_Shape *base_shape;

   Evas_Object *btn1;
   Evas_Object *btn2;

   Evas_Coord x;
   int dir;

   Eext_Floatingbutton_Pos pos;
   double last_pos;
   double pos_table[EEXT_FLOATINGBUTTON_LAST];

   Eina_Bool pos_fixed : 1;

} Eext_Floatingbutton_Data;

static void
_pos_recalc(Eo *obj, Eext_Floatingbutton_Data *sd)
{
   Evas_Coord w = 0;
   Eina_List *items;
   Evas_Object *edje = elm_layout_edje_get(obj);
   int count;

   items = elm_box_children_get(sd->box);
   count = eina_list_count(items);
   eina_list_free(items);

   edje_object_part_geometry_get(edje, TRACK_PART, NULL, NULL, &w, NULL);

   if (count >= 2)
     {
        sd->pos_table[EEXT_FLOATINGBUTTON_LEFT] = ELM_SCALE_SIZE(261) / (double)w;
        sd->pos_table[EEXT_FLOATINGBUTTON_RIGHT] = (w - ELM_SCALE_SIZE(261)) / (double)w;
     }
   else
     {
        sd->pos_table[EEXT_FLOATINGBUTTON_LEFT] = ELM_SCALE_SIZE(129) / (double)w;
        sd->pos_table[EEXT_FLOATINGBUTTON_RIGHT] = (w -ELM_SCALE_SIZE(129)) / (double)w;
     }
}

static void
_box_recalc(Eo *obj, Eext_Floatingbutton_Data *sd)
{
   int count;
   char buf[200];
   Evas_Object *edje = elm_layout_edje_get(obj);

   Eina_List *items = elm_box_children_get(sd->box);
   count = eina_list_count(items);
   eina_list_free(items);

   snprintf(buf, sizeof(buf), "elm,floatingbutton,state,%d", (count >= 2));
   elm_layout_signal_emit(obj, buf, "elm");
   edje_object_message_signal_process(edje);

   _pos_recalc(obj, sd);
}

static void
_resize_cb(void *data, Evas *e EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   _pos_recalc(obj, data);
}

static void
_vg_resize_cb(void *data, Evas *e EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   Evas_Coord x, y, w, h;
   evas_object_geometry_get(obj, &x, &y, &w, &h);
   //Base Shape
   evas_vg_shape_shape_reset((Efl_VG_Shape *)data);
   evas_vg_shape_shape_append_rect((Efl_VG_Shape *)data, 0, 0, w, h,
                                   ELM_SCALE_SIZE(50), 100);
}

static Eina_Bool
_anim_cb(void *data, double pos)
{
   Eext_Floatingbutton_Data *fbd = eo_data_scope_get(data, EEXT_FLOATINGBUTTON_CLASS);
   double cur_pos;
   double pivot;

   pivot = ecore_animator_pos_map(pos, ECORE_POS_MAP_DECELERATE, 0.2, 0.0);

   if (fbd->pos_table[fbd->pos] > fbd->last_pos)
     cur_pos = fbd->last_pos + (fbd->pos_table[fbd->pos] - fbd->last_pos) * pivot;
   else
     cur_pos = fbd->last_pos - (fbd->last_pos - fbd->pos_table[fbd->pos]) * pivot;

   edje_object_part_drag_value_set(elm_layout_edje_get(data), DRAGABLE_PART, cur_pos, 0.5);

   return ECORE_CALLBACK_RENEW;
}

static void
_update_pos(Eo *obj, Eext_Floatingbutton_Data *fbd, Eina_Bool anim)
{
   edje_object_part_drag_value_get(elm_layout_edje_get(obj), DRAGABLE_PART, &fbd->last_pos, NULL);

   if (!anim)
     edje_object_part_drag_value_set(elm_layout_edje_get(obj), DRAGABLE_PART, fbd->pos_table[fbd->pos], 0.5);
   else
     ecore_animator_timeline_add(0.2, _anim_cb, obj);
}

static void
_on_mouse_down(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Eext_Floatingbutton_Data *fbd = data;
   Evas_Object *edje = elm_layout_edje_get(obj);

   edje_object_part_geometry_get(edje, DRAGABLE_PART, &fbd->x, NULL, NULL, NULL);

   fbd->dir = 0;
}

static void
_on_mouse_move(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Eext_Floatingbutton_Data *fbd = data;
   Evas_Object *edje = elm_layout_edje_get(obj);
   Evas_Coord x, finger_size = elm_config_finger_size_get();

   edje_object_part_geometry_get(edje, DRAGABLE_PART, &x, NULL, NULL, NULL);

   if (x > (fbd->x + finger_size)) fbd->dir = 1;
   else if(x < (fbd->x - finger_size)) fbd->dir = -1;
}

static void
_on_mouse_up(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Eext_Floatingbutton_Data *fbd = data;
   Evas_Object *edje = elm_layout_edje_get(obj);
   Evas_Coord cur_x, track_w, left_w;
   double cur_pos;
   int i, count;

   if (fbd->dir)
     {
        Eina_List *items = elm_box_children_get(fbd->box);
        count = eina_list_count(items);
        eina_list_free(items);

        edje_object_part_geometry_get(edje, DRAGABLE_PART, &cur_x, NULL, NULL, NULL);
        edje_object_part_geometry_get(edje, TRACK_PART, NULL, NULL, &track_w, NULL);
        edje_object_part_geometry_get(edje, "elm.track.left_hidden", NULL, NULL, &left_w, NULL);

        cur_pos = (left_w + cur_x + ELM_SCALE_SIZE((count >= 2) ? (290 / 2) : (185 / 2))) / (double)track_w;

        if (fbd->dir > 0)
          {
             for (i = EEXT_FLOATINGBUTTON_LEFT_OUT; i < EEXT_FLOATINGBUTTON_RIGHT_OUT; ++i)
               if (cur_pos < (fbd->pos_table[i] + ((i == 1) ? 0.15 : 0))) break;
          }
        else
          {
             for (i = EEXT_FLOATINGBUTTON_RIGHT_OUT; i > EEXT_FLOATINGBUTTON_LEFT_OUT; --i)
               if (cur_pos > (fbd->pos_table[i] - ((i == 3) ? 0.15 : 0))) break;
          }

        fbd->pos = i;
     }

   _update_pos(obj, fbd, EINA_TRUE);
}

EOLIAN static void
_eext_floatingbutton_evas_object_smart_del(Eo *obj, Eext_Floatingbutton_Data *pd EINA_UNUSED)
{
   eo_do_super(obj, MY_CLASS, evas_obj_smart_del());
}

EOLIAN static void
_eext_floatingbutton_pos_fixed_set(Eo *obj, Eext_Floatingbutton_Data *sd, Eina_Bool pos_fixed)
{
   char buf[200];

   pos_fixed = !!pos_fixed;
   sd->pos_fixed = pos_fixed;

   if (pos_fixed)
     snprintf(buf, sizeof(buf), "elm,floatingbutton,state,fixed");
   else
     snprintf(buf, sizeof(buf), "elm,floatingbutton,state,nonfixed");

   elm_layout_signal_emit(obj, buf, "elm");
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
   evas_object_event_callback_add(obj, EVAS_CALLBACK_RESIZE, _resize_cb, priv);

   elm_layout_signal_callback_add(obj, "mouse,down,1", DRAGABLE_PART, _on_mouse_down, priv);
   elm_layout_signal_callback_add(obj, "mouse,up,1", DRAGABLE_PART, _on_mouse_up, priv);
   elm_layout_signal_callback_add(obj, "mouse,move", DRAGABLE_PART, _on_mouse_move, priv);

   priv->vg = evas_object_vg_add(evas_object_evas_get(obj));
   elm_layout_content_set(obj, "elm.swallow.vg", priv->vg);
   Efl_VG *base_root = evas_object_vg_root_node_get(priv->vg);
   priv->base_shape = evas_vg_shape_add(base_root);
   evas_vg_node_color_set(priv->base_shape, 255, 255, 255, 255);
   evas_object_event_callback_add(priv->vg, EVAS_CALLBACK_RESIZE, _vg_resize_cb, priv->base_shape);

   priv->box = elm_box_add(obj);
   elm_box_horizontal_set(priv->box, EINA_TRUE);
   elm_box_padding_set(priv->box, ELM_SCALE_SIZE(FLOATINGBUTTON_MID_PADDING_WIDTH), 0);
   elm_object_part_content_set(obj, "elm.swallow.box", priv->box);

   priv->pos = EEXT_FLOATINGBUTTON_RIGHT_OUT;
   priv->pos_fixed = EINA_FALSE;

   priv->pos_table[EEXT_FLOATINGBUTTON_LEFT_OUT] = 0.0;
   priv->pos_table[EEXT_FLOATINGBUTTON_CENTER] = 0.5;
   priv->pos_table[EEXT_FLOATINGBUTTON_RIGHT_OUT] = 1.0;

   _pos_recalc(obj, priv);
   _update_pos(obj, priv, EINA_FALSE);
}

EOLIAN static void
_eext_floatingbutton_class_constructor(Eo_Class *klass)
{
   evas_smart_legacy_type_register(MY_CLASS_NAME_LEGACY, klass);
}

static void
_btn_del_cb(void *data, Evas *e EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   Eext_Floatingbutton_Data *fbd = eo_data_scope_get((Eo *)data, EEXT_FLOATINGBUTTON_CLASS);

   elm_box_unpack(fbd->box, obj);
   _box_recalc(data, fbd);

   if (fbd->btn1 == obj) fbd->btn1 = NULL;
   else if (fbd->btn2 == obj) fbd->btn2 = NULL;

   _update_pos(obj, fbd, EINA_FALSE);
}

EOLIAN static Eina_Bool
_eext_floatingbutton_elm_container_content_set(Eo *obj, Eext_Floatingbutton_Data *sd, const char *part, Evas_Object *content)
{
   Eina_Bool int_ret = EINA_FALSE;

   if ((!part) || (!content)) return int_ret;

   if (!strcmp(part, BTN1_PART))
     {
        elm_object_style_set(content, "floatingbutton");

        if (sd->btn1) evas_object_del(sd->btn1);

        sd->btn1 = content;
        evas_object_show(sd->btn1);
        elm_box_pack_start(sd->box, content);

        evas_object_event_callback_add(content, EVAS_CALLBACK_DEL, _btn_del_cb, obj);

        int_ret = EINA_TRUE;
     }
   else if (!strcmp(part, BTN2_PART))
     {
        elm_object_style_set(content, "floatingbutton");

        if (sd->btn2) evas_object_del(sd->btn2);

        sd->btn2 = content;
        evas_object_show(sd->btn2);
        elm_box_pack_end(sd->box, content);

        evas_object_event_callback_add(content, EVAS_CALLBACK_DEL, _btn_del_cb, obj);

        int_ret = EINA_TRUE;
     }
   else
     eo_do_super(obj, MY_CLASS, int_ret = elm_obj_container_content_set(part, content));

   _box_recalc(obj, sd);

   return int_ret;
}

EOLIAN static Evas_Object *
_eext_floatingbutton_elm_container_content_get(Eo *obj, Eext_Floatingbutton_Data *sd, const char *part)
{
   Evas_Object *ret = NULL;

   if (!part) return ret;

   if (!strcmp(part, BTN1_PART))
     ret = sd->btn1;
   else if (!strcmp(part, BTN2_PART))
     ret = sd->btn2;
   else
     eo_do_super(obj, MY_CLASS, ret = elm_obj_container_content_get(part));

   return ret;
}

EOLIAN static Evas_Object *
_eext_floatingbutton_elm_container_content_unset(Eo *obj, Eext_Floatingbutton_Data *sd, const char *part)
{
   Evas_Object *ret = NULL;

   if (!part) return ret;

   if (!strcmp(part, BTN1_PART) && sd->btn1)
     {
        elm_box_unpack(sd->box, sd->btn1);
        evas_object_event_callback_del_full(sd->btn1, EVAS_CALLBACK_DEL, _btn_del_cb, obj);

        ret = sd->btn1;
        sd->btn1 = NULL;
     }
   else if (!strcmp(part, BTN2_PART) && sd->btn2)
     {
        elm_box_unpack(sd->box, sd->btn2);
        evas_object_event_callback_del_full(sd->btn2, EVAS_CALLBACK_DEL, _btn_del_cb, obj);

        ret = sd->btn2;
        sd->btn2 = NULL;
     }
   else
     eo_do_super(obj, MY_CLASS, ret = elm_obj_container_content_unset(part));

   _box_recalc(obj, sd);

   return ret;
}


#include "eext_floatingbutton.eo.c"
