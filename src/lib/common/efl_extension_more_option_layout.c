#include "efl_extension.h"
#include "efl_extension_private.h"
#include "circle/efl_extension_circle_private.h"
#include "common/efl_extension_common_private.h"

#define _MORE_OPTION_LAYOUT_DEBUG_ENABLED 0

// ====================================== util ==================================== //
#define _DEGREES_TO_RADIANS(degrees) (degrees * M_PI / 180.0)
#define _RADIANS_TO_DEGREES(radians) (radians * 180.0 / M_PI)

// -----------------------------------------queue util --------------------------------------//

static Eext_More_Option_Layout_Queue *
queue_create(int size)
{
   Eext_More_Option_Layout_Queue *q = calloc(1, sizeof(Eext_More_Option_Layout_Queue));

   q->first = 0;
   q->last = size-1;
   q->count = 0;
   q->size = size;
   q->q = (Eext_More_Option_Layout_Queue_Element *)calloc(size, sizeof(Eext_More_Option_Layout_Queue_Element));

   return q;
}

static void
queue_reset(Eext_More_Option_Layout_Queue *q)
{
   q->first = 0;
   q->last = q->size-1;
   q->count = 0;
}

static void
enqueue(Eext_More_Option_Layout_Queue *q, Eext_More_Option_Layout_Queue_Element x)
{
   if (q->count >= q->size)
     {
        // queue is full
     }
   else
     {
        q->last = (q->last+1) % q->size;
        q->q[ q->last ] = x;
        q->count = q->count + 1;
     }
}

static Eext_More_Option_Layout_Queue_Element
dequeue(Eext_More_Option_Layout_Queue *q)
{
   Eext_More_Option_Layout_Queue_Element x = 0;

   if (q->count <= 0)
     {
        // queue is empty
     }
   else
     {
        x = q->q[ q->first ];
        q->first = (q->first+1) % q->size;
        q->count = q->count - 1;
     }

   return(x);
}

static void
queue_destroy(Eext_More_Option_Layout_Queue *q)
{
   if (q)
     {
        free(q->q);
        free(q);
        q = NULL;
     }
}

static Eina_Bool
is_full(Eext_More_Option_Layout_Queue *q)
{
   return (q->count >= q->size) ? EINA_TRUE : EINA_FALSE;
}

// -----------------------------------------velocity util --------------------------------------//
#define MAX_CAPACITY 100
#define VELOCITY_COMPUTE_UNIT 0.1	// calculate velocity per last 100ms

static Eext_More_Option_Layout_Velocity_Tracker *
velocity_tracker_create()
{
   Eext_More_Option_Layout_Velocity_Tracker *velocity = (Eext_More_Option_Layout_Velocity_Tracker *) calloc(1, sizeof(Eext_More_Option_Layout_Velocity_Tracker));
   velocity->queue_delta_x = queue_create(MAX_CAPACITY);
   velocity->queue_delta_y = queue_create(MAX_CAPACITY);
   velocity->queue_event_time = queue_create(MAX_CAPACITY);
   velocity->is_computed = EINA_FALSE;

   return velocity;
}

static void
velocity_tracker_add_movement(Eext_More_Option_Layout_Velocity_Tracker *velocity, int x, int y, time_t time)
{
   if (is_full(velocity->queue_delta_x))
     {
        dequeue(velocity->queue_delta_x);
        dequeue(velocity->queue_delta_y);
        dequeue(velocity->queue_event_time);
     }

   enqueue(velocity->queue_delta_x, x);
   enqueue(velocity->queue_delta_y, y);
   enqueue(velocity->queue_event_time, time);
   velocity->is_computed = EINA_FALSE;
}

static void
velocity_tracker_reset(Eext_More_Option_Layout_Velocity_Tracker *velocity)
{
   queue_reset(velocity->queue_delta_x);
   queue_reset(velocity->queue_delta_y);
   queue_reset(velocity->queue_event_time);
   velocity->is_computed = EINA_FALSE;
}

static void
velocity_compute(Eext_More_Option_Layout_Velocity_Tracker *velocity)
{
   Eext_More_Option_Layout_Queue *delta_x = velocity->queue_delta_x;
   Eext_More_Option_Layout_Queue *delta_y = velocity->queue_delta_y;
   Eext_More_Option_Layout_Queue *event_time = velocity->queue_event_time;

   int unit = VELOCITY_COMPUTE_UNIT * CLOCKS_PER_SEC;

   int i = event_time->last;
   Eina_Bool max_time_breaked = EINA_FALSE;

   double velocity_x = 0;
   double velocity_y = 0;
   clock_t last_time = event_time->q[i];
   int last_x = delta_x->q[i];
   int last_y = delta_y->q[i];
   int index = 0;

   while (i != event_time->first)
     {
        int dx = delta_x->q[i];
        int dy = delta_y->q[i];
        clock_t time = event_time->q[i];

        if (index > 0)
          {
             velocity_x = velocity_x + (last_x - dx);
             velocity_y = velocity_y + (last_y - dy);
          }

        if (last_time - time > unit)
          {
             max_time_breaked = EINA_TRUE;
             break;
          }

        last_x = dx;
        last_y = dy;
        i = (i == 0) ? (event_time->size - 1) : (i - 1) % event_time->size;
        index += 1;
     }

   // short time gesture !! calculate velocity including last rest data
   if (!max_time_breaked)
     {
        int dx = delta_x->q[i];
        int dy = delta_y->q[i];

        velocity_x = velocity_x + (last_x - dx);
        velocity_y = velocity_y + (last_y - dy);
     }

   velocity->computed_velocity_x = velocity_x;
   velocity->computed_velocity_y = velocity_y;
   velocity->is_computed = EINA_TRUE;
}

static void
velocity_destroy(Eext_More_Option_Layout_Velocity_Tracker *velocity)
{
   if (velocity)
     {
        if (velocity->queue_delta_x) queue_destroy(velocity->queue_delta_x);
        if (velocity->queue_delta_y) queue_destroy(velocity->queue_delta_y);
        if (velocity->queue_event_time) queue_destroy(velocity->queue_event_time);

        free(velocity);
        velocity = NULL;
     }
}

// ----------------------------------------- interpolator util --------------------------------------//
static double
_interpolator_glide_out(double input)
{
   int i = 0;
   static double glide_out_start_x = 0.25f;
   static double glide_out_start_y = 0.46f;
   static double glide_out_end_x = 0.45f;
   static double glide_out_end_y = 1.0f;

   double ax, ay, bx, by, cx, cy, z, x = input;
   for (i = 1; i < 14; i++)
     {
        cx = 3 * glide_out_start_x;
        bx = 3 * (glide_out_end_x - glide_out_start_x) - cx;
        ax = 1 - cx - bx;
        z = x * (cx + x * (bx + x * ax)) - input;
        if (abs(z) < 1e-3)
          {
             break;
          }
        x -= z / (cx + x * (2 * bx + 3 * ax * x));
     }

   cy = 3 * glide_out_start_y;
   by = 3 * (glide_out_end_y - glide_out_start_y) - cy;
   ay = 1 - cy - by;

   return round(x * (cy + x * (by + x * ay)) * 10000) / (double) 10000;
}

// ----------------------------------------- incrementor util --------------------------------------//
static double
_incrementor(double start, double stop, double fraction)
{
   double result;
   result = (stop - start) * fraction + start;

   return result;
}

// ====================================== define, structure ==================================== //

/* more_option_layout data */
#define EEXT_MORE_OPTION_LAYOUT_DATA_KEY "eext_more_option_layout_data"
#define _ITEM_MANAGER_LAYOUT_DATA_KEY "item_manager_layout_data"

#define _MORE_OPTION_LAYOUT_SCREEN_WIDTH 360
#define _MORE_OPTION_LAYOUT_SCREEN_HEIGHT 360
#define _MORE_OPTION_LAYOUT_ITEM_WIDTH 360
#define _MORE_OPTION_LAYOUT_ITEM_HEIGHT 360
#define _MORE_OPTION_LAYOUT_ITEMS_BETWEEN_ANGLE 90
#define _MORE_OPTION_LAYOUT_ITEM_MIN_SCALE        0.38
#define _MORE_OPTION_LAYOUT_ITEM_MAX_SCALE        1
#define _MORE_OPTION_LAYOUT_ITEM_POSITION_ANIMATION_DURATION 0.3
#define _MORE_OPTION_LAYOUT_ITEM_ANIMATOR_DEFAULT_DURATION 1
#define _MORE_OPTION_LAYOUT_ITEM_TOUCH_SLOP 20 //20px
#define _MORE_OPTION_LAYOUT_ITEM_VELOCITY_THRESHOLD 50

// ----------------------------------------- controller API --------------------------------------//
static Eext_More_Option_Layout_Controller *_controller_create(void);

// ----------------------------------------- animator API --------------------------------------//
static Eina_Bool __animator_update_cb(void *data, double pos);
static void _animator_start(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld, Eext_More_Option_Layout_Animator_Info *animator);
static void _animator_cancel(Eext_More_Option_Layout_Animator_Info *animator);
static Eext_More_Option_Layout_Animator_Info *_animator_create(void);
static void _animator_destroy(Eext_More_Option_Layout_Animator_Info *animator);
static Eina_Bool _animator_running_get(Eext_More_Option_Layout_Animator_Info *animator);
static void _position_animator_update_cb(void *data, double animated_value, double fraction);
static void _animator_initialize(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld);

