/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/command_binding.h
 *
 * PURPOSE:
 *   Bind browser command events to stable Framework command IDs and explicit allow policies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_COMMAND_BINDING_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_COMMAND_BINDING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/types.h"
typedef struct UmiNativeWebCommandBinding { char element_id[UMI_NATIVE_WEB_ID_CAPACITY]; char command_id[UMI_NATIVE_WEB_ID_CAPACITY]; bool enabled; } UmiNativeWebCommandBinding;
/* Initialise validated command binding state. */
UmiStatus umi_native_web_command_binding_init(UmiNativeWebCommandBinding *binding, const char *element_id, const char *command_id, bool enabled);

#ifdef __cplusplus
}
#endif
#endif
