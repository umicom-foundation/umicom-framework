/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/media_canvas_gtk4.c
 *
 * PURPOSE:
 *   Render shared media/canvas/waveform viewport scaffolds for video, image, music, AI creator and design applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include "umicom/ui/gtk4/workstation/media_canvas.h"

static void draw_media(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data) {
    const UmiWsMediaViewport *viewport = data;
    (void)area;
    cairo_set_source_rgba(cr, 0.06, 0.07, 0.09, 1.0); cairo_paint(cr);
    cairo_set_source_rgba(cr, 0.25, 0.35, 0.55, 0.55);
    cairo_rectangle(cr, (double)width * 0.08, (double)height * 0.08, (double)width * 0.84, (double)height * 0.84); cairo_stroke(cr);
    if (viewport != NULL && viewport->show_safe_area) {
        cairo_rectangle(cr, (double)width * 0.15, (double)height * 0.15, (double)width * 0.70, (double)height * 0.70); cairo_stroke(cr);
    }
}

static void draw_waveform(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data) {
    const UmiWsWaveformSurface *waveform = data;
    int x;
    (void)area;
    cairo_set_source_rgba(cr, 0.30, 0.75, 0.62, 0.9); cairo_set_line_width(cr, 1.0);
    cairo_move_to(cr, 0.0, (double)height * 0.5);
    for (x = 0; x < width; x += 4) {
        double amp = (double)((x / 4) % 11 - 5) * (waveform != NULL ? waveform->vertical_scale : 1.0) * 2.5;
        cairo_line_to(cr, (double)x, (double)height * 0.5 + amp);
    }
    cairo_stroke(cr);
}

static void draw_canvas(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data) {
    const UmiWsCanvasSurface *canvas = data;
    int step = canvas != NULL && canvas->grid_size > 0 ? canvas->grid_size : 24;
    int p;
    (void)area;
    if (canvas != NULL && !canvas->show_grid) return;
    cairo_set_source_rgba(cr, 0.45, 0.5, 0.6, 0.16); cairo_set_line_width(cr, 1.0);
    for (p = 0; p < width; p += step) { cairo_move_to(cr, (double)p, 0.0); cairo_line_to(cr, (double)p, (double)height); }
    for (p = 0; p < height; p += step) { cairo_move_to(cr, 0.0, (double)p); cairo_line_to(cr, (double)width, (double)p); }
    cairo_stroke(cr);
}

GtkWidget *umi_gtk4_ws_media_viewport_create(const UmiWsMediaViewport *viewport) {
    GtkWidget *area = gtk_drawing_area_new();
    gtk_widget_add_css_class(area, "umicom-media-viewport");
    gtk_widget_set_hexpand(area, TRUE); gtk_widget_set_vexpand(area, TRUE);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw_media, (gpointer)viewport, NULL);
    return area;
}
GtkWidget *umi_gtk4_ws_waveform_create(const UmiWsWaveformSurface *waveform) {
    GtkWidget *area = gtk_drawing_area_new();
    gtk_widget_add_css_class(area, "umicom-waveform"); gtk_widget_set_hexpand(area, TRUE); gtk_widget_set_vexpand(area, TRUE);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw_waveform, (gpointer)waveform, NULL); return area;
}
GtkWidget *umi_gtk4_ws_canvas_create(const UmiWsCanvasSurface *canvas) {
    GtkWidget *area = gtk_drawing_area_new();
    gtk_widget_add_css_class(area, "umicom-design-canvas"); gtk_widget_set_hexpand(area, TRUE); gtk_widget_set_vexpand(area, TRUE);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw_canvas, (gpointer)canvas, NULL); return area;
}
