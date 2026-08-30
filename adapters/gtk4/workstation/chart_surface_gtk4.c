/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/chart_surface_gtk4.c
 *
 * PURPOSE:
 *   Render a reusable GTK4 chart viewport scaffold for Trader, profiling, analytics and operational time-series panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/chart_surface.h"

static void draw_chart(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data) {
    const UmiWsChartSurface *surface = data;
    int x;
    (void)area;
    cairo_set_line_width(cr, 1.0);
    cairo_set_source_rgba(cr, 0.5, 0.55, 0.65, 0.18);
    if (surface == NULL || surface->show_grid) {
        for (x = 0; x < width; x += 48) { cairo_move_to(cr, (double)x, 0.0); cairo_line_to(cr, (double)x, (double)height); }
        for (x = 0; x < height; x += 36) { cairo_move_to(cr, 0.0, (double)x); cairo_line_to(cr, (double)width, (double)x); }
        cairo_stroke(cr);
    }
    cairo_set_source_rgba(cr, 0.35, 0.75, 0.65, 0.9);
    cairo_move_to(cr, 0.0, (double)height * 0.72);
    cairo_curve_to(cr, (double)width * 0.25, (double)height * 0.60, (double)width * 0.55, (double)height * 0.48, (double)width, (double)height * 0.28);
    cairo_stroke(cr);
}

GtkWidget *umi_gtk4_ws_chart_surface_create(const UmiWsChartSurface *surface) {
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_label_new(surface != NULL ? surface->title : "Chart");
    GtkWidget *area = gtk_drawing_area_new();
    gtk_widget_add_css_class(root, "umicom-chart-surface");
    gtk_label_set_xalign(GTK_LABEL(header), 0.0F);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw_chart, (gpointer)surface, NULL);
    gtk_widget_set_hexpand(area, TRUE); gtk_widget_set_vexpand(area, TRUE);
    gtk_box_append(GTK_BOX(root), header); gtk_box_append(GTK_BOX(root), area);
    return root;
}
