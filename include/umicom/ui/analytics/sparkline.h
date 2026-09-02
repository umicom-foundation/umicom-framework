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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_SPARKLINE_H
#define UMICOM_UI_ANALYTICS_SPARKLINE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics sparkline summary data shared with callers of this public
 * contract.
 */
typedef struct UmiAnalyticsSparklineSummary { double first,last,minimum,maximum,change; size_t count; } UmiAnalyticsSparklineSummary;
/**
 * Provide the analytics sparkline summarize operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_sparkline_summarize(const double *values,size_t count,UmiAnalyticsSparklineSummary *out_summary);

#ifdef __cplusplus
}
#endif

#endif
