/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/bar.c
 *
 * PURPOSE:
 *   Validate OHLCV bars and calculate range.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of bar. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/bar.h"

#include <math.h>

/* Check every numeric input before comparisons. A non-finite market value can
 * otherwise pass ordinary range tests and later damage scales or indicators. */
int umi_bar_valid(const UmiBar *bar)
{
    if (bar == NULL || !isfinite(bar->open) || !isfinite(bar->high) ||
        !isfinite(bar->low) || !isfinite(bar->close) ||
        !isfinite(bar->volume)) {
        return 0;
    }
    return bar->high >= bar->low && bar->high >= bar->open &&
           bar->high >= bar->close && bar->low <= bar->open &&
           bar->low <= bar->close && bar->volume >= 0.0 &&
           bar->end_time_ms >= bar->start_time_ms;
}
/* Provide the bar range operation used by this module and its client applications. */
double umi_bar_range(const UmiBar *b){return umi_bar_valid(b)?b->high-b->low:0.0;}
/* Provide the bar typical price operation used by this module and its client applications. */
double umi_bar_typical_price(const UmiBar *b){return umi_bar_valid(b)?(b->high+b->low+b->close)/3.0:0.0;}
