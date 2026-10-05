/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/volume_profile.h
 * PURPOSE: Build a bounded price-volume histogram and contiguous value area for chart frontends.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_VOLUME_PROFILE_H
#define UMICOM_CHART_VOLUME_PROFILE_H
#include <stdbool.h>
#include "umicom/chart/candle.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_CHART_VOLUME_PROFILE_MAX_BINS 200U
    typedef struct UmiChartVolumeSample
    {
        double price, volume;
    } UmiChartVolumeSample;
    typedef struct UmiChartVolumeBin
    {
        double lower, upper, volume;
    } UmiChartVolumeBin;
    typedef struct UmiChartVolumeProfile
    {
        size_t bin_count, sample_count, control_bin, area_first, area_last;
        bool has_volume;
        double total_volume, area_volume, control_price, area_low, area_high;
        UmiChartVolumeBin bins[UMI_CHART_VOLUME_PROFILE_MAX_BINS];
    } UmiChartVolumeProfile;
    /* Samples represent observed price and non-negative volume, in one consistent
 * unit. Requested bins must be 2..200; equal prices collapse to one bin. Edges
 * are lower-inclusive and upper-exclusive, except the final inclusive edge.
 * The control bin has the largest volume; ties choose the lower price. Starting
 * there, the value area expands towards the larger adjacent volume until it
 * reaches at least area_fraction of the total. Ties expand downwards. A whole
 * bin is included, so the achieved fraction can exceed the requested one.
 * area_fraction must be finite and in (0,1]. All-zero volume gives has_volume
 * false, without inventing control/value-area levels. Output is unchanged on
 * failure. Inputs/output must not overlap. No memory is retained and no I/O runs. */
    UmiStatus UmiChartVolumeProfileCompute(const UmiChartVolumeSample *samples, size_t count, size_t bins,
                                           double area_fraction, UmiChartVolumeProfile *out);
    /* The candle adapter assigns each bar's entire volume to its CLOSE. This is
 * explicitly an approximation: bars do not identify actual trades at each price.
 * Use Compute with price-volume observations when a tick feed is available.
 * Candles must be chronological, valid OHLCV, and no more than UMI_CHART_MAX_POINTS. */
    UmiStatus UmiChartVolumeProfileFromCandles(const UmiChartCandle *candles, size_t count, size_t bins,
                                               double area_fraction, UmiChartVolumeProfile *out);
#ifdef __cplusplus
}
#endif
#endif
