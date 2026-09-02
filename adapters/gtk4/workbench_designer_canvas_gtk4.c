/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_designer_canvas_gtk4.c
 *
 * PURPOSE:
 *   Render semantic canvas items, selection and active state using GTK4 drawing-
 *   area callbacks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "workbench_designer_gtk4_internal.h"


/* Provide the draw canvas item operation used by this module and its client applications. */
static void draw_canvas_item(
    cairo_t *cr,
    const UmiWorkbenchDesignerCanvasItem *item)
{
    const UmiWorkbenchDesignerRect rect = item->screen_bounds;
    /* Apply this branch only when its contract condition is satisfied. */
    if (item->container) {
        cairo_set_source_rgba(cr, 0.22, 0.28, 0.34, 0.30);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (item->selected) {
        cairo_set_source_rgba(cr, 0.13, 0.48, 0.82, 0.72);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        cairo_set_source_rgba(cr, 0.14, 0.17, 0.20, 0.92);
    }
    cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
    cairo_fill_preserve(cr);
    /* Apply this branch only when its contract condition is satisfied. */
    if (item->primary) cairo_set_source_rgb(cr, 0.35, 0.82, 1.0);
    /* Use this fallback path when the earlier condition does not apply. */
    else cairo_set_source_rgb(cr, 0.40, 0.46, 0.52);
    cairo_set_line_width(cr, item->primary ? 2.5 : 1.0);
    cairo_stroke(cr);
    cairo_set_source_rgb(cr, 0.93, 0.95, 0.97);
    cairo_move_to(cr, rect.x + 8.0, rect.y + 18.0);
    cairo_show_text(cr, item->title[0] != '\0' ? item->title : item->node_id);
}

/*
 * Provide the workbench designer gtk4 canvas draw operation used by this module and its
 * client applications.
 */
void umi_workbench_designer_gtk4_canvas_draw(
    GtkDrawingArea *area,
    cairo_t *cr,
    int width,
    int height,
    gpointer user_data)
{
    UmiWorkbenchDesignerGtk4 *designer = user_data;
    UmiWorkbenchDesignerSession *session;
    const UmiWorkbenchDesignerCanvas *canvas;
    size_t index;
    (void)area;
    cairo_set_source_rgb(cr, 0.08, 0.09, 0.10);
    cairo_rectangle(cr, 0.0, 0.0, (double)width, (double)height);
    cairo_fill(cr);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (designer == NULL) return;
    session = umi_workbench_designer_service_active(
        designer->config.controller->service);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    canvas = umi_workbench_designer_session_canvas(session);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (canvas == NULL) return;
    cairo_select_font_face(
        cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 12.0);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < canvas->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (canvas->items[index].visible) {
            draw_canvas_item(cr, &canvas->items[index]);
        }
    }
}

/*
 * Provide the workbench designer gtk4 build canvas operation used by this module and its
 * client applications.
 */
GtkWidget *umi_workbench_designer_gtk4_build_canvas(
    UmiWorkbenchDesignerGtk4 *designer)
{
    GtkWidget *overlay = gtk_overlay_new();
    designer->canvas_area = gtk_drawing_area_new();
    gtk_widget_set_hexpand(designer->canvas_area, TRUE);
    gtk_widget_set_vexpand(designer->canvas_area, TRUE);
    gtk_widget_set_focusable(designer->canvas_area, TRUE);
    gtk_drawing_area_set_content_width(
        GTK_DRAWING_AREA(designer->canvas_area), 900);
    gtk_drawing_area_set_content_height(
        GTK_DRAWING_AREA(designer->canvas_area), 650);
    gtk_drawing_area_set_draw_func(
        GTK_DRAWING_AREA(designer->canvas_area),
        umi_workbench_designer_gtk4_canvas_draw,
        designer,
        NULL);
    gtk_overlay_set_child(GTK_OVERLAY(overlay), designer->canvas_area);
    designer->docking_area = gtk_drawing_area_new();
    gtk_widget_set_can_target(designer->docking_area, FALSE);
    gtk_drawing_area_set_draw_func(
        GTK_DRAWING_AREA(designer->docking_area),
        umi_workbench_designer_gtk4_docking_draw,
        designer,
        NULL);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), designer->docking_area);
    /* Apply this branch only when its contract condition is satisfied. */
    if (designer->config.show_minimap) {
        designer->minimap_area = gtk_drawing_area_new();
        gtk_widget_set_size_request(designer->minimap_area, 220, 140);
        gtk_widget_set_halign(designer->minimap_area, GTK_ALIGN_END);
        gtk_widget_set_valign(designer->minimap_area, GTK_ALIGN_END);
        gtk_widget_set_margin_end(designer->minimap_area, 12);
        gtk_widget_set_margin_bottom(designer->minimap_area, 12);
        gtk_drawing_area_set_draw_func(
            GTK_DRAWING_AREA(designer->minimap_area),
            umi_workbench_designer_gtk4_minimap_draw,
            designer,
            NULL);
        gtk_overlay_add_overlay(GTK_OVERLAY(overlay), designer->minimap_area);
    }
    return overlay;
}
