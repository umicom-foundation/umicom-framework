/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/render_context.h
 *
 * PURPOSE:
 *   Carry session, route, theme, density, locale and revision state through web rendering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_RENDER_CONTEXT_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_RENDER_CONTEXT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/types.h"

typedef struct UmiNativeWebRenderContext { char session_id[UMI_NATIVE_WEB_ID_CAPACITY]; char route[UMI_NATIVE_WEB_TEXT_CAPACITY]; char theme[64]; char density[64]; char locale[32]; uint64_t revision; } UmiNativeWebRenderContext;
/* Initialise render context with stable session and route identity. */
UmiStatus umi_native_web_render_context_init(UmiNativeWebRenderContext *context, const char *session_id, const char *route);
/* Validate required render context identity. */
UmiStatus umi_native_web_render_context_validate(const UmiNativeWebRenderContext *context);

#ifdef __cplusplus
}
#endif
#endif
