/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/focus_binding.c
 *
 * PURPOSE:
 *   Track semantic focus transitions independently of browser element implementation details.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/focus_binding.h"

#include <string.h>
UmiStatus umi_native_web_focus_binding_init(UmiNativeWebFocusBinding *binding, const char *element_id, bool focusable, int32_t tab_index){UmiStatus s;if(binding==NULL||element_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(binding,0,sizeof(*binding));s=umi_native_web_copy_text(binding->element_id,sizeof(binding->element_id),element_id);if(s!=UMI_STATUS_OK)return s;binding->focusable=focusable;binding->tab_index=tab_index;return UMI_STATUS_OK;}

