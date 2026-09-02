/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/runtime.h
 *
 * PURPOSE:
 *   Aggregate renderers, browser capabilities, sessions, event queues and patch state for one native-web frontend runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_RUNTIME_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_RUNTIME_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/renderer_registry.h"
#include "umicom/frontend/native_web/browser_capability.h"
#include "umicom/frontend/native_web/event_queue.h"
#include "umicom/frontend/native_web/patch_batch.h"
/**
 * Represent the native web runtime data shared with callers of this public contract.
 */
typedef struct UmiNativeWebRuntime { UmiNativeWebRendererRegistry renderers; UmiNativeWebBrowserCapability browser; UmiNativeWebEventQueue events; UmiNativeWebPatchBatch patches; uint64_t revision; bool started; } UmiNativeWebRuntime;
/* Initialise the native-web runtime and register all built-in semantic renderers. */
UmiStatus umi_native_web_runtime_init(UmiNativeWebRuntime *runtime);
/* Render a surface by semantic contract through the canonical native-web registry. */
UmiStatus umi_native_web_runtime_render(UmiNativeWebRuntime *runtime, const UmiNativeWebRenderContext *context, const UmiNativeWebSurface *surface, UmiNativeWebRenderResult *result);

#ifdef __cplusplus
}
#endif
#endif
