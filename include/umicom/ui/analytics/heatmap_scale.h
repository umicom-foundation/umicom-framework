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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_HEATMAP_SCALE_H
#define UMICOM_UI_ANALYTICS_HEATMAP_SCALE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics heatmap scale data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsHeatmapScale { double minimum; double center; double maximum; int diverging; } UmiAnalyticsHeatmapScale;
/**
 * Initialise analytics heatmap scale from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_heatmap_scale_init(UmiAnalyticsHeatmapScale *s,double minimum,double center,double maximum,int diverging);
/**
 * Provide the analytics heatmap scale normalize operation used by this module and its
 * client applications.
 */
UmiStatus umi_analytics_heatmap_scale_normalize(const UmiAnalyticsHeatmapScale *s,double value,double *out_normalized);

#ifdef __cplusplus
}
#endif

#endif
