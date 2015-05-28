#include <Elementary.h>
#include "circle/efl_extension_rotary.h"
#include "efl_extension_private.h"
#include <Ecore_X.h>
#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/XI2.h>


#define TIZEN_DEV "tizen_detent"

static const char SIG_ACTIVATED[] = "rotary,activated";
static const char SIG_DEACTIVATED[] = "rotary,deactivated";

static int EEXT_EVENT_ROTARY_ROTATE = 0;
static Eina_List *_callbacks = NULL;
static Eina_List *_obj_pri_callbacks = NULL;
static XIEventMask *_xi_event_mask = NULL;
static Evas_Object *_activated_obj = NULL;
static int _init_count = 0;
static Ecore_Event_Handler *_motion_handler = NULL;
static Ecore_Event_Handler *_rotate_handler = NULL;
static Eina_Bool _handler_registered = EINA_FALSE;
static Eina_List *handlers = NULL;

typedef struct _Eext_Rotary_Cb Eext_Rotary_Cb;
struct _Eext_Rotary_Cb
{
   Evas_Object            *obj;
   Eext_Rotary_Event_Cb    func;
   void                   *data;
   Eext_Callback_Priority  pri;
   Eina_Bool               del_registered;
};

typedef struct _Rotary_Raw_Info Rotary_Raw_Info;
struct _Rotary_Raw_Info
{
   int                         devid;
   Eext_Rotary_Event_Direction direction;
   unsigned int                time_stamp;
};

typedef struct _Eext_Rotary_Handler Eext_Rotary_Handler;
struct _Eext_Rotary_Handler
{
   Eext_Rotary_Handler_Cb  func;
   void                   *data;
};

typedef struct _Eext_Rotary_Obj_Pri Eext_Rotary_Obj_Pri;
struct _Eext_Rotary_Obj_Pri
{
   Evas_Object *obj;
   Eina_List   *pri_callbacks;
};

static Eina_Bool _init_Xi2_system(void);
static Eina_Bool _motion_changed_cb(void *data, int type, void *event_info);
static Eina_Bool _rotary_rotate_event_cb(void *data, int type, void *event_info);
static void _process_raw_event(void *data, int type, void *raw_event);
static void _object_deleted_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _remove_ecore_handlers(void);
static Eina_Bool _rotary_rotate_handler_cb(void *data, Eext_Rotary_Event_Info *info);
static int _pri_compare(const void *data1, const void *data2);
static void _activated_obj_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

EAPI Eina_Bool
eext_rotary_event_handler_add(Eext_Rotary_Handler_Cb func, void *data)
{
   Eext_Rotary_Handler *new_hd = NULL;
   if (!func) return EINA_FALSE;

   /* init event registration */
   if (!_init_count)
     {
        LOGE("init_count: %d", _init_count);
        if (!_init_Xi2_system())
          {
             LOGE("Failed to init Xi2 system");
             return EINA_FALSE;
          }
     }

   /* register event handlers */
   if (!_motion_handler)
     {
        _motion_handler = ecore_event_handler_add(ECORE_X_EVENT_GENERIC,
                                                  _motion_changed_cb, NULL);
        if (!_motion_handler)
          {
             LOGE("Cannot register generic ecore event handler");
             return EINA_FALSE;
          }
     }
   if (!_rotate_handler)
     {
        _rotate_handler = ecore_event_handler_add(EEXT_EVENT_ROTARY_ROTATE,
                                                  _rotary_rotate_event_cb, NULL);
        if (!_rotate_handler)
          {
             LOGE("Cannot register eext rotate event handler");
             return EINA_FALSE;
          }
     }

   new_hd = malloc(sizeof(Eext_Rotary_Handler));
   if (!new_hd)
     {
        LOGE("Cannot allocate memory");
        return EINA_FALSE;
     }
   new_hd->func = func;
   new_hd->data = data;
   handlers = eina_list_append(handlers, new_hd);

   return EINA_TRUE;
}

EAPI void *
eext_rotary_event_handler_del(Eext_Rotary_Handler_Cb func)
{
   Eina_List *l = NULL;
   Eext_Rotary_Handler *hd = NULL;
   void *data = NULL;

   EINA_LIST_FOREACH(handlers, l, hd)
     {
        if (hd->func == func)
          {
             data = hd->data;
             handlers = eina_list_remove(handlers, hd);
             free(hd);
             if (!handlers)
               {
                  _remove_ecore_handlers();
               }
             break;
          }
     }
   return data;
}

EAPI Eina_Bool
eext_rotary_object_event_callback_add(Evas_Object *obj, Eext_Rotary_Event_Cb func, void *data)
{
   LOGI("In");
   return eext_rotary_object_event_callback_priority_add(obj, 0, func, data);
}

