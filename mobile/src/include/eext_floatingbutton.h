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
 * @defgroup Floatingbutton Floatingbutton
 * @ingroup CAPI_EFL_EXTENSION_MODULE
 *
 * @brief Widget to add floating area for buttons
 *
 * The Floatingbutton widget is not a real button, but only provides areas
 * where application adds (maximum 2) buttons to.
 *
 * This widget inherits from the Layout one, so that all the Layout functions
 * can be used on it. However, it includes not only visible floating area,
 * but invisible track for moving floating area, so there is a possibility that
 * it works differently from that you intended.
 *
 * Default content parts of the floatingbutton widget that you can use for are:
 * @li "button1" - left area for button
 * @li "button2" - right area for button
 *
 * Floatingbutton can be hidden by siganl emission.
 * @li show - elm_object_signal_emit(fb, "elm,state,show", "");
 * @li hide - elm_object_signal_emit(fb, "elm,state,hide", "");
 */

/**
 * addtogroup Floatingbutton
 * @{
 */
#include "eext_floatingbutton_common.h"
#ifdef EFL_EO_API_SUPPORT
#include "eext_floatingbutton_eo.h"
#endif
#ifndef EFL_NOLEGACY_API_SUPPORT
#include "eext_floatingbutton_legacy.h"
#endif
/**
 * @}
 */

