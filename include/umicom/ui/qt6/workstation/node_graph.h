/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/node_graph.h
 *
 * PURPOSE:
 *   Render generic workflow/compositor/AI/audio/system node graphs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_NODE_GRAPH_H
#define UMICOM_UI_QT6_WORKSTATION_NODE_GRAPH_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/node_graph_surface.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 ws node graph descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_ws_node_graph_descriptor(void);
/**
 * Initialise qt6 ws node graph from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_ws_node_graph_create(const UmiWsNodeGraphSurface *graph);

#ifdef __cplusplus
}
#endif
#endif
