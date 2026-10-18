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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_CHART_ANNOTATION_H
#define UMICOM_UI_ANALYTICS_CHART_ANNOTATION_H

#include "umicom/ui/analytics/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics chart annotation data shared with callers of this public
 * contract.
 */
typedef struct UmiAnalyticsChartAnnotation { char id[UMI_ANALYTICS_ID_CAPACITY]; char text[UMI_ANALYTICS_TEXT_CAPACITY]; double x; double y; } UmiAnalyticsChartAnnotation;
/**
 * Initialise analytics chart annotation from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_chart_annotation_init(UmiAnalyticsChartAnnotation *item);
/**
 * Check that analytics chart annotation satisfies its contract before another service
 * relies on it.
 */
int umi_analytics_chart_annotation_valid(const UmiAnalyticsChartAnnotation *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_analytics_chart_annotation_archive_encode(const UmiAnalyticsChartAnnotation *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_analytics_chart_annotation_archive_decode(const void *bytes, size_t byte_count,
    UmiAnalyticsChartAnnotation *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiAnalyticsChartAnnotation object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_analytics_chart_annotation_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiAnalyticsChartAnnotation *workspace, UmiAnalyticsChartAnnotation *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_analytics_chart_annotation_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiAnalyticsChartAnnotation *expected, UmiAnalyticsChartAnnotation *value);


#ifdef __cplusplus
}
#endif

#endif