// ----------------------------------------- item API --------------------------------------//
static void _item_text_part_color_set(Evas_Object *layout, int a);
static void _item_text_part_hide(Evas_Object *layout);
static void _item_text_part_show(Evas_Object *layout);
static void _item_coords_update(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld, Eext_More_Option_Layout_Item *item, double position_x);
static void _items_transformation_update(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld);
static void _item_scale_change(Evas_Map *map, Evas_Object *object, double scale_x, double scale_y, double pivot_x, double pivot_y);
static int _item_compare_func(const void *a, const void *b);
static void _items_invalidate(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld);
static void _item_rearrange(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld);
static void _item_select(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld, int index, Eina_Bool animate);
static void _item_scroll_start(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld );
static void _item_scroll(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld, int delta_x, int delta_y);
static void _item_scroll_end_with_fling(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld, double velocity_x, double velocity_y);
static void _item_scroll_end(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld);
static void _item_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static Eext_More_Option_Layout_Item *_item_create(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld);

// ----------------------------------------- item manager API --------------------------------------//
static void _item_manager_scroll_position_set(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld, double position); // _set_scroll_position
static Eext_More_Option_Layout_Item_Manager_Layout_Data *_item_manager_layout_data_init(Evas_Object *obj, Evas_Object *more_option_layout);
static Eext_More_Option_Layout_Item_Manager_Layout_Data *_item_manager_layout_data_get(Evas_Object *item_manager_layout);
static void _item_manager_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _item_manager_show_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _item_manager_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _item_manager_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _item_manager_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _item_manager_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _item_manager_mouse_out_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static Eina_Bool _item_manager_is_moving(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld);


// ----------------------------------------- internal object cb & util --------------------------------------//
static void _drawer_back_cb(void *data, Evas_Object *obj, void *event_info);
static void _panel_scroll_cb(void *data, Evas_Object *obj, void *event_info);
static void _panel_unhold_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _panel_hold_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _panel_active_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _panel_inactive_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _panel_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _panel_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _center_item_changed_cb(void *data, Evas_Object *obj, void *event_info);
static void _index_sync(Eext_More_Option_Layout_Data *mold, int new_index);
static void _index_item_rearrange(Eext_More_Option_Layout_Data *mold, Eext_More_Option_Layout_Item_Manager_Layout_Data *imld);

// ----------------------------------------- internal object creation (elm-demo-tizen/drawer.c) --------------------------------------//
static Evas_Object *_drawer_layout_create(Evas_Object *parent);
static Evas_Object *_panel_create(Evas_Object *parent, Eext_More_Option_Layout_Data *mold);
static Evas_Object *_index_layout_create(Evas_Object *parent);
static Evas_Object *_item_manager_layout_create(Evas_Object *parent, Eext_More_Option_Layout_Data *mold);
static Evas_Object *_index_create(Evas_Object *parent);
static Evas_Object *_bg_create(Evas_Object *parent);

// ----------------------------------------- more option API --------------------------------------//
static Eext_More_Option_Layout_Data *_more_option_layout_data_init(Evas_Object *obj, Evas_Object *parent);
static Eext_More_Option_Layout_Data *_more_option_layout_data_get(const Evas_Object *more_option_layout);
static Eina_Bool _more_option_layout_rotary_cb(void *data, Evas_Object *obj, Eext_Rotary_Event_Info *info);
static void _more_option_layout_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);


// ====================================== implementation ==================================== //

// ----------------------------------------- controller API --------------------------------------//
static Eext_More_Option_Layout_Controller *
_controller_create(void)
{
   Eext_More_Option_Layout_Controller *controller = NULL;
   controller = malloc(sizeof(Eext_More_Option_Layout_Controller));
   if (!controller)
     {
        LOGE("controller is NULL!!");
        return NULL;
     }

   controller->animator = NULL;
   controller->running = EINA_FALSE;
   controller->data = NULL;

   return controller;
}

// ----------------------------------------- animator API --------------------------------------//
static Eina_Bool
__animator_update_cb(void *data, double pos)
{
   Eext_More_Option_Layout_Animator_Info *anim = NULL;
   anim = (Eext_More_Option_Layout_Animator_Info *) data;
   if (!anim)
     {
        LOGE("anim is NULL!!");
        return ECORE_CALLBACK_CANCEL;
     }

   Eext_More_Option_Layout_Controller *controller = NULL;
   controller = (Eext_More_Option_Layout_Controller *) anim->controller;
   if (!controller)
     {
        LOGE("controller is NULL!!");
        return ECORE_CALLBACK_CANCEL;
     }

   double animated_value = 0.0f;

   // Apply Interpolation
   double fraction = (anim->interpolator) ? anim->interpolator(pos) : pos;

   // Calculate increment
   if (anim->incrementor)
     {
        animated_value = anim->incrementor(anim->start_value, anim->stop_value, fraction);
     }

#if _MORE_OPTION_LAYOUT_DEBUG_ENABLED
   LOGI("animation is progressing!!");
#endif
   if (anim->update_cb)
     {
        anim->update_cb(controller->data, animated_value, fraction);
     }
   if (pos == 1) // Callback Animation is stopped
     {
#if _MORE_OPTION_LAYOUT_DEBUG_ENABLED
        LOGI("animation is stopped!! : animator(0x%x) animated_value(%f), fraction(%f)", controller->animator, animated_value, fraction);
#endif
        controller->running = EINA_FALSE;
        controller->animator = NULL;
        controller->data = NULL;

        return ECORE_CALLBACK_CANCEL;
     }

   return ECORE_CALLBACK_RENEW;
}

static void
_animator_start(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld, Eext_More_Option_Layout_Animator_Info *animator)
{
   if (!animator)
     {
        LOGE("animator is NULL!!");
        return;
     }

   Eext_More_Option_Layout_Controller *controller = NULL;
   controller = (Eext_More_Option_Layout_Controller *) animator->controller;
   if (!controller)
     {
        LOGE("controller is NULL!!");
        return;
     }

   controller->data = imld;
   controller->running = EINA_TRUE;

   controller->animator = ecore_animator_timeline_add(animator->duration, __animator_update_cb, animator);
#if _MORE_OPTION_LAYOUT_DEBUG_ENABLED
   LOGI("animator start !! : animator(0x%x), duration(%f)", controller->animator, animator->duration);
#endif
}

static void
_animator_cancel(Eext_More_Option_Layout_Animator_Info *animator)
{
   if (!animator)
     {
        LOGE("animator is NULL!!");
        return;
     }

   Eext_More_Option_Layout_Controller *controller = NULL;
   controller = (Eext_More_Option_Layout_Controller *) animator->controller;
   if (!controller)
     {
        LOGE("controller is NULL!!");
        return;
     }

   controller->running = EINA_FALSE;

   if (controller->animator)
     {
        ecore_animator_del(controller->animator);
        controller->animator = NULL;
     }

   controller->data = NULL;
}

static Eext_More_Option_Layout_Animator_Info *
_animator_create(void)
{
   Eext_More_Option_Layout_Animator_Info *animator = NULL;
   animator = calloc(1, sizeof(Eext_More_Option_Layout_Animator_Info));
   if (!animator)
     {
        LOGE("animator is NULL!!");
        return NULL;
     }

   animator->start_value = 0.0;
   animator->stop_value = 1.0;
   animator->duration = _MORE_OPTION_LAYOUT_ITEM_ANIMATOR_DEFAULT_DURATION;
   animator->update_cb = NULL;
   animator->interpolator = NULL;
   animator->incrementor = _incrementor;
   animator->controller = _controller_create();

   return animator;
}

static void
_animator_destroy(Eext_More_Option_Layout_Animator_Info *animator)
{
   _animator_cancel(animator);
   if (animator)
     {
        free(animator->controller);
        free(animator);
     }
}

static Eina_Bool
_animator_running_get(Eext_More_Option_Layout_Animator_Info *animator)
{
   if (!animator)
     {
        LOGE("animator is NULL!!");
        return EINA_FALSE;
     }

   Eext_More_Option_Layout_Controller *controller = NULL;
   controller = (Eext_More_Option_Layout_Controller *) animator->controller;
   if (!controller)
     {
        LOGE("controller is NULL!!");
        return EINA_FALSE;
     }

   return controller->running;
}

static void
_position_animator_update_cb(void *data, double animated_value, double fraction)
{
   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = (Eext_More_Option_Layout_Item_Manager_Layout_Data *)data;
   if (!imld)
     {
        LOGE("imld is NULL!!");
        return;
     }

   _item_manager_scroll_position_set(imld, round(animated_value));
   _items_transformation_update(imld);
   _items_invalidate(imld);
}

static void
_animator_initialize(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld)
{
   //position_animator
   imld->position_animator = _animator_create();
   imld->position_animator->interpolator = _interpolator_glide_out;
   imld->position_animator->duration = _MORE_OPTION_LAYOUT_ITEM_POSITION_ANIMATION_DURATION;
   imld->position_animator->update_cb = _position_animator_update_cb;
}

// ----------------------------------------- item API --------------------------------------//
static int
_color_multiplier(int color, int alpha)
{
   return color * alpha / 255;
}

