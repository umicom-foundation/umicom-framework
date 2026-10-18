/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/data_point.h
 *
 * PURPOSE:
 *   Represent one finite Cartesian chart sample.
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
#ifndef UMICOM_UI_ANALYTICS_DATA_POINT_H
#define UMICOM_UI_ANALYTICS_DATA_POINT_H

#include "umicom/ui/analytics/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics data point data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsDataPoint { double x; double y; int valid; } UmiAnalyticsDataPoint;
/**
 * Initialise analytics data point from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_analytics_data_point_init(UmiAnalyticsDataPoint *point, double x, double y);
/**
 * Check that analytics data point satisfies its contract before another service relies on
 * it.
 */
int umi_analytics_data_point_is_valid(const UmiAnalyticsDataPoint *point);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_analytics_data_point_archive_encode(const UmiAnalyticsDataPoint *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_analytics_data_point_archive_decode(const void *bytes, size_t byte_count,
    UmiAnalyticsDataPoint *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiAnalyticsDataPoint object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_analytics_data_point_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiAnalyticsDataPoint *workspace, UmiAnalyticsDataPoint *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_analytics_data_point_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiAnalyticsDataPoint *expected, UmiAnalyticsDataPoint *value);


#ifdef __cplusplus
}
#endif

#endif
