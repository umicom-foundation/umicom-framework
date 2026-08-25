/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/focus_binding.h
 *
 * PURPOSE:
 *   Track semantic focus transitions independently of browser element implementation details.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_FOCUS_BINDING_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_FOCUS_BINDING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/types.h"
typedef struct UmiNativeWebFocusBinding { char element_id[UMI_NATIVE_WEB_ID_CAPACITY]; bool focusable; int32_t tab_index; } UmiNativeWebFocusBinding;
/* Initialise validated focus binding state. */
UmiStatus umi_native_web_focus_binding_init(UmiNativeWebFocusBinding *binding, const char *element_id, bool focusable, int32_t tab_index);

#ifdef __cplusplus
}
#endif
#endif
