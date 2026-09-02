/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/viewport.h
 *
 * PURPOSE:
 *   Represent and zoom two-dimensional analytical viewports.
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
#ifndef UMICOM_UI_ANALYTICS_VIEWPORT_H
#define UMICOM_UI_ANALYTICS_VIEWPORT_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics viewport data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsViewport { double x_min,x_max,y_min,y_max; } UmiAnalyticsViewport;
/**
 * Initialise analytics viewport from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_viewport_init(UmiAnalyticsViewport *v,double x_min,double x_max,double y_min,double y_max);
/**
 * Provide the analytics viewport zoom operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_viewport_zoom(UmiAnalyticsViewport *v,double factor,double x_center,double y_center);

#ifdef __cplusplus
}
#endif

#endif
