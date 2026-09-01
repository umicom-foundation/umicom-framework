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
int umi_market_depth_valid(const UmiMarketDepth *d){return d!=NULL&&d->bid_count<=UMI_TRADING_MAX_DEPTH&&d->ask_count<=UMI_TRADING_MAX_DEPTH;}
double umi_market_depth_best_bid(const UmiMarketDepth *d){return umi_market_depth_valid(d)&&d->bid_count>0U?d->bids[0].price:0.0;}
double umi_market_depth_best_ask(const UmiMarketDepth *d){return umi_market_depth_valid(d)&&d->ask_count>0U?d->asks[0].price:0.0;}
