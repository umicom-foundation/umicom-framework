/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/heatmap_scale.h
 *
 * PURPOSE:
 *   Normalize sequential and diverging heatmap values.
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
#ifndef UMICOM_UI_ANALYTICS_HEATMAP_SCALE_H
#define UMICOM_UI_ANALYTICS_HEATMAP_SCALE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsHeatmapScale { double minimum; double center; double maximum; int diverging; } UmiAnalyticsHeatmapScale;
UmiStatus umi_analytics_heatmap_scale_init(UmiAnalyticsHeatmapScale *s,double minimum,double center,double maximum,int diverging);
UmiStatus umi_analytics_heatmap_scale_normalize(const UmiAnalyticsHeatmapScale *s,double value,double *out_normalized);

#ifdef __cplusplus
}
#endif

#endif
