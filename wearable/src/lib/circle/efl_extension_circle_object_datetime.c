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

#define EEXT_CIRCLE_OBJECT_DATETIME_TYPE "Eext_Circle_Object_Datetime"
#define MIN_DAYS_IN_MONTH 28
#define MAX_DAYS_IN_MONTH 31
#define DATETIME_FIELD_NAME_BUF_MAX 8

#define PICKER_MARK_PARTITION_COUNT_YEAR 50
#define PICKER_MARK_ANGLE_MONTH 30
#define PICKER_MARK_ANGLE_HOUR 30
#define PICKER_MARK_ANGLE_MIN 6

#define EEXT_CIRCLE_OBJECT_DATETIME_DATA_GET(circle_obj, data) \
   Eext_Circle_Object_Datetime_Data *data = NULL; \
   if (circle_obj && circle_obj->widget_data && \
       circle_obj->widget_type && !strcmp(circle_obj->widget_type, EEXT_CIRCLE_OBJECT_DATETIME_TYPE)) \
      data = (Eext_Circle_Object_Datetime_Data *)circle_obj->widget_data; \
   if (!data)

#define BG_FILE_COUNT 7

#define DATETIME_ITEM_NAME "default"

static const double BG_WIDTH = 360.0;
static const double BG_HEIGHT = 360.0;
static const double mark_width = 6;
static const double mark_height =23;
static const double radius = 168.5; //BG_HEIGHT / 2 - mark_height / 2;
static const double mark_radius = 2.000317; //1 / sqrt(1 - pow(mark_width / 2 / radius , 2)) * 2;

static char *bg_file_name[] = {
   "date_picker_day_28_bg",
   "date_picker_day_29_bg",
   "date_picker_day_30_bg",
   "date_picker_day_31_bg",
   "date_picker_month_bg",
   "date_picker_year_bg",
   "time_picker_bg",
   NULL
};

static const int picker_mark_angle_year[] =
{0,7,14,21,28,36,43,50,57,64,72,79,86,93,100,108,115,122,129,136,144,151,158,165,172,180,187,194,201,208,216,223,230,237,244,252,259,266,273,280,288,295,302,309,316,324,331,338,345,352};

static const int picker_mark_angle_day[4][31] =
//28 day
{{0,13,26,39,52,65,78,90,103,116,129,142,155,168,180,193,206,219,232,245,258,270,283,296,309,322,335,348},
//29 day
{0,13,25,38,50,63,75,87,100,112,125,137,149,162,174,187,199,211,224,236,249,261,273,286,298,311,323,335,348},
//30 day
{0,12,24,36,48,60,72,84,96,108,120,132,144,156,168,180,192,204,216,228,240,252,264,276,288,300,312,324,336,348},
//31 day
{0,12,24,36,47,59,70,82,93,105,116,128,139,151,163,175,186,198,209,221,232,244,255,267,278,290,302,314,325,337,349}};

static void
_eext_circle_object_datetime_picker_mark_angle_set(Eext_Circle_Object *obj, double angle)
{
   Eext_Circle_Object_Item *item;

   item = _eext_circle_object_item_get(obj, DATETIME_ITEM_NAME);

   _eext_circle_object_item_angle_offset_set(item, angle -(mark_radius / 2));
   _eext_circle_object_item_angle_set(item, mark_radius);

   _eext_circle_object_changed(obj);
}

static int
_max_days_get(int year, int month)
{
   struct tm time1;
   time_t t;
   int day;

   t = time(NULL);
   localtime_r(&t, &time1);
   time1.tm_year = year;
   time1.tm_mon = month;
   for (day = MIN_DAYS_IN_MONTH; day <= MAX_DAYS_IN_MONTH; day++)
     {
        time1.tm_mday = day;
        /* FIXME: To restrict month wrapping because of summer time in some locales,
         * ignore day light saving mode in mktime(). */
        time1.tm_isdst = -1;
        mktime(&time1);
        if (time1.tm_mday == 1) break;
     }

   day--;
   return day;
}

