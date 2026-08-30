/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/legend.h
 *
 * PURPOSE:
 *   Maintain bounded semantic legend entries and visibility state.
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
#ifndef UMICOM_UI_ANALYTICS_LEGEND_H
#define UMICOM_UI_ANALYTICS_LEGEND_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsLegendItem { char series_id[UMI_ANALYTICS_ID_CAPACITY]; char label[UMI_ANALYTICS_TEXT_CAPACITY]; int visible; } UmiAnalyticsLegendItem;
typedef struct UmiAnalyticsLegend { UmiAnalyticsLegendItem items[UMI_ANALYTICS_MAX_SERIES]; size_t count; } UmiAnalyticsLegend;
void umi_analytics_legend_init(UmiAnalyticsLegend *legend);
UmiStatus umi_analytics_legend_add(UmiAnalyticsLegend *legend,const char *series_id,const char *label);
UmiStatus umi_analytics_legend_set_visible(UmiAnalyticsLegend *legend,const char *series_id,int visible);

#ifdef __cplusplus
}
#endif

#endif
