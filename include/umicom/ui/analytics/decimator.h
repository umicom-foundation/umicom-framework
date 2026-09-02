/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/decimator.h
 *
 * PURPOSE:
 *   Preserve bucket extrema when reducing dense analytical series.
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
#ifndef UMICOM_UI_ANALYTICS_DECIMATOR_H
#define UMICOM_UI_ANALYTICS_DECIMATOR_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics decimated pair data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsDecimatedPair { double minimum; double maximum; } UmiAnalyticsDecimatedPair;
/**
 * Provide the analytics decimate min max operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_decimate_min_max(const double *values,size_t count,size_t buckets,UmiAnalyticsDecimatedPair *out_pairs,size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
