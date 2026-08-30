/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/node_graph_gtk4.c
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

#include "umicom/ui/gtk4/workstation/node_graph.h"

static const UmiWsGraphNode *find_node(const UmiWsNodeGraphSurface *graph, const char *id) {
    size_t i;
    if (graph == NULL || id == NULL) return NULL;
    for (i = 0U; i < graph->node_count; ++i) if (g_strcmp0(graph->nodes[i].node_id, id) == 0) return &graph->nodes[i];
    return NULL;
}

static void draw_graph(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data) {
    const UmiWsNodeGraphSurface *graph = data;
    size_t i;
    (void)area; (void)width; (void)height;
    if (graph == NULL) return;
    cairo_set_line_width(cr, 1.5);
    cairo_set_source_rgba(cr, 0.55, 0.55, 0.72, 0.55);
    for (i = 0U; i < graph->edge_count; ++i) {
        const UmiWsGraphNode *a = find_node(graph, graph->edges[i].source_id);
        const UmiWsGraphNode *b = find_node(graph, graph->edges[i].target_id);
        if (a != NULL && b != NULL) { cairo_move_to(cr, a->x, a->y); cairo_line_to(cr, b->x, b->y); }
    }
    cairo_stroke(cr);
    for (i = 0U; i < graph->node_count; ++i) {
        cairo_set_source_rgba(cr, 0.25, 0.45, 0.85, 0.85);
        cairo_rectangle(cr, graph->nodes[i].x - 36.0, graph->nodes[i].y - 16.0, 72.0, 32.0);
        cairo_fill(cr);
    }
}

GtkWidget *umi_gtk4_ws_node_graph_create(const UmiWsNodeGraphSurface *graph) {
    GtkWidget *area = gtk_drawing_area_new();
    gtk_widget_add_css_class(area, "umicom-node-graph");
    gtk_widget_set_hexpand(area, TRUE); gtk_widget_set_vexpand(area, TRUE);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw_graph, (gpointer)graph, NULL);
    return area;
}
