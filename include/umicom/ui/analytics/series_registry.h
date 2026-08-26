/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/series_registry.h
 *
 * PURPOSE:
 *   Register non-owning analytics series references by stable identity.
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
#ifndef UMICOM_UI_ANALYTICS_SERIES_REGISTRY_H
#define UMICOM_UI_ANALYTICS_SERIES_REGISTRY_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/analytics/series.h"
typedef struct UmiAnalyticsSeriesRegistry { UmiAnalyticsSeries *items[UMI_ANALYTICS_MAX_SERIES]; size_t count; uint64_t revision; } UmiAnalyticsSeriesRegistry;
void umi_analytics_series_registry_init(UmiAnalyticsSeriesRegistry *registry);
UmiStatus umi_analytics_series_registry_add(UmiAnalyticsSeriesRegistry *registry,UmiAnalyticsSeries *series);
UmiAnalyticsSeries *umi_analytics_series_registry_find(const UmiAnalyticsSeriesRegistry *registry,const char *id);

#ifdef __cplusplus
}
#endif

#endif
