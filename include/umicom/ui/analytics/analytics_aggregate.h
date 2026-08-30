/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/analytics_aggregate.h
 *
 * PURPOSE:
 *   Compute standard count/sum/min/max/average aggregations over finite samples.
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
#ifndef UMICOM_UI_ANALYTICS_ANALYTICS_AGGREGATE_H
#define UMICOM_UI_ANALYTICS_ANALYTICS_AGGREGATE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_analytics_aggregate_compute(UmiAnalyticsAggregateKind kind,const double *values,size_t count,double *out_value);

#ifdef __cplusplus
}
#endif

#endif
