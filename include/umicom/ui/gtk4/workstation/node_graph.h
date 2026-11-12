/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/node_graph.h
 *
 * PURPOSE:
 *   Render a generic GTK4 node-graph canvas for AI pipelines, compositor graphs, audio routing, workflows and system topology.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_NODE_GRAPH_H
#define UMICOM_UI_GTK4_WORKSTATION_NODE_GRAPH_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/node_graph_surface.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise gtk4 ws node graph from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_ws_node_graph_create(const UmiWsNodeGraphSurface *graph);


#ifdef __cplusplus
}
#endif
#endif
