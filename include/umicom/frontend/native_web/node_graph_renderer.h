/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/node_graph_renderer.h
 *
 * PURPOSE:
 *   Render semantic node-graph nodes/ports/links for AI, media and workflow applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_NODE_GRAPH_RENDERER_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_NODE_GRAPH_RENDERER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/renderer_descriptor.h"
/* Render the node graph semantic surface to standards-based browser markup. */
UmiStatus umi_native_web_node_graph_render(const UmiNativeWebRenderContext *context, const UmiNativeWebSurface *surface, UmiNativeWebRenderResult *result);
/* Return the renderer descriptor used for registry/conformance checks. */
UmiNativeWebRendererDescriptor umi_native_web_node_graph_renderer_descriptor(void);

#ifdef __cplusplus
}
#endif
#endif
