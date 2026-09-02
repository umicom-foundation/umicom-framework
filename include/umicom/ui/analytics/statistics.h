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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_STATISTICS_H
#define UMICOM_UI_ANALYTICS_STATISTICS_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics statistics data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsStatistics { size_t count; double minimum; double maximum; double sum; double mean; double variance; double standard_deviation; } UmiAnalyticsStatistics;
/**
 * Provide the analytics statistics compute operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_statistics_compute(const double *values,size_t count,UmiAnalyticsStatistics *out_stats);

#ifdef __cplusplus
}
#endif

#endif
