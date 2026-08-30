/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/status_indicator.h
 *
 * PURPOSE:
 *   Represent compact semantic status indicators for dashboards and workbenches.
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
#ifndef UMICOM_UI_ANALYTICS_STATUS_INDICATOR_H
#define UMICOM_UI_ANALYTICS_STATUS_INDICATOR_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsStatusIndicator { char label[UMI_ANALYTICS_TEXT_CAPACITY]; UmiAnalyticsSeverity severity; int active; } UmiAnalyticsStatusIndicator;
UmiStatus umi_analytics_status_indicator_init(UmiAnalyticsStatusIndicator *item);
int umi_analytics_status_indicator_valid(const UmiAnalyticsStatusIndicator *item);

#ifdef __cplusplus
}
#endif

#endif
