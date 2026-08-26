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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_ROLLING_STATISTICS_H
#define UMICOM_UI_ANALYTICS_ROLLING_STATISTICS_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_analytics_rolling_mean(const double *values,size_t count,size_t window,double *out_values,size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
