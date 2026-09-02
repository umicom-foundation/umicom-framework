/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/stacked_bar_chart.h
 *
 * PURPOSE:
 *   Accumulate positive/negative stack totals for stacked bars.
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
#ifndef UMICOM_UI_ANALYTICS_STACKED_BAR_CHART_H
#define UMICOM_UI_ANALYTICS_STACKED_BAR_CHART_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics stack total data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsStackTotal { double positive; double negative; } UmiAnalyticsStackTotal;
/**
 * Provide the analytics stacked bar total operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_stacked_bar_total(const double *values,size_t count,UmiAnalyticsStackTotal *out_total);

#ifdef __cplusplus
}
#endif

#endif
