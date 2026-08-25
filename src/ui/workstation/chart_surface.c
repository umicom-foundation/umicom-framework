/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/chart_surface.c
 *
 * PURPOSE:
 *   Implement chart-surface presentation and synchronisation state reusable by Trader, monitoring, profiling and analytics applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/chart_surface.h"

UmiStatus umi_ws_chart_surface_init(UmiWsChartSurface *surface, const char *chart_id, const char *title) {
    if (surface == NULL || !umi_ws_id_valid(chart_id) || title == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *surface = (UmiWsChartSurface){0};
    if (umi_ws_copy_text(surface->chart_id, sizeof(surface->chart_id), chart_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_ws_copy_text(surface->title, sizeof(surface->title), title) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    surface->interval_seconds = 60;
    surface->zoom = 1.0;
    surface->sync_symbol = true;
    surface->sync_time = true;
    surface->sync_crosshair = true;
    surface->show_grid = true;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_chart_surface_set_context_group(UmiWsChartSurface *surface, const char *group_id) {
    if (surface == NULL || !umi_ws_id_valid(group_id)) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ws_copy_text(surface->context_group, sizeof(surface->context_group), group_id);
}

UmiStatus umi_ws_chart_surface_set_interval(UmiWsChartSurface *surface, int32_t interval_seconds) {
    if (surface == NULL || interval_seconds <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    surface->interval_seconds = interval_seconds;
    return UMI_STATUS_OK;
}

double umi_ws_chart_surface_set_zoom(UmiWsChartSurface *surface, double zoom) {
    if (surface == NULL) return 0.0;
    if (zoom < 0.1) zoom = 0.1;
    if (zoom > 20.0) zoom = 20.0;
    surface->zoom = zoom;
    return zoom;
}
