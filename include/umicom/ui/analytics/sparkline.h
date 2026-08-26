/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/sparkline.h
 *
 * PURPOSE:
 *   Summarize compact time-series direction and range for dashboard cells.
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
#ifndef UMICOM_UI_ANALYTICS_SPARKLINE_H
#define UMICOM_UI_ANALYTICS_SPARKLINE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsSparklineSummary { double first,last,minimum,maximum,change; size_t count; } UmiAnalyticsSparklineSummary;
UmiStatus umi_analytics_sparkline_summarize(const double *values,size_t count,UmiAnalyticsSparklineSummary *out_summary);

#ifdef __cplusplus
}
#endif

#endif
