/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/volume_profile.c
 * PURPOSE: Calculate price buckets without fabricating bid/ask or intrabar trade information.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/volume_profile.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>

/* Keep the histogram independent of chart widgets and feed protocols. Callers choose the sample source; this owner defines bucket and value-area rules consistently. */
UmiStatus UmiChartVolumeProfileCompute(const UmiChartVolumeSample *samples, size_t count, size_t bins,
                                       double area_fraction, UmiChartVolumeProfile *out)
{
    if (out == NULL || (count && samples == NULL) || bins < 2U || bins > UMI_CHART_VOLUME_PROFILE_MAX_BINS ||
        !isfinite(area_fraction) || area_fraction <= 0.0 || area_fraction > 1.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_CHART_MAX_POINTS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    double low = 0.0, high = 0.0, volume_scale = 0.0;
    for (size_t i = 0U; i < count; ++i)
    {
        if (!isfinite(samples[i].price) || !isfinite(samples[i].volume) || samples[i].volume < 0.0)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (i == 0U)
            low = high = samples[i].price;
        low = fmin(low, samples[i].price);
        high = fmax(high, samples[i].price);
        volume_scale = fmax(volume_scale, samples[i].volume);
    }
    double span = high - low;
    if (!isfinite(span))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiChartVolumeProfile *r = calloc(1U, sizeof(*r));
    if (r == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    r->sample_count = count;
    r->bin_count = count ? (span == 0.0 ? 1U : bins) : 0U;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < r->bin_count; ++i)
    {
        r->bins[i].lower = i == 0U ? low : r->bins[i - 1U].upper;
        r->bins[i].upper =
            i + 1U == r->bin_count ? high : low + span * ((double)(i + 1U) / (double)r->bin_count);
        /* Very narrow ranges at large prices may have no representable edge.
         * Refuse collapsed buckets rather than silently assigning them twice. */
        if (r->bin_count > 1U && r->bins[i].upper <= r->bins[i].lower)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            goto done;
        }
    }
    if (volume_scale == 0.0)
    {
        *out = *r;
        goto done;
    }
    /* Accumulate scaled volume, then recover the original unit once. This
     * avoids overflow in intermediate products and keeps every bin comparable. */
    double total = 0.0;
    for (size_t i = 0U; i < count; ++i)
    {
        size_t index = 0U;
        while (index + 1U < r->bin_count && samples[i].price >= r->bins[index].upper)
            ++index;
        double weight = samples[i].volume / volume_scale;
        r->bins[index].volume += weight;
        total += weight;
    }
    if (total > DBL_MAX / volume_scale)
    {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto done;
    }
    r->has_volume = true;
    r->total_volume = total * volume_scale;
    for (size_t i = 1U; i < r->bin_count; ++i)
        if (r->bins[i].volume > r->bins[r->control_bin].volume)
            r->control_bin = i;
    r->area_first = r->area_last = r->control_bin;
    double area = r->bins[r->control_bin].volume;
    while (area < total * area_fraction && (r->area_first > 0U || r->area_last + 1U < r->bin_count))
    {
        bool below = r->area_first > 0U, above = r->area_last + 1U < r->bin_count;
        if (below && (!above || r->bins[r->area_first - 1U].volume >= r->bins[r->area_last + 1U].volume))
            area += r->bins[--r->area_first].volume;
        else
            area += r->bins[++r->area_last].volume;
    }
    r->area_volume = fmin(area, total) * volume_scale;
    r->area_low = r->bins[r->area_first].lower;
    r->area_high = r->bins[r->area_last].upper;
    r->control_price =
        r->bins[r->control_bin].lower + (r->bins[r->control_bin].upper - r->bins[r->control_bin].lower) / 2.0;
    for (size_t i = 0U; i < r->bin_count; ++i)
        r->bins[i].volume = fmin(r->bins[i].volume, total) * volume_scale;
    *out = *r;
done:
    free(r);
    return status;
}
/* Adapt existing candle history without claiming access to individual trades. A later tick-feed adapter can call Compute directly with actual observed prices. */
UmiStatus UmiChartVolumeProfileFromCandles(const UmiChartCandle *candles, size_t count, size_t bins,
                                           double area_fraction, UmiChartVolumeProfile *out)
{
    if (out == NULL || (count && candles == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_CHART_MAX_POINTS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiChartVolumeSample *samples = calloc(count ? count : 1U, sizeof(*samples));
    if (samples == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < count; ++i)
    {
        if (umi_chart_candle_validate(&candles[i]) != UMI_STATUS_OK ||
            (i > 0U && candles[i].time_ms <= candles[i - 1U].time_ms))
        {
            status = UMI_STATUS_INVALID_ARGUMENT;
            break;
        }
        samples[i] = (UmiChartVolumeSample){candles[i].close, candles[i].volume};
    }
    if (status == UMI_STATUS_OK)
        status = UmiChartVolumeProfileCompute(samples, count, bins, area_fraction, out);
    free(samples);
    return status;
}
