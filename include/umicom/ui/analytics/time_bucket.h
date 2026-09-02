/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/time_bucket.h
 *
 * PURPOSE:
 *   Bucket nanosecond timestamps into deterministic fixed intervals.
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
#ifndef UMICOM_UI_ANALYTICS_TIME_BUCKET_H
#define UMICOM_UI_ANALYTICS_TIME_BUCKET_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the analytics time bucket floor operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_time_bucket_floor(int64_t timestamp_ns,int64_t interval_ns,int64_t *out_bucket_start);

#ifdef __cplusplus
}
#endif

#endif
