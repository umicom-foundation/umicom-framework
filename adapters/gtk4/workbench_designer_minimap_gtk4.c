/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_designer_minimap_gtk4.c
 *
 * PURPOSE:
 *   Render the designer minimap overview and visible viewport using the toolkit-
 *   neutral minimap model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "workbench_designer_gtk4_internal.h"


void umi_workbench_designer_gtk4_minimap_draw(
    GtkDrawingArea *area,
    cairo_t *cr,
    int width,
    int height,
    gpointer user_data)
{
    UmiWorkbenchDesignerGtk4 *designer = user_data;
    UmiWorkbenchDesignerSession *session;
    const UmiWorkbenchDesignerCanvas *canvas;
    UmiWorkbenchDesignerMinimap minimap;
    UmiWorkbenchDesignerSize size;
    size_t index;
    (void)area;
    cairo_set_source_rgba(cr, 0.04, 0.05, 0.06, 0.92);
    cairo_rectangle(cr, 0.0, 0.0, (double)width, (double)height);
    cairo_fill(cr);
    if (designer == NULL) return;
    session = umi_workbench_designer_service_active(
        designer->config.controller->service);
    if (session == NULL) return;
    canvas = umi_workbench_designer_session_canvas(session);
    if (canvas == NULL) return;
    size.width = (double)width;
    size.height = (double)height;
    umi_workbench_designer_minimap_init(&minimap);
    if (umi_workbench_designer_minimap_build(
            &minimap, canvas,
            umi_workbench_designer_session_viewport(session), size) !=
        UMI_STATUS_OK) return;
    for (index = 0U; index < minimap.count; ++index) {
        const UmiWorkbenchDesignerMinimapItem *item = &minimap.items[index];
        cairo_set_source_rgba(
            cr, item->selected ? 0.25 : 0.35,
            item->selected ? 0.72 : 0.43,
            item->selected ? 0.95 : 0.52,
            0.75);
        cairo_rectangle(
            cr, item->bounds.x, item->bounds.y,
            item->bounds.width, item->bounds.height);
        cairo_fill(cr);
    }
    cairo_set_source_rgba(cr, 0.95, 0.95, 0.95, 0.9);
    cairo_set_line_width(cr, 1.5);
    cairo_rectangle(
        cr, minimap.viewport_bounds.x, minimap.viewport_bounds.y,
        minimap.viewport_bounds.width, minimap.viewport_bounds.height);
    cairo_stroke(cr);
}
