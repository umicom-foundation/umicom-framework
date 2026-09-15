/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/quote.c
 *
 * PURPOSE:
 *   Calculate spread and midpoint from validated bid/ask quotes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of quote. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/quote.h"
#include <math.h>
/* Check that quote satisfies its contract before another service relies on it. */
int umi_quote_valid(const UmiQuote *q){return q!=NULL&&isfinite(q->bid)&&isfinite(q->ask)&&isfinite(q->bid_size)&&isfinite(q->ask_size)&&q->bid>0.0&&q->ask>=q->bid&&q->bid_size>=0.0&&q->ask_size>=0.0;}
/* Provide the quote mid operation used by this module and its client applications. */
/*
 * Valid endpoints satisfy 0 < bid <= ask and are finite, so ask - bid is
 * bounded by ask. Do not add bid + ask first: two valid large prices could
 * overflow that intermediate sum. Invalid quotes keep the existing 0 sentinel.
 */
double umi_quote_mid(const UmiQuote *q){return umi_quote_valid(q)?q->bid+(q->ask-q->bid)*0.5:0.0;}
/* Provide the quote spread operation used by this module and its client applications. */
double umi_quote_spread(const UmiQuote *q){return umi_quote_valid(q)?q->ask-q->bid:0.0;}
