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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_VIEWPORT_H
#define UMICOM_UI_ANALYTICS_VIEWPORT_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsViewport { double x_min,x_max,y_min,y_max; } UmiAnalyticsViewport;
UmiStatus umi_analytics_viewport_init(UmiAnalyticsViewport *v,double x_min,double x_max,double y_min,double y_max);
UmiStatus umi_analytics_viewport_zoom(UmiAnalyticsViewport *v,double factor,double x_center,double y_center);

#ifdef __cplusplus
}
#endif

#endif
