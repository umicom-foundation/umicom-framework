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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_THRESHOLD_BAND_H
#define UMICOM_UI_ANALYTICS_THRESHOLD_BAND_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics threshold band data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsThresholdBand { double lower; double upper; UmiAnalyticsSeverity severity; } UmiAnalyticsThresholdBand;
/**
 * Initialise analytics threshold band from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_threshold_band_init(UmiAnalyticsThresholdBand *item);
/**
 * Check that analytics threshold band satisfies its contract before another service relies
 * on it.
 */
int umi_analytics_threshold_band_valid(const UmiAnalyticsThresholdBand *item);

#ifdef __cplusplus
}
#endif

#endif
