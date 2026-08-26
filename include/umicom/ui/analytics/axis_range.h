/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/axis_range.h
 *
 * PURPOSE:
 *   Expand finite chart ranges and apply proportional padding.
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
#ifndef UMICOM_UI_ANALYTICS_AXIS_RANGE_H
#define UMICOM_UI_ANALYTICS_AXIS_RANGE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsAxisRange { double minimum; double maximum; int initialized; } UmiAnalyticsAxisRange;
void umi_analytics_axis_range_reset(UmiAnalyticsAxisRange *range);
UmiStatus umi_analytics_axis_range_include(UmiAnalyticsAxisRange *range,double value);
UmiStatus umi_analytics_axis_range_padded(const UmiAnalyticsAxisRange *range,double fraction,UmiAnalyticsAxisRange *out_range);

#ifdef __cplusplus
}
#endif

#endif
