/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/bar_chart.h
 *
 * PURPOSE:
 *   Configure grouped bar-chart orientation and relative gap.
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
#ifndef UMICOM_UI_ANALYTICS_BAR_CHART_H
#define UMICOM_UI_ANALYTICS_BAR_CHART_H

#include "umicom/ui/analytics/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics bar chart data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsBarChart { UmiAnalyticsOrientation orientation; double gap_ratio; } UmiAnalyticsBarChart;
/**
 * Initialise analytics bar chart from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_bar_chart_init(UmiAnalyticsBarChart *item);
/**
 * Check that analytics bar chart satisfies its contract before another service relies on
 * it.
 */
int umi_analytics_bar_chart_valid(const UmiAnalyticsBarChart *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_analytics_bar_chart_archive_encode(const UmiAnalyticsBarChart *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_analytics_bar_chart_archive_decode(const void *bytes, size_t byte_count,
    UmiAnalyticsBarChart *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_analytics_bar_chart_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiAnalyticsBarChart *expected, UmiAnalyticsBarChart *value);


#ifdef __cplusplus
}
#endif

#endif