EAPI Eina_Bool
eext_rotary_object_event_callback_priority_add(Evas_Object *obj, Eext_Callback_Priority priority, Eext_Rotary_Event_Cb func, void *data)
{
   Eina_List *l;
   Eext_Rotary_Obj_Pri *op = NULL;
   Eext_Rotary_Cb *cb = NULL, *new_cb = NULL;
   Eina_Bool registered = EINA_FALSE;
   Eina_Bool added = EINA_FALSE;

   if ((!obj) || (!func)) return EINA_FALSE;

   if (!_handler_registered)
     {
        if (!eext_rotary_event_handler_add(_rotary_rotate_handler_cb, NULL))
          {
             LOGE("Cannot register rotary handler");
             return EINA_FALSE;
          }
       _handler_registered = EINA_TRUE;
   }

   new_cb = malloc(sizeof(Eext_Rotary_Cb));
   if (!new_cb)
     {
        LOGE("Cannot allocate memory");
        return EINA_FALSE;
     }
   new_cb->obj = obj;
   new_cb->func = func;
   new_cb->data = data;
   new_cb->pri = priority;
   new_cb->del_registered = EINA_FALSE;

   EINA_LIST_FOREACH(_obj_pri_callbacks, l, op)
     {
        if (op->obj == obj)
          {
             added = EINA_TRUE;
             op->pri_callbacks = eina_list_sorted_insert(op->pri_callbacks,
                                                         _pri_compare, new_cb);
             break;
          }
     }
   if (!added)
     {
        Eext_Rotary_Obj_Pri *new_op = NULL;

        new_op = calloc(1, sizeof(Eext_Rotary_Obj_Pri));
        if (!new_op)
          {
             LOGE("Cannot allocate memory");
             free(new_cb);
             return EINA_FALSE;
          }
        new_op->obj = obj;
        new_op->pri_callbacks = eina_list_append(new_op->pri_callbacks, new_cb);
        _obj_pri_callbacks = eina_list_append(_obj_pri_callbacks, new_op);
     }
   _callbacks = eina_list_append(_callbacks, new_cb);

   EINA_LIST_FOREACH(_callbacks, l, cb)
     {
        if (cb->obj == obj)
          {
             if (cb->del_registered)
               {
                  registered = EINA_TRUE;
                  break;
               }
          }
     }
   if (!registered)
     {
        evas_object_event_callback_add(obj, EVAS_CALLBACK_DEL,
                                       _object_deleted_cb, obj);
        new_cb->del_registered = EINA_TRUE;
     }

   return EINA_TRUE;
}

EAPI void *
eext_rotary_object_event_callback_del(Evas_Object *obj, Eext_Rotary_Event_Cb func)
{
   LOGI("In");
   Eina_List *l = NULL;
   Eext_Rotary_Cb *cb = NULL;
   void *data = NULL;

   LOGE("callback del 0x%x, %s, func : 0x%x", obj, evas_object_type_get(obj), func);

   EINA_LIST_FOREACH(_callbacks, l, cb)
     {
        if ((cb->func == func) && (cb->obj == obj))
          {
             data = cb->data;
             _callbacks = eina_list_remove(_callbacks, cb);
             LOGI("Removed cb from callbacks");
             break;
          }
        cb = NULL;
     }
   if (cb)
     {
        Eext_Rotary_Obj_Pri *op = NULL;

        EINA_LIST_FOREACH(_obj_pri_callbacks, l, op)
          {
             if (op->obj == obj)
               {
                  op->pri_callbacks = eina_list_remove(op->pri_callbacks, cb);
                  break;
               }
          }
        free(cb);
        LOGI("Freed cb");
     }
   if (!_callbacks)
     {
        eext_rotary_event_handler_del(_rotary_rotate_handler_cb);
     }
   LOGI("done");

   return data;
}

EAPI void
eext_rotary_object_event_activated_set(Evas_Object *obj, Eina_Bool activated)
{
   if (!obj)
     {
        return;
     }

   LOGE("eext_rotary_object_event_activated_set : 0x%x, %s, _activated_obj : 0x%x, activated : %d", obj, evas_object_type_get(obj), _activated_obj, activated);

   if (activated)
     {
        if (_activated_obj != obj)
          {
             if (_activated_obj != NULL)
               {
                  LOGE("Activation delete!!!!");
                  evas_object_event_callback_del(_activated_obj, EVAS_CALLBACK_DEL, _activated_obj_del_cb);
                  evas_object_smart_callback_call(_activated_obj, SIG_DEACTIVATED, NULL);
               }

             _activated_obj = obj;
             evas_object_event_callback_add(_activated_obj, EVAS_CALLBACK_DEL, _activated_obj_del_cb, NULL);
             evas_object_smart_callback_call(_activated_obj, SIG_ACTIVATED, NULL);
          }
     }
   else
     {
        if (obj == _activated_obj && _activated_obj != NULL)
          {
             evas_object_event_callback_del(_activated_obj, EVAS_CALLBACK_DEL, _activated_obj_del_cb);
             evas_object_smart_callback_call(_activated_obj, SIG_DEACTIVATED, NULL);
             _activated_obj = NULL;
          }
     }
}

