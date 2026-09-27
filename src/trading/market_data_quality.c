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
/* The signed subtraction below can overflow for valid int64_t timestamps.
 * The replacement compares signed times first, then subtracts unsigned values.
 * The prior implementation is retained for engineering review; public semantics
 * for ordinary timestamps and the zero-quality sentinel are unchanged. */
#if 0
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
#endif

/* Framework owns freshness arithmetic so every consumer handles the full
 * timestamp range consistently, rather than correcting it in a widget. */
double umi_market_data_quality_score(const UmiQuote *quote,
                                     int64_t now_ms,
                                     int64_t max_age_ms)
{
    if (!umi_quote_valid(quote) || max_age_ms <= 0 ||
        now_ms < quote->event_time_ms) {
        return 0.0;
    }
    /* Once ordered, unsigned subtraction is the exact mathematical distance
     * even when the two signed timestamps straddle zero. */
    const uint64_t age = (uint64_t)now_ms - (uint64_t)quote->event_time_ms;
    if (age >= (uint64_t)max_age_ms) return 0.0;
    return 1.0 - (double)age / (double)max_age_ms;
}
