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

#ifndef __EFL_EXTENSION_COMMON_PRIVATE_H__
#define __EFL_EXTENSION_COMMON_PRIVATE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Eext_Index_Layout_Item Eext_Index_Layout_Item;
struct _Eext_Index_Layout_Item {
   EEXT_OBJECT_ITEM;
   Eext_Circle_Object_Item *item;

   const char *letter;
   int priority;
   Eina_Bool selected;

   Evas_Smart_Cb func;
};

/* Index Layout data */
typedef struct _Eext_Index_Layout_Data Eext_Index_Layout_Data;
struct _Eext_Index_Layout_Data {
   Evas_Object *main_ly;
   Evas_Object *panel;
   Evas_Object *index;
   Evas_Object *cue;
   Ecore_Timer *hide_timer;

   int max_index;
   int max_item;
   int highlighted_index;
   int index_priority;
   int *item_index;
   int default_item_count;

   Eina_List *current_list;
   Eina_List *item_list;
   Eina_List *omit_list;

   Eina_Bool panel_hide;
   Eina_Bool second_list_exist;
};

////////////////////////////////////////////////////
// Eext_More_Option_Layout /////////////////////////
////////////////////////////////////////////////////
typedef long int Eext_More_Option_Layout_Queue_Element;

typedef struct _Eext_More_Option_Layout_Queue Eext_More_Option_Layout_Queue;
struct _Eext_More_Option_Layout_Queue {
   Eext_More_Option_Layout_Queue_Element *q;
   int first;
   int last;
   int count;
   int size;
};

typedef struct _Eext_More_Option_Layout_Velocity_Tracker Eext_More_Option_Layout_Velocity_Tracker;
struct _Eext_More_Option_Layout_Velocity_Tracker {
   Eext_More_Option_Layout_Queue *queue_delta_x;
   Eext_More_Option_Layout_Queue *queue_delta_y;
   Eext_More_Option_Layout_Queue *queue_event_time;
   double computed_velocity_x;
   double computed_velocity_y;
   Eina_Bool is_computed;
};

// animator update callback
typedef void (*Eext_More_Option_Layout_Animator_Update_Cb) (void *data, double animated_value, double fraction);
// interpolation callback
typedef double (*Eext_More_Option_Layout_Interpolator_Cb) (double input);
// increment callback
typedef double (*Eext_More_Option_Layout_Incrementor_Cb) (double start, double stop, double fraction);

typedef struct _Eext_More_Option_Layout_Controller Eext_More_Option_Layout_Controller;
struct _Eext_More_Option_Layout_Controller {
   Ecore_Animator  *animator;
   Eina_Bool       running;
   void            *data;
};

typedef struct _Eext_More_Option_Layout_Animator_Info Eext_More_Option_Layout_Animator_Info;
struct _Eext_More_Option_Layout_Animator_Info {
   double     start_value;
   double     stop_value;
   double     duration;

   Eext_More_Option_Layout_Animator_Update_Cb update_cb;
   Eext_More_Option_Layout_Interpolator_Cb    interpolator;
   Eext_More_Option_Layout_Incrementor_Cb     incrementor;

   Eext_More_Option_Layout_Controller *controller;
};

typedef struct _Eext_More_Option_Layout_Item_Manager_Layout_Data Eext_More_Option_Layout_Item_Manager_Layout_Data;
struct _Eext_More_Option_Layout_Item_Manager_Layout_Data {
   Evas_Object *more_option_layout;
   Evas_Object *item_manager;
   Eina_List *item_list;
   int item_count;
   int previous_index;
   int selected_index;
   double previous_scroll_x;
   double scroll_x;
   Eina_Bool item_scrolled;
   Eina_Bool item_manager_scrolled;
   Evas_Coord pressed_x;
   Evas_Coord pressed_y;
   Eina_Bool pressed;

   Eext_More_Option_Layout_Velocity_Tracker *velocity;
   Eext_More_Option_Layout_Animator_Info *position_animator;
};

typedef struct _Eext_More_Option_Layout_Data Eext_More_Option_Layout_Data;
struct _Eext_More_Option_Layout_Data {
   Evas_Object* parent;
   Evas_Object* more_option_layout;
   Evas_Object* panel;
   Evas_Object* index_layout;
   Evas_Object* item_manager_layout;
   Evas_Object* index;
   Evas_Object* bg;
   int selected_index_item;
   Eext_More_Option_Layout_Direction direction;
   Eina_Bool panel_activated;
};

typedef struct _Eext_More_Option_Layout_Item_Coords Eext_More_Option_Layout_Item_Coords;
struct _Eext_More_Option_Layout_Item_Coords {
   double x;
   double z;
   double scale;
   double depth;
   double angle;
};

typedef struct _Eext_More_Option_Layout_Item Eext_More_Option_Layout_Item;
struct _Eext_More_Option_Layout_Item {
   EEXT_OBJECT_ITEM;
   int index;
   Evas_Object *item_manager_layout;
   Eext_More_Option_Layout_Item_Coords coords;
};


#ifdef __cplusplus
}
#endif


#endif /* __EFL_EXTENSION_COMMON_PRIVATE_H__ */
