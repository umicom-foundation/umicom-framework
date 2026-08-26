/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/series.h
 *
 * PURPOSE:
 *   Own a bounded ordered collection of Cartesian analytics samples.
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
#ifndef UMICOM_UI_ANALYTICS_SERIES_H
#define UMICOM_UI_ANALYTICS_SERIES_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/analytics/data_point.h"
typedef struct UmiAnalyticsSeries { char id[UMI_ANALYTICS_ID_CAPACITY]; char label[UMI_ANALYTICS_TEXT_CAPACITY]; UmiAnalyticsDataPoint points[UMI_ANALYTICS_MAX_POINTS]; size_t count; uint64_t revision; } UmiAnalyticsSeries;
UmiStatus umi_analytics_series_init(UmiAnalyticsSeries *series,const char *id,const char *label);
UmiStatus umi_analytics_series_append(UmiAnalyticsSeries *series,double x,double y);
UmiStatus umi_analytics_series_at(const UmiAnalyticsSeries *series,size_t index,UmiAnalyticsDataPoint *out_point);

#ifdef __cplusplus
}
#endif

#endif
