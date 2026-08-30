/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/data_point.h
 *
 * PURPOSE:
 *   Represent one finite Cartesian chart sample.
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
#ifndef UMICOM_UI_ANALYTICS_DATA_POINT_H
#define UMICOM_UI_ANALYTICS_DATA_POINT_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsDataPoint { double x; double y; int valid; } UmiAnalyticsDataPoint;
UmiStatus umi_analytics_data_point_init(UmiAnalyticsDataPoint *point, double x, double y);
int umi_analytics_data_point_is_valid(const UmiAnalyticsDataPoint *point);

#ifdef __cplusplus
}
#endif

#endif
