/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/study_series.c
 *
 * PURPOSE:
 *   Implement deterministic bounded strategy studies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/study_series.h"

#include <math.h>

UmiStatus umi_strategy_study_sma(
    const double *values,
    size_t count,
    size_t period,
    double *outValue)
{
    size_t index;
    double sum = 0.0;

    if (values == NULL || outValue == NULL || period == 0U ||
        period > count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = count - period; index < count; ++index) {
        sum += values[index];
    }
    *outValue = sum / (double)period;
    return UMI_STATUS_OK;
}

UmiStatus umi_strategy_study_ema(
    const double *values,
    size_t count,
    size_t period,
    double *outValue)
{
    size_t index;
    double alpha;
    double ema;

    if (values == NULL || outValue == NULL || period == 0U ||
        count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    alpha = 2.0 / ((double)period + 1.0);
    ema = values[0];
    for (index = 1U; index < count; ++index) {
        ema = alpha * values[index] + (1.0 - alpha) * ema;
    }
    *outValue = ema;
    return UMI_STATUS_OK;
}

UmiStatus umi_strategy_study_rsi(
    const double *values,
    size_t count,
    size_t period,
    double *outValue)
{
    size_t index;
    double gains = 0.0;
    double losses = 0.0;

    if (values == NULL || outValue == NULL || period == 0U ||
        count <= period) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = count - period; index < count; ++index) {
        double change = values[index] - values[index - 1U];
        if (change >= 0.0) gains += change;
        else losses -= change;
    }
    if (losses == 0.0) {
        *outValue = gains > 0.0 ? 100.0 : 50.0;
    } else {
        double rs = gains / losses;
        *outValue = 100.0 - (100.0 / (1.0 + rs));
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_strategy_study_atr(
    const double *high,
    const double *low,
    const double *close,
    size_t count,
    size_t period,
    double *outValue)
{
    size_t index;
    double sum = 0.0;

    if (high == NULL || low == NULL || close == NULL ||
        outValue == NULL || period == 0U || count <= period) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = count - period; index < count; ++index) {
        double rangeOne = high[index] - low[index];
        double rangeTwo = fabs(high[index] - close[index - 1U]);
        double rangeThree = fabs(low[index] - close[index - 1U]);
        double trueRange = rangeOne;
        if (rangeTwo > trueRange) trueRange = rangeTwo;
        if (rangeThree > trueRange) trueRange = rangeThree;
        sum += trueRange;
    }
    *outValue = sum / (double)period;
    return UMI_STATUS_OK;
}

UmiStatus umi_strategy_study_vwap(
    const double *price,
    const double *volume,
    size_t count,
    double *outValue)
{
    size_t index;
    double weighted = 0.0;
    double totalVolume = 0.0;

    if (price == NULL || volume == NULL || outValue == NULL || count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < count; ++index) {
        if (volume[index] < 0.0) return UMI_STATUS_INVALID_ARGUMENT;
        weighted += price[index] * volume[index];
        totalVolume += volume[index];
    }
    if (totalVolume <= 0.0) return UMI_STATUS_INVALID_STATE;
    *outValue = weighted / totalVolume;
    return UMI_STATUS_OK;
}
