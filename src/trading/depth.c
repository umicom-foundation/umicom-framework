/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/depth.c
 *
 * PURPOSE:
 *   Validate market-depth levels and obtain best bid/ask.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of depth. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/depth.h"
#include "umicom/trading/instrument.h"
#include <math.h>
/* Check that market depth satisfies its contract before another service relies on it. */
/* Count-only validation could admit malformed bounded identity and non-finite levels before downstream lookup. The shared instrument validator and active-level checks replace it; the previous implementation remains for review. The previous implementation is retained for review. */
#if 0
int umi_market_depth_valid(const UmiMarketDepth *d){return d!=NULL&&d->bid_count<=UMI_TRADING_MAX_DEPTH&&d->ask_count<=UMI_TRADING_MAX_DEPTH;}
#endif
int umi_market_depth_valid(const UmiMarketDepth *depth)
{
    /* Validate the shared instrument contract before any caller compares its
     * bounded strings. Only active levels belong to this snapshot's evidence. */
    if (depth == NULL || !umi_instrument_valid(&depth->instrument) ||
        depth->bid_count > UMI_TRADING_MAX_DEPTH ||
        depth->ask_count > UMI_TRADING_MAX_DEPTH)
        return 0;
    for (size_t i = 0U; i < depth->bid_count; ++i)
        if (!isfinite(depth->bids[i].price) || !isfinite(depth->bids[i].size) ||
            depth->bids[i].size < 0.0)
            return 0;
    for (size_t i = 0U; i < depth->ask_count; ++i)
        if (!isfinite(depth->asks[i].price) || !isfinite(depth->asks[i].size) ||
            depth->asks[i].size < 0.0)
            return 0;
    /* Finite negative prices and zero sizes remain data values. Ordering,
     * crossed-book handling and event-time policy belong to the adapter's
     * declared feed contract, not this generic storage validator. */
    return 1;
}
/*
 * Provide the market depth best bid operation used by this module and its client
 * applications.
 */
double umi_market_depth_best_bid(const UmiMarketDepth *d){return umi_market_depth_valid(d)&&d->bid_count>0U?d->bids[0].price:0.0;}
/*
 * Provide the market depth best ask operation used by this module and its client
 * applications.
 */
double umi_market_depth_best_ask(const UmiMarketDepth *d){return umi_market_depth_valid(d)&&d->ask_count>0U?d->asks[0].price:0.0;}
