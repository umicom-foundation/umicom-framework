/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/threshold_band.h
 *
 * PURPOSE:
 *   Describe semantic threshold bands for risk, limits and operational analytics.
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
#ifndef UMICOM_UI_ANALYTICS_THRESHOLD_BAND_H
#define UMICOM_UI_ANALYTICS_THRESHOLD_BAND_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsThresholdBand { double lower; double upper; UmiAnalyticsSeverity severity; } UmiAnalyticsThresholdBand;
UmiStatus umi_analytics_threshold_band_init(UmiAnalyticsThresholdBand *item);
int umi_analytics_threshold_band_valid(const UmiAnalyticsThresholdBand *item);

#ifdef __cplusplus
}
#endif

#endif
