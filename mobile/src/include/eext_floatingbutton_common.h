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

/**
 * @addtogroup Floatingbutton
 *
 * @{
 */

/**
 * Positions where floatingbutton can be placed on
 */
typedef enum {
   EEXT_FLOATINGBUTTON_LEFT_OUT = 0, /**< hides in the left, but small handler will show only */
   EEXT_FLOATINGBUTTON_LEFT, /**< shows all of buttons, but lies on the left */
   EEXT_FLOATINGBUTTON_CENTER, /**< shows all of buttons, but lies on the center */
   EEXT_FLOATINGBUTTON_RIGHT, /**< shows all of buttons, but lies on the right */
   EEXT_FLOATINGBUTTON_RIGHT_OUT, /**< hides in the right, but small handler will show only */
   EEXT_FLOATINGBUTTON_LAST /**< indicates the last, do not use this **/
} Eext_Floatingbutton_Pos;

typedef enum {
   EEXT_FLOATINGBUTTON_MODE_DEFAULT = 0, /**< allows all positions */
   EEXT_FLOATINGBUTTON_MODE_BOTH_SIDES, /**< allows LEFT and RIGHT positions only */
   EEXT_FLOATINGBUTTON_MODE_LAST /**< indicates the last, do not use this **/
} Eext_Floatingbutton_Mode;

/**
 * @}
 */
