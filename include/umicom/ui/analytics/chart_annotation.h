/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/chart_annotation.h
 *
 * PURPOSE:
 *   Describe semantic chart annotations independent of renderer markup.
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
#ifndef UMICOM_UI_ANALYTICS_CHART_ANNOTATION_H
#define UMICOM_UI_ANALYTICS_CHART_ANNOTATION_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsChartAnnotation { char id[UMI_ANALYTICS_ID_CAPACITY]; char text[UMI_ANALYTICS_TEXT_CAPACITY]; double x; double y; } UmiAnalyticsChartAnnotation;
UmiStatus umi_analytics_chart_annotation_init(UmiAnalyticsChartAnnotation *item);
int umi_analytics_chart_annotation_valid(const UmiAnalyticsChartAnnotation *item);

#ifdef __cplusplus
}
#endif

#endif
