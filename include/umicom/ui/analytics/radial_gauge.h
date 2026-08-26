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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_RADIAL_GAUGE_H
#define UMICOM_UI_ANALYTICS_RADIAL_GAUGE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsRadialGauge { double start_degrees; double sweep_degrees; int needle; } UmiAnalyticsRadialGauge;
UmiStatus umi_analytics_radial_gauge_init(UmiAnalyticsRadialGauge *item);
int umi_analytics_radial_gauge_valid(const UmiAnalyticsRadialGauge *item);

#ifdef __cplusplus
}
#endif

#endif