static Eina_Bool
_init_Xi2_system(void)
{
   LOGI("In");
   Ecore_X_Display *display = NULL;
   Ecore_X_Window root_win;
   int opcode = 0, event = 0, error = 0;
   int major = XI_2_Major;
   int minor = XI_2_Minor;
   int dev_count = 0;
   int i = 0;

   if (_init_count > 0) return EINA_TRUE;
   _init_count++;

   EEXT_EVENT_ROTARY_ROTATE = ecore_event_type_new();
   _xi_event_mask = (XIEventMask *)calloc(1, sizeof(XIEventMask));
   display = ecore_x_display_get();
   root_win = ecore_x_window_root_first_get();

   /* Initialize Xi2 system */
   /* Check if X input extension available */
   if (!XQueryExtension((Display *)display, "XInputExtension", &opcode,
                        &event, &error))
     {
        LOGE("Failed to query XInputExtension");
        return EINA_FALSE;
     }

   /* We support version 2.0 */
   if (XIQueryVersion((Display *)display, &major, &minor) == BadRequest)
     {
        LOGE("Failed to query XI Version");
        return EINA_FALSE;
     }

   _xi_event_mask->deviceid = XIAllDevices;

   /* Check device id */
   XIDeviceInfo *dev_info = NULL;
   dev_info = XIQueryDevice(display, XIAllDevices, &dev_count);

   for (i = 0; i < dev_count; i++)
     {
        XIDeviceInfo *di = &(dev_info[i]);
        if (!strncmp(di->name, TIZEN_DEV, sizeof(TIZEN_DEV)))
          {
             _xi_event_mask->deviceid = di->deviceid;
             break;
          }
     }

   /* SelectXi2Event */
   _xi_event_mask->mask = (unsigned char *)(calloc(1, XIMaskLen(XI_LASTEVENT)));
   XISetMask(_xi_event_mask->mask, XI_RawMotion);
   _xi_event_mask->mask_len = sizeof(_xi_event_mask->mask);
   int ret = XISelectEvents((Display *)display, (Window)root_win, _xi_event_mask, 1);
   if (ret != 0)
     {
        LOGE("Failed to Select Events");
        return EINA_FALSE;
     }

   LOGI("Done");
   return EINA_TRUE;
}

static Eina_Bool
_motion_changed_cb(void *data, int type, void *event_info)
{
   Ecore_X_Event_Generic *ev = event_info;

   switch((int)(ev->evtype))
     {
      case XI_RawMotion:
         _process_raw_event(data, type, (XIRawEvent *)ev->data);
         break;
     }

   return ECORE_CALLBACK_PASS_ON;
}

static void
_rotary_rotate_event_free(void *udata EINA_UNUSED, void *fdata)
{
   LOGI("In: ddata: %p", fdata);
   free(fdata);
}

static void
_process_raw_event(void *data EINA_UNUSED, int type EINA_UNUSED, void *raw_event)
{
   XIRawEvent *xi_raw_event = raw_event;
   double *value = xi_raw_event->raw_values;
   Rotary_Raw_Info *event = NULL;

   if (_xi_event_mask->deviceid != xi_raw_event->deviceid)
     {
        return;
     }

   LOGI("In");
   event = calloc(1, sizeof(Rotary_Raw_Info));
   if (!event)
     {
        LOGE("Cannot allocate memory");
        return;
     }
   event->devid = xi_raw_event->deviceid;
   /** X(0): rotate: NOT USED;
     Y(1): timestamp;
     Z(2): direction (-1: counter clockwise, 1: clockwise, 0: None)
    **/
   if (XIMaskIsSet(xi_raw_event->valuators.mask, 1))
     {
        event->time_stamp = (unsigned int)(*(value + 1));
        LOGI("time stamp: %d", event->time_stamp);
     }
   if (XIMaskIsSet(xi_raw_event->valuators.mask, 2))
     {
        int dir = (int)(*(value + 2));
        LOGI("raw dir: %d", dir);
        switch (dir)
          {
           case -1:
              event->direction =  EEXT_ROTARY_DIRECTION_COUNTER_CLOCKWISE;
              LOGI("direction: Counter Clockwise");
              ecore_event_add(EEXT_EVENT_ROTARY_ROTATE, event,
                              _rotary_rotate_event_free, NULL);
              break;
           case 1:
              event->direction = EEXT_ROTARY_DIRECTION_CLOCKWISE;
              LOGI("direction: Clockwise");
              ecore_event_add(EEXT_EVENT_ROTARY_ROTATE, event,
                              _rotary_rotate_event_free, NULL);
              break;
           case 0:
              LOGI("Not detent");
              free(event);
              break;
           default:
              LOGI("Should not get in here");
              free(event);
          }
     }
   else
     {
        free(event);
     }
}

