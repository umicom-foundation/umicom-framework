/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/order_book.c
 *
 * PURPOSE:
 *   Calculate top-of-book liquidity and imbalance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of order book. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/order_book.h"
/*
 * Provide the order book top liquidity operation used by this module and its client
 * applications.
 */
double umi_order_book_top_liquidity(const UmiMarketDepth *d){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||d->bid_count==0U||d->ask_count==0U)return 0.0;return d->bids[0].size+d->asks[0].size;}
/*
 * Provide the order book imbalance operation used by this module and its client
 * applications.
 */
double umi_order_book_imbalance(const UmiMarketDepth *d){double total=umi_order_book_top_liquidity(d);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(total<=0.0)return 0.0;return (d->bids[0].size-d->asks[0].size)/total;}
