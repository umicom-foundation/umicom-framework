/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/dashboard_tile.h
 *
 * PURPOSE:
 *   Describe one grid-positioned dashboard tile and its semantic component identity.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral analytics capability extends canonical Umicom::ui.
 *   Existing Design System chart/gauge/heatmap specs and workstation surfaces
 *   remain authoritative; GTK4, Qt6, Native Web and thin applications render
 *   the same Framework-owned analytics semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_DASHBOARD_TILE_H
#define UMICOM_UI_ANALYTICS_DASHBOARD_TILE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsDashboardTile { char id[UMI_ANALYTICS_ID_CAPACITY]; char component_id[UMI_ANALYTICS_ID_CAPACITY]; uint16_t row; uint16_t column; uint16_t row_span; uint16_t column_span; } UmiAnalyticsDashboardTile;
UmiStatus umi_analytics_dashboard_tile_init(UmiAnalyticsDashboardTile *item);
int umi_analytics_dashboard_tile_valid(const UmiAnalyticsDashboardTile *item);

#ifdef __cplusplus
}
#endif

#endif