static void
_eext_circle_object_datetime_current_type_update(Eext_Circle_Object *obj)
{
   Eext_Circle_Object_Datetime_Data *data = (Eext_Circle_Object_Datetime_Data *)obj->widget_data;
   Evas_Object *datetime = obj->widget_object;
   Evas_Object *radio;
   struct tm t;
   int max_day;
   double picker_angle = 0.0;

   elm_datetime_value_get(datetime, &t);

   radio = elm_radio_selected_object_get(elm_object_part_content_get(datetime, "field0"));
   data->current_type = elm_radio_state_value_get(radio);
   data->rotary_angle = eext_circle_object_angle_get(datetime);

   switch (data->current_type)
     {
      case ELM_DATETIME_YEAR:
         obj->bg_image_index = 5;
         picker_angle = (double)(picker_mark_angle_year[(int)((t.tm_year + 1900) % PICKER_MARK_PARTITION_COUNT_YEAR)]);
         _eext_circle_object_datetime_picker_mark_angle_set(obj, picker_angle);
         break;
      case ELM_DATETIME_MONTH:
         obj->bg_image_index = 4;
         picker_angle = (double)((t.tm_mon) * PICKER_MARK_ANGLE_MONTH);
         _eext_circle_object_datetime_picker_mark_angle_set(obj, picker_angle);
         break;
      case ELM_DATETIME_DATE:
         max_day = _max_days_get(t.tm_year, t.tm_mon);
         obj->bg_image_index = max_day % 28;
         picker_angle = (double)(picker_mark_angle_day[max_day - MIN_DAYS_IN_MONTH][(t.tm_mday > max_day ? max_day:t.tm_mday) - 1]);
         _eext_circle_object_datetime_picker_mark_angle_set(obj, picker_angle);
         break;
      case ELM_DATETIME_HOUR:
         picker_angle = (double)((t.tm_hour) * PICKER_MARK_ANGLE_HOUR);
         _eext_circle_object_datetime_picker_mark_angle_set(obj, picker_angle);
         obj->bg_image_index = 6;
         break;
      case ELM_DATETIME_MINUTE:
         picker_angle = (double)(t.tm_min * PICKER_MARK_ANGLE_MIN);
         _eext_circle_object_datetime_picker_mark_angle_set(obj, picker_angle);
         obj->bg_image_index = 6;
         break;
      case ELM_DATETIME_AMPM:
      default:
         break;
     }

   _eext_circle_object_changed(obj);

}

static void
_eext_circle_object_datetime_circle_update(Eext_Circle_Object *obj)
{
   Eext_Circle_Object_Datetime_Data *data = (Eext_Circle_Object_Datetime_Data *)obj->widget_data;
   Evas_Object *datetime = obj->widget_object;
   struct tm t;
   int min, max;

   elm_datetime_value_get(datetime, &t);
   elm_datetime_field_limit_get(datetime, data->current_type, &min, &max);

   switch (data->current_type)
     {
        case ELM_DATETIME_YEAR:
           eext_circle_object_value_min_max_set(datetime, (double)min, (double)max);
           eext_circle_object_value_set(datetime, (double)t.tm_year);
           break;
        case ELM_DATETIME_MONTH:
           eext_circle_object_value_min_max_set(datetime, (double)min, (double)max);
           eext_circle_object_value_set(datetime, (double)t.tm_mon);
           break;
        case ELM_DATETIME_DATE:
           min = 1;
           max = _max_days_get(t.tm_year, t.tm_mon);
           eext_circle_object_value_min_max_set(datetime, (double)min, (double)max);
           eext_circle_object_value_set(datetime, (double)t.tm_mday);
           break;
        case ELM_DATETIME_HOUR:
           eext_circle_object_value_min_max_set(datetime, (double)min, (double)max);
           eext_circle_object_value_set(datetime, (double)t.tm_hour);
           break;
        case ELM_DATETIME_MINUTE:
           eext_circle_object_value_min_max_set(datetime, (double)min, (double)max);
           eext_circle_object_value_set(datetime, (double)t.tm_min);
           break;
        case ELM_DATETIME_AMPM:
           eext_circle_object_value_min_max_set(datetime, (double)min, (double)max);
           eext_circle_object_value_set(datetime, (double)t.tm_hour);
        default:
           break;
     }

   data->rotary_angle = eext_circle_object_angle_get(datetime);
}

static void
_eext_circle_object_datetime_radio_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;

   _eext_circle_object_datetime_current_type_update(circle_obj);
   _eext_circle_object_datetime_circle_update(circle_obj);
}

static void
_eext_circle_object_datetime_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;
   Eext_Circle_Object_Datetime_Data *widget_data = (Eext_Circle_Object_Datetime_Data *)circle_obj->widget_data;

   if (!widget_data->circle_changed)
     _eext_circle_object_datetime_circle_update(circle_obj);
}

