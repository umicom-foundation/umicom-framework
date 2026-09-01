/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_market_depth_book.c
 *
 * PURPOSE:
 *   Validate market depth book behaviour in the trading foundation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

#include <assert.h>
#include "test_trading_common.h"
int main(void){
    UmiMarketDepth d={0};d.instrument=test_instrument();d.bid_count=1;d.ask_count=1;d.bids[0]=(UmiDepthLevel){25000,6};d.asks[0]=(UmiDepthLevel){25001,2};
    assert(umi_market_depth_valid(&d));assert(umi_market_depth_best_bid(&d)==25000);assert(umi_market_depth_best_ask(&d)==25001);
    assert(umi_order_book_top_liquidity(&d)==8);assert(umi_order_book_imbalance(&d)==0.5);
    return 0;
}
