/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/candle.c
 *
 * PURPOSE:
 *   Validate OHLCV candles and expose simple candle measurements.
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

#include "umicom/chart/candle.h"

#include <math.h>

/* Check that a candle contains finite, ordered prices and a non-negative volume. */
UmiStatus umi_chart_candle_validate(const UmiChartCandle *candle)
{
    /* A candle cannot be inspected when its caller did not provide storage. */
    if (candle == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Non-finite numbers cannot be scaled safely and could corrupt chart geometry. */
    if (!isfinite(candle->open) || !isfinite(candle->high) ||
        !isfinite(candle->low) || !isfinite(candle->close) ||
        !isfinite(candle->volume)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* High and low must contain both body prices, while volume cannot be negative. */
    if (candle->high < candle->low ||
        candle->high < candle->open ||
        candle->high < candle->close ||
        candle->low > candle->open ||
        candle->low > candle->close ||
        candle->volume < 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}
/*
 * Provide the chart candle range operation used by this module and its client
 * applications.
 */
double umi_chart_candle_range(const UmiChartCandle *candle)
{
    /* Return zero when no candle is available so this measurement stays total. */
    return candle != NULL ? candle->high - candle->low : 0.0;
}
/*
 * Provide the chart candle is up operation used by this module and its client
 * applications.
 */
int umi_chart_candle_is_up(const UmiChartCandle *candle)
{
    /* Equal open and close is treated as non-declining for stable theme selection. */
    return candle != NULL && candle->close >= candle->open;
}