static void
_date_update(Eext_Circle_Object *obj, Eina_Bool value_up)
{
   Eext_Circle_Object_Datetime_Data *data = (Eext_Circle_Object_Datetime_Data *)obj->widget_data;
   Evas_Object *datetime = obj->widget_object;
   struct tm t;
   int min, max;
   int max_day;
   double picker_angle = 0.0;

   elm_datetime_value_get(datetime, &t);
   elm_datetime_field_limit_get(datetime, data->current_type, &min, &max);

   switch (data->current_type)
     {
        case ELM_DATETIME_YEAR:
           if (value_up) t.tm_year += 1;
           else t.tm_year -= 1;

           if (t.tm_year < min) t.tm_year = max;
           else if (t.tm_year > max) t.tm_year = min;

           picker_angle = (double)(picker_mark_angle_year[(int)((t.tm_year + 1900) % PICKER_MARK_PARTITION_COUNT_YEAR)]);
           _eext_circle_object_datetime_picker_mark_angle_set(obj, picker_angle);
           break;
        case ELM_DATETIME_MONTH:
           if (value_up) t.tm_mon += 1;
           else t.tm_mon -= 1;

           if (t.tm_mon < min) t.tm_mon = max;
           else if (t.tm_mon > max) t.tm_mon = min;

           picker_angle = (double)((t.tm_mon) * PICKER_MARK_ANGLE_MONTH);
           _eext_circle_object_datetime_picker_mark_angle_set(obj, picker_angle);
           break;
        case ELM_DATETIME_DATE:
           if (value_up) t.tm_mday += 1;
           else t.tm_mday -= 1;

           int old_mday = t.tm_mday;

           if (t.tm_mday < min) t.tm_mday = max;
           else if (t.tm_mday > max) t.tm_mday = min;
           else
            {
               elm_datetime_value_set(datetime, &t);
               elm_datetime_value_get(datetime, &t);
               if (t.tm_mday < old_mday)
                 {
                    if (value_up) t.tm_mday = min;
                    else
                      {
                         t.tm_mday = max;
                         elm_datetime_value_set(datetime, &t);
                         elm_datetime_value_get(datetime, &t);
                      }
                 }
            }

           max_day = _max_days_get(t.tm_year, t.tm_mon);

           picker_angle = (double)(picker_mark_angle_day[max_day - MIN_DAYS_IN_MONTH][(t.tm_mday > max_day ? max_day:t.tm_mday) - 1]);
           _eext_circle_object_datetime_picker_mark_angle_set(obj, picker_angle);
           break;
        case ELM_DATETIME_HOUR:
           if (value_up) t.tm_hour += 1;
           else t.tm_hour -= 1;

           if (t.tm_hour < min) t.tm_hour = max;
           else if (t.tm_hour > max) t.tm_hour = min;

           elm_datetime_value_set(datetime, &t);
           elm_datetime_value_get(datetime, &t);

           picker_angle = (double)((t.tm_hour) * PICKER_MARK_ANGLE_HOUR);
           _eext_circle_object_datetime_picker_mark_angle_set(obj, picker_angle);
           break;
        case ELM_DATETIME_MINUTE:
           if (value_up) t.tm_min += 1;
           else t.tm_min -= 1;

           if (t.tm_min < min) t.tm_min = max;
           else if (t.tm_min > max) t.tm_min = min;

           elm_datetime_value_set(datetime, &t);
           elm_datetime_value_get(datetime, &t);
           picker_angle = (double)(t.tm_min * PICKER_MARK_ANGLE_MIN);
           _eext_circle_object_datetime_picker_mark_angle_set(obj, picker_angle);
           break;
        case ELM_DATETIME_AMPM:
           if (value_up && t.tm_hour < 12) t.tm_hour += 12;
           else if (t.tm_hour >= 12) t.tm_hour -= 12;
        default:
           break;
     }

   elm_datetime_value_set(datetime, &t);
}

static Eina_Bool
_rotary_changed_cb(void *data, Evas_Object *obj, Eext_Rotary_Event_Info* info)
{
   Eext_Circle_Object *circle_obj = (Eext_Circle_Object *)data;

   if (info->direction == EEXT_ROTARY_DIRECTION_CLOCKWISE)
     _date_update(circle_obj, EINA_TRUE);
   else
     _date_update(circle_obj, EINA_FALSE);

   return EINA_TRUE;
}

static void
_eext_circle_object_datetime_del_cb(Eext_Circle_Object *obj)
{
   if (!obj) return;

   if (obj->widget_data)
     {
        free(obj->widget_data);
        obj->widget_data = NULL;
     }

   if (obj->bg_image_objs)
     {
        eina_list_free(obj->bg_image_objs);
        obj->bg_image_objs = NULL;
     }
}

static void
_eext_circle_object_datetime_disabled_cb(Eext_Circle_Object *obj)
{
   if (obj->disabled)
     {
        eext_rotary_object_event_callback_del(obj->widget_object, _rotary_changed_cb);
        eext_rotary_object_event_callback_del(obj->image_widget, _rotary_changed_cb);
     }
   else
     {
        eext_rotary_object_event_callback_add(obj->image_widget, _rotary_changed_cb, obj);
        eext_rotary_object_event_callback_add(obj->widget_object, _rotary_changed_cb, obj);
     }
}

