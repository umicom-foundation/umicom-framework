/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/indicator.h
 *
 * PURPOSE:
 *   Compute reusable moving-average indicators over chart series.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Chart data is independent of any frontend toolkit or renderer. Frontends render this shared model in their own way.
 */

#ifndef UMICOM_CHART_INDICATOR_H
#define UMICOM_CHART_INDICATOR_H
#include "umicom/chart/series.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Calculate a simple moving average after each complete period. Input and
 * output must be separate records because initialization clears the output.
 */
UmiStatus umi_chart_indicator_sma(
    const UmiChartSeries *input,
    size_t period,
    UmiChartSeries *output);
/**
 * Calculate an exponential moving average beginning with the first value.
 * Input and output must be separate records.
 */
UmiStatus umi_chart_indicator_ema(
    const UmiChartSeries *input,
    size_t period,
    UmiChartSeries *output);
#ifdef __cplusplus
}
#endif
#endif
