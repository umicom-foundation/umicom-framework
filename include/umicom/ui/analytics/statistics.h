/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/statistics.h
 *
 * PURPOSE:
 *   Compute numerically stable descriptive statistics using Welford accumulation.
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
#ifndef UMICOM_UI_ANALYTICS_STATISTICS_H
#define UMICOM_UI_ANALYTICS_STATISTICS_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsStatistics { size_t count; double minimum; double maximum; double sum; double mean; double variance; double standard_deviation; } UmiAnalyticsStatistics;
UmiStatus umi_analytics_statistics_compute(const double *values,size_t count,UmiAnalyticsStatistics *out_stats);

#ifdef __cplusplus
}
#endif

#endif
