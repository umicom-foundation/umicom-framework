/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/order_book.c
 *
 * PURPOSE:
 *   Calculate top-of-book liquidity and imbalance.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of order book. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/order_book.h"
double umi_order_book_top_liquidity(const UmiMarketDepth *d){if(d==NULL||d->bid_count==0U||d->ask_count==0U)return 0.0;return d->bids[0].size+d->asks[0].size;}
double umi_order_book_imbalance(const UmiMarketDepth *d){double total=umi_order_book_top_liquidity(d);if(total<=0.0)return 0.0;return (d->bids[0].size-d->asks[0].size)/total;}