static Eina_Bool
_rotary_rotate_event_cb(void *data EINA_UNUSED, int type EINA_UNUSED, void *event_info)
{
   LOGI("In");
   Eina_List *l;
   Eext_Rotary_Handler *hd;
   Rotary_Raw_Info *ev = event_info;
   Eext_Rotary_Event_Info info;
   info.direction = ev->direction;
   info.time_stamp = ev->time_stamp;

   EINA_LIST_FOREACH(handlers, l, hd)
     {
        if (!hd->func(hd->data, &info))
          {
             return ECORE_CALLBACK_DONE;
          }
     }

   return ECORE_CALLBACK_PASS_ON;
}

static int
_pri_compare(const void *data1, const void *data2)
{
   const Eext_Rotary_Cb *cb1 = data1;
   const Eext_Rotary_Cb *cb2 = data2;
   if (cb1 && cb2)
     {
        if (cb1->pri < cb2->pri)
          return -1;
        else if (cb1->pri > cb2->pri)
          return 1;
        else
          return 0;
     }

   return 0;
}

static Eina_Bool
_rotary_rotate_handler_cb(void *data EINA_UNUSED, Eext_Rotary_Event_Info *info)
{
   LOGE("_rotary_rotate_handler_cb : 0x%x, %s, %d", _activated_obj, evas_object_type_get(_activated_obj), evas_object_visible_get(_activated_obj));
   Eina_List *l;
   Eext_Rotary_Cb *cb;

   /* Deliver to the activated object and its parents */
   if ((_activated_obj) &&
       (evas_focus_state_get(evas_object_evas_get(_activated_obj))) &&
       (evas_object_visible_get(_activated_obj)))
     {
        Evas_Object *obj = NULL;
        Eext_Rotary_Obj_Pri *op = NULL;

        obj = _activated_obj;
        while(obj)
          {
             LOGE("Deliver rotary event to object: %p, %s", obj,
                  evas_object_type_get(obj));
             EINA_LIST_FOREACH(_obj_pri_callbacks, l, op)
               {
                  if (op->obj == obj)
                    {
                       EINA_LIST_FOREACH(op->pri_callbacks, l, cb)
                         {
                            if (!cb->func(cb->data, obj, info))
                              {
                                 return EINA_FALSE;
                              }
                         }
                       break;
                    }
               }
             obj = elm_object_parent_widget_get(obj);
          }
     }

   return EINA_TRUE;
}

static void
_remove_ecore_handlers(void)
{
   LOGI("In");
   if (_motion_handler)
     {
        ecore_event_handler_del(_motion_handler);
        _motion_handler = NULL;
        LOGI("removed _motion_handler");
     }
   if (_rotate_handler)
     {
        ecore_event_handler_del(_rotate_handler);
        _rotate_handler = NULL;
        LOGI("removed _rotate_handler");
     }
}

static void
_object_deleted_cb(void *data EINA_UNUSED, Evas *e EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   LOGE("In: data: %p, obj: %p", data, obj);
   Eina_List *l = NULL;

   Eext_Rotary_Obj_Pri *op = NULL;
   EINA_LIST_FOREACH(_obj_pri_callbacks, l, op)
     {
        if (op->obj == obj)
          {
             Eina_List *l1 = NULL;
             Eext_Rotary_Cb *cb = NULL;

             EINA_LIST_FOREACH(op->pri_callbacks, l1, cb)
               {
                  _callbacks = eina_list_remove(_callbacks, cb);
                  op->pri_callbacks = eina_list_remove(op->pri_callbacks, cb);
                  free(cb);
               }
             _obj_pri_callbacks = eina_list_remove(_obj_pri_callbacks, op);
             free(op);
             break;
          }
     }
   if (!_callbacks)
     {
        eext_rotary_event_handler_del(_rotary_rotate_handler_cb);
        _handler_registered = EINA_FALSE;
     }
   if (_activated_obj == obj)
     _activated_obj = NULL;
   LOGI("done");
}

static void
_activated_obj_del_cb(void *data EINA_UNUSED, Evas *e EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   LOGE("_activated_obj_del_cb : 0x%x", _activated_obj);

   evas_object_event_callback_del(_activated_obj, EVAS_CALLBACK_DEL, _activated_obj_del_cb);
   _activated_obj = NULL;
}
