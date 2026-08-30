/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/heatmap_cell.h
 *
 * PURPOSE:
 *   Describe one labelled heatmap cell for tooltip and accessibility projection.
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
#ifndef UMICOM_UI_ANALYTICS_HEATMAP_CELL_H
#define UMICOM_UI_ANALYTICS_HEATMAP_CELL_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsHeatmapCell { uint16_t row; uint16_t column; double value; char label[UMI_ANALYTICS_TEXT_CAPACITY]; } UmiAnalyticsHeatmapCell;
UmiStatus umi_analytics_heatmap_cell_init(UmiAnalyticsHeatmapCell *item);
int umi_analytics_heatmap_cell_valid(const UmiAnalyticsHeatmapCell *item);

#ifdef __cplusplus
}
#endif

#endif
