/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/candle_study.h
 * PURPOSE: Compute rolling volume-weighted averages and price bands independently of chart rendering.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_CANDLE_STUDY_H
#define UMICOM_CHART_CANDLE_STUDY_H
#include <stdbool.h>
#include "umicom/chart/candle.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef enum UmiChartCandleStudyKind
    {
        UMI_CHART_CANDLE_STUDY_VOLUME_WEIGHTED = 1,
        UMI_CHART_CANDLE_STUDY_BOLLINGER,
        UMI_CHART_CANDLE_STUDY_DONCHIAN
    } UmiChartCandleStudyKind;
    typedef struct UmiChartStudySample
    {
        int64_t time_ms;
        double centre, lower, upper;
        bool valid;
    } UmiChartStudySample;
    typedef struct UmiChartCandleStudy
    {
        bool bands;
        size_t count;
        UmiChartStudySample samples[UMI_CHART_MAX_POINTS];
    } UmiChartCandleStudy;
    /* Each complete window includes its final candle. Volume-weighted averages
 * use closing price and reported candle volume; they are rolling VWMA, not
 * session/tick VWAP. All-zero volume produces an invalid sample, which must
 * break a drawn line. Bollinger uses closing-price mean and two population
 * standard deviations. Donchian uses highest high/lowest low and their midpoint.
 *
 * Candles must have strictly increasing times and valid finite OHLCV. Period
 * is 2..200; fewer candles returns an empty result. Allocate this large result
 * on the heap. No input/output overlap is allowed. Errors leave out unchanged.
 * Recompute from canonical candles after corrections or timeframe changes;
 * chart panning should only clip the already computed result. */
    UmiStatus UmiChartCandleStudyCompute(const UmiChartCandle *candles, size_t count,
                                         UmiChartCandleStudyKind kind, size_t period,
                                         UmiChartCandleStudy *out);
#ifdef __cplusplus
}
#endif
#endif
