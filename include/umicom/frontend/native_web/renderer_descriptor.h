/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/renderer_descriptor.h
 *
 * PURPOSE:
 *   Describe semantic-contract renderers and invoke them through a stable C function table.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_RENDERER_DESCRIPTOR_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_RENDERER_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/render_context.h"
#include "umicom/frontend/native_web/render_result.h"

typedef struct UmiNativeWebSurface { char surface_id[UMI_NATIVE_WEB_ID_CAPACITY]; char semantic_contract[UMI_NATIVE_WEB_ID_CAPACITY]; char title[UMI_NATIVE_WEB_TEXT_CAPACITY]; char content[UMI_NATIVE_WEB_VALUE_CAPACITY]; uint64_t flags; } UmiNativeWebSurface;
typedef UmiStatus (*UmiNativeWebRenderFn)(const UmiNativeWebRenderContext *context, const UmiNativeWebSurface *surface, UmiNativeWebRenderResult *result);
typedef struct UmiNativeWebRendererDescriptor { char renderer_id[UMI_NATIVE_WEB_ID_CAPACITY]; char semantic_contract[UMI_NATIVE_WEB_ID_CAPACITY]; UmiNativeWebRenderFn render; uint64_t capability_flags; } UmiNativeWebRendererDescriptor;
/* Validate a renderer descriptor before registry insertion. */
UmiStatus umi_native_web_renderer_descriptor_validate(const UmiNativeWebRendererDescriptor *descriptor);
/* Render a common accessible surface wrapper used by specialised renderers. */
UmiStatus umi_native_web_render_surface_wrapper(const UmiNativeWebSurface *surface, const char *css_class, const char *role, UmiNativeWebRenderResult *result);

#ifdef __cplusplus
}
#endif
#endif