static void
_item_text_part_color_set(Evas_Object *layout, int alpha)
{
   Evas_Coord r, g, b;
   Evas_Coord origin_r, origin_g, origin_b;
   Evas_Object *main_text = (Evas_Object *)edje_object_part_object_get(elm_layout_edje_get(layout), "elm.text");
   if (main_text)
     {
        origin_r = 0;
        origin_g = 148;
        origin_b = 255;
        alpha = 255;
        r = _color_multiplier(origin_r, alpha);
        g = _color_multiplier(origin_g, alpha);
        b = _color_multiplier(origin_b, alpha);
        evas_object_color_set(main_text, r, g, b, alpha);
     }

   Evas_Object *sub_text = (Evas_Object *)edje_object_part_object_get(elm_layout_edje_get(layout), "elm.text.sub");
   if (sub_text)
     {
        origin_r = origin_g = origin_b = 249;
        alpha = 255;
        r = _color_multiplier(origin_r, alpha);
        g = _color_multiplier(origin_g, alpha);
        b = _color_multiplier(origin_b, alpha);
        evas_object_color_set(sub_text, r, g, b, alpha);
     }
   //Contact
   Evas_Object *text1 = (Evas_Object *)edje_object_part_object_get(elm_layout_edje_get(layout), "elm.text.1");
   if (text1)
     {
        origin_r = 0;
        origin_g = 148;
        origin_b = 255;
        alpha = 255;
        r = _color_multiplier(origin_r, alpha);
        g = _color_multiplier(origin_g, alpha);
        b = _color_multiplier(origin_b, alpha);
        evas_object_color_set(text1, r, g, b, alpha);
     }

   Evas_Object *text2 = (Evas_Object *)edje_object_part_object_get(elm_layout_edje_get(layout), "elm.text.2");
   if (text2)
     {
        origin_r = origin_g = origin_b = 249;
        alpha = 255;
        r = _color_multiplier(origin_r, alpha);
        g = _color_multiplier(origin_g, alpha);
        b = _color_multiplier(origin_b, alpha);
        evas_object_color_set(text2, r, g, b, alpha);
     }
}

static void
_item_text_part_hide(Evas_Object *layout)
{
   Evas_Object *edje = elm_layout_edje_get(layout);
   if (edje) edje_object_signal_emit(edje, "elm,state,text,hide", "elm");
}

static void
_item_text_part_show(Evas_Object *layout)
{
   Evas_Object *edje = elm_layout_edje_get(layout);
   if (edje) edje_object_signal_emit(edje, "elm,state,text,show", "elm");
}

static void
_item_coords_update(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld, Eext_More_Option_Layout_Item *item, double position_x)
{
   Eext_More_Option_Layout_Item_Coords *coords = &item->coords;

   int max_radius_x = 155; //155; // 135;
   int normal_radius_x = 120; // 155; //120;
   int min_radius_x = 155; // 155; // 100;

   double radius_x = normal_radius_x;
   double radius_z = 500;  // depth is 1000
   double angle = 0;

   if (abs(item->index - imld->selected_index) > (180 / _MORE_OPTION_LAYOUT_ITEMS_BETWEEN_ANGLE))
     {
        position_x = -(_MORE_OPTION_LAYOUT_ITEM_WIDTH * 2);
     }

   angle = ((double) position_x) / _MORE_OPTION_LAYOUT_ITEM_WIDTH * _MORE_OPTION_LAYOUT_ITEMS_BETWEEN_ANGLE;

   // 90 ~ 120
   if (abs(angle) <= 90)
     {
        radius_x = (normal_radius_x - max_radius_x) * (abs(angle) / 90.0) + max_radius_x;
     }
   else
     {
        radius_x = (min_radius_x - normal_radius_x) * ((abs(angle) - 90.0) / 90.0) + normal_radius_x;
     }

   coords->x = sin(_DEGREES_TO_RADIANS(angle)) * radius_x;
   coords->z = cos(_DEGREES_TO_RADIANS(angle)) * radius_z - radius_z;
   coords->depth = ceil(coords->z) + radius_z * 2;
   coords->scale = (_MORE_OPTION_LAYOUT_ITEM_MAX_SCALE - _MORE_OPTION_LAYOUT_ITEM_MIN_SCALE) * (1 - abs(coords->z) / (radius_z * 2)) + _MORE_OPTION_LAYOUT_ITEM_MIN_SCALE;
   coords->angle = angle;
#if _MORE_OPTION_LAYOUT_DEBUG_ENABLED
   LOGI("item(0x%x), x(%f), z(%f), depth(%f), scale(%f), angle(%f)", item, coords->x, coords->z, coords->depth, coords->scale, coords->angle);
#endif
}

static void
_items_transformation_update(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld)
{
   Eina_List *l = NULL;
   Eext_More_Option_Layout_Item *item = NULL;
   int i = 0;
   EINA_LIST_FOREACH(imld->item_list, l, item)
     {
        _item_coords_update(imld, item, (imld->scroll_x + _MORE_OPTION_LAYOUT_ITEM_WIDTH*i));
        i++;
     }
}

static void
_item_scale_change(Evas_Map *map, Evas_Object *object, double scale_x, double scale_y, double pivot_x, double pivot_y)
{
   if (!map)
     {
        LOGE("map is NULL!!");
        return;
     }
   if (!object)
     {
        LOGE("object is NULL!!");
        return;
     }

   evas_map_util_points_populate_from_object(map, object);
   evas_map_util_zoom(map, scale_x, scale_y, pivot_x, pivot_y);
//   evas_map_smooth_set(map, EINA_FALSE);
   evas_object_map_enable_set(object, EINA_TRUE);
   evas_object_map_set(object, map);
}

static int
_item_compare_func(const void *a, const void *b)
{
   Eext_More_Option_Layout_Item **left = (Eext_More_Option_Layout_Item **)a;
   Eext_More_Option_Layout_Item **right = (Eext_More_Option_Layout_Item **)b;

   if ((*left)->coords.z < (*right)->coords.z) return -1;
   if ((*left)->coords.z == (*right)->coords.z) return 0;

   return 1;
}

static void
_items_invalidate(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld)
{
#if _MORE_OPTION_LAYOUT_DEBUG_ENABLED
   double start_time = ecore_time_unix_get();
#endif
   int count = imld->item_count;
   int i = 0;
   Eina_List *l = NULL;
   Eext_More_Option_Layout_Item *item = NULL;
   Eext_More_Option_Layout_Item *sorted_items[count];

   EINA_LIST_FOREACH(imld->item_list, l, item)
     {
        sorted_items[i] = item;
        i++;
     }

   // sort order by z coordinate
   qsort((void *) sorted_items, count, sizeof(sorted_items[0]), _item_compare_func);

   double text_alpha = 0;
   double icon_darken_factor = 0;

   Evas_Coord x, y, w, h;
   evas_object_geometry_get(imld->item_manager, &x, &y, &w, &h);

   Evas_Map *map = NULL;
   map = evas_map_new(4);
   if (!map)
     {
        LOGE("map is NULL!!");
        return;
     }

   // apply transform and layout z-order
   for (i = 0; i < count; i++)
     {
        Eext_More_Option_Layout_Item *item = sorted_items[i];
        Evas_Object *layout = item->base.obj;

        double translation_x = item->coords.x;

        if (item->coords.z > -500 && translation_x > -_MORE_OPTION_LAYOUT_SCREEN_WIDTH * 0.2 && translation_x < _MORE_OPTION_LAYOUT_SCREEN_WIDTH * 0.2)
          {
             text_alpha = (255 - 0) * (1 - abs(translation_x) / (_MORE_OPTION_LAYOUT_SCREEN_WIDTH * 0.2));
          }
        else
          {
             text_alpha = 0;
          }

        if (item->coords.z > -500)
          {
             icon_darken_factor = (1 - 0.3) * (1 - abs(item->coords.z) / 500.0) + 0.3;
          }
        else
          {
             icon_darken_factor = 0.3;
          }

        // update Index
        if (abs(item->coords.angle) < _MORE_OPTION_LAYOUT_ITEMS_BETWEEN_ANGLE / 2 && imld->selected_index != item->index)
          {
             imld->selected_index = item->index;
             evas_object_smart_callback_call(imld->item_manager, "center,item,changed", (void*)imld->selected_index);
          }

        evas_object_raise(layout);
        evas_object_move(layout, x + translation_x, y);

        evas_object_color_set(layout, 255 * icon_darken_factor, 255 * icon_darken_factor, 255 * icon_darken_factor, 255);
        _item_text_part_color_set(layout, text_alpha);

        int focusing_index = -imld->scroll_x / _MORE_OPTION_LAYOUT_ITEM_WIDTH;

             if (item->index < focusing_index - 1 || item->index > focusing_index + 2)
               {
                  evas_object_hide(layout);
                  _item_text_part_hide(layout);
               }
             else
               {
                  evas_object_show(layout);
                  _item_text_part_show(layout);
               }
        _item_scale_change(map, elm_object_part_content_get(layout, "action_btn"), item->coords.scale, item->coords.scale, x+_MORE_OPTION_LAYOUT_ITEM_WIDTH/2, y+_MORE_OPTION_LAYOUT_SCREEN_HEIGHT/2);
     }
   if (map) evas_map_free(map);
#if _MORE_OPTION_LAYOUT_DEBUG_ENABLED
   LOGI("elapsed time(%f)", ecore_time_unix_get() - start_time);
#endif
}

