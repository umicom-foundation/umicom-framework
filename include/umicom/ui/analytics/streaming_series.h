/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/streaming_series.h
 *
 * PURPOSE:
 *   Maintain a bounded chronological ring buffer for live analytical samples.
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
#ifndef UMICOM_UI_ANALYTICS_STREAMING_SERIES_H
#define UMICOM_UI_ANALYTICS_STREAMING_SERIES_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/analytics/time_point.h"
/**
 * Represent the analytics streaming series data shared with callers of this public
 * contract.
 */
typedef struct UmiAnalyticsStreamingSeries { UmiAnalyticsTimePoint points[256]; size_t head; size_t count; uint64_t revision; } UmiAnalyticsStreamingSeries;
/**
 * Initialise analytics streaming series from caller-provided values so later operations
 * receive a known state.
 */
void umi_analytics_streaming_series_init(UmiAnalyticsStreamingSeries *s);
/**
 * Provide the analytics streaming series push operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_streaming_series_push(UmiAnalyticsStreamingSeries *s,int64_t ts,double value);
/**
 * Find analytics streaming series while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_analytics_streaming_series_at(const UmiAnalyticsStreamingSeries *s,size_t logical_index,UmiAnalyticsTimePoint *out_point);

#ifdef __cplusplus
}
#endif

#endif
