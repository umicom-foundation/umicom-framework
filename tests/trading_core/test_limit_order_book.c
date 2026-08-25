/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_limit_order_book.c
 *
 * PURPOSE:
 *   Exercise maintain bounded aggregated bid and ask levels with deterministic price ordering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/limit_order_book.h"
int main(void) {

    UmiTradingLimitOrderBook b;
    umi_trading_limit_order_book_init(&b);
    umi_trading_limit_order_book_update(&b,UMI_SIDE_BUY,100,10,1U);
    umi_trading_limit_order_book_update(&b,UMI_SIDE_BUY,101,5,1U);
    umi_trading_limit_order_book_update(&b,UMI_SIDE_SELL,103,5,1U);
    return umi_trading_limit_order_book_best(&b,UMI_SIDE_BUY)->price_ticks==101&&umi_trading_limit_order_book_best(&b,UMI_SIDE_SELL)->price_ticks==103?0:1;
}
