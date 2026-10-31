/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/startup_brand_panel.h
 *
 * PURPOSE:
 *   Share scalable Umicom startup artwork and scoped native presentation styles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_GTK_STARTUP_BRAND_PANEL_H
#define UMICOM_GTK_STARTUP_BRAND_PANEL_H
#include "brand_widget.h"

/* The display owns one provider. Selectors are restricted to the startup
 * surface; other workspaces retain their own theme and density choices. */
static inline void UmiGtkStartupStyle(GtkWidget *widget)
{
    GdkDisplay *display = gtk_widget_get_display(widget);
    if (g_object_get_data(G_OBJECT(display), "umicom-startup-style") != NULL) return;
    static const char css[] =
        ".umicom-session-surface { background: #111820; color: #e8eef5; }"
        ".umicom-session-surface label { color: #e8eef5; }"
        ".umicom-session-surface .session-muted { color: #aabacb; }"
        ".umicom-session-surface .session-title { font-size: 25px; font-weight: 600; }"
        ".umicom-session-surface .session-form { padding: 32px; }"
        ".umicom-session-surface button { min-height: 32px; border-radius: 5px; }"
        ".umicom-session-surface button.suggested-action { background: #007fa9; color: white; }"
        ".umicom-session-surface stackswitcher button { background: transparent;"
        " border: none; border-bottom: 2px solid transparent; border-radius: 0; padding: 8px; }"
        ".umicom-session-surface stackswitcher button:checked { border-bottom-color: #27b7df; }"
        ".umicom-session-surface button:focus-visible { outline: 2px solid #76d6ed; }"
        ".umicom-session-surface .session-step { background: #1c2935; padding: 12px; border-radius: 6px; }"
        ".umicom-session-surface .session-footer { padding: 14px 22px; }"
        ".umicom-session-surface progressbar trough { min-height: 3px; }"
        ".umicom-session-surface progressbar progress { background: #27b7df; }";
    GtkCssProvider *provider = gtk_css_provider_new();
#if GTK_CHECK_VERSION(4, 12, 0)
    gtk_css_provider_load_from_string(provider, css);
#else
    gtk_css_provider_load_from_data(provider, css, -1);
#endif
    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 1U);
    g_object_set_data_full(G_OBJECT(display), "umicom-startup-style", provider, g_object_unref);
}

/* Original vector curves adapt to the allocated area. They contain no external
 * photograph, embedded bitmap or broker artwork. The SVG mark is a separate
 * scale-aware widget, so its transparent centre stays transparent. */
static inline void UmiGtkStartupBackdropDraw(GtkDrawingArea *area, cairo_t *cr,
    int width, int height, gpointer data)
{
    (void)area; (void)data;
    if (width <= 0 || height <= 0) return;
    cairo_save(cr);
    cairo_scale(cr, (double)width, (double)height);
    cairo_pattern_t *gradient = cairo_pattern_create_linear(0, 0, 1, 1);
    cairo_pattern_add_color_stop_rgb(gradient, 0, .025, .055, .105);
    cairo_pattern_add_color_stop_rgb(gradient, 1, .04, .15, .21);
    cairo_set_source(cr, gradient);
    cairo_paint(cr);
    cairo_pattern_destroy(gradient);
    cairo_set_line_width(cr, 1.0 / (double)MAX(width, height));
    for (int i = 0; i < 24; ++i) {
        double offset = (double)i / 38.0;
        cairo_set_source_rgba(cr, .16, .64, .77, .10 + (double)(i % 5) * .025);
        cairo_move_to(cr, -.15, .15 + offset);
        cairo_curve_to(cr, .38, .98 - offset, .53, .05 + offset, 1.12, .40 + offset);
        cairo_stroke(cr);
    }
    cairo_restore(cr);
}

static inline GtkWidget *UmiGtkStartupBackdrop(GtkWidget *content)
{
    GtkWidget *overlay = gtk_overlay_new();
    GtkWidget *canvas = gtk_drawing_area_new();
    gtk_widget_set_hexpand(canvas, TRUE);
    gtk_widget_set_vexpand(canvas, TRUE);
    gtk_widget_set_can_target(canvas, FALSE);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(canvas),
        UmiGtkStartupBackdropDraw, NULL, NULL);
    gtk_overlay_set_child(GTK_OVERLAY(overlay), canvas);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), content);
    /* The child's requested size participates in allocation, so text is never
     * clipped merely because the decorative canvas has no minimum size. */
    gtk_overlay_set_measure_overlay(GTK_OVERLAY(overlay), content, TRUE);
    gtk_widget_add_css_class(overlay, "umicom-session-surface");
    UmiGtkStartupStyle(overlay);
    return overlay;
}
#endif
