/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/time_point.h
 *
 * PURPOSE:
 *   Represent one timestamped analytics sample.
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
#ifndef UMICOM_UI_ANALYTICS_TIME_POINT_H
#define UMICOM_UI_ANALYTICS_TIME_POINT_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsTimePoint { int64_t timestamp_ns; double value; } UmiAnalyticsTimePoint;
UmiStatus umi_analytics_time_point_init(UmiAnalyticsTimePoint *point, int64_t timestamp_ns, double value);
int umi_analytics_time_point_compare(const UmiAnalyticsTimePoint *left, const UmiAnalyticsTimePoint *right);

#ifdef __cplusplus
}
#endif

#endif
