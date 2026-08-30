/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/gauge_zone.h
 *
 * PURPOSE:
 *   Define ordered gauge threshold zones with semantic severity.
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
#ifndef UMICOM_UI_ANALYTICS_GAUGE_ZONE_H
#define UMICOM_UI_ANALYTICS_GAUGE_ZONE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsGaugeZone { double minimum; double maximum; UmiAnalyticsSeverity severity; } UmiAnalyticsGaugeZone;
UmiStatus umi_analytics_gauge_zone_init(UmiAnalyticsGaugeZone *item);
int umi_analytics_gauge_zone_valid(const UmiAnalyticsGaugeZone *item);

#ifdef __cplusplus
}
#endif

#endif
