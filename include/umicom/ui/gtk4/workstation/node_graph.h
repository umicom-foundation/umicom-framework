/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/node_graph.h
 *
 * PURPOSE:
 *   Render a generic GTK4 node-graph canvas for AI pipelines, compositor graphs, audio routing, workflows and system topology.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_NODE_GRAPH_H
#define UMICOM_UI_GTK4_WORKSTATION_NODE_GRAPH_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/node_graph_surface.h"

GtkWidget *umi_gtk4_ws_node_graph_create(const UmiWsNodeGraphSurface *graph);

#endif
