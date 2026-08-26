/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/chart_model.h
 *
 * PURPOSE:
 *   Compose canonical Design System chart specifications with axes and series references.
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
#ifndef UMICOM_UI_ANALYTICS_CHART_MODEL_H
#define UMICOM_UI_ANALYTICS_CHART_MODEL_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/chart_spec.h"
#include "umicom/ui/analytics/axis.h"
#include "umicom/ui/analytics/series_registry.h"
typedef struct UmiAnalyticsChartModel { char id[UMI_ANALYTICS_ID_CAPACITY]; char title[UMI_ANALYTICS_TEXT_CAPACITY]; UmiDesignChartSpec spec; UmiAnalyticsAxis x_axis; UmiAnalyticsAxis y_axis; UmiAnalyticsSeriesRegistry series; uint64_t revision; } UmiAnalyticsChartModel;
UmiStatus umi_analytics_chart_model_init(UmiAnalyticsChartModel *chart,const char *id,const char *title,UmiDesignChartKind kind);
UmiStatus umi_analytics_chart_model_add_series(UmiAnalyticsChartModel *chart,UmiAnalyticsSeries *series);

#ifdef __cplusplus
}
#endif

#endif