static void
_item_rearrange(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld)
{
   Eina_List *l = NULL;
   int i = 0;
   Eext_More_Option_Layout_Item *item = NULL;

   EINA_LIST_FOREACH(imld->item_list, l, item)
     {
        item->index = i;
        i++;
     }
   imld->item_count = i;

   imld->selected_index = 0;

#if _MORE_OPTION_LAYOUT_DEBUG_ENABLED
   LOGI("item is rearranged!! : item_count(%d)", imld->item_count);
#endif
}

static void
_item_select(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld, int index, Eina_Bool animate)
{
#if _MORE_OPTION_LAYOUT_DEBUG_ENABLED
   LOGI("item is selected!! : index(%d), animate(%d)", index, animate);
#endif
   if (animate)
     {
        if (_animator_running_get(imld->position_animator))
          {
             _animator_cancel(imld->position_animator);
          }
        imld->position_animator->start_value = imld->scroll_x;
        imld->position_animator->stop_value = -index*_MORE_OPTION_LAYOUT_ITEM_WIDTH;
        _animator_start(imld, imld->position_animator);
     }
   else
     {
        if (_animator_running_get(imld->position_animator))
          {
             _animator_cancel(imld->position_animator);
          }

        imld->selected_index = index;
        _item_manager_scroll_position_set(imld, _MORE_OPTION_LAYOUT_ITEM_WIDTH*(-index));
        _items_transformation_update(imld);
        _items_invalidate(imld);
     }
}

static void
_item_scroll_start(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld )
{
   if (_animator_running_get(imld->position_animator))
     {
        _animator_cancel(imld->position_animator);
     }

   imld->item_scrolled = EINA_TRUE;
   imld->previous_index = imld->selected_index;
   imld->previous_scroll_x = imld->scroll_x;
}

static void
_item_scroll(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld, int delta_x, int delta_y)
{
   if (!imld->item_scrolled)
     {
        return;
     }

   double scroll = imld->previous_scroll_x + delta_x;
   double end_pos = -(_MORE_OPTION_LAYOUT_ITEM_WIDTH * (imld->item_count - 1));

   if (scroll > 0)
     {
        scroll = scroll / 3.0;
     }
   else if (scroll < end_pos)
     {
        scroll = end_pos + (scroll - end_pos) / 3.0;
     }

   _item_manager_scroll_position_set(imld, scroll);
   _items_transformation_update(imld);
   _items_invalidate(imld);
}

static void
_item_scroll_end_with_fling(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld, double velocity_x, double velocity_y)
{
   if (!imld->item_scrolled)
     {
        return;
     }
   imld->item_scrolled = EINA_FALSE;

   int previous_scroll_x = -imld->previous_index * _MORE_OPTION_LAYOUT_ITEM_WIDTH;
   int scroll_diff = imld->scroll_x - previous_scroll_x;


   if (scroll_diff == 0)
     {
        return;
     }

   int next = (velocity_x < 0) ? imld->previous_index + 1 : imld->previous_index - 1;
   next = (next < 0) ? 0 : (next >= imld->item_count) ? imld->item_count - 1 : next;

   _item_select(imld, next, EINA_TRUE);
}

static void
_item_scroll_end(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld)
{
   if (!imld->item_scrolled)
     {
        return;
     }

   imld->item_scrolled = EINA_FALSE;

   int previous_scroll_x = -imld->previous_index * _MORE_OPTION_LAYOUT_ITEM_WIDTH;
   int scroll_diff = imld->scroll_x - previous_scroll_x;
   int next = imld->previous_index;

   if (scroll_diff == 0)
     {
        return;
     }

   if (abs(scroll_diff) > _MORE_OPTION_LAYOUT_ITEM_WIDTH / 2)
     {
        if (scroll_diff > 0)
          {
             next = imld->previous_index - 1;
          }
        else
          {
             next = imld->previous_index + 1;
          }
     }

   next = (next < 0) ? 0 : (next >= imld->item_count) ? imld->item_count - 1 : next;
   _item_select(imld, next, EINA_TRUE);
}

static void
_item_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   LOGI("called!!");

   Eext_More_Option_Layout_Item *item = data;

   if (item)
     {
        free(item);
     }
}

static void
_item_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   LOGI("item is clicked");

   Eext_More_Option_Layout_Item *item = data;
   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = NULL;

   imld = _item_manager_layout_data_get(item->item_manager_layout);
   evas_object_smart_callback_call(imld->more_option_layout, "item,selected", (Eext_Object_Item *)item);
}

static Eext_More_Option_Layout_Item *
_item_create(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld)
{
   Eext_More_Option_Layout_Item *item = NULL;
   Evas_Object *layout = NULL;
   Evas_Object *item_manager_layout = imld->item_manager;
   Evas_Coord x = 0, y = 0, w = 0, h = 0;

   layout = elm_layout_add(item_manager_layout);
   if (!layout)
     {
        LOGE("layout is NULL!!");
        return NULL;
     }

   evas_object_size_hint_weight_set(layout,EVAS_HINT_EXPAND,EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_layout_theme_set(layout, "layout", "action_button", "default");
   evas_object_geometry_get(item_manager_layout, &x, &y, &w, &h);
   //elm_widget_sub_object_add(item_manager_layout, layout);
   evas_object_smart_member_add(layout, item_manager_layout);
   evas_object_resize(layout, w, h);
   evas_object_move(layout, x, y);
   evas_object_hide(layout);

   item = (Eext_More_Option_Layout_Item *)calloc(1, sizeof(Eext_More_Option_Layout_Item));
   if (!item)
     {
        LOGE("item is NULL!!");
        return NULL;
     }

   if (item)
     {
        item->base.obj = layout;
        item->index = -1;
        item->item_manager_layout = item_manager_layout;
        item->coords.x = 0.0f;
        item->coords.z = 0.0f;
        item->coords.scale = 1.0f;
        item->coords.depth = 0.0f;
        item->coords.angle = 0.0f;
     }

   evas_object_event_callback_add(layout, EVAS_CALLBACK_DEL, _item_del_cb, item);
   elm_layout_signal_callback_add(layout, "elm,layout,clicked", "elm", _item_clicked_cb, item);

   return item;
}

// ----------------------------------------- item manager API --------------------------------------//
static void
_item_manager_scroll_position_set(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld, double position)
{
#if _MORE_OPTION_LAYOUT_DEBUG_ENABLED
   LOGI("scroll_x(%f)", position);
#endif
   imld->scroll_x = position;
}

static Eext_More_Option_Layout_Item_Manager_Layout_Data *
_item_manager_layout_data_init(Evas_Object *obj, Evas_Object *more_option_layout)
{
   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = NULL;

   imld = (Eext_More_Option_Layout_Item_Manager_Layout_Data *)calloc(1, sizeof(Eext_More_Option_Layout_Item_Manager_Layout_Data));
   if (!imld)
     {
        LOGE("imld is NULL!!");
        return NULL;
     }
   imld->more_option_layout = more_option_layout;
   imld->item_manager = obj;
   imld->item_list = NULL;
   imld->item_count = 0;
   imld->previous_index = -1;
   imld->selected_index = 0;
   imld->previous_scroll_x = 0.0f;
   imld->scroll_x = 0.0f;
   imld->item_scrolled = EINA_FALSE;
   imld->item_manager_scrolled = EINA_FALSE;
   imld->pressed_x = 0;
   imld->pressed_y = 0;
   imld->pressed = EINA_FALSE;
   imld->velocity = velocity_tracker_create();

   _animator_initialize(imld);

   LOGI("imld is initialized!!");

   return imld;
}

static Eext_More_Option_Layout_Item_Manager_Layout_Data *
_item_manager_layout_data_get(Evas_Object *item_manager_layout)
{
   if (!item_manager_layout)
     {
        LOGE("item_manager_layout is NULL!!");
        return NULL;
     }

   return evas_object_data_get(item_manager_layout, _ITEM_MANAGER_LAYOUT_DATA_KEY);
}

static void
_item_manager_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = NULL;
   imld = data;
   if (!imld)
     {
        LOGE("imld is NULL!!");
        return;
     }

   _item_select(imld, imld->selected_index, EINA_FALSE);
}

static void
_item_manager_show_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   LOGI("called!!");

   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = NULL;
   imld = data;
   if (!imld)
     {
        LOGE("imld is NULL!!");
        return;
     }

   _item_select(imld, imld->selected_index, EINA_FALSE);
}

static void
_item_manager_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   LOGI("called!!");

   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = NULL;
   imld = data;
   if (!imld)
     {
        LOGE("imld is NULL!!");
        return;
     }

   if (imld->position_animator)
     {
        _animator_destroy(imld->position_animator);
        imld->position_animator = NULL;
     }

   if (imld->velocity)
     {
        velocity_destroy(imld->velocity);
        imld->velocity = NULL;
     }

   if (imld->item_list)
     {
        imld->item_list = eina_list_free(imld->item_list);
     }
   evas_object_event_callback_del(obj, EVAS_CALLBACK_MOVE, _item_manager_move_cb);
   evas_object_event_callback_del(obj, EVAS_CALLBACK_SHOW, _item_manager_show_cb);

   evas_object_event_callback_del(obj, EVAS_CALLBACK_MOUSE_DOWN, _item_manager_mouse_down_cb);
   evas_object_event_callback_del(obj, EVAS_CALLBACK_MOUSE_MOVE, _item_manager_mouse_move_cb);
   evas_object_event_callback_del(obj, EVAS_CALLBACK_MOUSE_UP, _item_manager_mouse_up_cb);
   evas_object_event_callback_del(obj, EVAS_CALLBACK_MOUSE_OUT, _item_manager_mouse_out_cb);
   evas_object_smart_callback_del(obj, "center,item,changed", _center_item_changed_cb);

   evas_object_data_del(obj, _ITEM_MANAGER_LAYOUT_DATA_KEY);

   free(imld);
}

