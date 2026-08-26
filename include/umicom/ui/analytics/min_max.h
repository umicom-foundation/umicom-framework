/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/min_max.h
 *
 * PURPOSE:
 *   Compute finite minimum and maximum values from numeric samples.
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
#ifndef UMICOM_UI_ANALYTICS_MIN_MAX_H
#define UMICOM_UI_ANALYTICS_MIN_MAX_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsMinMax { double minimum; double maximum; size_t count; } UmiAnalyticsMinMax;
UmiStatus umi_analytics_min_max_compute(const double *values,size_t count,UmiAnalyticsMinMax *out_result);

#ifdef __cplusplus
}
#endif

#endif
