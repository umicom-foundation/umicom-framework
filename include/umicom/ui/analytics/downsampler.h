/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/downsampler.h
 *
 * PURPOSE:
 *   Uniformly downsample ordered data while preserving endpoints.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral analytics capability extends canonical Umicom::ui.
 *   Existing Design System chart/gauge/heatmap specs and workstation surfaces
 *   remain authoritative; GTK4, Qt6, Native Web and thin applications render
 *   the same Framework-owned analytics semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_DOWNSAMPLER_H
#define UMICOM_UI_ANALYTICS_DOWNSAMPLER_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/analytics/data_point.h"
UmiStatus umi_analytics_downsample_uniform(const UmiAnalyticsDataPoint *input,size_t input_count,UmiAnalyticsDataPoint *output,size_t target_count,size_t *out_count);

#ifdef __cplusplus
}
#endif

#endif
