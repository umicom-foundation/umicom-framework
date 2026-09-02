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

/**
 * Represent the analytics correlation heatmap data shared with callers of this public
 * contract.
 */
typedef struct UmiAnalyticsCorrelationHeatmap { uint16_t size; double values[UMI_ANALYTICS_MAX_MATRIX * UMI_ANALYTICS_MAX_MATRIX]; } UmiAnalyticsCorrelationHeatmap;
/**
 * Initialise analytics correlation heatmap from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_correlation_heatmap_init(UmiAnalyticsCorrelationHeatmap *m,uint16_t size);
/**
 * Copy analytics correlation heatmap into module-owned storage so callers keep ownership
 * of their input values.
 */
UmiStatus umi_analytics_correlation_heatmap_set(UmiAnalyticsCorrelationHeatmap *m,uint16_t a,uint16_t b,double correlation);
/**
 * Provide the analytics correlation heatmap get operation used by this module and its
 * client applications.
 */
UmiStatus umi_analytics_correlation_heatmap_get(const UmiAnalyticsCorrelationHeatmap *m,uint16_t a,uint16_t b,double *out_value);

#ifdef __cplusplus
}
#endif

#endif