static void
_item_manager_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = data;
   if (!imld)
     {
        LOGE("imld is NULL!!");
        return;
     }

   Eina_Bool moving = _item_manager_is_moving(imld);

   if (moving)
     {
        LOGI("item manager layout is moving!!");
        return;
     }

   Evas_Event_Mouse_Down *event = event_info;
   imld->pressed_x = event->canvas.x;
   imld->pressed_y = event->canvas.y;

   velocity_tracker_reset(imld->velocity);
   velocity_tracker_add_movement(imld->velocity, imld->pressed_x, imld->pressed_y, clock());

   imld->pressed = EINA_TRUE;
   imld->item_manager_scrolled = EINA_FALSE;
}

static void
_item_manager_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = data;
   if (!imld)
     {
        LOGE("imld is NULL!!");
        return;
     }

   Evas_Coord x = 0, y = 0;
   Eina_Bool moving = _item_manager_is_moving(imld);

   if (moving)
     {
        LOGI("item manager layout is moving!!");
        return;
     }

   if (!imld->pressed)
     {
        return;
     }

   Evas_Event_Mouse_Move *event = event_info;
   x = event->cur.canvas.x;
   y = event->cur.canvas.y;

   // Prevent touch slop
   if (!imld->item_manager_scrolled)
     {
        int distance = sqrt((x - imld->pressed_x) * (x - imld->pressed_x) + (y - imld->pressed_y) * (y - imld->pressed_y));
        if (distance > _MORE_OPTION_LAYOUT_ITEM_TOUCH_SLOP)
          {
             imld->pressed_x = x;
             imld->pressed_y = y;
             imld->item_manager_scrolled = EINA_TRUE;

             _item_scroll_start(imld);
          }

        return;
     }

   // calculate distance from down position
   int delta_x = x - imld->pressed_x;
   int delta_y = y - imld->pressed_y;

   velocity_tracker_add_movement(imld->velocity, x, y, clock());
   _item_scroll(imld, delta_x, delta_y);
}

static void
_item_manager_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = data;
   if (!imld)
     {
        LOGE("imld is NULL!!");
        return;
     }

   Eina_Bool moving = _item_manager_is_moving(imld);

   if (moving)
     {
        LOGI("item manager layout is moving!!");
        imld->pressed = EINA_FALSE;
        imld->item_manager_scrolled = EINA_FALSE;
        return;
     }

   if (!imld->pressed)
     {
        return;
     }

   velocity_compute(imld->velocity);
   double velocity_x = imld->velocity->computed_velocity_x / 2.0;
   double velocity_y = imld->velocity->computed_velocity_y / 2.0;

   if (imld->item_manager_scrolled)
     {
        if (abs(velocity_x) > _MORE_OPTION_LAYOUT_ITEM_VELOCITY_THRESHOLD /* || abs(velocity_y) > _MORE_OPTION_LAYOUT_ITEM_VELOCITY_THRESHOLD */)
          {
             _item_scroll_end_with_fling(imld, velocity_x, velocity_y);
          }
        else
          {
             _item_scroll_end(imld);
          }
     }

   imld->pressed = EINA_FALSE;
   imld->item_manager_scrolled = EINA_FALSE;
}

static void
_item_manager_mouse_out_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = data;

   Eina_Bool moving = _item_manager_is_moving(imld);

   if (moving)
     {
        LOGI("item manager layout is moving!!");
        imld->pressed = EINA_FALSE;
        imld->item_manager_scrolled = EINA_FALSE;
        return;
     }

   imld->pressed = EINA_FALSE;
   imld->item_manager_scrolled = EINA_FALSE;
}

static Eina_Bool
_item_manager_is_moving(Eext_More_Option_Layout_Item_Manager_Layout_Data *imld)
{
   Eext_More_Option_Layout_Data *mold = NULL;
   Evas_Coord x = 0, y = 0, w = 0, h = 0;

   if (!imld)
     {
        LOGE("imld is NULL!!");
        return EINA_FALSE;
     }

   mold = _more_option_layout_data_get(imld->more_option_layout);
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return EINA_FALSE;
     }

   evas_object_geometry_get(imld->item_manager, &x, &y, &w, &h);
   if (mold->direction == EEXT_MORE_OPTION_LAYOUT_DIRECTION_LEFT || mold->direction == EEXT_MORE_OPTION_LAYOUT_DIRECTION_RIGHT)
     {
        if (x != 0)
          {
             return EINA_TRUE;
          }
        else
          {
             return EINA_FALSE;
          }
     }
   else
     {
        if (y != 0)
          {
             return EINA_TRUE;
          }
        else
          {
             return EINA_FALSE;
          }
     }
}

// ----------------------------------------- internal object cb & util --------------------------------------//
static void
_drawer_back_cb(void *data, Evas_Object *obj, void *event_info)
{
   LOGI("called!!");

   Evas_Object *panel = obj;
   if (!panel)
     {
        LOGE("panel is NULL!!");
        return;
     }

   Eext_More_Option_Layout_Data *mold = data;
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return;
     }

   if (elm_object_scroll_freeze_get(panel))
     {
        LOGI("elm_object_scroll_freeze_pop is called");
        elm_object_scroll_freeze_pop(panel);
     }

   if (!elm_panel_hidden_get(panel)) elm_panel_hidden_set(panel, EINA_TRUE);

   //item index is initialized as zero.
   if (mold->item_manager_layout)
     {
        Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = _item_manager_layout_data_get(mold->item_manager_layout);
        if (imld) imld->selected_index = 0;
        mold->selected_index_item = 0;
        _index_sync(mold, mold->selected_index_item);
     }
}

static void
_panel_scroll_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eext_More_Option_Layout_Data *mold = data;
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return;
     }

   int col = 0;

   Elm_Panel_Scroll_Info *ev = event_info;
   if (mold->direction == EEXT_MORE_OPTION_LAYOUT_DIRECTION_LEFT || mold->direction == EEXT_MORE_OPTION_LAYOUT_DIRECTION_RIGHT)
     {
        col = 127 * ev->rel_x;
     }
   else
     {
        col = 127 * ev->rel_y;
     }

   /* Change color for background dim */
   evas_object_color_set(mold->bg, 0, 0, 0, col);
}

static void
_panel_unhold_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   LOGI("called!!");

   Eext_More_Option_Layout_Data *mold = data;
   if (mold)
     {
        evas_object_smart_callback_call(mold->more_option_layout, "more,option,unhold", NULL);
     }
}

static void
_panel_hold_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   LOGI("called!!");

   Eext_More_Option_Layout_Data *mold = data;
   if (mold)
     {
        evas_object_smart_callback_call(mold->more_option_layout, "more,option,hold", NULL);
     }
}

static void
_panel_active_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   LOGI("called!!");

   Eext_More_Option_Layout_Data *mold = data;
   if (mold)
     {
        evas_object_smart_callback_call(mold->more_option_layout, "more,option,opened", NULL);
        LOGI("more_option is opened!!");
        mold->panel_activated = EINA_TRUE;
     }

   eext_rotary_object_event_activated_set(mold->more_option_layout, EINA_TRUE);
   eext_object_event_callback_add(obj, EEXT_CALLBACK_BACK, _drawer_back_cb, mold);
}

static void
_panel_inactive_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   LOGI("called!!");

   Eext_More_Option_Layout_Data *mold = data;
   if (mold)
     {
        evas_object_smart_callback_call(mold->more_option_layout, "more,option,closed", NULL);
        LOGI("more_option is closed!!");
        mold->panel_activated = EINA_FALSE;
     }

   eext_rotary_object_event_activated_set(mold->more_option_layout, EINA_FALSE);
   eext_object_event_callback_del(obj, EEXT_CALLBACK_BACK, _drawer_back_cb);
}

static void
_panel_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   LOGI("called!!");

   evas_object_event_callback_del(obj, EVAS_CALLBACK_MOUSE_MOVE, _panel_mouse_move_cb);

   evas_object_smart_callback_del(obj, "scroll", _panel_scroll_cb);
   /* hold & unhold signal callback */
   elm_object_signal_callback_del(obj, "elm,state,hold", "elm", _panel_hold_cb);
   elm_object_signal_callback_del(obj, "elm,state,unhold", "elm", _panel_unhold_cb);
   /* active & inactive signal callback */
   elm_object_signal_callback_del(obj, "elm,state,active", "elm", _panel_active_cb);
   elm_object_signal_callback_del(obj, "elm,state,inactive", "elm", _panel_inactive_cb);
}

