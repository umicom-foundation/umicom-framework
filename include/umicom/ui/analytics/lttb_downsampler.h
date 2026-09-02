/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/lttb_downsampler.h
 *
 * PURPOSE:
 *   Apply Largest-Triangle-Three-Buckets downsampling to preserve visual shape.
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
#ifndef UMICOM_UI_ANALYTICS_LTTB_DOWNSAMPLER_H
#define UMICOM_UI_ANALYTICS_LTTB_DOWNSAMPLER_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/analytics/data_point.h"
/**
 * Provide the analytics downsample lttb operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_downsample_lttb(const UmiAnalyticsDataPoint *input,size_t input_count,UmiAnalyticsDataPoint *output,size_t threshold,size_t *out_count);

#ifdef __cplusplus
}
#endif

#endif
