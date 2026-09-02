/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/data_window.h
 *
 * PURPOSE:
 *   Resolve bounded visible index windows for large analytical datasets.
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
#ifndef UMICOM_UI_ANALYTICS_DATA_WINDOW_H
#define UMICOM_UI_ANALYTICS_DATA_WINDOW_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics data window data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsDataWindow { size_t first; size_t count; } UmiAnalyticsDataWindow;
/**
 * Provide the analytics data window resolve operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_data_window_resolve(size_t total,size_t first,size_t requested,UmiAnalyticsDataWindow *out_window);

#ifdef __cplusplus
}
#endif

#endif