static void
_panel_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eext_More_Option_Layout_Data *mold = data;

   if (mold->panel_activated == EINA_FALSE)
     {
        LOGI("panel is not activated");
        return;
     }

   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = _item_manager_layout_data_get(mold->item_manager_layout);
   if (!imld)
     {
        LOGE("imld is NULL!!");
        return;
     }

   Evas_Event_Mouse_Move *event = event_info;

   Evas_Coord x = event->cur.canvas.x;
   Evas_Coord y = event->cur.canvas.y;

   int distance = sqrt((x - imld->pressed_x) * (x - imld->pressed_x) + (y - imld->pressed_y) * (y - imld->pressed_y));
   if (distance > _MORE_OPTION_LAYOUT_ITEM_TOUCH_SLOP)
     {
        event->event_flags |= EVAS_EVENT_FLAG_ON_HOLD; // to prevent "clicked" event of the button on scrolling.
     }
}

static void
_center_item_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
   LOGI("called!!");

   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = NULL;
   Eext_More_Option_Layout_Data *mold = data;
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return;
     }
   imld = _item_manager_layout_data_get(mold->item_manager_layout);
   if (!imld)
     {
        LOGE("imld is NULL!!");
        return;
     }

   int new_index_item = (int)event_info;

   if (mold->selected_index_item == 0 && new_index_item == 1)
     {
        elm_object_scroll_freeze_push(mold->panel);
        LOGI("elm_object_scroll_freeze_push is called!!");
     }
   else if (mold->selected_index_item == 1 && new_index_item == 0)
     {
        elm_object_scroll_freeze_pop(mold->panel);
        LOGI("elm_object_scroll_freeze_pop is called!!");
     }

   if (new_index_item != mold->selected_index_item)
     {
        LOGI("new_index_item(%d)", new_index_item);
        mold->selected_index_item = new_index_item;
        _index_sync(mold, new_index_item);
        evas_object_smart_callback_call(mold->more_option_layout, "center,item,changed", (void*)new_index_item);
     }
}

static void
_index_sync(Eext_More_Option_Layout_Data *mold, int new_index)
{
   Elm_Object_Item *it = NULL;
   it = elm_index_item_find(mold->index, (void *)new_index);
   if (it)
     {
        elm_index_item_selected_set(it, EINA_TRUE);
     }
}

static void
_index_item_rearrange(Eext_More_Option_Layout_Data *mold, Eext_More_Option_Layout_Item_Manager_Layout_Data *imld)
{
   const char *index_item_even_style[19] = {
        "item/even_2",
        "item/even_3",
        "item/even_4",
        "item/even_5",
        "item/even_6",
        "item/even_7",
        "item/even_8",
        "item/even_9",
        "item/even_10", //center
        "item/even_11", //center
        "item/even_12",
        "item/even_13",
        "item/even_14",
        "item/even_15",
        "item/even_16",
        "item/even_17",
        "item/even_18",
        "item/even_19",
        "item/even_20",
   };
   const char *index_item_odd_style[19] = {
        "item/odd_1",
        "item/odd_2",
        "item/odd_3",
        "item/odd_4",
        "item/odd_5",
        "item/odd_6",
        "item/odd_7",
        "item/odd_8",
        "item/odd_9",
        "item/odd_10", //center
        "item/odd_11",
        "item/odd_12",
        "item/odd_13",
        "item/odd_14",
        "item/odd_15",
        "item/odd_16",
        "item/odd_17",
        "item/odd_18",
        "item/odd_19",
   };

   Elm_Object_Item *index_item = NULL;
   int i = 0;
   int j = 0;
   int item_count = imld->item_count;
   Eina_Bool even_number = EINA_FALSE;
   even_number = (item_count%2) ? EINA_FALSE : EINA_TRUE;
   int even_default_index = 8;
   int odd_default_index = 9;

   //remove index items.
   elm_index_item_clear(mold->index);
   //recreate index items
        if (even_number)
          {
             j = even_default_index - item_count/2 + 1;
          }
        else
          {
             j = odd_default_index - item_count/2;
          }

   for (i = 0; i < item_count; i++)
     {
        index_item = elm_index_item_append(mold->index, NULL, NULL, (void *)i);
        if (!index_item)
          {
             LOGE("index_item is NULL!!");
             return;
          }
        if (even_number)
          {
             elm_object_item_style_set(index_item, index_item_even_style[j]);
          }
        else
          {
             elm_object_item_style_set(index_item, index_item_odd_style[j]);
          }
        j++;
     }

   //always first item show
   if (elm_object_scroll_freeze_get(mold->panel))
     {
        LOGI("elm_object_scroll_freeze_pop is called");
        elm_object_scroll_freeze_pop(mold->panel);
     }
   mold->selected_index_item = 0;
   index_item = elm_index_item_find(mold->index, 0);
   elm_index_level_go(mold->index, 0);
   elm_index_item_selected_set(index_item, EINA_TRUE);
}

// ----------------------------------------- internal object creation (elm-demo-tizen/drawer.c) --------------------------------------//
static Evas_Object *
_drawer_layout_create(Evas_Object *parent)
{
   LOGI("called!!");

   Evas_Object *layout;
   layout = elm_layout_add(parent);
   elm_layout_theme_set(layout, "layout", "drawer", "panel");
   evas_object_show(layout);

   return layout;
}

static Evas_Object *
_panel_create(Evas_Object *parent, Eext_More_Option_Layout_Data *mold)
{
   LOGI("called!!");

   Evas_Object *panel;

   panel = elm_panel_add(parent);
   elm_panel_scrollable_set(panel, EINA_TRUE);
   elm_panel_scrollable_content_size_set(panel, 1.0);

   /* Default is visible, hide the content in default. */
   elm_panel_hidden_set(panel, EINA_TRUE);
   evas_object_show(panel);

   //Default : right
   elm_panel_orient_set(panel, ELM_PANEL_ORIENT_RIGHT);
   elm_object_part_content_set(parent, "elm.swallow.right", panel);

   evas_object_event_callback_add(panel, EVAS_CALLBACK_DEL, _panel_del_cb, NULL);
   evas_object_event_callback_add(panel, EVAS_CALLBACK_MOUSE_MOVE, _panel_mouse_move_cb, mold);

   return panel;
}

static Evas_Object *
_index_layout_create(Evas_Object *parent)
{
   LOGI("called!!");

   Evas_Object *index_layout = elm_layout_add(parent);
   elm_layout_theme_set(index_layout, "layout", "more_option", "index");
   evas_object_size_hint_weight_set(index_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(index_layout);
   elm_object_content_set(parent, index_layout);

   return index_layout;
}

static Evas_Object *
_item_manager_layout_create(Evas_Object *parent, Eext_More_Option_Layout_Data *mold)
{
   LOGI("called!!");

   if (!parent) return NULL;

   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = NULL;

   Evas_Object *item_manager_layout = elm_layout_add(parent);
   elm_layout_theme_set(item_manager_layout, "layout", "application", "default");

   evas_object_move(item_manager_layout, 0, 0);
   evas_object_resize(item_manager_layout, _MORE_OPTION_LAYOUT_SCREEN_WIDTH, _MORE_OPTION_LAYOUT_SCREEN_HEIGHT);

   imld = _item_manager_layout_data_init(item_manager_layout, mold->more_option_layout);
   evas_object_data_set(item_manager_layout, _ITEM_MANAGER_LAYOUT_DATA_KEY, imld);

   evas_object_event_callback_add(item_manager_layout, EVAS_CALLBACK_MOVE, _item_manager_move_cb, imld);
   evas_object_event_callback_add(item_manager_layout, EVAS_CALLBACK_DEL, _item_manager_del_cb, imld);
   evas_object_event_callback_add(item_manager_layout, EVAS_CALLBACK_SHOW, _item_manager_show_cb, imld);

   evas_object_event_callback_add(item_manager_layout, EVAS_CALLBACK_MOUSE_DOWN, _item_manager_mouse_down_cb, imld);
   evas_object_event_callback_add(item_manager_layout, EVAS_CALLBACK_MOUSE_MOVE, _item_manager_mouse_move_cb, imld);
   evas_object_event_callback_add(item_manager_layout, EVAS_CALLBACK_MOUSE_UP, _item_manager_mouse_up_cb, imld);
   evas_object_event_callback_add(item_manager_layout, EVAS_CALLBACK_MOUSE_OUT, _item_manager_mouse_out_cb, imld);

   return item_manager_layout;
}

static Evas_Object *
_index_create(Evas_Object *parent)
{
   LOGI("called!!");

   Evas_Object *index = NULL;
   index = elm_index_add(parent);
   elm_object_style_set(index, "circle");
   evas_object_size_hint_weight_set(index, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(index, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_index_horizontal_set(index, EINA_TRUE);
   elm_index_autohide_disabled_set(index, EINA_TRUE);
   elm_object_part_content_set(parent, "controller", index);

   return index;
}

static Evas_Object *
_bg_create(Evas_Object *parent)
{
   LOGI("called!!");

   Evas_Object *rect = NULL;
   rect = evas_object_rectangle_add(evas_object_evas_get(parent));
   evas_object_color_set(rect, 0, 0, 0, 0);
   evas_object_show(rect);

   return rect;
}

// ----------------------------------------- more option API --------------------------------------//
static Eext_More_Option_Layout_Data *
_more_option_layout_data_init(Evas_Object *obj, Evas_Object *parent)
{
   Eext_More_Option_Layout_Data *mold = NULL;

   mold = (Eext_More_Option_Layout_Data *)calloc(1, sizeof(Eext_More_Option_Layout_Data));
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return NULL;
     }

   mold->parent = parent;
   mold->more_option_layout = obj;
   mold->panel = NULL;
   mold->index_layout = NULL;
   mold->item_manager_layout = NULL;
   mold->index = NULL;
   mold->bg = NULL;
   mold->selected_index_item = -1;
   mold->direction = EEXT_MORE_OPTION_LAYOUT_DIRECTION_RIGHT;
   mold->panel_activated = EINA_FALSE;

   LOGI("mold is initialized!!");

   return mold;
}

static Eext_More_Option_Layout_Data *
_more_option_layout_data_get(const Evas_Object *more_option_layout)
{
   if (!more_option_layout)
     {
        LOGE("more_option_layout is NULL!!");
        return NULL;
     }

   return evas_object_data_get(more_option_layout, EEXT_MORE_OPTION_LAYOUT_DATA_KEY);
}

static Eina_Bool
_more_option_layout_rotary_cb(void *data, Evas_Object *obj, Eext_Rotary_Event_Info *info)
{
   LOGI("info->direction(%d)", info->direction);

   Evas_Object *item_manager_layout = (Evas_Object*)data;
   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = _item_manager_layout_data_get(item_manager_layout);
   if (!imld)
     {
        LOGE("imld is NULL!!");
        return ECORE_CALLBACK_PASS_ON;
     }
   Eext_More_Option_Layout_Data *mold = _more_option_layout_data_get(imld->more_option_layout);
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return ECORE_CALLBACK_PASS_ON;
     }
   if (mold->panel_activated == EINA_FALSE)
     {
        LOGI("panel is not activated");
        return ECORE_CALLBACK_PASS_ON;
     }
   Eina_Bool moving = _item_manager_is_moving(imld);

   if (moving)
     {
        LOGI("more_option is closed!!");
        return ECORE_CALLBACK_PASS_ON;
     }

   if (_animator_running_get(imld->position_animator))
     {
        LOGI("animation is already started!!");
        return ECORE_CALLBACK_PASS_ON;
     }

   if (info->direction == EEXT_ROTARY_DIRECTION_CLOCKWISE)
     {
        if (imld->selected_index < imld->item_count - 1)
          {
             _item_select(imld, imld->selected_index + 1, EINA_TRUE);
          }
     }
   else
     {
        if (imld->selected_index > 0)
          {
             _item_select(imld, imld->selected_index - 1, EINA_TRUE);
          }
     }

   return ECORE_CALLBACK_PASS_ON;
}

static void
_more_option_layout_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   LOGI("called!!");

   Eext_More_Option_Layout_Data *mold = data;

   evas_object_data_del(obj, EEXT_MORE_OPTION_LAYOUT_DATA_KEY);

   eext_rotary_object_event_callback_del(obj, _more_option_layout_rotary_cb);

   free(mold);
}

