/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_designer_docking_gtk4.c
 *
 * PURPOSE:
 *   Render active semantic docking guides and drop previews as a non-interactive
 *   GTK4 overlay.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "workbench_designer_gtk4_internal.h"


void umi_workbench_designer_gtk4_docking_draw(
    GtkDrawingArea *area,
    cairo_t *cr,
    int width,
    int height,
    gpointer user_data)
{
    UmiWorkbenchDesignerGtk4 *designer = user_data;
    UmiWorkbenchDesignerSession *session;
    const UmiWorkbenchDesignerDockingGuideModel *guides;
    size_t index;
    (void)area;
    (void)width;
    (void)height;
    if (designer == NULL) return;
    session = umi_workbench_designer_service_active(
        designer->config.controller->service);
    if (session == NULL) return;
    guides = umi_workbench_designer_session_docking_guides(session);
    if (guides == NULL || guides->count == 0U) return;
    for (index = 0U; index < guides->count; ++index) {
        const UmiWorkbenchDesignerDockingGuide *guide = &guides->guides[index];
        cairo_set_source_rgba(
            cr, 0.18, 0.62, 0.94, guide->active ? 0.75 : 0.30);
        cairo_rectangle(
            cr, guide->bounds.x, guide->bounds.y,
            guide->bounds.width, guide->bounds.height);
        if (guide->active) cairo_fill(cr); else cairo_stroke(cr);
    }
}
