/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/analytics_query.h
 *
 * PURPOSE:
 *   Describe provider-neutral analytical queries for dashboard datasets.
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
#ifndef UMICOM_UI_ANALYTICS_ANALYTICS_QUERY_H
#define UMICOM_UI_ANALYTICS_ANALYTICS_QUERY_H

#include "umicom/ui/analytics/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics query data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsQuery { char dataset_id[UMI_ANALYTICS_ID_CAPACITY]; char metric_id[UMI_ANALYTICS_ID_CAPACITY]; char group_by[UMI_ANALYTICS_ID_CAPACITY]; int64_t start_ns; int64_t end_ns; size_t limit; } UmiAnalyticsQuery;
/**
 * Initialise analytics query from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_query_init(UmiAnalyticsQuery *q,const char *dataset,const char *metric);
/**
 * Check that analytics query satisfies its contract before another service relies on it.
 */
int umi_analytics_query_valid(const UmiAnalyticsQuery *q);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_analytics_query_archive_encode(const UmiAnalyticsQuery *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_analytics_query_archive_decode(const void *bytes, size_t byte_count,
    UmiAnalyticsQuery *value);

#ifdef __cplusplus
}
#endif

#endif