// ====================================== public API implementation ==================================== //

EAPI Evas_Object *
eext_more_option_layout_add(Evas_Object *parent)
{
   LOGI("called!!");

   // parent should be naviframe obj !!!!!
   if (!parent)
     {
        LOGE("parent is NULL!!");
        return NULL;
     }
   Eext_More_Option_Layout_Data *mold = NULL;
   Evas_Object *more_option_layout = NULL;
   /* Create Layout for panel, Background, and Center View */
   more_option_layout = _drawer_layout_create(parent);

   mold = _more_option_layout_data_init(more_option_layout, parent);

   evas_object_data_set(mold->more_option_layout, EEXT_MORE_OPTION_LAYOUT_DATA_KEY, mold);

   evas_object_event_callback_add(mold->more_option_layout, EVAS_CALLBACK_DEL, _more_option_layout_del_cb, mold);

   /* Panel */
   mold->panel = _panel_create(mold->more_option_layout, mold);

   /* index layout */
   mold->index_layout = _index_layout_create(mold->panel);

   /* item manager layout */
   mold->item_manager_layout = _item_manager_layout_create(mold->index_layout, mold);
   elm_object_part_content_set(mold->index_layout, "scroller", mold->item_manager_layout);

   eext_rotary_object_event_callback_add(more_option_layout, _more_option_layout_rotary_cb, mold->item_manager_layout);

   /* index */
   mold->index = _index_create(mold->index_layout);

   evas_object_smart_callback_add(mold->item_manager_layout, "center,item,changed", _center_item_changed_cb, mold);

   /* Panel Background (Dimmed Area) */
   mold->bg = _bg_create(mold->more_option_layout);
   evas_object_smart_callback_add(mold->panel, "scroll", _panel_scroll_cb, mold);
   elm_object_part_content_set(mold->more_option_layout, "elm.swallow.bg", mold->bg);

   /* hold & unhold signal callback */
   elm_object_signal_callback_add(mold->panel, "elm,state,hold", "elm", _panel_hold_cb, mold);
   elm_object_signal_callback_add(mold->panel, "elm,state,unhold", "elm", _panel_unhold_cb, mold);

   /* active & inactive signal callback */
   elm_object_signal_callback_add(mold->panel, "elm,state,active", "elm", _panel_active_cb, mold);
   elm_object_signal_callback_add(mold->panel, "elm,state,inactive", "elm", _panel_inactive_cb, mold);

   return mold->more_option_layout;
}

EAPI void
eext_more_option_layout_direction_set(Evas_Object *obj, Eext_More_Option_Layout_Direction direction)
{
   LOGI("called!!");

   if (!obj)
     {
        LOGE("obj is NULL!!");
        return;
     }

   Eext_More_Option_Layout_Data *mold = NULL;
   mold = _more_option_layout_data_get(obj);
   if (mold)
     {
        elm_panel_orient_set(mold->panel, direction);
        mold->direction = direction;
     }
}

EAPI Eext_More_Option_Layout_Direction
eext_more_option_layout_direction_get(const Evas_Object *obj)
{
   LOGI("called!!");

   if (!obj)
     {
        LOGE("obj is NULL!!");
        return EEXT_MORE_OPTION_LAYOUT_DIRECTION_RIGHT; // defalut
     }

   Eext_More_Option_Layout_Data *mold = NULL;
   mold = _more_option_layout_data_get((Evas_Object *)obj);
   if (mold)
     {
        Elm_Panel_Orient orient = elm_panel_orient_get(mold->panel);
        return (Eext_More_Option_Layout_Direction)orient;
     }

   return EEXT_MORE_OPTION_LAYOUT_DIRECTION_RIGHT; // defalut
}

EAPI Eext_Object_Item *
eext_more_option_layout_item_append(Evas_Object *obj)
{
   LOGI("called!!");

   Eext_More_Option_Layout_Data *mold = NULL;
   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = NULL;
   Eext_More_Option_Layout_Item *item = NULL;

   mold = _more_option_layout_data_get(obj);
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return NULL;
     }

   imld = _item_manager_layout_data_get(mold->item_manager_layout);
   if (!imld)
     {
        LOGE("imld is NULL!!");
        return NULL;
     }

   item = _item_create(imld);
   if (!item)
     {
        LOGE("item is NULL!!");
        return NULL;
     }

   imld->item_list = eina_list_append(imld->item_list, item);

   _item_rearrange(imld);
   _index_item_rearrange(mold, imld);

   _item_select(imld, imld->selected_index, EINA_FALSE);

#if _MORE_OPTION_LAYOUT_DEBUG_ENABLED
   LOGI("item_list(0x%d), count(%d)", imld->item_list, imld->item_count);
#endif
   return (Eext_Object_Item *)item;
}

EAPI Eext_Object_Item *
eext_more_option_layout_item_prepend(Evas_Object *obj)
{
   LOGI("called!!");

   Eext_More_Option_Layout_Data *mold = NULL;
   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = NULL;
   Eext_More_Option_Layout_Item *item = NULL;

   mold = _more_option_layout_data_get(obj);
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return NULL;
     }

   imld = _item_manager_layout_data_get(mold->item_manager_layout);
   if (!imld)
     {
        LOGE("imld is NULL!!");
        return NULL;
     }

   item = _item_create(imld);
   if (!item)
     {
        LOGE("item is NULL!!");
        return NULL;
     }

   imld->item_list = eina_list_prepend(imld->item_list, item);

   _item_rearrange(imld);
   _index_item_rearrange(mold, imld);

   _item_select(imld, imld->selected_index, EINA_FALSE);

#if _MORE_OPTION_LAYOUT_DEBUG_ENABLED
   LOGI("item_list(0x%d), count(%d)", imld->item_list, imld->item_count);
#endif
   return (Eext_Object_Item *)item;
}

EAPI Eext_Object_Item *
eext_more_option_layout_item_insert_after(Evas_Object *obj, Eext_Object_Item *after)
{
   LOGI("called!!");

   Eext_More_Option_Layout_Data *mold = NULL;
   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = NULL;
   Eext_More_Option_Layout_Item *item = NULL;
   Eext_More_Option_Layout_Item *after_item = (Eext_More_Option_Layout_Item *)after;

   mold = _more_option_layout_data_get(obj);
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return NULL;
     }

   imld = _item_manager_layout_data_get(mold->item_manager_layout);
   if (!imld)
     {
        LOGE("imld is NULL!!");
        return NULL;
     }

   item = _item_create(imld);
   if (!item)
     {
        LOGE("item is NULL!!");
        return NULL;
     }

   imld->item_list = eina_list_append_relative(imld->item_list, item, after_item);

   _item_rearrange(imld);
   _index_item_rearrange(mold, imld);

   _item_select(imld, imld->selected_index, EINA_FALSE);

