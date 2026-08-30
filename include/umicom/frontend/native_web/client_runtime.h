/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/client_runtime.h
 *
 * PURPOSE:
 *   Emit the small Framework-controlled JavaScript runtime that applies DOM patches and forwards UI events.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_CLIENT_RUNTIME_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_CLIENT_RUNTIME_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/markup_buffer.h"
/* Emit the small browser runtime used for patch application and semantic event forwarding. */
UmiStatus umi_native_web_client_runtime_emit(const char *event_endpoint, UmiNativeWebMarkupBuffer *out_javascript);
/* Report whether a generated runtime contains any WebAssembly bootstrap marker. */
bool umi_native_web_client_runtime_uses_webassembly(const UmiNativeWebMarkupBuffer *runtime);

#ifdef __cplusplus
}
#endif
#endif
