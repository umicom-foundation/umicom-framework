/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/radial_gauge.h
 *
 * PURPOSE:
 *   Configure radial-gauge angular sweep and needle visibility.
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
#ifndef UMICOM_UI_ANALYTICS_RADIAL_GAUGE_H
#define UMICOM_UI_ANALYTICS_RADIAL_GAUGE_H

#include "umicom/ui/analytics/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics radial gauge data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsRadialGauge { double start_degrees; double sweep_degrees; int needle; } UmiAnalyticsRadialGauge;
/**
 * Initialise analytics radial gauge from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_radial_gauge_init(UmiAnalyticsRadialGauge *item);
/**
 * Check that analytics radial gauge satisfies its contract before another service relies
 * on it.
 */
int umi_analytics_radial_gauge_valid(const UmiAnalyticsRadialGauge *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_analytics_radial_gauge_archive_encode(const UmiAnalyticsRadialGauge *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_analytics_radial_gauge_archive_decode(const void *bytes, size_t byte_count,
    UmiAnalyticsRadialGauge *value);

#ifdef __cplusplus
}
#endif

#endif
