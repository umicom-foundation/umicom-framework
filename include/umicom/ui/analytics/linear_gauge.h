/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/linear_gauge.h
 *
 * PURPOSE:
 *   Configure horizontal/vertical linear gauge presentation.
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
#ifndef UMICOM_UI_ANALYTICS_LINEAR_GAUGE_H
#define UMICOM_UI_ANALYTICS_LINEAR_GAUGE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics linear gauge data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsLinearGauge { UmiAnalyticsOrientation orientation; int reversed; } UmiAnalyticsLinearGauge;
/**
 * Initialise analytics linear gauge from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_linear_gauge_init(UmiAnalyticsLinearGauge *item);
/**
 * Check that analytics linear gauge satisfies its contract before another service relies
 * on it.
 */
int umi_analytics_linear_gauge_valid(const UmiAnalyticsLinearGauge *item);

#ifdef __cplusplus
}
#endif

#endif
