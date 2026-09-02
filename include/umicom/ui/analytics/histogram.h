/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/histogram.h
 *
 * PURPOSE:
 *   Build fixed-width histograms over finite numeric samples.
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
#ifndef UMICOM_UI_ANALYTICS_HISTOGRAM_H
#define UMICOM_UI_ANALYTICS_HISTOGRAM_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics histogram data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsHistogram { double minimum; double maximum; size_t bin_count; size_t counts[UMI_ANALYTICS_MAX_BINS]; } UmiAnalyticsHistogram;
/**
 * Provide the analytics histogram build operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_histogram_build(const double *values,size_t count,size_t bins,UmiAnalyticsHistogram *out_histogram);

#ifdef __cplusplus
}
#endif

#endif
