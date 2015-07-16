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

/**
 * @addtogroup Floatingbutton
 *
 * @{
 */

/**
 * Positions where floatingbutton can be placed on
 */
typedef enum {
   EEXT_FLOATINGBUTTON_LEFT_OUT = 0, /**< hides in the left, but small hanlder will show only */
   EEXT_FLOATINGBUTTON_LEFT, /**< shows all of buttons, but lies on the left */
   EEXT_FLOATINGBUTTON_CENTER, /**< shows all of buttons, but lies on the center */
   EEXT_FLOATINGBUTTON_RIGHT, /**< shows all of buttons, but lies on the right */
   EEXT_FLOATINGBUTTON_RIGHT_OUT, /**< hides in the right, but small handler will show only */
   EEXT_FLOATINGBUTTON_LAST /** never used **/
} Eext_Floatingbutton_Pos;

/**
 * @}
 */
