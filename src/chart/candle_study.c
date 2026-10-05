/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/candle_study.c
 * PURPOSE: Keep rolling indicator mathematics and missing-volume gaps consistent across frontends.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/candle_study.h"
#include <math.h>
#include <stdlib.h>

/* Calculate one complete chronological window. A missing-volume average remains invalid so renderers can leave a gap instead of inventing a price. */
static UmiStatus Window(const UmiChartCandle *bars, size_t period, UmiChartCandleStudyKind kind,
                        UmiChartStudySample *out)
{
    out->time_ms = bars[period - 1U].time_ms;
    if (kind == UMI_CHART_CANDLE_STUDY_DONCHIAN)
    {
        double low = bars[0].low, high = bars[0].high;
        for (size_t i = 1U; i < period; ++i)
        {
            low = fmin(low, bars[i].low);
            high = fmax(high, bars[i].high);
        }
        out->lower = low;
        out->upper = high;
        out->centre = low / 2.0 + high / 2.0;
        out->valid = true;
        return UMI_STATUS_OK;
    }
    /* Normalise both axes before accumulating. Multiplying a large price by
     * a large volume can overflow even when their weighted average is finite. */
    double price_scale = 0.0, volume_scale = 0.0;
    for (size_t i = 0U; i < period; ++i)
    {
        price_scale = fmax(price_scale, fabs(bars[i].close));
        volume_scale = fmax(volume_scale, bars[i].volume);
    }
    if (kind == UMI_CHART_CANDLE_STUDY_VOLUME_WEIGHTED && volume_scale == 0.0)
        return UMI_STATUS_OK;
    /* Centre band calculations on an actual close. Nearby large prices then
     * retain their small differences instead of losing them during scaling.
     * Opposite extreme signs can overflow subtraction; use zero origin there. */
    double origin = 0.0;
    if (kind == UMI_CHART_CANDLE_STUDY_BOLLINGER)
    {
        double spread = 0.0;
        bool differences_finite = true;
        for (size_t i = 0U; i < period; ++i)
        {
            double delta = bars[i].close - bars[0].close;
            if (!isfinite(delta))
            {
                differences_finite = false;
                break;
            }
            spread = fmax(spread, fabs(delta));
        }
        if (differences_finite)
        {
            origin = bars[0].close;
            price_scale = spread;
        }
    }
    if (price_scale == 0.0)
        price_scale = 1.0;
    double mean = 0.0, weights = 0.0;
    for (size_t i = 0U; i < period; ++i)
    {
        double weight = kind == UMI_CHART_CANDLE_STUDY_VOLUME_WEIGHTED ? bars[i].volume / volume_scale : 1.0;
        weights += weight;
        mean += ((bars[i].close - origin) / price_scale) * weight;
    }
    mean /= weights;
    /* Rounding must not put a weighted mean outside its normalised range. */
    mean = fmax(-1.0, fmin(1.0, mean));
    out->centre = origin + mean * price_scale;
    out->lower = out->centre;
    out->upper = out->centre;
    if (kind == UMI_CHART_CANDLE_STUDY_BOLLINGER)
    {
        double variance = 0.0;
        for (size_t i = 0U; i < period; ++i)
        {
            double delta = (bars[i].close - origin) / price_scale - mean;
            variance += delta * delta;
        }
        double deviation = sqrt(variance / (double)period);
        out->lower = origin + (mean - 2.0 * deviation) * price_scale;
        out->upper = origin + (mean + 2.0 * deviation) * price_scale;
    }
    if (!isfinite(out->centre) || !isfinite(out->lower) || !isfinite(out->upper))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    out->valid = true;
    return UMI_STATUS_OK;
}
/* Validate the full candle sequence before calculation, then publish all samples at once. Rendering adapters never need to duplicate the mathematics. */
UmiStatus UmiChartCandleStudyCompute(const UmiChartCandle *candles, size_t count,
                                     UmiChartCandleStudyKind kind, size_t period, UmiChartCandleStudy *out)
{
    if (out == NULL || (count != 0U && candles == NULL) || period < 2U || period > 200U ||
        kind < UMI_CHART_CANDLE_STUDY_VOLUME_WEIGHTED || kind > UMI_CHART_CANDLE_STUDY_DONCHIAN)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_CHART_MAX_POINTS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < count; ++i)
        if (umi_chart_candle_validate(&candles[i]) != UMI_STATUS_OK ||
            (i > 0U && candles[i].time_ms <= candles[i - 1U].time_ms))
            return UMI_STATUS_INVALID_ARGUMENT;
    UmiChartCandleStudy *result = calloc(1U, sizeof(*result));
    if (result == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    result->bands = kind != UMI_CHART_CANDLE_STUDY_VOLUME_WEIGHTED;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t end = period; end <= count; ++end)
    {
        status = Window(candles + end - period, period, kind, &result->samples[result->count]);
        if (status != UMI_STATUS_OK)
            break;
        ++result->count;
    }
    if (status == UMI_STATUS_OK)
        *out = *result;
    free(result);
    return status;
}
