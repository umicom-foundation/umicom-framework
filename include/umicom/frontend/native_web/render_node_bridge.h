/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/render_node_bridge.h
 *
 * PURPOSE:
 *   Project the existing Umicom frontend render-tree snapshot into native-web semantic elements.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_RENDER_NODE_BRIDGE_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_RENDER_NODE_BRIDGE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/semantic_element.h"
#include "umicom/frontend/render_tree.h"
/* Convert a generic frontend render-node snapshot into a keyed native-web element. */
UmiStatus umi_native_web_render_node_bridge(const UmiFrontendRenderNodeSnapshot *snapshot, UmiNativeWebSemanticElement *out_element);

#ifdef __cplusplus
}
#endif
#endif
