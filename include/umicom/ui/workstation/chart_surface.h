/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/chart_surface.h
 *
 * PURPOSE:
 *   Define chart-surface presentation and synchronisation state reusable by Trader, monitoring, profiling and analytics applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_CHART_SURFACE_H
#define UMICOM_UI_WORKSTATION_CHART_SURFACE_H

#include <stdbool.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsChartSurface {
    char chart_id[UMI_UI_ID_CAPACITY];
    char title[UMI_UI_TEXT_CAPACITY];
    char context_group[UMI_UI_ID_CAPACITY];
    int32_t interval_seconds;
    double zoom;
    bool sync_symbol;
    bool sync_time;
    bool sync_crosshair;
    bool show_grid;
} UmiWsChartSurface;

UmiStatus umi_ws_chart_surface_init(UmiWsChartSurface *surface, const char *chart_id, const char *title);
UmiStatus umi_ws_chart_surface_set_context_group(UmiWsChartSurface *surface, const char *group_id);
UmiStatus umi_ws_chart_surface_set_interval(UmiWsChartSurface *surface, int32_t interval_seconds);
double umi_ws_chart_surface_set_zoom(UmiWsChartSurface *surface, double zoom);

#ifdef __cplusplus
}
#endif

#endif
