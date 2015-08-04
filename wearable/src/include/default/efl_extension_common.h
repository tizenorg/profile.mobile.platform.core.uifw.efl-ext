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

#ifndef __EFL_EXTENSION_COMMON_H__
#define __EFL_EXTENSION_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup CAPI_EFL_EXTENSION_COMMON_UI_MODULE Efl Extension Common UI
 * @ingroup CAPI_EFL_EXTENSION_MODULE
 * @brief This module provides common ui-controls which are compatible for various devices.
 * @{
 */

/**
 * @WEARABLE_ONLY
 * @typedef Eext_Object_Item
 * @brief An efl-extension object item handle.

 * @if WEARABLE @since_tizen 2.3.1
 * @endif
 */
typedef struct _Eext_Object_Item Eext_Object_Item;

#define EEXT_OBJECT_ITEM       Eext_Object_Item base

struct _Eext_Object_Item {
   Evas_Object *obj;
   void *data;
};

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __EFL_EXTENSION_COMMON_H__ */
