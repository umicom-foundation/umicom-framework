/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/kpi_trend.h
 *
 * PURPOSE:
 *   Estimate linear KPI trend slope over ordered observations.
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
#ifndef UMICOM_UI_ANALYTICS_KPI_TREND_H
#define UMICOM_UI_ANALYTICS_KPI_TREND_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_analytics_kpi_trend_slope(const double *values,size_t count,double *out_slope);

#ifdef __cplusplus
}
#endif

#endif
