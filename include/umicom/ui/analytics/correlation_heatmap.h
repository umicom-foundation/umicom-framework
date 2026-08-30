/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/correlation_heatmap.h
 *
 * PURPOSE:
 *   Maintain symmetric bounded correlation matrices with unit diagonals.
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
#ifndef UMICOM_UI_ANALYTICS_CORRELATION_HEATMAP_H
#define UMICOM_UI_ANALYTICS_CORRELATION_HEATMAP_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsCorrelationHeatmap { uint16_t size; double values[UMI_ANALYTICS_MAX_MATRIX * UMI_ANALYTICS_MAX_MATRIX]; } UmiAnalyticsCorrelationHeatmap;
UmiStatus umi_analytics_correlation_heatmap_init(UmiAnalyticsCorrelationHeatmap *m,uint16_t size);
UmiStatus umi_analytics_correlation_heatmap_set(UmiAnalyticsCorrelationHeatmap *m,uint16_t a,uint16_t b,double correlation);
UmiStatus umi_analytics_correlation_heatmap_get(const UmiAnalyticsCorrelationHeatmap *m,uint16_t a,uint16_t b,double *out_value);

#ifdef __cplusplus
}
#endif

#endif
