/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/indicator.c
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
 * The calculation is deliberately straightforward and deterministic. Frontends can optimise rendering without changing these shared data contracts.
 */

#include "umicom/chart/indicator.h"

#include <math.h>

/* Check a complete input series before writing output so a failed calculation
 * cannot leave a caller with a plausible-looking partial study. */
static int indicator_input_valid(
    const UmiChartSeries *input,
    size_t period,
    const UmiChartSeries *output)
{
    size_t index;

    /* Reject an impossible stored count before the validation loop reads the
     * fixed-size point array. This also protects callers that deserialize a
     * damaged or incompatible series record. */
    if (input == NULL || output == NULL || input == output || period == 0U ||
        period > UMI_CHART_MAX_POINTS ||
        input->point_count > UMI_CHART_MAX_POINTS) {
        return 0;
    }
    for (index = 0U; index < input->point_count; ++index) {
        if (!isfinite(input->points[index].value)) return 0;
    }
    return 1;
}

/* Calculate a rolling sum so a long period remains linear in point count. */
UmiStatus umi_chart_indicator_sma(
    const UmiChartSeries *input,
    size_t period,
    UmiChartSeries *output)
{
    size_t index;
    double rolling_sum = 0.0;
    UmiStatus status;

    if (!indicator_input_valid(input, period, output)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_chart_series_init(output, "sma", UMI_CHART_LINE);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < input->point_count; ++index) {
        rolling_sum += input->points[index].value;
        if (index >= period) {
            rolling_sum -= input->points[index - period].value;
        }
        /* A moving average is not emitted until a complete period exists. */
        if (index + 1U >= period) {
            status = umi_chart_series_add(
                output,
                (UmiChartPoint){
                    input->points[index].time_ms,
                    rolling_sum / (double)period});
            if (status != UMI_STATUS_OK) return status;
        }
    }
    return UMI_STATUS_OK;
}

/* Calculate a deterministic exponential moving average using the first point
 * as its seed and a smoothing factor derived from the selected period. */
UmiStatus umi_chart_indicator_ema(
    const UmiChartSeries *input,
    size_t period,
    UmiChartSeries *output)
{
    size_t index;
    double alpha;
    double average;
    UmiStatus status;

    if (!indicator_input_valid(input, period, output) ||
        input->point_count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_chart_series_init(output, "ema", UMI_CHART_LINE);
    if (status != UMI_STATUS_OK) return status;
    alpha = 2.0 / ((double)period + 1.0);
    average = input->points[0].value;
    for (index = 0U; index < input->point_count; ++index) {
        if (index > 0U) {
            average = alpha * input->points[index].value +
                      (1.0 - alpha) * average;
        }
        status = umi_chart_series_add(
            output,
            (UmiChartPoint){input->points[index].time_ms, average});
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
