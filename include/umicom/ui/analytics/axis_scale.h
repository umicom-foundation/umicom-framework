/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/axis_scale.h
 *
 * PURPOSE:
 *   Map axis-domain values into normalized presentation coordinates.
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
#ifndef UMICOM_UI_ANALYTICS_AXIS_SCALE_H
#define UMICOM_UI_ANALYTICS_AXIS_SCALE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/analytics/axis.h"
UmiStatus umi_analytics_axis_scale_normalize(const UmiAnalyticsAxis *axis,double value,double *out_normalized);

#ifdef __cplusplus
}
#endif

#endif
