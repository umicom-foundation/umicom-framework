/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/analytics_filter.h
 *
 * PURPOSE:
 *   Evaluate reusable numeric analytics filter predicates.
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
#ifndef UMICOM_UI_ANALYTICS_ANALYTICS_FILTER_H
#define UMICOM_UI_ANALYTICS_ANALYTICS_FILTER_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsFilter { UmiAnalyticsCompare comparison; double threshold; } UmiAnalyticsFilter;
UmiStatus umi_analytics_filter_init(UmiAnalyticsFilter *f,UmiAnalyticsCompare comparison,double threshold);
int umi_analytics_filter_matches(const UmiAnalyticsFilter *f,double value);

#ifdef __cplusplus
}
#endif

#endif
