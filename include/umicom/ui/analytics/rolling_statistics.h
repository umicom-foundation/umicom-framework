/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/rolling_statistics.h
 *
 * PURPOSE:
 *   Compute rolling means over bounded trailing windows.
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
#ifndef UMICOM_UI_ANALYTICS_ROLLING_STATISTICS_H
#define UMICOM_UI_ANALYTICS_ROLLING_STATISTICS_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the analytics rolling mean operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_rolling_mean(const double *values,size_t count,size_t window,double *out_values,size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