static void
_eext_circle_object_datetime_bg_image_append(Eext_Circle_Object *obj)
{
   Evas_Object *bg_image;
   char buf[PATH_MAX];
   int i;

   for (i = 0; i < BG_FILE_COUNT; i++)
     {
        bg_image = evas_object_image_add(evas_object_evas_get(obj->image_widget));

        snprintf(buf, sizeof(buf), "%s/%s.png", IMG_DIR, bg_file_name[i]);
        evas_object_image_file_set(bg_image, buf, NULL);

        if (evas_object_image_load_error_get(bg_image) == EVAS_LOAD_ERROR_NONE)
          obj->bg_image_objs = eina_list_append(obj->bg_image_objs, bg_image);
        else
          evas_object_del(bg_image);
     }
}

static void
_eext_circle_object_datetime_init(Eext_Circle_Object *obj, Evas_Object *datetime)
{
   Eext_Circle_Object_Datetime_Data *data;
   Eext_Circle_Object_Item *item;

   char buf[DATETIME_FIELD_NAME_BUF_MAX];
   int idx;

   obj->widget_object = datetime;
   obj->widget_type = EEXT_CIRCLE_OBJECT_DATETIME_TYPE;
   obj->del_func = _eext_circle_object_datetime_del_cb;
   obj->disabled_func = _eext_circle_object_datetime_disabled_cb;

   data = (Eext_Circle_Object_Datetime_Data *)calloc(1, sizeof(Eext_Circle_Object_Datetime_Data));
   obj->widget_data = (void *)data;

   item = _eext_circle_object_item_new();
   _eext_circle_object_item_color_set(item, 250, 125, 47, 255);
   _eext_circle_object_item_radius_set(item, radius);
   _eext_circle_object_item_line_width_set(item, mark_height);
   _eext_circle_object_item_angle_offset_set(item, item->angle_offset -(mark_radius / 2));
   _eext_circle_object_item_angle_set(item, mark_radius);
   _eext_circle_object_item_append(obj, item);

   elm_object_style_set(datetime, "datepicker/circle");

   evas_object_smart_callback_add(elm_object_part_content_get(datetime, "field0"), "changed",
                                  _eext_circle_object_datetime_radio_changed_cb, obj);
   evas_object_smart_callback_add(elm_object_part_content_get(datetime, "field1"), "changed",
                                  _eext_circle_object_datetime_radio_changed_cb, obj);
   evas_object_smart_callback_add(elm_object_part_content_get(datetime, "field2"), "changed",
                                  _eext_circle_object_datetime_radio_changed_cb, obj);

   for (idx = ELM_DATETIME_HOUR; idx <= ELM_DATETIME_AMPM; idx++)
     {
        snprintf(buf, sizeof(buf), "field%d", idx);

        Evas_Object *field_obj = elm_object_part_content_get(datetime, buf);
        if (field_obj)
          {
             Elm_Datetime_Field_Type field_type = (Elm_Datetime_Field_Type)evas_object_data_get(field_obj, "_field_type");
             if (field_type != ELM_DATETIME_AMPM)
               evas_object_smart_callback_add(field_obj, "changed",
                        _eext_circle_object_datetime_radio_changed_cb, obj);
          }
     }

   evas_object_smart_callback_add(datetime, "changed",
            _eext_circle_object_datetime_changed_cb, obj);

   _eext_circle_object_datetime_current_type_update(obj);
   _eext_circle_object_datetime_circle_update(obj);

   eext_rotary_object_event_callback_add(obj->widget_object, _rotary_changed_cb, obj);

   _eext_circle_object_datetime_bg_image_append(obj);
}

EAPI Evas_Object *
eext_circle_object_datetime_add(Evas_Object *datetime, Eext_Circle_Surface *surface)
{
   Evas_Object *obj;

   if (!datetime) return NULL;

   obj = elm_image_add(datetime);
   _eext_circle_object_init(obj, datetime, surface);

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return NULL;

   _eext_circle_object_datetime_init(circle_obj, datetime);

   return obj;
}

/**
 * FIXME(150420) : After deciding how to support current types to application developer, we will define these APIs' name and parameters.
 *
EAPI Elm_Datetime_Field_Type
eext_circle_object_datetime_selected_field_type_get(const Evas_Object *obj)
{
   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return 0;
   EEXT_CIRCLE_OBJECT_DATETIME_DATA_GET(circle_obj, data) return 0;

   return data->current_type;
}

EAPI void
eext_circle_object_datetime_selected_field_type_set(Evas_Object *obj, Elm_Datetime_Field_Type type)
{
   Evas_Object *radio;

   EEXT_CIRCLE_OBJECT_GET(obj, circle_obj) return;
   EEXT_CIRCLE_OBJECT_DATETIME_DATA_GET(circle_obj, data) return;

   if (data->current_type == type) return;

   radio = elm_radio_selected_object_get(elm_object_part_content_get(obj, "field0"));
   elm_radio_value_set(radio, type);
   data->current_type = type;
   _eext_circle_object_datetime_circle_update(circle_obj);
}
*/
