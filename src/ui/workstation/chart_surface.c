/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/chart_surface.c
 *
 * PURPOSE:
 *   Implement chart-surface presentation and synchronisation state reusable by Trader, monitoring, profiling and analytics applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/text.h"
#include "umicom/ui/workstation/chart_surface.h"
#include "../../base/value_construction_internal.h"

/*
 * Initialise ws chart surface from caller-provided values so later operations receive a
 * known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_ws_chart_surface_init(UmiWsChartSurface *surface, const char *chart_id, const char *title) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL || !umi_ws_id_valid(chart_id) || title == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *surface = (UmiWsChartSurface){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(surface->chart_id, sizeof(surface->chart_id), chart_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(surface->title, sizeof(surface->title), title) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    surface->interval_seconds = 60;
    surface->zoom = 1.0;
    surface->sync_symbol = true;
    surface->sync_time = true;
    surface->sync_crosshair = true;
    surface->show_grid = true;
    return UMI_STATUS_OK;
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_ws_chart_surface_init_build_value(UmiWsChartSurface *surface, const char *chart_id, const char *title) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL || !umi_ws_id_valid(chart_id) || title == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *surface = (UmiWsChartSurface){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(surface->chart_id, sizeof(surface->chart_id), chart_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(surface->title, sizeof(surface->title), title) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    surface->interval_seconds = 60;
    surface->zoom = 1.0;
    surface->sync_symbol = true;
    surface->sync_time = true;
    surface->sync_crosshair = true;
    surface->show_grid = true;
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_ws_chart_surface_init, UmiWsChartSurface, surface,
    (UmiWsChartSurface *surface, const char *chart_id, const char *title),
    umi_ws_chart_surface_init_build_value(candidate, chart_id, title))


/*
 * Provide the ws chart surface set context group operation used by this module and its
 * client applications.
 */
/* The presentation copy could erase a rejected field or overlap its input.
 * Retain it for review; the replacement validates before publishing any text. */
#if 0
UmiStatus umi_ws_chart_surface_set_context_group(UmiWsChartSurface *surface, const char *group_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL || !umi_ws_id_valid(group_id)) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ws_copy_text(surface->context_group, sizeof(surface->context_group), group_id);
}
#endif

UmiStatus umi_ws_chart_surface_set_context_group(UmiWsChartSurface *surface, const char *group_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL || !umi_ws_id_valid(group_id)) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_text_replace(surface->context_group, sizeof(surface->context_group), group_id);
}

/*
 * Provide the ws chart surface set interval operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_chart_surface_set_interval(UmiWsChartSurface *surface, int32_t interval_seconds) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL || interval_seconds <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    surface->interval_seconds = interval_seconds;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws chart surface set zoom operation used by this module and its client
 * applications.
 */
double umi_ws_chart_surface_set_zoom(UmiWsChartSurface *surface, double zoom) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL) return 0.0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (zoom < 0.1) zoom = 0.1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (zoom > 20.0) zoom = 20.0;
    surface->zoom = zoom;
    return zoom;
}
