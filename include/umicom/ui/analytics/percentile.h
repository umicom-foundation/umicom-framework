/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/percentile.h
 *
 * PURPOSE:
 *   Calculate bounded interpolated percentiles without mutating caller data.
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
#ifndef UMICOM_UI_ANALYTICS_PERCENTILE_H
#define UMICOM_UI_ANALYTICS_PERCENTILE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the analytics percentile operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_percentile(const double *values,size_t count,double percentile,double *out_value);

#ifdef __cplusplus
}
#endif

#endif
