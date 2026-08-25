/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/node_graph.h
 *
 * PURPOSE:
 *   Render generic workflow/compositor/AI/audio/system node graphs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_NODE_GRAPH_H
#define UMICOM_UI_QT6_WORKSTATION_NODE_GRAPH_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/node_graph_surface.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_ws_node_graph_descriptor(void);
UmiQt6WidgetHandle umi_qt6_ws_node_graph_create(const UmiWsNodeGraphSurface *graph);

#ifdef __cplusplus
}
#endif
#endif