#if _MORE_OPTION_LAYOUT_DEBUG_ENABLED
   LOGI("item_list(0x%d), count(%d)", imld->item_list, imld->item_count);
#endif
   return (Eext_Object_Item *)item;
}

EAPI Eext_Object_Item *
eext_more_option_layout_item_insert_before(Evas_Object *obj, Eext_Object_Item *before)
{
   LOGI("called!!");

   Eext_More_Option_Layout_Data *mold = NULL;
   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = NULL;
   Eext_More_Option_Layout_Item *item = NULL;
   Eext_More_Option_Layout_Item *before_item = (Eext_More_Option_Layout_Item *)before;

   mold = _more_option_layout_data_get(obj);
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return NULL;
     }

   imld = _item_manager_layout_data_get(mold->item_manager_layout);
   if (!imld)
     {
        LOGE("imld is NULL!!");
        return NULL;
     }

   item = _item_create(imld);
   if (!item)
     {
        LOGE("item is NULL!!");
        return NULL;
     }

   imld->item_list = eina_list_prepend_relative(imld->item_list, item, before_item);

   _item_rearrange(imld);
   _index_item_rearrange(mold, imld);

   _item_select(imld, imld->selected_index, EINA_FALSE);

#if _MORE_OPTION_LAYOUT_DEBUG_ENABLED
   LOGI("item_list(0x%d), count(%d)", imld->item_list, imld->item_count);
#endif
   return (Eext_Object_Item *)item;
}

EAPI void
eext_more_option_layout_item_del(Eext_Object_Item *item)
{
   LOGI("called!!");

   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = NULL;
   Eext_More_Option_Layout_Data *mold = NULL;
   Eext_More_Option_Layout_Item *more_option_item = (Eext_More_Option_Layout_Item *)item;

   if (!more_option_item)
     {
        LOGE("item is NULL!!");
        return;
     }
   imld = _item_manager_layout_data_get(more_option_item->item_manager_layout);
   if (!imld)
     {
        LOGE("imld is NULL!!");
        return;
     }
   mold = _more_option_layout_data_get(imld->more_option_layout);
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return;
     }

   imld->item_list = eina_list_remove(imld->item_list, more_option_item);
   evas_object_del(more_option_item->base.obj);

   _item_rearrange(imld);
   _index_item_rearrange(mold, imld);

   _item_select(imld, imld->selected_index, EINA_FALSE);

   return;
}

EAPI void
eext_more_option_layout_items_clear(Evas_Object *obj)
{
   LOGI("called!!");

   Evas_Object *more_option_layout = obj;
   Eext_More_Option_Layout_Data *mold = NULL;
   Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = NULL;
   Eext_More_Option_Layout_Item *item = NULL;

   if (!more_option_layout)
     {
        LOGE("more_option_layout is NULL!!");
        return;
     }

   mold = _more_option_layout_data_get(more_option_layout);
   if (!mold)
     {
        LOGE("mold is NULL!!");
        return;
     }
   imld = _item_manager_layout_data_get(mold->item_manager_layout);
   if (!imld)
     {
        LOGE("imld is NULL!!");
        return;
     }
   EINA_LIST_FREE(imld->item_list, item)
      evas_object_del(item->base.obj);

   _item_rearrange(imld);
   _index_item_rearrange(mold, imld);

   _item_select(imld, imld->selected_index, EINA_FALSE);

   return;
}

EAPI void
eext_more_option_layout_item_part_text_set(Eext_Object_Item *item, const char *part_name, const char *text)
{
   Eext_More_Option_Layout_Item *more_option_item = (Eext_More_Option_Layout_Item *)item;

   if (!more_option_item)
     {
        LOGE("item is NULL!!");
        return;
     }

   elm_object_part_text_set(more_option_item->base.obj, part_name, text);

   return;
}

EAPI void
eext_more_option_layout_item_domain_translatable_part_text_set(Eext_Object_Item *item, const char *part_name, const char *domain, const char *text)
{
   Eext_More_Option_Layout_Item *more_option_item = (Eext_More_Option_Layout_Item *)item;

   if (!more_option_item)
     {
        LOGE("item is NULL!!");
        return;
     }

   elm_object_domain_translatable_part_text_set(more_option_item->base.obj, part_name, domain, text);

   return;
}

EAPI const char *
eext_more_option_layout_item_part_text_get(const Eext_Object_Item *item, const char *part_name)
{
   Eext_More_Option_Layout_Item *more_option_item = (Eext_More_Option_Layout_Item*)item;

   if (!more_option_item)
     {
        LOGE("item is NULL!!");
        return NULL;
     }

   return elm_object_part_text_get(more_option_item->base.obj, part_name);
}

EAPI void
eext_more_option_layout_item_part_content_set(Eext_Object_Item *item, const char *part_name, Evas_Object *content)
{
   Eext_More_Option_Layout_Item *more_option_item = (Eext_More_Option_Layout_Item *)item;

   if (!more_option_item)
     {
        LOGE("item is NULL!!");
        return;
     }

   elm_object_part_content_set(more_option_item->base.obj, part_name, content);

   return;
}

EAPI Evas_Object *
eext_more_option_layout_item_part_content_get(const Eext_Object_Item *item, const char *part_name)
{
   Eext_More_Option_Layout_Item *more_option_item = (Eext_More_Option_Layout_Item*)item;

   if (!more_option_item)
     {
        LOGE("item is NULL!!");
        return NULL;
     }

   return elm_object_part_content_get(more_option_item->base.obj, part_name);
}

EAPI void
eext_more_option_layout_opened_set(Evas_Object *obj, Eina_Bool opened)
{
   LOGI("called!! : opened(%d)", opened);

   if (!obj)
     {
        LOGE("obj is NULL!!");
        return;
     }

   Eext_More_Option_Layout_Data *mold = NULL;
   mold = _more_option_layout_data_get(obj);
   if (mold)
     {
        if (elm_object_scroll_freeze_get(mold->panel))
          {
             LOGI("elm_object_scroll_freeze_pop is called");
             elm_object_scroll_freeze_pop(mold->panel);
          }
        Eext_More_Option_Layout_Item_Manager_Layout_Data *imld = _item_manager_layout_data_get(mold->item_manager_layout);
        if (!imld)
          {
             LOGE("imld is NULL!!");
             return;
          }
        //item index is initialized as zero.
        if (!opened && mold->item_manager_layout)
          {
             imld->selected_index = 0;
             mold->selected_index_item = 0;
             _index_sync(mold, mold->selected_index_item);
          }

        elm_panel_hidden_set(mold->panel, !opened);
        //for updating contents
        if (opened)
          {
             _item_select(imld, imld->selected_index, EINA_FALSE);
          }
     }
}

EAPI Eina_Bool
eext_more_option_layout_opened_get(Evas_Object *obj)
{
   if (!obj)
     {
        LOGE("obj is NULL!!");
        return EINA_FALSE;
     }

   Eext_More_Option_Layout_Data *mold = NULL;
   mold = _more_option_layout_data_get(obj);
   if (mold)
     {
        return !elm_panel_hidden_get(mold->panel);
     }

   return EINA_FALSE;
}

EAPI void
eext_more_option_layout_scrollable_set(Evas_Object *obj, Eina_Bool scrollable)
{
   LOGI("called!! : scrollable(%d)", scrollable);

   if (!obj)
     {
        LOGE("obj is NULL!!");
        return;
     }

   Eext_More_Option_Layout_Data *mold = NULL;
   mold = _more_option_layout_data_get(obj);
   if (mold)
     {
        if (elm_panel_scrollable_get(mold->panel) == scrollable)
          {
             LOGI("scrollable state is not changed");
             return;
          }
        evas_object_smart_callback_del(mold->panel, "scroll", _panel_scroll_cb);
        /* hold & unhold signal callback */
        elm_object_signal_callback_del(mold->panel, "elm,state,hold", "elm", _panel_hold_cb);
        elm_object_signal_callback_del(mold->panel, "elm,state,unhold", "elm", _panel_unhold_cb);
        /* active & inactive signal callback */
        elm_object_signal_callback_del(mold->panel, "elm,state,active", "elm", _panel_active_cb);
        elm_object_signal_callback_del(mold->panel, "elm,state,inactive", "elm", _panel_inactive_cb);

        elm_panel_scrollable_set(mold->panel, scrollable);

        evas_object_smart_callback_add(mold->panel, "scroll", _panel_scroll_cb, mold);
        /* hold & unhold signal callback */
        elm_object_signal_callback_add(mold->panel, "elm,state,hold", "elm", _panel_hold_cb, NULL);
        elm_object_signal_callback_add(mold->panel, "elm,state,unhold", "elm", _panel_unhold_cb, NULL);
        /* active & inactive signal callback */
        elm_object_signal_callback_add(mold->panel, "elm,state,active", "elm", _panel_active_cb, mold);
        elm_object_signal_callback_add(mold->panel, "elm,state,inactive", "elm", _panel_inactive_cb, mold);

        return;
     }

   return;
}

EAPI Eina_Bool
eext_more_option_layout_scrollable_get(const Evas_Object *obj)
{
   if (!obj)
     {
        LOGE("obj is NULL!!");
        return EINA_FALSE;
     }

   Eext_More_Option_Layout_Data *mold = NULL;
   mold = _more_option_layout_data_get((Evas_Object*)obj);
   if (mold)
     {
        return elm_panel_scrollable_get(mold->panel);
     }

   return EINA_FALSE;
}

