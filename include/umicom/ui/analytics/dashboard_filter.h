/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/dashboard_filter.h
 *
 * PURPOSE:
 *   Describe one dashboard-level textual filter propagated to compatible tiles.
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
#ifndef UMICOM_UI_ANALYTICS_DASHBOARD_FILTER_H
#define UMICOM_UI_ANALYTICS_DASHBOARD_FILTER_H

#include "umicom/ui/analytics/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics dashboard filter data shared with callers of this public
 * contract.
 */
typedef struct UmiAnalyticsDashboardFilter { char key[UMI_ANALYTICS_ID_CAPACITY]; char value[UMI_ANALYTICS_VALUE_CAPACITY]; int case_sensitive; } UmiAnalyticsDashboardFilter;
/**
 * Initialise analytics dashboard filter from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_dashboard_filter_init(UmiAnalyticsDashboardFilter *item);
/**
 * Check that analytics dashboard filter satisfies its contract before another service
 * relies on it.
 */
int umi_analytics_dashboard_filter_valid(const UmiAnalyticsDashboardFilter *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_analytics_dashboard_filter_archive_encode(const UmiAnalyticsDashboardFilter *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_analytics_dashboard_filter_archive_decode(const void *bytes, size_t byte_count,
    UmiAnalyticsDashboardFilter *value);

#ifdef __cplusplus
}
#endif

#endif
