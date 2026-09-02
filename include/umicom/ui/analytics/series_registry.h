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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_SERIES_REGISTRY_H
#define UMICOM_UI_ANALYTICS_SERIES_REGISTRY_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/analytics/series.h"
/**
 * Represent the analytics series registry data shared with callers of this public
 * contract.
 */
typedef struct UmiAnalyticsSeriesRegistry { UmiAnalyticsSeries *items[UMI_ANALYTICS_MAX_SERIES]; size_t count; uint64_t revision; } UmiAnalyticsSeriesRegistry;
/**
 * Initialise analytics series registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_analytics_series_registry_init(UmiAnalyticsSeriesRegistry *registry);
/**
 * Add analytics series registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_analytics_series_registry_add(UmiAnalyticsSeriesRegistry *registry,UmiAnalyticsSeries *series);
/**
 * Find analytics series registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiAnalyticsSeries *umi_analytics_series_registry_find(const UmiAnalyticsSeriesRegistry *registry,const char *id);

#ifdef __cplusplus
}
#endif

#endif
