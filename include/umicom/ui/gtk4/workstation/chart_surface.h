/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/chart_surface.h
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

#ifndef UMICOM_UI_GTK4_WORKSTATION_CHART_SURFACE_H
#define UMICOM_UI_GTK4_WORKSTATION_CHART_SURFACE_H

#include <gtk/gtk.h>
#include "umicom/chart/render_scene.h"
#include "umicom/ui/workstation/chart_surface.h"

/**
 * Initialise gtk4 ws chart surface from caller-provided values so later operations receive
 * a known state.
 */
GtkWidget *umi_gtk4_ws_chart_surface_create(const UmiWsChartSurface *surface);

/*
 * Create a chart surface with an independent copy of an optional render scene.
 * Pass NULL for scene when the application only needs an empty chart frame.
 */
GtkWidget *umi_gtk4_ws_chart_surface_create_with_scene(
    const UmiWsChartSurface *surface,
    const UmiChartRenderScene *scene
);

/*
 * Replace the scene owned by an existing chart widget and request a redraw.
 * Call this on the GTK main thread after live data produces a new scene.
 */
UmiStatus umi_gtk4_ws_chart_surface_set_scene(
    GtkWidget *chart_widget,
    const UmiChartRenderScene *scene
);

#endif
