/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/market_data_quality.c
 *
 * PURPOSE:
 *   Score basic quote freshness and crossed-market quality.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of market data quality. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/market_data_quality.h"
#include "umicom/trading/quote.h"

/*
 * Provide the market data quality score operation used by this module and its client
 * applications.
 */
double umi_market_data_quality_score(const UmiQuote *quote,
                                     int64_t now_ms,
                                     int64_t max_age_ms)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!umi_quote_valid(quote) || max_age_ms <= 0) {
        return 0.0;
    }

    const int64_t age_ms = now_ms - quote->event_time_ms;
    /* Apply this branch only when its contract condition is satisfied. */
    if (age_ms < 0 || age_ms > max_age_ms) {
        return 0.0;
    }

    return 1.0 - (double)age_ms / (double)max_age_ms;
}
