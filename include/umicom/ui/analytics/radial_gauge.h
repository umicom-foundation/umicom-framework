/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/radial_gauge.h
 *
 * PURPOSE:
 *   Configure radial-gauge angular sweep and needle visibility.
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
#ifndef UMICOM_UI_ANALYTICS_RADIAL_GAUGE_H
#define UMICOM_UI_ANALYTICS_RADIAL_GAUGE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics radial gauge data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsRadialGauge { double start_degrees; double sweep_degrees; int needle; } UmiAnalyticsRadialGauge;
/**
 * Initialise analytics radial gauge from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_radial_gauge_init(UmiAnalyticsRadialGauge *item);
/**
 * Check that analytics radial gauge satisfies its contract before another service relies
 * on it.
 */
int umi_analytics_radial_gauge_valid(const UmiAnalyticsRadialGauge *item);

#ifdef __cplusplus
}
#endif

#endif
