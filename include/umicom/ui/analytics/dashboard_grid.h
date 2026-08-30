/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/dashboard_grid.h
 *
 * PURPOSE:
 *   Validate non-overlapping dashboard tile placement on bounded logical grids.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral analytics capability extends canonical Umicom::ui.
 *   Existing Design System chart/gauge/heatmap specs and workstation surfaces
 *   remain authoritative; GTK4, Qt6, Native Web and thin applications render
 *   the same Framework-owned analytics semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_DASHBOARD_GRID_H
#define UMICOM_UI_ANALYTICS_DASHBOARD_GRID_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/analytics/dashboard_tile.h"
typedef struct UmiAnalyticsDashboardGrid { uint16_t rows; uint16_t columns; UmiAnalyticsDashboardTile tiles[UMI_ANALYTICS_MAX_ITEMS]; size_t count; } UmiAnalyticsDashboardGrid;
UmiStatus umi_analytics_dashboard_grid_init(UmiAnalyticsDashboardGrid *g,uint16_t rows,uint16_t columns);
UmiStatus umi_analytics_dashboard_grid_add(UmiAnalyticsDashboardGrid *g,const UmiAnalyticsDashboardTile *tile);

#ifdef __cplusplus
}
#endif

#endif
