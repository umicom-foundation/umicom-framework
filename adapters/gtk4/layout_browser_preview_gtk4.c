/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/layout_browser_preview_gtk4.c
 *
 * PURPOSE:
 *   Draw semantic Layout Browser thumbnails from the Framework minimap
 *   projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "workbench_designer_gtk4_internal.h"


/*
 * Provide the workbench layout browser gtk4 preview draw operation used by this module and
 * its client applications.
 */
void umi_workbench_layout_browser_gtk4_preview_draw(
    GtkDrawingArea *area,
    cairo_t *cr,
    int width,
    int height,
    gpointer user_data)
{
    UmiWorkbenchLayoutBrowserGtk4 *browser = user_data;
    size_t index;
    (void)area;
    cairo_set_source_rgb(cr, 0.07, 0.08, 0.09);
    cairo_rectangle(cr, 0.0, 0.0, (double)width, (double)height);
    cairo_fill(cr);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (browser == NULL || browser->model.count == 0U) return;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < browser->model.count && index < 12U; ++index) {
        const double x = 12.0 + (double)(index % 3U) * 112.0;
        const double y = 12.0 + (double)(index / 3U) * 48.0;
        cairo_set_source_rgba(cr, 0.18, 0.48, 0.72, 0.65);
        cairo_rectangle(cr, x, y, 96.0, 36.0);
        cairo_fill_preserve(cr);
        cairo_set_source_rgb(cr, 0.38, 0.70, 0.92);
        cairo_stroke(cr);
    }
}
